
#include "item.h"


void test_hash_diffs(city* c)
{
	solution* sol = new solution{c};
	itemizer items{c};
	
	for(int i=0; i<10000; i++)
	{	
		sol->random();
		
		for (int j=0;j<100;j++)
		{
			const neighbor_option* op = items.get_random_option(sol);
			
			solution copy {*sol};
			op->apply(&copy);
			
			if (!(get_hash(&copy) == op->get_hash(sol)))
			{
				std::cout << "The predicted hash is not he same as the computed hash :(" << std::endl;
				trap();
			}
		}
	}
	
	delete sol;
}