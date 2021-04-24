/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef ODF_DEBUG_H_
#define ODF_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>
#include <koodf_export.h>

extern const KOODF_EXPORT QLoggingCategory &ODF_LOG();

#define debugOdf qCDebug(ODF_LOG)
#define warnOdf qCWarning(ODF_LOG)
#define errorOdf qCCritical(ODF_LOG)

#endif
