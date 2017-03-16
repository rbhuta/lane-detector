#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace cv;
using namespace std;

Mat src;
string window_name = "Top Hat Filter using Integral Image";

/* Run with: g++ `pkg-config --cflags opencv` detectLanes.cpp `pkg-config --libs opencv` -o test; ./test */
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
	for (int i = 0; i < horizon; ++i)
	{
		SM[i] = SM_max / (src.rows - horizon) * (i - horizon); 
		Sm[i] = Sm_max / (src.rows - horizon) * (i - horizon);
	}

	vector<int> Row_Int_Image(src.cols); // Init vector length
	Mat TopHat_Image(src.rows, src.cols, CV_32SC1); // Init Tophat image matrix

	for (int i = src.rows; i > horizon; --i) {
		// Compute Individual Row by Row Integral Image
		int sum = (int)src.at<uchar>(i, 0);
		for (int j = 0; j < src.cols; ++j) {

			Row_Int_Image[j] = sum;
			sum += (int)src.at<uchar>(i,j);
		}

		// Apply TopHat Filter
		// Why must s be odd?
		int s = round(SM[i] + Sm[i])*(0.25);

		for (int u = 2*s; u < src.cols - (2*s); ++u) {
			TopHat_Image.at<int>(i, u) = (int)((1/(4*s)) * (2*(Row_Int_Image[u+s] - Row_Int_Image[u-s]) - (Row_Int_Image[u + 2*s] - Row_Int_Image[u - 2*s])));

			// cout << "Value: " << ((1/4*s) * (2*(Row_Int_Image[u+s] - Row_Int_Image[u-s]) - (Row_Int_Image[u + 2*s] - Row_Int_Image[u - 2*s]))) << endl;
		}
	}

	//Display Results
	// Mat dst; 
	// normalize(TopHat_Image, dst, 0, 1, NORM_MINMAX);
	// namedWindow( window_name, WINDOW_AUTOSIZE ); // Create a window to display results
	// imshow(window_name, dst);
	// moveWindow(window_name, 10, 10);
	// waitKey(0);

	return 0;
}
