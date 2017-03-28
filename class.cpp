#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include "class.h"

using namespace cv;
using namespace std;

class detectLane {
public:
	Mat src;
	vector<double> SM(src.rows);
	vector<double> Sm(src.rows);


}
void detectLane::run_filters(Mat& TopHat_Image = NULL, Mat& Loc_Grad_Image = NULL, Mat& Sym_Loc_Grad_Image = NULL){

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

		if(TopHat_Image != NULL){
			tophat_filter(Row_Int_Image, i, TopHat_Image, s);
		}	
		if(Loc_Grad_Image != NULL){
			local_gradient(Row_Int_Image, i, Loc_Grad_Image, s, thresh_grad);
		}
		if(Sym_Loc_Grad_Image != NULL){
			sym_local_gradient(Row_Int_Image, i, Sym_Loc_Grad_Image, s, thresh_sym_grad);
		}
		
	}

}

void detectLane::tophat_filter(vector<double>& Row_Int_Image, int row_idx, Mat& TopHat_Image, double s) {

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

void detectLane::local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Loc_Grad_Image, double s, int thresh_grad) {


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

void detectLane::sym_local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Sym_Loc_Grad_Image, double s, int thresh_sym_grad) {


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

void detectLane::lane_width_filter(Mat& Image, vector<double>& Sm, vector<double>& SM, int horizon) {

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