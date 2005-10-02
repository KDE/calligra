// -*- Mode: c++-mode; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef ATFINTERP_H
#define ATFINTERP_H

#include <qpointarray.h>
#include <qptrlist.h>
#include <qstringlist.h>

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
        QPtrList<Sign> var1;
        QPtrList<Sign> var2;
        QPtrList<Sign> var3;
        QPtrList<Sign> var4;
        QPtrList<Sign> var5;
        QPtrList<Sign> var6;
        QPtrList<Sign> result;
    };

    // constructure - destructure
    ATFInterpreter();
    ~ATFInterpreter();

    // load autoform
    void load(const QString &);

    // get pointarray/attribute list/points
    QPointArray getPointArray(int,int);
    QPtrList<AttribList> getAttribList();

protected:

    // fields
    static const int ST_WIDTH;
    static const int ST_HEIGHT;
    static const int ST_VARIABLE;
    static const int ST_NUMBER;
    static const int ST_OPERATOR;

    // operators
    static const char OP_EQUAL;
    static const char OP_PLUS;
    static const char OP_MINUS;
    static const char OP_MULT;
    static const char OP_DIV;
    static const char COMMENT;

    // block (structure) beginnings - ends
    static const char PNT_BG[];
    static const char X_BG[];
    static const char Y_BG[];
    static const char ATTR_BG[];
    static const char END[];

    // allowed variables
    static const char VAR_1;
    static const char VAR_2;
    static const char VAR_3;
    static const char VAR_4;
    static const char VAR_5;
    static const char VAR_6;
    static const char VAR_X;
    static const char VAR_Y;
    static const char VAR_VARIA;
    static const char VAR_PW;
    static const char VAR_W;
    static const char VAR_H;

    // level (depth) of the syntax
    static const int LEVEL_NULL;
    static const int LEVEL_POINT;
    static const int LEVEL_X;
    static const int LEVEL_Y;
    static const int LEVEL_ATTR;

    // numbers
    static const char NUM_0;
    static const char NUM_1;
    static const char NUM_2;
    static const char NUM_3;
    static const char NUM_4;
    static const char NUM_5;
    static const char NUM_6;
    static const char NUM_7;
    static const char NUM_8;
    static const char NUM_9;

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
    QPtrList<Sign> getVar(const QString&);

    // ********** variables **********

    // list of coordinates and pointers to coordinate/sign/value
    QPtrList<Coord> coordList;
    QPtrList<AttribList> attrLs;
    Coord *coordPtr;
    Sign *signPtr;
    Value *valuePtr;

    // list of lines
    QStringList lines;
};

#endif //ATFINTERP_H
