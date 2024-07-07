#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef int (*add_func_t)(int, int);

int main() {
    void *handle;
    add_func_t add;
    char *error;

    // Load the shared library
    handle = dlopen("./build/libexample.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    // Clear any existing error
    dlerror();

    // Get the symbol (function) from the shared library
    *(void **)(&add) = dlsym(handle, "add");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    // Use the function
    int result = add(3, 4);
    printf("Result of add(3, 4): %d\n", result);

    // Close the shared library
    dlclose(handle);

    return 0;
}
