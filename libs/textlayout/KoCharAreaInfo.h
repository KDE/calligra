/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
    KoCharAreaInfo() = default;

    KoCharAreaInfo(const QRectF &_rect, const QChar &_character)
        : rect(_rect)
        , character(_character)
    {
    }

    /// Area from the point of view of text selection markup.
    QRectF rect;

    /// The character rendered.
    QChar character;
};

#endif // KOCHARAREAINFO_H
