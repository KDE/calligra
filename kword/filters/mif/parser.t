#!
#! parser.t: This is a custom template for building a parser
#!
#$ IncludeTemplate("app.t");

####### Lex/yacc programs and options

LEX       =   flex
YACC    =     #$ $text = ($is_unix ? "yacc -d" : "byacc -d");

####### Lex/yacc files

LEXIN   =     #$ Expand("LEXINPUT");
LEXOUT  =     lex.yy.c
YACCIN  =     #$ Expand("YACCINPUT");
YACCOUT =     y.tab.c
YACCHDR =     y.tab.h
PARSER  =     #$ Expand("PARSER");

####### Process lex/yacc files

$(LEXOUT): $(LEXIN)
	$(LEX) $(LEXIN)

$(PARSER): $(YACCIN) $(LEXOUT)
	$(YACC) $(YACCIN)
	#$ $text = ($is_unix ? "-rm -f " : "-del ") . '$(PARSER)';
	#$ $text = ($is_unix ? "-mv " : "-ren ") . '$(YACCOUT) $(PARSER)';
