/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STENCILBOX_DEBUG_H
#define STENCILBOX_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &STENCILBOX_LOG();

#define debugStencilBox qCDebug(STENCILBOX_LOG)
#define warnStencilBox qCWarning(STENCILBOX_LOG)
#define errorStencilBox qCCritical(STENCILBOX_LOG)

#endif
