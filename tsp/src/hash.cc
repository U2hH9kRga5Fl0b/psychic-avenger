
#include "hash.h"

int get_prime_less_than(int n)
{
	std::cout << "Finding prime less than " << n << "..." << std::endl;
	if (n <= 2)
	{
		return 2;
	}
	
	bool* isprime = new bool[n];
	
	for (int i=0;i<n;i++)
	{
		isprime[i] = true;
	}
	
	int p = 2;
	while (p < n)
	{
		for (int j=p*p; j>0 && j<n; j+=p)
		{
			isprime[j] = false;
		}
		
		p++;
		while (p < n && !isprime[p])
		{
			p++;
		}
	}
	
	int ret = -1;
	for (int i=n-1; i>=0; i--)
	{
		if (isprime[i])
		{
			ret = i;
			break;
		}
	}
	
	delete[] isprime;
	
	std::cout << " ...Done." << std::endl;
	
	return ret;
}

uint32_t get_mask(int n)
{
	// (1 << (n % 8))
	switch(n)
	{
		case 0: return 0x01;
		case 1: return 0x02;
		case 2: return 0x04;
		case 3: return 0x08;
		case 4: return 0x10;
		case 5: return 0x20;
		case 6: return 0x40;
		case 7: return 0x80;
		default:
			std::cout << "Bad index in to byte." << std::endl;
			trap();
	}
};

namespace
{
	int** default_hash_key;
	
	void init_hash_key(int num_stops)
	{
		default_hash_key = new int*[num_stops];
		for (int i=0; i<num_stops; i++)
		{
			ret[i] = new int[num_stops];
			for (int j=0; j<num_stops; j++)
			{
				ret[i][j] = rand();
			}
		}
	}
	
	void destroy_hash_key(int nstops)
	{
		for (int i=0; i<nstops; i++)
		{
			delete[] default_hash_key[i];
		}
		delete[] default_hash_key;
	}
}



hash get_hash(const solution* sol, int **hash_key)
{
	int n = sol->get_city()->num_stops;
	
	int sum = 0;
	for (int i=0; i<n; i++)
	{
		sum += hash_key[i][sol->get_stop(i)];
	}
	return abs(sum);
}

void set_component(int component, int value)
{
	for (int i=0; i<num_hashes; i++)
	{
		hashes[i] += get_hash_key()[component][value];
	}
}


hash::hash() : next{nullptr}
{
	for (int i=0;i<num_hashes;i++)
	{
		hashes[i] = 0;
	}
}	

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
	return std::abs(hashes[0]) % prime;
}
