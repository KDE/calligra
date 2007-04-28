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
#ifndef MUSIC_STYLE_H
#define MUSIC_STYLE_H

#include "core/Chord.h"
#include <QtGui/QPen>
#include <QtGui/QPainter>

/**
 * This class contains various methods that define how music is rendered. Currently all hardcoded
 * implementations, but in the future this class would become pure virtual, with verious implementations.
 */
class MusicStyle {
public:
    MusicStyle();
    virtual ~MusicStyle();
    virtual QPen staffLinePen();
    virtual QPen stemPen();
    virtual void renderNoteHead(QPainter& painter, double x, double y, MusicCore::Chord::Duration duration);
private:
    QPen m_staffLinePen, m_stemPen;
    QFont m_font;
};

#endif // MUSIC_STYLE_H
