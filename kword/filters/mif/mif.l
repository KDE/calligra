/* $Id$
 *
 * This file is part of QtMFC, the MFC API for the Qt toolkit.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

%{
extern int lineno;
extern char curfile[_POSIX_PATH_MAX];
extern "C" int keyword( char* string );
%}

ws				[ \t]+
pcreal          -?[0-9]+\.[0-9]+\ pc
inchreal		-?[0-9]+\.[0-9]+\"
real			-?[0-9]+\.[0-9]+
integer			-?[0-9]+
comment			#.*$
qstring			\"[^\"\n]*\"
sqstring        \`[^\']*\'          
id				[_a-zA-Z][\._a-zA-Z0-9]*
externaldata	^&.*$
externaldatamarker	^=.*$
nl				\n
%%

{ws}			;
{real}			{ sscanf( yytext, "%lf", &yylval.real );
			      return REAL; }
{inchreal}		{ sscanf( yytext, "%lf\"", &yylval.real );
			      return INCHREAL; }
{pcreal}		{ sscanf( yytext, "%lf\"", &yylval.real );
			      return PCREAL; }
{integer}		{ sscanf( yytext, "%d", &yylval.integer );
                  return INTEGER; }
{qstring}		{ yylval.string = yytext;
				  return QSTRING; }
{sqstring}      { yylval.string = yytext;
                  return SQSTRING; }
{id}			{ if( yylval.cmd = keyword( yytext) ) {
						return yylval.cmd; }
				  yylval.string = yytext;
				  return ID; }
{comment} 		;
{externaldata}	;
{externaldatamarker}	;
{nl}			{ lineno++; }
.				{ return yytext[0]; }

%%

char **fileList;
unsigned currentFile = 0;
unsigned nFiles;
unsigned long totalCC = 0;
unsigned long totalWC = 0;
unsigned long totalLC = 0;

/*
 * the lexer calls yywrap to handle EOF conditions (e.g., to
 * connect to a new file, as we do in this case.)
 */

yywrap()
{
#ifdef K
	FILE *file;

	if ((currentFile != 0) && (nFiles > 1) && (currentFile < nFiles)) {
		/*
		 * we print out the statistics for the previous file.
		 */
		printf("%8lu %s\n", lineno, fileList[currentFile-1]);
		totalLC += lineno;
		lineno = 0;
		fclose(yyin);	/* done with that file */
	}

	while (fileList[currentFile] != (char *)0) {
		file = fopen(fileList[currentFile++], "r");
		if (file != NULL) {
			yyin = file;
			break;
		}
		fprintf(stderr,
			"could not open %s\n",
			fileList[currentFile-1]);
	}
	return (file ? 0 : 1);	/* 0 means there's more input */
#endif
return 1;
}
