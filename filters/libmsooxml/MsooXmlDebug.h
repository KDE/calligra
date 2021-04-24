/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#ifndef MSOOXML_DEBUG_H
#define MSOOXML_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

#include "komsooxml_export.h"

extern const KOMSOOXML_EXPORT QLoggingCategory &MSOOXML_LOG();

#define debugMsooXml qCDebug(MSOOXML_LOG)
#define warnMsooXml qCWarning(MSOOXML_LOG)
#define errorMsooXml qCCritical(MSOOXML_LOG)

#endif
