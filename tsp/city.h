

#ifndef CITY_H_
#define CITY_H_

#include <inttypes.h>
#include <string>

typedef double distance;

typedef enum
{
	UNIFORM_CITY_TYPE,
	CIRCLE_CITY_TYPE,
	CLUSTER_CITY_TYPE,
} city_type;


class city
{
public:
	city(city_type t, int nstops);
	~city();
	
	void plot(const std::string& file) const;
	distance get_cost(int i, int j) const { return distances[i * num_cities + j]; }
	
	int num_cities;
	distance *distances;
//	int num_drivers;
	double *locsx;
	double *locsy;
};


#endif