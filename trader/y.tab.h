#define DOCROOT 257
#define PORT 258
#define CGI 259
#define ALIAS 260
#define MIME 261
#define PIXDIR 262
#define NOT 263
#define EQ 264
#define NEQ 265
#define LEQ 266
#define GEQ 267
#define LE 268
#define GR 269
#define OR 270
#define AND 271
#define IN 272
#define EXIST 273
#define FIRST 274
#define RANDOM 275
#define MAX 276
#define MIN 277
#define WITH 278
#define MAX2 279
#define MIN2 280
#define BOOL 281
#define STRING 282
#define ID 283
#define NUM 284
#define FLOAT 285
typedef union
{
     char valb;
     int vali;
     double vald;
     char *name;
     void *ptr;
} YYSTYPE;
extern YYSTYPE yylval;
