
#include <cstddef>
#include <cstdint>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "../utils/cDataTypes.h"
#include "../utils/cArgParser.h"
#include "../utils/cStack.h"
#include "../utils/cHashTable.h"


typedef struct Matrix {
    DataType type;
    uint32_t n_row;
    uint32_t n_col;
    union data {
        uint8_t* u8p;
        uint32_t* u32p;
        uint64_t* u64p;
        int8_t* i8p;
        int32_t* i32p;
        int64_t* i64p;
        float* f32p;
        double* f64p;
        char* str;
        void* empty;

    } data;

} Matrix;




int32_t main(int argc, char* argv[]) {

    // Define and handle input arguments
    Argument input_args[5];
    ArgLstManager argMan = initArgManager(input_args, 5);

    Status addRet;
    addRet = addArg(&argMan, OPTIONAL, -1, "-c", "--code", true, true, "code_file");
    if (addRet != STAT_SUCCESS) {
        printf("Failed to add an argument to the argument manager.\n");
        return EXIT_FAILURE;

    }

    addArg(&argMan, POSITIONAL, 1, NULL, "code_file", true, true, "--code");
    if (addRet != STAT_SUCCESS) {
        printf("Failed to add an argument to the argument manager.\n");
        return EXIT_FAILURE;

    }

    addArg(&argMan, POSITIONAL, 2, NULL, "iterations", true, false, NULL);
    if (addRet != STAT_SUCCESS) {
        printf("Failed to add an argument to the argument manager.\n");
        return EXIT_FAILURE;

    }

    addArg(&argMan, POSITIONAL, 3, NULL, "width", true, false, NULL);
    if (addRet != STAT_SUCCESS) {
        printf("Failed to add an argument to the argument manager.\n");
        return EXIT_FAILURE;

    }

    addArg(&argMan, POSITIONAL, 4, NULL, "hight", true, false, NULL);
    if (addRet != STAT_SUCCESS) {
        printf("Failed to add an argument to the argument manager.\n");
        return EXIT_FAILURE;

    }

    Status parseRet = parseArgs(&argMan, argc, argv);
    if (parseRet != STAT_SUCCESS) {
        printf("Failed to parse the input arguments.\n");
        return EXIT_FAILURE;

    }


    // Define the 2D paint grid


    return EXIT_SUCCESS;

}


Matrix initMatrix(DataType type, uint32_t n_row, uint32_t n_col) {
    Matrix empty = {
        .type =  EMPTY,
        .n_row = 0,
        .n_col = 0,
        .data.empty = NULL

    };

    if (n_row == 0 || n_col == 0) {
        printf("initMatrix: warning, the supplied number of rows [%u] or columns [%u] are 0.\n", n_row, n_col);
        return empty;

    }


    size_t data_size;
    Matrix mat = {
        .type = type,
        .n_row = n_row,
        .n_col = n_col,

    };
    switch (type) {
        case U8: data_size = sizeof(uint8_t); mat.data.u8p = calloc(n_row * n_col, data_size); break;
        case U32: data_size = sizeof(uint32_t); mat.data.u32p = calloc(n_row * n_col, data_size); break;
        case U64: data_size = sizeof(uint64_t); mat.data.u64p = calloc(n_row * n_col, data_size); break;
        case I8: data_size = sizeof(int8_t); mat.data.i8p = calloc(n_row * n_col, data_size); break;
        case I32: data_size = sizeof(int32_t); mat.data.i32p = calloc(n_row * n_col, data_size); break;
        case I64: data_size = sizeof(int64_t);mat.data.i64p = calloc(n_row * n_col, data_size); break;
        case F32: data_size = sizeof(float); mat.data.f32p = calloc(n_row * n_col, data_size); break;
        case F64: data_size = sizeof(double); mat.data.f64p = calloc(n_row * n_col, data_size); break;
        case STR: data_size = sizeof(char*); mat.data.str = calloc(n_row * n_col, data_size); break;
        case EMPTY:
        default: printf("initMatrix: error, unknown data type provided returning an empty matrix.\n"); return empty;

    }

    // NULL check after the switch - works because all union members overlap
    if (mat.data.empty == NULL) {
        printf("initMatrix: error, memory allocation failed.\n");
        return empty;
    }

    return mat;

}


Status freeMatrix(Matrix* matrix) {
    if (matrix == NULL) {
        printf("freeMatrix: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    // NULL check before the switch - works because all union members overlap
    if (matrix -> data.empty == NULL) {
        printf("freeMatrix: warning, the data pointer of the provided matrix is NULL.\n");
        return STAT_FAILURE;

    }

    //because of the tagged union all members point to the same address so one free is enough
    free(matrix -> data.empty);

    matrix -> type = EMPTY;
    matrix -> n_row = 0;
    matrix -> n_col = 0;
    matrix -> data.empty = NULL;


    return STAT_SUCCESS;
}

Status getMat_u8(uint8_t* value_out, Matrix* matrix, uint32_t row, uint32_t col) {
    if (matrix == NULL || value_out == NULL) {
        printf("getMat_u8: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    if (matrix -> data.u8p == NULL) {
        printf("getMat_ u8: error, the data pointer is NULL.\n");
        return STAT_ERROR;

    }

    uint32_t index = row * matrix -> n_col + col;
    if (index >= matrix -> n_row * matrix -> n_col) {
        printf("getMat_u8: error, index outside matrix bounds.\n");
        return STAT_ERROR;

    }

    *value_out = matrix -> data.u8p[index];
    return STAT_SUCCESS;

}

Status getMat_u32(uint32_t* value_out, Matrix* matrix, uint32_t row, uint32_t col) {
    if (matrix == NULL || value_out == NULL) {
        printf("getMat_u32: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    if (matrix -> data.u32p == NULL) {
        printf("getMat_u32: error, the data pointer is NULL.\n");
        return STAT_ERROR;

    }

    uint32_t index = row * matrix -> n_col + col;
    if (index >= matrix -> n_row * matrix -> n_col) {
        printf("getMat_u32: error, index outside matrix bounds.\n");
        return STAT_ERROR;

    }

    *value_out = matrix -> data.u32p[index];
    return STAT_SUCCESS;

}

Status getMat_u64(uint64_t* value_out, Matrix* matrix, uint32_t row, uint32_t col) {
    if (matrix == NULL || value_out == NULL) {
        printf("getMat_u64: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    if (matrix -> data.u64p == NULL) {
        printf("getMat_u64: error, the data pointer is NULL.\n");
        return STAT_ERROR;

    }

    uint32_t index = row * matrix -> n_col + col;
    if (index >= matrix -> n_row * matrix -> n_col) {
        printf("getMat_u64: error, index outside matrix bounds.\n");
        return STAT_ERROR;

    }

    *value_out = matrix -> data.u64p[index];
    return STAT_SUCCESS;

}

Status getMat_i8(int8_t* value_out, Matrix* matrix, uint32_t row, uint32_t col) {
    if (matrix == NULL || value_out == NULL) {
        printf("getMat_i8: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    if (matrix -> data.i8p == NULL) {
        printf("getMat_i8: error, the data pointer is NULL.\n");
        return STAT_ERROR;

    }

    uint32_t index = row * matrix -> n_col + col;
    if (index >= matrix -> n_row * matrix -> n_col) {
        printf("getMat_i8: error, index outside matrix bounds.\n");
        return STAT_ERROR;

    }

    *value_out = matrix -> data.i8p[index];
    return STAT_SUCCESS;

}

Status getMat_i32(int32_t* value_out, Matrix* matrix, uint32_t row, uint32_t col) {
    if (matrix == NULL || value_out == NULL) {
        printf("getMat_i32: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    if (matrix -> data.i32p == NULL) {
        printf("getMat_i32: error, the data pointer is NULL.\n");
        return STAT_ERROR;

    }

    uint32_t index = row * matrix -> n_col + col;
    if (index >= matrix -> n_row * matrix -> n_col) {
        printf("getMat_i32: error, index outside matrix bounds.\n");
        return STAT_ERROR;

    }

    *value_out = matrix -> data.i32p[index];
    return STAT_SUCCESS;

}

Status getMat_i64(int64_t* value_out, Matrix* matrix, uint32_t row, uint32_t col) {
    if (matrix == NULL || value_out == NULL) {
        printf("getMat_i64: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    if (matrix -> data.i64p == NULL) {
        printf("getMat_i64: error, the data pointer is NULL.\n");
        return STAT_ERROR;

    }

    uint32_t index = row * matrix -> n_col + col;
    if (index >= matrix -> n_row * matrix -> n_col) {
        printf("getMat_i64: error, index outside matrix bounds.\n");
        return STAT_ERROR;

    }

    *value_out = matrix -> data.i64p[index];
    return STAT_SUCCESS;

}

Status getMat_f32(float* value_out, Matrix* matrix, uint32_t row, uint32_t col) {
    if (matrix == NULL || value_out == NULL) {
        printf("getMat_f32: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    if (matrix -> data.f32p == NULL) {
        printf("getMat_f32: error, the data pointer is NULL.\n");
        return STAT_ERROR;

    }

    uint32_t index = row * matrix -> n_col + col;
    if (index >= matrix -> n_row * matrix -> n_col) {
        printf("getMat_f32: error, index outside matrix bounds.\n");
        return STAT_ERROR;

    }

    *value_out = matrix -> data.f32p[index];
    return STAT_SUCCESS;

}

Status getMat_f64(double* value_out, Matrix* matrix, uint32_t row, uint32_t col) {
    if (matrix == NULL || value_out == NULL) {
        printf("getMat_f64: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    if (matrix -> data.f64p == NULL) {
        printf("getMat_f64: error, the data pointer is NULL.\n");
        return STAT_ERROR;

    }

    uint32_t index = row * matrix -> n_col + col;
    if (index >= matrix -> n_row * matrix -> n_col) {
        printf("getMat_f64: error, index outside matrix bounds.\n");
        return STAT_ERROR;

    }

    *value_out = matrix -> data.f64p[index];
    return STAT_SUCCESS;

}

Status getMat_str(char* value_out, Matrix* matrix, uint32_t row, uint32_t col) {
    if (matrix == NULL || value_out == NULL) {
        printf("getMat_str: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    if (matrix -> data.str == NULL) {
        printf("getMat_str: error, the data pointer is NULL.\n");
        return STAT_ERROR;

    }

    uint32_t index = row * matrix -> n_col + col;
    if (index >= matrix -> n_row * matrix -> n_col) {
        printf("getMat_str: error, index outside matrix bounds.\n");
        return STAT_ERROR;

    }

    *value_out = matrix -> data.str[index];
    return STAT_SUCCESS;

}

Status setMat_u8(uint8_t value_in, Matrix* matrix, uint32_t row, uint32_t col) {
    if (matrix == NULL) {
        printf("setMat_u8: error, NULL pointer provided.\n");
        return STAT_ERROR;

    }

    if (matrix -> data.u8p == NULL) {
        printf("setMat_u8: error, the data pointer is NULL.\n");
        return STAT_ERROR;

    }

    uint32_t index = row * matrix -> n_col + col;
    if (index >= matrix -> n_row * matrix -> n_col) {
        printf("setMat_u8: error, index outside matrix bounds.\n");
        return STAT_ERROR;

    }

    matrix -> data.u8p[index] = value_in;
    return STAT_SUCCESS;

}
