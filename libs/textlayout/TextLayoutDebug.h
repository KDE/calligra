/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TEXT_LAYOUT_DEBUG_H_
#define TEXT_LAYOUT_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>
#include <kotextlayout_export.h>

extern const KOTEXTLAYOUT_EXPORT QLoggingCategory &TEXT_LAYOUT_LOG();

#define debugTextLayout qCDebug(TEXT_LAYOUT_LOG)
#define warnTextLayout qCWarning(TEXT_LAYOUT_LOG)
#define errorTextLayout qCCritical(TEXT_LAYOUT_LOG)

#endif
