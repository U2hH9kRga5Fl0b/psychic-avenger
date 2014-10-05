#include <stdlib.h>

#include "city.h"

#include <cmath>
#include <iostream>
#include <cfloat>

namespace
{

void get_random_cities(city_type t, double *x, double *y, int n)
{
	const double PI = std::atan(1.0)*4;
	switch(t)
	{
		case UNIFORM_CITY_TYPE:
		{
			for (int i=0;i<n;i++)
			{
				x[i] = rand() / (double) RAND_MAX;
				y[i] = rand() / (double) RAND_MAX;
				
				std::cout << x[i] << ", " << y[i] << std::endl;
			}
		}
		break;
		case CIRCLE_CITY_TYPE:
		{
			for (int i=0;i<n;i++)
			{
				double radius = .9 + .2 * (rand() / (double) RAND_MAX);
				double theta = 2 * PI * (rand() / (double) RAND_MAX);
				
				x[i] = radius * std::cos(theta);
				y[i] = radius * std::sin(theta);
				
				std::cout << x[i] << ", " << y[i] << std::endl;
			}
		}
		break;
		case CLUSTER_CITY_TYPE:
		{
			int num_clusters = 3 + rand() % 5;
			
			double *myxs = new double[num_clusters];
			double *myys = new double[num_clusters];
			for (int i=0;i<num_clusters;i++)
			{
				myxs[i] = 10 * (rand() / (double) RAND_MAX);
				myys[i] = 10 * (rand() / (double) RAND_MAX);
			}
			
			for (int i=0;i<n;i++)
			{
				double radius = rand() / (double) RAND_MAX;
				double theta = 2 * PI * (rand() / (double) RAND_MAX);
				
				x[i] = radius * std::cos(theta);
				y[i] = radius * std::sin(theta);
				
				int ndx = rand() % num_clusters;
				x[i] += myxs[ndx];
				y[i] += myys[ndx];
				
				std::cout << x[i] << ", " << y[i] << std::endl;
			}
			
			delete[] myxs;
			delete[] myys;
		}
		break;
	}
	std::cout << "pi=" << PI << std::endl;
}

}


city::city(city_type t, int nstops_) :
	num_cities{nstops_},
	distances{new distance[nstops_ * nstops_]},
	locsx{new double[nstops_]},
	locsy{new double[nstops_]}
{
	get_random_cities(t, locsx, locsy, nstops_);
	
	for (int i=0; i<nstops_; i++)
	{
		for (int j=0; j<nstops_; j++)
		{
			double dx = locsx[i] - locsx[j];
			double dy = locsy[i] - locsy[j];
			distances[i * nstops_ + j] = sqrt(dx * dx + dy * dy);
		}
	}
}

city::~city()
{
	delete[] distances;
	delete[] locsx;
	delete[] locsy;
}


void city::plot (const std::string& filename) const
{
}
