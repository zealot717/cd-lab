#include "/home/bala/cd-lab/runtime/runtime.h"

int square(int n) {runtime_function_entry("square");

    

    int sum;
    if(n>2)
     runtime_function_exit("square");
     return 0;
    for(int i=0;i<1000;i++)
     sum+=0;
    runtime_function_exit("square");
    return sum;


    
     
}

int main() {
    int r = square(5);
    return 0;
}
