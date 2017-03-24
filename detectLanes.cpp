#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace cv;
using namespace std;

/* RUN COMMAND:
g++ `pkg-config --cflags opencv` detectLanes.cpp `pkg-config --libs opencv` -o test; ./test 

TODO:
- put functions into class
- add gui
- file explore via boost filesystem
- eigen speed up
------------------------------------------
- filter by lane width
- pre-detection gaussian/mean box filter
- optimize otsu thresholding
*/

Mat src;
string tophat_window = "Top Hat Filter";
string local_grad_window = "Local Gradient";
string sym_local_grad_window = "Symmetric Local Gradient";
void tophat_filter(vector<double>& Row_Int_Image, int row_idx, Mat& TopHat_Image, double s);
void local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Loc_Grad_Image, double s, int thresh_grad);
void sym_local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Sym_Loc_Grad_Image, double s, int thresh_sym_grad);
void lane_width_filter(Mat& Image, vector<double>& Sm, vector<double>& SM, int horizon);

int main( int, char** argv )
{

	// Collect image and param paths
	string image_path = "roma/BDXD54/";
	string param_path = "roma/BDXD54/";
	image_path += "IMG00006.jpg";
	param_path += "param.cal"; 

	// Read in image and horizon 
	src = imread(image_path, IMREAD_GRAYSCALE); // Load image as grayscale
	if(src.empty()) {
		cout << "Unable to read image" << endl;
		return 0;
	}

	int horizon, lane_width;
	double ignore;
	ifstream param(param_path.c_str());
	param >> horizon >> ignore >> lane_width;

	// Define Acceptability Range
	double SM_max = lane_width*6; // pixels in 30 cm
	double Sm_max = lane_width*2; // pixels in 10 cm
	vector<double> SM(src.rows);
	vector<double> Sm(src.rows);
	for (int i = src.rows; i > horizon; --i)
	{
		SM[i] = SM_max / (src.rows - horizon) * (i - horizon); 
		Sm[i] = Sm_max / (src.rows - horizon) * (i - horizon);
	}

	//Define thresholds
	int thresh_grad = 30;
	int thresh_sym_grad = 20;

	vector<double> Row_Int_Image(src.cols); // Init vector length
	Mat TopHat_Image = Mat::zeros(src.rows, src.cols, CV_64FC1); // Init Tophat image matrix
	Mat Loc_Grad_Image = Mat::zeros(src.rows, src.cols, CV_8UC1); // Init Loc_Gradient image matrix
	Mat Sym_Loc_Grad_Image = Mat::zeros(src.rows, src.cols, CV_8UC1); // Init Loc_Gradient image matrix

	for (int i = src.rows - 1; i > horizon; --i) {
		// Compute Individual Row by Row Integral Image
		int sum = (int)src.at<uchar>(i, 0);
		for (int j = 0; j < src.cols; ++j) {

			Row_Int_Image[j] = sum;
			sum += (int)src.at<uchar>(i,j);
		}

		// Apply TopHat Filter
		double s = round((SM[i] + Sm[i])*(0.25));
		if ((int)s % 2 == 0) { s += 1;} // s is always odd

		tophat_filter(Row_Int_Image, i, TopHat_Image, s);

		local_gradient(Row_Int_Image, i, Loc_Grad_Image, s, thresh_grad);
		
		sym_local_gradient(Row_Int_Image, i, Sym_Loc_Grad_Image, s, thresh_sym_grad);
		
		
	}

	lane_width_filter(Loc_Grad_Image, Sm, SM, horizon);
	lane_width_filter(Sym_Loc_Grad_Image, Sm, SM, horizon);


	//Display TopHat Results
	Mat tophat_binarized;
	TopHat_Image.convertTo(TopHat_Image,CV_8UC1, 10);
	threshold(TopHat_Image, tophat_binarized, 12, 255, THRESH_BINARY + THRESH_OTSU );
	namedWindow(tophat_window, WINDOW_NORMAL); // Create a window to display results
	resizeWindow(tophat_window, 512,  432);
	imshow(tophat_window, tophat_binarized);
	moveWindow(tophat_window, 20, 100);
	imwrite("tophat.jpg", tophat_binarized);

	//Display Loc Gradient Results 
	namedWindow(local_grad_window, WINDOW_NORMAL); // Create a window to display results
	resizeWindow(local_grad_window, 512,  432);
	imshow(local_grad_window, Loc_Grad_Image);
	moveWindow(local_grad_window, 680, 100);
	imwrite("loc_grad.jpg", Loc_Grad_Image);

	//Display Loc Gradient Results 
	namedWindow(sym_local_grad_window, WINDOW_NORMAL); // Create a window to display results
	resizeWindow(sym_local_grad_window, 512,  432);
	imshow(sym_local_grad_window, Sym_Loc_Grad_Image);
	moveWindow(sym_local_grad_window, 1340, 100);
	imwrite("sym_loc_grad.jpg", Sym_Loc_Grad_Image);

	waitKey(0);

	return 0;
}

void tophat_filter(vector<double>& Row_Int_Image, int row_idx, Mat& TopHat_Image, double s) {

	double* t;
	for (int u = 2*s; u < src.cols - (2*s); ++u) {

		int u_plus_s = u + s;
		int u_minus_s = u - s;
		int u_plus_2s = u + 2*s;
		int u_minus_2s = u - 2*s;

		//Use ptr access to row
		t = TopHat_Image.ptr<double>(row_idx);
		t[u] = ((1/(4*s)) * (2*(Row_Int_Image[u_plus_s] - Row_Int_Image[u_minus_s]) - (Row_Int_Image[u_plus_2s] - Row_Int_Image[u_minus_2s])));
	}
}

void local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Loc_Grad_Image, double s, int thresh_grad) {


	for (int u = 0; u < src.cols; ++u) {
		int u_max = min(int(u + 6*s), src.cols - 1);
		int u_min = max(int(u - 6*s), 1);
		int du = u_max - u_min + 1;

		uchar* src_ptr = src.ptr<uchar>(row_idx);
		uchar* loc_grad_ptr = Loc_Grad_Image.ptr<uchar>(row_idx);

		if (src_ptr[u] > thresh_grad + (Row_Int_Image[u_max] - Row_Int_Image[u_min]) / du) {
			loc_grad_ptr[u] = 255;
		}
	}
}

void sym_local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Sym_Loc_Grad_Image, double s, int thresh_sym_grad) {


	for (int u = 0; u < src.cols; ++u) {
		int u_right = min(int(u + 6*s), src.cols - 1);
		int du_right = u_right - u + 1;
		int u_left = max(int(u - 6*s), 1);
		int du_left = u - u_left + 1;

		uchar* src_ptr = src.ptr<uchar>(row_idx);
		uchar* sym_loc_grad_ptr = Sym_Loc_Grad_Image.ptr<uchar>(row_idx);

		if (src_ptr[u] > thresh_sym_grad + (Row_Int_Image[u_right] - Row_Int_Image[u]) / du_right && 
				src_ptr[u] > thresh_sym_grad + (Row_Int_Image[u] - Row_Int_Image[u_left])/ du_left) {
			sym_loc_grad_ptr[u] = 255;
		}
	}
}

void lane_width_filter(Mat& Image, vector<double>& Sm, vector<double>& SM, int horizon) {

	// Init variables
	Mat filtered_Image = Mat::zeros(Image.rows, Image.cols, CV_8UC1);

	int in_region = 0; 
	int pix_in_region = 0;	// # of pixels in connected region
	int region_start = 0; // start idx connected region
	int region_end = 0; // end idx connected region
	uchar* img_ptr;
	uchar* out_ptr;

	for (int i = Image.rows - 1; i > horizon; --i) {
		if (Sm[i] < 0) {
			break;
		}
		img_ptr = Image.ptr<uchar>(i);
		out_ptr = filtered_Image.ptr<uchar>(i);
		in_region = 0;
		pix_in_region = 0;

		for (int j = 0; j < Image.cols; ++j) {
			if (in_region) {
				if (img_ptr[j] == 0) { // no longer in region
					in_region = 0;
					if (pix_in_region >= Sm[i] && pix_in_region <= SM[i]) { //paint region if fit
						region_end = j;
						for (int k = region_start; k <= region_end; ++k) {
							out_ptr[k] = 255;
						}
						//cout << "painted a " << (int)out_ptr[region_end] << "\n"; 
					}
					pix_in_region = 0; // reset pix counter
				}
				else { //still in region
					pix_in_region += 1;
				}
			}
			else { //not in region
				if (img_ptr[j] > 0) {
					pix_in_region += 1;
					region_start = j;
					in_region = 1;
				}
			}
		}		
	}

	// namedWindow(local_grad_window, WINDOW_NORMAL); // Create a window to display results
	// resizeWindow(local_grad_window, 512,  432);
	// imshow(local_grad_window, filtered_Image);
	// moveWindow(local_grad_window, 680, 100);
	// waitKey(0);
	filtered_Image.copyTo(Image);
}

// function out = filter_by_RM_width(in, Sm, SM)

// [lig, col, chn] = size(in);
// out = zeros(lig, col);

// for v = lig : -1 : 1
//     if(Sm(v) < 0)
//         break;
//     end
    
//     allume = 0;
//     count = 0;
//     for u = 1 : col
//         if(allume == 0)
//             if(in(v,u) > 0)
//                 count = count + 1;
//                 allume = 1;
//                 u_start = u;
//             end
//         else
//             if(in(v,u) == 0)
//                 allume = 0;
//                 if(count >= Sm(v) && count <= SM(v));
//                     u_end = u;
//                     out(v,u_start:u_end) = 255;
//                 end
//                 count = 0;
//             else
//                 count = count + 1;
//             end
//         end
//     end
    
// end