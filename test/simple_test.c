#include "runtime.h"

void foo() {
    runtime_function_entry("foo");
    for (int i = 0; i < 1000000; i++) {
        int x = i * i;
    }
    runtime_function_exit("foo");
}

int main() {
    foo();
    return 0;
}
