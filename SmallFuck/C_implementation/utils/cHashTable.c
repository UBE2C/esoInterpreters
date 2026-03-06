
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cDataTypes.h"


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


//Public function prototypes
HashTable initHtable(uint64_t length);
Status freeHtable(HashTable* table);

Status insertData(HashTable* table, Value key, Value value);
Value getData(HashTable* table, Value key);
Status removeData(HashTable* table, Value key);


//Private function prototypes
uint64_t hashKey(uint64_t array_len, Value key);
DataNode* findLastNode(DataNode* node);
CompReturn compareKeys(Value key_1, Value key_2);
Status addNode(HashTable* table, uint64_t bucket_i, Value key, Value value);
Status freeNode(HashTable* table, uint64_t bucket_i, DataNode* node);
char* deepCopyString(char* str);
Status freeCopiedString(Value* data);


// ----- Public function declarations ----- //

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
HashTable initHtable(uint64_t length) {
    HashTable table = {
        .array_len = length,
        .main_array = calloc(length, sizeof(Bucket))
    };

    if (table.main_array == NULL) {
        printf("createHtable: an error has occurred while allocating the main array.\n");
        table.array_len = 0;
        return table;

    }

  return table;

}

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
Status freeHtable(HashTable* table) {
    uint64_t len = table -> array_len;
    Bucket* main_array = table -> main_array;

    for (uint64_t i = 0; i < len; i++) {
        if (main_array[i].chain_len != 0) {
            while (main_array[i].chain_len != 0) {
                DataNode* lastNode = findLastNode(main_array[i].first_node);
                freeNode(table, i, lastNode);

            }
        }
    }

    free(main_array);
    table -> main_array = NULL;
    table -> array_len = 0;
    return STAT_SUCCESS;

}

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
Status insertData(HashTable* table, Value key, Value value) {
    uint64_t array_len = table -> array_len;
    uint64_t hash_index = hashKey(array_len, key);

    if (hash_index == SIZE_MAX) {
        printf("insertData: error, hashing failed (likely out of bound access in main_array).\n");
        return STAT_ERROR;

    }

    if (table -> main_array[hash_index].store_active) {
        Value local_key = table -> main_array[hash_index].local_key;
        CompReturn comp_result = compareKeys(key, local_key);

        if (comp_result == CMP_TRUE) {
            printf("insertData: failure on key:value pair insert. The key already exists at index %llu.\n", hash_index);
            return STAT_FAILURE;

        } else if (comp_result == CMP_ERROR) {
            printf("insertData: error on key:value pair insert. The new key has an unsupported type.\n");
            return STAT_ERROR;

        }

        DataNode* first_node = table -> main_array[hash_index].first_node;
        if (first_node == NULL) {
            addNode(table, hash_index, key, value);
            return STAT_SUCCESS;

        } else {
            DataNode* current_node = first_node; //take the first node's pointer
            while (current_node != NULL) { //walk along the node chain until there is no more next node (the last node is also checked)
                Value node_key = current_node -> local_key;
                CompReturn comp_result = compareKeys(key, node_key);

                if (comp_result == CMP_TRUE) { //check if the stored key inside the node matches the new key and raise if yes
                    printf("insertData: failure on key:value pair insert. The key already exists at index %llu.\n", hash_index);
                    return STAT_FAILURE;

                } else if (comp_result == CMP_ERROR) {
                    printf("insertData: error on key:value pair insert. The new key has an unsupported type.\n");
                    return STAT_ERROR;

                }

                current_node = current_node -> nextNode;
            }

            addNode(table, hash_index, key, value);
            return STAT_SUCCESS;

        }

    } else {
        if (key.type == STR) { //if the key is a string do a deep copy to pass ownership
            char* new_str_p = deepCopyString(key.as.str);
            if (new_str_p == NULL) {
                printf("insertData: error, failed to pass string ownership during insert\n");
                return STAT_ERROR;

            }

            table -> main_array[hash_index].local_key.type = STR;
            table -> main_array[hash_index].local_key.as.str = new_str_p;

        } else {
            table -> main_array[hash_index].local_key = key; //for numeric values a shallow copy is enough

        }

        if (value.type == STR) { //if the value is a string do a deep copy to pass ownership
            char* new_str_p = deepCopyString(value.as.str);
            if (new_str_p == NULL) {
                printf("insertData: error, failed to pass string ownership during insert\n");
                return STAT_ERROR;

            }

            table -> main_array[hash_index].local_value.type = STR;
            table -> main_array[hash_index].local_value.as.str = new_str_p;

        } else {
            table -> main_array[hash_index].local_value = value;

        }


        table -> main_array[hash_index].store_active = true;
        return STAT_SUCCESS;

    }

    return STAT_FAILURE;

}

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
Value getData(HashTable* table, Value key) {
    Value data_out;
    Value empty = {
        .type = EMPTY,
        .as.empty = NULL
    };

    uint64_t array_len = table -> array_len;
    uint64_t hash_index = hashKey(array_len, key);

    if (hash_index == SIZE_MAX) {
        printf("getData: error, hashing failed (likely out of bound access in main_array).\n");
        return empty;

    }

    if (table -> main_array[hash_index].store_active) {
        Value local_key = table -> main_array[hash_index].local_key;
        CompReturn comp_result = compareKeys(key, local_key);

        if (comp_result == CMP_TRUE) {
            data_out = table -> main_array[hash_index].local_value;
            return data_out;

        } else if (comp_result == CMP_ERROR) {
            printf("getData: error in getting key:value pair. The given key has an unsupported type.\n");
            return empty;

        }

    }

    DataNode* first_node = table -> main_array[hash_index].first_node;
    if (first_node == NULL) {
        printf("getData: error in getting key:value pair. The given key does not exist.\n");
        return empty;

    } else {
        DataNode* current_node = first_node; //take the first node's pointer
        while (current_node != NULL) { //walk along the node chain until there is no more next node (the last node is also checked)
            Value node_key = current_node -> local_key;
            CompReturn comp_result = compareKeys(key, node_key);

            if (comp_result == CMP_TRUE) { //check if the stored key inside the node matches the new key and return if yes
                data_out = current_node -> local_value;
                return data_out;

            } else if (comp_result == CMP_ERROR) {
                printf("getData: error in getting key:value pair. The given key has an unsupported type.\n");
                return empty;

            }

            current_node = current_node -> nextNode;
        }

        printf("getData: error in getting key:value pair. The given key does not exist.\n");
        return empty;

    }

}

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
Status removeData(HashTable* table, Value key) {
    Value empty = {
        .type = EMPTY,
        .as.empty = NULL
    };

    uint64_t array_len = table -> array_len;
    uint64_t hash_index = hashKey(array_len, key);

    if (hash_index == SIZE_MAX) {
        printf("removeData: error, hashing failed (likely out of bound access in main_array).\n");
        return STAT_FAILURE;

    }

    if (table -> main_array[hash_index].store_active) {
        Value local_key = table -> main_array[hash_index].local_key;
        Value local_value = table -> main_array[hash_index].local_value;
        CompReturn comp_result = compareKeys(key, local_key);

        if (comp_result == CMP_TRUE) {
            if (local_key.type == STR) {
                freeCopiedString(&table -> main_array[hash_index].local_key);

            }

            if (local_value.type == STR) {
                freeCopiedString(&table -> main_array[hash_index].local_value);

            }

            table -> main_array[hash_index].local_key = empty;
            table -> main_array[hash_index].local_value = empty;
            table -> main_array[hash_index].store_active = false;
            return STAT_SUCCESS;

        } else if (comp_result == CMP_ERROR) {
            printf("removeData: error in getting key:value pair. The given key has an unsupported type.\n");
            return STAT_ERROR;

        }

    }

    DataNode* first_node = table -> main_array[hash_index].first_node;
    if (first_node == NULL) {
        printf("removeData: failure in getting key:value pair. The given key does not exist.\n");
        return STAT_FAILURE;

    } else {
        DataNode* current_node = first_node; //take the first node's pointer
        while (current_node != NULL) { //walk along the node chain until there is no more next node (the last node is also checked)
            Value node_key = current_node -> local_key;
            CompReturn comp_result = compareKeys(key, node_key);

            if (comp_result == CMP_TRUE) { //check if the stored key inside the node matches the new key and return if yes
                freeNode(table, hash_index, current_node);
                return STAT_SUCCESS;

            } else if (comp_result == CMP_ERROR) {
                printf("getData: error in getting key:value pair. The given key has an unsupported type.\n");
                return STAT_ERROR;

            }

            current_node = current_node -> nextNode;
        }

        printf("getData: failure in getting key:value pair. The given key does not exist.\n");
        return STAT_FAILURE;

    }

    return STAT_FAILURE;

}




// ----- Private function declarations ----- //

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
uint64_t hashKey(uint64_t array_len, Value key) {
    uint64_t hash_key;
    uint64_t len = array_len;
    if (len == 0) {
        printf("hashKey: key:value array len == 0, trying to add data to an uninitialized hash table array.\n");
        return SIZE_MAX;
    }

    if (key.type == STR) {
        /*
         * This is a variant of the djb2 hashing algorithm
        def hash(string):
            x = string[0] << 7
            for chr in string[1:]:
                x = ((1000003 * x) ^ chr) & (1<<32)
            return x
         */
         uint64_t x = (uint64_t) key.as.str[0];
         if (x == '\0') {
             printf("hashKey: an empty key string has been supplied.\n");
             return SIZE_MAX;
         }

         uint64_t iter = 1;
         while (key.as.str[iter] != '\0') {
             uint64_t chr = (uint64_t) key.as.str[iter];
             x = ((33 * x) ^ chr);

             iter++;
         }

         hash_key = x % len;

    } else {
        /*
         * This is a minor variation of the FNV hash algorithm known as FNV-1a:
         hash = offset_basis (for u32 offset_basis = 2166136261)
         for each octet_of_data to be hashed
                 hash = hash xor octet_of_data
                 hash = hash * FNV_prime (for u32 FNV_prime = 16777619)
         return hash

         */

        uint64_t key_value;
        uint8_t bytes;
        switch (key.type) {
            case U8:
                key_value = key.as.u8;
                bytes = 1;
                break;

            case U32:
                key_value = key.as.u32;
                bytes = 4;
                break;

            case U64:
                key_value = key.as.u64;
                bytes = 8;
                break;

            default:
                printf("hashKey: a wrong key type has been supplied, the value type must be str or an int/uint.\n");
                return SIZE_MAX;


        }

        uint64_t x = (uint64_t) 2166136261;
        for (int i = 0; i < bytes; i++) {
            uint8_t octet = (key_value >> (8*i)) & 0xFF;
            x ^= octet;
            x *=16777619;

        }

        hash_key = x % len;

    }

    return hash_key;
}

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
DataNode* findLastNode(DataNode* node) {
    if (node == NULL) {
        return NULL;
    }

    while (node -> nextNode != NULL) {
        node = node -> nextNode;
    }

    return node;

}

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
CompReturn compareKeys(Value key_1, Value key_2) {
    if (key_1.type == STR && key_2.type != STR) {
        return CMP_FALSE;

    } else if (key_1.type != STR && key_2.type == STR) {
        return CMP_FALSE;

    } else if (key_1.type == STR && key_2.type == STR) {
        int comp_result = strcmp(key_1.as.str, key_2.as.str);
        if (comp_result == 0) {
            return CMP_TRUE;

        } else {
            return CMP_FALSE;

        }

    } else {
        uint64_t key_1_value;
        uint64_t key_2_value;

        switch (key_1.type) {
            case U8:
                key_1_value = (uint64_t) key_1.as.u8;
                break;

            case U32:
                key_1_value = (uint64_t) key_1.as.u32;
                break;

            case U64:
                key_1_value = (uint64_t) key_1.as.u64;
                break;

            default:
            printf("compareKey: error in key comparison, key_1 has an unsupported type.\n");
            return CMP_ERROR;

        }

        switch (key_2.type) {
            case U8:
                key_2_value = (uint64_t) key_2.as.u8;
                break;

            case U32:
                key_2_value = (uint64_t) key_2.as.u32;
                break;

            case U64:
                key_2_value = (uint64_t) key_2.as.u64;
                break;

            default:
            printf("compareKey: error in key comparison, key_2 has an unsupported type.\n");
            return CMP_ERROR;

        }

        if (key_1_value == key_2_value) {
            return CMP_TRUE;

        } else {
        return CMP_FALSE;

        }

    }

}

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
Status addNode(HashTable* table, uint64_t bucket_i, Value key, Value value) {
    DataNode* newNode = calloc(1, sizeof(DataNode));
    if (newNode == NULL) {
        printf("addNode: an error has occurred while allocating a new data node to a main array bucket.\n");
        return STAT_ERROR;
    }


    Bucket* main_array = table -> main_array; //the main array of the hash table
    Bucket* target_bucket = main_array + bucket_i; //the bucket we are attaching the linked list to

    if (target_bucket -> chain_len == 0) { //in case the new node is the first one
        target_bucket -> first_node = newNode;

        newNode -> previousNode = NULL;
        newNode -> nextNode = NULL;

    } else {
        DataNode* lastNode = findLastNode(target_bucket -> first_node);
        lastNode -> nextNode = newNode;

        newNode -> previousNode = lastNode;
        newNode -> nextNode = NULL;

    }

    if (key.type == STR) {
        char* new_str_p = deepCopyString(key.as.str);
        if (new_str_p == NULL) {
            printf("insertData: error, failed to pass string ownership during insert\n");
            return STAT_ERROR;

        }

        newNode -> local_key.type = STR;
        newNode -> local_key.as.str = new_str_p;

    } else {
        newNode -> local_key = key;

    }

    if (value.type == STR) {
        char* new_str_p = deepCopyString(value.as.str);
        if (new_str_p == NULL) {
            printf("insertData: error, failed to pass string ownership during insert\n");
            return STAT_ERROR;

        }

        newNode -> local_value.type = STR;
        newNode -> local_value.as.str = new_str_p;

    } else {
        newNode -> local_value = value;

    }

    target_bucket -> chain_len++;
    return STAT_SUCCESS;

}

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
Status freeNode(HashTable* table, uint64_t bucket_i, DataNode* node) {
    Bucket* main_array = table -> main_array; //the main array of the hash table
    Bucket* target_bucket = main_array + bucket_i; //the bucket we find our node in

    if (node -> previousNode == NULL) { //if the node is the first node
        if (node -> nextNode != NULL) {
            DataNode* nextNode = node -> nextNode;
            nextNode -> previousNode = NULL; //set the next node's previous node pointer to null

            target_bucket -> first_node = nextNode;

        } else {
            target_bucket -> first_node = NULL;

        }

    } else if (node -> previousNode != NULL && node -> nextNode != NULL) {
        DataNode* previousNode = node -> previousNode;
        previousNode -> nextNode = node -> nextNode;

        DataNode* nextNode = node -> nextNode;
        nextNode -> previousNode = node -> previousNode;

    } else {
        DataNode* previousNode = node -> previousNode;
        previousNode -> nextNode = NULL;

    }

    if (node -> local_key.type == STR) {
        freeCopiedString(&node -> local_key);

    }

    if (node -> local_value.type == STR) {
        freeCopiedString(&node -> local_value);

    }


    free(node);
    target_bucket -> chain_len--;
    return STAT_SUCCESS;

}

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
char* deepCopyString(char* str) {
    char* stored_str = malloc(strlen(str) + 1); //allocate the memory block to hold the key/value string
    if (stored_str == NULL) {
        printf("deepCopyString: error, heap allocation of given string failed.\n");
        return NULL;

    }

    return strcpy(stored_str, str); //copy the string to the allocated block and pass the new pointer to the owner

}

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
Status freeCopiedString(Value* data) {
    if (data -> type != STR) {
        printf("deepCopyString: error, the passed Value does not hold a string.\n");
        return STAT_ERROR;
    }

    free(data -> as.str);
    data -> as.str = NULL;
    return STAT_SUCCESS;

}
