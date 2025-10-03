#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>

// Estructuras internas
typedef struct {
    unsigned long total, free, buffers, cached, swap_total, swap_free;
} MemInfo;

typedef struct {
    char model_name[256];
    int cores;
    int physical_ids;
} CPUInfo;

typedef struct {
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
} CPUSample;

// Prototipos
int parse_meminfo(MemInfo *mem);
int parse_cpuinfo(CPUInfo *cpu);
int parse_stat_samples(CPUSample *prev, CPUSample *curr, int num_cores);
void display_stats(const MemInfo *mem, const CPUInfo *cpu, const CPUSample *prev, const CPUSample *curr, int num_cores);

volatile sig_atomic_t running = 1;

void signal_handler(int sig) {
    running = 0;
}

int main(void) {
    signal(SIGINT, signal_handler);

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);   // Bajo uso
    init_pair(2, COLOR_CYAN, COLOR_BLACK);    // Etiquetas
    init_pair(3, COLOR_RED, COLOR_BLACK);     // Alto uso (>80%)
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // Medio uso (>50%)
    timeout(0); // No bloqueante

    MemInfo mem = {0};
    CPUInfo cpu = {0};
    CPUSample *prev = NULL;
    CPUSample *curr = NULL;

    if (parse_meminfo(&mem) != 0) {
        printw("ERROR: No se puede leer /proc/meminfo\n");
        refresh();
        sleep(3);
        endwin();
        return 1;
    }

    if (parse_cpuinfo(&cpu) != 0) {
        printw("ERROR: No se puede leer /proc/cpuinfo\n");
        refresh();
        sleep(3);
        endwin();
        return 1;
    }

    prev = calloc(cpu.cores, sizeof(CPUSample));
    curr = calloc(cpu.cores, sizeof(CPUSample));

    if (!prev || !curr) {
        printw("ERROR: Memoria insuficiente\n");
        refresh();
        sleep(3);
        endwin();
        return 1;
    }

    parse_stat_samples(prev, curr, cpu.cores);

    while (running) {
        sleep(2);

        parse_meminfo(&mem);
        parse_cpuinfo(&cpu);

        CPUSample *temp = prev;
        prev = curr;
        curr = temp;
        parse_stat_samples(prev, curr, cpu.cores);

        display_stats(&mem, &cpu, prev, curr, cpu.cores);

        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            running = 0;
        }
    }

    free(prev);
    free(curr);
    endwin();
    return 0;
}

int parse_meminfo(MemInfo *mem) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return -1;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemTotal: %lu kB", &mem->total) == 1) continue;
        if (sscanf(line, "MemFree: %lu kB", &mem->free) == 1) continue;
        if (sscanf(line, "Buffers: %lu kB", &mem->buffers) == 1) continue;
        if (sscanf(line, "Cached: %lu kB", &mem->cached) == 1) continue;
        if (sscanf(line, "SwapTotal: %lu kB", &mem->swap_total) == 1) continue;
        if (sscanf(line, "SwapFree: %lu kB", &mem->swap_free) == 1) continue;
    }
    fclose(fp);

    mem->total *= 1024;
    mem->free *= 1024;
    mem->buffers *= 1024;
    mem->cached *= 1024;
    mem->swap_total *= 1024;
    mem->swap_free *= 1024;

    return 0;
}

int parse_cpuinfo(CPUInfo *cpu) {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) return -1;

    cpu->model_name[0] = '\0';
    cpu->cores = 0;
    cpu->physical_ids = 0;

    char line[256];
    int seen_physid = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "model name%*[ :]%255[^\n]", cpu->model_name) == 1 && strlen(cpu->model_name) == 0) {
            continue;
        }
        if (strncmp(line, "processor", 9) == 0) {
            cpu->cores++;
        }
        if (sscanf(line, "physical id%*[ :]%d", &seen_physid) == 1) {
            if (seen_physid + 1 > cpu->physical_ids) {
                cpu->physical_ids = seen_physid + 1;
            }
        }
    }
    fclose(fp);

    if (strlen(cpu->model_name) == 0) {
        strcpy(cpu->model_name, "Unknown Processor");
    }

    return (cpu->cores > 0) ? 0 : -1;
}

// ✅ Versión robusta y corregida: Lee solo líneas válidas de cpuN
int parse_stat_samples(CPUSample *prev, CPUSample *curr, int num_cores) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return -1;

    char line[512];
    int core_index;

    // Inicializar todos los núcleos a cero (seguridad)
    for (int i = 0; i < num_cores; i++) {
        curr[i].user = curr[i].nice = curr[i].system = curr[i].idle = 0;
        curr[i].iowait = curr[i].irq = curr[i].softirq = curr[i].steal = 0;
        curr[i].guest = curr[i].guest_nice = 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        // Buscar líneas que empiecen con "cpu" seguido de un número
        if (sscanf(line, "cpu%d", &core_index) == 1) {
            // Verificar que el índice esté dentro del rango válido
            if (core_index >= 0 && core_index < num_cores) {
                // Intentar formato moderno (con guest y guest_nice)
                if (sscanf(line,
                           "cpu%d %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                           &core_index,
                           &curr[core_index].user, &curr[core_index].nice, &curr[core_index].system, &curr[core_index].idle,
                           &curr[core_index].iowait, &curr[core_index].irq, &curr[core_index].softirq, &curr[core_index].steal,
                           &curr[core_index].guest, &curr[core_index].guest_nice) == 11) {
                    continue;
                }

                // Si falla, intentar formato antiguo (sin guest/guest_nice)
                if (sscanf(line,
                           "cpu%d %lu %lu %lu %lu %lu %lu %lu %lu",
                           &core_index,
                           &curr[core_index].user, &curr[core_index].nice, &curr[core_index].system, &curr[core_index].idle,
                           &curr[core_index].iowait, &curr[core_index].irq, &curr[core_index].softirq, &curr[core_index].steal) == 9) {
                    curr[core_index].guest = curr[core_index].guest_nice = 0;
                    continue;
                }
            }
        }
        // Ignorar cualquier otra línea (intr, ctxt, btime, etc.)
    }

    fclose(fp);
    return 0;
}

void display_stats(const MemInfo *mem, const CPUInfo *cpu, const CPUSample *prev, const CPUSample *curr, int num_cores) {
    clear();

    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(0, 0, "CPU & MEMORY MONITOR - Real-time System Stats (Updated every 2s)");
    attroff(A_BOLD | COLOR_PAIR(2));
    mvprintw(1, 0, "============================================================");

    unsigned long used_phys = mem->total - mem->free - mem->buffers - mem->cached;
    unsigned long avail_phys = mem->free + mem->buffers + mem->cached;
    mvprintw(3, 0, "Physical Memory:");
    mvprintw(4, 4, "Total:   %8.1f MB", (double)mem->total / (1024*1024));
    mvprintw(5, 4, "Used:    %8.1f MB", (double)used_phys / (1024*1024));
    mvprintw(6, 4, "Available: %8.1f MB", (double)avail_phys / (1024*1024));

    unsigned long used_swap = mem->swap_total - mem->swap_free;
    mvprintw(8, 0, "Swap Memory:");
    mvprintw(9, 4, "Total:   %8.1f MB", (double)mem->swap_total / (1024*1024));
    mvprintw(10, 4, "Used:    %8.1f MB", (double)used_swap / (1024*1024));
    mvprintw(11, 4, "Free:    %8.1f MB", (double)mem->swap_free / (1024*1024));

    mvprintw(13, 0, "Processor:");
    mvprintw(14, 4, "%s", cpu->model_name);
    mvprintw(15, 4, "Cores: %d | Physical CPUs: %d", cpu->cores, cpu->physical_ids);

    mvprintw(17, 0, "CPU Load per Core (%):");
    for (int i = 0; i < num_cores && i < 16; i++) {
        if (prev[i].user == 0 && prev[i].idle == 0) {
            mvprintw(18 + i, 4, "Core %2d: [---%%] (waiting...)", i);
            continue;
        }

        unsigned long total_prev = prev[i].user + prev[i].nice + prev[i].system +
                                   prev[i].idle + prev[i].iowait + prev[i].irq +
                                   prev[i].softirq + prev[i].steal + prev[i].guest + prev[i].guest_nice;
        unsigned long total_curr = curr[i].user + curr[i].nice + curr[i].system +
                                   curr[i].idle + curr[i].iowait + curr[i].irq +
                                   curr[i].softirq + curr[i].steal + curr[i].guest + curr[i].guest_nice;

        if (total_curr <= total_prev) {
            mvprintw(18 + i, 4, "Core %2d: [---%%] (no change)", i);
            continue;
        }

        unsigned long delta_total = total_curr - total_prev;
        unsigned long delta_idle = curr[i].idle - prev[i].idle;
        unsigned long delta_busy = delta_total - delta_idle;
        double usage = (delta_busy * 100.0) / delta_total;

        if (usage > 80) attron(COLOR_PAIR(3));
        else if (usage > 50) attron(COLOR_PAIR(4));
        else attron(COLOR_PAIR(1));

        mvprintw(18 + i, 4, "Core %2d: [%5.1f%%]", i, usage);
        attroff(COLOR_PAIR(1) | COLOR_PAIR(3) | COLOR_PAIR(4));
    }

    mvprintw(LINES - 2, 0, "Press 'q' to quit | Updated at: ");
    time_t now = time(NULL);
    mvaddstr(LINES - 2, 30, ctime(&now));

    mvprintw(LINES - 1, 0, "Controls: q=quit");

    refresh();
}
