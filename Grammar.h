/* $Id: Grammar.h,v 1.1 1999/02/08 16:30:39 phelps Exp $
 *
 * A Grammar is a collection of Productions which, together with a
 * starting non-terminal, construe a language.  The Grammar can be
 * used to construct a set of parse tables with which a Parser may be
 * constructed
 */

#ifndef GRAMMAR_H
#define GRAMMAR_H

typedef struct Grammar_t *Grammar;

#include "List.h"

/* Allocates a new Grammar with the given Productions */
Grammar Grammar_alloc(List productions, int nonterminal_count, int terminal_count);

/* Frees the resources consumed by the receiver */
void Grammar_free(Grammar self);

/* Prints debugging information about the receiver */
void Grammar_debug(Grammar self, FILE *out);


#endif /* GRAMMAR_H */
