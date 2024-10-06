#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Structs for BMP headers
#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

// Function to check if the file already exists
bool fileExists(const char *filename) {
    FILE *file = fopen(filename, "rb");  // Try to open file in read mode
    if (file) {
        fclose(file);
        return true;  // File exists
    }
    return false;  // File does not exist
}

// Function to generate BMP image with 32 bits per pixel (RGBA)
void generateBMP32(const char *filename, int width, int height) {
    if (fileExists(filename)) {
        printf("File '%s' already exists. Skipping creation.\n", filename);
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Could not open file for writing.\n");
        return;
    }

    // Define headers
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    int fileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (width * 4) * height;

    // BMP file header
    fileHeader.bfType = 0x4D42;  // 'BM' in little-endian
    fileHeader.bfSize = fileSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // BMP info header (32-bit BMP, RGBA)
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 32;  // 32 bits per pixel (RGBA)
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = (width * 4) * height;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    // Write headers to file
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

    // Generate pixel data (RGBA)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Pixel data (B, G, R, A)
            uint8_t blue = (uint8_t)(255 * x / width);  // Blue gradient
            uint8_t green = (uint8_t)(255 * y / height);  // Green gradient
            uint8_t red = 0;  // Constant red value
            uint8_t alpha = 255;  // Full opacity

            // Write pixel (4 bytes: B, G, R, A)
            fwrite(&blue, 1, 1, file);
            fwrite(&green, 1, 1, file);
            fwrite(&red, 1, 1, file);
            fwrite(&alpha, 1, 1, file);
        }
    }

    fclose(file);
    printf("32 bpp BMP file created successfully: %s\n", filename);
}

