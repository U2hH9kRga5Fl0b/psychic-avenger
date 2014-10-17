/*
 * common.cc
 *
 *  Created on: Oct 16, 2014
 *      Author: thallock
 */



#include "common.h"

#include <iostream>


void breakpointhere(const std::string& msg)
{
	std::cout << "on the way out!" << std::endl;
	exit(-1);
}


