typedef union 
{
  QString        *ident;
  KSParseNode    *node;
  KScript::Long   _int;
  QString        *_str;
  ushort          _char;
  KScript::Double _float;
} YYSTYPE;
#define	T_AMPERSAND	257
#define	T_ASTERISK	258
#define	T_ATTRIBUTE	259
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
#define	T_INTERFACE	276
#define	T_LEFT_CURLY_BRACKET	277
#define	T_LEFT_PARANTHESIS	278
#define	T_LEFT_SQUARE_BRACKET	279
#define	T_LESS_THAN_SIGN	280
#define	T_MINUS_SIGN	281
#define	T_OUT	282
#define	T_PERCENT_SIGN	283
#define	T_PLUS_SIGN	284
#define	T_RAISES	285
#define	T_READONLY	286
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
#define	T_NEW	308
#define	T_DELETE	309
#define	T_WHILE	310
#define	T_IF	311
#define	T_ELSE	312
#define	T_FOR	313
#define	T_DO	314
#define	T_INCR	315
#define	T_DECR	316
#define	T_SPEC_OPEN	317
#define	T_SPEC_CLOSE	318
#define	T_MAIN	319
#define	T_FOREACH	320
#define	T_SUBST	321
#define	T_MATCH	322
#define	T_NOT	323
#define	T_RETURN	324
#define	T_SIGNAL	325
#define	T_EMIT	326
#define	T_IMPORT	327
#define	T_VAR	328
#define	T_UNKNOWN	329
#define	T_CATCH	330
#define	T_TRY	331
#define	T_RAISE	332


extern YYSTYPE yylval;
