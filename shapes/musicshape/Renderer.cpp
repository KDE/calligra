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
#include "core/StaffSystem.h"

using namespace MusicCore;

MusicRenderer::MusicRenderer(MusicStyle* style) : m_style(style), m_debug(false)
{
}

void MusicRenderer::renderSheet(QPainter& painter, Sheet* sheet)
{
    for (int i = 0; i < sheet->partCount(); i++) {
        renderPart(painter, sheet->part(i));
    }
    for (int i = 0; i < sheet->staffSystemCount(); i++) {
        StaffSystem* ss = sheet->staffSystem(i);
        if (ss->indent() == 0) continue;
        int b = ss->firstBar();
        Bar* bar = sheet->bar(b);
        double by = bar->position().y();
        double ind = ss->indent();

        for (int p = 0; p < sheet->partCount(); p++) {
            Part* part = sheet->part(p);
            for (int s = 0; s < part->staffCount(); s++) {
                Staff* staff = part->staff(s);
                double y = staff->top();
                double dy = staff->lineSpacing();

                painter.setPen(m_style->staffLinePen());
                for (int l = 0; l < staff->lineCount(); l++) {
                    painter.drawLine(QPointF(0, by + y + l * dy), QPointF(ind, by + y + l * dy));
                }

                Clef* clef = ss->clef(staff);
                RenderState foo;
                double x = 15;
                if (clef) {
                    renderClef(painter, clef, QPointF(x, by), foo, 0);
                    x += clef->width() + 15;
                }
                KeySignature* ks = staff->lastKeySignatureChange(b);
                if (ks) {
                    renderKeySignature(painter, ks, QPointF(x, by), foo, 0);
                }
            }
        }
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
        if (m_debug) {
            painter.setPen(QPen(Qt::green));
            painter.drawLine(QPointF(p.x(), p.y() + firstStaff - 3), QPointF(p.x(), p.y() + lastStaff + 3));
            painter.drawLine(QPointF(p.x() - bar->prefix(), p.y() + firstStaff - 3), QPointF(p.x() - bar->prefix(), p.y() + lastStaff + 3));
        }
        
        // check if the bar contains any elements, if not render a rest
        bool hasContents = false;
        for (int v = 0; v < part->voiceCount(); v++) {
            if (part->voice(v)->bar(bar)->elementCount() > 0) {
                hasContents = true;
                break;
            }
        }
        
        if (!hasContents) {
            QPointF pos = bar->position();
            double w = bar->size();
            for (int sid = 0; sid < part->staffCount(); sid++) {
                Staff* s = part->staff(sid);
                renderRest(painter, Chord::Whole, pos + QPointF(w/2, s->top() + s->lineSpacing()), Qt::black);
            }
        }
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
        QPointF prep = bar->prefixPosition() + QPointF(bar->prefix(), 0);
        painter.setPen(m_style->staffLinePen());
        for (int i = 0; i < staff->lineCount(); i++) {
            painter.drawLine(QPointF(p.x(), p.y() + y + i * dy), QPointF(p.x() + bar->size(), p.y() + y + i * dy));
        }
        if (bar->prefix() > 0) {
            QPointF q = bar->prefixPosition();
            for (int i = 0; i < staff->lineCount(); i++) {
                painter.drawLine(QPointF(q.x(), q.y() + y + i * dy), QPointF(q.x() + bar->prefix(), q.y() + y + i * dy));
            }
        }
        RenderState state;
        for (int e = 0; e < bar->staffElementCount(staff); e++) {
            StaffElement* se = bar->staffElement(staff, e);
            if (se->startTime() == 0) {
                renderStaffElement(painter, bar->staffElement(staff, e), prep, state, 1);
            } else {
                renderStaffElement(painter, bar->staffElement(staff, e), p, state, bar->scale());
            }
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
        painter.drawLine(pos + QPointF(me->x() * xScale, top + me->y() - 10), pos + QPointF(me->x() * xScale, top + me->y() + me->height() + 10));
        painter.drawLine(pos + QPointF(me->x() * xScale + me->width(), top + me->y() - 10), pos + QPointF(me->x() * xScale + me->width(), top + me->y() + me->height() + 10));

        painter.drawLine(pos + QPointF(me->x() * xScale - 10, top + me->y()), pos + QPointF(me->x() * xScale + me->width() + 10, top + me->y()));
        painter.drawLine(pos + QPointF(me->x() * xScale - 10, top + me->y() + me->height()), pos + QPointF(me->x() * xScale + me->width() + 10, top + me->y() + me->height()));
    }

    // TODO: make this less hacky
    Chord *c = dynamic_cast<Chord*>(me);
    if (c) renderChord(painter, c, pos, xScale, color);
}

void MusicRenderer::renderStaffElement(QPainter& painter, MusicCore::StaffElement* se, const QPointF& pos, RenderState& state, double xScale)
{
    double top = 0;
    top += se->staff()->top();
    if (m_debug) {
        painter.setPen(QPen(Qt::blue));
        painter.drawLine(pos + QPointF(se->x() * xScale, top + se->y() - 20), pos + QPointF(se->x() * xScale, top + se->y() + 20));
        painter.drawLine(pos + QPointF(se->x() * xScale + se->width(), top + se->y() - 20), pos + QPointF(se->x() * xScale + se->width(), top + se->y() + 20));

        painter.drawLine(pos + QPointF(se->x() * xScale - 10, top + se->y()), pos + QPointF(se->x() * xScale + se->width() + 10, top + se->y()));
        painter.drawLine(pos + QPointF(se->x() * xScale - 10, top + se->y() + se->height()), pos + QPointF(se->x() * xScale + se->width() + 10, top + se->y() + se->height()));
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
    // draw naturals for sharps
    int idx = 3;
    for (int i = 0; i < 7; i++) {
        if (ks->cancel(idx) > 0) {
            int line = 10;
            if (state.clef) line = state.clef->pitchToLine(idx);
            
            while (line < 0) line += 7;
            while (line >= 6) line -= 7;
            m_style->renderAccidental( painter, curx, pos.y() + s->top() + line * s->lineSpacing() / 2, 0 );
            
            curx += 6;
        }
        idx = (idx + 4) % 7;
    }
    
    // draw naturals for flats
    idx = 6;
    for (int i = 0; i < 7; i++) {
        if (ks->cancel(idx) < 0) {
            int line = 10;
            if (state.clef) line = state.clef->pitchToLine(idx);
            
            while (line < 0) line += 7;
            while (line >= 6) line -= 7;
            
            m_style->renderAccidental( painter, curx, pos.y() + s->top() + line * s->lineSpacing() / 2, 0 );
            
            curx += 6;
        }
        idx = (idx + 3) % 7;
    }
    
    // draw sharps
    idx = 3;
    for (int i = 0; i < 7; i++) {
        if (ks->accidentals(idx) > 0) {
            int line = 10;
            if (state.clef) line = state.clef->pitchToLine(idx);
            
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
            int line = 10;
            if (state.clef) line = state.clef->pitchToLine(idx);
            
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

void MusicRenderer::renderRest(QPainter& painter, Chord::Duration duration, const QPointF& pos, const QColor& color)
{
    m_style->renderRest(painter, pos.x(), pos.y(), duration, color);
}

void MusicRenderer::renderChord(QPainter& painter, Chord* chord, const QPointF& ref, double xScale, const QColor& color)
{
    double x = chord->x() * xScale;
    if (chord->noteCount() == 0) { // a rest
        Staff *s = chord->staff();
        renderRest(painter, chord->duration(), ref + QPointF(x, s->top() + (2 - (chord->duration() == Chord::Whole)) * s->lineSpacing()), color);
        return;
    }
    int topLine, bottomLine;
    VoiceBar* vb = chord->voiceBar();
    Bar* bar = vb->bar();
    int barIdx = bar->sheet()->indexOfBar(bar);
    double topy = 1e9, bottomy = -1e9;
    Staff* topStaff, *bottomStaff;
    
    for (int i = 0; i < chord->noteCount(); i++) {
        Note *n = chord->note(i);
        Staff * s = n->staff();
        Clef* clef = s->lastClefChange(barIdx);
        int line = 10;
        if (clef) line = clef->pitchToLine(n->pitch());

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

        double ypos = s->top() + line * s->lineSpacing() / 2;
        if (ypos < topy) {
            topy = ypos;
            topLine = line;
            topStaff = s;
        }
        if (ypos > bottomy) {
            bottomy = ypos;
            bottomLine = line;
            bottomStaff = s;
        }        
        
        m_style->renderNoteHead( painter, ref.x() + x, ref.y() + /*chord->y() +*/ s->top() + line * s->lineSpacing() / 2, chord->duration(), color );

        // render accidentals
        // TODO I think this could really use some improvements
        // fetch key signature of current bar
        VoiceBar* vb = chord->voiceBar();
        Bar* bar = vb->bar();
        KeySignature* ks = s->lastKeySignatureChange(bar);
        int curAccidentals = 0;
        if (ks) curAccidentals = ks->accidentals(n->pitch());
        // next check the bar for the last previous note in the same voice with the same pitch
        for (int e = 0; e < vb->elementCount(); e++) {
            Chord* c = dynamic_cast<Chord*>(vb->element(e));
            if (!c) continue;
            if (c == chord) break;
            for (int nid = 0; nid < c->noteCount(); nid++) {
                Note* note = c->note(nid);
                if (note->staff() != s) continue;
                if (note->pitch() == n->pitch()) {
                    curAccidentals = note->accidentals();
                }
            }
        }

        if (n->accidentals() != curAccidentals) {
            m_style->renderAccidental( painter, ref.x() + x - 10, ref.y() + /*chord->y() +*/ s->top() + line * s->lineSpacing() / 2, n->accidentals(), color );
        }

        // render dots of notes
        double dotX = x + 11;
        painter.setPen(m_style->noteDotPen(color));
        for (int i = 0; i < chord->dots(); i++) {
            painter.drawPoint(ref + QPointF(dotX, /*chord->y() +*/ s->top() + line * s->lineSpacing() / 2));
            dotX += 3;
        }
    }

    //Staff * s = chord->note(0)->staff(); // TODO: make this work with chords spanning multiple staves

    double center = (bottomLine + topLine) * 0.5;
    double stemLen = chord->stemLength() * 2;
    if (stemLen != 0.0 && stemLen != -0.0) {
        double stemX = x + 6;
        bool stemsUp = chord->stemDirection() == Chord::StemUp;
        if (!stemsUp) { stemX = x; }
//        if (center < 4) { stemX = x; stemsUp = false; }
        painter.setPen(m_style->stemPen(color));
        if (stemsUp) {
            painter.drawLine(ref + QPointF(stemX, /*chord->y() +*/ topStaff->top() + (topLine - stemLen) * topStaff->lineSpacing() / 2),
                             ref + QPointF(stemX, /*chord->y() +*/ bottomStaff->top() + bottomLine * bottomStaff->lineSpacing() / 2));
            m_style->renderNoteFlags( painter, ref.x() + stemX, ref.y() + /*chord->y() +*/ topStaff->top() + (topLine - stemLen) * topStaff->lineSpacing() / 2, chord->duration(), stemsUp, color );
        } else {
            painter.drawLine(ref + QPointF(stemX, /*chord->y() +*/ topStaff->top() + topLine * topStaff->lineSpacing() / 2),
                             ref + QPointF(stemX, /*chord->y() +*/ bottomStaff->top() + (bottomLine + stemLen) * bottomStaff->lineSpacing() / 2));
            m_style->renderNoteFlags( painter, ref.x() + stemX, ref.y() + /*chord->y() +*/ bottomStaff->top() + (bottomLine + stemLen) * bottomStaff->lineSpacing() / 2, chord->duration(), stemsUp, color );
        }
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

void MusicRenderer::renderAccidental(QPainter& painter, int accidentals, const QPointF& pos, const QColor& color)
{
    m_style->renderAccidental( painter, pos.x(), pos.y(), accidentals, color );
}
