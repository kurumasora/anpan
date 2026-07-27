#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "conveyor.h"

#define BR_STOCKER_MAX     (double)0.52
#define BR_STOCKER_MIN     (double)0.48

#define OVEN1_MAX     (double)2.02
#define OVEN1_MIN     (double)1.98

#define OVEN2_MAX     (double)1.02
#define OVEN2_MIN     (double)0.98

#define DO_STOCKER_MAX     (double)3.02
#define DO_STOCKER_MIN     (double)2.98

#define POS_MAX     (double)3.5
#define POS_MIN     (double)0.0

