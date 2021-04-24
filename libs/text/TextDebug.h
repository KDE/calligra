/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TEXT_DEBUG_H_
#define TEXT_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>
#include <kotext_export.h>

extern const KOTEXT_EXPORT QLoggingCategory &TEXT_LOG();

#define debugText qCDebug(TEXT_LOG)
#define warnText qCWarning(TEXT_LOG)
#define errorText qCCritical(TEXT_LOG)

#endif
