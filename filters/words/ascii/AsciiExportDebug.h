/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ASCIIEXPORT_DEBUG_H
#define ASCIIEXPORT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &ASCIIEXPORT_LOG();

#define debugAsciiExport qCDebug(ASCIIEXPORT_LOG)
#define warnAsciiExport qCWarning(ASCIIEXPORT_LOG)
#define errorAsciiExport qCCritical(ASCIIEXPORT_LOG)

#endif
