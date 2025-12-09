#include "actuator.h"
#include <stdio.h>
#include <stdlib.h>

struct buzzer_params {
    const char *label;
    int state;
};

static void buzzer_activate(void *p) {
    struct buzzer_params *bp = p;
    bp->state = 1;
    printf("BUZZER [%s] activado\n", bp->label);
}

static void buzzer_deactivate(void *p) {
    struct buzzer_params *bp = p;
    bp->state = 0;
    printf("BUZZER [%s] desactivado\n", bp->label);
}

static int buzzer_status(void *p) {
    struct buzzer_params *bp = p;
    return bp->state;
}

struct actuator *buzzer_create(const char *label) {
    struct actuator *a = malloc(sizeof(struct actuator));
    if (!a) return NULL;

    struct buzzer_params *p = malloc(sizeof(struct buzzer_params));
    if (!p) { free(a); return NULL; }

    p->label = label;
    p->state = 0;

    a->label = label;
    a->params = p;
    a->activate = buzzer_activate;
    a->deactivate = buzzer_deactivate;
    a->status = buzzer_status;

    return a;
}

