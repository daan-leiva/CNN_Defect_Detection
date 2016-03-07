#pragma once
class TimeFrame
{
public:
	TimeFrame(int start_minute, int start_seconds, int end_minute, int end_seconds);
	int GetStartFrame();
	int GetEndFrame();
private:
	const int frames_per_second = 30;
	const int seconds_per_minute = 60;
	int start_minute;
	int start_seconds;
	int end_minute;
	int end_seconds;
	int start_frame;
	int end_frame;
};

