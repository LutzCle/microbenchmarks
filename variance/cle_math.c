#include "cle_math.h"

#include <math.h>
#include <x86intrin.h>

#define SSE3_D_VEC_SIZE 2

#define DOUBLE_ABS_MASK 0x7FFFFFFFFFFFFFFF

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

    _variance = (_sum_squares - (_sum * _sum) / n) / n;

    return _variance;
}

double variance_onepass_sse3(double const* x, size_t n) {
    double _variance_d = 0;
    double _sum_d = 0;
    double _sum_squares_d = 0;
    __m128d const _zero = {0};
    __m128d _val = {0};
    __m128d _squared = {0};
    __m128d _sum = {0};
    __m128d _sum_squares = {0};
    __m128d _y_sum = {0};
    __m128d _y_squares = {0};
    __m128d _t_sum = {0};
    __m128d _t_squares = {0};
    __m128d _c_sum = {0};
    __m128d _c_squares = {0};

    for (size_t i = 0; i < n; i += SSE3_D_VEC_SIZE) {
        _val = _mm_load_pd(&x[i]);
        _y_sum = _mm_sub_pd(_val, _c_sum);
        _t_sum = _mm_add_pd(_sum, _y_sum);
        _c_sum = _mm_sub_pd(_t_sum, _sum);
        _c_sum = _mm_sub_pd(_c_sum, _y_sum);
        _sum = _t_sum;

        _squared = _mm_mul_pd(_val, _val);
        _y_squares = _mm_sub_pd(_squared, _c_squares);
        _t_squares = _mm_add_pd(_sum_squares, _y_squares);
        _c_squares = _mm_sub_pd(_t_squares, _sum_squares);
        _c_squares = _mm_sub_pd(_c_squares, _y_squares);
        _sum_squares = _t_squares;
    }
    _sum = _mm_hadd_pd(_sum, _zero);
    _sum_squares = _mm_hadd_pd(_sum_squares, _zero);
    _sum_d = _mm_cvtsd_f64(_sum);
    _sum_squares_d = _mm_cvtsd_f64(_sum_squares);

    if (n % SSE3_D_VEC_SIZE == 1) {
        _sum_d += x[n-1];
        _sum_squares_d += x[n-1] * x[n-1];
    }

    _variance_d = (_sum_squares_d - (_sum_d * _sum_d) / n) / n;

    return _variance_d;
}

double variance_onepass_kbn(double const* x, size_t n) {
    double _sum = 0;
    double _sum_squares = 0;
    double _squared = 0;
    double _variance = 0;
    double _c_sum = 0;
    double _c_squares = 0;
    double _t_sum = 0;
    double _t_squares = 0;

    _sum = x[0];
    _sum_squares = x[0] * x[0];
    for (size_t i = 1; i != n; ++i) {
        _t_sum = _sum + x[i];
        if (fabs(_sum) >= fabs(x[i])) {
            _c_sum = _c_sum + ((_sum - _t_sum) + x[i]);
        }
        else {
            _c_sum = _c_sum + ((x[i] - _t_sum) + _sum);
        }
        _sum = _t_sum;

        _squared = x[i] * x[i];
        _t_squares = _sum_squares + _squared;
        if (fabs(_sum_squares) >= fabs(_squared)) {
            _c_squares = _c_squares + ((_sum_squares - _t_squares) + _squared);
        }
        else {
            _c_squares = _c_squares + ((_squared - _t_squares) + _sum_squares);
        }
        _sum_squares = _t_squares;
    }
    _sum = _sum + _c_sum;
    _sum_squares = _sum_squares + _c_squares;

    _variance = (_sum_squares - (_sum * _sum) / n) / n;

    return _variance;
}

double variance_onepass_kbn_sse4_1(double const* x, size_t n) {
    double _variance_d = 0;
    double _sum_d = 0;
    double _sum_squares_d = 0;
    double _tmp_sum_d[2 * SSE3_D_VEC_SIZE] = {0};
    double _tmp_squares_d[2 * SSE3_D_VEC_SIZE] = {0};
    __m128d const _zero = {0};
    __m128d const _abs_mask = {DOUBLE_ABS_MASK, DOUBLE_ABS_MASK};
    __m128d _val = {0};
    __m128d _abs_val = {0};
    __m128d _abs_sum = {0};
    __m128d _abs_squared = {0};
    __m128d _abs_sum_squares = {0};
    __m128d _is_greater_sum = {0};
    __m128d _is_greater_squares = {0};
    __m128d _squared = {0};
    __m128d _sum = {0};
    __m128d _sum_squares = {0};
    __m128d _variance = {0};
    __m128d _c_sum = {0};
    __m128d _c_sum_ge = {0};
    __m128d _c_squares_ge = {0};
    __m128d _c_squares_lt = {0};
    __m128d _c_sum_lt = {0};
    __m128d _c_squares = {0};
    __m128d _t_sum = {0};
    __m128d _t_squares = {0};

    _sum = _mm_load_pd(&x[0]);
    _sum_squares = _mm_mul_pd(_sum, _sum);
    for (size_t i = SSE3_D_VEC_SIZE; i < n; i += SSE3_D_VEC_SIZE) {
        _val = _mm_load_pd(&x[i]);
        _squared = _mm_mul_pd(_val, _val);

        _t_sum = _mm_add_pd(_sum, _val);

        // if branch
        _c_sum_ge = _mm_sub_pd(_sum, _t_sum);
        _c_sum_ge = _mm_add_pd(_c_sum_ge, _val);
        // else branch
        _c_sum_lt = _mm_sub_pd(_val, _t_sum);
        _c_sum_lt = _mm_add_pd(_c_sum_lt, _sum);

        // do if (fabs(_sum) >= fabs(x[i]))
        _abs_sum = _mm_and_pd(_sum, _abs_mask);
        _abs_val = _mm_and_pd(_val, _abs_mask);
        _is_greater_sum = _mm_cmpge_pd(_abs_sum, _abs_val);
        _c_sum_ge = _mm_blendv_pd(_c_sum_ge, _c_sum_lt, _is_greater_sum);
        _c_sum = _mm_add_pd(_c_sum, _c_sum_ge);

        _sum = _t_sum;

        _t_squares = _mm_add_pd(_sum_squares, _squared);

        _c_squares_ge = _mm_sub_pd(_sum_squares, _t_squares);
        _c_squares_ge = _mm_add_pd(_c_squares_ge, _squared);

        _c_squares_lt = _mm_sub_pd(_squared, _t_squares);
        _c_squares_lt = _mm_add_pd(_c_squares_lt, _sum_squares);

        _abs_sum_squares = _mm_and_pd(_sum_squares, _abs_mask);
        _abs_squared = _mm_and_pd(_squared, _abs_mask);
        _is_greater_squares = _mm_cmpge_pd(_abs_sum_squares, _abs_squared);
        _c_squares_ge = _mm_blendv_pd(_c_squares_ge, _c_squares_lt, _is_greater_squares);
        _c_squares = _mm_add_pd(_c_squares, _c_squares_ge);

        _sum_squares = _t_squares;
    }
    _mm_store_pd(&_tmp_sum_d[0], _sum);
    _mm_store_pd(&_tmp_sum_d[SSE3_D_VEC_SIZE], _c_sum);
    _mm_store_pd(&_tmp_squares_d[0], _sum_squares);
    _mm_store_pd(&_tmp_squares_d[SSE3_D_VEC_SIZE], _c_squares);

    /* TODO: Use KBN summation to _mm_hsum_pd value and error vectors */
    _sum_d = _tmp_sum_d[0] + _tmp_sum_d[1] + _tmp_sum_d[2] + _tmp_sum_d[3];
    _sum_squares_d = _tmp_squares_d[0] + _tmp_squares_d[1] + _tmp_squares_d[2] + _tmp_squares_d[3];

    /* TODO: Handle (n % SSE3_D_VEC_SIZE == 1) case */

    _variance_d = (_sum_squares_d - (_sum_d * _sum_d) / n) / n;

    return _variance_d;
}

double variance_onepass_naive(double const* x, size_t n) {
    double _sum = 0;
    double _sum_squares = 0;
    double _variance = 0;

    for (size_t i = 0; i != n; ++i) {
        _sum += x[i];
        _sum_squares += x[i] * x[i];
    }

    _variance = (_sum_squares - (_sum * _sum) / n) / n;

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

