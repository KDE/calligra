/* This file is part of the KDE project
   Copyright (C) 2001 Graham Short <grahshrt@netscape.net>

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

#include <qproformula.h>

static QpFormulaConv gOverride[] =
{
   {20,  QpFormula::func2,         "AND("},    // (e.g.)  2 #AND# 3  => AND(2;3)
   {21,  QpFormula::func2,         "OR("},     // (e.g.)  2 #OR# 3  => OR(2;3)
   {22,  QpFormula::func1,         "NOT("},    // (e.g.)  #NOT# 3  => NOT(3)
   {24,  QpFormula::binaryOperand, "+"},       // string concat "&" => "+"
   {32,  QpFormula::func0,         "ERR()"},   // err => ERR()
   {34,  QpFormula::func1,         "INT("},    // int => INT
   {38,  QpFormula::func0,         "PI()"},    // pi => PI()
   {47,  QpFormula::func2,         "MOD("},    // mod => MOD
   {0,   0,                        0}
};

KSpreadFormula::KSpreadFormula(QpRecFormulaCell& pCell, QpTableNames& pTable)
   : QpFormula(pCell, pTable)
{
   formulaStart("=");   // quattro pro starts formulas with "+"
                        // kspread uses "="
   dropLeadingAt();     // quattro pro starts it's functions with '@'
                        // kspread doesn't like this
   argSeparator(";");   // quattro pro separates function arguments with ","
                        // kspread likes ";"

   // override some of the default conversions
   replaceFunc(gOverride);
}

KSpreadFormula::~KSpreadFormula()
{
}
