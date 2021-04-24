/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "FormulaDebug.h"

const QLoggingCategory &FORMULA_LOG()
{
    static const QLoggingCategory category("calligra.plugin.formulashape");
    return category;
}

const QLoggingCategory &FORMULAELEMENT_LOG()
{
    static const QLoggingCategory category("calligra.plugin.formulashape.element");
    return category;
}
