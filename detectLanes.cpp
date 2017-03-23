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
1) add local grad
2) add sym local grad
4) add gui
5) file explore via boost filesystem
6) eigen speed up
7) otsu method for binarization
8) connected region threshold filter
9) put functions into class
*/

Mat src;
string tophat_window = "Top Hat Filter";
string local_grad_window = "Local Gradient";
string sym_local_grad_window = "Symmetric Local Gradient";
void tophat_filter(vector<double>& Row_Int_Image, int row_idx, Mat& TopHat_Image, double s);
void local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Loc_Grad_Image, double s, int thresh_grad);
void sym_local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Sym_Loc_Grad_Image, double s, int thresh_sym_grad);

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

	//Display TopHat Results
	Mat tophat_binarized;
	TopHat_Image.converTo(tophat_binarized,CV_8UC1);
	threshold(TopHat_Image, tophat_binarized, 12, 255, THRESH_BINARY | THRESH_OTSU );
	namedWindow( tophat_window, WINDOW_AUTOSIZE ); // Create a window to display results
	imshow(tophat_window, tophat_binarized);
	moveWindow(tophat_window, 10, 10);
	imwrite("tophat.jpg", tophat_binarized);

	//Display Loc Gradient Results 
	namedWindow( local_grad_window, WINDOW_AUTOSIZE ); // Create a window to display results
	imshow(local_grad_window, Loc_Grad_Image);
	moveWindow(local_grad_window, 10, 10);
	imwrite("loc_grad.jpg", Loc_Grad_Image);

	//Display Loc Gradient Results 
	namedWindow( sym_local_grad_window, WINDOW_AUTOSIZE ); // Create a window to display results
	imshow(sym_local_grad_window, Sym_Loc_Grad_Image);
	moveWindow(sym_local_grad_window, 10, 10);
	imwrite("loc_grad.jpg", Sym_Loc_Grad_Image);

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

 // % now compute Sym Local Gradient Image (on 12.SM avg size)
 //        for u = 1 : col
 //            u_right = min(u+6*s, col);
 //            du_right = u_right - u + 1;
 //            u_left = max(u-6*s, 1);
 //            du_left = u - u_left + 1;
 //            if(gray(v,u) > thresh_sym_grad + (integral_gray(v, u_right)- integral_gray(v, u))/du_right...
 //                    && gray(v,u) > thresh_grad + (integral_gray(v, u)- integral_gray(v, u_left))/du_left)
 //                sym_loc_grad(v,u) = 255;
 //            end
 //        end