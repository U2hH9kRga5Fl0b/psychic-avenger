
#include "tabu_list.h"

bloom_filter::bloom_filter(int nbits_) :
	nbits{get_prime_less_than(nbits_)},
	nbytes{nbits / 8 + 4},
	mem{new uint8_t[nbytes]}
{
	erase_memory();
}

bloom_filter::~bloom_filter() { delete mem; }

bool bloom_filter::is_tabu(const hash& h)
{
	return stopped
	(
		get_bit_at_index
		(
			h.get_value(nbits)
		)
	);
}

void bloom_filter::mark_tabu(const hash& sol)
{
//	std::cout << " checking if visited. before: \n" << (*this) <<std::endl;
//	std::cout << std::dec << " index = " << sol.get_value(nbits) << std::endl;
	
	set_bit_at_index(sol.get_value(nbits));
	
	// just every so often...
	if (!(rand() % nbytes))
	{
		// check the saturation lvls
		if (get_saturation() > .8)
		{
			// and forget everything if we are saturated
			erase_memory();
		}
	}
	
//	std::cout << " after:\n" << (*this) << std::endl;
}
double bloom_filter::get_saturation() const
{
	constexpr int ntsts = 50;
	
	int sum = 0;
	for (int i=0; i<ntsts; i++)
	{
		if (get_bit_at_index(rand() % nbits))
		{
			sum++;
		}
	}
	return sum / (double) ntsts;
}

void bloom_filter::erase_memory()
{
	std::cout << "Erasing memory..." << std::endl;
	for (int i=0; i<nbytes; i++)
	{
		mem[i] = 0;
	}
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
}
bool bloom_filter::get_bit_at_index(int n) const
{
//	std::cout << "getting bit at " << std::dec << n << std::endl;
	
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
	mem[n / 8] = mem[n / 8] | get_mask(n % 8);
}




std::ostream& operator<<(std::ostream& out, const bloom_filter& filter)
{
#if 1
	out << std::hex;
	for (int i=0; i<filter.nbytes; i++)
	{
		out << (0xff & filter.mem[i]);
	}
#else
	for (int i=0; i<filter.nbits; i++)
	{
		out << filter.get_bit_at_index(i) ? '1' : '0';
	}
#endif
	return out << std::endl;
}















circular_hash_set::circular_hash_set(int size_) :
		size{size_} {}
circular_hash_set::~circular_hash_set() {}

bool circular_hash_set::is_tabu(const hash& h)
{
	return stopped(hashes.find(h) != hashes.end());
}

void circular_hash_set::mark_tabu(const hash& h)
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
	
	int hsi = hashes.size();
	if (hsi > size)
	{
		auto it = hashes.find(*first);
		first = it->next;
		hashes.erase(it);
	}
}
