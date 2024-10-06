#include <stdio.h>
#include <time.h>
#include "create_bmp_image.h"
#include "tcp_client.h"

int main() {
    // Create a 256x256 BMP image
    generateBMP32("test.bmp", 2048, 2048);

    clock_t start, end;
    double cpu_time_used;
    char filename[64];

        // Record the starting time
    start = clock();
    for(int i=0;i < 100; i++) {
        sprintf(filename, "build/test_tcp/%03d.bmp", i);
        tcp_client(filename);
    }
    end = clock();

    // Calculate the time taken by the function in seconds
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("The function took %f seconds to execute.\n", cpu_time_used);
    return 0;
}