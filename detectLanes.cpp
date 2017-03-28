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

/* RUN COMMAND:
g++ `pkg-config --cflags opencv` detectLanes.cpp `pkg-config --libs opencv` -o test; ./test 

TODO:
- put functions into class
- add gui
- file explore via boost filesystem
- eigen speed up
------------------------------------------
- test pre-processing gaussian & mean box filter
- optimize otsu thresholding

DONE:
- tophat
- loc_grad
- sym_loc_grad
- otsu?
*/


void tophat_filter(vector<double>& Row_Int_Image, int row_idx, Mat& TopHat_Image, double s);
void local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Loc_Grad_Image, double s, int thresh_grad);
void sym_local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Sym_Loc_Grad_Image, double s, int thresh_sym_grad);
void lane_width_filter(Mat& Image, vector<double>& Sm, vector<double>& SM, int horizon);


string tophat_window = "Top Hat Filter";
string local_grad_window = "Local Gradient";
string sym_local_grad_window = "Symmetric Local Gradient";

int main( int, char** argv )
{

	// Collect image and param paths
	string image_path = "roma/BDXD54/";
	string param_path = "roma/BDXD54/";
	image_path += "IMG00006.jpg";
	param_path += "param.cal"; 

	// Initialize instance of object
	detectLanes dL_object;

	// Read in image and horizon 
	dL_object.src = imread(image_path, IMREAD_GRAYSCALE); // Load image as grayscale
	if(src.empty()) {
		cout << "Unable to read image" << endl;
		return 0;
	}

	// Pre processing filtering
	blur(dL_object.src, dL_object.src, Size(9, 9));
	//GaussianBlur(src, src, Size(9, 9), 0);

	int horizon, lane_width;
	double ignore;
	ifstream param(param_path.c_str());
	param >> horizon >> ignore >> lane_width;

	// Define Acceptability Range
	double SM_max = lane_width*6; // pixels in 30 cm
	double Sm_max = lane_width*2; // pixels in 10 cm
	for (int i = src.rows; i > horizon; --i)
	{
		dL_object.SM[i] = SM_max / (dL_object.src.rows - horizon) * (i - horizon); 
		dL_object.Sm[i] = Sm_max / (dL_object.src.rows - horizon) * (i - horizon);
	}

	//Define thresholds
	int thresh_grad = 30;
	int thresh_sym_grad = 20;

	vector<double> Row_Int_Image(src.cols); // Init vector length
	Mat TopHat_Image = Mat::zeros(src.rows, src.cols, CV_64FC1); // Init Tophat image matrix
	Mat Loc_Grad_Image = Mat::zeros(src.rows, src.cols, CV_8UC1); // Init Loc_Gradient image matrix
	Mat Sym_Loc_Grad_Image = Mat::zeros(src.rows, src.cols, CV_8UC1); // Init Loc_Gradient image matrix

	// Running filters.  Arguements optional, input whatever filters you want (in order)
	dL_object.run_filters(TopHat_Image,Loc_Grad_Image,Sym_Loc_Grad_Image);

	//filter by lane marking width
	dL_object.lane_width_filter(Loc_Grad_Image, Sm, SM, horizon);
	dL_object.lane_width_filter(Sym_Loc_Grad_Image, Sm, SM, horizon);

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