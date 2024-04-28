/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#ifndef DOCX_DEBUG_H
#define DOCX_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &DOCX_LOG();

#define debugDocx qCDebug(DOCX_LOG)
#define warnDocx qCWarning(DOCX_LOG)
#define errorDocx qCCritical(DOCX_LOG)

#endif
