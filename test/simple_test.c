<<<<<<< HEAD
#include "/home/bala/cd-lab/runtime/runtime.h"

int square(int n) {
    

    int sum;
    if(n>2)
     return 0;
    for(int i=0;i<2000;i++)
     sum+=0;
    return sum;


    
     
}

int main() {
    int r = square(5);
=======
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
>>>>>>> ab548c4e0b28832d89d91d0f5285c3b405ba06a1
    return 0;
}
