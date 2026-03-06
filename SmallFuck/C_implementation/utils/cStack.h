/*
 * #-------------------------------#
 * |                               |
 * |  Custom stack implementation  |
 * |      Gábor Bakos (UBE2C)      |
 * |                               |
 * #-------------------------------#
 *
 */


// ----- Include guard ----- //
#ifndef CSTACK_H
#define CSTACK_H


// ----- Necessary includes ----- //
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "cDataTypes.h"


//----- Struct and enum definitions ----- //

 /*
  * Node element of a doubly linked collision chain.
  *
  * Stores:
  *   - numeric and char data
  *   - Pointers to previous and next nodes
  *
  * Nodes are dynamically allocated on the heap.
  */
typedef struct Node {
    Value data;
    DataType node_type;
    struct Node* left_node;
    struct Node* right_node;

} Node;

/*
 * Stack container implemented as a doubly linked list.
 *
 * Stores:
 *   - Pointer to the top element (stack_pointer_right)
 *   - Pointer to the bottom element (stack_pointer_left)
 *   - Current number of elements (length)
 *
 * Nodes are dynamically allocated on the heap.
 * The Stack struct itself does not allocate memory.
 *
 * Supports push/pop operations from both ends.
 */
typedef struct Stack {
    Node* stack_pointer_right;
    Node* stack_pointer_left;
    uint32_t length;

} Stack;


// ----- Function prototypes ----- //
// Public function prototypes //

/*
 * Initializes and returns an empty stack.
 *
 * The stack is initialized with:
 *   length = 0
 *   stack_pointer_left = NULL
 *   stack_pointer_right = NULL
 *
 * RETURNS:
 * A properly initialized empty stack structure.
 *
 * NOTE:
 * This function does not allocate memory for the stack itself.
 * It returns a stack struct by value.
 */
Stack initStack(void);

/*
 * Pushes a Value onto the right (top) side of the stack.
 *
 * PARAMETERS:
 *   stack - Pointer to the stack.
 *   data - A Value struct to be stored in the new node.
 *
 * RETURNS:
 *   STAT_SUCCESS on success
 *   STAT_ERROR if memory allocation fails
 *
 * WARNING:
 *   Allocates a Node on the heap.
 *   If the stored value is a pointer to a heap allocated variable, the caller remains responsible
 *   for managing any heap memory stored inside Value (e.g., strings).
 */
Status stackPush(Stack* stack, Value data);

/*
 * Pushes a Value onto the left (bottom) side of the stack.
 *
 * PARAMETERS:
 *   stack - Pointer to the stack.
 *   data - A Value struct to be stored in the new node.
 *
 * RETURNS:
 *   STAT_SUCCESS on success
 *   STAT_ERROR if memory allocation fails
 *
 * WARNING:
 *   Allocates a Node on the heap.
 *   If the stored value is a pointer to a heap allocated variable, the caller remains responsible
 *   for managing any heap memory stored inside Value (e.g., strings).
 */
Status stackPushLeft(Stack* stack, Value data);

/*
 * Removes and returns the Value at the top of the stack.
 *
 * PARAMETERS:
 *   stack - Pointer to a valid Stack.
 *
 * BEHAVIOR:
 *   - Removes the rightmost Node.
 *   - Frees the Node structure.
 *
 * RETURNS:
 *   Stored Value if stack is non-empty.
 *   A Value with type EMPTY if stack is empty.
 *
 * NOTE:
 *   Returned data ownership remains with the caller.
 */
Value stackPop(Stack* stack);

/*
 * Removes and returns the Value at the bottom of the stack.
 *
 * PARAMETERS:
 *   stack - Pointer to a valid Stack.
 *
 * BEHAVIOR:
 *   - Removes the leftmost Node.
 *   - Frees the Node structure.
 *
 * RETURNS:
 *   Stored Value if stack is non-empty.
 *   EMPTY Value if stack is empty.
 */
Value stackPopLeft(Stack* stack);

/*
 * Returns the Value at the top of the stack without removing it.
 *
 * PARAMETERS:
 *   stack - Pointer to a valid Stack.
 *
 * RETURNS:
 *   Top Value if stack is non-empty.
 *   EMPTY Value if stack is empty.
 *
 * NOTE:
 *   Does not modify the stack.
 */
Value peekStack(Stack* stack);

/*
 * Returns the Value at the bottom of the stack without removing it.
 *
 * PARAMETERS:
 *   stack - Pointer to a valid Stack.
 *
 * RETURNS:
 *   Bottom Value if stack is non-empty.
 *   EMPTY Value if stack is empty.
 */
Value peekStackLeft(Stack* stack);


/*
 * A debug function which prints all Values stored in the stack,
 * starting from the top (right side) down to the bottom (left side).
 *
 * PARAMETERS:
 *   stack - Pointer to the stack.
 *
 * RETURNS:
 *   STAT_SUCCESS if the stack contains at least one node.
 *   STAT_ERROR if the stack is empty.
 *
 * NOTES:
 *   - The function traverses the stack from stack_pointer_right
 *     to stack_pointer_left using the left_node links.
 *   - Each node's type and value are printed in order.
 *   - The stack structure and its contents are not modified.
 *   - This function is intended for debugging and inspection.
 *
 * WARNING:
 *   - If the stack contains string values (STR), the function assumes
 *     that the stored char* is valid and null-terminated.
 */
Status readStack(Stack* stack);


/*
 * Removes all elements from the stack.
 *
 * PARAMETERS:
 *   stack - Pointer to a valid Stack structure.
 *
 * BEHAVIOR:
 *   - Iteratively pops elements from the stack until it becomes empty.
 *   - Frees all internally allocated Node structures.
 *   - Leaves the Stack in a valid, empty state.
 *
 * RETURNS:
 *   STAT_SUCCESS  if one or more elements were removed.
 *   STAT_FAILURE  if the stack was already empty.
 *   STAT_ERROR    if a NULL stack pointer is provided.
 *
 * NOTE:
 *   - Does not free the Stack structure itself (as it is not heap allocated).
 *   - Safe to call on an already empty stack.
 *   - After successful execution, stack->length == 0.
 */
Status clearStack(Stack* stack);

// Private function prototypes //

/*
 * Converts a DataType enum value into its corresponding
 * human-readable string representation.
 *
 * PARAMETERS:
 *   t - A DataType enumeration value.
 *
 * RETURNS:
 *   A pointer to a string literal representing the given DataType.
 *   Returns "UNKNOWN" if the enum value is not recognized.
 *
 * NOTES:
 *   - The returned string is a string literal and must NOT be modified
 *     or freed by the caller.
 *   - This function does not allocate memory.
 *   - Intended for debugging, logging, and diagnostic output.
 */
const char* typeToString(DataType t);


#endif
