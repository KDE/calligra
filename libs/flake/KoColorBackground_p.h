/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCOLORBACKGROUND_P_H
#define KOCOLORBACKGROUND_P_H

#include "KoShapeBackground_p.h"

#include <QColor>

class KoColorBackgroundPrivate : public KoShapeBackgroundPrivate
{
public:
    KoColorBackgroundPrivate()
        : color(Qt::black)
        , style(Qt::SolidPattern){};

    QColor color;
    Qt::BrushStyle style;
};

#endif // KOCOLORBACKGROUND_P_H
