#include "solution.h"

#include "common.h"

#include <iomanip>

solution::solution(city *c_) :
	path{new int[c_->num_cities]},
	already_serviced{new bool[c_->num_cities]},
	c{c_}
{
	int n = c_->num_cities;
	for (int i=0;i<n;i++)
	{
		already_serviced[i] = 0;
		path[i] = -1;
	}
}

solution::~solution()
{
	delete[] path;
	delete[] already_serviced;
}


distance solution::get_cost() const
{
	if (path[0] < 0)
	{
		return 0;
	}
	
	
	distance cost = 0;
	
	int n = c->num_cities;
	
	for (int i=0;i<n-1; i++)
	{
		int next = path[i+1];
		if (next < 0)
		{
			break;
		}
		
		cost += c->get_cost(path[i], next);
	}
	
	return cost;
}

int solution::get_num_serviced() const
{
	int n = c->num_cities;
	for (int i=0;i<n;i++)
	{
		if (path[i] < 0)
		{
			return i;
		}
	}
	return n;
}

void solution::plot(const std::string& filename) const
{
	for (int i=0; i<c->num_cities; i++)
	{
		std::cout << path[i] /*<< ": " << locsx[i] << ", " << locsy[i] */ << std::endl;
	}
}


solution& solution::operator=(const solution& other)
{
	int ncities = c->num_cities;
	if (ncities != other.c->num_cities)
	{
		std::cout << "assignment to wrong city size." << std::endl;
		exit(-1);
	}
	
	c = other.c;
	
	for (int i=0;i<ncities;i++)
	{
		path[i] = other.path[i];
		already_serviced[i] = other.already_serviced[i];
	}
}


bool solution::operator<(const solution& other) const
{
	int delta = get_num_serviced() - other.get_num_serviced();
	
	if (delta == 0)
	{
		return get_cost() < other.get_cost();
	}
	
	return delta > 0;
}


void solution::random()
{
	int n = c->num_cities;
	for (int i=0;i<n;i++)
	{
		already_serviced[i] = 0;
	}
	for (int i=0;i<n;i++)
	{
		int ndx;
		do
		{
			ndx = rand() % n;
		} while (already_serviced[ndx]);
		
		path[i] = ndx;
		already_serviced[ndx] = 1;
	}
}



std::ostream& operator<<(std::ostream& out, const solution& sol)
{
	int n = sol.c->num_cities;
	
	out << "[c=" << std::setw(12) << sol.get_cost() << "][";
	for (int i=0; i<n; i++)
	{
		out << std::setw(3) << sol.path[i] << " ";
	}
	out << std::endl;
	for (int i=0; i<n; i++)
	{
		out << std::setw(3) << i << ":"<< std::setw(3) << sol.already_serviced[i] << " ";
	}
	
	return out << "]";
}


#if 0

	
	
	solution s{&c};
	std::cout << s << std::endl;
	s.insert_at(0,0);
	std::cout << s << std::endl;
	s.insert_at(1,1);
	std::cout << s << std::endl;
	s.insert_at(2,2);
	std::cout << s << std::endl;
	s.insert_at(3,3);
	std::cout << s << std::endl;
	s.insert_at(4,2);
	std::cout << s << std::endl;
	s.insert_at(5,0);
	std::cout << s << std::endl;
	s.insert_at(6,6);
	std::cout << s << std::endl;
	s.remove_at(6);
	std::cout << s << std::endl;
	s.remove_at(0);
	std::cout << s << std::endl;
	s.remove_at(2);
	std::cout << s << std::endl;
#endif

void solution::insert_at(int stop, int idx)
{
	int n = c->num_cities;
	already_serviced[stop] = 1;
	
	int prev = path[idx];
	path[idx] = stop;
	for (int i=idx + 1; i < n; i++)
	{
		if (prev < 0)
		{
			break;
		}
		int next = path[i];
		path[i] = prev;
		prev = next;
	}
}


int solution::remove_at(int idx)
{
	int n = c->num_cities;
	int stop = path[idx];
	already_serviced[stop] = false;
	for (int i=idx; i < (n-1); i++)
	{
		path[i] = path[i+1];
	}
	return stop;
}

bool solution::is_valid()
{
	int n = c->num_cities;
	
	int count_alread = 0;
	int count_path = 0;
	
	for (int i=0;i<n;i++)
	{
		for (int j=0;j<n;j++)
		{
			if (path[i] < 0 && path[j] >= 0 && i < j)
			{
				std::cout << "positive after -1" << std::endl;
				trap();
			}
			
			if (path[i] == path[j] && path[i] >= 0 && i != j)
			{
				std::cout << "duplicate!!" << std::endl;
				trap();
			}
		}
		
		if (path[i] >= 0 && !already_serviced[path[i]])
		{
			std::cout << "already_serviced doesn't know about " << path[i] << " at index " << i << std::endl;
			trap();
		}
		if (already_serviced[i])
		{
			count_alread++;
		}
		if (path[i] >= 0)
		{
			count_path++;
		}
		if (path[i] >= n)
		{
			std::cout << "path goes to invalid stop!" << std::endl;
			trap();
		}
	}
	if (count_alread != count_path)
	{
		std::cout << "mismatch" << std::endl;
		trap();
	}
}



int solution::get_index_of_stop(int stop) const
{
	std::cout << "Implement me!" << std::endl;
	exit(-1);
}