/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SheetsDebug.h"

const QLoggingCategory &SHEETS_LOG()
{
    static const QLoggingCategory category("calligra.sheets");
    return category;
}

const QLoggingCategory &SHEETSFORMULA_LOG()
{
    static const QLoggingCategory category("calligra.sheets.formula");
    return category;
}

const QLoggingCategory &SHEETSODF_LOG()
{
    static const QLoggingCategory category("calligra.sheets.odf");
    return category;
}

const QLoggingCategory &SHEETSRENDER_LOG()
{
    static const QLoggingCategory category("calligra.sheets.render");
    return category;
}

const QLoggingCategory &SHEETSUI_LOG()
{
    static const QLoggingCategory category("calligra.sheets.ui");
    return category;
}

const QLoggingCategory &SHEETSSTYLE_LOG()
{
    static const QLoggingCategory category("calligra.sheets.style");
    return category;
}

const QLoggingCategory &SHEETSDAMAGE_LOG()
{
    static const QLoggingCategory category("calligra.sheets.damage");
    return category;
}

const QLoggingCategory &SHEETSTABLESHAPE_LOG()
{
    static const QLoggingCategory category("calligra.sheets.tableshape");
    return category;
}
