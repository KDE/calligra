// This file is part of KSpread
// SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef SCRIPTINGDEBUG_H
#define SCRIPTINGDEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &SHEETSSCRIPTING_LOG();

#define debugSheetsScripting qCDebug(SHEETSSCRIPTING_LOG)
#define warnSheetsScripting qCWarning(SHEETSSCRIPTING_LOG)
#define errorSheetsScripting qCCritical(SHEETSSCRIPTING_LOG)

#endif
