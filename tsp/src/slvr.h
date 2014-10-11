#ifndef SOLVER_H_
#define SOLVER_H_

#include "viewer.h"

class optimizer
{
public:
	optimizer(const std::string& name, city *c, int freq);
	~optimizer();
	
	bool offer();
	
	solution best;
	solution current;
	
	optimizer& operator=(const optimizer& other) = delete;
private:
	
	viewer vbest;
	viewer vcurrent;
};


#endif