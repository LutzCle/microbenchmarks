#ifndef CLE_MATH_H
#define CLE_MATH_H

#include <stddef.h> /* size_t */

double mean(double const* values, size_t size);

double variance_onepass(double const* values, size_t size);
double variance_onepass_uncompensated(double const* values, size_t size);
double variance_twopass(double const* values, size_t size);
double variance_welford(double const* values, size_t size);

#endif /* CLE_MATH_H */
