typedef union
{
  QString        *ident;
  KSParseNode    *node;
  KScript::Long   _int;
  QString        *_str;
  ushort          _char;
  KScript::Double _float;
} YYSTYPE;
#define	T_MATCH_LINE	257
#define	T_LINE	258
#define	T_INPUT	259
#define	T_AMPERSAND	260
#define	T_ASTERISK	261
#define	T_CASE	262
#define	T_CHARACTER_LITERAL	263
#define	T_CIRCUMFLEX	264
#define	T_COLON	265
#define	T_COMMA	266
#define	T_CONST	267
#define	T_DEFAULT	268
#define	T_ENUM	269
#define	T_EQUAL	270
#define	T_FALSE	271
#define	T_FLOATING_PT_LITERAL	272
#define	T_GREATER_THAN_SIGN	273
#define	T_IDENTIFIER	274
#define	T_IN	275
#define	T_INOUT	276
#define	T_INTEGER_LITERAL	277
#define	T_LEFT_CURLY_BRACKET	278
#define	T_LEFT_PARANTHESIS	279
#define	T_LEFT_SQUARE_BRACKET	280
#define	T_LESS_THAN_SIGN	281
#define	T_MINUS_SIGN	282
#define	T_OUT	283
#define	T_PERCENT_SIGN	284
#define	T_PLUS_SIGN	285
#define	T_RIGHT_CURLY_BRACKET	286
#define	T_RIGHT_PARANTHESIS	287
#define	T_RIGHT_SQUARE_BRACKET	288
#define	T_SCOPE	289
#define	T_SEMICOLON	290
#define	T_SHIFTLEFT	291
#define	T_SHIFTRIGHT	292
#define	T_SOLIDUS	293
#define	T_STRING_LITERAL	294
#define	T_STRUCT	295
#define	T_SWITCH	296
#define	T_TILDE	297
#define	T_TRUE	298
#define	T_VERTICAL_LINE	299
#define	T_LESS_OR_EQUAL	300
#define	T_GREATER_OR_EQUAL	301
#define	T_ASSIGN	302
#define	T_NOTEQUAL	303
#define	T_MEMBER	304
#define	T_WHILE	305
#define	T_IF	306
#define	T_ELSE	307
#define	T_FOR	308
#define	T_DO	309
#define	T_INCR	310
#define	T_DECR	311
#define	T_MAIN	312
#define	T_FOREACH	313
#define	T_SUBST	314
#define	T_MATCH	315
#define	T_NOT	316
#define	T_RETURN	317
#define	T_IMPORT	318
#define	T_VAR	319
#define	T_CATCH	320
#define	T_TRY	321
#define	T_RAISE	322
#define	T_RANGE	323
#define	T_CELL	324
#define	T_FROM	325
#define	T_PLUS_ASSIGN	326
#define	T_MINUS_ASSIGN	327
#define	T_AND	328
#define	T_OR	329
#define	T_DOLLAR	330
#define	T_UNKNOWN	331


extern YYSTYPE yylval;
