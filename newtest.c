#include "/home/bala/cd-lab/runtime/runtime.h"

#include <stdio.h>
#include <stdlib.h>

void compute(int n) {
    int* arr = (int*)malloc(n * sizeof(int));
    if (!arr) return;

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

