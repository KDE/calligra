/*
** A parser for XML.
**
** Copyright (C) 1998 D. Richard Hipp
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
** Author contact information:
**   drh@acm.org
**   http://www.hwaci.com/drh/
*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "kword_xml2html.h"

/*
** Every token is one of the following types.
*/
// typedef enum TokenType {
//   TT_Word,        /* Anything besides markup and whitespace */
//   TT_Space,       /* Spaces, not counting "\n" characters */
//   TT_EOL,         /* A sequence of spaces ending with "\n" */
//   TT_Markup       /* Any markup sequence */
// } TokenType;

// /*
// ** Each token is represented by an instance of the following structure.
// */
// typedef struct Token Token;
// struct Token {
//   int offset;         /* Offset from start of file.  Used for error messages */
//   TokenType eType;    /* What type of token is this. */
//   char *zText;        /* The actual text of the token */
//   Token *pNext;       /* Next token in the list */
// };

// /*
// ** Arguments to a markup token are recorded as a list of the
// ** following structure.
// */
// typedef struct Arg Arg;
// struct Arg {
//   int offset;      /* Index of first char of name -- for error messages */
//   char *zName;     /* Name of argument */
//   char *zValue;    /* Value of the argument */
//   Arg *pNext;      /* Next argument in the list, or NULL */
// };

// /*
// ** Markup tokens have a few extra fields, shown by this structure.
// */
// typedef struct Markup Markup;
// struct Markup {
//   Token token;       /* Base class.  Must be first */
//   Token *pContent;   /* Tokens between <MARKUP> and </MARKUP> */
//   Arg *pArg;         /* Arguments to this token */
// };

/*
** The maximum length of a line of output for error messages.
*/
#define LINELEN 80

/*
** The number of errors encountered
*/
static int nError = 0;

/*
** Mark code that should never execute with the CANT_HAPPEN macro.
** Also define our own assert macro.
*/
#define CANT_HAPPEN  AssertFailed(__FILE__,__LINE__)
#ifndef ASSERT
#define ASSERT(X)    if(!(X)){AssertFailed(__FILE__,__LINE__);}
#endif
static void AssertFailed(const char *zFile, int line){
  fprintf(stderr,"Assertion filed on line %d of %s.\n", line, zFile);
  exit(1);
}

/*
** Open and read a file into memory.  Return a pointer
** to the buffer containing the text of this file.  The buffer will
** be null-terminated.  The calling function is responsible for reclaiming
** storage using free().
**
** Inputs:
** The name of the file to be read.
**
** Outputs:
** A pointer to a null-terminated string that contains the complete
** text of the file.  If the file could not be opened for any reason,
** an error message is issued and a NULL pointer is returned.
*/
char *ReadFileIntoMemory(const char *fileName){
  FILE *in;             /* Input file stream */
  char *textBuf;        /* A buffer in which to put entire text of input */
  int toRead;           /* Amount of input file read to read */
  int got;              /* Amount read so far */
  struct stat statBuf;  /* Status buffer for the file */

  in = fopen(fileName,"rb");
  if( in==0 ){
    fprintf(stderr,"%s: can't open file.\n",fileName);
    nError++;
    return 0;
  }
  if( fstat(fileno(in),&statBuf)!=0 ){
    fprintf(stderr,"%s: fstat() failed -- can't get size of file.\n", fileName);
    fclose(in);
    nError++;
    return 0;
  }
  textBuf = (char*) malloc( statBuf.st_size + 1 );
  if( textBuf==0 ){
    fprintf(stderr,"%s: can't malloc %ld bytes to hold this file.\n",
      fileName, statBuf.st_size + 1);
    fclose(in);
    nError++;
    return 0;
  }
  textBuf[statBuf.st_size] = 0;
  toRead = statBuf.st_size;
  got = 0;
  while( toRead ){
    int n = fread(&textBuf[got],1,toRead,in);
    if( n<=0 ) break;
    toRead -= n;
    got += n;
  }
  fclose(in);
  if( got!=statBuf.st_size ){
    fprintf(stderr,"%s: only %d of %ld bytes read from this file.\n",
      fileName, got, statBuf.st_size);
    free(textBuf);
    nError++;
    return 0;
  }
  textBuf[got] = 0;
  return textBuf;
}

/*
** Print an error message across multiple lines (if necessary) so that
** no line exceeds LINELEN characters in length.  Put the given prefix at
** the beginning of each line.
**
** Lines of output are broken at space or "-" characters
** where possible.
**
** Inputs:
**   *  Text to be printing at the beginning of every line.
**   *  The error message text.
*/
static void BreakLines(char *zPrefix, char *zErr){
  int i;                 /* For looping through the error message */
  int lenPrefix;         /* Length of the prefix */
  int lenErr;            /* Length of the error message */

  lenPrefix = zPrefix ? strlen(zPrefix) : 0;
  lenErr = strlen(zErr);
  for(;;){
    /*
    ** The complete error message fits on one line.
    */
    if( lenPrefix + lenErr < LINELEN ){
      fprintf(stderr,"%s%s\n",zPrefix,zErr);
      return;
    }

    /*
    ** The error message won't fit, so try to break it at a
    ** space or '-' character.
    */
    i = LINELEN - lenPrefix;
    while( i>=0 ){
      if( zErr[i]==' ' ){
        fprintf(stderr,"%s%.*s\n",zPrefix,i,zErr);
        break;
      }else if( zErr[i]=='-' ){
        fprintf(stderr,"%s%.*s\n",zPrefix,i+1,zErr);
        break;
      }
      i--;
    }

    /*
    ** The error message doesn't contain a space or '-'.  We'll
    ** just have to split it in the middle of a word.  (Bummer)
    */
    if( i<0 ){
      i = LINELEN - lenPrefix;
      fprintf(stderr,"%s%.*s\n",zPrefix, i, zErr);
    }

    /*
    ** Prepare variables to deal with the rest of the error message
    ** that didn't fit on this line, then loop again.
    */
    zErr += i+1;
    lenErr -= i+1;
  }
}

/*
** This routine prints an error message associated with a particular
** place in a file.  The line of the file on which the error occurred
** is printed together with the line number and a pointer to the position
** on the line where the error occurred.
**
** Consider an example:
**
**    line 16:  PRIM_IN   = [I]GX, [J]GY, [2..8,I]GZ
**            Undefined variable ---^
**
** In this example, the format string specifies an error message
** of ``Undefined variable'' and the lenSpot parameter
** is 1.  In the next example, the lenSpot parameter is 10:
**
**    line 11:%GIP( [1..10]GX : INT I=1..10 INITAILIZE [I]GX TO I*2 )
**                          syntax error ---^^^^^^^^^^
**
** Inputs:
**   *  The complete text of the file that contains the error.
**   *  The name of the file that contains the error.
**   *  A pointer to the string of the 2nd parameter to the beginning of
**      the text that is in error.
**   *  The length of the text in the input file that is wrong.
**   *  A format string for the error message.  Note that the error
**      message is rendered into a static buffer (of size about 1000)
**      so the format string should take care not to overflow this buffer.
**   *  A variable argument list to accompany the format string.
*/
static void ErrorAtCharV(
  const char *zFile,         /* Complete text of file containing error */
  int offset,                /* Error begins at zFile[offset] */
  int lenSpot,               /* Number of incorrect characters */
  const char *zFormat,       /* Format string for the error message */
  va_list ap                 /* Remaining arguments for vsprintf */
){
  int lineno;                /* Line number in the file where error occurs */
  int charno;                /* Character where error starts.  left margin=0 */
  const char *z;             /* For scanning thru the file */
  char *z2;                  /* For scanning thru zErr and zBuf */
  const char *zLineStart;    /* Start of the line containing the error */
  int lenLine;               /* Length of the line containing the error */
  int lenErr;                /* Length of the error message text */
  int lenPrefix;             /* Length of the prefix string */
  int i;                     /* Loop counter */
  char zBuf[LINELEN+100];    /* Staging buffer */
  char zPrefix[LINELEN+100]; /* Prefix string for this error message */
  char zErr[1000];           /* Text of the error message */
  static char zUp[] =
    "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";

  lineno = 1;
  charno = 0;
  zLineStart = zFile;
  for(i=0, z=zFile; *z && i<offset; z++, i++){
    if( *z=='\n' ){
      zLineStart = &z[1];
      lineno++;
      charno = 0;
    }else{
      charno++;
    }
  }
  lenLine = charno;
  while( zLineStart[lenLine] && zLineStart[lenLine]!='\n' ){ lenLine++; }
  sprintf(zPrefix,"line %d: ",lineno);
  lenPrefix = strlen(zPrefix);
  if( lenPrefix + lenLine >= LINELEN ){
    while( *zLineStart==' ' && charno>0 ){
      zLineStart++;
      charno--;
      lenLine--;
    }
  }
  if( lenSpot >= (LINELEN*3)/4 - lenPrefix ){
    lenSpot = 1;
  }
  if( lenPrefix + lenLine >= 80 ){
    if( charno + lenPrefix + lenSpot < LINELEN-5 ){
      lenLine = (LINELEN - 1) - lenPrefix;
    }else{
      int shift = charno + lenPrefix + lenSpot - (LINELEN-5);
      zLineStart += shift;
      lenLine -= shift;
      charno -= shift;
    }
  }
  if( lenPrefix + lenLine >= LINELEN ){
    lenLine = (LINELEN-1) - lenPrefix;
  }
  vsprintf(zErr,zFormat,ap);
  z2 = strchr(zErr,'\n');
  if( z2 ) *z2 = 0;
  lenErr = strlen(zErr);

  sprintf(zBuf,"%s%.*s\n",zPrefix,lenLine,zLineStart);
  for(z2=zBuf; *z2; z2++){
    if( *z2=='\t' ){
       *z2 = ' ';
    }
  }
  fprintf(stderr,"%s",zBuf);
  sprintf(zPrefix,"%*s",lenPrefix, "");
  if( charno + lenSpot + lenPrefix + lenErr + 4 < LINELEN ){
    /* Error message to the right of the error.
    **
    **   test2.spgn:14:  IQ3 : CFLOAT,
    **                   ^^^--- Unknown Variable.
    */
    fprintf(stderr,"%s%*s%.*s--- %s\n",
       zPrefix, charno, "", lenSpot, zUp, zErr);
  }else if( charno >= lenErr + 4 ){
    /* Error message to the left of the error.
    **
    **   test2.spgn:14:  PRIM_IN = V3, V4, [1,5]Q3  THRESHOLD = 5.2
    **                             Threshold must be integer ---^^^
    */
    fprintf(stderr,"%s%*s%s ---%.*s\n",
      zPrefix, charno - (lenErr + 4), "", zErr, lenSpot, zUp);
  }else if( lenErr < LINELEN - lenPrefix ){
    /* Error message centered below the error.
    **
    **   test2.spgn:14:  PRIM_IN = V3, V4, [1,5]Q3  THRESHOLD = 5.2
    **                                     ^^^^^
    **                    Only two indices specified on a 3-D family
    */
    int start;    /* Start of the error message */
    start = (lenPrefix + charno + lenSpot/2) - (lenErr/2);
    if( start < lenPrefix ){
      start = lenPrefix;
    }else if( start + lenErr > LINELEN ){
      start = LINELEN - lenErr;
    }
    fprintf(stderr,"%s%*s%.*s\n",zPrefix,charno,"",lenSpot, zUp);
    fprintf(stderr,"%s%*s%s\n", zPrefix, start-lenPrefix, "", zErr);
  }else{
    /* Multi-line error messages:
    **
    **   test2.spgn:14:  PRIM_IN = V3, V4, [1,5]Q3  VARIABLE THRESHOLD = 5.2
    **                                              ^^^^^^^^
    **                 The Threshold NEP cannot be variable.  Thresholds must be
    **                 start-time scalar integer quantities.
    */
    fprintf(stderr,"%s%*s%.*s\n",zPrefix,charno,"",lenSpot, zUp);
    BreakLines(zPrefix,zErr);
  }
}

/*
** This is a version of ErrorAtCharV that takes a variable number
** of arguments.
*/
void ErrorAtChar(
  const char *zFile,         /* Complete text of file containing error */
  int offset,                /* Error begins at zFile[offset] */
  int lenSpot,               /* Number of incorrect characters */
  const char *zFormat,       /* Format string for the error message */
  ...                        /* Remaining arguments for vsprintf */
){
  va_list ap;          /* Variable argument list */

  va_start(ap,zFormat);
  ErrorAtCharV(zFile, offset, lenSpot, zFormat, ap);
  va_end(ap);
}

/****************** Begin Character Entity Translator *************/
/*
** The next section of code implements routines used to translate
** character entity references into their corresponding strings.
** Examples:
**
**         &amp;          "&"
**         &lt;           "<"
**         &gt;           ">"
**         &nbsp;         " "
*/

/* Each entity reference is recorded as an instance of the following
** structure
*/
typedef struct Er Er;
struct Er {
  const char *zName;     /* The name of this entity reference.  ex:  "amp" */
  const char *zValue;    /* The value for this entity.          ex:  "&" */
  Er *pNext;       /* Next entity with the same hash on zName */
};

/* The size of the hash table.  For best results this should
** be a prime number which is about the same size as the number of
** character entity references known to the system. */
#define ER_HASH_SIZE 7

/* The following flag is TRUE if entity reference hash table needs
** to be initialized.
*/
static int bErNeedsInit = 1;

/* The hash table
**
** If the name of an entity reference hashes to the value H, then
** apErHash[H] will point to a linked list of Er structures, one of
** which will be the Er structure for that entity reference
*/
static Er *apErHash[ER_HASH_SIZE];

/* Hash an entity reference name.  The value returned is an integer
** between 0 and Er_HASH_SIZE-1, inclusive.
*/
static int ErHash(const char *zName){
  int h = 0;      /* The hash value to be returned */
  char c;         /* The next character in the name being hashed */

  while( (c=*zName)!=0 ){
    h = h<<5 ^ h ^ c;
    zName++;
  }
  if( h<0 ) h = -h;
  return h % ER_HASH_SIZE;
}

/* The following is a table of all entity references.  To create
** new character entities, add entries to this table.
**
** Note: For the decoder to work, the name of the entity reference
** must not be shorter than the value.
*/
static Er er_sequences[] = {
  { "amp",       "&",        0 },
  { "lt",        "<",        0 },
  { "gt",        ">",        0 },
  { "apos",      "'",        0 },
  { "quot",      "\"",       0 },
  { "nbsp",      " ",        0 },
};

/* Initialize the entity reference hash table
*/
static void ErInit(void){
  int h;  /* The hash on a entity */

  for(unsigned int i=0; i<sizeof(er_sequences)/sizeof(er_sequences[0]); i++){
    h = ErHash(er_sequences[i].zName);
    er_sequences[i].pNext = apErHash[h];
    apErHash[h] = &er_sequences[i];
  }
}

/* Translate entity references in the string "z".  "z" is overwritten
** with the translated sequence.
**
** Unrecognized entity references are unaltered.
**
** Example:
**
**      input =    "AT&amp;T &gt MCI"
**      output =   "AT&T > MCI"
*/
void TranslateEntityRefs(char *z){
  int from;   /* Read characters from this position in z[] */
  int to;     /* Write characters into this position in z[] */
  int h;      /* A hash on the entity reference */
  const char *zVal; /* The substituted value */
  Er *p;      /* For looping down the entity reference collision chain */

  from = to = 0;
  if( bErNeedsInit ){
    ErInit();
    bErNeedsInit = 0;
  }
  while( z[from] ){
    if( z[from]=='&' ){
      int i = from+1;
      int c;
      while( z[i] && isalpha(z[i]) ){ i++; }
      c = z[i];
      z[i] = 0;
      h = ErHash(&z[from+1]);
      p = apErHash[h];
      while( p && strcmp(p->zName,&z[from+1])!=0 ){ p = p->pNext; }
      z[i] = c;
      if( p ){
        zVal = (char *) p->zValue;
        while( *zVal ){
          z[to++] = *(zVal++);
        }
        from = i;
        if( c==';' ) from++;
      }else{
        z[to++] = z[from++];
      }
    }else{
      z[to++] = z[from++];
    }
  }
  z[to] = 0;
}
/******************* End Character Entity Reference Translator ***************/

/*
** Allocate memory.  This routine will never return unsuccessfully.
** If sufficent memory can not be allocated, an error message is
** printed and the process aborts.
*/
void *SafeMalloc(int nByte){
  void *x = malloc( nByte );
  if( x==0 ){
    fprintf(stderr,"Unable to allocate %d bytes of memory.\n", nByte);
    exit(1);
  }
  return x;
}
#define SafeFree free

/* An array to map all upper-case characters into their corresponding
** lower-case character.
*/
unsigned char UpperToLowerCase[] = {
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
     18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
     36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
     54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99,100,101,102,103,
    104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
    122, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,
    108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,
    126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
    144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,
    162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
    180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,
    198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,
    216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,
    234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,
    252,253,254,255
};

/* Compare strings without case.  Strings of digits compare
** in numerical order */
int StrICmp(const char *atext, const char *btext){
  register unsigned char *a, *b, *map, ca, cb;
  int result;

  map = UpperToLowerCase;
  a = (unsigned char *)atext;
  b = (unsigned char *)btext;
  do{
    if( (ca=map[*a++])!=(cb=map[*b++]) ) break;
  }while( ca!=0 );
  if( isdigit(ca) ){
    if( !isdigit(cb) ){
      result = 1;
    }else{
      int acnt, bcnt;
      acnt = bcnt = 0;
      while( isdigit(*a++) ) acnt++;
      while( isdigit(*b++) ) bcnt++;
      result = acnt - bcnt;
      if( result==0 ) result = ca-cb;
    }
  }else if( isdigit(cb) ){
    result = -1;
  }else{
    result = ca - cb;
  }
  return result;
}

/* Compare strings without case.  Strings of digits compare
** in numerical order.  No more than "n" characters are compared */
int StrNICmp(const char *atext, const char *btext, int n){
  register unsigned char *a, *b, *map, ca, cb;
  int result;

  map = UpperToLowerCase;
  a = (unsigned char *)atext;
  b = (unsigned char *)btext;
  if( n>0 ){
    do{
      if( (ca=map[*a++])!=(cb=map[*b++]) ) break;
    }while( ca!=0 && n-- >1 );
  }
  if( n>0 ){
    if( isdigit(ca) ){
      if( !isdigit(cb) ){
        result = 1;
      }else{
        int acnt, bcnt;
        acnt = bcnt = 1;
        while( acnt<n && isdigit(*a++) ) acnt++;
        while( bcnt<n && isdigit(*b++) ) bcnt++;
        result = acnt - bcnt;
        if( result==0 ) result = ca-cb;
      }
    }else if( isdigit(cb) ){
      result = -1;
    }else{
      result = ca - cb;
    }
  }else{
    result = 0;
  }
  return result;
}

/*
** Copy N character from zSrc to zDest, then add a null terminator
** onto zDest.  Convert each character copied to upper case.
*/
static void StrUNCpy(char *zDest, const char *zSrc, int N){
  int c;
  while( N-- && (c=*(zSrc++))!=0 ){
    *(zDest++) = (islower(c) ? toupper(c) : c);
  }
  *zDest = 0;
}

/*
** Parse the XML for a file beginning at zFile[*piFile] and continuing
** to the first unmatched </...> tag.  Return a pointer to the parsed
** XML.  The *piFile integer is left pointing at the character that
** terminated the parse.
*/
Token *ParseXml(
  const char *zFile,  /* Complete text of the file being parsed */
  int *piFile         /* Index of next unparsed character in zFile */
){
  int i;              /* For looping thru the file */
  Token *pHead = 0;   /* Head of the token list to be returned */
  Token *pTail = 0;   /* Tail of the list to be returned */
  Token *p;           /* The next token to be constructed */
  Markup *pM;         /* The next markup token */
  int c;              /* Next character of the input file */
  int go = 1;         /* Keep looping until this becomes 0 */

  i = *piFile;
  while( go && (c=zFile[i])!=0 ){
    int iStart = i;
    if( isspace(c) ){
      while( (c=zFile[i])!='\n' && isspace(c) ){ i++; }
      if( c=='\n' ){
        p = (Token*) SafeMalloc( sizeof(Token) + (i - iStart) + 2 );
        p->offset = iStart;
        p->eType = TT_EOL;
        p->zText = (char*)&p[1];
        strncpy(p->zText, &zFile[iStart], i - iStart + 1);
        p->zText[i - iStart + 1] = 0;
        i++;
      }else{
        p = (Token*) SafeMalloc( sizeof(Token) + (i - iStart) + 1 );
        p->offset = iStart;
        p->eType = TT_Space;
        p->zText = (char*)&p[1];
        strncpy(p->zText, &zFile[iStart], i - iStart);
        p->zText[i - iStart] = 0;
      }
    }else if( c!='<' ){
      while( (c=zFile[i])!=0 && !isspace(c) && c!='<' ){ i++; }
      p = (Token*) SafeMalloc( sizeof(Token) + (i - iStart) + 1 );
      p->offset = iStart;
      p->eType = TT_Word;
      p->zText = (char*)&p[1];
      strncpy(p->zText, &zFile[iStart], i - iStart);
      p->zText[i - iStart] = 0;
      TranslateEntityRefs(p->zText);
    }else if( zFile[i+1]=='/' ){
      go = 0;
      continue;
    }else{
      int j, n;
      int hasContent = 1;
      i++;
      if( zFile[i]=='!' && zFile[i+1]=='-' && zFile[i+2]=='-' ){
        i += 3;
        while( (c=zFile[i])!=0 &&
            (c!='-' || zFile[i+1]!='-' || zFile[i+2]!='>')){ i++; }
        if( zFile[i] ){
          i += 3;
        }else{
          ErrorAtChar(zFile, iStart, 4, "Unterminated comment");
          nError++;
        }
        continue;
      }
      while( (c=zFile[i])!=0 && c!='>' && !isspace(c) ){ i++; }
      if( c==0 ){
        ErrorAtChar(zFile, iStart, i-iStart, "Missing \">\"");
        nError++;
        continue;
      }
      n = i - iStart - 1;
      if( n==0 ){
        ErrorAtChar(zFile, iStart, 1, "Null markup name");
        nError++;
        continue;
      }
      pM = (Markup*) SafeMalloc( sizeof(Markup) + n + 1 );
      pM->token.offset = iStart;
      pM->token.eType = TT_Markup;
      pM->token.zText = (char*)&pM[1];
      for(j=0; j<n; j++){
        c = zFile[iStart+j+1];
        if( islower(c) ){
          pM->token.zText[j] = toupper(c);
        }else{
          pM->token.zText[j] = c;
        }
      }
      pM->token.zText[n] = 0;
      pM->pContent = 0;
      pM->pArg = 0;
      while( isspace(zFile[i]) ){ i++; }
      while( zFile[i] && zFile[i]!='>' && zFile[i]!='/' ){
        int iArgName = i;
        int nArgName;
        int iArgVal = 0;
        int nArgVal = 0;
        Arg *pArg;
        while( (c=zFile[i])!=0 && c!='=' && c!='>' && !isspace(c) ){ i++; }
        nArgName = i - iArgName;
        while( isspace(zFile[i]) ){ i++; }
        if( zFile[i]=='=' ){ i++; }
        while( isspace(zFile[i]) ){ i++; }
        if( zFile[i]=='>' || zFile[i]==0 ){
          iArgVal = iArgName;
          nArgVal = nArgName;
        }else if( (c=zFile[i])=='\"' || c=='\'' ){
          int cDelim = c;
          i++;
          iArgVal = i;
          while( (c=zFile[i])!=0 && c!=cDelim ){ i++; }
          nArgVal = i - iArgVal;
          if( c==0 ){
            ErrorAtChar(zFile, iArgVal-1, 1, "Unterminated string");
            nError++;
          }else{
            i++;
          }
        }else if( c!=0 && c!='>' ){
          iArgVal = i;
          while( (c=zFile[i])!=0 && c!='>' && !isspace(c) ){ i++; }
          if( c==0 ){
            ErrorAtChar(zFile, iStart, 1, "Missing \">\"");
            nError++;
          }
          nArgVal = i - iArgVal;
        }
        pArg = (Arg*) SafeMalloc( sizeof(Arg) + nArgName + nArgVal + 2 );
        pArg->offset = iArgName;
        pArg->zName = (char*)&pArg[1];
        StrUNCpy(pArg->zName, &zFile[iArgName], nArgName);
        pArg->zValue = &pArg->zName[nArgName+1];
        strncpy(pArg->zValue, &zFile[iArgVal], nArgVal);
        pArg->zValue[nArgVal] = 0;
        TranslateEntityRefs(pArg->zValue);
        pArg->pNext = pM->pArg;
        pM->pArg = pArg;
        while( isspace(zFile[i]) ){ i++; }
      }
      if( zFile[i]=='/' ){
        hasContent = 0;
        i++;
        if( zFile[i]!='>' ){
          ErrorAtChar(zFile,i-1,1, "Syntax Error");
          nError++;
          while( zFile[i] && zFile[i]!='>' ){ i++; }
        }
      }
      if( zFile[i]=='>' ){ i++; }
      if( hasContent && pM->token.zText[n-1]=='/' ){
        pM->token.zText[n-1] = 0;
        hasContent = 0;
      }
      if( hasContent ){
        pM->pContent = ParseXml(zFile, &i);
        if( zFile[i]!='<' || zFile[i+1]!='/'
            || StrNICmp(&zFile[i+2], pM->token.zText, n)!=0 ){
          ErrorAtChar(zFile, pM->token.offset, 1, "Unterminated element");
          ErrorAtChar(zFile, i-1, 1, "Inserted \"</%s>\" here",
            pM->token.zText);
          nError++;
          continue;
        }
        i += n+2;
        while( isspace(zFile[i]) ){ i++; }
        if( zFile[i]=='>' ){
          i++;
        }else{
          ErrorAtChar(zFile, i-1, 1, "Missing \">\"");
          nError++;
        }
      }
      p = &pM->token;
    }
    if( p ){
      p->pNext = 0;
      if( pTail==0 ){
        pHead = pTail = p;
      }else{
        pTail->pNext = p;
        pTail = p;
      }
    }
  }
  if( pTail ){
    pTail->pNext = 0;
  }
  *piFile = i;
  return pHead;
}

/*
** Recursively delete all elements of an XML parse tree.
*/
void DeleteXml(Token *p){
  Token *pNext;
  while( p ){
    pNext = p->pNext;
    if( p->eType==TT_Markup ){
      Markup *pM = (Markup*)p;
      Arg *pArg;
      Arg *pNextArg;
      for(pArg=pM->pArg; pArg; pArg=pNextArg){
        pNextArg = pArg->pNext;
        free(pArg);
      }
      DeleteXml(pM->pContent);
    }
    free(p);
    p = pNext;
  }
}

/*
** Print an entire XML parse tree.  This routine is used for
** testing and debugging purposes only.
*/
void PrintXml(Token *p, int indent){
  Markup *pM;
  Arg *pArg;
  while( p ){
    switch( p->eType ){
      case TT_Word:
        printf("%*sWord   \"%s\"\n", indent, "", p->zText);
        break;
      case TT_Space:
        printf("%*sSpace  \"%s\"\n", indent, "", p->zText);
        break;
      case TT_EOL:
        printf("%*sEnd Of Line\n", indent, "");
        break;
      case TT_Markup:
        printf("%*sMarkup <%s", indent, "", p->zText);
        pM = (Markup*)p;
        for(pArg = pM->pArg; pArg; pArg=pArg->pNext){
          printf(" %s=\"%s\"", pArg->zName, pArg->zValue);
        }
        printf(">\n");
        PrintXml(pM->pContent, indent+3);
        break;
      default:
        CANT_HAPPEN;
        break;
    }
    p = p->pNext;
  }
}

/*
** This routine returns a pointer to the value of an argument
** to a markup.  If the argument is not on the markup, the
** default value specified by the 3rd argument is returned instead.
*/
char *GetArg(Markup *pM, const char *zArgName, char *zDefault){
  Arg *p;
  if( pM==0 || zArgName==0 || *zArgName==0 ) return zDefault;
  for(p=pM->pArg; p; p=p->pNext){
    if( StrICmp(p->zName,zArgName)==0 ){
      return p->zValue;
    }
  }
  return zDefault;
}

/*
** Find the first instance of particular markup.
*/
Markup *FindMarkup(Token *p, const char *zName){
  while( p ){
    if( p->eType==TT_Markup ){
      Markup *pM = (Markup*)p;
      if( StrICmp(pM->token.zText,zName)==0 ){
        return pM;
      }
    }
    p = p->pNext;
  }
  return 0;
}

/*
** Test is a token is a particular type of markup
*/
int IsMarkup(Token *p, const char *zName){
  Markup *pM = (Markup*)p;
  return p && p->eType==TT_Markup && StrICmp(pM->token.zText,zName)==0;
}

/*
** Return the first non-space token in the list
*/
Token *SkipSpace(Token *p){
  while( p && (p->eType==TT_Space || p->eType==TT_EOL) ){
    p = p->pNext;
  }
  return p;
}

/*
** Scan the list of tokens given and extract a list of words.
** The return value is a pointer to an array of pointers to
** strings.  The calling function should invoke free() on the
** returned value to reclaim memory.  The array is null-terminated.
**
** The words can be all space-separated.  Or they can be comma-separated.
** If there is any comma anywhere in the list, then it is assumed
** that comma separation is used, otherwise use space separation.
*/
char **WordList(Token *pList, int *pN){
  Token *p;
  int n;
  int nByte;
  char *z;
  char **az;
  int usesCommas = 0;
  int len;

  n = nByte = 0;
  for(p=pList; p; p=p->pNext){
    if( p->eType!=TT_Word ) continue;
    n++;
    len = strlen(p->zText);
    nByte += len + 1;
    if( len>0 && p->zText[len-1]==',' ){ usesCommas = 1; }
  }
  z = (char*) malloc( nByte + (n+1)*sizeof(char*) );
  if( z==0 ){
    if( pN ) *pN = 0;
    return 0;
  }
  az = (char**)z;
  z += (n+1)*sizeof(char*);
  n = 0;
  *z = 0;
  for(p=pList; p; p=p->pNext){
    if( p->eType!=TT_Word ) continue;
    if( *z==0 ){
      az[n++] = z;
    }else{
      strcat(z," ");
    }
    strcat(z, p->zText);
    len = strlen(z);
    if( !usesCommas ){
      z += len + 1;
      *z = 0;
    }else if( len>0 && z[len-1]==',' ){
      z[len-1] = 0;
      z += len;
      *z = 0;
    }
  }
  az[n] = 0;
  if( pN ) *pN = n;
  return az;
}

#ifdef TEST1
void main(int argc, char **argv){
  int i;
  for(i=1; i<argc; i++){
    char *zFile;
    Token *p;
    zFile = ReadFileIntoMemory(argv[i]);
    if( zFile ){
      int j = 0;
      p = ParseXml(zFile,&j);
      PrintXml(p, 0);
      DeleteXml(p);
    }
  }
}
#endif


/* Compare strings case sensitive.  Strings of digits compare
** in numerical order */
int StrCmp(const char *atext,const char *btext){
  register unsigned char *a, *b, ca, cb;
  int result;

  a = (unsigned char *)atext;
  b = (unsigned char *)btext;
  do{
    if( (ca= *a++)!=(cb= *b++) ) break;
  }while( ca!=0 );
  if( isdigit(ca) ){
    if( !isdigit(cb) ){
      result = 1;
    }else{
      int acnt, bcnt;
      acnt = bcnt = 0;
      while( isdigit(*a++) ) acnt++;
      while( isdigit(*b++) ) bcnt++;
      result = acnt - bcnt;
      if( result==0 ) result = ca-cb;
    }
  }else if( isdigit(cb) ){
    result = -1;
  }else{
    result = ca - cb;
  }
  return result;
}

/* Make a copy of a string
*/
char *StrDup(char *zOrig){
  char *zNew = (char*) SafeMalloc( strlen(zOrig)+1 );
  strcpy(zNew,zOrig);
  return zNew;
}

/* Compare strings case sensitive.  Strings of digits compare
** in numerical order.  No more than "n" characters are compared */
int StrNCmp(const char *atext, const char *btext, int n){
  register unsigned char *a, *b, ca, cb;
  int result;

  a = (unsigned char *)atext;
  b = (unsigned char *)btext;
  if( n>0 ){
    do{
      if( (ca= *a++)!=(cb= *b++) ) break;
    }while( ca!=0 && n-- >1 );
  }
  if( n>0 ){
    if( isdigit(ca) ){
      if( !isdigit(cb) ){
        result = 1;
      }else{
        int acnt, bcnt;
        acnt = bcnt = 1;
        while( acnt<n && isdigit(*a++) ) acnt++;
        while( bcnt<n && isdigit(*b++) ) bcnt++;
        result = acnt - bcnt;
        if( result==0 ) result = ca-cb;
      }
    }else if( isdigit(cb) ){
      result = -1;
    }else{
      result = ca - cb;
    }
  }else{
    result = 0;
  }
  return result;
}

/* Compare two Arg structures.  Used for sorting
*/
static int CompareArg(Arg *pA, Arg *pB){
  return StrICmp(pA->zName,pB->zName);
}

/*
** Merge two sorted lists of Arg structures into a single
** sorted list and return a pointer to the new list
*/
static Arg *MergeArgList(Arg *pA, Arg *pB){
  Arg *pList = 0;
  Arg **ppEnd = &pList;
  Arg *p;
  while( pA && pB ){
    if( CompareArg(pA,pB)<=0 ){
      p = pA;
      pA = pA->pNext;
    }else{
      p = pB;
      pB = pB->pNext;
    }
    p->pNext = 0;
    *ppEnd = p;
    ppEnd = &p->pNext;
  }
  if( pA==0 ){
    *ppEnd = pB;
  }else{
    *ppEnd = pA;
  }
  return pList;
}

/*
** Use the merge-sort algorithm to sort a linked list of
** Arg structures.  Return a pointer to the new list.
*/
static Arg *SortArg(Arg *pList){
  int i;
  Arg *p;
  Arg *aBucket[30];
  int nBucket;

  nBucket = sizeof(aBucket)/sizeof(aBucket[0]);
  for(i=0; i<nBucket; i++){ aBucket[i] = 0; }
  while( pList ){
    p = pList;
    pList = pList->pNext;
    p->pNext = 0;
    for(i=0; i<nBucket-1 && aBucket[i]; i++){
      p = MergeArgList(aBucket[i],p);
      aBucket[i] = 0;
    }
    aBucket[i] = MergeArgList(aBucket[i],p);
  }
  p = 0;
  for(i=0; i<nBucket; i++){
    p = MergeArgList(aBucket[i],p);
  }
  return p;
}

/*
** Issue an error message if a markup contains an argument that is
** not given on the list.
*/
void RestrictArgs(char *zFile, Markup *pM, ...){
  int i, N;
  va_list ap;
  Arg *p;
  char *z;
  char *azAllow[500];

  va_start(ap,pM);
  z = "";
  for(N=0; z!=0 && N<sizeof(azAllow)/sizeof(azAllow[0]); N++) {
    z = va_arg(ap,char*);
    azAllow[N] = z;
  }
  va_end(ap);
  for(p=pM->pArg; p; p=p->pNext){
    for(i=0; i<N-1; i++){
      if( StrICmp(azAllow[i],p->zName)==0 ) break;
    }
    if( i>=N ){
      ErrorAtChar(zFile, p->offset, 1, "Unrecognized argument");
      nError++;
    }
  }
}
