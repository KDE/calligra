/*
 *  SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef ODF2_DEBUG_H_
#define ODF2_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>

#include "koodf2_export.h"

extern const KOODF2_EXPORT QLoggingCategory &ODF2_LOG();

#define debugOdf2 qCDebug(ODF2_LOG)
#define warnOdf2 qCWarning(ODF2_LOG)
#define errorOdf2 qCCritical(ODF2_LOG)

#endif
