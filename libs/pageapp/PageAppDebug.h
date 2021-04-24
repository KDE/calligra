/*
 *  SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef PAGEAPP_DEBUG_H_
#define PAGEAPP_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>

#include <kopageapp_export.h>

extern const KOPAGEAPP_EXPORT QLoggingCategory &PAGEAPP_LOG();

#define debugPageApp qCDebug(PAGEAPP_LOG)
#define warnPageApp qCWarning(PAGEAPP_LOG)
#define errorPageApp qCCritical(PAGEAPP_LOG)

#endif
