
#include "viewer.h"
#include "geometry.h"

#include "common.h"

#include <opencv/highgui.h>

#define default_width  500
#define default_height 500

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
			
				next_locx += default_width + 50;
				if (next_locx > maxx)
				{
					next_locx = 100;
					next_locy += default_height + 50;
				
					if (next_locy > maxy)
					{
						break;
					}


				}
			}
		}
		
		for (int i=0; i<locs.size(); i++)
		{
			if (!locs.at(i).used)
			{
				return i;
			}
		}
		
		std::cout << "No more available locations for a screen!";
		trap();
	}
}

void release_viewing_resources()
{
#if GRAPHICS
	cv::destroyAllWindows();
#endif
	locs.clear();
//	pthread_mutex_destroy(&mut);
}

viewer::viewer(solution* sol_, const std::string& name, int freq_) :
	sol{sol_},
	winname{name},
#if GRAPHICS
	mat{cv::Mat::zeros(default_height, default_width, CV_8UC3)},
	color{
		255 * (rand() / (double) RAND_MAX),
		255 * (rand()  / (double) RAND_MAX),
		255 * (rand()  / (double) RAND_MAX)},
//	color{255, 255, 255},

	vid {"output/" + name + ".mpeg", CV_FOURCC('P','I','M','1'), 120, cv::Size(default_width, default_height)},
#endif
	minx{DBL_MAX},
	miny{DBL_MAX},
	maxx{-DBL_MAX},
	maxy{-DBL_MAX},
	freq{freq_},
	start{std::chrono::steady_clock::now()},
	costs{"output/" + name + ".txt"},
	count{0}
{
	city* c = sol->get_city();
	get_bounds(c->locsx, c->locsy, c->num_cities, minx, maxx, miny, maxy);
	
	std::lock_guard<std::mutex> lock(mut);
	

	location = get_next_loc();
	locs.at(location).used = true;
	
#if GRAPHICS	
	cvNamedWindow(winname.c_str(), CV_WINDOW_AUTOSIZE);
	cvMoveWindow(winname.c_str(), locs.at(location).x, locs.at(location).y);
#endif
	
	cnt = INT_MAX;
}

void viewer::snapshot(const std::string& filename)
{
	std::lock_guard<std::mutex> lock(mut);
#if GRAPHICS
	cv::imwrite(filename, mat);
#endif
}

viewer::~viewer()
{
	std::lock_guard<std::mutex> lock(mut);
#if GRAPHICS
	cv::destroyWindow(winname);
#endif
	locs.at(location).used = false;
}



void viewer::pause(int length)
{
#if GRAPHICS
	cv::waitKey(length);
#endif
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
	
	int num_cities = sol->get_city()->num_cities;
	
#if GRAPHICS
	mat = cv::Scalar(0, 0, 0);
	
	costs << sol->get_cost() << ' ' << count++ << ' ' << 
		std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() 
		<< "; ..." << std::endl;
	
	
	double x = sol->get_city()->locsx[sol->get_stop(0)];
	double y = sol->get_city()->locsy[sol->get_stop(0)];
	x = default_width  * (x - minx) / (maxx - minx);
	y = default_height * (y - miny) / (maxy - miny);
	
	cv::Point prev((int)x, (int)y);
	cv::circle(mat, prev, 3, color);
	
	for (int i=1; i<num_cities; i++)
	{
		if (sol->get_stop(i) < 0)
		{
			break;
		}
		
		x = sol->get_city()->locsx[sol->get_stop(i)];
		y = sol->get_city()->locsy[sol->get_stop(i)];
		
		x = default_width  * (x - minx) / (maxx - minx);
		y = default_height * (y - miny) / (maxy - miny);
		
		cv::Point next((int) x, (int) y);
		cv::circle(mat, next, 3, color);
		
		cv::line(mat, prev, next, color, 1, 8, 0);
		
		prev = next;
	}
	
	std::stringstream s;
	s << "cost=" << sol->get_cost();
	
	cv::Scalar invert = cv::Scalar::all(255) - color;
	
	cv::putText(mat, s.str(), cv::Point(0,20), CV_FONT_HERSHEY_PLAIN, 1.0, invert);
	vid.write(mat);
	
	imshow(winname, mat);
	cv::waitKey(1);
#else
//	std::cout << (*sol) << std::endl;
#endif
}

#if GRAPHICS
void view_city(city* ci, const std::string& name)
{
	cvNamedWindow(name.c_str(), CV_WINDOW_AUTOSIZE);
	cvMoveWindow(name.c_str(), 100,100);
	
	int height = 500;
	int width  = 500;
	
	cv::Mat mat{cv::Mat::zeros(height, width, CV_8UC3)};
	view_city(ci, mat, width, height);
	
	imshow(name.c_str(), mat);
	cv::waitKey(0);
}

void view_city(city* bigcity, cv::Mat& mat, int width, int height)
{
	int n = bigcity->num_cities;
	double xmin, xmax, ymin, ymax;
	get_bounds(bigcity->locsx, bigcity->locsy, bigcity->num_cities, xmin, xmax, ymin, ymax);
	
	cv::Scalar color{
		255 * (rand() / (double) RAND_MAX),
		255 * (rand() / (double) RAND_MAX),
		255 * (rand() / (double) RAND_MAX)};
	
	for (int i=0; i<bigcity->num_cities; i++)
	{
		double x = bigcity->locsx[i];
		double y = bigcity->locsy[i];
		x = width  * (x - xmin) / (xmax - xmin);
		y = height * (y - ymin) / (ymax - ymin);
		cv::Point next{(int)x, (int)y};
		cv::circle(mat, next, 2, color);
	}
}
#endif