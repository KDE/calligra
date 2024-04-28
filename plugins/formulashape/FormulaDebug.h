/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
