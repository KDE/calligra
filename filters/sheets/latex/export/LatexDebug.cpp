/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "LatexDebug.h"

const QLoggingCategory &LATEX_LOG()
{
    static const QLoggingCategory category("calligra.filter.kspread2tex");
    return category;
}
