/*
 *  Copyright (c) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
