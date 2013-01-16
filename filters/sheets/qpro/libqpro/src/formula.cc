/*
 *  Copyright (c) 2001 Graham Short.  <grahshrt@netscape.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <qpro/common.h>

#include <iostream>
#include <strstream>

#include <string.h>

#include <qpro/formula.h>
#include <qpro/stream.h>

// ------------------------------------------------------------------


QpFormulaStack::QpFormulaStack()
        : cIdx(-1)
        , cMax(3)
{
    cStack = new char*[cMax];
}

QpFormulaStack::~QpFormulaStack()
{
    while (cIdx >= 0) {
        delete [] cStack[cIdx--];
    }
    delete cStack;
}

void
QpFormulaStack::bracket(const char* pBefore, const char* pAfter)
{
    if (cIdx >= 0) {
        int lLen = strlen(cStack[cIdx]) + 1;

        if (pBefore) lLen += strlen(pBefore);

        if (pAfter) lLen += strlen(pAfter);

        char* lNew = new char[ lLen ];

        lNew[0] = '\0';

        if (pBefore) strcpy(lNew, pBefore);

        strcat(lNew, cStack[cIdx]);

        if (pAfter) strcat(lNew, pAfter);

        delete [] cStack[cIdx];
        cStack[cIdx] = lNew;
    }
}

void
QpFormulaStack::join(int pCnt, const char* pSeparator)
{
    int lFirstIdx = 1 - pCnt;  // really 0 - pCnt +1

    if (pCnt > 0 && (cIdx + lFirstIdx) >= 0) {
        int lSepLen = strlen(pSeparator);
        int lLen    = lSepLen * (pCnt - 1) + 1;  // +1 for null terminator
        for (int lIdx = lFirstIdx; lIdx <= 0; ++lIdx) {
            lLen += strlen(cStack[cIdx + lIdx]);
        }

        char* lNew = new char[lLen];

        lNew[0] = '\0';

        for (int lIdx = lFirstIdx; lIdx <= 0; ++lIdx) {
            strcat(lNew, cStack[cIdx + lIdx]);
            if (lIdx != 0) {
                strcat(lNew, pSeparator);
            }
        }

        pop(pCnt);

        push(lNew);

        delete [] lNew;
    }
}

void
QpFormulaStack::pop(int pCnt)
{
    while (cIdx >= 0 && pCnt--) {
        delete [] cStack[cIdx--];
    }
}

void
QpFormulaStack::push(const char* pString)
{
    ++cIdx;

    if (cIdx == cMax) {
        cMax += 10;

        char** cTemp = new char*[cMax];

        for (int lIdx = 0; lIdx < cIdx; ++lIdx) {
            cTemp[lIdx] = cStack[lIdx];
        }

        delete [] cStack;
        cStack = cTemp;
    }

    cStack[cIdx] = strcpy(new char[strlen(pString)+1], pString);
}

const char*
QpFormulaStack::top()
{
    return (cIdx >= 0 ? cStack[cIdx] : 0);
}

const char*
QpFormulaStack::operator [](int pIdx)
{
    char* lResult = 0;

    if (pIdx <= 0 && (cIdx + pIdx) >= 0) {
        lResult = cStack[cIdx + pIdx];
    }

    return lResult;
}

// ------------------------------------------------------------------

static const QpFormulaConv gConv[] = {
    {0,   QpFormula::floatFunc,       0},
    {1,   QpFormula::ref,             0},
    {2,   QpFormula::ref,             0},
    {4,   QpFormula::func1,         "("},
    {5,   QpFormula::intFunc,         0},
    {6,   QpFormula::stringFunc,      0},
// {7, default ??? don't know what this is ???
    {8,   QpFormula::unaryOperand,  "-"},
    {9,   QpFormula::binaryOperand, "+"},
    {10,  QpFormula::binaryOperand, "-"},
    {11,  QpFormula::binaryOperand, "*"},
    {12,  QpFormula::binaryOperand, "/"},
    {13,  QpFormula::binaryOperand, "^"},
    {14,  QpFormula::binaryOperand, "="},
    {15,  QpFormula::binaryOperand, "<>"},
    {16,  QpFormula::binaryOperand, "<="},
    {17,  QpFormula::binaryOperand, ">="},
    {18,  QpFormula::binaryOperand, "<"},
    {19,  QpFormula::binaryOperand, ">"},
    {20,  QpFormula::binaryOperand, "#AND#"},
    {21,  QpFormula::binaryOperand, "#OR#"},
    {22,  QpFormula::unaryOperand,  "#NOT#"},
    {23,  QpFormula::unaryOperand,  "+"},
    {24,  QpFormula::binaryOperand, "&"},
// {25, Halt ??? don't know what this is ???
// {26, dll ??? don't know what this is ???
// {27, extended no operands ??? don't know what this is ???
// {28, extended operands ??? don't know what this is ???
// {29, Reserved
// {30, Reserved
// {31, NA
    {32,  QpFormula::func0,         "@err"},
    {33,  QpFormula::func1,         "@abs("},
    {34,  QpFormula::func1,         "@int("},
    {35,  QpFormula::func1,         "@sqrt("},
    {36,  QpFormula::func1,         "@log("},
    {37,  QpFormula::func1,         "@ln("},
    {38,  QpFormula::func0,         "@pi"},
    {39,  QpFormula::func1,         "@sin("},
    {40,  QpFormula::func1,         "@cos("},
    {41,  QpFormula::func1,         "@tan("},
    {42,  QpFormula::func2,         "@atan2("},
    {43,  QpFormula::func1,         "@atan("},
    {44,  QpFormula::func1,         "@asin("},
    {45,  QpFormula::func1,         "@acos("},
    {46,  QpFormula::func1,         "@exp("},
    {47,  QpFormula::func2,         "@mod("},
    {48,  QpFormula::funcV,         "@choose("},
    {49,  QpFormula::func1,         "@isna("},
    {50,  QpFormula::func1,         "@iserr("},
    {51,  QpFormula::func0,         "@false"},
    {52,  QpFormula::func0,         "@true"},
    {53,  QpFormula::func0,         "@rand"},
    {54,  QpFormula::func3,         "@date("},
    {55,  QpFormula::func0,         "@now"},
    {56,  QpFormula::func3,         "@pmt("},
    {57,  QpFormula::func3,         "@pv("},
    {58,  QpFormula::func3,         "@fv("},
    {59,  QpFormula::func3,         "@if("},
    {60,  QpFormula::func1,         "@day("},
    {61,  QpFormula::func1,         "@month("},
    {62,  QpFormula::func1,         "@year("},
    {63,  QpFormula::func2,         "@round("},
    {64,  QpFormula::func3,         "@time("},
    {65,  QpFormula::func1,         "@hour("},
    {66,  QpFormula::func1,         "@minute("},
    {67,  QpFormula::func1,         "@second("},
    {68,  QpFormula::func1,         "@isnumber("},
    {69,  QpFormula::func1,         "@isstring("},
    {70,  QpFormula::func1,         "@length("},
    {71,  QpFormula::func1,         "@value("},
    {72,  QpFormula::func2,         "@string("},
    {73,  QpFormula::func3,         "@mid("},
    {74,  QpFormula::func1,         "@char("},
    {75,  QpFormula::func1,         "@code("},
    {76,  QpFormula::func3,         "@find("},
    {77,  QpFormula::func1,         "@dateVal("},
    {78,  QpFormula::func1,         "@timeVal("},
    {79,  QpFormula::func1,         "@cellPtr("},
    {80,  QpFormula::funcV,         "@sum("},
    {81,  QpFormula::funcV,         "@avg("},
    {82,  QpFormula::funcV,         "@count("},
    {83,  QpFormula::funcV,         "@min("},
    {84,  QpFormula::funcV,         "@max("},
    {85,  QpFormula::func3,         "@vlookup("},  // would have expected func4 ???
    {86,  QpFormula::func2,         "@npv("},
    {87,  QpFormula::funcV,         "@var("},
    {88,  QpFormula::funcV,         "@std("},
    {89,  QpFormula::func2,         "@irr("},
    {90,  QpFormula::func3,         "@hlookup("},  // would have expected func4 ???
    {91,  QpFormula::func3,         "@dsum("},
    {92,  QpFormula::func3,         "@davg("},
    {93,  QpFormula::func3,         "@dcount("},
    {94,  QpFormula::func3,         "@dmin("},
    {95,  QpFormula::func3,         "@dmax("},
    {96,  QpFormula::func3,         "@dvar("},
    {97,  QpFormula::func3,         "@dstd("},
    {98,  QpFormula::func3,         "@index("},
    {99,  QpFormula::func1,         "@cols("},
    {100, QpFormula::func1,         "@rows("},
    {101, QpFormula::func2,         "@repeat("},
    {102, QpFormula::func1,         "@upper("},
    {103, QpFormula::func1,         "@lower("},
    {104, QpFormula::func2,         "@left("},
    {105, QpFormula::func2,         "@right("},
    {106, QpFormula::func4,         "@replace("},
    {107, QpFormula::func1,         "@proper("},
    {108, QpFormula::func2,         "@cell("},
    {109, QpFormula::func1,         "@trim("},
    {110, QpFormula::func1,         "@clean("},
    {111, QpFormula::func1,         "@s("},
    {112, QpFormula::func1,         "@n("},
    {113, QpFormula::func1,         "@exact("},
//   {114, QpFormula::func1,         "@call("},
    {115, QpFormula::func1,         "@@("},
    {116, QpFormula::func3,         "@rate("},
    {117, QpFormula::func3,         "@term("},
    {118, QpFormula::func3,         "@cterm("},
    {119, QpFormula::func3,         "@sln("},
    {120, QpFormula::func4,         "@syd("},
    {121, QpFormula::func4,         "@ddb("},
    {122, QpFormula::funcV,         "@stds("},
    {123, QpFormula::funcV,         "@vars("},
    {124, QpFormula::func1,         "@dstds("},
    {125, QpFormula::func1,         "@dvars("},
    {126, QpFormula::func1,         "@pval("},
    {127, QpFormula::func1,         "@paymt("},
    {128, QpFormula::func1,         "@fval("},
    {129, QpFormula::func1,         "@nper("},
    {130, QpFormula::func1,         "@irate("},
    {131, QpFormula::func1,         "@ipaymt("},
    {132, QpFormula::func1,         "@ppaymt("},
    {133, QpFormula::func1,         "@sumproduct("},
    {134, QpFormula::func1,         "@memavail("},
    {135, QpFormula::func1,         "@mememsavail("},
    {136, QpFormula::func1,         "@fileexists("},
    {137, QpFormula::func1,         "@curval("},
    {138, QpFormula::func1,         "@degrees("},
    {139, QpFormula::func1,         "@radians("},
    {140, QpFormula::func1,         "@hextonum("},
    {141, QpFormula::func1,         "@numtohex("},
    {142, QpFormula::func1,         "@today("},
    {143, QpFormula::func3,         "@npv("},
    {144, QpFormula::func1,         "@cellindex2d("},
    {145, QpFormula::func1,         "@version("},
    {154, QpFormula::func1,         "@sheets("},
    {157, QpFormula::func1,         "@index3d("},
    {158, QpFormula::func1,         "@cellindex3d("},
    {159, QpFormula::func1,         "@property("},
    {160, QpFormula::func1,         "@ddelink("},
    {161, QpFormula::func1,         "@command("},
    {0,   0,                             0}
};

QpFormula::QpFormula(QpRecFormulaCell& pCell, QpTableNames& pTable)
        : cArgSeparator(strcpy(new char[2], ","))
        , cCell(pCell)
        , cFormula((unsigned char*)pCell.formula(), (unsigned int)pCell.formulaLen())
        , cFormulaRefs((unsigned char*)&pCell.formula()[pCell.formulaReferences()]
                       , (unsigned)(pCell.formulaLen() - pCell.formulaReferences())
                      )
        , cReplaceFunc(0)
        , cFormulaStart(strcpy(new char[2], "+"))
        , cIdx(0)
        , cDropLeadingAt(0)
        , cTable(pTable)
{
}

QpFormula::~QpFormula()
{
    delete [] cArgSeparator;
    cArgSeparator = 0;

    delete [] cFormulaStart;
    cFormulaStart = 0;

    cReplaceFunc = 0;
}

void
QpFormula::argSeparator(const char* pArg)
{
    delete [] cArgSeparator;
    cArgSeparator = strcpy(new char[strlen(pArg)+1], pArg);
}

char*
QpFormula::formula()
{
    QP_UINT8 lOperand;

    cStack.push(cFormulaStart);

    while (cFormula >> lOperand, cFormula && lOperand != 3) {
        int lFound = 0;
        int lIdx;

        if (cReplaceFunc != 0) {
            // search through override list for this function/operand
            for (lIdx = 0
                        ; !lFound && cReplaceFunc[lIdx].cFunc != 0
                    ; ++lIdx
                ) {
                if (cReplaceFunc[lIdx].cOperand == lOperand) {
                    lFound = -1;
                    QP_DEBUG("Processing " << (int)lOperand << endl);
                    (*cReplaceFunc[lIdx].cFunc)(*this, cReplaceFunc[lIdx].cArg);
                }
            }
        }

        // if no override then find the default
        for (lIdx = 0
                    ; !lFound && gConv[lIdx].cFunc != 0
                ; ++lIdx
            ) {
            if (gConv[lIdx].cOperand == lOperand) {
                lFound = -1;
                QP_DEBUG("Processing " << (int)lOperand << endl);
                (*gConv[lIdx].cFunc)(*this, gConv[lIdx].cArg);
            }
        }

        QP_DEBUG("Top = " << cStack.top() << endl);
    }

    cStack.join(2, "");

    QP_DEBUG("Formula = " << cStack.top() << endl);
    return strcpy(new char[strlen(cStack.top())+1], cStack.top());
}

void
QpFormula::formulaStart(const char* pFirstChar)
{
    delete [] cFormulaStart;
    cFormulaStart = strcpy(new char[strlen(pFirstChar)+1], pFirstChar);
}

void
QpFormula::binaryOperandReal(const char* pOper)
{
    cStack.join(2, pOper);
}

void
QpFormula::absKludgeReal(const char*/*pOper*/)
{
    // kspread doesn't (yet) have the abs function so do it ourselves
    // using 'if( (arg) < 0, -(arg), arg )'

    cStack.bracket();

    char* lArg = strcpy(new char[strlen(cStack.top())+1], cStack.top());

    cStack.bracket("", "<0");

    cStack.push(lArg);
    cStack.bracket("-", "");

    cStack.push(lArg);

    cStack.join(3, cArgSeparator);

    cStack.bracket("if(");

    delete [] lArg;
}

void
QpFormula::func0Real(const char* pFunc)
{
    const char* lFunc = (cDropLeadingAt && pFunc[0] == '@' ? &pFunc[1] : pFunc);

    cStack.push(lFunc);
}

void
QpFormula::func1Real(const char* pFunc)
{
    const char* lFunc = (cDropLeadingAt && pFunc[0] == '@' ? &pFunc[1] : pFunc);

    cStack.bracket(lFunc);
}

void
QpFormula::func2Real(const char* pFunc)
{
    const char* lFunc = (cDropLeadingAt && pFunc[0] == '@' ? &pFunc[1] : pFunc);

    cStack.join(2, cArgSeparator);
    cStack.bracket(lFunc);
}

void
QpFormula::func3Real(const char* pFunc)
{
    const char* lFunc = (cDropLeadingAt && pFunc[0] == '@' ? &pFunc[1] : pFunc);

    cStack.join(3, cArgSeparator);
    cStack.bracket(lFunc);
}

void
QpFormula::func4Real(const char* pFunc)
{
    const char* lFunc = (cDropLeadingAt && pFunc[0] == '@' ? &pFunc[1] : pFunc);

    cStack.join(4, cArgSeparator);
    cStack.bracket(lFunc);
}

void
QpFormula::funcVReal(const char* pFunc)
{
    QP_INT8     lCnt;
    const char* lFunc = (cDropLeadingAt && pFunc[0] == '@' ? &pFunc[1] : pFunc);

    cFormula >> lCnt;

    cStack.join(lCnt, cArgSeparator);
    cStack.bracket(lFunc);
}

void
QpFormula::floatFuncReal(const char*)
{
    QP_INT64   lFloat;
    std::ostrstream lNum;

    cFormula >> lFloat;

    lNum << lFloat << ends;

    cStack.push(lNum.str());

    lNum.rdbuf()->freeze(0);
}

void
QpFormula::intFuncReal(const char*)
{
    QP_INT16 lInt;
    std::ostrstream lNum;

    cFormula >> lInt;

    lNum << lInt << ends;

    cStack.push(lNum.str());

    lNum.rdbuf()->freeze(0);
}

void
QpFormula::dropLeadingAt(int pBool)
{
    cDropLeadingAt = pBool;
}

void
QpFormula::refReal(const char*)
{
    char lRef[100];  // ??? hard coded length

    cCell.cellRef(lRef, cTable, cFormulaRefs);

    cStack.push(lRef);
}

void
QpFormula::replaceFunc(QpFormulaConv* pFuncEntry)
{
    cReplaceFunc = pFuncEntry;
}

void
QpFormula::stringFuncReal(const char*)
{
    char* lString = 0;

    cFormula >> lString;

    char* lQuoteString = new char[strlen(lString)+3];

    lQuoteString[0] = '"';
    strcpy(&lQuoteString[1], lString);
    strcat(lQuoteString, "\"");

    cStack.push(lQuoteString);

    delete [] lString;
    delete [] lQuoteString;
}

void
QpFormula::unaryOperandReal(const char* pOper)
{
    cStack.bracket(pOper, "");
}


