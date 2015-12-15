#include "timer.h"

#include <time.h>

cle_timer_t timer_start() {
    cle_timer_t _timer = {0};

    clock_gettime(CLOCK_MONOTONIC, &_timer.ts);

    return _timer;
}

/*
 * Returns time in nanoseconds
 */
uint64_t timer_stop(cle_timer_t _timer_start) {
    cle_timer_t _timer_stop = {0};
    time_t _seconds = 0;
    long _nanos = 0;
    uint64_t _time = 0;

    clock_gettime(CLOCK_MONOTONIC, &_timer_stop.ts);

    _seconds = _timer_stop.ts.tv_sec - _timer_start.ts.tv_sec;
    _nanos = _timer_stop.ts.tv_nsec - _timer_start.ts.tv_nsec;

    _time = ((uint64_t) _seconds) * (1000L * 1000L * 1000L) + ((uint64_t) _nanos);

    return _time;
}
