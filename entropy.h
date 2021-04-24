#ifndef __ENTROPY_H
#define __ENTROPY_H

#include <stdint.h>

float calculate_entropy(char* text);

float calculate_entropy_direct(uint8_t* codes,int len);

double entropy_speed_test(char* src,int iteration);

#endif