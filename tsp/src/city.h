

#ifndef CITY_H_
#define CITY_H_

#include <inttypes.h>
#include <string>
#include <fstream>

typedef double distance;

typedef enum
{
	UNIFORM_CITY_TYPE,
	CIRCLE_CITY_TYPE,
	CLUSTER_CITY_TYPE,
	RADIAL_CITY_TYPE,
	SPIRAL_CITY_TYPE,
} city_type;


void generate_points(city_type t, int n, double *x, double *y);

class city
{
public:
	city(city_type t, int nstops);
	city(int nstops, double *x, double *y);
	~city();
	
	void plot(const std::string& file) const;
	distance get_cost(int i, int j) const { return distances[i * num_stops + j]; }
	
	int num_stops;
	distance *distances;
//	int num_drivers;
	double *locsx;
	double *locsy;
	
	friend std::ostream& operator<<(std::ostream& out, const city& c);
private:
	void refresh_distances();
};


city* load_city(std::istream& in);

#endif