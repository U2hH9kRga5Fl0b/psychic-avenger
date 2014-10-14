
#ifndef HASH_H_
#define HASH_H_

#include "solution.h"
#include "common.h"

#include <set>

#define num_hashes 1
constexpr int primes[] = {5003, 34303, 28631, 1483, 1063, 113, 3677, 7537, 10067, 13859 };

class hash
{
public:
	hash(const solution* so) :
		next{nullptr}
	{
		int n = so->get_city()->num_cities;
		for (int i=0;i<num_hashes;i++)
		{
			hashes[i] = primes[i];
		}
		
		for (int i=0; i<n; i++)
		{
			for (int j=0; j<num_hashes; j++)
			{
				hashes[j] += so->get_stop(i) * primes[j];
			}
		}
	}
	
	~hash() {}
	
	bool operator<(const hash& other) const
	{
		for (int i=0; i<num_hashes; i++)
		{
			if (hashes[i] != other.hashes[i])
			{
				return hashes[i] < other.hashes[i];
			}
		}
		return false;
	}
	bool operator==(const hash& other) const
	{
		for (int i=0; i<num_hashes; i++)
		{
			if (hashes[i] != other.hashes[i])
			{
				return false;
			}
		}
		return true;
	}
	hash& operator=(const hash& other)
	{
		for (int i=0; i<num_hashes; i++)
		{
			hashes[i] = other.hashes[i];
		}
	}
	int get_value(int prime) const
	{
		int val = hashes[0];
		if (val < 0)
		{
			return (-val) % prime;
		}
		else
		{
			return val % prime;
		}
	}
	
	hash* next;
private:
	int hashes[num_hashes];
};

int get_prime_less_than(int n);

class bloom_filter
{
public:
	bloom_filter(int nbits);
	~bloom_filter();
	
	void set_size(int nbits);
	int get_size() const { return nbits; };
	
	// wolverine style...
	void erase_memory();
	
	bool already_visited(const solution* sol, bool save);
	double get_saturation() const;
	size_t get_approx_memory_usage() const;
	
	friend std::ostream& operator<<(std::ostream& out, const bloom_filter& filter);
private:
	bool might_have(const hash& h) const;
	void add(const hash& h);
	
	bool get_bit_at_index(int n) const;
	void set_bit_at_index(int n);
	
	int nbits;
	int nbytes;
	uint8_t *mem;
};


class circular_hash_set
{
public:
	circular_hash_set();
	circular_hash_set(int size);
	~circular_hash_set();
	
	void erase_memory();
	
	int get_size() const;
	void set_size(int size);
	size_t get_approx_memory_usage() const;
	
	bool already_visited(const solution* sol, bool save);
private:
	bool contains(const hash& h) const;
	void add(hash& h);
	
	hash* first;
	hash* last;
	
	int size;
	std::set<hash> hashes;
};


#endif