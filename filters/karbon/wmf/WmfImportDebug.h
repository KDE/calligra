/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-only
 */

#ifndef WMFIMPORT_DEBUG_H
#define WMFIMPORT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &WMFIMPORT_LOG();

#define debugWmf qCDebug(WMFIMPORT_LOG)
#define warnWmf qCWarning(WMFIMPORT_LOG)
#define errorWmf qCCritical(WMFIMPORT_LOG)

#endif
