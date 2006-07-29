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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qproformula.h>

static QpFormulaConv gOverride[] =
{
   {14,  QpFormula::binaryOperand, "=="},      // '=' => '=='
   {15,  QpFormula::binaryOperand, "!="},      // '<>' => '!='
   {20,  QpFormula::func2,         "AND("},    // (e.g.)  2 #AND# 3  => AND(2;3)
   {21,  QpFormula::func2,         "OR("},     // (e.g.)  2 #OR# 3  => OR(2;3)
   {22,  QpFormula::func1,         "NOT("},    // (e.g.)  #NOT# 3  => NOT(3)
   {24,  QpFormula::binaryOperand, "+"},       // string concat "&" => "+"
   {32,  QpFormula::func0,         "ERR()"},   // @err => ERR()
   {33,  QpFormula::absKludge,     0},         // @abs => if( (arg)<0; -(arg); (arg))
   {34,  QpFormula::func1,         "INT("},    // @int => INT
   {38,  QpFormula::func0,         "PI()"},    // @pi => PI()
   {47,  QpFormula::func2,         "MOD("},    // @mod => MOD
   {51,  QpFormula::func0,         "(1==0)"},  // @false => (1==0)
   {52,  QpFormula::func0,         "(1==1)"},  // @true  => (1==1)
   {53,  QpFormula::func0,         "rand()"},  // @rand  => rand()
   {68,  QpFormula::func1,         "ISNUM("},  // @isnumber => ISNUM
   {69,  QpFormula::func1,         "ISTEXT("}, // @isstring => ISTEXT
   {70,  QpFormula::func1,         "len("},    // @length   => len
   {81,  QpFormula::funcV,         "average("}, // @avg    => average
   {87,  QpFormula::funcV,         "variance("},// @var    => variance
   {88,  QpFormula::funcV,         "stddev("},  // @std    => stddev
   {101, QpFormula::func2,         "REPT("},    // @repeat => REPT
   {0,   0,                        0}
};

KSpread::Formula::Formula(QpRecFormulaCell& pCell, QpTableNames& pTable)
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

KSpread::Formula::~Formula()
{
}
