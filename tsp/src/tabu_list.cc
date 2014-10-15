
bloom_filter::bloom_filter(int nbits_) :
	nbits{0},
	nbytes{0},
	mem{nullptr}
{
	set_size(nbits_);
}

bloom_filter::~bloom_filter() { delete mem; }

bool bloom_filter::is_tabu(const hash& sol)
{
	return stopped(get_bit_at_index(h.get_value(nbits))));
}

void bloom_filter::mark_tabu(const hash& sol)
{
	std::cout << " checking if visited. before: \n" << (*this) <<std::endl;
	set_bit_at_index(sol.get_value(nbits));
	
	if (!(rand() % nbytes))
	{
		if (get_saturation() > .5)
		{
			erase_memory;
		}
	}
	
	std::cout << " after:\n" << (*this) << std::endl;
}
double bloom_filter::get_saturation() const
{
	int sum = 0;
	for (int i=0; i<50; i++)
	{
		if (get_bit_at_index(rand() % nbits))
		{
			sum++;
		}
	}
	return sum / (double) ntsts;
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




circular_hash_set::circular_hash_set(int size_) :
		size{size_} {}
circular_hash_set::~circular_hash_set() {}

bool circular_hash_set::is_tabu(const hash& sol)
{
	return stopped(hashs.find(h) != hashes.end());
}

void circular_hash_set::mark_tabu(hash& h)
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
