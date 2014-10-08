#ifndef SOLUTION_H_
#define SOLUTION_H_

#include "city.h"

#include <iostream>
#include <algorithm>

class solution
{
public:
	solution(city* c);
	~solution();
	
	void random(std::function<void(void)> callback=[](){});
	void nearest(std::function<void(void)> callback=[](){});
	void nearest_additional(std::function<void(void)> callback=[](){});
	void empty();
	
	// could cache these...
	distance get_cost() const;
	bool is_valid();
	
	void insert_at(int stop, int idx);
	int remove_at(int idx);
	
	int get_num_serviced() const
	{
		return length();
	}
	bool has_serviced(int stop) const
	{
		return serviced_index[stop] >= 0;
	}
// Be careful when you use this method...
	void service(int index, int stop)
	{
//		int oldstop = path[index];
//		if (oldstop >= 0)
//		{
//			serviced_index[oldstop] = -1;
//		}
		if (stop >= 0)
		{
			serviced_index[stop] = index;
		}
		path[index] = stop;
	}
	int get_stop(int ndx) const
	{
		return path[ndx];
	}
	int get_index_of_stop(int stop) const
	{
		return serviced_index[stop];
	}
	city* get_city() const { return c; }
	int length() const
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
	
	solution& operator=(const solution& other);
	bool operator<(const solution& other) const;
	friend std::ostream& operator<<(std::ostream& out, const solution& sol);
private:
	void assign_serviced_indices();
	
	int *serviced_index;
	int *path;
	city *c;
};



#endif