/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Graham Short <grahshrt@netscape.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef QPROFORMULA_H
#define QPROFORMULA_H

#include <qpro/formula.h>

namespace Calligra
{
namespace Sheets
{
namespace QuattroPro
{

class Formula : public QpFormula
{
public:
    Formula(QpRecFormulaCell &pCell, QpTableNames &pTable);
    ~Formula();
};

}
}
}

#endif // QPROFORMULA_H
