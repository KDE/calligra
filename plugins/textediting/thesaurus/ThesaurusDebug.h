/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef THESAURUS_DEBUG_H
#define THESAURUS_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &THESAURUS_LOG();

#define debugThesaurus qCDebug(THESAURUS_LOG)
#define warnThesaurus qCWarning(THESAURUS_LOG)
#define errorThesaurus qCCritical(THESAURUS_LOG)

#endif
