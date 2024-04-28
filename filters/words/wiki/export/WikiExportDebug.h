/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef WIKIEXPORT_DEBUG_H
#define WIKIEXPORT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &WIKIEXPORT_LOG();

#define debugWiki qCDebug(WIKIEXPORT_LOG)
#define warnWiki qCWarning(WIKIEXPORT_LOG)
#define errorWiki qCCritical(WIKIEXPORT_LOG)

#endif
