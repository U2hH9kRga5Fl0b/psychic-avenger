#include <stdlib.h>

#include "city.h"
#include "common.h"

#include <cmath>
#include <iostream>
#include <cfloat>


std::ostream& operator<<(std::ostream& out, const city& c)
{
	out << c.num_cities << std::endl;
	for (int i=0;i<c.num_cities;i++)
	{
		out << c.locsx[i] << "\t" << c.locsy[i] << std::endl;
		
	}
	return out;
}
city* load_city(std::istream& in)
{
	int size = -1;
	in >> size;
	if (size < 0)
	{
		std::cout << "Bad city file: need to have a non-negative number of stops." << std::endl;
		trap();
	}
	
	double* locxs = new double[size];
	double* locys = new double[size];
	
	for (int i=0;i<size;i++)
	{
		if (!in)
		{
			std::cout << "Hit end of city file prematurely (x): " << i << std::endl;
			trap();
		}
		in >> locxs[i];
		if (!in)
		{
			std::cout << "Hit end of city file prematurely (y): " << i << std::endl;
			trap();
		}
		in >> locys[i];
	}
	
	return new city{size, locxs, locys};
}

void generate_points(city_type t, int n, double *x, double *y)
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


void city::refresh_distances()
{
	for (int i=0; i<num_cities; i++)
	{
		for (int j=0; j<num_cities; j++)
		{
			double dx = locsx[i] - locsx[j];
			double dy = locsy[i] - locsy[j];
			distances[i * num_cities + j] = sqrt(dx * dx + dy * dy);
		}
	}
}

city::city(int nstops_, double *x, double *y) :
	num_cities{nstops_},
	distances{new distance[nstops_ * nstops_]},
	locsx{x},
	locsy{y}
{
	refresh_distances();
}

city::city(city_type t, int nstops_) :
	num_cities{nstops_},
	distances{new distance[nstops_ * nstops_]},
	locsx{new double[nstops_]},
	locsy{new double[nstops_]}
{
	generate_points(t, nstops_, locsx, locsy);
	refresh_distances();
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
