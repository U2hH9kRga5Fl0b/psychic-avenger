
#ifndef HASH_H_
#define HASH_H_

#include "solution.h"
#include "common.h"
#include <iomanip>

#include <set>

#define num_hashes 1

extern int** default_hash_key;

int get_prime_less_than(int n);

class hash
{
public:
	hash();
	~hash();
	
	void inline set_component(int component, int value)
	{
		for (int i=0; i<num_hashes; i++)
		{
//	{
//		std::ofstream out{"hash_log.txt", std::ofstream::app};
//		out << " + keys[" << i << "][" << value << "] = " << hashes[i] << " we had added " << default_hash_key[i][value] << std::endl;
//	}
//			std::cout << std::setw(4) << value << ' ';
			hashes[i] += default_hash_key[component][value];
		}
	}
	int get_value(int prime) const;
	
	bool operator<(const hash& other) const;
	bool operator==(const hash& other) const;
	hash& operator=(const hash& other);
	friend std::ostream& operator<<(std::ostream& out, const hash& other);
	
	hash* next;
private:
	int hashes[num_hashes];
};


hash get_hash(const solution* sol);
hash get_hash_unambig(const solution* sol);

void init_hash_key(int nstops);
void destroy_hash_key(int nstops);


#endif