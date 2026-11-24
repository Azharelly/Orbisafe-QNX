#include <cstdio>
#include <cstring>         
#include <sys/slog2.h>     

constexpr const char* buffer_set_name = "probe_slog2"; 
constexpr const char* buffer_name = "probe_log";
constexpr int pages = 1;                               

int main() {
    slog2_buffer_set_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.buffer_set_name = buffer_set_name;
    cfg.num_buffers = 1;
    cfg.verbosity_level = SLOG2_INFO;          
    cfg.buffer_config[0].buffer_name = buffer_name;
    cfg.buffer_config[0].num_pages = pages;

    slog2_buffer_t handles[1];
    if (slog2_register(&cfg, handles, 0) == -1) {
        printf("slog2_register failed\n");
        return 1;
    }

    int failures = 0;
    if (slog2c(handles[0], 0, SLOG2_INFO, "probe: constant message") == -1) {
        failures++;
    }
    for (int seq = 1; seq <= 3; seq++) {
        if (slog2f(handles[0], 0, SLOG2_WARNING, "probe: formatted message %d", seq) == -1) {
            failures++;
        }
    }

    slog2c(handles[0], 0, SLOG2_DEBUG1, "probe: this one should be filtered");

    if (failures == 0) {
        printf("probe_slog2: messages written, now run: slog2info -b %s\n", buffer_set_name);
        printf("RESULT: WORKS (written; confirm with slog2info)\n");
        return 0;
    }
    printf("RESULT: DOES NOT WORK (%d write failures)\n", failures);
    return 1;
}