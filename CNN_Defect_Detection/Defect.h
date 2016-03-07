#pragma once

#include <vector>
#include "TimeFrame.h"
#include <string>

using namespace std;

class Defect
{
public:
	Defect(string videoname);
	void AddTimeFrame(int startminute, int startseconds, int endminutes, int endseconds);
	string GetFilePath();
	bool ContainsDefects();
	int GetStartFrame(int index);
	int GetEndFrame(int index);
	int GetNumberOfDefects();
private:
	string video_path;
	vector<TimeFrame> time_gaps;
};

