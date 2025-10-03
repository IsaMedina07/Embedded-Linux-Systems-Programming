// src/assignment_sensor.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static volatile sig_atomic_t keep_running = 1;

void handle_sigterm(int sig) {
    (void)sig;
    keep_running = 0;
}

void iso8601_now(char *buf, size_t bufsz) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm;
    gmtime_r(&ts.tv_sec, &tm);
    // YYYY-MM-DDTHH:MM:SS.mmmZ
    int ms = ts.tv_nsec / 1000000;
    strftime(buf, bufsz, "%Y-%m-%dT%H:%M:%S", &tm);
    size_t len = strlen(buf);
    snprintf(buf+len, bufsz - len, ".%03dZ", ms);
}

int main(int argc, char **argv) {
    const char *device = "/dev/urandom";
    const char *logfile = "/tmp/assignment_sensor.log";
    unsigned int interval = 5; // seconds default

    for (int i=1;i<argc;i++) {
        if ((strcmp(argv[i], "--device")==0 || strcmp(argv[i], "-d")==0) && i+1<argc) {
            device = argv[++i];
        } else if ((strcmp(argv[i], "--logfile")==0 || strcmp(argv[i], "-l")==0) && i+1<argc) {
            logfile = argv[++i];
        } else if ((strcmp(argv[i], "--interval")==0 || strcmp(argv[i], "-i")==0) && i+1<argc) {
            interval = (unsigned int)atoi(argv[++i]);
            if (interval == 0) interval = 1;
        } else if ((strcmp(argv[i], "--help")==0 || strcmp(argv[i], "-h")==0)) {
            fprintf(stderr, "Usage: %s [--interval <s>] [--logfile <path>] [--device <path>]\n", argv[0]);
            return 0;
        }
    }

    // Setup signal handler
    struct sigaction sa;
    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    // Open device (read binary)
    FILE *dev = fopen(device, "rb");
    if (!dev) {
        fprintf(stderr, "FATAL: cannot open device '%s': %s\n", device, strerror(errno));
        return 2;
    }

    // Determine log path: try /tmp (provided by caller). If not writable, failover to /var/tmp
    FILE *logf = fopen(logfile, "a");
    if (!logf) {
        // try fallback
        const char *fallback = "/var/tmp/assignment_sensor.log";
        fprintf(stderr, "WARN: cannot open logfile %s (%s). Trying fallback %s\n", logfile, strerror(errno), fallback);
        logf = fopen(fallback, "a");
        if (!logf) {
            fprintf(stderr, "FATAL: cannot open fallback logfile %s: %s\n", fallback, strerror(errno));
            fclose(dev);
            return 3;
        } else {
            logfile = fallback;
        }
    }

    // We'll fflush after each fprintf to avoid partial lines.
    // Also set line buffering on logf for safety (though files are usually fully buffered).
    setvbuf(logf, NULL, _IOLBF, 0);

    // Buffer for timestamp
    char nowbuf[64];
    // Main loop
    while (keep_running) {
        uint32_t value;
        size_t r = fread(&value, sizeof(value), 1, dev);
        if (r != 1) {
            fprintf(stderr, "ERROR: failed reading device: %s\n", strerror(errno));
            // don't exit immediately; sleep then continue
        } else {
            // Write log line
            iso8601_now(nowbuf, sizeof(nowbuf));
            // Human-readable hex value
            fprintf(logf, "%s | 0x%08X\n", nowbuf, value);
            fflush(logf);
        }
        // Sleep but break early if signaled
        for (unsigned int s=0; s<interval && keep_running; ++s) {
            sleep(1);
        }
    }

    // Graceful shutdown
    fprintf(stderr, "INFO: received stop signal, shutting down.\n");
    fclose(dev);
    fflush(logf);
    fclose(logf);
    return 0;
}
        }
    }

    signal(SIGTERM, handle_sigterm);

    while (running) {
        unsigned int val = rand(); // simulamos lectura de sensor
        fprintf(logf, "%s | %u\n", iso8601_timestamp(), val);
        fflush(logf);
        sleep(interval);
    }

    fclose(logf);
    return 0;
}
