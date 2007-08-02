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
#ifndef MUSIC_RENDERER_H
#define MUSIC_RENDERER_H

#include "core/Chord.h"
#include <QColor>
#include <QPointF>

class MusicStyle;
class QPainter;

namespace MusicCore {
    class Sheet;
    class Part;
    class Staff;
    class Voice;
    class VoiceElement;
    class StaffElement;
    class Clef;
    class KeySignature;
    class TimeSignature;
    class Chord;
}

class MusicRenderer {
public:
    struct RenderState {
        MusicCore::Clef* clef;
    };

    MusicRenderer(MusicStyle* style);

    void renderSheet(QPainter& painter, MusicCore::Sheet* sheet);
    void renderPart(QPainter& painter, MusicCore::Part* part);
    void renderStaff(QPainter& painter, MusicCore::Staff* staff);
    void renderVoice(QPainter& painter, MusicCore::Voice* voice, const QColor& color = Qt::black);
    void renderElement(QPainter& painter, MusicCore::VoiceElement* element, QPointF pos, RenderState& state, double xScale, const QColor& color = Qt::black);
    void renderStaffElement(QPainter& painter, MusicCore::StaffElement* element, QPointF pos, RenderState& state, double xScale);

    void renderClef(QPainter& painter, MusicCore::Clef* clef, QPointF pos, RenderState& state, double xScale);
    void renderKeySignature(QPainter& painter, MusicCore::KeySignature* keySignature, QPointF pos, RenderState& state, double xScale);
    void renderTimeSignature(QPainter& painter, MusicCore::TimeSignature* timeSignature, QPointF pos, double xScale);
    void renderChord(QPainter& painter, MusicCore::Chord* chord, QPointF ref, RenderState& state, double xScale, const QColor& color = Qt::black);
    void renderRest(QPainter& painter, MusicCore::Chord::Duration duration, QPointF pos, const QColor& color = Qt::black);
    void renderNote(QPainter& painter, MusicCore::Chord::Duration duration, QPointF pos, double stemLength, const QColor& color = Qt::black);
private:
    MusicStyle* m_style;
    bool m_debug;
};

#endif // MUSIC_RENDERER_H
