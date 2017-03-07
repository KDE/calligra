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

#ifndef FORMULA_DEBUG_H
#define FORMULA_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

#include "koformula_export.h"


extern const KOFORMULA_EXPORT QLoggingCategory &FORMULA_LOG();

#define debugFormula qCDebug(FORMULA_LOG)
#define warnFormula qCWarning(FORMULA_LOG)
#define errorFormula qCCritical(FORMULA_LOG)

extern const QLoggingCategory &FORMULAELEMENT_LOG();

#define debugFormulaElement qCDebug(FORMULAELEMENT_LOG)
#define warnFormulaElement qCWarning(FORMULAELEMENT_LOG)
#define errorFormulaElement qCCritical(FORMULAELEMENT_LOG)

#endif
