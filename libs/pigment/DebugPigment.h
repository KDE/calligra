/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _DEBUG_PIGMENT_H_
#define _DEBUG_PIGMENT_H_

#include <QDebug>
#include <QLoggingCategory>
#include <pigment_export.h>

extern const PIGMENTCMS_EXPORT QLoggingCategory &PIGMENT_log();

#define dbgPigment qCDebug(PIGMENT_log)
#define dbgPigmentCCS dbgPigment
#define dbgPigmentCSRegistry dbgPigment
#define dbgPigmentCS dbgPigment

#define warnPigment qCWarning(PIGMENT_log)
#define errorPigment qCCritical(PIGMENT_log)

#endif
