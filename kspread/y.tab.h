#define NUM 257
#define RANGE 258
#define BOOL 259
#define ID 260
#define WENN 261
#define LEQ 262
#define GEQ 263
#define NEQ 264
#define NOT 265
#define OR 266
#define AND 267
typedef union
{
     double dbl;
     char b;
     char* name;
     void* range;
     void* param;
} YYSTYPE;
extern YYSTYPE yylval;
