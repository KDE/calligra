/***************************************************************************
 * KoKrossDebug.h
 * This file is part of the KDE project
 * Copyright (c) 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KOKROSS_DEBUG_H_
#define KOKROSS_DEBUG_H_

#include <QDebug>
#include <QLoggingCategory>

#include <kokross_export.h>

extern const KOKROSS_EXPORT QLoggingCategory &KOKROSS_LOG();

#define debugKoKross qCDebug(KOKROSS_LOG)
#define warnKoKross qCWarning(KOKROSS_LOG)
#define errorKoKross qCCritical(KOKROSS_LOG)

#endif
