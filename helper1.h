#ifndef HELP1_H
#define HELP1_H

#include <stdio.h>

// this function will read the input from the client/upstream server
// and return 1 if the input is valid
// while other int if the input is invalid(i.e. not implemented)
int read_input(unsigned char* buffer_temp, int size);


#endif
