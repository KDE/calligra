typedef union
{
     char b;
     double dbl;
     char* name;
     void* range;
     void* param;
} YYSTYPE;
#define	NUM	258
#define	RANGE	259
#define	BOOL	260
#define	ID	261
#define	WENN	262
#define	LEQ	263
#define	GEQ	264
#define	NEQ	265
#define	NOT	266
#define	OR	267
#define	AND	268


extern YYSTYPE deplval;
