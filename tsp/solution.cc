#include "solution.h"

#include "common.h"

#include <float.h>

#include <iomanip>

solution::solution(city *c_) :
	path{new int[c_->num_cities]},
	serviced_index{new int[c_->num_cities]},
	c{c_}
{
	empty();
}

solution::~solution()
{
	delete[] path;
	delete[] serviced_index;
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
		serviced_index[i] = other.serviced_index[i];
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

void solution::assign_serviced_indices()
{
	int n = c->num_cities;
	for (int i=0;i<n;i++)
	{
		serviced_index[i] = -1;
	}
	for (int i=0;i<n;i++)
	{
		int stop = path[i];
		if (path[i] >= 0)
		{
			serviced_index[path[i]] = i;
		}
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
		out << "[" << std::setw(3) << i << ":"<< std::setw(3) << sol.serviced_index[i] << "]";
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
	
	assign_serviced_indices();
}


int solution::remove_at(int idx)
{
	int n = c->num_cities;
	int stop = path[idx];
	
	for (int i=idx; i < (n-1); i++)
	{
		path[i] = path[i+1];
	}
	return stop;
	
	assign_serviced_indices();
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
				std::cout << *this << std::endl;
				std::cout << "positive after -1" << std::endl;
				trap();
			}
			
			if (path[i] == path[j] && path[i] >= 0 && i != j)
			{
				std::cout << *this << std::endl;
				std::cout << "duplicate!!" << std::endl;
				trap();
			}
		}
		
		if (path[i] >= 0 && serviced_index[path[i]] < 0)
		{
			std::cout << *this << std::endl;
			std::cout << "serviced_index doesn't know about " << path[i] << " at index " << i << std::endl;
			trap();
		}
		if (serviced_index[i] >= 0)
		{
			count_alread++;
		}
		if (path[i] >= 0)
		{
			count_path++;
		}
		if (path[i] >= n)
		{
			std::cout << *this << std::endl;
			std::cout << "path goes to invalid stop!" << std::endl;
			trap();
		}
		if (serviced_index[i] >= 0 && path[serviced_index[i]] != i)
		{
			std::cout << *this << std::endl;
			std::cout << "already serviced index is inaccurate." << std::endl;
			trap();
		}
	}
	if (count_alread != count_path)
	{
		std::cout << *this << std::endl;
		std::cout << "mismatch" << std::endl;
		trap();
	}
}


void solution::empty()
{
	int n = c->num_cities;
	for (int i=0;i<n;i++)
	{
		serviced_index[i] = path[i] = -1;
	}
}


void solution::random(std::function<void(void)> callback)
{
	empty();
	int n = c->num_cities;
	
	int *order = new int[n];
	
	for (int i=0; i<n; i++)
	{
		order[i] = i;
	}
	for (int i=0; i<2*n; i++)
	{
		int ndx1 = rand() % n;
		int ndx2 = rand() % n;
		
		int tmp = order[ndx1];
		order[ndx1] = order[ndx2];
		order[ndx2] = tmp;
	}
	
	for (int i=0; i<n; i++)
	{
		service(i, order[i]);
		callback();
	}
	
	delete[] order;
	
//	assign_serviced_indices();
}

void solution::nearest(std::function<void(void)> callback)
{
	empty();
	int n = c->num_cities;
	
	int* remaining = new int[n];
	for (int i=0;i<n;i++)
	{
		remaining[i] = i;
	}
	
	int prev = rand() % n; // first one chosen at random...
	service(0, prev);
	
	{
		int tmp = remaining[0];
		remaining[0] = remaining[prev];
		remaining[prev] = tmp;
	}
	
	for (int i=1;i<n;i++)
	{
		int idxmin = i;
		double min = c->get_cost(prev, remaining[idxmin]);
		
		for (int j=i+1; j<n; j++)
		{
			double d = c->get_cost(prev, remaining[j]);
			if (d < min)
			{
				idxmin = j;
				min = d;
			}
		}
		
		{
			int tmp = remaining[i];
			remaining[i] = remaining[idxmin];
			remaining[idxmin] = tmp;
		}
		
		service(i, idxmin);
		
		prev = remaining[idxmin];
		
		callback();
		is_valid();
	}
	
	delete[] remaining;
}
