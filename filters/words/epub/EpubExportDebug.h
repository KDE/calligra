/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef EPUBEXPORT_DEBUG_H
#define EPUBEXPORT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &EPUBEXPORT_LOG();

#define debugEpub qCDebug(EPUBEXPORT_LOG)
#define warnEpub qCWarning(EPUBEXPORT_LOG)
#define errorEpub qCCritical(EPUBEXPORT_LOG)

#endif
