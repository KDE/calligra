typedef union
{
  QString        *ident;
  KSParseNode    *node;
  KScript::Long   _int;
  QString        *_str;
  ushort          _char;
  KScript::Double _float;
} YYSTYPE;
#define	T_FILE_OP	257
#define	T_MATCH_LINE	258
#define	T_LINE	259
#define	T_INPUT	260
#define	T_AMPERSAND	261
#define	T_ASTERISK	262
#define	T_CASE	263
#define	T_CHARACTER_LITERAL	264
#define	T_CIRCUMFLEX	265
#define	T_COLON	266
#define	T_COMMA	267
#define	T_CONST	268
#define	T_DEFAULT	269
#define	T_ENUM	270
#define	T_EQUAL	271
#define	T_FALSE	272
#define	T_FLOATING_PT_LITERAL	273
#define	T_GREATER_THAN_SIGN	274
#define	T_IDENTIFIER	275
#define	T_IN	276
#define	T_INOUT	277
#define	T_INTEGER_LITERAL	278
#define	T_LEFT_CURLY_BRACKET	279
#define	T_LEFT_PARANTHESIS	280
#define	T_LEFT_SQUARE_BRACKET	281
#define	T_LESS_THAN_SIGN	282
#define	T_MINUS_SIGN	283
#define	T_OUT	284
#define	T_PERCENT_SIGN	285
#define	T_PLUS_SIGN	286
#define	T_RIGHT_CURLY_BRACKET	287
#define	T_RIGHT_PARANTHESIS	288
#define	T_RIGHT_SQUARE_BRACKET	289
#define	T_SCOPE	290
#define	T_SEMICOLON	291
#define	T_SHIFTLEFT	292
#define	T_SHIFTRIGHT	293
#define	T_SOLIDUS	294
#define	T_STRING_LITERAL	295
#define	T_PRAGMA	296
#define	T_STRUCT	297
#define	T_SWITCH	298
#define	T_TILDE	299
#define	T_TRUE	300
#define	T_VERTICAL_LINE	301
#define	T_CLASS	302
#define	T_LESS_OR_EQUAL	303
#define	T_GREATER_OR_EQUAL	304
#define	T_ASSIGN	305
#define	T_NOTEQUAL	306
#define	T_MEMBER	307
#define	T_DELETE	308
#define	T_WHILE	309
#define	T_IF	310
#define	T_ELSE	311
#define	T_FOR	312
#define	T_DO	313
#define	T_INCR	314
#define	T_DECR	315
#define	T_MAIN	316
#define	T_FOREACH	317
#define	T_SUBST	318
#define	T_MATCH	319
#define	T_NOT	320
#define	T_RETURN	321
#define	T_SIGNAL	322
#define	T_EMIT	323
#define	T_IMPORT	324
#define	T_VAR	325
#define	T_UNKNOWN	326
#define	T_CATCH	327
#define	T_TRY	328
#define	T_RAISE	329
#define	T_RANGE	330
#define	T_CELL	331
#define	T_FROM	332
#define	T_PLUS_ASSIGN	333
#define	T_MINUS_ASSIGN	334
#define	T_AND	335
#define	T_OR	336
#define	T_DOLLAR	337


extern YYSTYPE yylval;
