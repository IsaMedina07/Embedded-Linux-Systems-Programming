#include "actuator.h"
#include <stdlib.h>

void actuator_destroy(struct actuator *a) {
    if (!a) return;
    if (a->params) free(a->params);
    free(a);
}

