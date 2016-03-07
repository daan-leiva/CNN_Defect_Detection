#include "Defect.h"
#include <string>


Defect::Defect(string videopath) :
	video_path(videopath)
{

}

void Defect::AddTimeFrame(int startminute, int startseconds, int endminute, int endseconds)
{
	TimeFrame frame(startminute, startseconds, endminute, endseconds);
	time_gaps.push_back(frame);
}

string Defect::GetFilePath()
{
	return video_path;
}

bool Defect::ContainsDefects()
{
	return time_gaps.size() > 0;
}

int Defect::GetStartFrame(int index)
{
	return time_gaps.at(index).GetStartFrame();
}

int Defect::GetEndFrame(int index)
{
	return time_gaps.at(index).GetEndFrame();
}

int Defect::GetNumberOfDefects()
{
	return time_gaps.size();
}
