/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PPT_DEBUG_H
#define PPT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &PPT_LOG();

#define debugPpt qCDebug(PPT_LOG)
#define warnPpt qCWarning(PPT_LOG)
#define errorPpt qCCritical(PPT_LOG)

#endif
