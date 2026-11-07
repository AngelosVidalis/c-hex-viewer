#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"

/*
#define ANSI_COLOR_GREEN   "\x1b[32m"  // Green
#define ANSI_COLOR_YELLOW  "\x1b[33m"  // Yellow
#define ANSI_COLOR_BLUE    "\x1b[34m"  // Blue
#define ANSI_COLOR_CYAN    "\x1b[36m"  // Cyan
#define ANSI_COLOR_RESET   "\x1b[0m"   // Reset
*/

short int MAX_ARG_NUM = 4;
short int MIN_ARG_NUM = 3;

short int MARKED_BYTE_MAX_LEN = 4;
char MARKED_BYTE_FIRST_CHAR = '0';
char MARKED_BYTE_SECOND_CHAR = 'x';






int main(int argc, char *argv[])
{
    // Check arguments
    if (argc > MAX_ARG_NUM || argc < MIN_ARG_NUM  )
    {
        printf("Usage: %s <filename> <separator_bytes> <marked-byte>\n", argv[0]);
        return EXIT_FAILURE;
    }


    /* Check .bin extension - REMOVE TO ONLY OPEN .bin files
    char *extension = strstr(argv[1], ".bin");

    if (extension == NULL) {
        return 1;
    }
    */


    char *filename = argv[1];

    // Validate separator bytes argument
    char *endptr;
    long bytes_per_line = strtol(argv[2], &endptr, 10);

    if (*endptr != '\0' || bytes_per_line < 1 || bytes_per_line > 64 || argv[2] == endptr)
    {
        printf("<bytes>: integer between 1 and 64\n");
        return EXIT_FAILURE;
    }

    // Validate marked byte argument


    unsigned short int third_arg; // Since marke byte is always 2 bytes, unsgined short int is enough on most modern systems
    bool is_third_arg_valid = false; // Make sure


    if ( argc == MAX_ARG_NUM && argv[3] != NULL ) {

        if( argv[3][0] != MARKED_BYTE_FIRST_CHAR || argv[3][1] != MARKED_BYTE_SECOND_CHAR || strlen(argv[3]) != MARKED_BYTE_MAX_LEN )
        {
            //printf("%d",  argv[3][0] != MARKED_BYTE_FIRST_CHAR );
            //printf("%d", argv[3][1] != MARKED_BYTE_SECOND_CHAR);
            //printf("%d", strlen(argv[3]) != MARKED_BYTE_MAX_LEN);


            printf("<marked_byte>: 0xHEX e.g. 0xFF");
            return EXIT_FAILURE;
        }

        char *endptr;
        long int_val = strtol(argv[3], &endptr, 16); // '0xFF' string to the hex int value. S

        if (*endptr != '\0' || argv[3] == endptr) {

            printf("<marked_byte>: 0xHexNum e.g. 0xFF");
            return EXIT_FAILURE;
        }

        third_arg = int_val;
        is_third_arg_valid = true;

    }











    // Initialize variables
    size_t offset = 0;
    unsigned char buffer[bytes_per_line];

    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        perror("fopen");
        return 1;
    }


    // Read first block
    size_t bytes_read = fread(buffer, 1, bytes_per_line, file);

    printf("\n");


    // Read and display file contents
    while (bytes_read > 0)
    {

        // Print offset
        printf("%08zX    ", offset);
        offset += bytes_read;


        // Print hexadecimal values
        for (size_t i = 0; i < bytes_read; i++) {


            if( argc == MAX_ARG_NUM && is_third_arg_valid && buffer[i] == third_arg ) {
                printf(ANSI_COLOR_RED "%02X " ANSI_COLOR_RESET, buffer[i]);

            }else
            {
            printf("%02X ", buffer[i]);
            }
        }


        // Fill missing spaces
        if (bytes_read < bytes_per_line)
        {

            for (size_t i = 0; i < bytes_per_line - bytes_read; i++)
            {
                printf("   ");
            }

        }


        // Print ASCII representation
        printf(" | ");

        for (size_t i = 0; i < bytes_read; i++)
        {

            if (isprint(buffer[i]))
            {
                printf("%c", buffer[i]);
            } else
            {
                printf(".");
            }

        }

        printf("\n");


        // Read next block
        bytes_read = fread(buffer, 1, bytes_per_line, file);
    }


    // Close file
    fclose(file);

    return EXIT_SUCCESS;
}