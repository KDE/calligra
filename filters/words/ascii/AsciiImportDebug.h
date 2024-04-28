/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ASCIIIMPORT_DEBUG_H
#define ASCIIIMPORT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &ASCIIIMPORT_LOG();

#define debugAsciiImport qCDebug(ASCIIIMPORT_LOG)
#define warnAsciiImport qCWarning(ASCIIIMPORT_LOG)
#define errorAsciiImport qCCritical(ASCIIIMPORT_LOG)

#endif
