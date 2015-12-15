#include "cle_math.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>

#define RUNS 10
#define SIZE ((1L << 17) * 100) /* 100 MB */

void timed_run(double (*f)(double const*, size_t), size_t runs,
        double const* vals, size_t n) {
    cle_timer_t _timer;
    uint64_t run_time = 0;

    /* Warm up */
    (*f)(vals, n);

    /* Do runs */
    for (size_t r = 0; r != runs; ++r) {
        _timer = timer_start();
        (*f)(vals, n);
        run_time += timer_stop(_timer);
    }
    run_time /= runs;

    printf("Mean time (ms) over %d runs: %llu\n", runs, run_time / 1000 / 1000);
}

int main() {
    double *vals = malloc(SIZE * sizeof(double));
    if (vals == NULL) {
        fprintf(stderr, "Failed to malloc value array\n");
    }

    printf("OnePass\n");
    timed_run(&variance_onepass, RUNS, vals, SIZE);

    printf("\nOnePassUncompensated\n");
    timed_run(&variance_onepass_uncompensated, RUNS, vals, SIZE);

    printf("\nTwoPass\n");
    timed_run(&variance_twopass, RUNS, vals, SIZE);

    printf("\nWelford\n");
    timed_run(&variance_welford, RUNS, vals, SIZE);

    free(vals);
}

