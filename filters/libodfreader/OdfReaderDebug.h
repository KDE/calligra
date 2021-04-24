/*
 *  SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef ODFREADER_DEBUG_H_
#define ODFREADER_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>

#include "koodfreader_export.h"

extern const KOODFREADER_EXPORT QLoggingCategory &ODFREADER_LOG();

#define debugOdfReader qCDebug(ODFREADER_LOG)
#define warnOdfReader qCWarning(ODFREADER_LOG)
#define errorOdfReader qCCritical(ODFREADER_LOG)

#endif
