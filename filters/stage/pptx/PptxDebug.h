/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#ifndef PPTX_DEBUG_H
#define PPTX_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &PPTX_LOG();

#define debugPptx qCDebug(PPTX_LOG)
#define warnPptx qCWarning(PPTX_LOG)
#define errorPptx qCCritical(PPTX_LOG)

#endif
