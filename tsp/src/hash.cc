
#include "hash.h"
#include <iomanip>

int** default_hash_key;

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

void init_hash_key(int num_stops)
{
	default_hash_key = new int*[num_stops];
	for (int i=0; i<num_stops; i++)
	{
		default_hash_key[i] = new int[num_stops];
		for (int j=0; j<num_stops; j++)
		{
			default_hash_key[i][j] = rand();
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

hash get_hash(const solution* sol)
{
	int n = sol->get_city()->num_stops;
	
	hash h;
	
	for (int i=0; i<n; i++)
	{
		h.set_component(i, sol->get_stop(i));
	}
	
	return h;
}

hash get_hash_unambig(const solution* sol) { return get_hash(sol); }

int hash::get_value(int prime) const
{
	return std::abs(hashes[0]) % prime;
}

hash::hash() : next{nullptr}
{
	for (int i=0; i<num_hashes; i++)
	{
		hashes[i] = 0;
	}
}

hash::~hash()
{
	
}

bool hash::operator<(const hash& other) const
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

bool hash::operator==(const hash& other) const
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

hash& hash::operator=(const hash& other)
{
	for (int i=0; i<num_hashes; i++)
	{
		hashes[i] = other.hashes[i];
	}
	return (*this);
}



std::ostream& operator<<(std::ostream& out, const hash& other)
{
	out << std::dec;
	
	for (int i=0; i<num_hashes; i++)
	{
		out << 'h' << i << '=' << std::setw(5) << other.hashes[i] << ' ';
	}
	return out << std::endl;
}