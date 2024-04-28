/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VARIABLES_DEBUG_H
#define VARIABLES_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &VARIABLES_LOG();

#define debugVariables qCDebug(VARIABLES_LOG)
#define warnVariables qCWarning(VARIABLES_LOG)
#define errorVariables qCCritical(VARIABLES_LOG)

#endif
