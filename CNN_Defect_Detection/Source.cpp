#include <iostream>
#include <opencv2/imgproc/imgproc.hpp> // files to process images
#include <opencv2/highgui/highgui.hpp> // files for user interface
#include <fann_cpp.h>
#include <fstream>

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
	// window names
	char* normal_window_name = "Normal video";
	char* canny_window_name = "Edge video";
	char* subsample_window_name = "Subsampled video";
	// create windows
	namedWindow(normal_window_name, WINDOW_NORMAL);
	namedWindow(canny_window_name, WINDOW_NORMAL);
	namedWindow(subsample_window_name, WINDOW_NORMAL);
	// Open video capture
	VideoCapture cap;
	cap.open(video_file_path);
	if (!cap.isOpened())
		cout << "\n********\nCouldn't open video\n******" << endl;
	// Multiple frames
	Mat normal_frame;
	Mat canny_frame;
	Mat subsample_frame;
	// Load first frame to print data and create array
	cap >> normal_frame;
	cout << "Original size: ";
	cout << normal_frame.cols << ", " << normal_frame.rows << endl;
	cout << "Subsample size: ";
	cout << normal_frame.cols / (2 * subsample_ratio) << ", " << normal_frame.rows / (2 * subsample_ratio) << endl;

	//
	// Neural network
	//

	// create array to hold data for CNN
	unsigned char* pixel_data = new unsigned char[normal_frame.cols / (2 * subsample_ratio)*normal_frame.rows / (2 * subsample_ratio)];
	// create CNN variables
	const unsigned int num_input = normal_frame.cols / (2 * subsample_ratio)*normal_frame.rows / (2 * subsample_ratio);
	const unsigned int num_output = 1;
	const unsigned int num_layers = 3;
	const unsigned int num_neurons_hidden = normal_frame.cols / (2 * subsample_ratio)*normal_frame.rows / (2 * subsample_ratio);
	const float desired_error = (const float) 0.0001;
	const unsigned int max_epochs = 5000000;
	const unsigned int epochs_between_reports = 100;
	// create Neural Network
	try
	{
		struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
		fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
		fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
		// create file pointer
		ofstream pixel_data_file;
		pixel_data_file.open("training.data", ios::app);
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
			for (int i = 0; i < subsample_ratio - 1; i++)
				pyrDown(subsample_frame, subsample_frame);
			// load normal frame
			imshow(normal_window_name, normal_frame);
			// load canny frame
			imshow(canny_window_name, canny_frame);
			// load subsample frame
			imshow(subsample_window_name, subsample_frame);
			// load matri into 1D vector
			for (int i = 0; i < subsample_frame.cols; i++)
				for (int j = 0; j < subsample_frame.rows; j++)
					pixel_data[i*subsample_frame.cols + j] = subsample_frame.at<unsigned char>(i, j);
			// write vector to text
			// write wether text should be a defect
			if (pixel_data_file.is_open())
			{
				for (int i = 0; i < subsample_frame.rows*subsample_frame.cols; i++)
					pixel_data_file << pixel_data[i] << " ";
			}
			// finalizes the loading process
			waitKey(1);
		}
	}
	catch (Exception e)
	{

	}
	return 0;
}