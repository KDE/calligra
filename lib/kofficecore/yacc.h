typedef union
{
     char valb;
     int vali;
     double vald;
     char *name;
     void *ptr;
} YYSTYPE;
#define	NOT	258
#define	EQ	259
#define	NEQ	260
#define	LEQ	261
#define	GEQ	262
#define	LE	263
#define	GR	264
#define	OR	265
#define	AND	266
#define	IN	267
#define	EXIST	268
#define	MAX	269
#define	MIN	270
#define	BOOL	271
#define	STRING	272
#define	ID	273
#define	NUM	274
#define	FLOAT	275


extern YYSTYPE yylval;
