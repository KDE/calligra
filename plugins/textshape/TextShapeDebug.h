/*
   SPDX-FileCopyrightText: 2017 Dag Andersen <danders@get2net.dk>*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEXTSHAPE_DEBUG_H
#define TEXTSHAPE_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &TEXTSHAPE_LOG();

#define debugTextShape qCDebug(TEXTSHAPE_LOG) << Q_FUNC_INFO
#define warnTextShape qCWarning(TEXTSHAPE_LOG)
#define errorTextShape qCCritical(TEXTSHAPE_LOG)

#endif
