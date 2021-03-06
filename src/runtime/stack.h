#ifndef RUNTIME_PARSE_STACK_H_
#define RUNTIME_PARSE_STACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "runtime/array.h"
#include "runtime/tree.h"
#include "runtime/error_costs.h"
#include <stdio.h>

typedef struct Stack Stack;

typedef unsigned StackVersion;
#define STACK_VERSION_NONE ((StackVersion)-1)

typedef struct {
  TreeArray trees;
  StackVersion version;
} StackSlice;
typedef Array(StackSlice) StackSliceArray;

typedef struct {
  Length position;
  unsigned depth;
  TSStateId state;
} StackSummaryEntry;
typedef Array(StackSummaryEntry) StackSummary;

// Create a stack.
Stack *ts_stack_new(TreePool *);

// Release the memory reserved for a given stack.
void ts_stack_delete(Stack *);

// Get the stack's current number of versions.
uint32_t ts_stack_version_count(const Stack *);

// Get the state at the top of the given version of the stack. If the stack is
// empty, this returns the initial state, 0.
TSStateId ts_stack_state(const Stack *, StackVersion);

// Get the number of trees that have been pushed to a given version of
// the stack.
unsigned ts_stack_push_count(const Stack *, StackVersion);

// In the event that trees were permanently removed from some version
// of the stack, decrease the version's push count to account for the
// removal.
void ts_stack_decrease_push_count(Stack *, StackVersion, unsigned);

// Get the last external token associated with a given version of the stack.
Tree *ts_stack_last_external_token(const Stack *, StackVersion);

// Set the last external token associated with a given version of the stack.
void ts_stack_set_last_external_token(Stack *, StackVersion, Tree *);

// Get the position of the given version of the stack within the document.
Length ts_stack_position(const Stack *, StackVersion);

// Push a tree and state onto the given version of the stack.
//
// This transfers ownership of the tree to the Stack. Callers that
// need to retain ownership of the tree for their own purposes should
// first retain the tree.
void ts_stack_push(Stack *, StackVersion, Tree *, bool, TSStateId);

// Pop the given number of entries from the given version of the stack. This
// operation can increase the number of stack versions by revealing multiple
// versions which had previously been merged. It returns an array that
// specifies the index of each revealed version and the trees that were
// removed from that version.
StackSliceArray ts_stack_pop_count(Stack *, StackVersion, uint32_t count);

// Remove an error at the top of the given version of the stack.
StackSliceArray ts_stack_pop_error(Stack *, StackVersion);

// Remove any pending trees from the top of the given version of the stack.
StackSliceArray ts_stack_pop_pending(Stack *, StackVersion);

// Remove any all trees from the given version of the stack.
StackSliceArray ts_stack_pop_all(Stack *, StackVersion);

unsigned ts_stack_depth_since_error(Stack *, StackVersion);

int ts_stack_dynamic_precedence(Stack *, StackVersion);

// Compute a summary of all the parse states near the top of the given
// version of the stack and store the summary for later retrieval.
void ts_stack_record_summary(Stack *, StackVersion, unsigned max_depth);

// Retrieve a summary of all the parse states near the top of the
// given version of the stack.
StackSummary *ts_stack_get_summary(Stack *, StackVersion);

unsigned ts_stack_error_cost(const Stack *, StackVersion version);

bool ts_stack_merge(Stack *, StackVersion, StackVersion);

bool ts_stack_can_merge(Stack *, StackVersion, StackVersion);

void ts_stack_force_merge(Stack *, StackVersion, StackVersion);

void ts_stack_halt(Stack *, StackVersion);

bool ts_stack_is_halted(Stack *, StackVersion);

void ts_stack_renumber_version(Stack *, StackVersion, StackVersion);

void ts_stack_swap_versions(Stack *, StackVersion, StackVersion);

StackVersion ts_stack_copy_version(Stack *, StackVersion);

// Remove the given version from the stack.
void ts_stack_remove_version(Stack *, StackVersion);

void ts_stack_clear(Stack *);

bool ts_stack_print_dot_graph(Stack *, const char **, FILE *);

typedef void (*StackIterateCallback)(void *, TSStateId, uint32_t);

void ts_stack_iterate(Stack *, StackVersion, StackIterateCallback, void *);

#ifdef __cplusplus
}
#endif

#endif  // RUNTIME_PARSE_STACK_H_
