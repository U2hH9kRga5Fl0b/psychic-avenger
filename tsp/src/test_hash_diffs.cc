
#include "item.h"


void test_hash_diffs(city* c)
{
	solution* sol = new solution{c};
	itemizer items{c};
	
	for(int i=0; i<100000; i++)
	{	
		sol->random();
		sol->is_valid();
		
		int num_options = items.get_num_options();
		for (int j=0;j<num_options;j++)
//		for (int j=0;j<100;j++)
		{
//			const neighbor_option* op = items.get_random_option(sol);
			const neighbor_option* op = items.get_option(j);
			
			solution copy {*sol};
			op->apply(&copy);
			copy.is_valid();
			
			if (!(get_hash(&copy) == op->get_hash(sol)))
			{
				std::cout << "for operation " << (*op) << " on solution\n" << (*sol) << "\nto solution\n" << copy << std::endl;
				std::cout << "The predicted hash is not the same as the computed hash :(" << std::endl;
				trap();
			}

			if (abs(sol->get_cost() + op->get_improvement(sol) - copy.get_cost()) > 1e-14)
			{
				std::cout << std::endl;
				std::cout << "for operation " << (*op) << " on solution\n" << (*sol) << "\nto solution\n" << copy << std::endl;
				std::cout << "The predicted cost is not the same as the computed cost :(" << std::endl;

				std::cout << "cost=" << copy.get_cost() << " imp=" << op->get_improvement(sol) << " old=" << sol->get_cost() << std::endl;
				trap();
			}
		}
	}
	
	delete sol;
}
