/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Graham Short <grahshrt@netscape.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "qproformula.h"

static QpFormulaConv gOverride[] = {
    {14,  QpFormula::binaryOperand, "=="},      // '=' => '=='
    {15,  QpFormula::binaryOperand, "!="},      // '<>' => '!='
    {20,  QpFormula::func2,         "AND("},    // (e.g.)  2 #AND# 3  => AND(2;3)
    {21,  QpFormula::func2,         "OR("},     // (e.g.)  2 #OR# 3  => OR(2;3)
    {22,  QpFormula::func1,         "NOT("},    // (e.g.)  #NOT# 3  => NOT(3)
    {24,  QpFormula::binaryOperand, "+"},       // string concat "&" => "+"
    {32,  QpFormula::func0,         "ERR()"},   // @err => ERR()
    {33,  QpFormula::absKludge,     nullptr},         // @abs => if( (arg)<0; -(arg); (arg))
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
    {0,   nullptr,                        nullptr}
};

Calligra::Sheets::QuattroPro::Formula::Formula(QpRecFormulaCell& pCell, QpTableNames& pTable)
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

Calligra::Sheets::QuattroPro::Formula::~Formula()
= default;
