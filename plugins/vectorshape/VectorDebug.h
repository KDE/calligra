/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VECTOR_DEBUG_H
#define VECTOR_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &VECTOR_LOG();

#define debugVector qCDebug(VECTOR_LOG)
#define warnVector qCWarning(VECTOR_LOG)
#define errorVector qCCritical(VECTOR_LOG)

#endif
