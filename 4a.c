#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global definitions
#define WIDTH 512
#define HEIGHT 512

// Function prototypes
int readPGMText(const char *filename, unsigned char *pixels, int width, int height);
int writePGMText(const char *filename, unsigned char *pixels, int width, int height);
int writePGMBinary(const char *filename, unsigned char *pixels, int width, int height);
void embedLSB(unsigned char *coverPixels, unsigned char *secretPixels, int width, int height);
void extractLSB(unsigned char *coverPixels, unsigned char *outputPixels, int width, int height);

int main() {
    const char cover_image[] = "baboon.pgm";
    const char secret_image[] = "farm.pgm";
    const char stego_image[] = "stego_image_bin.pgm";
    const char extracted_secret[] = "extracted_secret.pgm";

    unsigned char *coverPixels = NULL;
    unsigned char *secretPixels = NULL;
    unsigned char *outputPixels = NULL;

    // Dynamically allocate memory
    coverPixels = (unsigned char *)malloc(WIDTH * HEIGHT);
    if (!coverPixels) {
        fprintf(stderr, "Error: Memory allocation failed for coverPixels.\n");
        return 1;
    }

    secretPixels = (unsigned char *)malloc(WIDTH * HEIGHT);
    if (!secretPixels) {
        fprintf(stderr, "Error: Memory allocation failed for secretPixels.\n");
        free(coverPixels);
        return 1;
    }

    outputPixels = (unsigned char *)malloc(WIDTH * HEIGHT);
    if (!outputPixels) {
        fprintf(stderr, "Error: Memory allocation failed for outputPixels.\n");
        free(coverPixels);
        free(secretPixels);
        return 1;
    }

    // Read cover image
    if (readPGMText(cover_image, coverPixels, WIDTH, HEIGHT)) {
        fprintf(stderr, "Error: Unable to read cover image.\n");
        free(coverPixels);
        free(secretPixels);
        free(outputPixels);
        return 1;
    }

    // Read secret image
    if (readPGMText(secret_image, secretPixels, WIDTH, HEIGHT)) {
        fprintf(stderr, "Error: Unable to read secret image.\n");
        free(coverPixels);
        free(secretPixels);
        free(outputPixels);
        return 1;
    }

    // Embed secret image into cover image
    embedLSB(coverPixels, secretPixels, WIDTH, HEIGHT);

    // Write stego image in binary format
    if (writePGMBinary(stego_image, coverPixels, WIDTH, HEIGHT)) {
        fprintf(stderr, "Error: Unable to write stego image.\n");
        free(coverPixels);
        free(secretPixels);
        free(outputPixels);
        return 1;
    }

    // Extract secret image from stego image
    extractLSB(coverPixels, outputPixels, WIDTH, HEIGHT);

    // Write extracted secret image in text format
    if (writePGMText(extracted_secret, outputPixels, WIDTH, HEIGHT)) {
        fprintf(stderr, "Error: Unable to write extracted secret image.\n");
    }

    // Free dynamically allocated memory
    free(coverPixels);
    free(secretPixels);
    free(outputPixels);

    return 0;
}

// Function to read an ASCII PGM file
int readPGMText(const char *filename, unsigned char *pixels, int width, int height) {
    FILE *file = fopen(filename, "r");
    if (!file) return 1;

    char buffer[16];
    if (!fgets(buffer, sizeof(buffer), file) || buffer[0] != 'P' || buffer[1] != '2') {
        fclose(file);
        return 1;
    }

    // Skip comments
    do {
        if (!fgets(buffer, sizeof(buffer), file)) {
            fclose(file);
            return 1;
        }
    } while (buffer[0] == '#');

    int w, h, maxVal;
    if (sscanf(buffer, "%d %d", &w, &h) != 2 || w != width || h != height || fscanf(file, "%d", &maxVal) != 1 || maxVal != 255) {
        fclose(file);
        return 1;
    }

    for (int i = 0; i < width * height; ++i) {
        if (fscanf(file, "%hhu", &pixels[i]) != 1) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

// Function to write an ASCII PGM file
int writePGMText(const char *filename, unsigned char *pixels, int width, int height) {
    FILE *file = fopen(filename, "w");
    if (!file) return 1;

    fprintf(file, "P2\n# Created by stego_lsb.c\n%d %d\n255\n", width, height);

    for (int i = 0; i < width * height; ++i) {
        fprintf(file, "%d\n", pixels[i]);
    }

    fclose(file);
    return 0;
}

// Function to write a binary PGM file
int writePGMBinary(const char *filename, unsigned char *pixels, int width, int height) {
    FILE *file = fopen(filename, "wb");
    if (!file) return 1;

    fprintf(file, "P5\n# Created by stego_lsb.c\n%d %d\n255\n", width, height);
    fwrite(pixels, sizeof(unsigned char), width * height, file);

    fclose(file);
    return 0;
}

// Function to embed secret image into cover image
void embedLSB(unsigned char *coverPixels, unsigned char *secretPixels, int width, int height) {
    for (int i = 0; i < width * height; ++i) {
        *(coverPixels + i) = (*(coverPixels + i) & 0xF0) | (*(secretPixels + i) >> 4);
    }
}

// Function to extract secret image from stego image
void extractLSB(unsigned char *coverPixels, unsigned char *outputPixels, int width, int height) {
    for (int i = 0; i < width * height; ++i) {
        *(outputPixels + i) = (*(coverPixels + i) & 0x0F) << 4;
    }
}
