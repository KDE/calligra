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
#include "Engraver.h"
#include "core/Bar.h"
#include "core/Sheet.h"
#include "core/Voice.h"
#include "core/Part.h"
#include "core/VoiceBar.h"
#include "core/VoiceElement.h"
#include "core/Clef.h"
#include "core/Staff.h"
#include "core/StaffSystem.h"
#include "core/KeySignature.h"
#include "core/Chord.h"
#include "core/Note.h"

#include <limits.h>

#include <QtCore/QList>
#include <QtCore/QVarLengthArray>

#include <kdebug.h>

using namespace MusicCore;

Engraver::Engraver()
{
}

void Engraver::engraveSheet(Sheet* sheet, QSizeF size, bool engraveBars)
{
    if (engraveBars) {
        // engrave all bars in the sheet
        for (int i = 0; i < sheet->barCount(); i++) {
            engraveBar(sheet->bar(i));
        }
    }

    // now layout bars in staff systems
    int curSystem = 0;
    QPointF p(0, sheet->staffSystem(curSystem)->top());
    int lastStart = 0;
    double lineWidth = size.width();
    double indent = 0;
    bool prevPrefixPlaced = false;
    for (int i = 0; i < sheet->barCount(); i++) {
        Bar* bar = sheet->bar(i);
        bool prefixPlaced = false;
        if (i > 0 && p.x() + bar->desiredSize() + bar->prefix() - indent > lineWidth) {
            // scale all sizes
            // first calculate the total scalable width and total fixed width of all preceding bars
            double scalable = 0, fixed = 0;
            for (int j = lastStart; j < i; j++) {
                scalable += sheet->bar(j)->size();
                fixed += sheet->bar(j)->prefix();
            }
            fixed += bar->prefix();
            if (prevPrefixPlaced) {
                fixed -= sheet->bar(lastStart)->prefix();
            }
            // now scale factor is (available width - fixed) / scalable width;
            double factor = (lineWidth - fixed) / scalable;
            QPointF sp = sheet->bar(lastStart)->position() - QPointF(sheet->bar(lastStart)->prefix(), 0);
            for (int j = lastStart; j < i; j++) {
                sheet->bar(j)->setPosition(sp + QPointF(sheet->bar(j)->prefix(), 0));
                sheet->bar(j)->setSize(sheet->bar(j)->desiredSize() * factor);
                sp.setX(sp.x() + sheet->bar(j)->size() + sheet->bar(j)->prefix());
            }
            lastStart = i;
            if (bar->prefix() > 0) {
                bar->setPrefixPosition(sp);
                prefixPlaced = true;
            }
            prevPrefixPlaced = prefixPlaced;

            curSystem++;
            p.setY(sheet->staffSystem(curSystem)->top());
            sheet->staffSystem(curSystem)->setFirstBar(i);

            indent = 0;
            QList<Clef*> clefs;
            // Extra space for clef/key signature repeating            
            for (int partIdx = 0; partIdx < sheet->partCount(); partIdx++) {
                Part* part = sheet->part(partIdx);
                for (int staffIdx = 0; staffIdx < part->staffCount(); staffIdx++) {
                    Staff* staff = part->staff(staffIdx);
                    double w = 0;
                    Clef* clef = staff->lastClefChange(i, 0);
                    if (clef) {
                        w += clef->width() + 15;
                        clefs.append(clef);
                    }
                    KeySignature* ks = staff->lastKeySignatureChange(i);
                    if (ks) w += ks->width() + 15;
                    if (w > indent) indent = w;
                }
            }
            sheet->staffSystem(curSystem)->setIndent(indent);
            sheet->staffSystem(curSystem)->setLineWidth(lineWidth);
            sheet->staffSystem(curSystem)->setClefs(clefs);
            lineWidth = size.width() - indent;
            p.setX(indent - bar->prefix());
        }
        sheet->bar(i)->setPosition(p + QPointF(bar->prefix(), 0), !prefixPlaced);
        sheet->bar(i)->setSize(sheet->bar(i)->desiredSize());
        p.setX(p.x() + sheet->bar(i)->size() + bar->prefix());
    }
    // potentially scale last staff system if it is too wide
    if (p.x() - indent > lineWidth) {
        double scalable = 0, fixed = 0;
        for (int j = lastStart; j < sheet->barCount(); j++) {
            scalable += sheet->bar(j)->size();
            fixed += sheet->bar(j)->prefix();
        }
        // now scale factor is (available width - fixed) / scalable width;
        double factor = (lineWidth - fixed) / scalable;
        QPointF sp = sheet->bar(lastStart)->position() - QPointF(sheet->bar(lastStart)->prefix(), 0);
        for (int j = lastStart; j < sheet->barCount(); j++) {
            sheet->bar(j)->setPosition(sp + QPointF(sheet->bar(j)->prefix(), 0));
            sheet->bar(j)->setSize(sheet->bar(j)->desiredSize() * factor);
            sp.setX(sp.x() + sheet->bar(j)->size() + sheet->bar(j)->prefix());
        }
    }

    sheet->setStaffSystemCount(curSystem+1);
}

void Engraver::engraveBar(Bar* bar)
{
    Sheet* sheet = bar->sheet();
    int barIdx = sheet->indexOfBar(bar);

    // collect all voices in all parts
    QList<VoiceBar*> voices;
    QList<int> voiceIds;
    for (int p = 0; p < sheet->partCount(); p++) {
        Part* part = sheet->part(p);
        for (int v = 0; v < part->voiceCount(); v++) {
            voices.append(bar->voice(part->voice(v)));
            voiceIds.append(v);
        }
    }

    QVarLengthArray<int> nextTime(voices.size());
    QVarLengthArray<int> nextIndex(voices.size());
    // initialize stuff to 0
    for (int i = 0; i < voices.size(); i++) {
        nextTime[i] = 0;
        nextIndex[i] = 0;
    }

    // collect staff elements in all staffs
    int staffCount = 0;
    for (int p = 0; p < sheet->partCount(); p++) {
        staffCount += sheet->part(p)->staffCount();
    }

    QVarLengthArray<QList<StaffElement*> > staffElements(staffCount);

    for (int st = 0, p = 0; p < sheet->partCount(); p++) {
        Part* part = sheet->part(p);
        for (int s = 0; s < part->staffCount(); s++, st++) {
            Staff* staff = part->staff(s);
            for (int i = 0; i < bar->staffElementCount(staff); i++) {
                staffElements[st].append(bar->staffElement(staff, i));
            }
        }
    }
    
    QMultiMap<Staff*, VoiceBar*> staffVoices;
    foreach (VoiceBar* vb, voices) {
        for (int e = 0; e < vb->elementCount(); e++) {
            Staff* s = vb->element(e)->staff();
            if (!staffVoices.contains(s, vb)) staffVoices.insert(s, vb);
        }
    }

    double x = 0; // this is the end position of the last placed elements
    bool endOfPrefix = false;
    QList<StaffElement*> prefix;
    // loop until all elements are placed
    for (;;) {
        // find earliest start time
        int time = INT_MAX;
        for (int i = 0; i < voices.size(); i++) {
            if (nextIndex[i] < voices[i]->elementCount()) {
                if (nextTime[i] < time) time = nextTime[i];
            }
        }

        bool staffElement = false;
        int priority = INT_MIN;
        for (int s = 0; s < staffCount; s++) {
            if (staffElements[s].size() > 0) {
                if (staffElements[s][0]->startTime() <= time) {
                    time = staffElements[s][0]->startTime();
                    staffElement = true;
                    if (staffElements[s][0]->priority() > priority) {
                        priority = staffElements[s][0]->priority();
                    }
                }
            }
        }

        if ((!staffElement || time > 0) && !endOfPrefix) {
            // we've reached the end of the prefix; now update all already placed staff elements to have correct
            // (negative) x coordinates, and set the size of the prefix.
            if (prefix.size() > 0) {
                double prefixSize = x + 5;
                bar->setPrefix(prefixSize);
                foreach (StaffElement* se, prefix) {
                    se->setX(se->x() - prefixSize);
                }
                x = 0;
            } else {
                bar->setPrefix(0.0);
            }
            endOfPrefix = true;
        }
                
        // none found, break
        if (time == INT_MAX) break;

        double maxEnd = x;
        // now update all items with correct start time
        if (staffElement) {
            for (int s = 0; s < staffCount; s++) {
                if (staffElements[s].size() > 0 && staffElements[s][0]->startTime() == time && staffElements[s][0]->priority() == priority) {
                    StaffElement* se = staffElements[s].takeAt(0);
                    double xpos = x + 15;
                    se->setX(xpos);
                    double xend = se->width() + xpos;
                    if (xend > maxEnd) maxEnd = xend;
                    if (!endOfPrefix) prefix.append(se);
                }
            }
        } else {
            for (int i = 0; i < voices.size(); i++) {
                if (nextTime[i] == time && nextIndex[i] < voices[i]->elementCount()) {
                    // If it is a chord, also figure out correct stem direction for the chord; right now
                    // direction is only based on position of the notes, but in the future this should
                    // also depend on other chord in other voices in the same staff
                    Chord* c = dynamic_cast<Chord*>(voices[i]->element(nextIndex[i]));
                    if (c) {
                        // if this is the continuation or end of a beam, the first chord in the beam has the
                        // correct stem direction already
                        if (c->beamType(0) == Chord::BeamContinue || c->beamType(0) == Chord::BeamEnd) {
                            c->setStemDirection(c->beamStart(0)->stemDirection());
                        } else {
                            Staff* staff = c->staff();
                            if (staffVoices.count(staff) > 1) {
                                int voiceIdx = voiceIds[i];
                                c->setStemDirection(voiceIdx & 1 ? Chord::StemDown : Chord::StemUp);
                            } else {
                                int topLine = 0, bottomLine = 0;
                                double topy = 1e9, bottomy = -1e9;
                                for (int n = 0; n < c->noteCount(); n++) {
                                    Note* note = c->note(n);
                                    Staff * s = note->staff();
                                    Clef* clef = s->lastClefChange(barIdx);
                                    int line = clef->pitchToLine(note->pitch());
                                    double ypos = s->top() + line * s->lineSpacing() / 2;
                                    if (ypos < topy) {
                                        topy = ypos;
                                        topLine = line;
                                    }
                                    if (ypos > bottomy) {
                                        bottomy = ypos;
                                        bottomLine = line;
                                    }        
                                }
                                double center = (bottomLine + topLine) * 0.5;
                                c->setStemDirection(center < 4 ? Chord::StemDown : Chord::StemUp);
                            }
                        }
                    }
                    
                    double xpos = x + 15;
                    voices[i]->element(nextIndex[i])->setX(xpos);
                    double xend = voices[i]->element(nextIndex[i])->width() + xpos;
                    if (xend > maxEnd) maxEnd = xend;
                    nextTime[i] += voices[i]->element(nextIndex[i])->length();
                    nextIndex[i]++;
                }
            }
        }

        x = maxEnd;
    }
    if (x < 30) x = 30;
    bar->setDesiredSize(x + 15);
}
