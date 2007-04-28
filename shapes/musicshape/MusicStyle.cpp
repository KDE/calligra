/* This file is part of the KDE project
 * Copyright (C) 2007 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "MusicStyle.h"
using namespace MusicCore;

MusicStyle::MusicStyle() : m_font("Emmentaler")
{
    m_font.setPixelSize(35);
    m_staffLinePen.setWidthF(0.8);
    m_staffLinePen.setCapStyle(Qt::RoundCap);
    m_staffLinePen.setColor(Qt::black);
    m_stemPen.setWidthF(1.2);
    m_stemPen.setCapStyle(Qt::RoundCap);
    m_stemPen.setColor(Qt::black);
}

MusicStyle::~MusicStyle()
{
}

QPen MusicStyle::staffLinePen()
{
    return m_staffLinePen;
}

QPen MusicStyle::stemPen()
{
    return m_stemPen;
}

void MusicStyle::renderNoteHead(QPainter& painter, double x, double y, Chord::Duration duration)
{
    painter.setPen(QPen(Qt::SolidLine));
    painter.setFont(m_font);
    QPointF p(x, y);
    switch (duration) {
        case Chord::HundredTwentyEighth:
        case Chord::SixtyFourth:
        case Chord::ThirtySecond:
        case Chord::Sixteenth:
        case Chord::Eighth:
        case Chord::Quarter:
            painter.drawText(p, QString(0xE125));
            break;
        case Chord::Half:
            painter.drawText(p, QString(0xE124));
            break;
        case Chord::Whole:
            painter.drawText(p, QString(0xE123));
            break;
        case Chord::Breve:
            painter.drawText(p, QString(0xE122));
            break;
    }
}
