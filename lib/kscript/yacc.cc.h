typedef union 
{
  QString        *ident;
  KSParseNode    *node;
  KScript::Long   _int;
  QString        *_str;
  ushort          _char;
  KScript::Double _float;
} YYSTYPE;
#define	T_AMPERSAND	258
#define	T_ASTERISK	259
#define	T_ATTRIBUTE	260
#define	T_CASE	261
#define	T_CHARACTER_LITERAL	262
#define	T_CIRCUMFLEX	263
#define	T_COLON	264
#define	T_COMMA	265
#define	T_CONST	266
#define	T_DEFAULT	267
#define	T_ENUM	268
#define	T_EQUAL	269
#define	T_FALSE	270
#define	T_FLOATING_PT_LITERAL	271
#define	T_GREATER_THAN_SIGN	272
#define	T_IDENTIFIER	273
#define	T_IN	274
#define	T_INOUT	275
#define	T_INTEGER_LITERAL	276
#define	T_INTERFACE	277
#define	T_LEFT_CURLY_BRACKET	278
#define	T_LEFT_PARANTHESIS	279
#define	T_LEFT_SQUARE_BRACKET	280
#define	T_LESS_THAN_SIGN	281
#define	T_MINUS_SIGN	282
#define	T_OUT	283
#define	T_PERCENT_SIGN	284
#define	T_PLUS_SIGN	285
#define	T_RAISES	286
#define	T_READONLY	287
#define	T_RIGHT_CURLY_BRACKET	288
#define	T_RIGHT_PARANTHESIS	289
#define	T_RIGHT_SQUARE_BRACKET	290
#define	T_SCOPE	291
#define	T_SEMICOLON	292
#define	T_SHIFTLEFT	293
#define	T_SHIFTRIGHT	294
#define	T_SOLIDUS	295
#define	T_STRING_LITERAL	296
#define	T_PRAGMA	297
#define	T_STRUCT	298
#define	T_SWITCH	299
#define	T_TILDE	300
#define	T_TRUE	301
#define	T_VERTICAL_LINE	302
#define	T_CLASS	303
#define	T_LESS_OR_EQUAL	304
#define	T_GREATER_OR_EQUAL	305
#define	T_ASSIGN	306
#define	T_NOTEQUAL	307
#define	T_MEMBER	308
#define	T_NEW	309
#define	T_DELETE	310
#define	T_WHILE	311
#define	T_IF	312
#define	T_ELSE	313
#define	T_FOR	314
#define	T_DO	315
#define	T_INCR	316
#define	T_DECR	317
#define	T_SPEC_OPEN	318
#define	T_SPEC_CLOSE	319
#define	T_MAIN	320
#define	T_FOREACH	321
#define	T_SUBST	322
#define	T_MATCH	323
#define	T_NOT	324
#define	T_RETURN	325
#define	T_SIGNAL	326
#define	T_EMIT	327
#define	T_IMPORT	328
#define	T_VAR	329
#define	T_UNKNOWN	330
#define	T_CATCH	331
#define	T_TRY	332
#define	T_RAISE	333
#define	T_RANGE	334
#define	T_CELL	335


extern YYSTYPE yylval;
