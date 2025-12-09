#include "actuator.h"
#include <stdio.h>
#include <stdlib.h>

struct led_params {
    const char *label;
    int state;
};

static void led_activate(void *p) {
    struct led_params *lp = p;
    lp->state = 1;
    printf("LED [%s] activado\n", lp->label);
}

static void led_deactivate(void *p) {
    struct led_params *lp = p;
    lp->state = 0;
    printf("LED [%s] desactivado\n", lp->label);
}

static int led_status(void *p) {
    struct led_params *lp = p;
    return lp->state;
}

struct actuator *led_create(const char *label) {
    struct actuator *a = malloc(sizeof(struct actuator));
    if (!a) return NULL;

    struct led_params *p = malloc(sizeof(struct led_params));
    if (!p) { free(a); return NULL; }

    p->label = label;
    p->state = 0;

    a->label = label;
    a->params = p;
    a->activate = led_activate;
    a->deactivate = led_deactivate;
    a->status = led_status;

    return a;
}
