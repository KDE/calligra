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
#define OR 268
#define AND 269
#define IN 270
#define EXIST 271
#define FIRST 272
#define RANDOM 273
#define MAX 274
#define MIN 275
#define WITH 276
#define BOOL 277
#define STRING 278
#define ID 279
#define NUM 280
#define FLOAT 281
typedef union
{
     char valb;
     int vali;
     double vald;
     char *name;
     void *ptr;
} YYSTYPE;
extern YYSTYPE yylval;
