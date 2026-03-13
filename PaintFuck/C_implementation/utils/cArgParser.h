/*
 * #-------------------------------#
 * |                               |
 * |     Custom argument parser    |
 * |         implementation        |
 * |      Gábor Bakos (UBE2C)      |
 * |                               |
 * #-------------------------------#
 *
 */


// ----- Include guard ----- //
#ifndef CARGPARSER_H
#define CARGPARSER_H


// ----- Necessary includes ----- //
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "cDataTypes.h"


// ----- Enum and struct definitions ----- //

/*
 * Describes the type of a CLI argument.
 *
 *   POSITIONAL - A bare value identified by its position among non-flag tokens.
 *   OPTIONAL   - A flag-based argument (e.g. -o / --output) followed by a value.
 *   BOOLEAN    - A flag-based argument (e.g. -v / --verbose) that carries no value.
 *                Its presence is recorded via the matched field.
 *   ARG_UNKNOWN - Sentinel / uninitialized state.
 */
typedef enum ArgType {
    POSITIONAL,
    OPTIONAL,
    BOOLEAN,
    ARG_UNKNOWN

} ArgType;

/*
 * Represents a single registered CLI argument.
 *
 * Stores:
 *   - The argument type (POSITIONAL, OPTIONAL, or BOOLEAN)
 *   - For POSITIONAL args: the 1-based position index among non-flag tokens
 *   - For OPTIONAL/BOOLEAN args: short and long flag strings (e.g. "-o", "--output")
 *   - Whether the argument is required
 *   - Whether the argument is mutually exclusive with another argument
 *   - The long_flag of the exclusive counterpart (if applicable)
 *   - Whether the argument was matched during parsing
 *   - The parsed value string (NULL for unmatched or BOOLEAN args)
 *
 * Argument structs are not heap allocated — they are stored in a
 * caller-provided array managed by ArgLstManager.
 */
typedef struct Argument {
    ArgType     type;
    int32_t     position;
    const char* short_flag;
    const char* long_flag;
    bool        required;
    bool        exclusive;
    bool        matched;
    const char* exclusive_flags;
    const char* value;

} Argument;

/*
 * Manages a fixed-size array of registered Argument entries.
 *
 * Stores:
 *   - A pointer to the caller-provided Argument array
 *   - The total capacity of the array (len)
 *   - The index of the next free slot (next_arg)
 *
 * The ArgLstManager does not allocate memory — it operates on
 * an Argument array provided by the caller at initialization.
 */
typedef struct ArgLstManager {
    int32_t   len;
    uint32_t  next_arg;
    Argument* args_array;

} ArgLstManager;


// ----- Function prototypes ----- //
// Public function prototypes //

/*
 * Initializes and returns an empty ArgLstManager.
 *
 * PARAMETERS:
 *   arg_list - Pointer to a caller-allocated array of Argument structs.
 *   n_args   - The number of entries in arg_list (capacity).
 *
 * RETURNS:
 *   A properly initialized ArgLstManager structure.
 *
 * NOTE:
 *   This function does not allocate memory.
 *   It returns an ArgLstManager by value.
 *   The caller is responsible for ensuring arg_list remains valid
 *   for the lifetime of the manager.
 *
 * EXAMPLE:
 *   Argument args[3];
 *   ArgLstManager manager = initArgManager(args, 3);
 */
ArgLstManager initArgManager(Argument* arg_list, int32_t n_args);

/*
 * Registers a new argument in the manager.
 *
 * PARAMETERS:
 *   arg_list_manager - Pointer to an initialized ArgLstManager.
 *   type             - The argument type: POSITIONAL, OPTIONAL, or BOOLEAN.
 *   position         - For POSITIONAL args: the 1-based position index among
 *                      non-flag tokens. Pass -1 for OPTIONAL and BOOLEAN args.
 *   short_flag       - Short flag string (e.g. "-o"). May be NULL for POSITIONAL args.
 *   long_flag        - Long flag string or positional name (e.g. "--output", "input_file").
 *   required         - If true, parseArgs will error if this argument has no value
 *                      and no exclusive pair is present.
 *   exclusive        - If true, this argument is mutually exclusive with the
 *                      argument identified by exclusive_flags.
 *   exclusive_flags  - The long_flag of the mutually exclusive counterpart.
 *                      Pass NULL if exclusive is false.
 *
 * RETURNS:
 *   STAT_SUCCESS on success.
 *   STAT_ERROR   if arg_list_manager is NULL or the manager is full.
 *
 * EXAMPLE:
 *   addArg(&manager, OPTIONAL,   -1, "-o", "--output", true,  false, NULL);
 *   addArg(&manager, BOOLEAN,    -1, "-v", "--verbose", false, false, NULL);
 *   addArg(&manager, POSITIONAL,  1, NULL, "input_file", true, false, NULL);
 */
Status addArg(ArgLstManager* arg_list_manager, ArgType type, int32_t position, const char* short_flag, const char* long_flag, bool required, bool exclusive, const char* exclusive_flags);

/*
 * Parses the command line arguments against the registered argument list.
 *
 * PARAMETERS:
 *   arg_list_manager - Pointer to an initialized ArgLstManager with registered args.
 *   argc             - Argument count as received by main().
 *   argv             - Argument vector as received by main().
 *
 * BEHAVIOR:
 *   - Iterates over argv starting at index 1 (skipping the program name).
 *   - Tokens starting with '-' are matched as OPTIONAL or BOOLEAN args by flag.
 *   - OPTIONAL args consume the following token as their value.
 *   - BOOLEAN args record a match without consuming a value token.
 *   - Remaining tokens are matched as POSITIONAL args by their 1-based
 *     position index among non-flag tokens.
 *   - After parsing, validates that all required args have a value,
 *     unless their exclusive pair was matched instead.
 *   - Checks that no two mutually exclusive args are both present.
 *
 * RETURNS:
 *   STAT_SUCCESS  if all arguments parsed and validated correctly.
 *   STAT_FAILURE  if argc == 1 (no user arguments provided).
 *   STAT_ERROR    if any of the following occur:
 *                   - arg_list_manager is NULL
 *                   - an OPTIONAL flag has no following value token
 *                   - an unrecognised flag is encountered
 *                   - a required argument is missing and has no exclusive pair
 *                   - two mutually exclusive arguments are both present
 *
 * NOTE:
 *   Stored values are pointers into argv — they are not copied.
 *   The caller must ensure argv remains valid for as long as the
 *   parsed values are in use.
 *
 * EXAMPLE:
 *   Status result = parseArgs(&manager, argc, argv);
 *   if (result == STAT_ERROR) { ... }
 */
Status parseArgs(ArgLstManager* arg_list_manager, int32_t argc, char** argv);

/*
 * Retrieves the parsed value of an argument by its flag string.
 *
 * PARAMETERS:
 *   arg_list_manager - Pointer to an initialized ArgLstManager.
 *   flag             - The short or long flag string to look up (e.g. "-o", "--output").
 *
 * RETURNS:
 *   Pointer to the parsed value string if the argument was matched and has a value.
 *   NULL if the argument was not matched, has no value (e.g. BOOLEAN),
 *        the flag is not registered, or either parameter is NULL.
 *
 * NOTE:
 *   The returned pointer points into argv — do not free or modify it.
 *   For BOOLEAN args, use the matched field directly to check presence.
 *
 * EXAMPLE:
 *   const char* output = getArgValue(&manager, "--output");
 *   const char* output = getArgValue(&manager, "-o");
 */
const char* getArgValue(ArgLstManager* arg_list_manager, const char* flag);


// Private function prototypes //

/*
 * Searches the registered argument list for an entry matching the given token.
 *
 * PARAMETERS:
 *   arg_list_manager - Pointer to an initialized ArgLstManager.
 *   arg              - The token string to match (a flag or positional value).
 *   argv_index       - For OPTIONAL/BOOLEAN matching: the current argv index (unused
 *                      for flag matching but passed for interface consistency).
 *                      For POSITIONAL matching: the 1-based positional counter.
 *
 * RETURNS:
 *   The index into args_array of the matched Argument on success.
 *   -1 if no match is found or a parameter is NULL.
 *
 * NOTES:
 *   - OPTIONAL and BOOLEAN args are matched by short_flag or long_flag string comparison.
 *   - POSITIONAL args are matched by position index and only if not already matched.
 *   - This is an internal function and is not intended for direct use by callers.
 */
int32_t matchArg(ArgLstManager* arg_list_manager, const char* arg, int32_t argv_index);

/*
 * Checks whether the exclusive counterpart of a given argument is present and has a value.
 *
 * PARAMETERS:
 *   arg_list_manager - Pointer to an initialized ArgLstManager.
 *   arg_index        - Index of the argument whose exclusive pair should be checked.
 *
 * RETURNS:
 *   STAT_SUCCESS  if the exclusive pair is registered and has a parsed value.
 *   STAT_FAILURE  if the argument is not exclusive, or its pair has no value.
 *   STAT_ERROR    if arg_list_manager is NULL or arg_index is negative.
 *
 * NOTES:
 *   - The exclusive pair is identified by matching the exclusive_flags string
 *     against the short_flag and long_flag of all registered arguments.
 *   - This is an internal function used by parseArgs during validation.
 */
Status isExclusivePairPresent(ArgLstManager* arg_list_manager, int32_t arg_index);

/*
 * Converts an ArgType enum value into its human-readable string representation.
 *
 * PARAMETERS:
 *   type - An ArgType enumeration value.
 *
 * RETURNS:
 *   A pointer to a string literal representing the given ArgType.
 *   Returns "UNKNOWN" if the enum value is not recognized.
 *
 * NOTES:
 *   - The returned string is a string literal and must NOT be modified or freed.
 *   - This function does not allocate memory.
 *   - Intended for debugging and diagnostic output.
 */
const char* printType(ArgType type);


#endif
