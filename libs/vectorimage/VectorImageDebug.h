/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef VECTOR_IMAGE_DEBUG_H_
#define VECTOR_IMAGE_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>
#include <kovectorimage_export.h>

extern const KOVECTORIMAGE_EXPORT QLoggingCategory &VECTOR_IMAGE_LOG();

#define debugVectorImage qCDebug(VECTOR_IMAGE_LOG)
#define warnVectorImage qCWarning(VECTOR_IMAGE_LOG)
#define errorVectorImage qCCritical(VECTOR_IMAGE_LOG)

#endif
