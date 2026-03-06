//----- Handle includes -----
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// ----- Define the dynamically handled char array
typedef struct {
    uint8_t* string_start;
    uint32_t capacity;
    uint32_t length;

} chr_array;

// ----- Main function -----
int32_t main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Error in msfInterpreter: wrong number of input parameters. Only the code should be provided.\n");
        return 1;

    }

    chr_array code = {
        .capacity = 1,
        .length = 0,
        .string_start = calloc(code.capacity, sizeof(uint8_t)),
    };

    if (code.string_start == NULL) {
        perror("Error in msfInterpreter: dynamic memory allocation failed.");
        return 2;
    }

    uint8_t memcell = 0;
    char* input_arg = argv[1];

    uint32_t code_pointer = 0;
    uint32_t str_pointer = 0;
    while (input_arg[code_pointer] != '\0') {
        if (input_arg[code_pointer] == '+') {
           memcell++;

        } else if (input_arg[code_pointer] == '.') {
            //Check and grow output string capacity as needed
            if (code.capacity == code.length) {
                uint32_t new_size = code.capacity * 2;
                uint8_t* new_adrs = realloc(code.string_start, sizeof(uint8_t) * new_size);
                if (new_adrs == NULL) {
                    printf("Error in msfInterpreter: dynamic memory allocation failed\n");
                    return 2;
                } else {
                    code.string_start = new_adrs;

                }

                code.capacity = new_size;

            }

            code.string_start[str_pointer] = memcell;
            code.length++;
            str_pointer++;

        } else {
            ;

        }

        code_pointer++;
    }

    for (uint32_t i = 0; i < code.length; i++) {
        printf("%c", code.string_start[i]);

    }


    free(code.string_start);

    return EXIT_SUCCESS;

}
