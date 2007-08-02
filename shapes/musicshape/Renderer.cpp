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
#include "Renderer.h"
#include "MusicStyle.h"

#include "core/Sheet.h"
#include "core/Part.h"
#include "core/Voice.h"
#include "core/Staff.h"
#include "core/VoiceBar.h"
#include "core/Chord.h"
#include "core/Note.h"
#include "core/Clef.h"
#include "core/Bar.h"
#include "core/KeySignature.h"
#include "core/TimeSignature.h"

using namespace MusicCore;

MusicRenderer::MusicRenderer(MusicStyle* style) : m_style(style), m_debug(false)
{
}

void MusicRenderer::renderSheet(QPainter& painter, Sheet* sheet)
{
    for (int i = 0; i < sheet->partCount(); i++) {
        renderPart(painter, sheet->part(i));
    }
}

void MusicRenderer::renderPart(QPainter& painter, Part* part)
{
    for (int i = 0; i < part->staffCount(); i++) {
        renderStaff(painter, part->staff(i));
    }
    double firstStaff = part->staff(0)->top();
    int c = part->staffCount()-1;
    double lastStaff = part->staff(c)->bottom();
    for (int b = 0; b < part->sheet()->barCount(); b++) {
        Bar* bar = part->sheet()->bar(b);
        QPointF p = bar->position();
        painter.drawLine(QPointF(p.x() + bar->size(), p.y() + firstStaff), QPointF(p.x() + bar->size(), p.y() + lastStaff));
    }
    for (int i = 0; i < part->voiceCount(); i++) {
        renderVoice(painter, part->voice(i));
    }
}

void MusicRenderer::renderStaff(QPainter& painter, Staff *staff )
{
    double dy = staff->lineSpacing();
    double y = staff->top();
    for (int b = 0; b < staff->part()->sheet()->barCount(); b++) {
        Bar* bar = staff->part()->sheet()->bar(b);
        QPointF p = bar->position();
        painter.setPen(m_style->staffLinePen());
        for (int i = 0; i < staff->lineCount(); i++) {
            painter.drawLine(QPointF(p.x(), p.y() + y + i * dy), QPointF(p.x() + bar->size(), p.y() + y + i * dy));
        }
        RenderState state;
        for (int e = 0; e < bar->staffElementCount(staff); e++) {
            renderStaffElement(painter, bar->staffElement(staff, e), p, state, bar->scale());
        }
    }
}

void MusicRenderer::renderVoice(QPainter& painter, Voice *voice, const QColor& color)
{
    RenderState state;
    state.clef = 0;
    for (int b = 0; b < voice->part()->sheet()->barCount(); b++) {
        Bar* bar = voice->part()->sheet()->bar(b);
        QPointF p = bar->position();
        VoiceBar* vb = voice->bar(bar);
        for (int e = 0; e < vb->elementCount(); e++) {
            if (vb->element(e)->staff()) {
                state.clef = vb->element(e)->staff()->lastClefChange(b, 0);
            }
            renderElement(painter, vb->element(e), p, state, bar->scale(), color);
        }
    }
}

void MusicRenderer::renderElement(QPainter& painter, VoiceElement* me, const QPointF& pos, RenderState& state, double xScale, const QColor& color)
{
    double top = 0;
    if (me->staff()) top += me->staff()->top();
    if (m_debug) {
        painter.setPen(QPen(Qt::blue));
        painter.drawLine(pos + QPointF(me->x() * xScale, top + me->y() - 20), pos + QPointF(me->x() * xScale, top + me->y() + 20));
        painter.drawLine(pos + QPointF(me->x() * xScale + me->width(), top + me->y() - 20), pos + QPointF(me->x() * xScale + me->width(), top + me->y() + 20));
    }

    // TODO: make this less hacky
    Chord *c = dynamic_cast<Chord*>(me);
    if (c) renderChord(painter, c, pos, state, xScale, color);
}

void MusicRenderer::renderStaffElement(QPainter& painter, MusicCore::StaffElement* se, const QPointF& pos, RenderState& state, double xScale)
{
    double top = 0;
    top += se->staff()->top();
    if (m_debug) {
        painter.setPen(QPen(Qt::blue));
        painter.drawLine(pos + QPointF(se->x() * xScale, top + se->y() - 20), pos + QPointF(se->x() * xScale, top + se->y() + 20));
        painter.drawLine(pos + QPointF(se->x() * xScale + se->width(), top + se->y() - 20), pos + QPointF(se->x() * xScale + se->width(), top + se->y() + 20));
    }

    Clef *cl = dynamic_cast<Clef*>(se);
    if (cl) renderClef(painter, cl, pos, state, xScale);
    KeySignature *ks = dynamic_cast<KeySignature*>(se);
    if (ks) renderKeySignature(painter, ks, pos, state, xScale);
    TimeSignature* ts = dynamic_cast<TimeSignature*>(se);
    if (ts) renderTimeSignature(painter, ts, pos, xScale);
}


void MusicRenderer::renderClef(QPainter& painter, Clef *c, const QPointF& pos, RenderState& state, double xScale)
{
    state.clef = c;
    Staff* s = c->staff();
    m_style->renderClef(painter, pos.x() + c->x() * xScale, pos.y() + s->top() + (s->lineCount() - c->line()) * s->lineSpacing(), c->shape());
}

void MusicRenderer::renderKeySignature(QPainter& painter, KeySignature* ks, const QPointF& pos, RenderState& state, double xScale)
{
    Staff * s = ks->staff();
    double curx = pos.x() + ks->x() * xScale;
    // draw sharps
    int idx = 3;
    for (int i = 0; i < 7; i++) {
        if (ks->accidentals(idx) > 0) {
            int line = 14;
            if (state.clef && state.clef->shape() == Clef::FClef) line = 4;
            if (state.clef) {
                line -= 2*state.clef->line();
            } else {
                line -= 4;
            }
            line = line - idx;
            while (line < 0) line += 7;
            while (line >= 6) line -= 7;

            m_style->renderAccidental( painter, curx, pos.y() + s->top() + line * s->lineSpacing() / 2, 1 );

            curx += 6;
        }
        idx = (idx + 4) % 7;
    }

    // draw flats
    idx = 6;
    for (int i = 0; i < 7; i++) {
        if (ks->accidentals(idx) < 0) {
            int line = 14;
            if (state.clef && state.clef->shape() == Clef::FClef) line = 4;
            if (state.clef) {
                line -= 2*state.clef->line();
            } else {
                line -= 4;
            }
            line = line - idx;
            while (line < 0) line += 7;
            while (line >= 6) line -= 7;

            m_style->renderAccidental( painter, curx, pos.y() + s->top() + line * s->lineSpacing() / 2, -1 );

            curx += 6;
        }
        idx = (idx + 3) % 7;
    }
}

void MusicRenderer::renderTimeSignature(QPainter& painter, TimeSignature* ts, const QPointF& pos, double xScale)
{
    Staff* s = ts->staff();
    double hh = 0.5 * (s->lineCount() - 1) * s->lineSpacing();
    m_style->renderTimeSignatureNumber( painter, pos.x() + ts->x() * xScale, pos.y() + s->top() + hh, ts->width(), ts->beats());
    m_style->renderTimeSignatureNumber( painter, pos.x() + ts->x() * xScale, pos.y() + s->top() + 2*hh, ts->width(), ts->beat());
}

static double stemLength(Chord::Duration duration)
{
    switch (duration) {
        case Chord::Breve:
        case Chord::Whole:
            return 0;
        case Chord::Half:
        case Chord::Quarter:
        case Chord::Eighth:
            return 7;
        case Chord::Sixteenth:
            return 8;
        case Chord::ThirtySecond:
            return 9.5;
        case Chord::SixtyFourth:
            return 11;
        case Chord::HundredTwentyEighth:
            return 12.5;
    }
    return 0;
}

void MusicRenderer::renderRest(QPainter& painter, Chord::Duration duration, const QPointF& pos, const QColor& color)
{
    m_style->renderRest(painter, pos.x(), pos.y(), duration, color);
}

void MusicRenderer::renderChord(QPainter& painter, Chord* chord, const QPointF& ref, RenderState& state, double xScale, const QColor& color)
{
    double x = chord->x() * xScale;
    if (chord->noteCount() == 0) { // a rest
        Staff *s = chord->staff();
        renderRest(painter, chord->duration(), ref + QPointF(x, s->top() + (2 - (chord->duration() == Chord::Whole)) * s->lineSpacing()), color);
        return;
    }
    Note *n = chord->note(0);
    Staff * s = n->staff();
    int line = 14;
    if (state.clef && state.clef->shape() == Clef::FClef) line = 4;
    if (state.clef) {
        line -= 2*state.clef->line();
    } else {
        line -= 4;
    }
    line = line - n->pitch();
    if (line > 9) { // lines under the bar
        painter.setPen(m_style->staffLinePen(color));
        for (int i = 10; i <= line; i+= 2) {
            double y = s->top() + i * s->lineSpacing() / 2;
            painter.drawLine(ref + QPointF(x - 4, y), ref + QPointF(x + 10, y));
        }
    } else if (line < -1) { // lines above the bar
        painter.setPen(m_style->staffLinePen(color));
        for (int i = -2; i >= line; i-= 2) {
            double y = s->top() + i * s->lineSpacing() / 2;
            painter.drawLine(ref + QPointF(x - 4, y), ref + QPointF(x + 10, y));
        }
    }

    double stemLen = -stemLength(chord->duration()); // TODO: make this depend on the number of flags/length of the note
    double stemX = x + 6;
    bool stemsUp = true;
    if (line < 4) { stemLen = -stemLen; stemX = x; stemsUp = false; }
    if (stemLen != 0.0 && stemLen != -0.0) {
        painter.setPen(m_style->stemPen(color));
        painter.drawLine(ref + QPointF(stemX, chord->y() + s->top() + line * s->lineSpacing() / 2),
                     ref + QPointF(stemX, chord->y() + s->top() + (line + stemLen) * s->lineSpacing() / 2));
    }
    m_style->renderNoteHead( painter, ref.x() + x, ref.y() + chord->y() + s->top() + line * s->lineSpacing() / 2, chord->duration(), color );
    m_style->renderNoteFlags( painter, ref.x() + stemX, ref.y() + chord->y() + s->top() + (line + stemLen) * s->lineSpacing() / 2, chord->duration(), stemsUp, color );

    // render accidentals
    if (n->accidentals()) {
        m_style->renderAccidental( painter, ref.x() + x - 10, ref.y() + chord->y() + s->top() + line * s->lineSpacing() / 2, n->accidentals(), color );
    }

    // render dots of notes
    double dotX = x + 11;
    painter.setPen(m_style->noteDotPen());
    for (int i = 0; i < chord->dots(); i++) {
        painter.drawPoint(ref + QPointF(dotX, chord->y() + s->top() + line * s->lineSpacing() / 2));
        dotX += 3;
    }
}

void MusicRenderer::renderNote(QPainter& painter, MusicCore::Chord::Duration duration, const QPointF& pos, double stemLength, const QColor& color)
{
    m_style->renderNoteHead(painter, pos.x(), pos.y(), duration, color);

    if (duration <= MusicCore::Chord::Half) {
        painter.setPen(m_style->stemPen(color));
        painter.drawLine(pos + QPointF(6, -stemLength), pos + QPointF(6, 0));
    }
    if (duration <= MusicCore::Chord::Eighth) {
        m_style->renderNoteFlags(painter, pos.x()+6, pos.y() - stemLength, duration, true, color);
    }
}

