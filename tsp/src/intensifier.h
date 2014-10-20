/*
 * intensifier.h
 *
 *  Created on: Oct 19, 2014
 *      Author: thallock
 */

#ifndef INTENSIFIER_H_
#define INTENSIFIER_H_

#include "tabu_list.h"
#include "path_relinking.h"

#include <list>


class intensifier
{
public:
	intensifier(city* c, tabu_list* list, int max_length, std::function<void(const solution*sol)> callback);
	virtual ~intensifier();

	void intensify();
	void discovered(solution* local_minima);

private:
	itemizer items;
	tabu_list* list;
	solution **population;
	solution tmp;
	int max_length;
	std::function<void(const solution*)> f;
};


bool order_tabu_search(solution* sol, itemizer& items, tabu_list* list, std::function<void(const solution* s)> callback);

#endif /* INTENSIFIER_H_ */
