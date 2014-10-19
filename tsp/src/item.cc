#include "item.h"

#include "geometry.h"
#include "common.h"

#include <algorithm>
#include <list>
#include <sstream>


#include <float.h>


itemizer::itemizer(city* c)
{
	std::list<neighbor_option*> local_options;
	int n = c->num_stops;
	
	for (int i=0; i<n; i++)
	{
		local_options.push_back(new resched_option{i, -1, c});

		for(int j=0; j<n; j++)
		{
			if (i==j)
			{
				continue;
			}
			local_options.push_back(new resched_option{i, j, c});
			
			if (j < i)
			{
				continue;
			}
			local_options.push_back(new two_opt{i, j, c});
//











			local_options.push_back(new swap_option{i, j, c});
		}
	}
	
	options = new neighbor_option*[num_options = local_options.size()];
	
	int ndx = 0;
	auto end = local_options.end();
	for (auto it = local_options.begin(); it != end; ++it)
	{
		options[ndx++] = (*it);
	}
}

itemizer::~itemizer()
{
	for (int i=0; i<num_options; i++)
	{
		delete options[i];
	}
	delete[] options;
}

const neighbor_option* itemizer::get_best_option(const solution* sol)
{
	return get_nth_best_option(sol, 0);
}

const neighbor_option* itemizer::get_nth_best_option(const solution* sol, int n, bool hurt)
{
	n = std::max(1, n);
	
	std::vector<const neighbor_option*> better;
	
	if (num_options < n)
	{
		std::cout << "Only " << num_options << " options, can't get the " << n << "-th best!" << std::endl;
		trap();
	}
	
	double* cs = new double[n];
	const neighbor_option** b = new const neighbor_option*[n];
	
	for (int i=0;i<n;i++)
	{
		b[i] = nullptr;
	}
	
	for (int i=0; i<num_options; i++)
	{
		const neighbor_option* opt = options[i];
		if (!opt->in_bounds(sol))
		{
			continue;
		}
		
		double imp = opt->get_improvement(sol);
		if (hurt && imp < 0.0)
		{
			continue;
		}
		
		if (b[0] != nullptr && cs[0] < imp)
		{
			continue;
		}
		
		
		for (int j=0; ; j++)
		{
			if (j == n-1)
			{
				cs[j] = imp;
				b[j] = opt;
				break;
			}
			
			if (b[j+1] == nullptr || cs[j+1] > imp)
			{
				cs[j] = cs[j+1];
				b[j] = b[j+1];
				continue;
			}
			
			cs[j] = imp;
			b[j] = opt;
		}
	}
	
	const neighbor_option* ret = nullptr;
	for (int i=0; i<n; i++)
	{
		if (b[i] != nullptr)
		{
			ret = b[i];
			
			break;
		}
	}
	
	delete[] cs;
	delete[] b;
	
	return ret;
}


const neighbor_option* itemizer::get_best_of_sample(const solution* sol, int nsamples)
{
	nsamples = std::max(1, nsamples);
	
	const neighbor_option* best = nullptr;
	double best_cost = DBL_MAX;
	for (int i=0; i<nsamples; i++)
	{
		const neighbor_option* alt;
		
		do
		{
			alt = options[rand() % num_options];
		} while (!alt->in_bounds(sol));
		
		double c = alt->get_improvement(sol);
		if (c < best_cost)
		{
			best_cost = c;
			best = alt;
		}
	}
	return best;
}


const neighbor_option* itemizer::get_random_option(const solution* sol)
{
	if (sol->get_num_serviced() < 5)
	{
		return nullptr;
	}
	
	// Could run through these linearly from a random starting point...
	const neighbor_option* op = nullptr;
	do
	{
		op = options[rand() % num_options];
	} while (!op->in_bounds(sol));

	return op;
}


void itemizer::print_improvements(const solution* sol) const
{
	for (int i=0; i<num_options; i++)
	{
		const neighbor_option* op = options[i];
		if (!op->in_bounds(sol))
		{
			std::cout << 'X';
			continue;
		}
		if (op->get_improvement(sol) < 0.0)
		{
			std::cout << '1';
		}
		else
		{
			std::cout << ' ';
		}
	}
	std::cout << std::endl;
}


std::ostream& operator<<(std::ostream& out, const neighbor_option& op)
{
	return out << "[t=" << op.get_name() << "][s1=" << op.stop1 << "][s2=" << op.stop2 << "]";
}

std::string two_opt::get_name() const
{
	return "swap";
}

std::string resched_option::get_name() const
{
	return "resched";
}

void itemizer::sort(const solution* sol)
{
	std::sort(options, options + num_options, [sol](const neighbor_option* op1, const neighbor_option* op2)
	{
		return op1->get_improvement(sol) < op2->get_improvement(sol);
	});
}
