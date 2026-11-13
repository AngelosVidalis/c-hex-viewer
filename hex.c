#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>

#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"

/*
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
*/

unsigned char max_args = 5;
unsigned char min_args = 3;
unsigned char mark_len = 4;
unsigned char flag_len = 2;
char mark_prefix_first = '0';
char mark_prefix_second = 'x';
char flag_prefix = '-';
char show_all_addr = 'r';

int count_hex_digits_math(long long n) {
    if (n == 0) return 1;
    n = llabs(n);

    // Change of base rule: log16(n) = log10(n) / log10(16)
    return (int)floor(log10(n) / log10(16)) + 1;
}

void print_hex(unsigned char *buf, size_t nread,
               size_t line_width, size_t *file_off,
               bool mark_enabled, bool flag_show_all_addr,
               bool flag_enabled, unsigned char mark_byte)
{
    // Print offset
    printf("%08zX    ", *file_off);

    if (flag_enabled && flag_show_all_addr)
    {
        for (size_t i = 0, byte_off = *file_off; i < nread; i++, byte_off++)
        {
            printf("%02zX ", byte_off);
        }
        printf("\n%12s", "");
    }

    unsigned char off_digits = count_hex_digits_math(*file_off);
    int space_loops = (line_width - nread)*(2 + 1);

    // Print hexadecimal values
    for (size_t i = 0; i < nread; i++) {

        if (mark_enabled && buf[i] == mark_byte) {
            printf(ANSI_COLOR_RED "%02X" ANSI_COLOR_RESET, buf[i]);
        } else {
            printf("%02X", buf[i]);
        }

        if (flag_enabled && flag_show_all_addr && off_digits > 2) {


            for (unsigned char i = 0; i < off_digits - 2; i++) {
                printf(" ");
            }
            space_loops = (line_width - nread)*(2 + 1 +(off_digits - 2));

        }
        printf(" ");
    }

    // Fill missing spaces
    if (nread < line_width) {
        for (size_t i = 0; i < space_loops; i++) {
            printf(" ");
        }
    }

    // Print ASCII representation
    printf(" | ");

    for (size_t i = 0; i < nread; i++)
    {
        char c = isprint(buf[i]) ? buf[i] : '.';

        if (mark_enabled && buf[i] == mark_byte)
        {
            printf(ANSI_COLOR_RED "%c" ANSI_COLOR_RESET, c);
        }
        else
        {
            printf("%c", c);
        }
    }

    *file_off += nread;

    printf("\n");

    if (flag_enabled && flag_show_all_addr)
    {
        printf("\n");
    }
}

void print_offset(unsigned char *buf, size_t line_width, size_t start, size_t nread){

    printf("%08zX    ", start);

    for (size_t i = 0, byte_off = start; i < nread; i++, byte_off++)
    {
        printf("%02zX ", byte_off);
    }

    printf("\n%12s", "");

    unsigned char off_digits = count_hex_digits_math(start);

    // Print hexadecimal values
    for (size_t i = 0; i < nread; i++) {
        printf("%02X ", buf[i]);

        for (unsigned char i = 0; i < off_digits - 2; i++) {
            printf(" ");
        }
    }

    int space_loops_recipe = (line_width - nread)*(2 + 1 +(off_digits - 2));

    // Fill missing spaces
    if (nread < line_width) {
        for (size_t i = 0; i < space_loops_recipe; i++) {
            printf(" ");
        }
    }

    // Print ASCII representation
    printf(" | ");

    for (size_t i = 0; i < nread; i++) {
        if (isprint(buf[i])) {
            printf("%c", buf[i]);
        } else {
            printf(".");
        }
    }

    printf("\n");
}

// -------- MAIN -----------

int main(int argc, char *argv[])
{
    // Check arguments
    if (argc > max_args || argc < min_args)
    {
        fprintf(stderr, "Invalid arguments in file %s at line # %d\n", __FILE__, __LINE__ - 2);
        return EXIT_FAILURE;
    }

    /* Check .bin extension - REMOVE TO ONLY OPEN .bin files
    char *ext = strstr(argv[1], ".bin");

    if (ext == NULL) {
        return EXIT_FAILURE;
    }
    */

    char *filename = argv[1];

    // Validate separator bytes argument
    char *width_endptr;
    long line_width = strtol(argv[2], &width_endptr, 10);

    if (*width_endptr != '\0' || line_width < 1 || line_width > 64 || argv[2] == width_endptr)
    {
        fprintf(stderr, "Invalid bytes-per-line value in file %s at line # %d\n", __FILE__, __LINE__ - 2);
        return EXIT_FAILURE;
    }

    // Validate marked byte argument
    unsigned char mark_byte;
    bool mark_enabled = false;

    if (argv[3] != NULL)
    {
        if (argv[3][0] != mark_prefix_first || argv[3][1] != mark_prefix_second || strlen(argv[3]) != mark_len)
        {
            fprintf(stderr, "Invalid marked byte format in file %s at line # %d\n", __FILE__, __LINE__ - 2);
            return EXIT_FAILURE;
        }

        char *mark_endptr;
        unsigned char mark_val = strtol(argv[3], &mark_endptr, 16);

        if (*mark_endptr != '\0' || argv[3] == mark_endptr)
        {
            fprintf(stderr, "Invalid hexadecimal byte in file %s at line # %d\n", __FILE__, __LINE__ - 2);
            return EXIT_FAILURE;
        }
        mark_byte = mark_val;
        mark_enabled = true;

    }

    // Validate flag
    bool flag_enabled = false;
    bool flag_show_all_addr = false;


    for (int i = 3; i < argc; i++)
    {

        printf("[DEBUG] Checking argument %d of %d\n", i, argc - 1);

        if ( !mark_enabled && ( argv[i] == NULL || argv[i][0] != flag_prefix || strlen(argv[i]) != flag_len ) )
        {
            fprintf(stderr, "Invalid command-line flag in file %s at line # %d\n", __FILE__, __LINE__ - 2);
            return EXIT_FAILURE;
        } else
        {
            if (argv[i][1] == show_all_addr)
            {
                flag_show_all_addr = true;
                flag_enabled = true;
            }
        }
    }

    // Initialize variables
    size_t file_off = 0;
    unsigned char buf[line_width];

    FILE *fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        fprintf(stderr, "fopen() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
        return EXIT_FAILURE;
    }

    // Read first block
    size_t nread = fread(buf, 1, line_width, fp);

    printf("\n");
    printf("========== HEX VIEWER ==========\n");
    printf("File           : %s\n", filename);
    printf("Bytes per line : %ld\n", line_width);
    printf("Highlight      : %s\n", mark_enabled ? "Enabled" : "Disabled");
    printf("Byte addresses : %s\n", flag_show_all_addr ? "Enabled" : "Disabled");
    printf("--------------------------------\n\n");

    // Read and display file contents
    while (nread > 0)
    {
        print_hex(buf, nread, line_width, &file_off, mark_enabled, flag_show_all_addr, flag_enabled, mark_byte);

        // Read next block
        nread = fread(buf, 1, line_width, fp);
    }

    printf("\n--------------------------------\n");
    printf("Total bytes read: %zu\n", file_off);
    printf("--------------------------------\n");

    // Close file
    fclose(fp);

    // Byte editing
    while (true) {

        char line[20];
        char offset_hex[9];
        char edit_op[2];
        char edit_hex[3];

        printf("\n");
        printf("========== BYTE EDITOR ==========\n");
        printf("Operations:\n");
        printf("  -s   Substitute byte\n");
        printf("  -a   Add byte\n");
        printf("  -d   Delete byte\n");
        printf("\nUsage: -f <OFFSET> -<OP> <HEX>\n");
        printf("Example: -f 0010 -s FF\n");
        printf("---------------------------------\n");
        printf("Enter command: ");

        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            break;
        }

        int count = sscanf(line, " -f %8s -%1s %2s", offset_hex, edit_op, edit_hex);

        if(count < 2)
        {
            fprintf(stderr, "sscanf() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
            return EXIT_FAILURE;
        };

        //printf("Byte offset: %s\n", offset_hex);
        //printf("Edit operation: %s\n", edit_op);
        //printf("Edit byte: %s\n", edit_hex);

        bool invalid_input = false;
        char *offset_endptr;
        char *edit_endptr;

        long target_off = strtol(offset_hex, &offset_endptr, 16);
        unsigned char edit_byte;

        if ((*edit_op != 's' && *edit_op != 'd' && *edit_op != 'a')
            || *offset_endptr != '\0' || offset_hex == offset_endptr )
        {
            invalid_input = true;
        }

        if(count == 3)
        {
            edit_byte = strtol(edit_hex, &edit_endptr, 16);
            if( *edit_endptr != '\0' || edit_hex == edit_endptr )
            {
                invalid_input = true;
            }
        }


        while (invalid_input)
        {
            fprintf(stderr, "Invalid edit command in file %s at line # %d\n", __FILE__, __LINE__ - 2);
            printf("Try again: ");

            fgets(line, sizeof(line), stdin);
            count = sscanf(line, " -f %8s -%1s %2s", offset_hex, edit_op, edit_hex);

            target_off = strtol(offset_hex, &offset_endptr, 16);

            if ((*edit_op != 's' && *edit_op != 'd' && *edit_op != 'a')
            || *offset_endptr != '\0' || offset_hex == offset_endptr )
            {
                invalid_input = true;
            }

            if  (count == 3)
            {

                edit_byte = strtol(edit_hex, &edit_endptr, 16);
                if( *edit_endptr != '\0' || edit_hex == edit_endptr )
                {
                    invalid_input = true;
                }
            }
        }

        if ( *edit_op == 'd')

        {
            FILE *edit_fp = fopen(filename, "rb+");

            if (edit_fp == NULL)
            {
                fprintf(stderr, "fopen() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
                return EXIT_FAILURE;
            }

            if (fseek(edit_fp, -1, SEEK_END) != 0) // fseek(edit_fp, 0, SEEK_END) get's  offset from EOF
            {
                fprintf(stderr, "fseek() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
                fclose(edit_fp);
                return EXIT_FAILURE;
            }

            if( ftell(edit_fp) == target_off)
            {
                long flen = ftell(edit_fp);
                int fd = fileno(fp);

                if( ftruncate(fd, flen-1) == -1 )
                {
                    fprintf(stderr, "ftruncate() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
                    fclose(edit_fp);
                    return EXIT_FAILURE;
                }

            }else{

                unsigned char num_chunks = (ftell(edit_fp) - target_off+1) / line_width; // +1 since their subtraction gives us the # of chars between them, without including the ending char.
                int temp_off = target_off;

                for ( int i = 0; i < num_chunks+1; i++)
                {

                    if( fseek(edit_fp, temp_off+1, SEEK_SET))
                    {
                        fprintf(stderr, "fseek() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
                        fclose(edit_fp);
                        return EXIT_FAILURE;
                    };

                    size_t n_read = fread(buf, sizeof(unsigned char), line_width, edit_fp);

                    if( fseek(edit_fp, temp_off, SEEK_SET))
                    {
                        fprintf(stderr, "fseek() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
                        fclose(edit_fp);
                        return EXIT_FAILURE;
                    };

                    fwrite(buf, sizeof(unsigned char), n_read, edit_fp);
                    temp_off += n_read;
                }

                long flen = ftell(edit_fp);
                int fd = fileno(fp);

                if( ftruncate(fd, flen) == -1 )
                {
                    fprintf(stderr, "ftruncate() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
                    fclose(edit_fp);
                    return EXIT_FAILURE;
                }

            }

            long start = target_off - (target_off % line_width); // to read beginning address
            fseek(edit_fp, start, SEEK_SET);
            size_t nread = fread(buf, 1, line_width, edit_fp);

            printf("\n---------- FILE CONTENT ----------\n");
            print_offset(buf, line_width, start, nread);
            printf("----------------------------------\n");

            fclose(edit_fp);

        }


        if (*edit_op == 's')
        {
            FILE *edit_fp = fopen(filename, "rb+");

            if (edit_fp == NULL)
            {
                fprintf(stderr, "fopen() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
                return EXIT_FAILURE;
            }

            fseek(edit_fp, target_off, SEEK_SET);
            unsigned char raw_byte = (unsigned char)edit_byte;

            size_t r1;
            if ( (r1 = fwrite(&raw_byte, sizeof(unsigned char), 1, edit_fp)) < 1)
            {
                fprintf(stderr, "fwrite() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
                fclose(edit_fp);
                return EXIT_FAILURE;
            }

            printf("\n[WRITE] r1: 0x%08lX | New byte: 0x%02X\n",target_off, edit_byte);

            long start = target_off - (target_off % line_width); // to read beginning address
            fseek(edit_fp, start, SEEK_SET);
            size_t nread = fread(buf, 1, line_width, edit_fp);

            printf("\n---------- FILE CONTENT ----------\n");
            print_offset(buf, line_width, start, nread);
            printf("----------------------------------\n");

            fclose(edit_fp);

        }

        if ( *edit_op == 'a'){

            FILE *edit_fp = fopen(filename, "ab+");


            if (edit_fp == NULL)
            {
                fprintf(stderr, "fopen() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
                return EXIT_FAILURE;
            }

            unsigned char raw_byte = (unsigned char)edit_byte;
            fseek(edit_fp, -1, SEEK_END);

            size_t r1;
            if ( (r1 = fwrite(&raw_byte, sizeof(unsigned char), 1, edit_fp)) < 1)
            {
                fprintf(stderr, "fwrite() failed in file %s at line # %d\n", __FILE__, __LINE__ - 2);
                fclose(edit_fp);
                return EXIT_FAILURE;
            }

            printf("\n[WRITE] r1: 0x%08lX | New byte: 0x%02X\n",target_off, edit_byte);


            long start = ftell(edit_fp) - ( ftell(edit_fp) % line_width); // to read beginning address
            fseek(edit_fp, start, SEEK_SET);
            size_t nread = fread(buf, 1, line_width, edit_fp);

            printf("\n---------- FILE CONTENT ----------\n");
            print_offset(buf, line_width, start, nread);
            printf("----------------------------------\n");

            fclose(edit_fp);

        }

    }
}

