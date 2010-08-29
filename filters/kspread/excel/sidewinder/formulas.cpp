/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006,2009 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
   Copyright (C) 2009,2010 Sebastian Sauer <sebsauer@kdab.com>

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
   Boston, MA 02110-1301, USA
 */
#include "formulas.h"

#include <iostream>
#include <iomanip>

#include "excel.h"
#include "utils.h"

namespace Swinder
{

//=============================================
//          FormulaToken
//=============================================

class FormulaToken::Private
{
public:
    unsigned ver;
    unsigned id;
    std::vector<unsigned char> data;
};

FormulaToken::FormulaToken()
{
    d = new Private;
    d->ver = Excel97;
    d->id = Unused;
}

FormulaToken::FormulaToken(unsigned t)
{
    d = new Private;
    d->ver = Excel97;
    d->id = t;
}

FormulaToken::FormulaToken(const FormulaToken& token)
{
    d = new Private;
    d->ver = token.d->ver;
    d->id = token.id();

    d->data.resize(token.d->data.size());
    for (unsigned i = 0; i < d->data.size(); i++)
        d->data[i] = token.d->data[i];
}

void FormulaToken::operator=(const FormulaToken& token)
{
    d = new Private;
    d->ver = token.d->ver;
    d->id = token.id();

    d->data.resize(token.d->data.size());
    for (unsigned i = 0; i < d->data.size(); i++)
        d->data[i] = token.d->data[i];
}

FormulaToken::~FormulaToken()
{
    delete d;
}

unsigned FormulaToken::version() const
{
    return d->ver;
}

void FormulaToken::setVersion(unsigned v)
{
    d->ver = v;
}

unsigned FormulaToken::id() const
{
    return d->id;
}

const char* FormulaToken::idAsString() const
{
    const char* s = 0;

    switch (d->id) {
    case Matrix:       s = "Matrix"; break;
    case Table:        s = "Table"; break;
    case Add:          s = "Add"; break;
    case Sub:          s = "Sub"; break;
    case Mul:          s = "Mul"; break;
    case Div:          s = "Div"; break;
    case Power:        s = "Power"; break;
    case Concat:       s = "Concat"; break;
    case LT:           s = "LT"; break;
    case LE:           s = "LE"; break;
    case EQ:           s = "EQ"; break;
    case GE:           s = "GE"; break;
    case GT:           s = "GT"; break;
    case NE:           s = "NE"; break;
    case Intersect:    s = "Intersect"; break;
    case Union:        s = "Union"; break;
    case Range:        s = "Range"; break;
    case UPlus:        s = "UPlus"; break;
    case UMinus:       s = "UMinus"; break;
    case Percent:      s = "Percent"; break;
    case Paren:        s = "Paren"; break;
    case String:       s = "String"; break;
    case MissArg:      s = "MissArg"; break;
    case ErrorCode:    s = "ErrorCode"; break;
    case Bool:         s = "Bool"; break;
    case Integer:      s = "Integer"; break;
    case Array:        s = "Array"; break;
    case Function:     s = "Function"; break;
    case FunctionVar:  s = "FunctionVar"; break;
    case Name:         s = "Name"; break;
    case Ref:          s = "Ref"; break;
    case RefErr:       s = "RefErr"; break;
    case RefN:         s = "RefN"; break;
    case Area:         s = "Area"; break;
    case AreaErr:      s = "AreaErr"; break;
    case AreaN:        s = "AreaN"; break;
    case NameX:        s = "NameX"; break;
    case Ref3d:        s = "Ref3d"; break;
    case RefErr3d:     s = "RefErr3d"; break;
    case Float:        s = "Float"; break;
    case Area3d:       s = "Area3d"; break;
    case AreaErr3d:    s = "AreaErr3d"; break;
    case NatFormula:   s = "NatFormula"; break;
    case Sheet:        s = "Sheet"; break;
    case EndSheet:     s = "EndSheet"; break;
    case MemArea:      s = "MemArea"; break;
    case MemErr:       s = "MemErr"; break;
    case MemNoMem:     s = "MemNoMem"; break;
    case MemFunc:      s = "MemFunc"; break;
    case MemAreaN:     s = "MemAreaN"; break;
    case MemNoMemN:    s = "MemNoMemN"; break;
    case Attr: switch (attr()) {
        case AttrChoose:    s = "AttrChoose"; break;
        default:            s = "Attr"; break;
    } break;
    case 0:            s = ""; break; // NOPE...
    default:
        s = "Unknown";
        printf("Unhandled formula id %i as string\n", d->id);
        break;
    }

    return s;
}


unsigned FormulaToken::size() const
{
    unsigned s = 0; // on most cases no data

    switch (d->id) {
    case Add:
    case Sub:
    case Mul:
    case Div:
    case Power:
    case Concat:
    case LT:
    case LE:
    case EQ:
    case GE:
    case GT:
    case NE:
    case Intersect:
    case Union:
    case Range:
    case UPlus:
    case UMinus:
    case Percent:
    case Paren:
    case MissArg:
        s = 0; break;

    case Attr:
        switch (attr()) {
            case AttrChoose:    s = 3 + 2 * (1+readU16(&(d->data[1]))); break;
            default:            s = 3; break;
        } break;

    case ErrorCode:
    case Bool:
        s = 1; break;

    case Integer:
        s = 2; break;

    case Array:
        s = 7; break;

    case Function:
        s = 2; break;

    case FunctionVar:
        s = 3; break;

    case Matrix:
    case Table:
        s = (d->ver == Excel97) ? 4 : 3; break;

    case Name:
        s = (d->ver == Excel97) ? 4 : 14; break;

    case Ref:
    case RefErr:
    case RefN:
        s = (d->ver == Excel97) ? 4 : 3; break;

    case Area:
    case AreaErr:
    case AreaN:
        s = (d->ver == Excel97) ? 8 : 6; break;

    case NameX:
        s = (d->ver == Excel97) ? 6 : 24; break;

    case Ref3d:
    case RefErr3d:
        s = (d->ver == Excel97) ? 6 : 17; break;

    case Float:
        s = 8; break;

    case Area3d:
    case AreaErr3d:
        s = (d->ver == Excel97) ? 10 : 20; break;

    case MemArea:
        s = 6; break;

    case MemErr:
        s = 6; break;

    case 0: // NOPE
        s = 0; break;

    case MemFunc:
        s = 2; break;

    case NatFormula:
    case Sheet:
    case EndSheet:
    case MemNoMem:
    case MemAreaN:
    case MemNoMemN:
    default:
        // WARNING this is unhandled case
        printf("Unhandled formula token with id %i\n", d->id);
        break;
    }

    return s;
}

void FormulaToken::setData(unsigned size, const unsigned char* data)
{
    d->data.resize(size);
    for (unsigned i = 0; i < size; i++)
        d->data[i] = data[i];
}

Value FormulaToken::value() const
{
    Value result;

    unsigned char* buf;
    buf = new unsigned char[d->data.size()];
    for (unsigned k = 0; k < d->data.size(); k++)
        buf[k] = d->data[k];

    // FIXME sanity check: verify size of data
    switch (d->id) {
    case ErrorCode:
        result = errorAsValue(buf[0]);
        break;

    case Bool:
        result = Value(buf[0] != 0);
        break;

    case Integer:
        result = Value((int)readU16(buf));
        break;

    case Float:
        result = Value(readFloat64(buf));
        break;

    case String: {
        EString estr = (version() == Excel97) ?
                       EString::fromUnicodeString(buf, false, d->data.size()) :
                       EString::fromByteString(buf, false, d->data.size());
        result = Value(estr.str());
        break;
    }

    default: break;
    }

    delete [] buf;

    return result;
}

unsigned FormulaToken::functionIndex() const
{
    // FIXME check data size
    unsigned index = 0;
    unsigned char buf[2];

    if (d->id == Function) {
        buf[0] = d->data[0];
        buf[1] = d->data[1];
        index = readU16(buf);
    }

    if (d->id == FunctionVar) {
        buf[0] = d->data[1];
        buf[1] = d->data[2];
        index = readU16(buf);
    }

    return index;
}

struct FunctionEntry {
    const char *name;
    int params;
    bool varParams;
};

static const FunctionEntry FunctionEntries[] = {
    { "COUNT",           1, true },     // 0
    { "IF",              0, true },     // 1
    { "ISNA",            1, false },    // 2
    { "ISERROR",         1, false },    // 3
    { "SUM",             0, true },     // 4
    { "AVERAGE",         0, true },     // 5
    { "MIN",             0, true },     // 6
    { "MAX",             0, true },     // 7
    { "ROW",             0, true },     // 8
    { "COLUMN",          0, true },     // 9
    { "NA",              0, false },    // 10
    { "NPV",             0, true },     // 11
    { "STDEV",           0, true },     // 12
    { "DOLLAR",          0, true },     // 13
    { "FIXED",           0, true },     // 14
    { "SIN",             1, false },    // 15
    { "COS",             1, false },    // 16
    { "TAN",             1, false },    // 17
    { "ATAN",            1, false },    // 18
    { "PI",              0, false },    // 19
    { "SQRT",            1, false },    // 20
    { "EXP",             1, false },    // 21
    { "LN",              1, false },    // 22
    { "LOG10",           1, false },    // 23
    { "ABS",             1, false },    // 24
    { "INT",             1, false },    // 25
    { "SIGN",            1, false },    // 26
    { "ROUND",           2, false },    // 27
    { "LOOKUP",          0, true },     // 28
    { "INDEX",           0, true },     // 29
    { "REPT",            2, false },    // 30
    { "MID",             3, false },    // 31
    { "LEN",             1, false },    // 32
    { "VALUE",           1, false },    // 33
    { "TRUE",            0, false },    // 34
    { "FALSE",           0, false },    // 35
    { "AND",             0, true },     // 36
    { "OR",              0, true },     // 37
    { "NOT",             1, false },    // 38
    { "MOD",             2, false },    // 39
    { "DCOUNT",          3, false },    // 40
    { "DSUM",            3, false },    // 41
    { "DAVERAGE",        3, false },    // 42
    { "DMIN",            3, false },    // 43
    { "DMAX",            3, false },    // 44
    { "DSTDEV",          3, false },    // 45
    { "VAR",             0, true },     // 46
    { "DVAR",            3, false },    // 47
    { "TEXT",            2, false },    // 48
    { "LINEST",          0, true },     // 49
    { "TREND",           0, true },     // 50
    { "LOGEST",          0, true },     // 51
    { "GROWTH",          0, true },     // 52
    { "GOTO",            0, false },    // 53
    { "HALT",            0, true },     // 54
    { "RETURN",          0, true },     // 55
    { "PV",              0, true },     // 56
    { "FV",              0, true },     // 57
    { "NPER",            0, true },     // 58
    { "PMT",             0, true },     // 59
    { "RATE",            0, true },     // 60
    { "MIRR",            3, false },    // 61
    { "IRR",             0, true },     // 62
    { "RAND",            0, false },    // 63
    { "MATCH",           0, true },     // 64
    { "DATE",            3, false },    // 65
    { "TIME",            3, false },    // 66
    { "DAY",             1, false },    // 67
    { "MONTH",           1, false },    // 68
    { "YEAR",            1, false },    // 69
    { "WEEKDAY",         0, true },     // 70
    { "HOUR",            1, false },    // 71
    { "MINUTE",          1, false },    // 72
    { "SECOND",          1, false },    // 73
    { "NOW",             0, false },    // 74
    { "AREAS",           1, false },    // 75
    { "ROWS",            1, false },    // 76
    { "COLUMNS",         1, false },    // 77
    { "OFFSET",          0, true },     // 78
    { "ABSREF",          2, false },    // 79
    { "RELREF",          2, false },    // 80
    { "ARGUMENT",        0, true },     // 81
    { "SEARCH",          0, true },     // 82
    { "TRANSPOSE",       1, false },    // 83
    { "ERROR",           0, true },     // 84
    { "STEP",            0, false },    // 85
    { "TYPE",            1, false },    // 86
    { "ECHO",            0, true },
    { "SETNAME",         0, true },     // deprecated...
    { "CALLER",          0, false },
    { "DEREF",           1, false },
    { "WINDOWS",         0, true },
    { "SERIES",          4, true },
    { "DOCUMENTS",       0, true },
    { "ACTIVECELL",      0, false },    // deprecated...
    { "SELECTION",       0, false },
    { "RESULT",          0, true },
    { "ATAN2",           2, false },    // 97
    { "ASIN",            1, false },    // 98
    { "ACOS",            1, false },    // 99
    { "CHOOSE",          0, true },     // 100
    { "HLOOKUP",         0, true },     // 101
    { "VLOOKUP",         0, true },     // 102
    { "LINKS",           0, true },
    { "INPUT",           0, true },
    { "ISREF",           1, false },    // 105
    { "GETFORMULA",      1, false },    // deprecated...
    { "GETNAME",         0, true },     // deprecated...
    { "SETVALUE",        2, false },    // deprecated...
    { "LOG",             0, true },     // 109
    { "EXEC",            0, true },
    { "CHAR",            1, false },    // 111
    { "LOWER",           1, false },    // 112
    { "UPPER",           1, false },    // 113
    { "PROPER",          1, false },    // 114
    { "LEFT",            0, true },     // 115
    { "RIGHT",           0, true },     // 116
    { "EXACT",           2, false },    // 117
    { "TRIM",            1, false },    // 118
    { "REPLACE",         4, false },    // 119
    { "SUBSTITUTE",      0, true },     // 120
    { "CODE",            1, false },    // 121
    { "NAMES",           0, true },
    { "DIRECTORY",       0, true },
    { "FIND",            0, true },     // 124
    { "CELL",            0, true },     // 125
    { "ISERR",           1, false },    // 126
    { "ISTEXT",          1, false },    // 127
    { "ISNUMBER",        1, false },    // 128
    { "ISBLANK",         1, false },    // 129
    { "T",               1, false },    // 130
    { "N",               1, false },    // 131
    { "FOPEN",           0, true },     // not portable, insecure, deprecated
    { "FCLOSE",          1, false },    // not portable, insecure, deprecated
    { "FSIZE",           1, false },    // not portable, insecure, deprecated
    { "FREADLN",         1, false },    // not portable, insecure, deprecated
    { "FREAD",           2, false },    // not portable, insecure, deprecated
    { "FWRITELN",        2, false },    // not portable, insecure, deprecated
    { "FWRITE",          2, false },    // not portable, insecure, deprecated
    { "FPOS",            0, true },     // not portable, insecure, deprecated
    { "DATEVALUE",       1, false },    // 140
    { "TIMEVALUE",       1, false },    // 141
    { "SLN",             3, false },    // 142
    { "SYD",             4, false },    // 143
    { "DDB",             0, true },     // 144
    { "GETDEF",          0, true },
    { "REFTEXT",         0, true },
    { "TEXTREF",         0, true },
    { "INDIRECT",        0, true },     // 148
    { "REGISTER",        0, true },
    { "CALL",            0, true },
    { "ADDBAR",          0, true },     // deprecated
    { "ADDMENU",         0, true },     // deprecated
    { "ADDCOMMAND",      0, true },     // deprecated
    { "ENABLECOMMAND",   0, true },     // deprecated
    { "CHECKCOMMAND",    0, true },     // deprecated
    { "RENAMECOMMAND",   0, true },     // deprecated
    { "SHOWBAR",         0, true },     // deprecated
    { "DELETEMENU",      0, true },     // deprecated
    { "DELETECOMMAND",   0, true },     // deprecated
    { "GETCHARTITEM",    0, true },     // deprecated
    { "DIALOGBOX",       0, true },     // deprecated
    { "CLEAN",           1, false },    // 162
    { "MDETERM",         1, false },    // 163
    { "MINVERSE",        1, false },    // 164
    { "MMULT",           2, false },    // 165
    { "FILES",           0, true },     // not portable, insecure, deprecated
    { "IPMT",            0, true },     // 167
    { "PPMT",            0, true },     // 168
    { "COUNTA",          0, true },     // 169
    { "CANCELKEY",       1, true },
    { "FOR",             0, true },
    { "WHILE",           1, false },
    { "BREAK",           0, false },
    { "NEXT",            0, false },
    { "INITIATE",        2, false },
    { "REQUEST",         2, false },
    { "POKE",            3, false },
    { "EXECUTE",         2, false },
    { "TERMINATE",       1, false },
    { "RESTART",         0, true },
    { "HELP",            0, true },
    { "GETBAR",          0, true },
    { "PRODUCT",         0, true },     // 183
    { "FACT",            1, false },    // 184
    { "GETCELL",         0, true },
    { "GETWORKSPACE",    1, false },
    { "GETWINDOW",       0, true },
    { "GETDOCUMENT",     0, true },
    { "DPRODUCT",        3, false },    // 189
    { "ISNONTEXT",       1, false },    // 190
    { "GETNOTE",         0, true },
    { "NOTE",            0, true },
    { "STDEVP",          0, true },     // 193
    { "VARP",            0, true },     // 194
    { "DSTDEVP",         3, false },    // 195
    { "DVARP",           3, false },    // 196
    { "TRUNC",           0, true },     // 197
    { "ISLOGICAL",       1, false },    // 198
    { "DCOUNTA",         3, false },    // 199
    { "DELETEBAR",       1, false },
    { "UNREGISTER",      1, false },
    { "Unknown202",      0 },
    { "Unknown203",      0 },
    { "USDOLLAR",        0, true },
    { "FINDB",           0, true },
    { "SEARCHB",         0, true },
    { "REPLACEB",        4, false },
    { "LEFTB",           0, true },
    { "RIGHTB",          0, true },
    { "MIDB",            3, false },
    { "LENB",            1, false },
    { "ROUNDUP",         2, false },    // 212
    { "ROUNDDOWN",       2, false },    // 213
    { "ASC",             1, false },
    { "DBCS",            1, false },
    { "RANK",            0, true },     // 216
    { "Unknown217",      0 },
    { "Unknown218",      0 },
    { "ADDRESS",         0, true },     // 219
    { "DAYS360",         0, true },     // 220
    { "CURRENTDATE",     0, false },    // 221
    { "VBD",             0, true },     // 222
    { "ELSE",            0, false },
    { "ELSE.IF",         1, false },
    { "END.IF",          0, false },
    { "FOR.CELL",        0, true },
    { "MEDIAN",          0, true },     // 227
    { "SUMPRODUCT",      0, true },     // 228
    { "SINH",            1, false },    // 229
    { "COSH",            1, false },    // 230
    { "TANH",            1, false },    // 231
    { "ASINH",           1, false },    // 232
    { "ACOSH",           1, false },    // 233
    { "ATANH",           1, false },    // 234
    { "DGET",            3, false },    // 235
    { "CREATEOBJECT",    0, true },
    { "VOLATILE",        0, true },
    { "LASTERROR",       0, false },
    { "CUSTOMUNDO",      0, true },
    { "CUSTOMREPEAT",    0, true },
    { "FORMULACONVERT",  0, true },
    { "GETLINKINFO",     0, true },
    { "TEXTBOX",         0, true },
    { "INFO",            1, false },    // 244
    { "GROUP",           0, false },
    { "GETOBJECT",       0, true },
    { "DB",              0, true },     // 247
    { "PAUSE",           0, true },
    { "Unknown249",      0 },
    { "Unknown250",      0 },
    { "RESUME",          0, true },
    { "FREQUENCY",       2, false },    // 252
    { "ADDTOOLBAR",      0, true },
    { "DELETETOOLBAR",   0, true },
    { "USER.DEFINED.FUNCTION", 0, true },
    { "RESETTOOLBAR",    1, false },
    { "EVALUATE",        1, false },
    { "GETTOOLBAR",      0, true },
    { "GETTOOL",         0, true },
    { "SPELLINGCHECK",   0, true },
    { "ERRORTYPE",       1, false },    // 261
    { "APPTITLE",        0, true },
    { "WINDOWTITLE",     0, true },
    { "SAVETOOLBAR",     0, true },
    { "ENABLETOOL",      3, false },
    { "PRESSTOOL",       3, false },
    { "REGISTERID",      0, true },
    { "GETWORKBOOK",     0, true },
    { "AVEDEV",          0, true },     // 269
    { "BETADIST",        0, true },     // 270
    { "GAMMALN",         1, false },    // 271
    { "BETAINV",         0, true },     // 272
    { "BINOMDIST",       4, false },    // 273
    { "CHIDIST",         2, false },    // 274
    { "CHIINV",          2, false },    // 275
    { "COMBIN",          2, false },    // 276
    { "CONFIDENCE",      3, false },    // 277
    { "CRITBINOM",       3, false },    // 278
    { "EVEN",            1, false },    // 279
    { "EXPONDIST",       3, false },    // 280
    { "FDIST",           3, false },    // 281
    { "FINV",            3, false },    // 282
    { "FISHER",          1, false },    // 283
    { "FISHERINV",       1, false },    // 284
    { "FLOOR",           2, false },    // 285
    { "GAMMADIST",       4, false },    // 286
    { "GAMMAINV",        3, false },    // 287
    { "CEIL",            2, false },    // 288
    { "HYPGEOMDIST",     4, false },    // 289
    { "LOGNORMDIST",     3, false },    // 290
    { "LOGINV",          3, false },    // 291
    { "NEGBINOMDIST",    3, false },    // 292
    { "NORMDIST",        4, false },    // 293
    { "NORMSDIST",       1, false },    // 294
    { "NORMINV",         3, false },    // 295
    { "NORMSINV",        1, false },    // 296
    { "STANDARDIZE",     3, false },    // 297
    { "ODD",             1, false },    // 298
    { "PERMUT",          2, false },    // 299
    { "POISSON",         3, false },    // 300
    { "TDIST",           3, false },    // 301
    { "WEIBULL",         4, false },    // 302
    { "SUMXMY2",         2, false },    // 303
    { "SUMX2MY2",        2, false },    // 304
    { "SUMX2DY2",        2, false },    // 305
    { "CHITEST",         2, false },    // 306
    { "CORREL",          2, false },    // 307
    { "COVAR",           2, false },    // 308
    { "FORECAST",        3, false },    // 309
    { "FTEST",           2, false },    // 310
    { "INTERCEPT",       2, false },    // 311
    { "PEARSON",         2, false },    // 312
    { "RSQ",             2, false },    // 313
    { "STEYX",           2, false },    // 314
    { "SLOPE",           2, false },    // 315
    { "TTEST",           4, false },    // 316
    { "PROB",            0, true },     // 317
    { "DEVSQ",           0, true },     // 318
    { "GEOMEAN",         0, true },     // 319
    { "HARMEAN",         0, true },     // 320
    { "SUMSQ",           0, true },     // 321
    { "KURT",            0, true },     // 322
    { "SKEW",            0, true },     // 323
    { "ZTEST",           0, true },     // 324
    { "LARGE",           2, false },    // 325
    { "SMALL",           2, false },    // 326
    { "QUARTILE",        2, false },    // 327
    { "PERCENTILE",      2, false },    // 328
    { "PERCENTRANK",     0, true },     // 329
    { "MODALVALUE",      0, true },     // 330
    { "TRIMMEAN",        2, false },    // 331
    { "TINV",            2, false },    // 332
    { "Unknown333",      0 },
    { "MOVIECOMMAND",    0, true },
    { "GETMOVIE",        0, true },
    { "CONCATENATE",     0, true },     // 336
    { "POWER",           2, false },    // 337
    { "PIVOTADDDATA",    0, true },
    { "GETPIVOTTABLE",   0, true },
    { "GETPIVOTFIELD",   0, true },
    { "GETPIVOTITEM",    0, true },
    { "RADIANS",         1, false },    // 342
    { "DEGREES",         1, false },    // 343
    { "SUBTOTAL",        0, true },     // 344
    { "SUMIF",           0, true },     // 345
    { "COUNTIF",         2, false },    // 346
    { "COUNTBLANK",      1, false },    // 347
    { "SCENARIOGET",     0, true },
    { "OPTIONSLISTSGET", 1, false },
    { "ISPMT",           4, false },
    { "DATEDIF",         3, false },
    { "DATESTRING",      1, false },
    { "NUMBERSTRING",    2, false },
    { "ROMAN",           0, true },     // 354
    { "OPENDIALOG",      0, true },
    { "SAVEDIALOG",      0, true },
    { "VIEWGET",         0, true },
    { "GETPIVOTDATA",    2, true },     // 358
    { "HYPERLINK",       1, true },
    { "PHONETIC",        1, false },
    { "AVERAGEA",        0, true },     // 361
    { "MAXA",            0, true },     // 362
    { "MINA",            0, true },     // 363
    { "STDEVPA",         0, true },     // 364
    { "VARPA",           0, true },     // 365
    { "STDEVA",          0, true },     // 366
    { "VARA",            0, true },     // 367
    { "BAHTTEXT",        1, false },    // 368
    //TODO; following formulas are not supported in KSpread yet
    { "THAIDAYOFWEEK",   1, false },    // 369
    { "THAIDIGIT",       1, false },    // 370
    { "THAIMONTHOFYEAR", 1, false },    // 371
    { "THAINUMSOUND",    1, false },    // 372
    { "THAINUMSTRING",   1, false },    // 373
    { "THAISTRINGLENGTH", 1, false },   // 374
    { "ISTHAIDIGIT",     1, false },    // 375
    { "ROUNDBAHTDOWN",   1, false },    // 376
    { "ROUNDBAHTUP",     1, false },    // 377
    { "THAIYEAR",        1, false },    // 378
    { "RTD",             1, false },    // 379
    { "ISHYPERLINK",     1, false }     // 380
};

static const FunctionEntry* functionEntry(const QString& functionName)
{
    static QHash<QString, const FunctionEntry*> entries;
    if (entries.isEmpty()) {
        for (int i = 0; i <= 380; i++) {
            entries[QString::fromAscii(FunctionEntries[i].name)] = &FunctionEntries[i];
        }
    }
    return entries.value(functionName);
}

const char* FormulaToken::functionName() const
{
    if (functionIndex() > 367) return 0;
    return FunctionEntries[ functionIndex()].name;
}

unsigned FormulaToken::functionParams() const
{
    unsigned params = 0;

    if (d->id == Function) {
        if (functionIndex() > 367) return 0;
        params = FunctionEntries[ functionIndex()].params;
    }

    if (d->id == FunctionVar) {
        params = (unsigned)d->data[0];
        params &= 0x7f;
    }

    return params;
}

unsigned FormulaToken::functionIndex(const QString &functionName)
{
    const FunctionEntry* e = functionEntry(functionName);
    if (e) return e - FunctionEntries;
    return -1;
}

unsigned FormulaToken::functionParams(const QString &functionName)
{
    const FunctionEntry* e = functionEntry(functionName);
    if (e) return e->params;
    return 0;
}

bool FormulaToken::fixedFunctionParams(const QString &functionName)
{
    const FunctionEntry* e = functionEntry(functionName);
    if (e) return !e->varParams;
    return false;
}

unsigned FormulaToken::attr() const
{
    unsigned attr = 0;
    if (d->id == Attr && d->data.size() > 0) {
        attr = (unsigned) d->data[0];
    }
    return attr;
}

unsigned long FormulaToken::nameIndex() const
{
    // FIXME check data size !
    unsigned long ni = 0;
    unsigned char buf[4];
    if (d->id == Name) {
        if (d->ver == Excel97) {
            buf[0] = d->data[0];
            buf[1] = d->data[1];
            buf[2] = d->data[2];
            buf[3] = d->data[3];
            ni = readU32(buf);
        }
        else if (d->ver == Excel95) {
            buf[0] = d->data[8];
            buf[1] = d->data[9];
            ni = readU16(buf);
        }
    }
    return ni;
}

unsigned long FormulaToken::nameXIndex() const
{
    // FIXME check data size !
    unsigned long ni = 0;
    unsigned char buf[4];
    if (d->id == NameX) {
        if (d->ver == Excel97) {
            buf[0] = d->data[2];
            buf[1] = d->data[3];
            buf[2] = d->data[4];
            buf[3] = d->data[5];
            ni = readU32(buf);
        }
        else if (d->ver == Excel95) {
            buf[0] = d->data[10];
            buf[1] = d->data[11];
            ni = readU16(buf);
        }
    }
    return ni;
}

QString FormulaToken::area(unsigned row, unsigned col, bool relative) const
{
    // FIXME check data size !
    unsigned char buf[2];
    int row1Ref, row2Ref, col1Ref, col2Ref;
    bool row1Relative, col1Relative;
    bool row2Relative, col2Relative;

    if (version() == Excel97) {
        buf[0] = d->data[0];
        buf[1] = d->data[1];
        row1Ref = readU16(buf);

        buf[0] = d->data[2];
        buf[1] = d->data[3];
        row2Ref = readU16(buf);

        buf[0] = d->data[4];
        buf[1] = d->data[5];
        col1Ref = readU16(buf);

        buf[0] = d->data[6];
        buf[1] = d->data[7];
        col2Ref = readU16(buf);

        row1Relative = col1Ref & 0x8000;
        col1Relative = col1Ref & 0x4000;
        col1Ref &= 0x3fff;

        row2Relative = col2Ref & 0x8000;
        col2Relative = col2Ref & 0x4000;
        col2Ref &= 0x3fff;

        if (relative) {
            if (row1Ref & 0x8000) row1Ref -= 0x10000;
            if (row2Ref & 0x8000) row2Ref -= 0x10000;
            if (col1Ref & 0x80) col1Ref -= 0x100;
            if (col2Ref & 0x80) col2Ref -= 0x100;
        }
    } else {
        buf[0] = d->data[0];
        buf[1] = d->data[1];
        row1Ref = readU16(buf);

        buf[0] = d->data[2];
        buf[1] = d->data[3];
        row2Ref = readU16(buf);

        buf[0] = d->data[4];
        buf[1] = 0;
        col1Ref = readU16(buf);

        buf[0] = d->data[5];
        buf[1] = 0;
        col2Ref = readU16(buf);

        row1Relative = row2Ref & 0x8000;
        col1Relative = row2Ref & 0x4000;
        row1Ref &= 0x3fff;

        row2Relative = row2Ref & 0x8000;
        col2Relative = row2Ref & 0x4000;
        row2Ref &= 0x3fff;

        if (relative) {
            if (row1Ref & 0x2000) row1Ref -= 0x4000;
            if (row2Ref & 0x2000) row2Ref -= 0x4000;
            if (col1Ref & 0x80) col1Ref -= 0x100;
            if (col2Ref & 0x80) col2Ref -= 0x100;
        }
    }

    if (relative) {
        row1Ref += row;
        row2Ref += row;
        col1Ref += col;
        col2Ref += col;
    }

    QString result;
    result.append(QString("["));    // OpenDocument format

    if (!col1Relative)
        result.append(QString("$"));
    result.append(Cell::columnLabel(col1Ref));
    if (!row1Relative)
        result.append(QString("$"));
    result.append(QString::number(row1Ref + 1));
    result.append(QString(":"));
    if (!col2Relative)
        result.append(QString("$"));
    result.append(Cell::columnLabel(col2Ref));
    if (!row2Relative)
        result.append(QString("$"));
    result.append(QString::number(row2Ref + 1));

    result.append(QString("]"));  // OpenDocument format

    return result;
}

QString FormulaToken::area3d(const std::vector<QString>& externSheets, unsigned /*row*/, unsigned /*col*/) const
{
    if (version() != Excel97) {
        return QString("Unknown");
    }

    unsigned sheetRef = readU16(&d->data[0]);

    // FIXME check data size !
    unsigned char buf[2];
    int row1Ref, row2Ref, col1Ref, col2Ref;
    bool row1Relative, col1Relative;
    bool row2Relative, col2Relative;

    buf[0] = d->data[2];
    buf[1] = d->data[3];
    row1Ref = readU16(buf);

    buf[0] = d->data[4];
    buf[1] = d->data[5];
    row2Ref = readU16(buf);

    buf[0] = d->data[6];
    buf[1] = d->data[7];
    col1Ref = readU16(buf);

    buf[0] = d->data[8];
    buf[1] = d->data[9];
    col2Ref = readU16(buf);

    row1Relative = col1Ref & 0x8000;
    col1Relative = col1Ref & 0x4000;
    col1Ref &= 0x3fff;

    row2Relative = col2Ref & 0x8000;
    col2Relative = col2Ref & 0x4000;
    col2Ref &= 0x3fff;

    QString result;
    result.append(QString("["));    // OpenDocument format

    if (sheetRef >= externSheets.size())
        result.append(QString("Error"));
    else
        result.append(externSheets[sheetRef]);
    result.append(QString("."));

    if (!col1Relative)
        result.append(QString("$"));
    result.append(Cell::columnLabel(col1Ref));
    if (!row1Relative)
        result.append(QString("$"));
    result.append(QString::number(row1Ref + 1));
    result.append(QString(":"));
    if (!col2Relative)
        result.append(QString("$"));
    result.append(Cell::columnLabel(col2Ref));
    if (!row2Relative)
        result.append(QString("$"));
    result.append(QString::number(row2Ref + 1));

    result.append(QString("]"));  // OpenDocument format

    return result;
}

std::pair<unsigned, QRect> FormulaToken::filterArea3d() const
{
    if (version() != Excel97) {
        return std::make_pair(0, QRect());
    }

    unsigned sheetRef = readU16(&d->data[0]);

    // FIXME check data size !
    unsigned char buf[2];
    int row1Ref, row2Ref, col1Ref, col2Ref;

    buf[0] = d->data[2];
    buf[1] = d->data[3];
    row1Ref = readU16(buf);

    buf[0] = d->data[4];
    buf[1] = d->data[5];
    row2Ref = readU16(buf);

    buf[0] = d->data[6];
    buf[1] = d->data[7];
    col1Ref = readU16(buf);

    buf[0] = d->data[8];
    buf[1] = d->data[9];
    col2Ref = readU16(buf);

    col1Ref &= 0x3fff;
    col2Ref &= 0x3fff;

    QRect range(col1Ref, row1Ref, col2Ref - col1Ref + 1, row2Ref - row1Ref + 1);
    return std::make_pair(sheetRef, range);
}

QString FormulaToken::areaMap(unsigned row, unsigned col)
{
    unsigned char buf[4];
    buf[0] = d->data[0];
    unsigned ptg = readU8(buf);
    const int type = (ptg & 0x20 ? 1 : 0) + (ptg & 0x60 ? 2 : 0);
    //Q_ASSERT(type == 1 || type == 2 || type == 3);
    buf[0] = d->data[5];
    buf[1] = d->data[6];
    unsigned cce = readU16(buf);
    //printf( "SIZE=%i\n", cce );
    if (cce < 7) {
        printf("Error: Invalid size %i for formula areaMap of type %i\n", cce, type);
        return QString();
    }

    // remove the first seven elements cause they are done
    d->data.erase(d->data.begin(), d->data.begin() + 7);
    //unsigned size, const unsigned char* data

    QString result;
    switch (type) {
    case 0x01: // REFERENCE, specifies a reference to a range.
        result = ref(row, col);
        break;
    case 0x02: // VALUE, specifies a single value of a simple type. The type can be a Boolean, a number, a string, or an error code.
        result = value().asString();
        break;
    case 0x03: // ARRAY, specifies an array of values.
        result = array(row, col);
        break;
    }

    //d->data.erase(d->data.begin(), d->data.begin() + cce);
    return result;
}

QString FormulaToken::ref(unsigned /*row*/, unsigned /*col*/) const
{
    // FIXME check data size !
    // FIXME handle shared formula
    unsigned char buf[2];
    int rowRef, colRef;
    bool rowRelative, colRelative;

    if (version() == Excel97) {
        buf[0] = d->data[0];
        buf[1] = d->data[1];
        rowRef = readU16(buf);

        buf[0] = d->data[2];
        buf[1] = d->data[3];
        colRef = readU16(buf);

        rowRelative = colRef & 0x8000;
        colRelative = colRef & 0x4000;
        colRef &= 0x3fff;
    } else {
        buf[0] = d->data[0];
        buf[1] = d->data[1];
        rowRef = readU16(buf);

        buf[0] = d->data[2];
        buf[1] = 0;
        colRef = readU16(buf);

        rowRelative = rowRef & 0x8000;
        colRelative = rowRef & 0x4000;
        rowRef &= 0x3fff;
    }

    QString result;

    result.append(QString("["));    // OpenDocument format

    if (!colRelative)
        result.append(QString("$"));
    result.append(Cell::columnLabel(colRef));
    if (!rowRelative)
        result.append(QString("$"));
    result.append(QString::number(rowRef + 1));

    result.append(QString("]"));  // OpenDocument format

    return result;
}

QString FormulaToken::refn(unsigned row, unsigned col) const
{
    // FIXME check data size !
    // FIXME handle shared formula
    unsigned char buf[2];
    int rowRef, colRef;
    bool rowRelative, colRelative;

    if (version() == Excel97) {
        buf[0] = d->data[0];
        buf[1] = d->data[1];
        rowRef = readS16(buf);

        buf[0] = d->data[2];
        buf[1] = d->data[3];
        colRef = readU16(buf);

        rowRelative = colRef & 0x8000;
        colRelative = colRef & 0x4000;
        colRef &= 0xff;
        if (colRef & 0x80) {
            colRef = colRef - 0x100;
        }
    } else {
        buf[0] = d->data[0];
        buf[1] = d->data[1];
        rowRef = readU16(buf);

        buf[0] = d->data[2];
        colRef = readS8(buf);

        rowRelative = rowRef & 0x8000;
        colRelative = rowRef & 0x4000;
        rowRef &= 0x3fff;
        if (rowRef & 0x2000) {
            rowRef = rowRef - 0x4000;
        }
    }

    if (colRelative) colRef += col;
    if (rowRelative) rowRef += row;
    QString result;

    result.append(QString("["));    // OpenDocument format

    if (!colRelative)
        result.append(QString("$"));
    result.append(Cell::columnLabel(qMax(0, colRef)));
    if (!rowRelative)
        result.append(QString("$"));
    result.append(QString::number(rowRef + 1));

    result.append(QString("]"));  // OpenDocument format

    return result;
}

QString FormulaToken::ref3d(const std::vector<QString>& externSheets, unsigned /*row*/, unsigned /*col*/) const
{
    if (version() != Excel97) {
        return QString("Unknown");
    }

    unsigned sheetRef = readU16(&d->data[0]);

    // FIXME check data size !
    // FIXME handle shared formula
    unsigned char buf[2];
    int rowRef, colRef;
    bool rowRelative, colRelative;

    buf[0] = d->data[2];
    buf[1] = d->data[3];
    rowRef = readU16(buf);

    buf[0] = d->data[4];
    buf[1] = d->data[5];
    colRef = readU16(buf);

    rowRelative = colRef & 0x8000;
    colRelative = colRef & 0x4000;
    colRef &= 0x3fff;

    QString result;

    result.append(QString("["));    // OpenDocument format

    if (sheetRef >= externSheets.size())
        result.append(QString("Error"));
    else
        result.append(externSheets[sheetRef]);
    result.append(QString("."));

    if (!colRelative)
        result.append(QString("$"));
    result.append(Cell::columnLabel(colRef));
    if (!rowRelative)
        result.append(QString("$"));
    result.append(QString::number(rowRef + 1));

    result.append(QString("]"));  // OpenDocument format


    return result;
}

QString FormulaToken::array(unsigned row, unsigned col) const
{
    /*
    unsigned char buf[2];
    buf[0] = d->data[1]; // specs say this should be at the first byte but seems its not true...
    const unsigned opts = readU8(buf);
    const bool reserved1 = opts & 0x01;
    const bool reserved2 = opts & 0x02;
    const bool reserved3 = opts & 0x04;
    const bool reserved4 = opts & 0x08;
    const bool reserved5 = opts & 0x10;
    Q_ASSERT(!reserved1 && !reserved2 && !reserved3 && !reserved4 && !reserved5);
    const int type = ((opts & 0x20) ? 1 : 0) + ((opts & 0x60) ? 2 : 0);
    printf("%i\n",type);
    Q_ASSERT(type == 2 || type == 3);
    // remove the first two elements cause they are done
    d->data.erase(d->data.begin(), d->data.begin() + 2);
    QString result;
    switch (type) {
    case 0x01: // REFERENCE, specifies a reference to a range.
        result = ref(row, col);
        break;
    case 0x02: // VALUE, specifies a single value of a simple type. The type can be a Boolean, a number, a string, or an error code.
        result = value().asString();
        break;
    case 0x03: // ARRAY, specifies an array of values.
        result = array(row, col);
        break;
    }
    //Q_ASSERT(false);
    return result;
    */
    return QString();
}

std::pair<unsigned, unsigned> FormulaToken::baseFormulaRecord() const
{
    if (version() == Excel97) {
        return std::make_pair(readU16(&d->data[0]), readU16(&d->data[2]));
    } else {
        return std::make_pair(readU16(&d->data[0]), (unsigned)d->data[2]);
    }
}

std::ostream& operator<<(std::ostream& s,  Swinder::FormulaToken token)
{
    s << std::setw(2) << std::hex << token.id() << std::dec;
    // s  << "  Size: " << std::dec << token.size();
    s << "  ";

    switch (token.id()) {
    case FormulaToken::ErrorCode:
    case FormulaToken::Bool:
    case FormulaToken::Integer:
    case FormulaToken::Float:
    case FormulaToken::String: {
        Value v = token.value();
        s << v;
    }
    break;

    case FormulaToken::Function:
        s << "Function " << token.functionName();
        break;

    default:
        s << token.idAsString();
        break;
    }

    return s;
}

FormulaTokens FormulaDecoder::decodeFormula(unsigned size, unsigned pos, const unsigned char* data, unsigned version)
{
    FormulaTokens tokens;
    const unsigned formula_len = readU16(data + pos);
    for (unsigned j = pos + 2; j < size;) {
        unsigned ptg = data[j++];
        ptg = ((ptg & 0x40) ? (ptg | 0x20) : ptg) & 0x3F;
        FormulaToken t(ptg);
        t.setVersion(version);

        if (t.id() == FormulaToken::String) {
            // find bytes taken to represent the string
            EString estr = (version == Excel97) ?
                           EString::fromUnicodeString(data + j, false, formula_len) :
                           EString::fromByteString(data + j, false, formula_len);
            t.setData(estr.size(), data + j);
            j += estr.size();
        } else {
            // normal, fixed-size token
            if (t.size() > 0) {
                t.setData(t.size(), data + j);
                j += t.size();
            }
        }
        tokens.push_back(t);
    }
    return tokens;
}

typedef std::vector<QString> UStringStack;

static void mergeTokens(UStringStack* stack, unsigned count, QString mergeString)
{
    if (!stack) return;
    if (stack->size() < count) return;

    QString s1, s2;

    while (count) {
        count--;

        QString last = (*stack)[stack->size()-1];
        QString tmp = last;
        tmp.append(s1);
        s1 = tmp;

        if (count) {
            tmp = mergeString;
            tmp.append(s1);
            s1 = tmp;
        }

        stack->resize(stack->size() - 1);
    }

    stack->push_back(s1);
}

#ifdef SWINDER_XLS2RAW
static void dumpStack(std::vector<QString> stack)
{
    std::cout << std::endl;
    std::cout << "Stack now is: " ;
    if (!stack.size())
        std::cout << "(empty)" ;

    for (unsigned i = 0; i < stack.size(); i++)
        std::cout << "  " << i << ": " << stack[i] << std::endl;
    std::cout << std::endl;
}
#endif

QString FormulaDecoder::decodeFormula(unsigned row, unsigned col, bool isShared, const FormulaTokens& tokens)
{
    UStringStack stack;

    for (unsigned c = 0; c < tokens.size(); c++) {
        FormulaToken token = tokens[c];

#ifdef SWINDER_XLS2RAW
        std::cout << "Formula Token " << c << ": ";
        std::cout <<  token.id() << "  ";
        std::cout << token.idAsString() << std::endl;
#endif

        switch (token.id()) {
        case FormulaToken::Add:
            mergeTokens(&stack, 2, QString("+"));
            break;

        case FormulaToken::Sub:
            mergeTokens(&stack, 2, QString("-"));
            break;

        case FormulaToken::Mul:
            mergeTokens(&stack, 2, QString("*"));
            break;

        case FormulaToken::Div:
            mergeTokens(&stack, 2, QString("/"));
            break;

        case FormulaToken::Power:
            mergeTokens(&stack, 2, QString("^"));
            break;

        case FormulaToken::Concat:
            mergeTokens(&stack, 2, QString("&"));
            break;

        case FormulaToken::LT:
            mergeTokens(&stack, 2, QString("<"));
            break;

        case FormulaToken::LE:
            mergeTokens(&stack, 2, QString("<="));
            break;

        case FormulaToken::EQ:
            mergeTokens(&stack, 2, QString("="));
            break;

        case FormulaToken::GE:
            mergeTokens(&stack, 2, QString(">="));
            break;

        case FormulaToken::GT:
            mergeTokens(&stack, 2, QString(">"));
            break;

        case FormulaToken::NE:
            mergeTokens(&stack, 2, QString("<>"));
            break;

        case FormulaToken::Intersect:
            mergeTokens(&stack, 2, QString("!"));
            break;

        case FormulaToken::Union:
            mergeTokens(&stack, 2, QString("~"));
            break;

        case FormulaToken::Range:
            mergeTokens(&stack, 2, QString(":"));
            break;

        case FormulaToken::UPlus: {
            QString str("+");
            str.append(stack[stack.size()-1]);
            stack[stack.size()-1] = str;
            break;
        }

        case FormulaToken::UMinus: {
            QString str("-");
            str.append(stack[ stack.size()-1 ]);
            stack[stack.size()-1] = str;
            break;
        }

        case FormulaToken::Percent:
            stack[stack.size()-1].append(QString("%"));
            break;

        case FormulaToken::Paren: {
            QString str("(");
            str.append(stack[ stack.size()-1 ]);
            str.append(QString(")"));
            stack[stack.size()-1] = str;
            break;
        }

        case FormulaToken::MissArg:
            // just ignore
            stack.push_back(QString(" "));
            break;

        case FormulaToken::String: {
            QString str('\"');
            str.append(token.value().asString());
            str.append(QString('\"'));
            stack.push_back(str);
            break;
        }

        case FormulaToken::Bool:
            if (token.value().asBoolean())
                stack.push_back(QString("TRUE"));
            else
                stack.push_back(QString("FALSE"));
            break;

        case FormulaToken::Integer:
            stack.push_back(QString::number(token.value().asInteger()));
            break;

        case FormulaToken::Float:
            stack.push_back(QString::number(token.value().asFloat()));
            break;

        case FormulaToken::Array:
            stack.push_back(token.array(row, col));
            break;

        case FormulaToken::Ref:
            stack.push_back(token.ref(row, col));
            break;

        case FormulaToken::RefN:
            stack.push_back(token.refn(row, col));
            break;

        case FormulaToken::Ref3d:
            stack.push_back(token.ref3d(externSheets(), row, col));
            break;

        case FormulaToken::Area:
            stack.push_back(token.area(row, col));
            break;

        case FormulaToken::AreaN:
            stack.push_back(token.area(row, col, true));
            break;

        case FormulaToken::Area3d:
            stack.push_back(token.area3d(externSheets(), row, col));
            break;

        case FormulaToken::Function: {
            mergeTokens(&stack, token.functionParams(), QString(";"));
            if (stack.size()) {
                QString str(token.functionName() ? token.functionName() : "??");
                str.append(QString("("));
                str.append(stack[stack.size()-1]);
                str.append(QString(")"));
                stack[stack.size()-1] = str;
            }
            break;
        }

        case FormulaToken::FunctionVar:
            if (token.functionIndex() != 255) {
                mergeTokens(&stack, token.functionParams(), QString(";"));
                if (stack.size()) {
                    QString str;
                    if (token.functionIndex() != 255)
                        str = token.functionName() ? token.functionName() : "??";
                    str.append(QString("("));
                    str.append(stack[stack.size()-1]);
                    str.append(QString(")"));
                    stack[stack.size()-1] = str;
                }
            } else {
                unsigned count = token.functionParams() - 1;
                mergeTokens(&stack, count, QString(";"));
                if (stack.size()) {
                    QString str;
                    str.append(QString("("));
                    str.append(stack[ stack.size()-1 ]);
                    str.append(QString(")"));
                    stack[stack.size()-1] = str;
                }
            }
            break;

        case FormulaToken::Attr:
            if (token.attr() & 0x10) { // SUM
                mergeTokens(&stack, 1, QString(";"));
                if (stack.size()) {
                    QString str("SUM");
                    str.append(QString("("));
                    str.append(stack[ stack.size()-1 ]);
                    str.append(QString(")"));
                    stack[stack.size()-1] = str;
                }
            }
            break;

        case FormulaToken::Name:
            stack.push_back(nameFromIndex(token.nameIndex()-1));
            break;

        case FormulaToken::NameX:
            stack.push_back(externNameFromIndex(token.nameXIndex()-1));
            break;

        case FormulaToken::Matrix: {
            std::pair<unsigned, unsigned> formulaCellPos = token.baseFormulaRecord();
            if( isShared ) {
              FormulaTokens ft = sharedFormulas(formulaCellPos);
              if (ft.size() > 0)
                  stack.push_back(decodeFormula(row, col, isShared, ft));
            } else {
              // "2.5.198.58 PtgExp" says that if its not a sharedFormula then it's an indication that the
              // result is an reference to cells. So, we can savly ignore that case...
              std::cout << "MATRIX first=%i second=" << formulaCellPos.first << " " << formulaCellPos.second << std::endl;
            }
            break;
        }

        case FormulaToken::Table: {
            std::pair<unsigned, unsigned> formulaCellPos = token.baseFormulaRecord();
            if( isShared ) {
              DataTableRecord* dt = tableRecord(formulaCellPos);
              if(dt)
                  stack.push_back(dataTableFormula(row, col, dt));
            } else {
              std::cout << "TABLE first=%i second=" << formulaCellPos.first << " " << formulaCellPos.second << std::endl;
            }
            break;
        }

        case FormulaToken::MemArea:
            // does nothing
            break;

        case FormulaToken::AreaErr:
        case FormulaToken::AreaErr3d:
        case FormulaToken::RefErr:
        case FormulaToken::RefErr3d:
            stack.push_back(QString("#REF!"));
            break;

        case FormulaToken::MemErr: // specifies that the result is an error-code
            break;

        case FormulaToken::ErrorCode:
            stack.push_back(token.value().asString());
            break;

        case 0: break; // NOPE

        case FormulaToken::MemFunc:
            // as far as I can tell this is only meta-data
            break;

        case FormulaToken::NatFormula:
        case FormulaToken::Sheet:
        case FormulaToken::EndSheet:
        case FormulaToken::MemNoMem:
        case FormulaToken::MemAreaN:
        case FormulaToken::MemNoMemN:
        default:
            // FIXME handle this !
            std::cout << "Unhandled token=" << token.idAsString() << std::endl;
            stack.push_back(QString("Unknown"));
            break;
        };

#ifdef SWINDER_XLS2RAW
        dumpStack(stack);
#endif
    }

    QString result;
    for (unsigned i = 0; i < stack.size(); i++)
        result.append(stack[i]);

#ifdef SWINDER_XLS2RAW
    std::cout << "FORMULA Result: " << result << std::endl;
#endif
    return result;
}

QString FormulaDecoder::dataTableFormula(unsigned row, unsigned col, const DataTableRecord* record)
{
    QString result("MULTIPLE.OPERATIONS(");

    unsigned formulaRow = 0, formulaCol = 0;
    switch (record->direction()) {
    case DataTableRecord::InputRow:
        formulaRow = row;
        formulaCol = record->firstColumn() - 1;
        break;
    case DataTableRecord::InputColumn:
        formulaRow = record->firstRow() - 1;
        formulaCol = col;
        break;
    case DataTableRecord::Input2D:
        formulaRow = record->firstRow() - 1;
        formulaCol = record->firstColumn() - 1;
        break;
    }

    result.append(QString("[.$"));
    result.append(Cell::columnLabel(formulaCol));
    result.append(QString("$"));
    result.append(QString::number(formulaRow + 1));
    result.append(QString("]"));

    if (record->direction() == DataTableRecord::Input2D) {
        result.append(QString(";[.$"));
        result.append(Cell::columnLabel(record->inputColumn2()));
        result.append(QString("$"));
        result.append(QString::number(record->inputRow2() + 1));
        result.append(QString("]"));
    } else {
        result.append(QString(";[.$"));
        result.append(Cell::columnLabel(record->inputColumn1()));
        result.append(QString("$"));
        result.append(QString::number(record->inputRow1() + 1));
        result.append(QString("]"));
    }

    if (record->direction() == DataTableRecord::Input2D || record->direction() == DataTableRecord::InputColumn) {
        result.append(QString(";[.$"));
        result.append(Cell::columnLabel(record->firstColumn() - 1));
        result.append(QString::number(row + 1));
        result.append(QString("]"));
    }

    if (record->direction() == DataTableRecord::Input2D) {
        result.append(QString(";[.$"));
        result.append(Cell::columnLabel(record->inputColumn1()));
        result.append(QString("$"));
        result.append(QString::number(record->inputRow1() + 1));
        result.append(QString("]"));
    }

    if (record->direction() == DataTableRecord::Input2D || record->direction() == DataTableRecord::InputRow) {
        result.append(QString(";[."));
        result.append(Cell::columnLabel(col));
        result.append(QString("$"));
        result.append(QString::number(record->firstRow() - 1 + 1));
        result.append(QString("]"));
    }

    result.append(QString(")"));

#ifdef SWINDER_XLS2RAW
    std::cout << "DATATABLE Result: " << result << std::endl;
#endif
    return result;
}

} // namespace Swinder
