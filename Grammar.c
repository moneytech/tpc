/* $Id: Grammar.c,v 1.7 1999/02/08 23:18:26 phelps Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include "Grammar.h"
#include "Kernel.h"
#include "Production.h"
#include "Terminal.h"

struct Grammar_t
{
    /* The number of productions */
    int production_count;

    /* The number of non-terminals in the grammar */
    int nonterminal_count;

    /* The number of terminals in the grammar */
    int terminal_count;

    /* The Grammar's productions */
    Production *productions;

    /* An array which maps Nonterminal indices to a List of Productions */
    List *productionsByNonterminal;

    /* This table records which productions are spontaneously
     * generated by a given nonterminal.  If generates[3][4] is
     * non-zero, then there is a production of the form
     * <nonterminal-3> ::= <nonterminal-4> ... in the grammar */
    char **generates;
};


/*
 *
 * Static functions
 *
 */


/* Mark the box that indicates that non-terminal x generates non-terminal x */
static void MarkGenerates(Grammar self, int x, int y)
{
    int i;

    /* If the box is already marked then we've no further work to do */
    if (self -> generates[x][y])
    {
	return;
    }

    /* Mark the box */
    self -> generates[x][y] = 1;

    /* Propagate the mark to any non-terminal which generates us */
    for (i = 0; i < self -> nonterminal_count; i++)
    {
	if ((self -> generates[i] != NULL) && (self -> generates[i][x] != 0))
	{
	    MarkGenerates(self, i, y);
	}
    }

    /* Propagate the marks of the non-terminal we just generated */
    if (self -> generates[y] != NULL)
    {
	for (i = 0; i < self -> nonterminal_count; i++)
	{
	    if (self -> generates[y][i] != 0)
	    {
		MarkGenerates(self, x, i);
	    }
	}
    }
}

/* Copies the contents of the productions list into the receiver's productions */
static void PopulateProductions(Production production, Grammar self, int *index)
{
    Component component = Production_getFirstComponent(production);
    int i = Production_getNonterminalIndex(production);
    self -> productions[(*index)++] = production;

    /* If this is the first time we've encountered this non-terminal,
     * then update our tables */
    if (self -> productionsByNonterminal[i] == NULL)
    {
	self -> productionsByNonterminal[i] = List_alloc();
	self -> generates[i] = (char *)calloc(self -> nonterminal_count, sizeof(char));
	self -> generates[i][i] = 1;
    }

    /* Append the production to the end of the list for the non-terminal */
    List_addLast(self -> productionsByNonterminal[i], production);

    /* Record a mark in the "generates" table for this non-terminal
     * and first Component of the production if that component is a
     * Nonterminal */
    if (Component_isNonterminal(component))
    {
	int j = Nonterminal_getIndex((Nonterminal)component);
	MarkGenerates(self, i, j);
    }
}


/* Match two integers */
static int Equals(int x, int y)
{
    return x == y;
}

/* Updates the goto table for the given production and offset */
static void UpdateGoto(Grammar self, List *table, Production production, int offset)
{
    Component component = Production_getComponent(production, offset);
    int pair;
    int index;

    /* Figure out what the index should be */
    if (Component_isNonterminal(component))
    {
	index = Nonterminal_getIndex((Nonterminal) component);
    }
    else
    {
	index = self -> nonterminal_count + Terminal_getIndex((Terminal) component);
    }

    /* Compute the encoded pair */
    pair = Grammar_encode(self, production, offset + 1);

    /* Make sure the table has a List at that index */
    if (table[index] == NULL)
    {
	table[index] = List_alloc();
    }
    /* Make sure the item isn't already in the List */
    else
    {
 	if (List_findFirstWith(table[index], (ListFindWithFunc)Equals, (void *)pair) != NULL)
	{
	    return;
	}
    }

    /* Append the encoded item to the list */
    List_addLast(table[index], (void *) pair);
}

/* Populates the Goto table with the Productions at offset 0 */
static void PopulateGotoTable(Production production, Grammar self, List *table)
{
    UpdateGoto(self, table, production, 0);
}


/*
 *
 * Exported functions
 *
 */

/* Allocates a new Grammar with the given Productions */
Grammar Grammar_alloc(List productions, int nonterminal_count, int terminal_count)
{
    Grammar self;
    int index = 0;

    /* Allocate space for a new Grammar */
    if ((self = (Grammar) malloc(sizeof(struct Grammar_t))) == NULL)
    {
	fprintf(stderr, "*** Out of memory!\n");
	exit(1);
    }

    /* Set some initial values */
    self -> production_count = List_size(productions);
    self -> nonterminal_count = nonterminal_count;
    self -> terminal_count = terminal_count;

    /* Copy the productions into the receiver */
    self -> productions = (Production *)calloc(self -> production_count, sizeof(Production));
    self -> productionsByNonterminal = (List *)calloc(nonterminal_count, sizeof(List));
    self -> generates = (char **)calloc(nonterminal_count, sizeof(char *));
    List_doWithWith(productions, PopulateProductions, self, &index);

    return self;
}

/* Frees the resources consumed by the receiver */
void Grammar_free(Grammar self)
{
    int index;

    for (index = 0; index < self -> production_count; index++)
    {
	Production_free(self -> productions[index]);
    }

    free(self);
}

/* Pretty-prints the receiver */
void Grammar_debug(Grammar self, FILE *out)
{
    int index;

    fprintf(out, "Grammar %p\n", self);
    for (index = 0; index < self -> nonterminal_count; index++)
    {
	fprintf(out, "  %d ", index);
	List_doWith(self -> productionsByNonterminal[index], Production_print, out);
	fprintf(out, "\n");
    }
}


/* Answers the number of nonterminals in the receiver */
int Grammar_nonterminalCount(Grammar self)
{
    return self -> nonterminal_count;
}

/* Answers the number of terminals in the receiver */
int Grammar_terminalCount(Grammar self)
{
    return self -> terminal_count;
}


/* Computes the contribute of the encoded production/offset (and
 * derived productions) to the goto table */
void Grammar_computeGoto(Grammar self, List *table, int number)
{
    Production production;
    int offset = Grammar_decode(self, number, &production);

    /* If the kernel production has more components, then add them to the list */
    if (offset < Production_getCount(production))
    {
	Component component = Production_getComponent(production, offset);
	UpdateGoto(self, table, production, offset);

	/* If the Component is a nonterminal, then compute the Goto for
	 * all of the productions of all of the non-terminals in the
	 * generates table for this non-terminal */
	if (Component_isNonterminal(component))
	{
	    int index = Nonterminal_getIndex((Nonterminal) component);
	    int j;

	    for (j = 0; j < self -> nonterminal_count; j++)
	    {
		if (self -> generates[index][j] != 0)
		{
		    List list = self -> productionsByNonterminal[j];
		    List_doWithWith(list, PopulateGotoTable, self, table);
		}
	    }
	}
    }
}


/* Encodes a Production and offset in a single integer that sorts nicely */
int Grammar_encode(Grammar self, Production production, int offset)
{
    int count = self -> production_count;
    return (Production_getIndex(production) + 1) - (count * (offset + 1));
}

/* Answers the Production and offset encoded in the integer */
int Grammar_decode(Grammar self, int number, Production *production_return)
{
    int count = self -> production_count;
    *production_return = self -> productions[count + number % count - 1];
    return - (number / count);
}

/* Construct the set of LR(0) states */
void Grammar_getLR0States(Grammar self)
{
    List list = List_alloc();
    List queue = List_alloc();
    Kernel kernel = Kernel_alloc(self, self -> productions[0]);
    int count = self -> nonterminal_count + self -> terminal_count;

    List_addLast(list, kernel);
    List_addLast(queue, kernel);

    /* Loop until we don't have anything left on the queue */
    while ((kernel = List_dequeue(queue)) != NULL)
    {
	Kernel *table;
	int index;

	/* Print the current kernel */
	Kernel_debug(kernel, stdout);

	/* Get the goto table for the kernel */
	table = Kernel_getGotoTable(kernel);

	/* Walk through it to see if we've already got those Kernel items */
	for (index = 0; index < count; index++)
	{
	    Kernel k = table[index];
	    if ((k != NULL) && (List_findFirstWith(list, (ListFindWithFunc)Kernel_equals, k) == NULL))
	    {
		List_addLast(list, k);
		List_addLast(queue, k);
	    }
	}
    }

    printf("%ld states\n", List_size(list));
}
