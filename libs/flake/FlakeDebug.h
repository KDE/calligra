/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef FLAKE_DEBUG_H_
#define FLAKE_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>
#include <flake_export.h>

extern const FLAKE_EXPORT QLoggingCategory &FLAKE_LOG();

#define debugFlake qCDebug(FLAKE_LOG)
#define warnFlake qCWarning(FLAKE_LOG)
#define errorFlake qCCritical(FLAKE_LOG)

#endif
