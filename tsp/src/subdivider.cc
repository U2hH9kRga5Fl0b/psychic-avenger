
#include "subdivider.h"


#include "common.h"
#include "geometry.h"



#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>




subcity::subcity(city* p, city* s, int* m) :
	parent{p}, c{s}, mapping{m} {}
subcity::~subcity() { /*delete parent;*/ delete c; delete mapping; }



namespace
{
	bool in_bounds(double x, double y,
		       double xmin, double xmax, double ymin, double ymax)
	{
		if (x < xmin)
		{
			return false;
		}
		if (x > xmax)
		{
			return false;
		}
		if (y < ymin)
		{
			return false;
		}
		if (y > ymax)
		{
			return false;
		}
		return true;
	}
	
	
}


subcity *get_subcity(city *old, double xmin, double xmax, double ymin, double ymax)
{
	int n = old->num_stops;
	int count = 0;
	for (int i=0;i<n;i++)
	{
		if (in_bounds(old->locsx[i], old->locsy[i],
			xmin, xmax, ymin, ymax))
		{
			count++;
		}
	}
	
	double* xs = new double[count];
	double* ys = new double[count];
	int *map = new int[count];
	
	count = 0;
	for (int i=0;i<n;i++)
	{
		if (!in_bounds(old->locsx[i], old->locsy[i],
			xmin, xmax, ymin, ymax))
		{
			continue;
		}
		xs[count] = old->locsx[i];
		ys[count] = old->locsy[i];
		map[count] = i;
		count++;
	}
	
	return new subcity(old, new city(count, xs, ys), map);
}


void append_to(subcity *subcity, solution *subsol, solution *sol)
{
	if (subsol == nullptr)
	{
		return;
	}
	
	
//	std::cout << "Trying to append this: " << std::endl;
//	std::cout << *subsol << std::endl;
//	std::cout << "to this: " << std::endl;
//	std::cout << *sol << std::endl;
//	for (int i=0;i<subcity->c->num_stops;i++)
//	{
//		std::cout << " " << subcity->mapping[i];
//	}
//	std::cout << std::endl;
	
	// Right now we will just be really dumb and append them....
	if (sol->get_city() != subcity->parent)
	{
		std::cout << "Bad arguments to append_to" << std::endl;
		trap();
		return;
	}
	
	int n = sol->get_city()->num_stops;
	
	int ndx = sol->length();
	
	if (ndx >= n)
	{
		std::cout << "Cannot append to this solution: it is already full!" << std::endl;
		trap();
		return;
	}
	
	int ln = subsol->get_city()->num_stops;
	
	for (int i=0; i < ln; i++, ndx++)
	{
		if (ndx >= n)
		{
			std::cout << "cannot append, because the subcity doesn't fit into this solution!" << std::endl;
			trap();
			return;
		}
		
		int stop = subcity->mapping[subsol->get_stop(i)];
		sol->set_path_at(ndx, stop);
	}
	
//	std::cout << "Created this: " << std::endl;
//	std::cout << *sol << std::endl;
	
	sol->is_valid();
}


#include "viewer.h"

namespace
{
	
	



bool inited;

void draw_square(
	city* bigcity,
	double xmini, double xmaxi, double ymini, double ymaxi)
{
#if GRAPHICS
	double xmin, xmax, ymin, ymax;
	get_bounds(bigcity->locsx, bigcity->locsy, bigcity->num_stops, xmin, xmax, ymin, ymax);
	
	int height = 500;
	int width  = 500;
	cv::Mat mat{cv::Mat::zeros(height, width, CV_8UC3)};

	view_city(bigcity, mat, width, height);
	
	if (!inited)
	{
	//	location = get_next_loc();
	//	locs.at(location).used = true;
		cvNamedWindow("current_square", CV_WINDOW_AUTOSIZE);
		cvMoveWindow("current_square", 100,100);
		inited = true;
	}
	
	int pxmin = (int)(width  * (xmini - xmin) / (xmax - xmin));
	int pxmax = (int)(width  * (xmaxi - xmin) / (xmax - xmin));
	int pymin = (int)(height * (ymini - ymin) / (ymax - ymin));
	int pymax = (int)(height * (ymaxi - ymin) / (ymax - ymin));
	
	std::cout << pxmin << "\n";
	
	cv::Scalar color2{
		255 * (rand() / (double) RAND_MAX),
		255 * (rand() / (double) RAND_MAX),
		255 * (rand() / (double) RAND_MAX)};
	
	cv::line(mat, cv::Point(pxmin,pymin), cv::Point(pxmax,pymin), color2, 1, 8, 0);
	cv::line(mat, cv::Point(pxmax,pymin), cv::Point(pxmax,pymax), color2, 1, 8, 0);
	cv::line(mat, cv::Point(pxmax,pymax), cv::Point(pxmin,pymax), color2, 1, 8, 0);
	cv::line(mat, cv::Point(pxmin,pymax), cv::Point(pxmin,pymin), color2, 1, 8, 0);
	
	imshow("current_square", mat);
	cv::waitKey(1000);
#else
	std::cout << xmini << ", " << xmaxi << ", " << ymini << ", " << ymaxi << std::endl;
#endif
}




city* bigcity_513;

solution *backtrack(city *beginning,
	double xmin, double xmax, double ymin, double ymax,
	int max_num_stops,
	void (improve)(solution* s),
	int& total_num_solved)
{
	std::cout << "backtracking to [" << xmin << "," << xmax << "],[" << ymin << "," << ymax << "] with n=" << beginning->num_stops << std::endl;
	std::cout.flush();
	
//	std::this_thread::sleep_for(std::chrono::milliseconds{1000});
	
	if (beginning->num_stops == 0)
	{
		return nullptr;
	}
	
	
	if (beginning->num_stops < max_num_stops)
	{
		draw_square(bigcity_513,
			xmin, xmax, ymin, ymax);
		solution *sol = new solution{beginning};
		sol->random();
		improve(sol);
		return sol;
	}
	
	double midx = (xmin + xmax) / 2;
	double midy = (ymin + ymax) / 2;
	
	solution *sol = new solution{beginning};
	
	// subdivide the city up into 4 regions...
	
	// upper right
	{
		subcity *sub = get_subcity(beginning, midx, xmax, midy, ymax);
		solution* subsol =  backtrack(sub->c, midx, xmax, midy, ymax, max_num_stops, improve, total_num_solved);
		append_to(sub, subsol, sol);
		delete subsol;
		delete sub;
	}
	// upper left
	{
		subcity *sub = get_subcity(beginning, xmin, midx, midy, ymax);
		solution* subsol =  backtrack(sub->c, xmin, midx, midy, ymax, max_num_stops, improve, total_num_solved);
		append_to(sub, subsol, sol);
		delete subsol;
		delete sub;
	}
	// lower left
	{
		subcity *sub = get_subcity(beginning, xmin, midx, ymin, midy);
		solution* subsol =  backtrack(sub->c, xmin, midx, ymin, midy, max_num_stops, improve, total_num_solved);
		append_to(sub, subsol, sol);
		delete subsol;
		delete sub;
	}
	// lower right
	{
		subcity *sub = get_subcity(beginning, midx, xmax, ymin, midy);
		solution* subsol =  backtrack(sub->c, midx, xmax, ymin, midy, max_num_stops, improve, total_num_solved);
		append_to(sub, subsol, sol);
		delete subsol;
		delete sub;
	}
	
	
	draw_square(
		bigcity_513,
		xmin, xmax, ymin, ymax);
	improve(sol);
	
	if (sol->get_city()->num_stops > total_num_solved)
	{
		total_num_solved = sol->get_city()->num_stops;
		std::cout << "We have now solved a " <<  total_num_solved << " city." << std::endl;
	}
	
	
	
	
	return sol;
}


}


solution *subdivide_backtrack(city *beginning, void (improve)(solution* s), int max_num_stops)
{
	bigcity_513 = beginning;
	double xmin_513, xmax_513, ymin_513, ymax_513;
	get_bounds(beginning->locsx, beginning->locsy, beginning->num_stops,
		xmin_513, xmax_513, ymin_513, ymax_513);
	
	int total_num_solved = 0;
	
	return backtrack(beginning, xmin_513, xmax_513, ymin_513, ymax_513, max_num_stops, improve, total_num_solved);
}



























