
#include "subdivider.h"

#include <thread>

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
	int n = old->num_cities;
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
//	for (int i=0;i<subcity->c->num_cities;i++)
//	{
//		std::cout << " " << subcity->mapping[i];
//	}
//	std::cout << std::endl;
	
	// Right now we will just be really dumb and append them....
	if (sol->c != subcity->parent)
	{
		std::cout << "Bad arguments to append_to" << std::endl;
		exit(-1);
		return;
	}
	
	int n = sol->c->num_cities;
	
	int ndx;
	for (ndx=0;ndx<n;ndx++)
	{
		if (sol->path[ndx] < 0)
		{
			break;
		}
	}
	if (ndx >= n)
	{
		std::cout << "Cannot append to this solution: it is already full!" << std::endl;
		exit(-1);
		return;
	}
	
	int ln = subsol->c->num_cities;
	
	for (int i=0; i < ln; i++, ndx++)
	{
		if (ndx >= n)
		{
			std::cout << "cannot append, because the subcity doesn't fit into this solution!" << std::endl;
			exit(-1);
			return;
		}
		
		int stop = subcity->mapping[subsol->path[i]];
		sol->path[ndx] = stop;
		sol->already_serviced[stop] = true;
	}
	
//	std::cout << "Created this: " << std::endl;
//	std::cout << *sol << std::endl;
	
	sol->is_valid();
}



namespace
{
	
	



bool inited;

void draw_square(
	city* bigcity,
	double xmini, double xmaxi, double ymini, double ymaxi)
{	
	int n = bigcity->num_cities;
	double xmin, xmax, ymin, ymax;
	get_bounds(bigcity->locsx, bigcity->locsy, bigcity->num_cities, xmin, xmax, ymin, ymax);
	
	int height = 500;
	int width  = 500;
	cv::Mat mat{cv::Mat::zeros(height, width, CV_8UC3)};
	cv::Scalar color{
		255 * (rand() / (double) RAND_MAX),
		255 * (rand() / (double) RAND_MAX),
		255 * (rand() / (double) RAND_MAX)};
	
	for (int i=0; i<bigcity->num_cities; i++)
	{
		double x = bigcity->locsx[i];
		double y = bigcity->locsy[i];
		x = width  * (x - xmin) / (xmax - xmin);
		y = height * (y - ymin) / (ymax - xmin);
		cv::Point next{(int)x, (int)y};
		cv::circle(mat, next, 2, color);
	}
	
	
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
}




city* bigcity_513;

solution *backtrack(city *beginning,
	double xmin, double xmax, double ymin, double ymax,
	int max_num_cities,
	void (improve)(solution* s),
	int& total_num_solved)
{
	std::cout << "backtracking to [" << xmin << "," << xmax << "],[" << ymin << "," << ymax << "] with n=" << beginning->num_cities << std::endl;
	std::cout.flush();
	
//	std::this_thread::sleep_for(std::chrono::milliseconds{1000});
	
	if (beginning->num_cities == 0)
	{
		return nullptr;
	}
	
	
	if (beginning->num_cities < max_num_cities)
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
		solution* subsol =  backtrack(sub->c, midx, xmax, midy, ymax, max_num_cities, improve, total_num_solved);
		append_to(sub, subsol, sol);
		delete subsol;
		delete sub;
	}
	// upper left
	{
		subcity *sub = get_subcity(beginning, xmin, midx, midy, ymax);
		solution* subsol =  backtrack(sub->c, xmin, midx, midy, ymax, max_num_cities, improve, total_num_solved);
		append_to(sub, subsol, sol);
		delete subsol;
		delete sub;
	}
	// lower left
	{
		subcity *sub = get_subcity(beginning, xmin, midx, ymin, midy);
		solution* subsol =  backtrack(sub->c, xmin, midx, ymin, midy, max_num_cities, improve, total_num_solved);
		append_to(sub, subsol, sol);
		delete subsol;
		delete sub;
	}
	// lower right
	{
		subcity *sub = get_subcity(beginning, midx, xmax, ymin, midy);
		solution* subsol =  backtrack(sub->c, midx, xmax, ymin, midy, max_num_cities, improve, total_num_solved);
		append_to(sub, subsol, sol);
		delete subsol;
		delete sub;
	}
	
	
	draw_square(
		bigcity_513,
		xmin, xmax, ymin, ymax);
	improve(sol);
	
	if (sol->c->num_cities > total_num_solved)
	{
		total_num_solved = sol->c->num_cities;
		std::cout << "We have now solved a " <<  total_num_solved << " city." << std::endl;
	}
	
	
	
	
	return sol;
}


}


solution *subdivide_backtrack(city *beginning, void (improve)(solution* s), int max_num_cities)
{
	bigcity_513 = beginning;
	double xmin_513, xmax_513, ymin_513, ymax_513;
	get_bounds(beginning->locsx, beginning->locsy, beginning->num_cities,
		xmin_513, xmax_513, ymin_513, ymax_513);
	
	int total_num_solved = 0;
	
	return backtrack(beginning, xmin_513, xmax_513, ymin_513, ymax_513, max_num_cities, improve, total_num_solved);
}



























