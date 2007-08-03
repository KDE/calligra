/* This file is part of the KDE project
 * Copyright 2007 Marijn Kruisselbrink <m.Kruisselbrink@student.tue.nl>
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
#include "NoteEntryAction.h"

#include "../SimpleEntryTool.h"
#include "../MusicShape.h"
#include "../Renderer.h"

#include "../core/Staff.h"
#include "../core/Clef.h"
#include "../core/Voice.h"
#include "../core/Part.h"
#include "../core/VoiceBar.h"
#include "../core/Sheet.h"
#include "../core/Bar.h"

#include "../commands/CreateChordCommand.h"
#include "../commands/AddNoteCommand.h"

#include <kicon.h>
#include <kdebug.h>

using namespace MusicCore;

static QIcon getIcon(Chord::Duration duration, bool isRest)
{
    QString base = isRest ? "music-rest-" : "music-note-";
    switch (duration) {
        case Chord::Breve:          return KIcon(base + "breve");
        case Chord::Whole:          return KIcon(base + "whole");
        case Chord::Half:           return KIcon(base + "half");
        case Chord::Quarter:        return KIcon(base + "quarter");
        case Chord::Eighth:         return KIcon(base + "eighth");
        case Chord::Sixteenth:      return KIcon(base + "16th");
        case Chord::ThirtySecond:   return KIcon(base + "32nd");
        case Chord::SixtyFourth:    return KIcon(base + "64th");
        case Chord::HundredTwentyEighth: return KIcon(base + "128th");
    }
    return KIcon();
}

static QString getText(Chord::Duration duration, bool isRest)
{
    QString base = isRest ? "rest" : "note";
    switch (duration) {
        case Chord::Breve:          return "Double whole " + base;
        case Chord::Whole:          return "Whole " + base;
        case Chord::Half:           return "Half " + base;
        case Chord::Quarter:        return "Quarter " + base;
        case Chord::Eighth:         return "Eighth " + base;
        case Chord::Sixteenth:      return "16th " + base;
        case Chord::ThirtySecond:   return "32nd " + base;
        case Chord::SixtyFourth:    return "64th " + base;
        case Chord::HundredTwentyEighth: return "128th " + base;
    }
    return "Unknown " + base;
}

NoteEntryAction::NoteEntryAction(Chord::Duration duration, bool isRest, SimpleEntryTool* tool)
    : AbstractMusicAction(getIcon(duration, isRest), getText(duration, isRest), tool)
    , m_duration(duration), m_isRest(isRest)
{
}

void NoteEntryAction::renderPreview(QPainter& painter, const QPointF& point)
{
    if (!m_isRest) {
        double sl = 3.5;
        if (m_duration < MusicCore::Chord::Sixteenth) sl += 1;
        if (m_duration < MusicCore::Chord::ThirtySecond) sl += 1;
        m_tool->shape()->renderer()->renderNote(painter, m_duration, point - QPointF(3, 0), sl * 5, Qt::gray);
    } else {
        m_tool->shape()->renderer()->renderRest(painter, m_duration, point, Qt::gray);
    }
}

void NoteEntryAction::mousePress(Staff* staff, int bar, const QPointF& pos)
{
    Clef* clef = staff->lastClefChange(bar);

    Voice* voice = staff->part()->voice(m_tool->voice());
    VoiceBar* vb = voice->bar(bar);

    // find element before which to insert the chord
    int before = 0;
    double realX = pos.x() / staff->part()->sheet()->bar(bar)->scale();
    for (int i = 0; i < vb->elementCount(); i++) {
        VoiceElement* e = vb->element(i);
        if (e->x() >= realX) break;
        before++;
    }

    Chord* join = NULL;
    if (before > 0) join = dynamic_cast<Chord*>(vb->element(before-1));
    if (join && join->x() + join->width() >= realX) {
        if (clef && !m_isRest) {
            int line = staff->line(pos.y());
            int pitch = clef->lineToPitch(line);
            m_tool->addCommand(new AddNoteCommand(m_tool->shape(), join, staff, m_duration, pitch));
        } else {
            // TODO make it a rest
        }
    } else {
        if (clef && !m_isRest) {
            int line = staff->line(pos.y());
            int pitch = clef->lineToPitch(line);
            m_tool->addCommand(new CreateChordCommand(m_tool->shape(), vb, staff, m_duration, before, pitch));
        } else {
            m_tool->addCommand(new CreateChordCommand(m_tool->shape(), vb, staff, m_duration, before));
        }
    }
}

