
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "cDataTypes.h"


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


//Public function prototypes
Stack initStack(void);
Status stackPush(Stack* stack, Value data);
Status stackPushLeft(Stack* stack, Value data);
Value stackPop(Stack* stack);
Value stackPopLeft(Stack* stack);
Value peekStack(Stack* stack);
Value peekStackLeft(Stack *stack);
Status clearStack(Stack* stack);

Status readStack(Stack* stack);

//Private function prototypes
const char* typeToString(DataType t);


// ----- Public function declarations ----- //

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
Stack initStack(void) {
    Stack stack = {
        .stack_pointer_right = NULL,
        .stack_pointer_left = NULL,
        .length = 0
    };

    return stack;
}

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
Status stackPush(Stack* stack, Value data) {
    Node* new_node = calloc(1, sizeof(Node));

    if (new_node == NULL) {
        printf("Error in stackPush: node allocation failed.\n");
        return STAT_ERROR;
    }

    if (stack -> length == 0) {
        new_node -> left_node = NULL;
        stack -> stack_pointer_left = new_node;

    } else {
        Node* previous = stack -> stack_pointer_right;
        new_node -> left_node = previous;
        previous -> right_node = new_node;

    }

    new_node -> right_node = NULL;
    new_node -> node_type = data.type;
    new_node -> data = data;

    stack -> length += 1;
    stack -> stack_pointer_right = new_node;

    return STAT_SUCCESS;

}

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
Status stackPushLeft(Stack* stack, Value data) {
    Node* new_node = calloc(1, sizeof(Node));

    if (new_node == NULL) {
        printf("Error in stackPushLeft: node allocation failed.\n");
        return STAT_ERROR;
    }

    if (stack -> length == 0) {
        new_node -> left_node = NULL;
        stack -> stack_pointer_right = new_node;

    } else {
        Node* previous = stack -> stack_pointer_left;
        new_node -> right_node = previous;
        previous -> left_node = new_node;

    }

    new_node -> left_node = NULL;
    new_node -> node_type = data.type;
    new_node -> data = data;

    stack -> length += 1;
    stack -> stack_pointer_left = new_node;

    return STAT_SUCCESS;

}


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
Value stackPop(Stack* stack) {
    if (stack -> length == 0) {
        //printf("Stack: the stack is empty.\n");
        Value empty = {
            .type = EMPTY,
            .as.empty = NULL
        };
        return empty;
    }

    Node* top_node = stack -> stack_pointer_right;
    Value return_data = top_node -> data;

    stack -> length -= 1;

    if (stack -> length == 0) {
        stack -> stack_pointer_left = NULL;
        stack -> stack_pointer_right = NULL;

    } else {
    stack -> stack_pointer_right = top_node -> left_node;

    Node* previous = stack -> stack_pointer_right;
    previous -> right_node = NULL;

    }

    free(top_node);

    return return_data;

}

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
Value stackPopLeft(Stack* stack) {
    if (stack -> length == 0) {
        //printf("Stack: the stack is empty.\n");
        Value empty = {
            .type = EMPTY,
            .as.empty = NULL
        };
        return empty;
    }

    Node* bottom_node = stack -> stack_pointer_left;
    Value return_data = bottom_node -> data;

    stack -> length -= 1;

    if (stack -> length == 0) {
        stack -> stack_pointer_left = NULL;
        stack -> stack_pointer_right = NULL;

    } else {
        stack -> stack_pointer_left = bottom_node -> right_node;

        Node* previous = stack -> stack_pointer_left;
        previous -> left_node = NULL;
    }

    free(bottom_node);

    return return_data;

}

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
Value peekStack(Stack* stack) {
    if (stack -> length == 0) {
        //printf("Stack: the stack is empty.\n");
        Value empty = {
            .type = EMPTY,
            .as.empty = NULL
        };
        return empty;
    }

    Node* top_node = stack -> stack_pointer_right;

    return top_node -> data;

}

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
Value peekStackLeft(Stack* stack) {
    if (stack -> length == 0) {
        //printf("Stack: the stack is empty.\n");
        Value empty = {
            .type = EMPTY,
            .as.empty = NULL
        };
        return empty;
    }

    Node* bottom_node = stack -> stack_pointer_left;

    return bottom_node -> data;

}

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
Status clearStack(Stack* stack) {
    if (stack == NULL) {
        printf("clearStack: NULL stack pointer provided.\n");
        return STAT_ERROR;

    }

    if (stack -> length == 0) {
        //printf("clearStack: the stack is already empty.\n");
        return STAT_FAILURE;

    }

    while (stack -> length > 0) {
        stackPop(stack);

    }

    return STAT_SUCCESS;
}

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
Status readStack(Stack* stack) {
    if (stack -> length == 0) {
        //printf("Stack: the stack is empty.\n");
        return STAT_ERROR;
    }

    Node* current_node = stack -> stack_pointer_right;
    uint64_t index = 0;
    while (current_node != NULL) {
        const char* type = typeToString(current_node -> node_type);

        switch (current_node -> node_type) {
            case U8:
                printf("Node[%llu]:\n<Type: %s>\n<Value %c>\n", index, type, current_node -> data.as.u8);
                break;
            case U32:
                printf("Node[%llu]:\n<Type: %s>\n<Value %u>\n", index, type, current_node -> data.as.u32);
                break;
            case U64:
                printf("Node[%llu]:\n<Type: %s>\n<Value %llu>\n", index, type, current_node -> data.as.u64);
                break;
            case I8:
                printf("Node[%llu]:\n<Type: %s>\n<Value %i>\n", index, type, current_node -> data.as.i8);
                break;
            case I32:
                printf("Node[%llu]:\n<Type: %s>\n<Value %i>\n", index, type, current_node -> data.as.i32);
                break;
            case I64:
                printf("Node[%llu]:\n<Type: %s>\n<Value %lld>\n", index, type, current_node -> data.as.i64);
                break;
            case F32:
                printf("Node[%llu]:\n<Type: %s>\n<Value %f>\n", index, type, current_node -> data.as.f32);
                break;
            case F64:
                printf("Node[%llu]:\n<Type: %s>\n<Value %lf>\n", index, type, current_node -> data.as.f64);
                break;
            case STR:
                printf("Node[%llu]:\n<Type: %s>\n<Value %s>\n", index, type, current_node -> data.as.str);
                break;
            case EMPTY:
                printf("Node[%llu]:\n<Type: %s>\n<Value NULL>\n", index, type);
                break;
        }

        current_node = current_node -> left_node;
        index++;

    }

    return STAT_SUCCESS;

}

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
const char* typeToString(DataType t) {
    switch (t) {
        case U8: return "U8";
        case U32: return "U32";
        case U64: return "U64";
        case I8: return "I8";
        case I32: return "I32";
        case I64: return "I64";
        case F32: return "F32";
        case F64: return "F64";
        case STR: return "STR";
        case EMPTY: return "EMPTY";
        default: return "UNKNOWN";
    }
}
