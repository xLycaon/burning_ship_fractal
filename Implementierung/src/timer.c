#include <stdio.h>
#include <limits.h>

#include "timer.h"

#define GIGA 1000000000L
#define NANO 1E-9

static void ts_add(struct timespec a, struct timespec b, struct timespec* res) {
    res->tv_nsec = a.tv_nsec + b.tv_nsec;
    res->tv_sec = res->tv_nsec / GIGA;
    res->tv_nsec %= GIGA;
    res->tv_sec += a.tv_sec + b.tv_sec;
}

static void ts_sub(struct timespec minuend, struct timespec subtrahend, struct timespec* res) {
    res->tv_sec = minuend.tv_sec;
    res->tv_nsec = minuend.tv_nsec - subtrahend.tv_nsec;
    while (res->tv_nsec < 0) {
        res->tv_nsec += GIGA;
        res->tv_sec -= 1;
    }
    res->tv_sec -= subtrahend.tv_sec;
}

static int ts_cmp(struct timespec a, struct timespec b) {
    if (a.tv_sec == b.tv_sec) {
        if (a.tv_nsec < b.tv_nsec) {
            return -1;
        } else if (a.tv_nsec == b.tv_nsec) {
            return 0;
        }
    } else if (a.tv_sec < b.tv_sec) {
        return -1;
    }
    return 1;
}

static double to_secs(struct timespec t) {
    return (double) t.tv_sec + NANO * (double) t.tv_nsec;
}

void time_fn(burning_ship_t fn, struct BS_Params params, unsigned n, int* err) {

    struct timespec run;
    struct timespec start;
    struct timespec end;

    struct timespec cum = {0, 0};
    struct timespec min = {LONG_MAX, LONG_MAX};
    struct timespec max = {0, 0};

    for (unsigned i = 0; i < n; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        fn(params.start, params.width, params.height, params.res, params.n, params.img);
        clock_gettime(CLOCK_MONOTONIC, &end);
        if (end.tv_sec < start.tv_sec) {
            *err = -1;
            return;
        }
        if ((end.tv_sec == start.tv_sec) && (end.tv_nsec < start.tv_nsec)) {
            *err = -1;
            return;
        }
        ts_sub(end, start, &run);
        ts_add(cum, run, &cum);
        if (ts_cmp(min, run) > 0)
            min = run;
        if (ts_cmp(max, run) < 0)
            max = run;
    }

    printf("BENCHMARK: Image of width %lu and height %lu generated %u times.\n",
           params.width, params.height, n);
    printf("CUMULATED: %E S.\n",
           to_secs(cum));
    if (n > 1) {
        printf("AVERAGE: %E s.\n",
               to_secs(cum) / n);
        printf("MIN: %E s.\n",
               to_secs(min));
        printf("MAX: %E s.\n",
               to_secs(max));
    }
    *err = 0;
}
