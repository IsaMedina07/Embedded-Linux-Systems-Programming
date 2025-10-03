#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

static int running = 1;

void handle_sigterm(int signum) {
    (void)signum;
    running = 0;
}

char* iso8601_timestamp() {
    static char buf[32];
    time_t now = time(NULL);
    struct tm tm;
    gmtime_r(&now, &tm);
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return buf;
}

int main(int argc, char *argv[]) {
    int interval = 5; // default seconds
    const char *logpath = "/tmp/assignment_sensor.log";
    FILE *logf;

    if (argc > 1) interval = atoi(argv[1]);  // opcional CLI flag simple

    logf = fopen(logpath, "a");
    if (!logf) {
        logpath = "/var/tmp/assignment_sensor.log"; // fallback
        logf = fopen(logpath, "a");
        if (!logf) {
            perror("No se pudo abrir archivo de log");
            return 1;
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
