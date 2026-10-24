#include "log.h"

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

static FILE* log_file = nullptr;

void print_log(int target_account_id, const char* format, ...) {
    if (log_file == NULL) {
        log_file = fopen("logs.log", "a+");
        if (log_file == NULL) {
            printf("[FATAL ERROR] Failed to open log handle\n");
            return;
        }
    }

    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[32];

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(log_file, "[%s @ Account: %d] ",
            timestamp, target_account_id);

    va_list args;
    va_start(args, format);

    vfprintf(log_file, format, args);

    va_end(args);

    fprintf(log_file, "\n");
    fflush(log_file);
}