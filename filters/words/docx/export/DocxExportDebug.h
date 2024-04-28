/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DOCXEXPORT_DEBUG_H
#define DOCXEXPORT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &DOCXEXPORT_LOG();

#define debugDocx qCDebug(DOCXEXPORT_LOG)
#define warnDocx qCWarning(DOCXEXPORT_LOG)
#define errorDocx qCCritical(DOCXEXPORT_LOG)

#endif
