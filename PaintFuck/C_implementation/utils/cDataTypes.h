/*
 * #-----------------------------------------#
 * |                                         |
 * |  Shared datatypes and constructors for  |
 * |    the custom library implementations   |
 * |           Gábor Bakos (UBE2C)           |
 * |                                         |
 * #-----------------------------------------#
 *
 */


// ----- Include guard ----- //
#ifndef DATATYPES_H
#define DATATYPES_H


// ----- Necessary includes ----- //
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>


//----- Struct and enum definitions ----- //

/*
 * Enumerates all supported data types that can be stored
 * inside a Value structure.
 *
 * Used to determine:
 *   - How data is interpreted
 *   - How keys are compared
 *   - How memory is managed (e.g., string deep copy)
 */
 typedef enum DataType {
     U8,
     U32,
     U64,
     I8,
     I32,
     I64,
     F32,
     F64,
     STR,
     EMPTY,

 } DataType;

 /*
  * Standardized return codes for public and internal operations.
  *
  * Used to indicate success, logical failure, or internal error.
  */
 typedef enum Status {
     STAT_ERROR = -1, //Unexpected internal error
     STAT_FAILURE = 0, //Operation completed but did not succeed logically
     STAT_SUCCESS = 1 //Operation successful

 } Status;

 /*
  * Generic tagged data container used to store data on the stack.
  *
  * Stores:
  *   - A DataType tag describing the active type
  *   - A union holding the underlying data
  *
  * Memory Management:
  *   - Primitive types are stored directly.
  *
  *
  * The 'type' field must always reflect the active union member.
  */
  typedef struct Value {
      DataType type;
      union {
          uint8_t u8;
          uint32_t u32;
          uint64_t u64;
          int8_t i8;
          int32_t i32;
          int64_t i64;
          float f32;
          double f64;
          char* str;
          void* empty;

      } as;

  } Value;

  /*
   * Return codes for key comparison operations.
   *
   * Used by compareKeys() to indicate equality result
   * or invalid comparison.
   */
  typedef enum CompReturn {
      CMP_ERROR = -1,
      CMP_FALSE = 0,
      CMP_TRUE = 1,

  } CompReturn;


  // ----- Constructor Function prototypes ----- //

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
  Value makeValue_str(char* data);


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
  Value makeValue_u8(uint8_t data);

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
  Value makeValue_u32(uint32_t data);

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
  Value makeValue_u64(uint64_t data);

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
  Value makeValue_i8(int8_t data);

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
  Value makeValue_i32(int32_t data);

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
  Value makeValue_i64(int64_t data);

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
  Value makeValue_f32(float data);

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
  Value makeValue_f64(double data);

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
  Value makeKey_str(char* data);

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
  Value makeKey_u8(uint8_t data);

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
  Value makeKey_u32(uint32_t data);

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
  Value makeKey_u64(uint64_t data);

 #endif
