#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include "sensor.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static FILE *csv = NULL;
static char csv_buf[256];
static int csv_available = 0;
static unsigned int rng_state = 0;

void sensor_init(const char *csv_path) {
    if (csv_path) {
        csv = fopen(csv_path, "r");
        if (csv) {
            csv_available = 1;
            /* skip header optionally */
            if (fgets(csv_buf, sizeof(csv_buf), csv) == NULL) {
                /* empty file */
                csv_available = 0;
            }
        } else {
            csv_available = 0;
        }
    } else {
        csv_available = 0;
    }
    /* seed simple LCG from time */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    rng_state = (unsigned int)(ts.tv_nsec ^ ts.tv_sec);
}

static unsigned int rand32(void) {
    /* simple LCG */
    rng_state = rng_state * 1664525u + 1013904223u;
    return rng_state;
}

double sensor_read(void) {
    if (csv_available && csv) {
        if (fgets(csv_buf, sizeof(csv_buf), csv) != NULL) {
            double v = 0.0;
            /* accept lines with single number or CSV -> parse first token */
            char *tok = strtok(csv_buf, ", \t\r\n");
            if (tok) {
                if (sscanf(tok, "%lf", &v) == 1) {
                    return v;
                }
            }
            /* fallback to random if parse fails */
        } else {
            /* EOF -> rewind to start after header */
            rewind(csv);
            /* skip header */
            if (fgets(csv_buf, sizeof(csv_buf), csv) == NULL) {
                /* no usable data, mark unavailable */
                csv_available = 0;
                fclose(csv);
                csv = NULL;
            } else {
                /* try read next */
                if (fgets(csv_buf, sizeof(csv_buf), csv) != NULL) {
                    double v = 0.0;
                    char *tok = strtok(csv_buf, ", \t\r\n");
                    if (tok && sscanf(tok, "%lf", &v) == 1) return v;
                }
            }
        }
    }

    /* random fallback: produce values in range [0, 10) */
    return (double)(rand32() % 10000) / 1000.0;
}
