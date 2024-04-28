/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef HTMLIMPORT_DEBUG_H
#define HTMLIMPORT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &HTMLIMPORT_LOG();

#define debugHtml qCDebug(HTMLIMPORT_LOG)
#define warnHtml qCWarning(HTMLIMPORT_LOG)
#define errorHtml qCCritical(HTMLIMPORT_LOG)

#endif
