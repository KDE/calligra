/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef ATFINTERP_H
#define ATFINTERP_H

#include <qpointarray.h>
#include <qlist.h>
#include <qstringlist.h>

/******************************************************************/
/* class ATFInterpreter                                           */
/******************************************************************/

class ATFInterpreter {

public:

  // structure of an attribute list
  struct AttribList
  {
    bool isVariable;
    int pwDiv;
  };

  // coordinate structure
  struct CoordStruct
  {
    QString a;
    QString b;
    QString c;
    QString d;
    QString e;
    QString f;
    QString result;
  };

  // attribute structure
  struct AttribStruct
  {
    QString isVariable;
    QString pwDiv;
  };

  //point structure
  struct PointStruct
  {
    CoordStruct x;
    CoordStruct y;
    AttribStruct attrib;
  };

  // structure of signs
  struct Sign
  {
    char op;
    int num;
    char var;
    int type;
  };

  // structure of values
  struct Value
  {
    QList<Sign> var1;
    QList<Sign> var2;
    QList<Sign> var3;
    QList<Sign> var4;
    QList<Sign> var5;
    QList<Sign> var6;
    QList<Sign> result;
  };

  // constructure - destructure
  ATFInterpreter();
  ~ATFInterpreter();

  // load autoform
  void load(const QString &);

  // get pointarray/attribute list/points
  QPointArray getPointArray(int,int);
  QList<AttribList> getAttribList();

protected:

  // fields
  static const int ST_WIDTH    = 1;
  static const int ST_HEIGHT   = 2;
  static const int ST_VARIABLE = 3;
  static const int ST_NUMBER   = 4;
  static const int ST_OPERATOR = 5;

  // operators
  static const char OP_EQUAL   = '=';
  static const char OP_PLUS    = '+';
  static const char OP_MINUS   = '-';
  static const char OP_MULT    = '*';
  static const char OP_DIV     = '/';
  static const char COMMENT    = '#';

  // block (structure) beginnings - ends
  static const char PNT_BG[];
  static const char X_BG[];
  static const char Y_BG[];
  static const char ATTR_BG[];
  static const char END[];

  // allowed variables
  static const char VAR_1      = 'a';
  static const char VAR_2      = 'b';
  static const char VAR_3      = 'c';
  static const char VAR_4      = 'd';
  static const char VAR_5      = 'e';
  static const char VAR_6      = 'f';
  static const char VAR_X      = 'x';
  static const char VAR_Y      = 'y';
  static const char VAR_VARIA  = 'v';
  static const char VAR_PW     = 'p';
  static const char VAR_W      = 'w';
  static const char VAR_H      = 'h';

  // level (depth) of the syntax
  static const int LEVEL_NULL  = 0;
  static const int LEVEL_POINT = 1;
  static const int LEVEL_X     = 2;
  static const int LEVEL_Y     = 3;
  static const int LEVEL_ATTR  = 4;

  // numbers
  static const char NUM_0      = '0';
  static const char NUM_1      = '1';
  static const char NUM_2      = '2';
  static const char NUM_3      = '3';
  static const char NUM_4      = '4';
  static const char NUM_5      = '5';
  static const char NUM_6      = '6';
  static const char NUM_7      = '7';
  static const char NUM_8      = '8';
  static const char NUM_9      = '9';

  // structure of coordinates
  struct Coord
  {
    Value pntX;
    Value pntY;
    bool isVariable;
    int pwDiv;
  };

  // interpret the code
  void interpret();

  // get varaible of a line
  QList<Sign> getVar(QString);

  // ********** variables **********

  // list of coordinates and pointers to coordinate/sign/value
  QList<Coord> coordList;
  QList<AttribList> attrLs;
  Coord *coordPtr;
  Sign *signPtr;
  Value *valuePtr;

  // list of lines and points and pointer to a point
  QStringList lines;
  QList<PointStruct> pointList;
  PointStruct *pntPtr;

};

#endif //ATFINTERP_H
