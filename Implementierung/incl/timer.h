#pragma once

#include <sys/time.h>

typedef struct {
	struct timeval **runs;
	struct timeval *avg;
	struct timeval *min;
	struct timeval *max;
	struct timeval cum_avg;
	struct timeval cum_min;
	struct timeval cum_max;
} FnStat;
