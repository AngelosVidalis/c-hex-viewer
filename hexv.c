#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    // Check arguments
    if (argc != 3) {
        printf("Usage: %s <filename> <bytes>\n", argv[0]);
        return 1;
    }


    // Check .bin extension
    char *extension = strstr(argv[1], ".bin");

    if (extension == NULL) {
        return 1;
    }

    char *filename = argv[1];


    // Validate bytes argument
    char *endptr;
    long bytes_per_line = strtol(argv[2], &endptr, 10);

    if (*endptr != '\0' || bytes_per_line < 1 || bytes_per_line > 64 || argv[2] == endptr) {
        printf("<bytes>: integer between 1 and 64\n");
        return 1;
    }


    // Initialize variables
    size_t offset = 0;
    unsigned char buffer[bytes_per_line];

    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        perror("fopen");
        return 1;
    }


    // Read first block
    size_t bytes_read = fread(buffer, 1, bytes_per_line, file);

    printf("LOCATION                HEX                       ASCII\n");


    // Read and display file contents
    while (bytes_read > 0) {

        // Print offset
        printf("%08zX    ", offset);
        offset += bytes_read;


        // Print hexadecimal values
        for (size_t i = 0; i < bytes_read; i++) {
            printf("%02X ", buffer[i]);
        }


        // Fill missing spaces
        if (bytes_read < bytes_per_line) {

            for (size_t i = 0; i < bytes_per_line - bytes_read; i++) {
                printf("   ");
            }

        }


        // Print ASCII representation
        printf(" | ");

        for (size_t i = 0; i < bytes_read; i++) {

            if (isprint(buffer[i])) {
                printf("%c", buffer[i]);
            } else {
                printf(".");
            }

        }

        printf("\n");


        // Read next block
        bytes_read = fread(buffer, 1, bytes_per_line, file);
    }


    // Close file
    fclose(file);

    return 0;
}