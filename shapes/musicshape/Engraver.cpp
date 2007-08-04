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
    for (int i = 0; i < sheet->barCount(); i++) {
        if (i > 0 && p.x() + sheet->bar(i)->desiredSize() - indent > lineWidth) {
            // scale all sizes
            double factor = lineWidth / (p.x() - indent);
            QPointF sp = sheet->bar(lastStart)->position();
            for (int j = lastStart; j < i; j++) {
                sheet->bar(j)->setPosition(sp);
                sheet->bar(j)->setSize(sheet->bar(j)->desiredSize() * factor);
                sp.setX(sp.x() + sheet->bar(j)->size());
            }

            lastStart = i;

            p.setX(0);
            curSystem++;
            p.setY(sheet->staffSystem(curSystem)->top());
            sheet->staffSystem(curSystem)->setFirstBar(i);

            indent = 0;
            // Extra space for clef/key signature repeating
            for (int partIdx = 0; partIdx < sheet->partCount(); partIdx++) {
                Part* part = sheet->part(partIdx);
                for (int staffIdx = 0; staffIdx < part->staffCount(); staffIdx++) {
                    Staff* staff = part->staff(staffIdx);
                    double w = 0;
                    Clef* clef = staff->lastClefChange(i, 0);
                    if (clef) w += clef->width() + 15;
                    KeySignature* ks = staff->lastKeySignatureChange(i);
                    if (ks) w += ks->width() + 15;
                    if (w > indent) indent = w;
                }
            }
            sheet->staffSystem(curSystem)->setIndent(indent);
            sheet->staffSystem(curSystem)->setLineWidth(lineWidth);
            lineWidth = size.width() - indent;
            p.setX(indent);
        }
        sheet->bar(i)->setPosition(p);
        sheet->bar(i)->setSize(sheet->bar(i)->desiredSize());
        p.setX(p.x() + sheet->bar(i)->size());
    }
    // potentially scale last staff system if it is too wide
    if (p.x() - indent > lineWidth) {
        double factor = lineWidth / (p.x() - indent);
        QPointF sp = sheet->bar(lastStart)->position();
        for (int j = lastStart; j < sheet->barCount(); j++) {
            sheet->bar(j)->setPosition(sp);
            sheet->bar(j)->setSize(sheet->bar(j)->desiredSize() * factor);
            sp.setX(sp.x() + sheet->bar(j)->size());
        }
    }

    sheet->setStaffSystemCount(curSystem+1);
}

void Engraver::engraveBar(Bar* bar)
{
    Sheet* sheet = bar->sheet();

    // collect all voices in all parts
    QList<VoiceBar*> voices;
    for (int p = 0; p < sheet->partCount(); p++) {
        Part* part = sheet->part(p);
        for (int v = 0; v < part->voiceCount(); v++) {
            voices.append(bar->voice(part->voice(v)));
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

    double x = 0; // this is the end position of the last placed elements
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
        for (int s = 0; s < staffCount; s++) {
            if (staffElements[s].size() > 0) {
                if (staffElements[s][0]->startTime() <= time) {
                    time = staffElements[s][0]->startTime();
                    staffElement = true;
                }
            }
        }

        // none found, break
        if (time == INT_MAX) break;

        double maxEnd = x;
        // now update all items with correct start time
        if (staffElement) {
            for (int s = 0; s < staffCount; s++) {
                if (staffElements[s].size() > 0 && staffElements[s][0]->startTime() == time) {
                    StaffElement* se = staffElements[s].takeAt(0);
                    double xpos = x + 15;
                    se->setX(xpos);
                    double xend = se->width() + xpos;
                    if (xend > maxEnd) maxEnd = xend;
                }
            }
        } else {
            for (int i = 0; i < voices.size(); i++) {
                if (nextTime[i] == time && nextIndex[i] < voices[i]->elementCount()) {
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
    bar->setDesiredSize(x + 15);
}
