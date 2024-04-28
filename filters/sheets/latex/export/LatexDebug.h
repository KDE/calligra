/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LATEX_DEBUG_H
#define LATEX_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &LATEX_LOG();

#define debugLatex qCDebug(LATEX_LOG)
#define warnLatex qCWarning(LATEX_LOG)
#define errorLatex qCCritical(LATEX_LOG)

#endif
