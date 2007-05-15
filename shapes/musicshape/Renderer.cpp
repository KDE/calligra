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
    double lastStaff = part->staff(c)->top() + part->staff(c)->lineSpacing() * (part->staff(c)->lineCount()-1);
    double x = 0;
    for (int b = 0; b < part->sheet()->barCount(); b++) {
        x += part->sheet()->bar(b)->size();
        painter.drawLine(QPointF(x, firstStaff), QPointF(x, lastStaff));
    }
    for (int i = 0; i < part->voiceCount(); i++) {
        renderVoice(painter, part->voice(i));
    }
}

void MusicRenderer::renderStaff(QPainter& painter, Staff *staff )
{
    double dy = staff->lineSpacing();
    double y = staff->top();
    painter.setPen(m_style->staffLinePen());
    for (int i = 0; i < staff->lineCount(); i++) {
        painter.drawLine(QPointF(0.0, y + i * dy), QPointF(1000.0, y + i * dy));
    }
}

void MusicRenderer::renderVoice(QPainter& painter, Voice *voice)
{
    RenderState state;
    state.clef = 0;
    double x = 0;
    for (int b = 0; b < voice->part()->sheet()->barCount(); b++) {
        VoiceBar* vb = voice->bar(voice->part()->sheet()->bar(b));
        for (int e = 0; e < vb->elementCount(); e++) {
            renderElement(painter, vb->element(e), x, 0, state);
        }
        x += voice->part()->sheet()->bar(b)->size();
    }
}

void MusicRenderer::renderElement(QPainter& painter, MusicElement* me, double x, double y, RenderState& state)
{
    double top = y;
    if (me->staff()) top += me->staff()->top();
    if (m_debug) {
        painter.setPen(QPen(Qt::blue));
        painter.drawLine(QPointF(x + me->x(), top + me->y() - 20), QPointF(x + me->x(), top + me->y() + 20));
        painter.drawLine(QPointF(x + me->x() + me->width(), top + me->y() - 20), QPointF(x + me->x() + me->width(), top + me->y() + 20));
    }

    // TODO: make this less hacky
    Chord *c = dynamic_cast<Chord*>(me);
    if (c) renderChord(painter, c, x, state);
    Clef *cl = dynamic_cast<Clef*>(me);
    if (cl) renderClef(painter, cl, x, state);
    KeySignature *ks = dynamic_cast<KeySignature*>(me);
    if (ks) renderKeySignature(painter, ks, x, state);
    TimeSignature* ts = dynamic_cast<TimeSignature*>(me);
    if (ts) renderTimeSignature( painter, ts, x );
}

void MusicRenderer::renderClef(QPainter& painter, Clef *c, double x, RenderState& state)
{
    Staff* s = c->staff();
    m_style->renderClef(painter, x + c->x(), s->top() + (s->lineCount() - c->line()) * s->lineSpacing(), c->shape());
    state.clef = c;
}

void MusicRenderer::renderKeySignature(QPainter& painter, KeySignature* ks, double x, RenderState& state)
{
    Staff * s = ks->staff();
    double curx = x + ks->x();
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

            m_style->renderAccidental( painter, curx, s->top() + line * s->lineSpacing() / 2, 1 );

            curx += 10;
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

            m_style->renderAccidental( painter, curx, s->top() + line * s->lineSpacing() / 2, -1 );

            curx += 10;
        }
        idx = (idx + 3) % 7;
    }
}

void MusicRenderer::renderTimeSignature(QPainter& painter, TimeSignature* ts, double x)
{
    Staff* s = ts->staff();
    double hh = 0.5 * (s->lineCount() - 1) * s->lineSpacing();
    m_style->renderTimeSignatureNumber( painter, x + ts->x(), s->top() + hh, ts->width(), ts->beats());
    m_style->renderTimeSignatureNumber( painter, x + ts->x(), s->top() + 2*hh, ts->width(), ts->beat());
}

void MusicRenderer::renderChord(QPainter& painter, Chord* chord, double x, RenderState& state)
{
    x = x + chord->x();
    if (chord->noteCount() == 0) { // a rest
        Staff *s = chord->staff();
        m_style->renderRest( painter, x, s->top() + (2 - (chord->duration() == Chord::Whole)) * s->lineSpacing(), chord->duration() );
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
        painter.setPen(m_style->staffLinePen());
        for (int i = 10; i <= line; i+= 2) {
            double y = s->top() + i * s->lineSpacing() / 2;
            painter.drawLine(QPointF(x - 4, y), QPointF(x + 15, y));
        }
    } else if (line < -1) { // lines above the bar
        painter.setPen(m_style->staffLinePen());
        for (int i = -2; i >= line; i-= 2) {
            double y = s->top() + i * s->lineSpacing() / 2;
            painter.drawLine(QPointF(x - 4, y), QPointF(x + 15, y));
        }
    }

    double stemLen = -7;
    double stemX = x + 11;
    if (line < 4) { stemLen = 7; stemX = x; }
    painter.setPen(m_style->stemPen());
    painter.drawLine(QPointF(stemX, chord->y() + s->top() + line * s->lineSpacing() / 2),
                     QPointF(stemX, chord->y() + s->top() + (line + stemLen) * s->lineSpacing() / 2));
    m_style->renderNoteHead( painter, x, chord->y() + s->top() + line * s->lineSpacing() / 2, chord->duration() );
}

void MusicRenderer::renderNote(QPainter& painter, MusicCore::Chord::Duration duration, double x, double y, double stemLength, QColor color)
{
    m_style->renderNoteHead(painter, x, y, duration, color);

    if (duration <= MusicCore::Chord::Half) {
        painter.setPen(m_style->stemPen(color));
        painter.drawLine(QPointF(x+11, y - stemLength), QPointF(x+11, y));
    }
    if (duration <= MusicCore::Chord::Eighth) {
        m_style->renderNoteFlags(painter, x+11, y - stemLength, duration, color);
    }
}

