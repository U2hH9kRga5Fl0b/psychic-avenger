
#include "viewer.h"

#include <opencv/highgui.h>

#define width  640
#define height 480

#include <vector>
#include <mutex>

namespace
{
	typedef struct { int x; int y; bool used;} img_loc;
	
	std::vector<img_loc> locs;
	std::mutex mut;
	
	int get_next_loc()
	{
//		std::lock_guard<std::mutex> lock(mut);
		
		if (locs.size() == 0)
		{
			int maxx = 5000;
			int maxy = 1080;
		
			int next_locx = 100;
			int next_locy = 100;
			
			while (true)
			{
				img_loc l;
				l.x = next_locx;
				l.y = next_locy;
				l.used = false;
				
				locs.push_back(l);
			
				next_locx += width + 50;
				if (next_locx > maxx)
				{
					next_locx = 100;
					next_locy += height + 50;
				
					if (next_locy > maxy)
					{
						break;
					}
				}
			}
		}
		
		for (int i=0;i<locs.size();i++)
		{
			if (!locs.at(i).used)
			{
				return i;
			}
		}
		
		std::cout << "No more available locations for a screen!";
		exit(-1);
	}
}

viewer::viewer(solution* sol_, const std::string& name, int freq_) :
	sol{sol_},
	winname{name},
	mat{cv::Mat::zeros(height, width, CV_8UC3)},
	color{
		255 * (rand() / (double) RAND_MAX),
		255 * (rand()  / (double) RAND_MAX),
		255 * (rand()  / (double) RAND_MAX)},
//	color{255, 255, 255},
	minx{DBL_MAX},
	miny{DBL_MAX},
	maxx{-DBL_MAX},
	maxy{-DBL_MAX},
	freq{freq_}
{
	city *c = sol->c;
	for (int i=0; i<c->num_cities; i++)
	{
		double x = c->locsx[i];
		double y = c->locsy[i];
		
		if (x < minx)
		{
			minx = x;
		}
		if (x > maxx)
		{
			maxx = x;
		}
		if (y < miny)
		{
			miny = y;
		}
		if (y > maxy)
		{
			maxy = y;
		}
	}
	
	std::lock_guard<std::mutex> lock(mut);
	
	cvNamedWindow(winname.c_str(), CV_WINDOW_AUTOSIZE);
	
	location = get_next_loc();
	locs.at(location).used = true;
	cvMoveWindow(winname.c_str(), locs.at(location).x, locs.at(location).y);
	
	
	cnt = INT_MAX;
}

void viewer::snapshot(const std::string& filename)
{
	
	std::lock_guard<std::mutex> lock(mut);
	cv::imwrite(filename, mat);
}

viewer::~viewer()
{
	std::lock_guard<std::mutex> lock(mut);
	cv::destroyWindow(winname);
	locs.at(location).used = false;
}

void viewer::update()
{
	std::lock_guard<std::mutex> lock(mut);
	if (freq == -1)
	{
		return;
	}
	else if (freq != 0)
	{
		if (cnt++ < freq)
		{
			return;
		}
		cnt = 0;
	}
	
	
	int num_cities = sol->c->num_cities;
	
	mat = cv::Scalar(0, 0, 0);
	
	cv::Point prev(sol->c->locsx[sol->path[0]],
		       sol->c->locsy[sol->path[0]]);
	
	for (int i=1; i<num_cities; i++)
	{
		if (sol->path[i] < 0)
		{
			break;
		}
		
		double x = sol->c->locsx[sol->path[i]];
		double y = sol->c->locsy[sol->path[i]];
		
		x = width  * (x - minx) / (maxx - minx);
		y = height * (y - miny) / (maxy - miny);
		
		cv::Point next((int) x, (int) y);
		
		cv::circle(mat, next, 3, color);
		cv::line(mat, prev, next, color, 1, 8, 0);
		
		prev = next;
	}
	
	std::stringstream s;
	s << "cost=" << sol->get_cost();
	
	cv::putText(mat, s.str(), cv::Point(0,20), CV_FONT_HERSHEY_PLAIN, 1.0, color);
	
	imshow(winname, mat);
	cv::waitKey(1);
}
