#include<FlexLexer.h>

#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>

#include "stack.h"

int main(int argc, char* argv[] )
{
	const char * file_name = strdup(argv[1]);
	ifstream input;
	input.open(file_name, ios::in);
	yyFlexLexer* parser = new yyFlexLexer(&input);
	
	/* allocate initial stack */
    stack = (Stack *) malloc(stack_size * sizeof(Stack));

    if ( stack == NULL)
	{
		cerr << "texparser: not enough memory for stacks\n";
		return 3;
    }

	while(parser->yylex() != 0);

	return 0;
}
