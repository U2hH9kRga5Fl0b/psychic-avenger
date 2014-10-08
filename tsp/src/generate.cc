#include "viewer.h"
#include "common.h"


#include <iostream>
#include <fstream>

void trap(const std::string& msg)
{
	std::cout << "on the way out!" << std::endl;
	exit(-1);
}

void print_usage(int argc, char **argv)
{
	std::cout << "usage: " << argv[0] << " (u|k|c) <num_cities> <outfile>" << std::endl;
	trap();
}

int main(int argc, char **argv)
{
	srand(time(nullptr));
	
	if (argc != 4)
	{
		print_usage(argc, argv);
	}
	
	std::string arg1{argv[1]};
	if (arg1.size() != 1)
	{
		print_usage(argc, argv);
	}
	
	int num_stops = -1;
	num_stops = atoi(argv[2]);
	if (num_stops < 0)
	{
		print_usage(argc, argv);
	}
	
	city* c = nullptr;
	switch(arg1.at(0))
	{
		case 'u':
			c = new city{UNIFORM_CITY_TYPE, num_stops};
			break;
		case 'k':
			c = new city{CLUSTER_CITY_TYPE, num_stops};
			break;
		case 'c':
			c = new city{CIRCLE_CITY_TYPE, num_stops};
			break;
		default:
			print_usage(argc, argv);
	}
	
	{
		std::ofstream outfile{std::string{argv[3]}};
		if (!outfile.is_open())
		{
			std::cout << "Unable to open output file!!" << std::endl;
			print_usage(argc, argv);
		}
		outfile << (*c);
	}
	
	view_city(c, "city");
	
	return 0;
}