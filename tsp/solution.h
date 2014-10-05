#ifndef SOLUTION_H_
#define SOLUTION_H_

#include "city.h"

#include <iostream>

class solution
{
public:
	solution(city* c);
	~solution();
	
	void random();
	
	distance get_cost() const;
	int get_num_serviced() const;
	
	
	void insert_at(int stop, int idx);
	int remove_at(int idx);
	
	
	
	bool has_serviced(int stop) const { return already_serviced[stop]; };
	
	void plot(const std::string& filename) const;
	
	solution& operator=(const solution& other);
	bool operator<(const solution& other) const;
	friend std::ostream& operator<<(std::ostream& out, const solution& sol);
	
	bool is_valid();
	
	int *path;
	bool *already_serviced;
	city *c;
};



#endif