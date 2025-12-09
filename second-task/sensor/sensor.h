#ifndef SENSOR_H
#define SENSOR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void sensor_init(const char *csv_path); /* if csv_path != NULL, try to feed from CSV */
double sensor_read(void); /* returns next sample (blocking not required) */

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_H */
