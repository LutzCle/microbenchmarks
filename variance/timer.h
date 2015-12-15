#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <time.h>

typedef struct cle_timer {
    struct timespec ts;
} cle_timer_t;

cle_timer_t timer_start();
uint64_t timer_stop(cle_timer_t);

#endif /* TIMER_H */
