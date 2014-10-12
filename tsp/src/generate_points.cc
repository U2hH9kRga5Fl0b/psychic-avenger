
#include "city.h"
#include <random>
#include <iostream>
#include <iomanip>

void generate_points(city_type t, int n, double *x, double *y)
{
	double cone_factor = .5;
	double cone_start = 10 * rand() / (double) RAND_MAX;
	
	const double PI = std::atan(1.0)*4;
	switch(t)
	{
		case UNIFORM_CITY_TYPE:
		{
			for (int i=0;i<n;i++)
			{
				x[i] = rand() / (double) RAND_MAX;
				y[i] = rand() / (double) RAND_MAX;
			}
		}
		break;
		case CIRCLE_CITY_TYPE:
		{
			for (int i=0;i<n;i++)
			{
				double radius = .9 + .2 * (rand() / (double) RAND_MAX);
				double theta = cone_start + 2 * PI * cone_factor * (rand() / (double) RAND_MAX);
				
				x[i] = radius * std::cos(theta);
				y[i] = radius * std::sin(theta);
			}
		}
		break;
		case CLUSTER_CITY_TYPE:
		{
			int num_clusters = 3 + rand() % 5;
			std::cout << "Generating " << num_clusters << " clusters." << std::endl;
			
			double *myxs = new double[num_clusters];
			double *myys = new double[num_clusters];
			for (int i=0;i<num_clusters;i++)
			{
				myxs[i] = 15 * (rand() / (double) RAND_MAX);
				myys[i] = 15 * (rand() / (double) RAND_MAX);
				
				std::cout << "cluster " << i << " is at (" << std::setw(12) << myxs[i] << ", " << std::setw(12) << myys[i] << ")" << std::endl;
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
			}
			
			delete[] myxs;
			delete[] myys;
		}
		break;
		case RADIAL_CITY_TYPE:
		{
			std::default_random_engine generator;
			std::normal_distribution<double> distribution(0.0, 1.0);
			
			for (int i=0;i<n;i++)
			{
				double radius = distribution(generator);
				double theta = cone_start + 2 * PI * cone_factor * (rand() / (double) RAND_MAX);
				
				x[i] = radius * std::cos(theta);
				y[i] = radius * std::sin(theta);
			}
		}
		break;
		case SPIRAL_CITY_TYPE:
		{
			for (int i=0;i<n;i++)
			{
				double t = 50 * (rand() / (double) (RAND_MAX));
				double out = .3;
				
				x[i] = t * std::cos(t / out);
				y[i] = t * std::sin(t / out);
			}
		}
		break;
	}
}
