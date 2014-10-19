#ifndef SOLUTION_H_
#define SOLUTION_H_

#include "city.h"

#include <iostream>
#include <algorithm>

class solution
{
public:
	solution(city* c);
	solution(const solution& other);
	~solution();
	
	void random(std::function<void(void)> callback=[](){});
	void nearest(std::function<void(void)> callback=[](){});
	void nearest_additional(std::function<void(void)> callback=[](){});
	void empty();
	
	// could cache this...
	distance get_cost() const;
	bool is_valid();
	
	void insert_at_ndx(int stop, int idx);
	int remove_at_ndx(int idx);
	
	int get_num_serviced() const
	{
		return length();
	}
	bool has_serviced(int stop) const
	{
		return serviced_index[stop] >= 0;
	}

//	Be careful when you use this method...
	void set_path_at(int index, int stop)
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

	inline void swap(int idx1, int idx2)
	{
		int s1 = path[idx1];
		int s2 = path[idx2];

		serviced_index[s1] = idx2;
		serviced_index[s2] = idx1;

		path[idx1] = s2;
		path[idx2] = s1;
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
		int n = c->num_stops;
		for (int i=0; i<n; i++)
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
	
	void assign_serviced_indices();
private:
	int *path;
	int *serviced_index;
	city *c;
};



#endif
