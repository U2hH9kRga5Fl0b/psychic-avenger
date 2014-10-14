
#include "geometry.h"


#define EPSILON 1e-15

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>

bool intersect(
	double x1b, double y1b,
	double x1e, double y1e,
	
	double x2b, double y2b,
	double x2e, double y2e,
	
	double& x_intersection, double& y_intersection)
{
	double m1 = (y1e - y1b) / (x1e - x1b);
	double m2 = (y2e - y2b) / (x2e - x2b);

	double b1 = y1b - m1 * x1b;
	double b2 = y2b - m2 * x2b;
	
	
	x_intersection = (b2 - b1) / (m1 - m2);
	y_intersection = m1 * x_intersection + b1;
	
//	std::cout << " ---------------------------------------- " << std::endl;
//	std::cout << -b1 + y1e - m1 * x1e << std::endl;
//	std::cout << -b2 + y2e - m2 * x2e << std::endl;
//	std::cout << -y_intersection + m2 * x_intersection + b2 << std::endl;
//	std::cout << -y1b + m1 * x1b + b1 << std::endl;
//	std::cout << -y2b + m2 * x2b + b2 << std::endl;
//	std::cout << -y1e + m1 * x1e + b1 << std::endl;
//	std::cout << -y2e + m2 * x2e + b2 << std::endl;
//	std::cout << y_intersection - (m1 * x_intersection + b1) << std::endl;
//	std::cout << y_intersection - (m2 * x_intersection + b2) << std::endl;
	
	if (x_intersection > std::max(x1e, x1b))
	{
		return false;
	}
	if (x_intersection < std::min(x1e, x1b))
	{
		return false;
	}
	if (y_intersection > std::max(y1e, y1b))
	{
		return false;
	}
	if (y_intersection < std::min(y1e, y1b))
	{
		return false;
	}
	if (x_intersection > std::max(x2e, x2b))
	{
		return false;
	}
	if (x_intersection < std::min(x2e, x2b))
	{
		return false;
	}
	if (y_intersection > std::max(y2e, y2b))
	{
		return false;
	}
	if (y_intersection < std::min(y2e, y2b))
	{
		return false;
	}
	
	return true;
}



void test_intersect()
{
#if GRAPHICS
	cv::Scalar color1{
		255 * (rand() / (double) RAND_MAX),
		255 * (rand()  / (double) RAND_MAX),
		255 * (rand()  / (double) RAND_MAX)};
	cv::Scalar color2{
		255 * (rand() / (double) RAND_MAX),
		255 * (rand()  / (double) RAND_MAX),
		255 * (rand()  / (double) RAND_MAX)};
	cv::Scalar color3{
		255 * (rand() / (double) RAND_MAX),
		255 * (rand()  / (double) RAND_MAX),
		255 * (rand()  / (double) RAND_MAX)};
	
	int width  = 640;
	int height = 480;
	
	cv::Mat mat = cv::Mat::zeros(height, width, CV_8UC3);
	
	cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("test", 100, 100);
	
	
	for (;;)
	{
		mat = cv::Scalar(0, 0, 0);
	
		double x1 = width  * (rand() / (double) RAND_MAX);
		double x2 = width  * (rand() / (double) RAND_MAX);
		double x3 = width  * (rand() / (double) RAND_MAX);
		double x4 = width  * (rand() / (double) RAND_MAX);
		double y1 = height * (rand() / (double) RAND_MAX);
		double y2 = height * (rand() / (double) RAND_MAX);
		double y3 = height * (rand() / (double) RAND_MAX);
		double y4 = height * (rand() / (double) RAND_MAX);
		
		{
			cv::Point first((int) x1, (int) y1);
			cv::Point second((int) x2, (int) y2);
			cv::line(mat, first, second, color1, 1, 8, 0);
		}
	
		{
			cv::Point first((int) x3, (int) y3);
			cv::Point second((int) x4, (int) y4);
			cv::line(mat, first, second, color2, 1, 8, 0);
		}
		
		double x,y;
	
		std::stringstream s;
		s << "value=" << intersect(
			x1, y1,
			x2, y2,
			x3, y3,
			x4, y4,
			
			x, y
		);
		
		cv::circle(mat, cv::Point((int)x1, (int)y1), 5, color1);
		cv::circle(mat, cv::Point((int)x2, (int)y2), 5, color1);
		cv::circle(mat, cv::Point((int)x3, (int)y3), 5, color2);
		cv::circle(mat, cv::Point((int)x4, (int)y4), 5, color2);
		
		cv::circle(mat, cv::Point((int)x, (int)y), 5, color3);
	
		cv::putText(mat, s.str(), cv::Point(0,20), CV_FONT_HERSHEY_PLAIN, 1.0, color3);
	
		cv::imshow("test", mat);
		cv::waitKey(2000);
	}
#else
	std::cout << "Unable to test intersect without graphics." << std::endl;
#endif
}



bool find_intersecting_paths(solution* sol, int& idx1, int& idx2, int attempts)
{
	int n = sol->get_city()->num_stops;
	
	if (sol->length() <= 4)
	{
		// need at least 4 points...
		return false;
	}
	
	do
	{
		idx1 = rand() % (n-1);
	} while (sol->get_stop(idx1 + 1) < 0 || sol->get_stop(idx1) < 0);
	
	do
	{
		idx2 = rand() % (n-1);
	} while (sol->get_stop(idx2 + 1) < 0 || sol->get_stop(idx2) < 0 || idx1 == idx2);
	
	if (idx1 > idx2)
	{
		int tmp = idx1;
		idx1 = idx2;
		idx2 = tmp;
	}
	
	int start1 = idx1;
	int start2 = idx2 - 1;
	
	city* c = sol->get_city();
	
	for (int i=0; i<attempts; i++)
	{
		double x_inter, y_inter;
		
		
		
		if (intersect(
			c->locsx[idx1  ], c->locsy[idx1  ],
			c->locsx[idx1+1], c->locsy[idx1+1],
			c->locsx[idx2  ], c->locsy[idx2  ],
			c->locsx[idx2+1], c->locsy[idx2+1],
			x_inter, y_inter))
		{
		
			return true;
		}
		
		if (idx2 == start2 && idx1 == start1)
		{
 			// we have looped all the way around
			return false;
		}
		
		idx2++;
		if (idx2 + 1 >= n || sol->get_stop(idx2 + 1) < 0)
		{
			idx2 = 0;
		}
		if (idx2 == start2)
		{
			idx1++;
			if (idx1 + 1 >= n || sol->get_stop(idx1 + 1) < 0)
			{
				idx1 = 0;
			}
		}
	}

	return false;
}



void get_bounds(double *allx, double *ally, int n,
		double& minx, double& maxx, double& miny, double& maxy)
{
	minx = +DBL_MAX;
	maxx = -DBL_MAX;
	miny = +DBL_MAX;
	maxy = -DBL_MAX;
	
	for (int i=0; i<n; i++)
	{
		double x = allx[i];
		double y = ally[i];
		
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
}