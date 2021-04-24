/*
 *  SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef MAIN_DEBUG_H_
#define MAIN_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>
#include <komain_export.h>

extern const KOMAIN_EXPORT QLoggingCategory &MAIN_LOG();

#define debugMain qCDebug(MAIN_LOG)
#define warnMain qCWarning(MAIN_LOG)
#define errorMain qCCritical(MAIN_LOG)

extern const KOMAIN_EXPORT QLoggingCategory &FILTER_LOG();

#define debugFilter qCDebug(FILTER_LOG)
#define warnFilter qCWarning(FILTER_LOG)
#define errorFilter qCCritical(FILTER_LOG)

#endif
