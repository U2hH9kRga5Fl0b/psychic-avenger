

#ifndef VIEWER_H_
#define VIEWER_H_

#include "common.h"

#include "solution.h"

#include <chrono>
#include <mutex>

#include <opencv2/opencv.hpp>


#if GRAPHICS
void view_city(city* ci, const std::string& name);
void view_city(city* bigcity, cv::Mat& mat, int width, int height);
#endif

class viewer
{
public:
	viewer(const std::string& name);
	~viewer();
	
	void update(const solution* sol);
	void snapshot(const solution* sol, const std::string& filename);
	
	void pause(int length=0);
private:
	solution *sol;
	std::string winname;

#if GRAPHICS
	cv::Mat mat;
	cv::Scalar color;
	cv::VideoWriter vid;
#endif
	int location;
	
	std::chrono::steady_clock::time_point start;
	std::ofstream costs;
	int count;

	std::mutex mmut;

};

#endif
