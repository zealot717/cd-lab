<<<<<<< HEAD

#define _POSIX_C_SOURCE 199309L
#define _GNU_SOURCE
=======
// runtime/runtime.c
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1

#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <papi.h>

#define MAX_FUNCS 1024
#define MAX_EVENTS 4
#define MAX_NAME_LEN 128

typedef struct {
    char func_name[MAX_NAME_LEN];
    double start_time;
    double end_time;
    long long papi_start[MAX_EVENTS];
    long long papi_end[MAX_EVENTS];
} FunctionCall;

static FunctionCall logs[MAX_FUNCS];
static int log_index = 0;
static int event_set = PAPI_NULL;
static int num_events = 0;
static int event_codes[MAX_EVENTS];
<<<<<<< HEAD
static char* event_names[MAX_EVENTS];  // char* instead of const char* for strdup

static __thread long long thread_start_values[MAX_EVENTS];
=======
static const char* event_names[MAX_EVENTS];

static __thread long long thread_start_values[MAX_EVENTS];  // per-thread start snapshot
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1
static __thread struct timespec thread_start_time;
static __thread const char* thread_func_name = NULL;

static int initialized = 0;

<<<<<<< HEAD
static double get_time_in_seconds(const struct timespec* ts) {
=======
static double get_time_in_seconds(struct timespec* ts) {
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1
    return ts->tv_sec + ts->tv_nsec / 1e9;
}

void init_papi() {
    if (initialized) return;

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI init failed!\n");
        exit(1);
    }

    if (PAPI_create_eventset(&event_set) != PAPI_OK) {
        fprintf(stderr, "Failed to create event set\n");
        exit(1);
    }

<<<<<<< HEAD
    const char* env = getenv("TRACE_PAPI_EVENTS");
    if (env && strlen(env) > 0) {
        fprintf(stderr, "TRACE_PAPI_EVENTS = %s\n", env);
        char* env_copy = strdup(env);
        char* token = strtok(env_copy, ",");
        while (token && num_events < MAX_EVENTS) {
            token[strcspn(token, "\n")] = 0; // remove newline
            event_names[num_events] = strdup(token);
            if (PAPI_event_name_to_code(event_names[num_events], &event_codes[num_events]) != PAPI_OK) {
                fprintf(stderr, "Invalid PAPI event name: %s\n", event_names[num_events]);
                exit(1);
            }
            num_events++;
            token = strtok(NULL, ",");
        }
        free(env_copy);
    } else {
        // fallback defaults
        event_names[0] = strdup("PAPI_TOT_INS");
        event_names[1] = strdup("PAPI_L1_DCM");
        num_events = 2;
        for (int i = 0; i < num_events; i++) {
            if (PAPI_event_name_to_code(event_names[i], &event_codes[i]) != PAPI_OK) {
                fprintf(stderr, "Error mapping default event: %s\n", event_names[i]);
                exit(1);
            }
        }
    }

    if (PAPI_add_events(event_set, event_codes, num_events) != PAPI_OK) {
        fprintf(stderr, "Failed to add events to event set\n");
=======
    // Customize these as needed or make dynamic later
    event_names[0] = "PAPI_TOT_INS";
    event_names[1] = "PAPI_L1_DCM";
    num_events = 2;

    if (PAPI_event_name_to_code(event_names[0], &event_codes[0]) != PAPI_OK ||
        PAPI_event_name_to_code(event_names[1], &event_codes[1]) != PAPI_OK) {
        fprintf(stderr, "Error mapping event names to codes\n");
        exit(1);
    }

    if (PAPI_add_events(event_set, event_codes, num_events) != PAPI_OK) {
        fprintf(stderr, "Failed to add events\n");
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1
        exit(1);
    }

    initialized = 1;
}

void runtime_function_entry(const char* func_name) {
    init_papi();

    thread_func_name = func_name;
    clock_gettime(CLOCK_MONOTONIC, &thread_start_time);
<<<<<<< HEAD

    if (PAPI_start(event_set) != PAPI_OK) {
        fprintf(stderr, "PAPI_start failed\n");
    }

    if (PAPI_read(event_set, thread_start_values) != PAPI_OK) {
        fprintf(stderr, "PAPI_read (entry) failed\n");
=======
    if (PAPI_start(event_set) != PAPI_OK) {
        fprintf(stderr, "PAPI_start failed\n");
    }
    if (PAPI_read(event_set, thread_start_values) != PAPI_OK) {
        fprintf(stderr, "PAPI_read start failed\n");
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1
    }
}

void runtime_function_exit(const char* func_name) {
    if (!initialized) return;

    long long end_values[MAX_EVENTS];
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    if (PAPI_read(event_set, end_values) != PAPI_OK) {
<<<<<<< HEAD
        fprintf(stderr, "PAPI_read (exit) failed\n");
    }

    if (PAPI_stop(event_set, NULL) != PAPI_OK) {
        fprintf(stderr, "PAPI_stop failed\n");
=======
        fprintf(stderr, "PAPI_read end failed\n");
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1
    }

    if (log_index < MAX_FUNCS) {
        FunctionCall* f = &logs[log_index++];
        snprintf(f->func_name, MAX_NAME_LEN, "%s", thread_func_name);
        f->start_time = get_time_in_seconds(&thread_start_time);
        f->end_time = get_time_in_seconds(&end_time);
<<<<<<< HEAD
        for (int i = 0; i < num_events; ++i) {
=======
        for (int i = 0; i < num_events; i++) {
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1
            f->papi_start[i] = thread_start_values[i];
            f->papi_end[i] = end_values[i];
        }
    }
<<<<<<< HEAD
=======

    if (PAPI_stop(event_set, NULL) != PAPI_OK) {
        fprintf(stderr, "PAPI_stop failed\n");
    }
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1
}

static void dump_logs() {
    FILE* f = fopen("function_metrics.csv", "w");
    if (!f) {
        perror("fopen");
        return;
    }

    fprintf(f, "function_name,start_timestamp,end_timestamp");
<<<<<<< HEAD
    for (int i = 0; i < num_events; ++i) {
=======
    for (int i = 0; i < num_events; i++) {
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1
        fprintf(f, ",%s", event_names[i]);
    }
    fprintf(f, "\n");

<<<<<<< HEAD
    for (int i = 0; i < log_index; ++i) {
        FunctionCall* fc = &logs[i];
        fprintf(f, "%s,%.9f,%.9f", fc->func_name, fc->start_time, fc->end_time);
        for (int j = 0; j < num_events; ++j) {
=======
    for (int i = 0; i < log_index; i++) {
        FunctionCall* fc = &logs[i];
        fprintf(f, "%s,%.9f,%.9f", fc->func_name, fc->start_time, fc->end_time);
        for (int j = 0; j < num_events; j++) {
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1
            fprintf(f, ",%lld", fc->papi_end[j] - fc->papi_start[j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}

__attribute__((destructor))
void shutdown_runtime() {
    dump_logs();
    PAPI_shutdown();
}
