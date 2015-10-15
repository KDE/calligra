/* This file is part of the KDE project
   Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>

   Contains portions of sqlite3.c licensed under public domain. The author disclaims
   copyright to this source code.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "sqlitefunctions.h"

#include <kdebug.h>

#include <cctype>

static bool tryExec(sqlite3 *db, const char *sql)
{
    return SQLITE_OK == sqlite3_exec(db, sql, 0 /*callback*/,
                                     0 /* 1st argument to callback */, 0 /*err*/);
}

// BEGIN from sqlite3.c
#define sqlite3Toupper(x)   toupper((unsigned char)(x))
#define sqlite3Isalpha(x)   isalpha((unsigned char)(x))

/*
** Compute the soundex encoding of a word.
**
** IMP: R-59782-00072 The soundex(X) function returns a string that is the
** soundex encoding of the string X.
*/
static void soundexFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  char zResult[8];
  const uchar *zIn;
  int i, j;
  static const uchar iCode[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 0, 1, 2, 0, 0, 2, 2, 4, 5, 5, 0,
    1, 2, 6, 2, 3, 0, 1, 0, 2, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 0, 1, 2, 0, 0, 2, 2, 4, 5, 5, 0,
    1, 2, 6, 2, 3, 0, 1, 0, 2, 0, 2, 0, 0, 0, 0, 0,
  };
  Q_ASSERT(argc==1);
  zIn = (uchar*)sqlite3_value_text(argv[0]);
  if( zIn==0 ) zIn = (uchar*)"";
  for(i=0; zIn[i] && !sqlite3Isalpha(zIn[i]); i++){}
  if( zIn[i] ){
    uchar prevcode = iCode[zIn[i]&0x7f];
    zResult[0] = sqlite3Toupper(zIn[i]);
    for(j=1; j<4 && zIn[i]; i++){
      int code = iCode[zIn[i]&0x7f];
      if( code>0 ){
        if( code!=prevcode ){
          prevcode = code;
          zResult[j++] = code + '0';
        }
      }else{
        prevcode = 0;
      }
    }
    while( j<4 ){
      zResult[j++] = '0';
    }
    zResult[j] = 0;
    sqlite3_result_text(context, zResult, 4, SQLITE_TRANSIENT);
  }else{
    /* IMP: R-64894-50321 The string "?000" is returned if the argument
    ** is NULL or contains no ASCII alphabetic characters. */
    sqlite3_result_text(context, "?000", 4, SQLITE_STATIC);
  }
}

bool createCustomSQLiteFunctions(sqlite3 *db)
{
    int eTextRep = SQLITE_UTF8;
#if SQLITE_VERSION_NUMBER >= 3008003
    eTextRep |= SQLITE_DETERMINISTIC;
#endif
    if (!tryExec(db, "SELECT SOUNDEX()")) {
        int res = sqlite3_create_function_v2(
            db,
            "SOUNDEX",
            1, //nArg
            eTextRep,
            0, // pApp
            soundexFunc,
            0, // xStep
            0, // xFinal
            0 // xDestroy
        );
        if (res != SQLITE_OK) {
            return false;
        }
    }
    return true;
}

// END from sqlite3.c
