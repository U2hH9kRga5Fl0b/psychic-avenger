
#ifndef HASH_H_
#define HASH_H_

#include "solution.h"
#include "common.h"

#include <set>

#define num_hashes 1

class hash
{
public:
	hash();
	~hash();
	
	void set_component(int component, int value);
	int get_value(int prime) const;
	
	bool operator<(const hash& other) const;
	bool operator==(const hash& other) const;
	hash& operator=(const hash& other);
	
	hash* next;
private:
	int hashes[num_hashes];
};


int get_prime_less_than(int n);


#endif