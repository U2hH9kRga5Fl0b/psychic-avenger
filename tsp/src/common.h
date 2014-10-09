
#ifndef COMMON_H_
#define COMMON_H_

void breakpointhere(const std::string &msg="death to us all!");
#define trap()  do { breakpointhere( ); __builtin_unreachable ();} while(0);

#define DEBUG 1

#endif