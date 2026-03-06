
// ----- Necessary includes ----- //
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <regex.h>
#include <assert.h>
#include "cDataTypes.h"
#include "cStack.h"
#include "cHashTable.h"


//----- Struct and enum definitions ----- //
typedef enum FlagType {
    FLAG_INPUT_CODE,
    FLAG_INPUT_TAPE,
    FLAG_OUTPUT,
    FLAG_UNKNOWN

} FlagType;


typedef struct FlagEntry {
    const char* short_flag;
    const char* long_flag;
    FlagType type;

} FlagEntry;

const FlagEntry FlagLookup[] = {
    {"-c", "--code", FLAG_INPUT_CODE},
    {"-t", "--tape", FLAG_INPUT_TAPE},
    {"-o", "--output", FLAG_OUTPUT},

};

const size_t FlagCount = (sizeof(FlagLookup) / sizeof(FlagLookup[0]));

typedef struct CharBuffer {
    char* buffer;
    uint64_t len;

} CharBuffer;

typedef struct ReadBuffer {
    DataType type;
    void* buffer;
    uint64_t len;

} ReadBuffer;



// ----- Function prototypes ----- //
// Public function prototypes //
FlagType parseFlag(const char* arg);
Status readFile(CharBuffer* target_buffer, const char* filename);
Status readBinary(ReadBuffer* target_buffer, DataType type, const char* filename);
Status writeFile(CharBuffer* target_buffer, const char* filename);
Status clearCharBuffer(CharBuffer* target_buffer);
Status clearReadBuffer(ReadBuffer* target_buffer);
bool hasExtension(const char* arg, const char* extension);
Status translateCharTape(int32_t num_tape[], CharBuffer* tape_buffer);
Status translateIntTape(int32_t num_tape[], uint64_t num_tape_len, char* char_tape, uint64_t char_tape_len);
Status mapLoops(Stack* stack, HashTable* hash_table, CharBuffer* code);


// Private function prototypes //
uint32_t getFileSize(FILE* file);

int32_t main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("sfInterpreter: error, no arguments were provided.\n");
        return EXIT_FAILURE;
    }

    /* ----- Handle main function argument parsing ----- */
    //Variable definitions
    bool matchRet_c;
    bool matchRet_t;
    bool matchRet_o;


    char* code_arg = NULL;
    char* tape_arg = NULL;

    const char* code_path = NULL;
    const char* tape_path = NULL;
    const char* output_path = NULL;


    CharBuffer code;
    CharBuffer tape;

    //ReadBuffer code;
    //ReadBuffer tape;

    Status readRet_c;
    Status readRet_t;
    Status writeRet_o;
    Status clearRet;

    //Parse all input args
    int32_t arg_iter = 1;
    while (arg_iter < argc) {
        if (argv[arg_iter][0] == '-') {
            FlagType type = parseFlag(argv[arg_iter]);

            switch (type) {
                case FLAG_INPUT_CODE:
                    if (arg_iter + 1 >= argc) {
                        printf("sfInterpreter: code argument is missing after the last flag provided [%d]:[%s].\n", arg_iter, argv[arg_iter]);
                        return EXIT_FAILURE;
                    }
                    code_arg = argv[arg_iter + 1];
                    arg_iter++;
                    break;
                case FLAG_INPUT_TAPE:
                    if (arg_iter + 1 >= argc) {
                        printf("sfInterpreter: tape argument is missing after the last flag provided [%d]:[%s].\n", arg_iter, argv[arg_iter]);
                        return EXIT_FAILURE;
                    }
                    tape_arg = argv[arg_iter + 1];
                    arg_iter++;
                    break;
                case FLAG_OUTPUT:
                    if (arg_iter + 1 >= argc) {
                        printf("sfInterpreter: output argument is missing after the last flag provided [%d]:[%s].\n", arg_iter, argv[arg_iter]);
                        return EXIT_FAILURE;
                    }
                    matchRet_o = hasExtension(argv[arg_iter + 1], ".txt");
                    if (matchRet_o == false) {
                        printf("sfInterpreter: the provided argument [%d]:[%s] is invalid.\nAccepted extensions: output file -> .txt\n", arg_iter + 1, argv[arg_iter + 1]);
                    }
                    output_path = argv[arg_iter + 1];
                    arg_iter++;
                    break;
                default:
                    printf("sfInterpreter: the provided argument [%d]:[%s] is invalid.\nAccepted extensions:\ncode file -> .sf\ntape file -> .sft\noutput file -> .txt\n", arg_iter, argv[arg_iter]);
                    return EXIT_FAILURE;

            }

        } else {
            matchRet_c = hasExtension(argv[arg_iter], ".sf");
            if (matchRet_c == false) {
                matchRet_t = hasExtension(argv[arg_iter], ".sft");
                if (matchRet_t == false) {
                    printf("sfInterpreter: the provided argument [%d]:[%s] is invalid.\nAccepted extensions:\ncode file -> .sf\ntape file -> .sft\n", arg_iter, argv[arg_iter]);
                    return EXIT_FAILURE;

                } else {
                    tape_path = argv[arg_iter];

                }

            } else {
                code_path = argv[arg_iter];

            }

        }

        arg_iter++;
    }


    //input argument error handling
    if (code_path != NULL && code_arg != NULL) {
        printf("sfInterpreter: two source codes were provided: %s, %s\n", code_path, code_arg);
        return EXIT_FAILURE;

    } else if (code_path == NULL && code_arg == NULL) {
        printf("sfInterpreter: no source code was provided\n");
        return EXIT_FAILURE;

    }

    if (tape_path != NULL && tape_arg != NULL) {
        printf("sfInterpreter: two source tapes were provided: %s, %s\n", tape_path, tape_arg);
        return EXIT_FAILURE;

    } else if (tape_path == NULL && tape_arg == NULL) {
        printf("sfInterpreter: no source tape was provided\n");
        return EXIT_FAILURE;

    }

    //code and tape file and inline string reading and allocation
    if (code_path != NULL) {
        readRet_c = readFile(&code, code_path);
        if (readRet_c == STAT_ERROR) {
            printf("sfInterpreter: error, during reading target file.\n");
            return EXIT_FAILURE;

        }
        assert(code.buffer != NULL);

    } else {
        code.buffer = deepCopyString(code_arg);
        if (code.buffer == NULL) {
            printf("sfInterpreter: error, deepCopy of the provided code string failed.\n");
            return EXIT_FAILURE;

        }
        code.len = strlen(code.buffer) + 1;

    }

    if (tape_path != NULL) {
        readRet_t = readFile(&tape, tape_path);
        if (readRet_t == STAT_ERROR) {
            printf("sfInterpreter: error, during reading target file.\n");
            return EXIT_FAILURE;

        }
        assert(tape.buffer != NULL);

    } else {
        tape.buffer = deepCopyString(tape_arg);
        if (tape.buffer == NULL) {
            printf("sfInterpreter: error, deepCopy of the provided tape string failed.\n");
            return EXIT_FAILURE;

        }
        tape.len = strlen(tape.buffer) + 1;

    }


    /* ----- Handle tape conversion and code pre-processing ----- */

    //Tape translation to a numeric form for operations
    if (tape.len-1 < 0) {
        printf("sfInterpreter: error, numeric tape length is less than 0, the tape was empty.\n");
        return EXIT_FAILURE;
    }
    uint64_t numTapeLen = tape.len-1;
    int32_t numTape[numTapeLen];
    Status charTransRet = translateCharTape(numTape, &tape);
    if (charTransRet == STAT_FAILURE) {
        printf("sfInterpreter: error, translating the tape string to a numeric form failed.\n");
        return EXIT_FAILURE;

    }

    //Allocate the necessary stack and hash map for loop mapping
    Stack lStack = initStack();
    HashTable lMap = initHtable(code.len * 2); //NOTE: must be de-allocated!
    Status mapRet = mapLoops(&lStack, &lMap, &code);
    if (mapRet == STAT_ERROR) {
        printf("sfInterpreter: error, mapping loops in the code failed.\n");
        return EXIT_FAILURE;

    }


    /* ----- Main code parsing/execution loop ----- */

    uint64_t charTapeLen = tape.len;
    char char_tape[tape.len];
    CharBuffer output_tape;
    Value loopkupVal;
    bool task_failed = false;

    uint64_t code_iter = 0;
    int64_t tape_pointer = 0;
    while (code_iter < code.len) {
        char instruction = code.buffer[code_iter];

        if (instruction == '>') {
            tape_pointer++;

            if (tape_pointer >= (int64_t) numTapeLen) {
                Status intTransRet = translateIntTape(numTape, numTapeLen, char_tape, charTapeLen);
                if (intTransRet == STAT_ERROR) {
                    printf("sfInterpreter: error, output tape translation failed.\n");
                    return EXIT_FAILURE;

                }

                if (output_path != NULL) {
                    printf("sfInterpreter: tape overflow detected.\nOutput tape state was saved into the output file.\n");
                    task_failed = true;
                    goto write_output;

                } else {
                    printf("sfInterpreter: tape overflow detected.\nOutput tape state: %s\n", char_tape);
                    return EXIT_FAILURE;

                }

            }

        } else if (instruction == '<') {
            tape_pointer--;

            if (tape_pointer < 0) {
                Status intTransRet = translateIntTape(numTape, numTapeLen, char_tape, charTapeLen);
                if (intTransRet == STAT_ERROR) {
                    printf("sfInterpreter: error, output tape translation failed.\n");
                    return EXIT_FAILURE;

                }

                if (output_path != NULL) {
                    printf("sfInterpreter: tape underflow detected.\nOutput tape state was saved into the output file.\n");
                    task_failed = true;
                    goto write_output;

                } else {
                    printf("sfInterpreter: tape underflow detected.\nOutput tape state: %s\n", char_tape);
                    return EXIT_FAILURE;

                }

            }

        } else if (instruction == '*') {
            numTape[tape_pointer] ^= 1;

        } else if (instruction == '[' && numTape[tape_pointer] == 0) {
                loopkupVal = getData(&lMap, makeKey_u64(code_iter));
                if (loopkupVal.type == EMPTY) {
                    printf("sfInterpreter: error, failed to retrieve loop jump point.\n");
                    return EXIT_FAILURE;

                }

                code_iter = loopkupVal.as.u64;

        } else if (instruction == ']' && numTape[tape_pointer] == 1) {
            loopkupVal = getData(&lMap, makeKey_u64(code_iter));
            if (loopkupVal.type == EMPTY) {
                printf("sfInterpreter: error, failed to retrieve loop jump point.\n");
                return EXIT_FAILURE;

            }

            code_iter = loopkupVal.as.u64;

        }



        code_iter++;
    }

    Status intTransRet = translateIntTape(numTape, numTapeLen, char_tape, charTapeLen);
    if (intTransRet == STAT_ERROR) {
        printf("sfInterpreter: error, output tape translation failed.\n");
        return EXIT_FAILURE;

    }

    write_output:
        if (output_path != NULL) {
            output_tape.buffer = char_tape;
            output_tape.len = charTapeLen;
            writeRet_o = writeFile(&output_tape, output_path);
            if (writeRet_o == STAT_FAILURE) {
                printf("sfInterpreter: error, failed to write to output path.\n");
                return EXIT_FAILURE;

            }

        } else {
            printf("sfInterpreter: Process completed.\nOutput tape state: %s\n", char_tape);
            //printf("%s\n", char_tape);


        }


    /* ----- Cleaning up after main parsing/execution loop ----- */

    Status clearStackRet = clearStack(&lStack);
    if (clearStackRet == STAT_ERROR) {
        printf("sfInterpreter: error, failed to clear the stack.\n");
    }

    Status freeRet = freeHtable(&lMap);
    if (freeRet == STAT_ERROR) {
        printf("sfInterpreter: error, failed to free the hash table.\n");
    }

    clearRet = clearCharBuffer(&code);
    if (clearRet == STAT_ERROR) {
        printf("sfInterpreter: error, failed to clear the code buffer.\n");
    }

    clearRet = clearCharBuffer(&tape);
    if (clearRet == STAT_ERROR) {
        printf("sfInterpreter: error, failed to clear the tape buffer.\n");
    }


    /* ----- Returning the proper exit codes ----- */

    //If a tape over or underflow was detected return failure
    if (task_failed) {
        return EXIT_FAILURE;

    }

    //If main loop finished successfully return success
    return EXIT_SUCCESS;
}


// ----- Functions ----- //
// Public function prototypes //
FlagType parseFlag(const char* arg) {
    for (size_t i = 0; i < FlagCount; i++) {
        if (strcmp(arg, FlagLookup[i].short_flag) == 0 ||
            strcmp(arg, FlagLookup[i].long_flag) == 0) {
            return FlagLookup[i].type;
        }

    }

    return FLAG_UNKNOWN;
}


Status readFile(CharBuffer* target_buffer, const char* filename) {
    bool jump_clean = false; //the cleanup flag is false by default

    //open the target file
    FILE* open_file = fopen(filename, "r");
    if (open_file == NULL) { //error handling
        printf("readFile: error, failed to open the target file as it might not exists.\nHINT: Check the filename and path.\n");
        return STAT_ERROR;
    }

    //get the size of the target file
    uint32_t file_size = getFileSize(open_file);
    if (file_size == 0) {//error handling
        printf("readFile: error, the target file seems to be empty.\n");
        jump_clean = true;
        goto cleanup;

    }

    //allocate the memory to hold the file content
    target_buffer -> buffer = calloc(file_size + 1, sizeof(char)); //size +1 for the null terminator
    if (target_buffer -> buffer == NULL) {//error handling
        printf("readFile: error, failed to allocate buffer memory.\n");
        jump_clean = true;
        goto cleanup;
    }

    //set the null terminator at the end and set the length of the string
    target_buffer -> buffer[file_size] = '\0';
    target_buffer -> len = file_size;

    //read the target file byte-by-byte into the string buffer
    uint64_t readRet = fread(target_buffer -> buffer, sizeof(char), file_size, open_file);
    if (readRet != file_size) {//error handling
        printf("readFile: warning, failed to read the whole file.\n");
        jump_clean = true;
        goto cleanup;
    }

    //close the file
    int closeRet = fclose(open_file);
    if (closeRet != 0) {//error handling
        printf("readFile: error, failed to properly close the target file.\n");
        return STAT_ERROR;

    }

    //error handling with a file closing
    cleanup:
        if (jump_clean == true) {

            if (open_file != NULL) fclose(open_file);
            free(target_buffer -> buffer);
            target_buffer -> buffer = NULL;
            target_buffer -> len = 0;
            return STAT_ERROR;

        }

    return STAT_SUCCESS;

}


Status readBinary(ReadBuffer* target_buffer, DataType type, const char* filename) {
    bool jump_clean = false; //the cleanup flag is false by default

    size_t offset;
    uint64_t readRet;

    //Init target buffer values
    target_buffer -> type = type;
    target_buffer -> buffer = NULL;
    target_buffer -> len = 0;

    switch (type) {
        case U8:
            offset = sizeof(uint8_t);
            break;
        case U32:
            offset = sizeof(uint32_t);
            break;
        case U64:
            offset = sizeof(uint64_t);
            break;
        case I8:
            offset = sizeof(int8_t);
            break;
        case I32:
            offset = sizeof(int32_t);
            break;
        case I64:
            offset = sizeof(int64_t);
            break;
        case STR:
            offset = sizeof(char);
            break;
        default:
            printf("readFile: error, invalid data type.\n");
            return STAT_ERROR;
            break;

    }

    //open the target file
    FILE* open_file = fopen(filename, "rb");
    if (open_file == NULL) { //error handling
        printf("readFile: error, failed to open the target file as it might not exists.\nHINT: Check the filename and path.\n");
        return STAT_ERROR;
    }

    //get the size of the target file
    uint32_t file_size = getFileSize(open_file);
    if (file_size == 0) {//error handling
        printf("readFile: error, the target file seems to be empty.\n");
        jump_clean = true;
        goto cleanup;

    }
    if (file_size % offset != 0) {
        printf("readFile: file size is not aligned with element size.\n");
        jump_clean = true;
        goto cleanup;

    } else {
        file_size = file_size / offset;

    }

    //allocate the memory to hold the file content
    target_buffer -> buffer = calloc(file_size + 1, offset); //size +1 for the null terminator

    if (target_buffer -> buffer == NULL) {//error handling
        printf("readFile: error, failed to allocate buffer memory.\n");
        jump_clean = true;
        goto cleanup;
    }

    //set the null terminator at the end and set the length of the string
    target_buffer -> len = file_size;

    //read the target file byte-by-byte into the string buffer
    readRet = fread(target_buffer -> buffer, offset, file_size, open_file);
    if (readRet != file_size) {//error handling
        printf("readFile: warning, failed to read the whole file.\n");
        jump_clean = true;
        goto cleanup;
    }

    //if the buffer is a char buffer, it has to be appended with a null terminator
    if (type == STR) {
        ((char*) target_buffer -> buffer)[file_size] = '\0'; //cast the buffer into a char buffer to append

    }

    //close the file
    int closeRet = fclose(open_file);
    if (closeRet != 0) {//error handling
        printf("readFile: error, failed to properly close the target file.\n");
        return STAT_ERROR;

    }

    //error handling with a file closing
    cleanup:
        if (jump_clean == true) {
            if (open_file != NULL) fclose(open_file);
            free(target_buffer -> buffer);
            target_buffer -> len = 0;
            return STAT_ERROR;

        }

    return STAT_SUCCESS;

}



Status writeFile(CharBuffer* target_buffer, const char* filename) {
    FILE* open_file = fopen(filename, "w");
    if (open_file == NULL) {
        printf("readFile: error, failed to open or create the target file.\n");
        return STAT_ERROR;
    }

    uint64_t file_size = target_buffer -> len;
    uint64_t writeRet = fwrite(target_buffer -> buffer, sizeof(char), file_size, open_file);
    if (writeRet < file_size) {
        printf("writeFile: warning, failed to write the whole file.\n");

    }

    int closeRet = fclose(open_file);
    if (closeRet != 0) {
        printf("readFile: error, failed to properly close the target file.\n");
        return STAT_ERROR;

    }

    return STAT_SUCCESS;

}


Status clearCharBuffer(CharBuffer* target_buffer) {
    if (target_buffer -> buffer == NULL) {
        printf("clearCharBuffer: warning, the target buffer is empty.\n");
        return STAT_FAILURE;

    }

    free(target_buffer -> buffer);
    target_buffer -> buffer = NULL;
    target_buffer -> len = 0;

    return STAT_SUCCESS;

}

Status clearReadBuffer(ReadBuffer* target_buffer) {
    if (target_buffer -> buffer == NULL) {
        printf("clearCharBuffer: warning, the target buffer is empty.\n");
        return STAT_FAILURE;

    }

    free(target_buffer -> buffer);
    target_buffer -> buffer = NULL;
    target_buffer -> len = 0;

    return STAT_SUCCESS;

}

bool hasExtension(const char* arg, const char* extension)  {
    uint64_t arg_len = strlen(arg);
    uint64_t extension_len = strlen(extension);

    if (arg_len < extension_len) {
        return false;
    }

     if(strcmp(arg + (arg_len - extension_len), extension) != 0){
         return false;

     } else {
         return true;

     }

}


// Public function prototypes //
uint32_t getFileSize(FILE* file) {
    //move the memory pointer to the end of the file with 0 offset
    int seekRet = fseek(file, 0, SEEK_END);
    if (seekRet != 0) {
        printf("getFileSize: error, fseek failed.\n");
        return 0;

    }

    //ftell will return the byte size of the file up to the pointer (here the end)
    int64_t file_size = ftell(file);

    //return the memory pointer in file to the beginning
    rewind(file);

    //check the return of tell for partial reads and read errors
    if (file_size == 0) {
        printf("getFileSize: warning, the file appears to be empty.\n");
        return 0;

    } else if (file_size < 0) {
        printf("getFileSize: error, ftell failed.\n");
        return 0;
    }

    return (uint32_t) file_size;

}

Status translateCharTape(int32_t num_tape[], CharBuffer* tape_buffer) {
    if (tape_buffer -> buffer == NULL || tape_buffer -> len == 0) {
        printf("translateCharTape: error, the supplied char tape is empty.\n");
        return STAT_ERROR;

    }

    for (uint64_t i = 0; i < tape_buffer -> len -1; i++) {
        char c = tape_buffer -> buffer[i];
        if (c == '0' || c == '1' ) {
            num_tape[i] = tape_buffer -> buffer[i] - '0';

        } else if (c == '\n') {
            continue;

        } else {
            printf("translateCharTape: error, unexpected symbol in the tape [%llu] : [%c].\n", i, c);
            return STAT_ERROR;

        }

    }

    if (num_tape == NULL) {
        printf("translateCharTape: error, the translated int array is empty.\n");
        return STAT_ERROR;
    }

    return STAT_SUCCESS;

}

Status translateIntTape(int32_t num_tape[], uint64_t num_tape_len, char* char_tape, uint64_t char_tape_len) {
    if (num_tape == NULL) {
        printf("translateIntTape: error, the supplied integer tape is empty.\n");
        return STAT_ERROR;

    }

    for (uint64_t i = 0; i < num_tape_len; i++) {
        int32_t num = num_tape[i];
        if (num == 0 || num == 1 ) {
            char_tape[i] = (char) num_tape[i] + '0';

        } else {
            printf("translateIntTape: error, unexpected symbol in the tape [%llu] : [%i].\n", i, num);
            return STAT_ERROR;

        }

    }

    if (char_tape == NULL) {
        printf("translateIntTape: error, the translated char array is empty.\n");
        return STAT_ERROR;
    }

    char_tape[char_tape_len-1] = '\0';

    return STAT_SUCCESS;

}

Status mapLoops(Stack* stack, HashTable* hash_table, CharBuffer* code) {
    if (stack == NULL) {
        printf("mapLoops: error, the given stack was now initialized properly.\n");
        return STAT_ERROR;

    }

    if (hash_table == NULL) {
        printf("mapLoops: error, the given hash_table was now initialized properly.\n");
        return STAT_ERROR;

    }

    if (code -> buffer == NULL || code -> len == 0) {
        printf("translateTape: error, the supplied code buffer is empty.\n");
        return STAT_ERROR;

    }



    for (uint64_t i = 0; i < code -> len; i++) {
        if (code -> buffer[i] == '[') {
            stackPush(stack, makeValue_u64(i));

        } else if (code -> buffer[i] == ']') {
            //forward mapping
            Value popped_val = stackPop(stack);
            if (popped_val.type == EMPTY) {
                printf("mapLoops: syntax error, loop closing found without opening pair at position [%llu].\n", i);
                return STAT_ERROR;

            } else if (popped_val.type != U64) {
                printf("mapLoops: error, corrupted stack state (value type [%s] found at position [%llu] instead of U64).\n", typeToString(popped_val.type), i);
                clearStack(stack);
                return STAT_ERROR;

            }

            uint64_t loop_start = popped_val.as.u64;
            //forward mapping
            insertData(hash_table, makeKey_u64(loop_start), makeValue_u64(i));

            //reverse mapping
            insertData(hash_table, makeKey_u64(i), makeValue_u64(loop_start));

        }

    }

    //checking if the stack is empty as expected
    Value empty_value = stackPop(stack);
    if (empty_value.type != EMPTY) {
        printf("mapLoops: syntax error, loop opening found without closing pair at position [%llu].\n", empty_value.as.u64);
        clearStack(stack);
        return STAT_ERROR;

    }

    return STAT_SUCCESS;

}
