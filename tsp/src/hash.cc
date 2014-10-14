
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

bloom_filter::bloom_filter(int nbits_) :
	nbits{0},
	nbytes{0},
	mem{nullptr}
{
	set_size(nbits_);
}

bloom_filter::~bloom_filter() { delete mem; }

bool bloom_filter::might_have(const hash& h) const
{
	return get_bit_at_index(h.get_value(nbits));
}
void bloom_filter::add(const hash& h)
{
	return set_bit_at_index(h.get_value(nbits));
}
bool bloom_filter::already_visited(const solution* sol, bool save)
{
	hash h{sol};
	if (might_have(h))
	{
		return true;
	}
	else
	{
		if (save)
		{
			std::cout << " checking if visited. before: \n" << (*this) <<std::endl;
			add(h);
			std::cout << " after:\n" << (*this) << std::endl;
		}
		return false;
	}
}
double bloom_filter::get_saturation() const
{
	int sum = 0;

#if 1
	constexpr int ntsts = 50;
	for (int i=0; i<ntsts; i++)
	{
		if (get_bit_at_index(rand() % nbits))
		{
			sum++;
		}
	}
	return sum / (double) ntsts;
#else
	for (int i=0;i<nbits;i++)
	{
		if (get_bit_at_index(i))
		{
			sum++;
		}
	}
	return sum / (double) nbits;
#endif
}



std::ostream& operator<<(std::ostream& out, const bloom_filter& filter)
{
	for (int i=0; i<filter.nbytes; i++)
	{
		out << std::hex << (0xff & filter.mem[i]);
	}
	return out << std::endl;
}

void bloom_filter::erase_memory()
{
	for (int i=0; i<nbytes; i++)
	{
		mem[i] = 0;
	}
}

bool bloom_filter::get_bit_at_index(int n) const
{
	if (n < 0 || n > nbits)
	{
		std::cout << " Bad index into bloom filter: " << n << std::endl;
		trap();
	}
	return mem[n / 8] & get_mask(n % 8);
}

void bloom_filter::set_bit_at_index(int n)
{
	if (n < 0 || n > nbits)
	{
		std::cout << " Bad index into bloom filter: " << n << std::endl;
		trap();
	}
	mem[n / 8] |= get_mask(n % 8);
}

void bloom_filter::set_size(int nbits_)
{
	nbits = get_prime_less_than(nbits_);
	nbytes = (nbits + 2 * 8) / 8;
	delete[] mem;
	mem = new uint8_t[nbytes];
	erase_memory();
}

circular_hash_set::circular_hash_set(int size_) :
		size{size_} {}
circular_hash_set::circular_hash_set() :
		circular_hash_set {100}
		{}
circular_hash_set::~circular_hash_set() {}
	
int circular_hash_set::get_size() const
{
	return size;
}

void circular_hash_set::set_size(int size_)
{
	if (size_ < hashes.size())
	{
		// overkill
		hashes.clear();
		first = nullptr;
		last = nullptr;
	}
	size = size_;
}
bool circular_hash_set::already_visited(const solution* sol, bool save)
{
	hash h{sol};
	if (contains(h))
	{
		return true;
	}
	else
	{
		if (save)
		{
			add(h);
		}
		return false;
	}
}
bool circular_hash_set::contains(const hash& h) const
{
	return hashes.find(h) != hashes.end();
}

void circular_hash_set::add(hash& h)
{
	auto pair = hashes.insert(h);
	hash* ptr = (hash*) &(*pair.first);
	
	if (first == nullptr)
	{
		first = ptr;
		last = ptr;
	}
	else if (last != nullptr)
	{
		last->next = ptr;
		last = ptr;
	}
	
	if (hashes.size() > size)
	{
		auto it = hashes.find(*first);
		first = it->next;
		hashes.erase(it);
	}
}


size_t bloom_filter::get_approx_memory_usage() const
{
	return nbytes;
}

size_t circular_hash_set::get_approx_memory_usage() const
{
	return sizeof(hash) * hashes.size();
}

void circular_hash_set::erase_memory()
{
	hashes.clear();
	first = last = nullptr;
}
