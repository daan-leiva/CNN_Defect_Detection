#include <iostream>
#include <opencv2/imgproc/imgproc.hpp> // files to process images
#include <opencv2/highgui/highgui.hpp> // files for user interface

using namespace std;
using namespace cv;

/*
This program
*/

//
// function headers
//

//
// global variables
//
// location of video we will use to train
const char* video_file_path = R"(C:\Users\ResearchBeast\Documents\E16-SMH-041-E16-SMH-039_012215_1372_1.MPG)";
const char* image_file_path = R"(C:\Users\ResearchBeast\Documents\Visual Studio 2015\Projects\OpenCVUSefulPrograms\OpenCVUSefulPrograms\Debug\SamplePipeCrack.jpg)";
// edge detection variables
int lowThreshold = 250;
int ratio = 3;
// subsampling variables
int subsample_ratio = 2;
int main() {
	// window for regular video
	char* normal_window_name = "Normal video";
	char* canny_window_name = "Edge video";
	char* subsample_window_name = "Subsampled video";
	namedWindow(normal_window_name, WINDOW_NORMAL);
	namedWindow(canny_window_name, WINDOW_NORMAL);
	namedWindow(subsample_window_name, WINDOW_NORMAL);
	// Video capture variable
	VideoCapture cap;
	cap.open(video_file_path);
	// check that video is opened succesfully
	if (!cap.isOpened())
		cout << "\n********\nCouldn't open video\n******" << endl;
	// frame to hold normal video
	Mat normal_frame;
	// frame to load canny image
	Mat canny_frame;
	// frame to hold subsampled video
	Mat subsample_frame;
	// load first frame for useful purposes
	cap >> normal_frame;
	// print key datums
	cout << "Original size: ";
	cout << normal_frame.cols << ", " << normal_frame.rows << endl;
	cout << "Subsample size: ";
	cout << normal_frame.cols/(2*subsample_ratio) << ", " << normal_frame.rows / (2 * subsample_ratio) << endl;
	// create array to hold data for CNN
	int* picture_pixels = new int[normal_frame.cols / (2 * subsample_ratio)*normal_frame.rows / (2 * subsample_ratio)];
	// play video
	while (1) {
		// load current grame from stream
		cap >> normal_frame;
		subsample_frame = normal_frame;
		// check that there is frames left
		if (!normal_frame.data)
			break;
		// create canny frame
		Canny(normal_frame, canny_frame, lowThreshold, lowThreshold*ratio);
		// calculate modified frame and load it		
		pyrDown(canny_frame, subsample_frame);
		try
		{
			for (int i = 0; i < subsample_ratio - 1; i++)
				pyrDown(subsample_frame, subsample_frame);
		}
		catch (Exception e)
		{
			cout << "Exception";
		}
		// load normal frame
		imshow(normal_window_name, normal_frame);
		// load canny frame
		imshow(canny_window_name, canny_frame);
		// load subsample frame
		imshow(subsample_window_name, subsample_frame);
		// load matrix into 1D vector
		int* pixel_data = (int*)subsample_frame.data;
		for (int i = 0; i < subsample_frame.cols*subsample_frame.rows; i++)
			cout << pixel_data[i] << endl;
		// finalizes the loading process
		waitKey(1);
	}
	return 0;
}