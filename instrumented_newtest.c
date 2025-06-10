#include <stdlib.h>
static void __init_papi_env() __attribute__((constructor));
static void __init_papi_env() {
  setenv("TRACE_PAPI_EVENTS", "PAPI_L1_DCM,PAPI_TOT_INS", 1);
}

#include "/home/bala/cd-lab/runtime/runtime.h"

#include <stdio.h>
#include <stdlib.h>

void compute(int n) {runtime_function_entry("compute");

    int* arr = (int*)malloc(n * sizeof(int));
    if (!arr) runtime_function_exit("compute");
    return;

    for (int i = 0; i < n; i++) {
        arr[i] = i * i;
    }

    long long sum = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] % 2 == 0)
            sum += arr[i];
        else
            sum -= arr[i];
    }

    printf("Final sum: %lld\n", sum);
    free(arr);
}

int main() {
    compute(10000);  // triggers enough memory access + branches
    return 0;
}

