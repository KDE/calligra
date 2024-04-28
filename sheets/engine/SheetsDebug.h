/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SHEETS_DEBUG_H
#define SHEETS_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

#include "sheets_engine_export.h"

extern const CALLIGRA_SHEETS_ENGINE_EXPORT QLoggingCategory &SHEETS_LOG(); // 36001

#define debugSheets qCDebug(SHEETS_LOG)
#define warnSheets qCWarning(SHEETS_LOG)
#define errorSheets qCCritical(SHEETS_LOG)

extern const CALLIGRA_SHEETS_ENGINE_EXPORT QLoggingCategory &SHEETSFORMULA_LOG(); // 36002

#define debugSheetsFormula qCDebug(SHEETSFORMULA_LOG)
#define warnSheetsFormula qCWarning(SHEETSFORMULA_LOG)
#define errorSheetsFormula qCCritical(SHEETSFORMULA_LOG)

extern const CALLIGRA_SHEETS_ENGINE_EXPORT QLoggingCategory &SHEETSODF_LOG(); // 36003

#define debugSheetsODF qCDebug(SHEETSODF_LOG)
#define warnSheetsODF qCWarning(SHEETSODF_LOG)
#define errorSheetsODF qCCritical(SHEETSODF_LOG)

extern const CALLIGRA_SHEETS_ENGINE_EXPORT QLoggingCategory &SHEETSRENDER_LOG(); // 36004

#define debugSheetsRender qCDebug(SHEETSRENDER_LOG)
#define warnSheetsRender qCWarning(SHEETSRENDER_LOG)
#define errorSheetsRender qCCritical(SHEETSRENDER_LOG)

extern const CALLIGRA_SHEETS_ENGINE_EXPORT QLoggingCategory &SHEETSUI_LOG(); // 36005

#define debugSheetsUI qCDebug(SHEETSUI_LOG)
#define warnSheetsUI qCWarning(SHEETSUI_LOG)
#define errorSheetsUI qCCritical(SHEETSUI_LOG)

extern const CALLIGRA_SHEETS_ENGINE_EXPORT QLoggingCategory &SHEETSSTYLE_LOG(); // 36006

#define debugSheetsStyle qCDebug(SHEETSSTYLE_LOG)
#define warnSheetsStyle qCWarning(SHEETSSTYLE_LOG)
#define errorSheetsStyle qCCritical(SHEETSSTYLE_LOG)

extern const CALLIGRA_SHEETS_ENGINE_EXPORT QLoggingCategory &SHEETSDAMAGE_LOG(); // 36007

#define debugSheetsDamage qCDebug(SHEETSDAMAGE_LOG)
#define warnSheetsDamage qCWarning(SHEETSDAMAGE_LOG)
#define errorSheetsDamage qCCritical(SHEETSDAMAGE_LOG)

extern const CALLIGRA_SHEETS_ENGINE_EXPORT QLoggingCategory &SHEETSTABLESHAPE_LOG();

#define debugSheetsTableShape qCDebug(SHEETSTABLESHAPE_LOG)
#define warnSheetsTableShape qCWarning(SHEETSTABLESHAPE_LOG)
#define errorSheetsTableShape qCCritical(SHEETSTABLESHAPE_LOG)

#endif
