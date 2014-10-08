

#ifndef VIEWER_H_
#define VIEWER_H_

#include "solution.h"

#include <opencv2/opencv.hpp>


void view_city(city* ci, const std::string& name);
void view_city(city* bigcity, cv::Mat& mat, int width, int height);

class viewer
{
public:
	viewer(solution* sol, const std::string& name, int freq=0);
	~viewer();
	
	void update();
	void snapshot(const std::string& filename);
	
	void pause();
private:
	solution *sol;
	std::string winname;

	cv::Mat mat;
	cv::Scalar color;
	
	double minx, miny, maxx, maxy;
	
	int freq;
	int cnt;
	
	int location;
};

#endif