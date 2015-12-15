#include "cle_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <gmp.h>
#include <gsl/gsl_statistics_double.h>

#define SIZE 1000000

double variance_gmp(double const* vals, size_t n);

void run(double const* vals, size_t n) {
    double stddev_one = 0;
    double var_oneu = 0;
    double stddev_two = 0;
    double var_wel = 0;
    double var_gsl = 0;
    double mean_gsl = 0;
    double var_gmp = 0;
    double diff_one = 0;
    double diff_oneu = 0;
    double diff_two = 0;
    double diff_wel = 0;
    double diff_gsl = 0;

    stddev_one = variance_onepass(vals, n);
    stddev_two = variance_twopass(vals, n);
    var_oneu = variance_onepass_uncompensated(vals, n);
    var_wel = variance_welford(vals, n);

    mean_gsl = gsl_stats_mean(vals, 1, n);
    var_gsl = gsl_stats_variance_with_fixed_mean(vals, 1, n, mean_gsl);

    var_gmp = variance_gmp(vals, n);

    diff_one = var_gmp - stddev_one;
    diff_oneu = var_gmp - var_oneu;
    diff_two = var_gmp - stddev_two;
    diff_wel = var_gmp - var_wel;
    diff_gsl = var_gmp - var_gsl;

    printf("Variances (difference from GMP)\n");
    printf("GMP: %f\n", var_gmp);
    printf("GSL: %f (%f)\n", var_gsl, diff_gsl);
    printf("OnePass: %f (%f)\n", stddev_one, diff_one);
    printf("OnePassU: %f (%f)\n", var_oneu, diff_oneu);
    printf("TwoPass: %f (%f)\n", stddev_two, diff_two);
    printf("Welfold: %f (%f)\n", var_wel, diff_wel);
}

/*
 * Calculate variance
 * Note: GMP is an arbitrary precision math library - this is base of comparision
 */
double variance_gmp(double const* vals, size_t n) {
    mpq_t _qn;
    mpq_t _mean;
    mpq_t _sum;
    mpq_t _difference;
    mpq_t _square;
    mpq_t _variance;
    double _dvariance = 0;

    mpq_t *_qvals = malloc(sizeof(mpq_t) * n);
    if (_qvals == NULL) {
        fprintf(stderr, "Couldn't allocate qvals\n");
    }

    /* Convert to GMP rational type */
    mpq_init(_qn);
    mpq_set_ui(_qn, n, 1);
    for (size_t i = 0; i != n; ++i) {
        mpq_init(_qvals[i]);
        mpq_set_d(_qvals[i], vals[i]);
    }

    /* Calculate mean */
    mpq_init(_mean);
    for (size_t i = 0; i != n; ++i) {
        mpq_add(_mean, _mean, _qvals[i]);
    }
    mpq_div(_mean, _mean, _qn);

    /* Calculate variance */
    mpq_init(_sum);
    mpq_init(_difference);
    mpq_init(_square);
    for (size_t i = 0; i != n; ++i) {
        mpq_sub(_difference, _qvals[i], _mean);
        mpq_mul(_square, _difference, _difference);
        mpq_add(_sum, _sum, _square);
    }

    mpq_init(_variance);
    mpq_div(_variance, _sum, _qn);

    /* Convert to double */
    _dvariance = mpq_get_d(_variance);

    /* Free space */
    for (size_t i = 0; i != n; ++i) {
        mpq_clear(_qvals[i]);
    }
    free(_qvals);
    mpq_clear(_qn);
    mpq_clear(_mean);
    mpq_clear(_sum);
    mpq_clear(_difference);
    mpq_clear(_square);
    mpq_clear(_variance);

    return _dvariance;
}

/*
 * Test with random values
 */
void test_random(double * vals, size_t n) {
    srand(time(NULL));
    for (size_t i = 0; i != n; ++i) {
        vals[i] = rand();
    }
    run(vals, n);
}

/*
 * Test with large but nearly equal values
 * for large sum of squares but small variance
 */
void test_approx_equal(double * vals, size_t n) {
    const double LARGE_NUMBER = 10000000;
    const int SMALL_VARIANCE = 100;
    for (size_t i = 0; i != n; ++i) {
        vals[i] = LARGE_NUMBER - rand() % SMALL_VARIANCE;
    }
    run(vals, n);
}

/*
 * Test with ascending values
 */
void test_ascending(double * vals, size_t n) {
    for (size_t i = 0; i != n; ++i) {
        vals[i] += i;
    }
    run(vals, n);
}

/*
 * Test with descending values
 */
void test_descending(double * vals, size_t n) {
    for (size_t i = 0; i != n; ++i) {
        vals[i] = (n / 2) * (n - 1) - ((i / 2) * (i - 1));
    }
    run(vals, n);
}

/*
 * Test with alternating values
 */
void test_alternating(double * vals, size_t n) {
    const double LARGE_NUMBER = 10000000;
    const double SMALL_NUMBER = 100;
    const int VARIANCE = 100;
    for (size_t i = 0; i != n; ++i) {
        if (i % 2 == 0) {
            vals[i] = LARGE_NUMBER - rand() % VARIANCE;
        }
        else {
            vals[i] = SMALL_NUMBER - rand() % VARIANCE;
        }
    }
    run(vals, n);
}

int main() {
    double *vals = malloc(SIZE * sizeof(double));
    if (vals == NULL) {
        fprintf(stderr, "Failed to malloc value array\n");
    }

    printf("\nRandom\n");
    test_random(vals, SIZE);

    printf("\nApproximately equal with small variance\n");
    test_approx_equal(vals, SIZE);

    printf("\nAscending\n");
    test_ascending(vals, SIZE);

    printf("\nDescending\n");
    test_descending(vals, SIZE);

    printf("\nAlternating\n");
    test_alternating(vals, SIZE);

    free(vals);
}

