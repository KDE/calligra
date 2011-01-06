/*
 * Ajouter une pile contenant tout les environnements.
 *
 * To compil :
 * flex -+ -otexparser.cc texparser.lex
 * g++ -o texparser texparser.cc -lfl -lm
 *
 */

%{
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include "stack.h"

/* Stack handling */
void pop(int);
void push(int);

int ind = 0;

%}

%x MATHMODE ENV

commande \\[a-zA-Z]+{param}?{option}?{group}?

group \{[^\{]*\}

option \[.*\]

param \(.*\)

b_math \\\(
e_math \\\)
math \$

b_env \\begin\{{letter}+\}
e_env \\end\{{letter}+\}

display \$\$

comments \%.*\n

ws [ \n\t]
space ({ws}|\~|\\space)
texte [^\\\n\$\%]+

symbol ("$"("\\"{atoz}+|.)"$"|"\\#"|"\\$"|"\\%"|"\\ref")
letter [A-Za-z]

%%
<INITIAL,ENV>{b_env} {
	printf("Entre dans un env. : %s\n", yytext);
	push(ENV);
	BEGIN(ENV);
}

<ENV>{e_env} {
	printf("Sort d'un env.\n");
	pop(ENV);
	if(stackp == 0)
		BEGIN(INITIAL);
}

<INITIAL,ENV>{commande} {
	printf("Commande : %s\n", yytext);
	printf("%d\n", ind);
	ind++;
}

<INITIAL,ENV>"\n" {
	printf("Nouvelle ligne\n");
}

<MATHMODE>{math} {
	printf("Sort du mode math\n");
	pop(MATH);
	BEGIN(INITIAL);
}

<INITIAL,ENV>{math} {
	printf("Entre dans le mode math\n");
	push(MATH);
	BEGIN(MATHMODE);
}


<INITIAL,ENV>{comments} {
	printf("commentaire : %s\n", yytext);
}

<INITIAL,ENV>{texte} {
	printf("texte : %s\n", yytext);
}

<MATHMODE>{texte} {
	printf("texte mathematique : %s\n", yytext);
}

%%


void push(int name)
{
	if(stackp == 0)
	{
		fprintf(stdout, "init. stack!\n");
		stack = (Stack *) malloc(stack_size * sizeof(Stack));
	}
    if ( stackp == stack_size )
	{
		/* extend stack */
		stack_size *= 2;
		stack = (Stack *) realloc(stack, stack_size * sizeof(Stack));
		if ( stack == NULL )
		{
			fprintf(stderr, "texparser: stack out of memory");
			exit(3);
	    }
		fprintf(stdout, "%d", stack);
    }
    
    /*if ( (stack[stackp].name =
		(int *) malloc(strlen(name) + 1)) == NULL )
	{
		fprintf(stderr, "texparser: out of memory\n");
		exit(3);
    }*/

    stack[stackp].name = name;
	fprintf(stdout, "type added in stack : %d\n", name);
    ++stackp;
}


void pop(int name)
{
    if ( stack == 0 )
    {
       	fprintf(stderr, "texparser: Stack underflow\n");
		exit(4);
    }

	if(stack[stackp - 1].name == name)
	{
    	--stackp;
	    //free(stack[stackp].name);
	}
	else
	{
		fprintf(stderr, "texparser : Bad env.\n");
	}
}

