#include <iostream>
#include <opencv2/imgproc/imgproc.hpp> // files to process images
#include <opencv2/highgui/highgui.hpp> // files for user interface
#include <fstream>
#include <string>
#include <vector>
#include <doublefann.h>
#include "Defect.h"

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
const char* video_file_path = R"(C:\Users\ResearchBeast\Documents\E16-SMH-041-E16-SMH-039_012215_1372_1.MPG)";
const char* image_file_path = R"(C:\Users\ResearchBeast\Documents\Visual Studio 2015\Projects\OpenCVUSefulPrograms\OpenCVUSefulPrograms\Debug\SamplePipeCrack.jpg)";
const int frames_per_second = 30;
// edge detection variables
int lowThreshold = 250;
int ratio = 3;
// subsampling variables
int subsample_ratio = 3;
int main() {
	// location of video we will use to train
	vector<Defect> videos;
	// Get all of the Defects in list
	ifstream ifs(R"(C:\Users\ResearchBeast\Documents\Visual Studio 2015\Projects\CNN_Defect_Detection\Debug\videodefectdata.txt)");
	string line;
	while (getline(ifs, line))
	{
		if (!isdigit(line.at(0)))
		{
			Defect def(line);
			videos.push_back(def);
		}
		else
		{
			// get start time
			string start_time = line;
			getline(ifs, line);
			// get end time
			string end_time = line;
			videos.back().AddTimeFrame(
				stoi(start_time.substr(0, start_time.find(":"))),
				stoi(start_time.substr(start_time.find(":") + 1, start_time.length())),
				stoi(end_time.substr(0, end_time.find(":"))),
				stoi(end_time.substr(end_time.find(":") + 1, end_time.length()))
				);
		}
	}
	// window names
	char* normal_window_name = "Normal video";
	char* canny_window_name = "Edge video";
	char* subsample_window_name = "Subsampled video";
	// create windows
	namedWindow(normal_window_name, WINDOW_NORMAL);
	namedWindow(canny_window_name, WINDOW_NORMAL);
	namedWindow(subsample_window_name, WINDOW_NORMAL);
	// Multiple frames
	Mat normal_frame;
	Mat canny_frame;
	Mat subsample_frame;
	// Open video capture
	VideoCapture cap;
	// load first video to get variables sizes for CNN
	cap.open(videos.at(0).GetFilePath());
	if (!cap.isOpened())
		cout << "\n********\nCouldn't open video\n******" << endl;
	// Load first frame to print data and create array
	cap >> normal_frame;
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
	//
	// Part 1 Create Training File
	//
	int defect_index = 0;
	bool are_defects_left = false;
	bool on_defect = false;
	// create training data file
	ofstream pixel_data_file;
	pixel_data_file.open("training.data", ios::app);
	for (std::vector<Defect>::iterator it = videos.begin(); it != videos.end(); ++it) {
		// check if video contains defect
		are_defects_left = it->ContainsDefects();
		defect_index = 0;
		// open video
		cap.open(it->GetFilePath());
		if (!cap.isOpened())
			cout << "\n********\nCouldn't open video\n******" << endl;
		// Load first frame to print data and create array
		cap >> normal_frame;
		cout << "Original size: ";
		cout << normal_frame.cols << ", " << normal_frame.rows << endl;
		cout << "Subsample size: ";
		cout << normal_frame.cols / (2 * subsample_ratio) << ", " << normal_frame.rows / (2 * subsample_ratio) << endl;
		try
		{
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
					pixel_data_file << "\n";
					// if there are no defects left then enter zero
					if (!are_defects_left)
					{
						pixel_data_file << "0\n";
					}
					else
					{
						if (cap.get(CV_CAP_PROP_POS_FRAMES) > it->GetStartFrame(defect_index) && cap.get(CV_CAP_PROP_POS_FRAMES) < it->GetEndFrame(defect_index))
						{
							pixel_data_file << "1\n";
							on_defect = true;
						}
						else
						{
							pixel_data_file << "0\n";
							if (on_defect)
							{
								defect_index++;
								if (defect_index > it->GetNumberOfDefects())
								{
									are_defects_left = false;
								}
							}
						}

					}

				}
				// finalizes the loading process
				waitKey(1);
			}
		}
		catch (Exception e)
		{

		}

		//
		// Part 2 Train Nework
		//


		// create Neural Network
		struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
		fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
		fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	}
	return 0;
}