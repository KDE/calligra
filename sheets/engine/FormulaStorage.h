/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_FORMULA_STORAGE
#define KSPREAD_FORMULA_STORAGE

#include "Formula.h"
#include "PointStorage.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Storage
 * \ingroup Value
 * Stores formulas.
 */
class FormulaStorage : public PointStorage<Formula>
{
public:
    FormulaStorage &operator=(const PointStorage<Formula> &o)
    {
        PointStorage<Formula>::operator=(o);
        return *this;
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_FORMULA_STORAGE
