#include "TimeFrame.h"



TimeFrame::TimeFrame(int startminute, int startseconds, int endminute, int endseconds) :
	start_minute(startminute), start_seconds(startseconds), end_minute(endminute), end_seconds(endseconds)
{
	start_frame = startminute*seconds_per_minute*frames_per_second + start_seconds*frames_per_second;
	end_frame = endminute*seconds_per_minute*frames_per_second + end_seconds*frames_per_second;	
}

int TimeFrame::GetStartFrame()
{
	return start_frame;
}

int TimeFrame::GetEndFrame()
{
	return end_frame;
}
