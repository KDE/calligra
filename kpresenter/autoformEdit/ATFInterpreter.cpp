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

#include "ATFInterpreter.h"

#include <q3valuestack.h>
#include <QFile>
//Added by qt3to4:
#include <Q3PointArray>
#include <Q3PtrList>

const char ATFInterpreter::PNT_BG[]   = "POINT {";
const char ATFInterpreter::X_BG[]     = "X {";
const char ATFInterpreter::Y_BG[]     = "Y {";
const char ATFInterpreter::ATTR_BG[]  = "ATTRIB {";
const char ATFInterpreter::END[]       = "}";

// fields
const int ATFInterpreter::ST_WIDTH    = 1;
const int ATFInterpreter::ST_HEIGHT   = 2;
const int ATFInterpreter::ST_VARIABLE = 3;
const int ATFInterpreter::ST_NUMBER   = 4;
const int ATFInterpreter::ST_OPERATOR = 5;

// operators
const char ATFInterpreter::OP_EQUAL   = '=';
const char ATFInterpreter::OP_PLUS    = '+';
const char ATFInterpreter::OP_MINUS   = '-';
const char ATFInterpreter::OP_MULT    = '*';
const char ATFInterpreter::OP_DIV     = '/';
const char ATFInterpreter::COMMENT    = '#';

// allowed variables
const char ATFInterpreter::VAR_1      = 'a';
const char ATFInterpreter::VAR_2      = 'b';
const char ATFInterpreter::VAR_3      = 'c';
const char ATFInterpreter::VAR_4      = 'd';
const char ATFInterpreter::VAR_5      = 'e';
const char ATFInterpreter::VAR_6      = 'f';
const char ATFInterpreter::VAR_X      = 'x';
const char ATFInterpreter::VAR_Y      = 'y';
const char ATFInterpreter::VAR_VARIA  = 'v';
const char ATFInterpreter::VAR_PW     = 'p';
const char ATFInterpreter::VAR_W      = 'w';
const char ATFInterpreter::VAR_H      = 'h';

// level (depth) of the syntax
const int ATFInterpreter::LEVEL_NULL  = 0;
const int ATFInterpreter::LEVEL_POINT = 1;
const int ATFInterpreter::LEVEL_X     = 2;
const int ATFInterpreter::LEVEL_Y     = 3;
const int ATFInterpreter::LEVEL_ATTR  = 4;

// numbers
const char ATFInterpreter::NUM_0      = '0';
const char ATFInterpreter::NUM_1      = '1';
const char ATFInterpreter::NUM_2      = '2';
const char ATFInterpreter::NUM_3      = '3';
const char ATFInterpreter::NUM_4      = '4';
const char ATFInterpreter::NUM_5      = '5';
const char ATFInterpreter::NUM_6      = '6';
const char ATFInterpreter::NUM_7      = '7';
const char ATFInterpreter::NUM_8      = '8';
const char ATFInterpreter::NUM_9      = '9';

/******************************************************************/
/* class ATFInterpreter                                           */
/******************************************************************/

/*====================== constructor =============================*/
ATFInterpreter::ATFInterpreter() {
}

/*======================= destrcutor =============================*/
ATFInterpreter::~ATFInterpreter()
{
    coordList.setAutoDelete(true);
    attrLs.setAutoDelete( true );
}

void ATFInterpreter::load(const QString & fileName)
{
    QString line;
    QFile ptA(fileName);

    coordList.clear();
    lines.clear();

    if (ptA.open(QIODevice::ReadOnly))
    {
        while (!ptA.atEnd())
        {
            ptA.readLine(line,255);
            lines.append(line.simplified());
        }
        ptA.close();
        interpret();
    }
}

Q3PointArray ATFInterpreter::getPointArray(int wid,int heig)
{
    unsigned int px = 0,py = 0,a = 0,b = 0,c = 0,d = 0,e = 0,f = 0;
    unsigned int tmp = 0,num = 0;
    bool calc = false,res = false;
    char op = OP_EQUAL,var = VAR_1;
    Q3PtrList<Sign> slp;
    Q3PointArray pntArray(coordList.count());

    if (!coordList.isEmpty())
    {
        for (coordPtr=coordList.first();coordPtr != 0;coordPtr=coordList.next())
        {
            for (unsigned int i = 1; i<= 14; i++)
            {
                switch (i)
                {
                case 1: slp = coordPtr->pntX.var1; break;
                case 2: slp = coordPtr->pntX.var2; break;
                case 3: slp = coordPtr->pntX.var3; break;
                case 4: slp = coordPtr->pntX.var4; break;
                case 5: slp = coordPtr->pntX.var5; break;
                case 6: slp = coordPtr->pntX.var6; break;
                case 7: slp = coordPtr->pntX.result; break;
                case 8: slp = coordPtr->pntY.var1; break;
                case 9: slp = coordPtr->pntY.var2; break;
                case 10: slp = coordPtr->pntY.var3; break;
                case 11: slp = coordPtr->pntY.var4; break;
                case 12: slp = coordPtr->pntY.var5; break;
                case 13: slp = coordPtr->pntY.var6; break;
                case 14: slp = coordPtr->pntY.result; break;
                }
                if (!slp.isEmpty())
                {
                    tmp = 0;
                    for (signPtr=slp.first();signPtr != 0;signPtr=slp.next())
                    {
                        switch (signPtr->type)
                        {
                        case ST_WIDTH: { num = wid; calc = true; res = false;} break;
                        case ST_HEIGHT: { num = heig; calc = true; res = false;} break;
                        case ST_OPERATOR: { op = signPtr->op; calc = false; res = false;} break;
                        case ST_VARIABLE: { var = signPtr->var; calc = false; res = true;} break;
                        case ST_NUMBER: { num = signPtr->num; calc = true; res = false;} break;
                        }
                        if (calc)
                        {
                            switch (op)
                            {
                            case OP_EQUAL: tmp = num; break;
                            case OP_PLUS: tmp += num; break;
                            case OP_MINUS: tmp -= num; break;
                            case OP_MULT: tmp *= num; break;
                            case OP_DIV: tmp /= num; break;
                            }
                        }
                        else if (res)
                        {
                            switch (var)
                            {
                            case VAR_1: num = a; break;
                            case VAR_2: num = b; break;
                            case VAR_3: num = c; break;
                            case VAR_4: num = d; break;
                            case VAR_5: num = e; break;
                            case VAR_6: num = f; break;
                            }
                            switch (op)
                            {
                            case OP_EQUAL: tmp = num; break;
                            case OP_PLUS: tmp += num; break;
                            case OP_MINUS: tmp -= num; break;
                            case OP_MULT: tmp *= num; break;
                            case OP_DIV: tmp /= num; break;
                            }
                        }
                    }
                    if (i == 1 || i == 8) a = tmp;
                    if (i == 2 || i == 9) b = tmp;
                    if (i == 3 || i == 10) c = tmp;
                    if (i == 4 || i == 11) d = tmp;
                    if (i == 5 || i == 12) e = tmp;
                    if (i == 6 || i == 13) f = tmp;
                    if (i == 7) px = tmp;
                    if (i == 14) py = tmp;
                }
            }
            pntArray.setPoint(coordList.at(),px,py);
        }
    }
    return pntArray;
}

Q3PtrList<ATFInterpreter::AttribList> ATFInterpreter::getAttribList()
{
    if(!attrLs.isEmpty())
        return attrLs;

    AttribList *attribPtr;

    if (!coordList.isEmpty())
    {
        for (coordPtr=coordList.first();coordPtr != 0;coordPtr=coordList.next())
        {
            attribPtr = new AttribList;
            attribPtr->isVariable = coordPtr->isVariable;
            attribPtr->pwDiv = coordPtr->pwDiv;
            attrLs.append(attribPtr);
        }
    }
    attrLs.setAutoDelete(true);
    return attrLs;
}

void ATFInterpreter::interpret()
{
    Q3ValueStack<int> level;
    Value value;
    bool v = false;
    int pw = 1;
    CoordStruct coord;
    AttribStruct attrib;

    level.push(LEVEL_NULL);

    for (QStringList::Iterator it=lines.begin(); it!=lines.end(); ++it)
    {
        if (!(*it).isEmpty() && (*it).at(0) != COMMENT)
        {
            if (level.top() == LEVEL_NULL)
            {
                if ((*it) == PNT_BG)
                {
                    coordPtr = new Coord;
                    level.push(LEVEL_POINT);
                }
            }
            else if (level.top() == LEVEL_POINT)
            {
                if ((*it) == X_BG)
                    level.push(LEVEL_X);
                else if ((*it) == Y_BG)
                    level.push(LEVEL_Y);
                else if ((*it) == ATTR_BG)
                    level.push(LEVEL_ATTR);
                else if ((*it) == QString(END))
                {
                    level.pop();
                    coordList.append(coordPtr);
                }
            }
            else if (level.top() == LEVEL_X || level.top() == LEVEL_Y || level.top() == LEVEL_ATTR)
            {
                switch (((*it).at(0)).latin1())
                {
                case VAR_1:
                {
                    coord.a = (*it);
                    value.var1 = getVar(*it);
                } break;
                case VAR_2:
                {
                    coord.b = (*it);
                    value.var2 = getVar(*it);
                } break;
                case VAR_3:
                {
                    coord.c = (*it);
                    value.var3 = getVar(*it);
                } break;
                case VAR_4:
                {
                    coord.d = (*it);
                    value.var4 = getVar(*it);
                } break;
                case VAR_5:
                {
                    coord.e = (*it);
                    value.var5 = getVar(*it);
                } break;
                case VAR_6:
                {
                    coord.f = (*it);
                    value.var6 = getVar(*it);
                } break;
                case VAR_X: case VAR_Y:
                {
                    coord.result = (*it);
                    value.result = getVar(*it);
                } break;
                case VAR_VARIA:
                {
                    if ((*it).find('0') != -1) v = false;
                    else v = true;
                    attrib.isVariable = (*it);
                } break;
                case VAR_PW:
                {
                    pw = 1; pw = ((*it).at(4)).latin1() - 48;
                    attrib.pwDiv = (*it);
                } break;
                case '}':
                {
                    switch (level.top())
                    {
                    case LEVEL_X:
                    {
                        coordPtr->pntX = value;
                        coord.a = QString::null;
                        coord.b = QString::null;
                        coord.c = QString::null;
                        coord.d = QString::null;
                        coord.e = QString::null;
                        coord.f = QString::null;
                        coord.result = QString::null;
                    } break;
                    case LEVEL_Y:
                    {
                        coordPtr->pntY = value;
                        coord.a = QString::null;
                        coord.b = QString::null;
                        coord.c = QString::null;
                        coord.d = QString::null;
                        coord.e = QString::null;
                        coord.f = QString::null;
                        coord.result = QString::null;
                    } break;
                    case LEVEL_ATTR:
                    {
                        coordPtr->isVariable = v;
                        coordPtr->pwDiv = pw;
                        pw = 1;
                        v = false;
                        attrib.isVariable = QString::null;
                        attrib.pwDiv = 1;
                    } break;
                    }
                    level.pop();
                } break;
                }
            }
        }
    }
}

Q3PtrList<ATFInterpreter::Sign> ATFInterpreter::getVar(const QString &s)
{
    Q3PtrList<Sign> list;

    for (unsigned int i=1; i<s.length(); ++i)
    {
        if(s.at(i)==' ')
            continue;
        signPtr = new Sign;
        switch (s.at(i).latin1())
        {
        case VAR_W: signPtr->type = ST_WIDTH; break;
        case VAR_H: signPtr->type = ST_HEIGHT; break;
        case VAR_1:
        {
            signPtr->type = ST_VARIABLE;
            signPtr->var = VAR_1;
        } break;
        case VAR_2:
        {
            signPtr->type = ST_VARIABLE;
            signPtr->var = VAR_2;
        } break;
        case VAR_3:
        {
            signPtr->type = ST_VARIABLE;
            signPtr->var = VAR_3;
        } break;
        case VAR_4:
        {
            signPtr->type = ST_VARIABLE;
            signPtr->var = VAR_4;
        } break;
        case VAR_5:
        {
            signPtr->type = ST_VARIABLE;
            signPtr->var = VAR_5;
        } break;
        case VAR_6:
        {
            signPtr->type = ST_VARIABLE;
            signPtr->var = VAR_6;
        } break;
        case OP_EQUAL:
        {
            signPtr->type = ST_OPERATOR;
            signPtr->op = OP_EQUAL;
        } break;
        case OP_PLUS:
        {
            signPtr->type = ST_OPERATOR;
            signPtr->op = OP_PLUS;
        } break;
        case OP_MINUS:
        {
            signPtr->type = ST_OPERATOR;
            signPtr->op = OP_MINUS;
        } break;
        case OP_DIV:
        {
            signPtr->type = ST_OPERATOR;
            signPtr->op = OP_DIV;
        } break;
        case OP_MULT:
        {
            signPtr->type = ST_OPERATOR;
            signPtr->op = OP_MULT;
        } break;
        case NUM_0: case NUM_1: case NUM_2: case NUM_3: case NUM_4:
        case NUM_5: case NUM_6: case NUM_7: case NUM_8: case NUM_9:
        {
            signPtr->type = ST_NUMBER;
            if (s.length() - 1 > i)
            {
                switch (s.at(i+1).latin1())
                {
                case NUM_0: case NUM_1: case NUM_2: case NUM_3: case NUM_4:
                case NUM_5: case NUM_6: case NUM_7: case NUM_8: case NUM_9:
                {
                    signPtr->num = (s.at(i).latin1() - 48) * 10 + s.at(i+1).latin1() - 48;
                    i++;
                } break;
                default:
                    signPtr->num = s.at(i).latin1() - 48; break;
                }
            }
            else
                signPtr->num = s.at(i).latin1() - 48;
        } break;
        }
        list.append(signPtr);
    }
    return list;
}
