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

#include <atfinterpreter.h>

#include <qstack.h>
#include <qfile.h>

const char ATFInterpreter::PNT_BG[]   = "POINT{";
const char ATFInterpreter::X_BG[]     = "X{";
const char ATFInterpreter::Y_BG[]     = "Y{";
const char ATFInterpreter::ATTR_BG[]  = "ATTRIB{";
const char ATFInterpreter::END[]       = "}";

/******************************************************************/
/* class ATFInterpreter                                           */
/******************************************************************/

/*====================== constructor =============================*/
ATFInterpreter::ATFInterpreter( QObject* parent, const char* name )
    : QObject(parent,name)
{
    //coordList.setAutoDelete(true);
    //lines.setAutoDelete(true);
    //pointList.setAutoDelete(true);
}

/*======================= destrcutor =============================*/
ATFInterpreter::~ATFInterpreter()
{
    coordList.setAutoDelete(true);
    lines.setAutoDelete(true);
    pointList.setAutoDelete(true);

    pointList.clear();
    coordList.clear();
    lines.clear();
}

/*====================== load autoform ===========================*/
void ATFInterpreter::load(const QString & fileName)
{
    QString line;
    char* cLine = new char[256];
    QFile ptA(fileName);

    coordList.clear();
    lines.clear();
    pointList.clear();

    if (ptA.open(IO_ReadOnly))
    {
        while (!ptA.atEnd())
        {
            ptA.readLine(cLine,256);
            line = simplify(QString(qstrdup(cLine)));
            lines.append(qstrdup(line.latin1()));
        }
        ptA.close();
        delete cLine;
        interpret();
    }
}

/*======================= new autoform ===========================*/
void ATFInterpreter::newAutoform()
{
    coordList.clear();
    lines.clear();
    pointList.clear();
    interpret();
}

/*===================== save autoform ============================*/
void ATFInterpreter::save(const QString & fileName)
{
    if (!lines.isEmpty())
    {
        QFile f(fileName);
        QString line;
        unsigned int i = 0;

        if (f.open(IO_WriteOnly))
        {
            for (line = lines.first();i < lines.count();line = lines.next(),i++)
            {
                line += "\n";
                f.writeBlock(line.latin1(),line.length());
            }
        }
    }
}


/*==================== get point array ===========================*/
QPointArray ATFInterpreter::getPointArray(int wid,int heig)
{
    unsigned int px = 0,py = 0,a = 0,b = 0,c = 0,d = 0,e = 0,f = 0;
    unsigned int tmp = 0,num = 0;
    bool calc = false,res = false;
    char op = OP_EQUAL,var = VAR_1;
    QList<Sign> slp;
    QPointArray pntArray(QPointArray(coordList.count()));

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

/*===================== get attrib list =========================*/
QList<ATFInterpreter::AttribList> ATFInterpreter::getAttribList()
{
    QList<AttribList> attrLs;
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
    return attrLs;
}

/*========================== change variable =====================*/
void ATFInterpreter::changeVar(int pnt,int structur,int var,QString str)
{
    CoordStruct coord;

    str = simplify(str);
    if (!pointList.isEmpty())
    {
        pntPtr = pointList.at(pnt);
        if (structur < 2)
        {
            if (structur == 0) coord = pntPtr->x;
            else coord = pntPtr->y;
            if (var == 0) coord.a = qstrdup(str.latin1());
            else if (var == 1) coord.b = qstrdup(str.latin1());
            else if (var == 2) coord.c = qstrdup(str.latin1());
            else if (var == 3) coord.d = qstrdup(str.latin1());
            else if (var == 4) coord.e = qstrdup(str.latin1());
            else if (var == 5) coord.f = qstrdup(str.latin1());
            else if (var == 6) coord.result = qstrdup(str.latin1());
            if (structur == 0) pntPtr->x = coord;
            else pntPtr->y = coord;
        }
        else
        {
            if (var == 0) pntPtr->attrib.isVariable = qstrdup(str.latin1());
            else pntPtr->attrib.pwDiv = qstrdup(str.latin1());
        }
        makeLines();
        coordList.clear();
        pointList.clear();
        interpret();
    }
}

/*======================== get number of points ==================*/
int ATFInterpreter::getNumOfPoints()
{
    return pointList.count();
}

/*========================= insert Point =========================*/
void ATFInterpreter::insertPoint(int index,bool pos)
{
    // pos == true  => insert after index
    // pos == false => insert before index

    CoordStruct *coord1 = new CoordStruct;
    CoordStruct *coord2 = new CoordStruct;
    AttribStruct *attrib = new AttribStruct;

    pntPtr = new PointStruct;

    coord1->a = qstrdup("a=0");
    coord1->b = qstrdup("b=0");
    coord1->c = qstrdup("c=0");
    coord1->d = qstrdup("d=0");
    coord1->e = qstrdup("e=0");
    coord1->f = qstrdup("f=0");
    coord1->result = qstrdup("result=a");
    pntPtr->x = *coord1;
    coord2->a = qstrdup("a=0");
    coord2->b = qstrdup("b=0");
    coord2->c = qstrdup("c=0");
    coord2->d = qstrdup("d=0");
    coord2->e = qstrdup("e=0");
    coord2->f = qstrdup("f=0");
    coord2->result = qstrdup("result=a");
    pntPtr->y = *coord2;
    attrib->isVariable = qstrdup("v=0");
    attrib->pwDiv = qstrdup("p=1");
    pntPtr->attrib = *attrib;

    if (index > 0 && (unsigned int)index <= pointList.count())
    {
        if (pos) pointList.insert(index,pntPtr);
        else pointList.insert(index-1,pntPtr);
    }
    else if (pointList.isEmpty())
    {
        pointList.append(pntPtr);
    }
    makeLines();
    coordList.clear();
    pointList.clear();
    interpret();
}

/*=========================== delete point =======================*/
void ATFInterpreter::deletePoint(int pnt)
{
    if (!pointList.isEmpty() && pnt < static_cast<int>(pointList.count()))
    {
        pointList.remove(pnt);
        if (!pointList.isEmpty())
        {
            makeLines();
            coordList.clear();
            pointList.clear();
            interpret();
        }
        else
        {
            lines.clear();
            coordList.clear();
            pointList.clear();
            interpret();
        }
    }
}

/*========================= interpret ============================*/
void ATFInterpreter::interpret()
{
    QStack<ls> level;
    QString line;
    Value value;
    bool v = false;
    int pw = 1;
    ls *lPtr;
    CoordStruct coord;
    AttribStruct attrib;

    level.setAutoDelete(true);
    lPtr = new ls;
    lPtr->l = LEVEL_NULL;
    level.push(lPtr);

    unsigned int i = 0;
    for (line = simplify(lines.first());i < lines.count();line = simplify(lines.next()),i++)
    {
        if (!line.isEmpty() && line.at(0) != COMMENT)
        {
            if (level.top()->l == LEVEL_NULL)
            {
                if (line == PNT_BG)
                {
                    coordPtr = new Coord;
                    lPtr = new ls;
                    lPtr->l = LEVEL_POINT;
                    level.push(lPtr);
                    pntPtr = new PointStruct;
                }
            }
            else if (level.top()->l == LEVEL_POINT)
            {
                lPtr = new ls;
                if (line == X_BG)
                {
                    lPtr->l = LEVEL_X;
                    level.push(lPtr);
                }
                else if (line == Y_BG)
                {
                    lPtr->l = LEVEL_Y;
                    level.push(lPtr);
                }
                else if (line == ATTR_BG)
                {
                    lPtr->l = LEVEL_ATTR;
                    level.push(lPtr);
                }
                else if (line == QString(END))
                {
                    level.pop();
                    coordList.append(coordPtr);
                    pointList.append(pntPtr);
                }
            }
            else if (level.top()->l == LEVEL_X || level.top()->l == LEVEL_Y || level.top()->l == LEVEL_ATTR)
            {
                switch (QChar(line[0]))
                {
                case VAR_1:
                {
                    coord.a = qstrdup(line.latin1());
                    value.var1 = getVar(line.remove(0,1));
                } break;
                case VAR_2:
                {
                    coord.b = qstrdup(line.latin1());
                    value.var2 = getVar(line.remove(0,1));
                } break;
                case VAR_3:
                {
                    coord.c = qstrdup(line.latin1());
                    value.var3 = getVar(line.remove(0,1));
                } break;
                case VAR_4:
                {
                    coord.d = qstrdup(line.latin1());
                    value.var4 = getVar(line.remove(0,1));
                } break;
                case VAR_5:
                {
                    coord.e = qstrdup(line.latin1());
                    value.var5 = getVar(line.remove(0,1));
                } break;
                case VAR_6:
                {
                    coord.f = qstrdup(line.latin1());
                    value.var6 = getVar(line.remove(0,1));
                } break;
                case VAR_X: case VAR_Y:
                {
                    coord.result = qstrdup(line.latin1());
                    value.result = getVar(line.remove(0,1));
                } break;
                case VAR_VARIA:
                {
                    if (line.at(2) == '0') v = false;
                    else v = true;
                    attrib.isVariable = qstrdup(line.latin1());
                } break;
                case VAR_PW:
                {
                    pw = 1; pw = ((char)QChar(line.at(2))) - 48;
                    attrib.pwDiv = qstrdup(line.latin1());
                } break;
                case '}':
                {
                    switch (level.top()->l)
                    {
                    case LEVEL_X:
                    {
                        coordPtr->pntX = value;
                        pntPtr->x = coord;
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
                        pntPtr->y = coord;
                        coord.a = QString::null;
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
                        pntPtr->attrib = attrib;
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
    makeLines();
}

/*====================== simplyfy a string =======================*/
QString ATFInterpreter::simplify(QString s)
{
    QString res;
    QString str = s.stripWhiteSpace();

    for (unsigned int i=0;i < str.length();i++)
        if (str.at(i) != ' ') res.insert(res.length(),str.at(i));
    return res;
}

/*====================== get variable ===========================*/
QList<ATFInterpreter::Sign> ATFInterpreter::getVar(QString _s)
{
    QCString s(_s.ascii());

    QList<Sign> list;
    for (unsigned int i=0;i < s.length();i++)
    {
        signPtr = new Sign;
        switch (s.at(i))
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
                switch (s.at(i+1))
                {
                case NUM_0: case NUM_1: case NUM_2: case NUM_3: case NUM_4:
                case NUM_5: case NUM_6: case NUM_7: case NUM_8: case NUM_9:
                {
                    signPtr->num = (s.at(i) - 48) * 10 + s.at(i+1) - 48;
                    i++;
                } break;
                default:
                    signPtr->num = s.at(i) - 48; break;
                }
            }
            else
                signPtr->num = s.at(i) - 48;
        } break;
        }
        list.append(signPtr);
    }
    return list;
}

/*===================== make lines ==============================*/
void ATFInterpreter::makeLines()
{
    QString tmp,tmp2;
    unsigned int i = 1;

    if (!pointList.isEmpty())
    {
        lines.clear();
        lines.append("####################################");
        tmp = "# ";
        tmp += name();
        tmp += " Autoform";
        lines.append(qstrdup(tmp.latin1()));
        lines.append("# Generated with KAutoformEdit");
        lines.append("# (c) by Reginald Stadlbauer 1998");
        lines.append("# E-Mail: reggie@kde.org");
        lines.append("####################################");
        lines.append("");
        for (pntPtr = pointList.first();pntPtr != 0;pntPtr = pointList.next(),i++)
        {
            tmp = "################ ";
            tmp2 = "";
            tmp2.setNum(i);
            tmp += tmp2;
            tmp +=". Point";
            lines.append(qstrdup(tmp.latin1()));
            lines.append("");
            lines.append("POINT {");
            lines.append("");
            lines.append("  X {");

            if (!pntPtr->x.a.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->x.a).latin1()));
            else
                lines.append("    a = 0");
            if (!pntPtr->x.b.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->x.b).latin1()));
            else
                lines.append("    b = 0");
            if (!pntPtr->x.c.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->x.c).latin1()));
            else
                lines.append("    c = 0");
            if (!pntPtr->x.d.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->x.d).latin1()));
            else
                lines.append("    d = 0");
            if (!pntPtr->x.e.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->x.e).latin1()));
            else
                lines.append("    e = 0");
            if (!pntPtr->x.f.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->x.f).latin1()));
            else
                lines.append("    f = 0");
            if (!pntPtr->x.result.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->x.result).latin1()));
            else
                lines.append("    x = a");
            lines.append("  }");
            lines.append("");
            lines.append("  Y {");
            if (!pntPtr->y.a.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->y.a).latin1()));
            else
                lines.append("    a = 0");
            if (!pntPtr->y.b.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->y.b).latin1()));
            else
                lines.append("    b = 0");
            if (!pntPtr->y.c.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->y.c).latin1()));
            else
                lines.append("    c = 0");
            if (!pntPtr->y.d.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->y.d).latin1()));
            else
                lines.append("    d = 0");
            if (!pntPtr->y.e.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->y.e).latin1()));
            else
                lines.append("    e = 0");
            if (!pntPtr->y.f.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->y.f).latin1()));
            else
                lines.append("    f = 0");
            if (!pntPtr->y.result.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->y.result).latin1()));
            else
                lines.append("    y = a");
            lines.append("  }");
            lines.append("");
            lines.append("  ATTRIB {");
            if (!pntPtr->attrib.isVariable.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->attrib.isVariable).latin1()));
            else
                lines.append("    v = 0");
            if (!pntPtr->attrib.pwDiv.isEmpty())
                lines.append(qstrdup(stretch(pntPtr->attrib.pwDiv).latin1()));
            else
                lines.append("    p = 1");
            lines.append("  }");
            lines.append("");
            lines.append("}");
            lines.append("");
        }
    }
}

/*================== stretch a line =============================*/
QString ATFInterpreter::stretch(QString s)
{
    QString res = "";
    unsigned int i;

    for (i=0;i < s.length()-1;i++)
    {
        res += s.at(i);
        if ((isNum((char)QChar(s.at(i))) && !isNum((char)QChar(s.at(i+1)))) || (!isNum((char)QChar(s.at(i)))))
            res += " ";
    }
    res += s.at(s.length()-1);
    return res;
}

/*====================== is a number ============================*/
bool ATFInterpreter::isNum(char c)
{
    if (c >= 48 && c <= 57) return true;
    else return false;
}

#include <atfinterpreter.moc>
