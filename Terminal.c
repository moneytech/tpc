/* $Id: Terminal.c,v 1.5 1999/02/11 07:39:07 phelps Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Terminal.h"
#include "Component.h"

struct Terminal_t
{
    /* The receiver's Component functions */
    ComponentFunctions functions;

    /* The terminal's name */
    char *name;

    /* The terminal's index */
    int index;
};


/*
 *
 * Static function definitions
 *
 */

/* Always returns false */
int False(Terminal self)
{
    return 0;
}

/* Marks the terminals which can appear as the first element of the receiver */
void MarkFirst(Terminal self, Grammar grammar, char *table)
{
    table[self -> index] = 1;
}


/* The method table */
static ComponentFunctions functions =
{
    (PrintMethod) Terminal_print,
    (IsNonterminalMethod) False,
    (MarkFirstMethod) MarkFirst
};



/* Answers a new Terminal */
Terminal Terminal_alloc(char *name, int index)
{
    Terminal self;

    /* Allocates memory for the Terminal */
    if ((self = (Terminal) malloc(sizeof(struct Terminal_t))) == NULL)
    {
	fprintf(stderr, "*** Out of memory\n");
	exit(1);
    }

    self -> functions = functions;
    self -> name = strdup(name);
    self -> index = index;
    return self;
}

/* Frees the resources consumed by the receiver */
void Terminal_free(Terminal self)
{
    free(self);
}

/* Pretty-prints the receiver */
void Terminal_print(Terminal self, FILE *out)
{
    fprintf(out, "%s ", self -> name);
}

/* Answers the receiver's index */
int Terminal_getIndex(Terminal self)
{
    return self -> index;
}

