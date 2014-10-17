
#include "routines.h"
#include "hash.h"
#include <iomanip>


#ifdef HAVE_MAIN

int main()
{
	srand(time(nullptr));
	
	int max_prime = get_prime_less_than(50);
	int num_to_perm = 3000;
	int nsamples = 1000;
	
	int* perm = new int[num_to_perm];
	for (int i=0;i<num_to_perm;i++)
	{
		perm[i] = i;
	}
	
	int *collisions1 = new int[max_prime];
	int *collisions2 = new int[max_prime];
	
	int **hash_keys = new int*[num_to_perm];
	for (int i=0;i<num_to_perm;i++)
	{
		hash_keys[i] = new int[num_to_perm];
		for (int j=0;j<num_to_perm;j++)
		{
			hash_keys[i][j] = rand();
		}
	}
	
	
	int col1s = 0;
	int col2s = 0;

	int p = get_prime_less_than(1000);
	
	for (int tst=0;tst<nsamples;tst++)
	{
		for (int i=0; i<max_prime; i++)
		{
			collisions1[i] = collisions2[i] = 0;
		}
	
		for (int t=0; t<max_prime/4; t++)
		{
#if 1
			std::random_shuffle(perm, perm + num_to_perm);
#else
			std::next_permutation(perm, perm + num_to_perm);
#endif
			if (num_to_perm < 50)
			{
				std::cout << "Next perm is " << std::endl;
				for (int i=0;i<num_to_perm;i++)
				{
					std::cout << std::setw(5) << perm[i];
				}
				std::cout << std::endl;
			}
			
			{
				int hash1 = 0;
				int acc = 1;
				for (int i=0; i<num_to_perm; i++)
				{
					acc *= p;
					hash1 += 
#if 0
						hash_keys[1][i] 
#else
						acc
#endif
						* perm[i];
				}
				hash1 = std::abs(hash1) % max_prime;
				
				std::cout << "h1=" << hash1;
				
				if (collisions1[hash1])
				{
					col1s++;
				}
				collisions1[hash1]++;
			}
			
			
			{
				int hash2 = 0;
				for (int i=0; i<num_to_perm; i++)
				{
					hash2 += hash_keys[perm[i]][i];
				}
				hash2 = std::abs(hash2) % max_prime;
				std::cout << " h2=" << hash2 << std::endl;
				
				if (collisions2[hash2])
				{
					col2s++;
				}
				collisions2[hash2]++;
			}
			
			for (int i=0;i<max_prime;i++)
			{
				std::cout << std::setw(5) << collisions1[i];
			}
			std::cout << std::endl;
			for (int i=0;i<max_prime;i++)
			{
				std::cout << std::setw(5) << collisions2[i];
			}
			std::cout << std::endl;
		}
	}
	
	std::cout << "Number of collisions: for first: " << col1s << " and for second: " << col2s << std::endl;
	
	for (int i=0; i<num_to_perm; i++)
	{
		delete hash_keys[i];
	}
	delete[] hash_keys;
	
	delete[] perm;
	delete[] collisions1;
	delete[] collisions2;
}
#endif
