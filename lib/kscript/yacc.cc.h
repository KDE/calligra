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
#define	T_CASE	260
#define	T_CHARACTER_LITERAL	261
#define	T_CIRCUMFLEX	262
#define	T_COLON	263
#define	T_COMMA	264
#define	T_CONST	265
#define	T_DEFAULT	266
#define	T_ENUM	267
#define	T_EQUAL	268
#define	T_FALSE	269
#define	T_FLOATING_PT_LITERAL	270
#define	T_GREATER_THAN_SIGN	271
#define	T_IDENTIFIER	272
#define	T_IN	273
#define	T_INOUT	274
#define	T_INTEGER_LITERAL	275
#define	T_LEFT_CURLY_BRACKET	276
#define	T_LEFT_PARANTHESIS	277
#define	T_LEFT_SQUARE_BRACKET	278
#define	T_LESS_THAN_SIGN	279
#define	T_MINUS_SIGN	280
#define	T_OUT	281
#define	T_PERCENT_SIGN	282
#define	T_PLUS_SIGN	283
#define	T_RIGHT_CURLY_BRACKET	284
#define	T_RIGHT_PARANTHESIS	285
#define	T_RIGHT_SQUARE_BRACKET	286
#define	T_SCOPE	287
#define	T_SEMICOLON	288
#define	T_SHIFTLEFT	289
#define	T_SHIFTRIGHT	290
#define	T_SOLIDUS	291
#define	T_STRING_LITERAL	292
#define	T_PRAGMA	293
#define	T_STRUCT	294
#define	T_SWITCH	295
#define	T_TILDE	296
#define	T_TRUE	297
#define	T_VERTICAL_LINE	298
#define	T_CLASS	299
#define	T_LESS_OR_EQUAL	300
#define	T_GREATER_OR_EQUAL	301
#define	T_ASSIGN	302
#define	T_NOTEQUAL	303
#define	T_MEMBER	304
#define	T_DELETE	305
#define	T_WHILE	306
#define	T_IF	307
#define	T_ELSE	308
#define	T_FOR	309
#define	T_DO	310
#define	T_INCR	311
#define	T_DECR	312
#define	T_MAIN	313
#define	T_FOREACH	314
#define	T_SUBST	315
#define	T_MATCH	316
#define	T_NOT	317
#define	T_RETURN	318
#define	T_SIGNAL	319
#define	T_EMIT	320
#define	T_IMPORT	321
#define	T_VAR	322
#define	T_UNKNOWN	323
#define	T_CATCH	324
#define	T_TRY	325
#define	T_RAISE	326
#define	T_RANGE	327
#define	T_CELL	328
#define	T_FROM	329


extern YYSTYPE yylval;
