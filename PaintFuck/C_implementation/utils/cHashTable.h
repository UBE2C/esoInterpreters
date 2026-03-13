/*
 * #------------------------------------#
 * |                                    |
 * |  Custom hash table implementation  |
 * |       Gábor Bakos (UBE2C)          |
 * |                                    |
 * #------------------------------------#
 *
 */


// ----- Include guard ----- //
#ifndef CHASHTABLE_H
#define CHASHTABLE_H


// ----- Necessary includes ----- //
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cDataTypes.h"


//----- Struct and enum definitions ----- //

/*
 * Represents a single bucket in the hash table.
 *
 * Contains:
 *   - Inline storage for the first key:value pair
 *   - Pointer to a collision chain (if present)
 *   - Length of the attached collision chain
 *   - Flag indicating whether inline storage is active
 *
 * Buckets are allocated as part of the main array.
 */
typedef struct Bucket {
    Value local_key;
    Value local_value;
    uint64_t chain_len;
    struct DataNode* first_node;

    bool store_active;

} Bucket;

/*
 * Node element of a doubly linked collision chain.
 *
 * Stores:
 *   - Key and value data
 *   - Pointers to previous and next nodes
 *
 * Nodes are dynamically allocated on the heap.
 */
typedef struct DataNode {
    Value local_key;
    Value local_value;
    struct DataNode* previousNode;
    struct DataNode* nextNode;

} DataNode;

/*
 * Primary hash table structure.
 *
 * Contains:
 *   - Dynamically allocated array of buckets
 *   - Length of the bucket array
 *
 * All table operations operate on this structure.
 */
typedef struct HashTable {
    uint64_t array_len;
    struct Bucket* main_array;

} HashTable;


// ----- Function prototypes ----- //
// Public function prototypes //

/*
 * Creates and initializes a new hash table.
 *
 * PARAMETERS:
 *   length - Number of buckets to allocate for the table.
 *
 * BEHAVIOR:
 *   - Allocates internal storage for the specified number of buckets.
 *   - Initializes all buckets to an empty state.
 *
 * RETURNS:
 *   A fully initialized HashTable structure.
 *
 * NOTES:
 *   - The returned table owns dynamically allocated memory.
 *   - The table must be released using freeHtable() when no longer needed.
 *
 * EXAMPLE:
 *   HashTable dict = createHtable(100);
 */
HashTable initHtable(uint64_t length);

/*
 * Frees all memory associated with a hash table.
 *
 * PARAMETERS:
 *   table - Pointer to the HashTable to release.
 *
 * BEHAVIOR:
 *   - Frees all stored key:value pairs.
 *   - Frees all collision chains.
 *   - Frees internal bucket storage.
 *   - Leaves the table in an invalid state.
 *
 * RETURNS:
 *   STAT_SUCCESS (1)  - Table successfully freed.
 *   STAT_FAILURE (0)  - Table was already empty or invalid.
 *   STAT_ERROR  (-1)  - An internal error occurred.
 *
 * NOTES:
 *   - The table must not be used after this call.
 *
 * EXAMPLE:
 *   Status s = freeHtable(&dict);
 */
Status freeHtable(HashTable* table);

/*
 * Inserts a key:value pair into the hash table.
 *
 * PARAMETERS:
 *   table  - Pointer to the target HashTable.
 *   key    - Key element (constructed via makeKey_xy()).
 *   value  - Value element (constructed via makeValue_xy()).
 *
 * BEHAVIOR:
 *   - If the key does not exist, a copy of the key and value
 *     is stored in the table.
 *   - If the key already exists, insertion fails and the table
 *     remains unchanged.
 *
 * OWNERSHIP:
 *   - The table makes internal copies of the provided key and value.
 *   - The caller retains ownership of the original arguments.
 *   - String keys and values are deep-copied.
 *
 * RETURNS:
 *   STAT_SUCCESS (1)  - Key:value pair successfully inserted.
 *   STAT_FAILURE (0)  - Key already exists.
 *   STAT_ERROR  (-1)  - An internal error occurred (e.g., allocation failure).
 *
 * EXAMPLE:
 *   Status s = insertData(&dict,
 *                         makeKey_str("Name"),
 *                         makeValue_str("Dean"));
 */
Status insertData(HashTable* table, Value key, Value value);

/*
 * Retrieves the value associated with a given key.
 *
 * PARAMETERS:
 *   table - Pointer to the target HashTable.
 *   key   - Key to look up (constructed via makeKey_xy()).
 *
 * BEHAVIOR:
 *   - Searches the table for the given key.
 *   - If found, returns the associated value.
 *   - If not found, returns a Value indicating failure
 *     (implementation-defined error state).
 *
 * RETURNS:
 *   The stored Value corresponding to the key, or an empty Value upon any error.
 *
 * NOTES:
 *   - The returned Value is owned by the table.
 *   - The caller must NOT free or modify returned string data.
 *   - To monitor errors, check for the EMPTY Value type.
 *
 * EXAMPLE:
 *   Value v = getData(&dict, makeKey_str("Name"));
 */
Value getData(HashTable* table, Value key);

/*
 * Removes a key:value pair from the hash table.
 *
 * PARAMETERS:
 *   table - Pointer to the target HashTable.
 *   key   - Key identifying the entry to remove (constructed via makeKey_xy()).
 *
 * BEHAVIOR:
 *   - If the key exists, the associated key:value pair
 *     is removed and its memory is freed.
 *   - If the key does not exist, no changes are made.
 *
 * RETURNS:
 *   STAT_SUCCESS (1)  - Entry successfully removed.
 *   STAT_FAILURE (0)  - Key not found.
 *   STAT_ERROR  (-1)  - An internal error occurred.
 *
 * EXAMPLE:
 *   Status s = removeData(&dict, makeKey_str("Name"));
 */
Status removeData(HashTable* table, Value key);


//Private function prototypes //

/*
 *  Computes the hash index for a given key.
 *
 * PARAMETERS:
 *   array_len - Length of the hash table's main bucket array.
 *   key       - Key to be hashed.
 *
 * BEHAVIOR:
 *   - Computes a deterministic hash based on the key type and value.
 *   - The returned index is guaranteed to be within [0, array_len - 1].
 *
 * RETURNS:
 *   Bucket index corresponding to the hashed key.
 *
 * NOTE:
 *   - Behavior is undefined if array_len == 0.
 *
 * EXAMPLE:
 *   uint64_t idx = hashKey(dict.length, makeKey_str("Name"));
 */
uint64_t hashKey(uint64_t array_len, Value key);

/*
 * Traverses a collision chain and returns the last node.
 *
 * PARAMETERS:
 *   node - Pointer to the first node in a collision chain.
 *
 * BEHAVIOR:
 *   - Iterates through the linked list until the final node
 *     (node->next == NULL) is reached.
 *
 * RETURNS:
 *   Pointer to the last DataNode in the chain.
 *   NULL if the input node is NULL.
 *
 * NOTE:
 *   - Does not modify the chain.
 *
 * EXAMPLE:
 *   DataNode* tail = findLastNode(bucket->head);
 */
DataNode* findLastNode(DataNode* node);

/*
 * Compares two keys for equality.
 *
 * PARAMETERS:
 *   key_1 - First key to compare.
 *   key_2 - Second key to compare.
 *
 * BEHAVIOR:
 *   - Compares both type and underlying value.
 *   - String keys are compared using content comparison
 *     (e.g., strcmp), not pointer equality.
 *
 * RETURNS:
 *   COMP_EQUAL     - Keys are equal.
 *   COMP_NOT_EQUAL - Keys are not equal.
 *   COMP_ERROR     - Types are incompatible or invalid.
 *
 * NOTE:
 *   - Keys of different types are considered not equal.
 *
 * EXAMPLE:
 *   if (compareKeys(k1, k2) == COMP_EQUAL) { ... }
 */
CompReturn compareKeys(Value key_1, Value key_2);

/*
 * Allocates and appends a new DataNode to a specific bucket.
 *
 * PARAMETERS:
 *   table    - Pointer to the target HashTable.
 *   bucket_i - Index of the target bucket.
 *   key      - Key to insert (will be copied internally).
 *   value    - Value to insert (will be copied internally).
 *
 * BEHAVIOR:
 *   - Allocates a new DataNode on the heap.
 *   - If the bucket is empty, the node becomes the head.
 *   - Otherwise, the node is appended to the end of the
 *     collision chain.
 *   - Performs deep copy for string data.
 *
 * RETURNS:
 *   STAT_SUCCESS - Node successfully added.
 *   STAT_FAILURE - Logical failure (e.g., duplicate key).
 *   STAT_ERROR   - Allocation failure or invalid input.
 *
 * NOTE:
 *   - Intended as an internal helper for insertData().
 */
Status addNode(HashTable* table, uint64_t bucket_i, Value key, Value value);

/*
 * Removes and frees a specific DataNode from a bucket chain.
 *
 * PARAMETERS:
 *   table    - Pointer to the target HashTable.
 *   bucket_i - Index of the bucket containing the node.
 *   node     - Pointer to the DataNode to remove.
 *
 * BEHAVIOR:
 *   - Unlinks the node from the collision chain.
 *   - Frees associated key/value data (including deep-copied strings).
 *   - Frees the DataNode structure itself.
 *
 * RETURNS:
 *   STAT_SUCCESS - Node successfully removed and freed.
 *   STAT_FAILURE - Node not found in chain.
 *   STAT_ERROR   - Invalid input.
 *
 * NOTE:
 *   - Intended as an internal helper for removeData() and freeHtable().
 */
Status freeNode(HashTable* table, uint64_t bucket_i, DataNode* node);

/*
 * Creates a deep copy of a null-terminated string.
 *
 * PARAMETERS:
 *   str - Source string.
 *
 * BEHAVIOR:
 *   - Allocates sufficient heap memory to store the string
 *     including the null terminator.
 *   - Copies the contents of the source string.
 *
 * RETURNS:
 *   Pointer to the newly allocated copy.
 *   NULL if allocation fails or input is NULL.
 *
 * NOTE:
 *   - Caller is responsible for freeing the returned memory.
 *   - Intended for internal allocations of string keys or values.
 *
 * EXAMPLE:
 *   char* copy = deepCopyString("hello");
 */
char* deepCopyString(char* str);

/*
 * Frees internally copied string data stored inside a Value.
 *
 * PARAMETERS:
 *   data - Pointer to the Value containing heap-allocated string data.
 *
 * BEHAVIOR:
 *   - Frees the string if the Value type represents a string.
 *   - Sets internal pointer to NULL after freeing.
 *
 * RETURNS:
 *   STAT_SUCCESS - Memory freed successfully.
 *   STAT_FAILURE - Value did not contain allocated string data.
 *   STAT_ERROR   - Invalid input.
 *
 * NOTE:
 *   - Does not free the Value struct itself.
 *   - Intended for internal cleanup routines.
 */
Status freeCopiedString(Value* data);


#endif
