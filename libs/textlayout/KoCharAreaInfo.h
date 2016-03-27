/* This file is part of the KDE project

   Copyright (C) 2016 Friedrich W. H. Kossebau <kossebau@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KOCHARAREAINFO_H
#define KOCHARAREAINFO_H

#include "kotextlayout_export.h"

#include <QChar>
#include <QRectF>

/**
 * KoCharAreaInfo describes a rendered character and the area it covers,
 * from the point of view of text selection markup.
 * Currently only used for collecting data as needed for the plugins for Okular.
 */
class KOTEXTLAYOUT_EXPORT KoCharAreaInfo
{
public:
    KoCharAreaInfo()
    {}

    KoCharAreaInfo(const QRectF& _rect, const QChar& _character)
    : rect(_rect)
    , character(_character)
    {}

    /// Area from the point of view of text selection markup.
    QRectF rect;

    /// The character rendered.
    QChar character;
};

#endif // KOCHARAREAINFO_H
