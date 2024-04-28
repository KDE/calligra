/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef HTMLEXPORT_DEBUG_H
#define HTMLEXPORT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &HTMLEXPORT_LOG();

#define debugHtml qCDebug(HTMLEXPORT_LOG)
#define warnHtml qCWarning(HTMLEXPORT_LOG)
#define errorHtml qCCritical(HTMLEXPORT_LOG)

#endif
