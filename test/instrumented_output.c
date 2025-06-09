int square(int x) {runtime_function_entry("square");

    return x * x;
runtime_function_exit("square");
}

int main() {
    int r = square(5);
    return 0;
}
