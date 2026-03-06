
// ----- Necessary includes ----- //
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "cDataTypes.h"

// ----- Function prototypes ----- //

//Public function prototypes
Value makeValue_str(char* data);
Value makeValue_u8(uint8_t data);
Value makeValue_u32(uint32_t data);
Value makeValue_u64(uint64_t data);
Value makeValue_i8(int8_t data);
Value makeValue_i32(int32_t data);
Value makeValue_i64(int64_t data);
Value makeValue_f32(float data);
Value makeValue_f64(double data);

Value makeKey_str(char* data);
Value makeKey_u8(uint8_t data);
Value makeKey_u32(uint32_t data);
Value makeKey_u64(uint64_t data);

// ----- Public function declarations ----- //

/*
 * Constructs a Value of type <char*> for use as a hash table value.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged with the appropriate type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - For makeValue_str(), the string pointer is stored as-is.
 *     The table performs deep copying during insertion.
 *
 * RETURNS:
 *   A properly initialized Value structure.
 *
 * EXAMPLE:
 *   makeValue_str("testValue")
 *
 *   char* name = "Bob"
 *
 *   makeValue_str(name)
 */
Value makeValue_str(char* data) {
    Value value = {
        .type = STR,
        .as.str = data
    };

    return value;
}

/*
 * Constructs a Value of type <uint8_t> for use as a hash table value.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged with the appropriate type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized Value structure.
 *
 * EXAMPLE:
 *   makeValue_u8(150)
 *
 *   makeValue_u8('a')
 *
 *   uint8_t num = 250
 *
 *   makeValue_u8(num)
 */
Value makeValue_u8(uint8_t data) {
    Value value = {
        .type = U8,
        .as.u8 = data
    };

    return value;
}

/*
 * Constructs a Value of type <uint32_t> for use as a hash table value.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged with the appropriate type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized Value structure.
 *
 * EXAMPLE:
 *   makeValue_u32(1500)
 *
 *   uint32_t num = 1500
 *
 *   makeValue_u32(num)
 */
Value makeValue_u32(uint32_t data) {
    Value value = {
        .type = U32,
        .as.u32 = data
    };

    return value;
}

/*
 * Constructs a Value of type <uint64_t> for use as a hash table value.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged with the appropriate type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized Value structure.
 *
 * EXAMPLE:
 *   makeValue_u64(2500)
 *
 *   uint64_t num = 10500
 *
 *   makeValue_u64(num)
 */
Value makeValue_u64(uint64_t data) {
    Value value = {
        .type = U64,
        .as.u64 = data
    };

    return value;
}

/*
 * Constructs a Value of type <int8_t> for use as a hash table value.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged with the appropriate type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized Value structure.
 *
 * EXAMPLE:
 *   makeValue_i8(-10)
 *
 *   int8_t num = -120
 *
 *   makeValue_i8(num)
 */
Value makeValue_i8(int8_t data) {
    Value value = {
        .type = I8,
        .as.i8 = data
    };

    return value;
}

/*
 * Constructs a Value of type <int32_t> for use as a hash table value.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged with the appropriate type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized Value structure.
 *
 * EXAMPLE:
 *   makeValue_i32(-190)
 *
 *   int32_t num = 300
 *
 *   makeValue_i32(num)
 */
Value makeValue_i32(int32_t data) {
    Value value = {
        .type = I32,
        .as.i32 = data
    };

    return value;
}

/*
 * Constructs a Value of type <int64_t> for use as a hash table value.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged with the appropriate type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized Value structure.
 *
 * EXAMPLE:
 *   makeValue_i64(9000)
 *
 *   int64_t num = -3000
 *
 *   makeValue_i64(num)
 */
Value makeValue_i64(int64_t data) {
    Value value = {
        .type = I64,
        .as.i64 = data
    };

    return value;
}

/*
 * Constructs a Value of type <float> for use as a hash table value.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged with the appropriate type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized Value structure.
 *
 * EXAMPLE:
 *   makeValue_f32(1.1234567)
 *
 *   float num = 3.14
 *
 *   makeValue_f32(num)
 */
Value makeValue_f32(float data) {
    Value value = {
        .type = F32,
        .as.f32 = data
    };

    return value;
}

/*
 * Constructs a Value of type <double> for use as a hash table value.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged with the appropriate type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized Value structure.
 *
 * EXAMPLE:
 *   makeValue_f64(1.123456789101112)
 *
 *   double num = -3.14
 *
 *   makeValue_f64(num)
 */
Value makeValue_f64(double data) {
    Value value = {
        .type = F64,
        .as.f64 = data
    };

    return value;
}

/*
 * Constructs a Value of type <char*> for use as a hash table key.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged as a key of the specified type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - For makeKey_str(), the string pointer is stored as-is.
 *     The table performs deep copying during insertion.
 *
 * RETURNS:
 *   A properly initialized key Value structure.
 *
 * EXAMPLE:
 *   makeKey_str("testValue")
 *
 *   char* name = "Bob"
 *
 *   makeKey_str(name)
 */
Value makeKey_str(char* data) {
    Value key = {
        .type = STR,
        .as.str = data
    };

    return key;
}

/*
 * Constructs a Value of type <uint8_t> for use as a hash table key.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged as a key of the specified type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized key Value structure.
 *
 * EXAMPLE:
 *   makeKey_u8(150)
 *
 *   makeKey_u8('a')
 *
 *   uint8_t num = 250
 *
 *   makeKey_u8(num)
 */
Value makeKey_u8(uint8_t data) {
    Value key = {
        .type = U8,
        .as.u8 = data
    };

    return key;
}

/*
 * Constructs a Value of type <uint32_t> for use as a hash table key.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged as a key of the specified type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized key Value structure.
 *
 * EXAMPLE:
 *   makeKey_u32(1500)
 *
 *   uint32_t num = 1500
 *
 *   makeKey_u32(num)
 */
Value makeKey_u32(uint32_t data) {
    Value key = {
        .type = U32,
        .as.u32 = data
    };

    return key;
}

/*
 * Constructs a Value of type <uint64_t> for use as a hash table key.
 *
 * PARAMETERS:
 *   data - The data to wrap inside the Value structure.
 *
 * BEHAVIOR:
 *   - Creates a Value tagged as a key of the specified type.
 *   - Does NOT allocate heap memory (except for string type).
 *
 * NOTES:
 *   - The table performs copy during insertion.
 *
 * RETURNS:
 *   A properly initialized key Value structure.
 *
 * EXAMPLE:
 *   makeKey_u64(2500)
 *
 *   uint64_t num = 10500
 *
 *   makeKey_u64(num)
 */
Value makeKey_u64(uint64_t data) {
    Value key = {
        .type = U64,
        .as.u64 = data
    };

    return key;
}
