#ifndef INTPRESSOR_H
#define INTPRESSOR_H

#include <Arduino.h>

namespace Intpressor
{
    int compress(const int *values, const int *sizes, int numValues, char *bytes);
    void extract(const char *chars, const int *sizes, int numValues, int *result);
    const int MAX_VALUES = 100;
}

#endif // INTPRESSOR_H
