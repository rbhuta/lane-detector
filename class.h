#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace cv;
using namespace std;

class detectLane {
public:
	Mat src;
	vector<double> SM(src.rows);
	vector<double> Sm(src.rows);

}
void detectLane::run_filters(Mat& TopHat_Image = NULL, Mat& Loc_Grad_Image = NULL, Mat& Sym_Loc_Grad_Image = NULL);

void detectLane::tophat_filter(vector<double>& Row_Int_Image, int row_idx, Mat& TopHat_Image, double s);

void detectLane::local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Loc_Grad_Image, double s, int thresh_grad);

void detectLane::sym_local_gradient(vector<double>& Row_Int_Image, int row_idx, Mat& Sym_Loc_Grad_Image, double s, int thresh_sym_grad);

void detectLane::lane_width_filter(Mat& Image, vector<double>& Sm, vector<double>& SM, int horizon);