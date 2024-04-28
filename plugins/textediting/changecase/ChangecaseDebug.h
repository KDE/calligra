/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CHANGECASE_DEBUG_H
#define CHANGECASE_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &CHANGECASE_LOG();

#define debugChangecase qCDebug(CHANGECASE_LOG)
#define warnChangecase qCWarning(CHANGECASE_LOG)
#define errorChangecase qCCritical(CHANGECASE_LOG)

#endif
