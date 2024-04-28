/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PDFIMPORT_DEBUG_H
#define PDFIMPORT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &PDFIMPORT_LOG();

#define debugPdf qCDebug(PDFIMPORT_LOG)
#define warnPdf qCWarning(PDFIMPORT_LOG)
#define errorPdf qCCritical(PDFIMPORT_LOG)

#endif
