#include "cle_math.h"

/*
 * Calculate variance
 * Uses Kahan summation algorithm
 */
double variance_onepass(double const* x, size_t n) {
    double _sum = 0;
    double _sum_squares = 0;
    double _variance = 0;
    double _y_sum = 0;
    double _y_squares = 0;
    double _t_sum = 0;
    double _t_squares = 0;
    double _c_sum = 0;
    double _c_squares = 0;

    for (size_t i = 0; i != n; ++i) {
        _y_sum = x[i] - _c_sum;
        _t_sum = _sum + _y_sum;
        _c_sum = (_t_sum - _sum) - _y_sum;
        _sum = _t_sum;

        _y_squares = x[i] * x[i] - _c_squares;
        _t_squares = _sum_squares + _y_squares;
        _c_squares = (_t_squares - _sum_squares) - _y_squares;
        _sum_squares = _t_squares;
    }

    _variance = _sum_squares / n - (_sum * _sum) / (n * n);

    return _variance;
}

double variance_onepass_uncompensated(double const* x, size_t n) {
    double _sum = 0;
    double _sum_squares = 0;
    double _variance = 0;

    for (size_t i = 0; i != n; ++i) {
        _sum += x[i];
        _sum_squares += x[i] * x[i];
    }

    _variance = _sum_squares / n - (_sum * _sum) / (n * n);

    return _variance;

}

/*
 * Calculate mean
 * Uses Kahan summation algorithm
 */
double mean(double const* x, size_t n) {
    double _sum = 0;
    double _mean = 0;
    double _y = 0;
    double _t = 0;
    double _c = 0;

    for (size_t i = 0; i != n; ++i) {
        _y = x[i] - _c;
        _t = _sum + _y;
        _c = (_t - _sum) - _y;
        _sum = _t;
    }

    _mean = _sum / n;

    return _mean;
}

/*
 * Calculate variance
 * Uses Kahan summation algorithm
 */
double variance_twopass(double const* x, size_t n) {
    double _sum = 0;
    double _diff = 0;
    double _mean = 0;
    double _variance = 0;
    double _y = 0;
    double _t = 0;
    double _c = 0;

    _mean = mean(x, n);

    for (size_t i = 0; i != n; ++i) {
        _diff = x[i] - _mean;
        _y = _diff * _diff;
        _t = _sum + _y;
        _c = (_t - _sum) - _y;
        _sum = _t;
    }

    _variance = _sum / n;

    return _variance;
}

/*
 * Calculate variance
 * Uses Welford's variance algorithm
 * http://jonisalonen.com/2013/deriving-welfords-method-for-computing-variance/
 */
double variance_welford(double const* x, size_t n) {
    double _sum = 0;
    double _mean = 0;
    double _delta = 0;

    for (size_t i = 0; i != n; ++i) {
        _delta = x[i] - _mean;
        _mean += _delta / (i + 1);
        _sum += _delta * (x[i] - _mean);
    }

    return _sum / n;
}

