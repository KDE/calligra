/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef AUTOCORRECTION_DEBUG_H_
#define AUTOCORRECTION_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>
#include <autocorrection_export.h>

extern const AUTOCORRECTION_EXPORT QLoggingCategory &AUTOCORRECTION_LOG();

#define debugAutoCorrection qCDebug(AUTOCORRECTION_LOG) << Q_FUNC_INFO
#define warnAutoCorrection qCWarning(AUTOCORRECTION_LOG) << Q_FUNC_INFO
#define errorAutoCorrection qCCritical(AUTOCORRECTION_LOG) << Q_FUNC_INFO

#endif
