#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <time.h>
#include "sensor.h"
#include "actuator.h"

double now_monotonic_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(void) {
    struct actuator *led = led_create("LED1");
    struct actuator *buzzer = buzzer_create("Buzzer1");

    if (!led || !buzzer) return 1;

    led->activate(led->params);
    buzzer->activate(buzzer->params);

    printf("Tiempo actual: %f\n", now_monotonic_seconds());

    led->deactivate(led->params);
    buzzer->deactivate(buzzer->params);

    actuator_destroy(led);
    actuator_destroy(buzzer);

    return 0;
}


