#ifndef ACTUATOR_H
#define ACTUATOR_H

struct actuator {
    const char *label;
    void *params;

    void (*activate)(void *);
    void (*deactivate)(void *);
    int  (*status)(void *);
};

/* LED */
struct actuator *led_create(const char *label);

/* Buzzer */
struct actuator *buzzer_create(const char *label);

/* Destructor común */
void actuator_destroy(struct actuator *a);

#endif

