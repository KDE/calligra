typedef union
{
  QString        *ident;
  KSParseNode    *node;
  KScript::Long   _int;
  QString        *_str;
  ushort          _char;
  KScript::Double _float;
} YYSTYPE;
#define	T_FILE_OP	258
#define	T_MATCH_LINE	259
#define	T_LINE	260
#define	T_INPUT	261
#define	T_AMPERSAND	262
#define	T_ASTERISK	263
#define	T_CASE	264
#define	T_CHARACTER_LITERAL	265
#define	T_CIRCUMFLEX	266
#define	T_COLON	267
#define	T_COMMA	268
#define	T_CONST	269
#define	T_DEFAULT	270
#define	T_ENUM	271
#define	T_EQUAL	272
#define	T_FALSE	273
#define	T_FLOATING_PT_LITERAL	274
#define	T_GREATER_THAN_SIGN	275
#define	T_IDENTIFIER	276
#define	T_IN	277
#define	T_INOUT	278
#define	T_INTEGER_LITERAL	279
#define	T_LEFT_CURLY_BRACKET	280
#define	T_LEFT_PARANTHESIS	281
#define	T_LEFT_SQUARE_BRACKET	282
#define	T_LESS_THAN_SIGN	283
#define	T_MINUS_SIGN	284
#define	T_OUT	285
#define	T_PERCENT_SIGN	286
#define	T_PLUS_SIGN	287
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
#define	T_DELETE	309
#define	T_WHILE	310
#define	T_IF	311
#define	T_ELSE	312
#define	T_FOR	313
#define	T_DO	314
#define	T_INCR	315
#define	T_DECR	316
#define	T_MAIN	317
#define	T_FOREACH	318
#define	T_SUBST	319
#define	T_MATCH	320
#define	T_NOT	321
#define	T_RETURN	322
#define	T_SIGNAL	323
#define	T_EMIT	324
#define	T_IMPORT	325
#define	T_VAR	326
#define	T_UNKNOWN	327
#define	T_CATCH	328
#define	T_TRY	329
#define	T_RAISE	330
#define	T_RANGE	331
#define	T_CELL	332
#define	T_FROM	333
#define	T_PLUS_ASSIGN	334
#define	T_MINUS_ASSIGN	335
#define	T_AND	336
#define	T_OR	337
#define	T_DOLLAR	338


extern YYSTYPE yylval;
