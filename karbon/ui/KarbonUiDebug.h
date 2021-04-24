/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KARBONUI_DEBUG_H
#define KARBONUI_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

#include <karbonui_export.h>

extern const KARBONUI_EXPORT QLoggingCategory &KARBONUI_LOG();

#define debugKarbonUi qCDebug(KARBONUI_LOG)
#define warnKarbonUi qCWarning(KARBONUI_LOG)
#define errorKarbonUi qCCritical(KARBONUI_LOG)

#endif
