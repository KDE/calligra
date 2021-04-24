/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef WIDGETS_DEBUG_H_
#define WIDGETS_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>
#include <kowidgets_export.h>

extern const KOWIDGETS_EXPORT QLoggingCategory &WIDGETS_LOG();

#define debugWidgets qCDebug(WIDGETS_LOG)
#define warnWidgets qCWarning(WIDGETS_LOG)
#define errorWidgets qCCritical(WIDGETS_LOG)

#endif
