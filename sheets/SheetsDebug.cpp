/*
   Copyright (c) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 
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
   Boston, MA 02110-1301, USA.
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

