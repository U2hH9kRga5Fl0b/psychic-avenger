
#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <functional>

void breakpointhere(const std::string &msg="death to us all!");
#define trap()  do { breakpointhere( ); __builtin_unreachable ();} while(0);

#define DEBUG 1





#define GRAPHICS 1




#endif
