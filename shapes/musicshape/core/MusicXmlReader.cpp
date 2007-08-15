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
#include "MusicXmlReader.h"
#include "Sheet.h"
#include "Part.h"
#include "Chord.h"
#include "Voice.h"
#include "VoiceBar.h"
#include "KeySignature.h"
#include "Bar.h"
#include "Clef.h"
#include "TimeSignature.h"

#include <KoXmlReader.h>

#include <kdebug.h>

#include <QHash>

#include <math.h>

#define NS_MUSIC "http://www.koffice.org/music"

namespace MusicCore {
namespace MusicXmlReader {

static QString getProperty(const KoXmlElement& elem, const char *propName)
{
    KoXmlElement propElem = KoXml::namedItemNS(elem, NS_MUSIC, propName);
    return propElem.text();
}

static Chord::Duration parseDuration(const QString& type, int length, int div)
{
    if (type == "128th")        return Chord::HundredTwentyEighth;
    else if (type == "64th")    return Chord::SixtyFourth;
    else if (type == "32nd")    return Chord::ThirtySecond;
    else if (type == "16th")    return Chord::Sixteenth;
    else if (type == "eighth")  return Chord::Eighth;
    else if (type == "quarter") return Chord::Quarter;
    else if (type == "half")    return Chord::Half;
    else if (type == "whole")   return Chord::Whole;
    else if (type == "breve")   return Chord::Breve;

    // else try to parse it from length
    double fact = 26880.0 / div;
    int ticks = (int) round(length * fact);
    // TODO: take number of dots into account
    if (ticks <= VoiceElement::Note128Length)       return Chord::HundredTwentyEighth;
    else if (ticks <= VoiceElement::Note64Length)   return Chord::SixtyFourth;
    else if (ticks <= VoiceElement::Note32Length)   return Chord::ThirtySecond;
    else if (ticks <= VoiceElement::Note16Length)   return Chord::Sixteenth;
    else if (ticks <= VoiceElement::Note8Length)    return Chord::Eighth;
    else if (ticks <= VoiceElement::QuarterLength)  return Chord::Quarter;
    else if (ticks <= VoiceElement::HalfLength)     return Chord::Half;
    else if (ticks <= VoiceElement::WholeLength)    return Chord::Whole;
    else                                            return Chord::Breve;
}

static Clef* loadClef(const KoXmlElement& element, Staff* staff)
{
    QString shapeStr = getProperty(element, "sign");
    Clef::ClefShape shape = Clef::GClef;
    int line = 2;
    if (shapeStr == "G") {
        line = 2;
        shape = Clef::GClef;
    } else if (shapeStr == "F") {
        line = 4;
        shape = Clef::FClef;
    } else if (shapeStr == "C") {
        line = 3;
        shape = Clef::CClef;
    }
    
    QString lineStr = getProperty(element, "line");
    if (!lineStr.isNull()) line = lineStr.toInt();
    
    int octave = 0;
    QString octaveStr = getProperty(element, "clef-octave-change");
    if (!octaveStr.isNull()) octave = octaveStr.toInt();
    
    return new Clef(staff, 0, shape, line, octave);
}

static TimeSignature* loadTimeSignature(const KoXmlElement& element, Staff* staff)
{
    int beats = getProperty(element, "beats").toInt();
    int beat = getProperty(element, "beat-type").toInt();
    
    return new TimeSignature(staff, 0, beats, beat);
}

static void loadPart(const KoXmlElement& partElement, Part* part)
{
    Sheet* sheet = part->sheet();

    KoXmlElement barElem;

    int curBar = 0;
    int curDivisions = 26880;
    Chord* lastNote = NULL;

    forEachElement(barElem, partElement) {
        if (barElem.namespaceURI() != NS_MUSIC || barElem.localName() != "measure") continue;

        Bar* bar = NULL;
        if (curBar >= sheet->barCount()) {
            bar = sheet->addBar();
        } else {
            bar = sheet->bar(curBar);
        }

        KoXmlElement e;
        forEachElement(e, barElem) {
            if (e.namespaceURI() != NS_MUSIC) continue;

            if (e.localName() == "attributes") {
                KoXmlElement attr;

                QString staffCountStr = getProperty(e, "staves");
                if (!staffCountStr.isNull()) {
                    int staffCount = staffCountStr.toInt();
                    while (staffCount > part->staffCount()) {
                        part->addStaff();
                    }
                }
                
                forEachElement(attr, e) {
                    if (attr.namespaceURI() != NS_MUSIC) continue;
                    
                    if (attr.localName() == "divisions") {
                        curDivisions = attr.text().toInt();
                    } else if (attr.localName() == "key") {
                        QString number = attr.attribute("number");
                        int staffId = 0;
                        if (!number.isNull()) staffId = number.toInt() - 1;
                        
                        KeySignature* ks = new KeySignature(part->staff(staffId), 0, getProperty(attr, "fifths").toInt());
                        bar->addStaffElement(ks);
                    } else if (attr.localName() == "clef") {
                        QString number = attr.attribute("number");
                        int staffId = 0;
                        if (!number.isNull()) staffId = number.toInt() - 1;
                        
                        Clef* clef = loadClef(attr, part->staff(staffId));
                        bar->addStaffElement(clef);
                    } else if (attr.localName() == "time") {
                        QString number = attr.attribute("number");
                        int staffId = 0;
                        if (!number.isNull()) staffId = number.toInt() - 1;
                        
                        TimeSignature* ts = loadTimeSignature(attr, part->staff(staffId));
                        bar->addStaffElement(ts);
                    }
                }
                
            } else if (e.localName() == "note") {
                QString staffStr = getProperty(e, "staff");
                int staffId = 0;
                if (!staffStr.isNull()) staffId = staffStr.toInt() - 1;
                                
                if (KoXml::namedItemNS(e, NS_MUSIC, "chord").isNull()) {
                    // no chord element, so this is the start of a new chord
                    int length = getProperty(e, "duration").toInt();
                    QString type = getProperty(e, "type");
                    Chord::Duration duration = parseDuration(type, length, curDivisions);
                    
                    QString voiceStr = getProperty(e, "voice");
                    int voiceId = 0;
                    if (!voiceStr.isNull()) voiceId = voiceStr.toInt() - 1;
                    while (voiceId >= part->voiceCount()) {
                        part->addVoice();
                    }
                    
                    //TODO dots
                    Staff* staff = part->staff(staffId);
                    lastNote = new Chord(staff, duration);
                    Voice* voice = part->voice(voiceId);
                    voice->bar(bar)->addElement(lastNote);
                }

                KoXmlElement pitch = KoXml::namedItemNS(e, NS_MUSIC, "pitch");
                if (!pitch.isNull()) {
                    QString step = getProperty(pitch, "step");
                    int octave = getProperty(pitch, "octave").toInt();
                    int note = step[0].toAscii() - 'A';
                    note -= 2;
                    if (note < 0) note += 7;
                    note += (octave - 4) * 7;
                    lastNote->addNote(part->staff(staffId), note);
                }
            }
        }

        curBar++;
    }
}

Sheet* loadSheet(const KoXmlElement& scoreElement)
{
    Sheet* sheet = new Sheet();

    QHash<QString, Part*> parts;

    kDebug() << "loading sheet ======================================================================================";

    KoXmlElement partList = KoXml::namedItemNS(scoreElement, NS_MUSIC, "part-list");
    KoXmlElement elem;
    forEachElement(elem, partList) {
        if (elem.namespaceURI() == NS_MUSIC && elem.localName() == "score-part") {
            QString id = elem.attribute("id");
            QString name = getProperty(elem, "part-name");
            QString abbr = getProperty(elem, "part-abbreviation");
            Part* p = sheet->addPart(name);
            p->setShortName(abbr);
            // always add one voice and one staff
            p->addVoice();
            p->addStaff();
            parts[id] = p;
        }
    }

    forEachElement(elem, scoreElement) {
        if (elem.namespaceURI() == NS_MUSIC && elem.localName() == "part") {
            QString id = elem.attribute("id");
            loadPart(elem, parts[id]);
        }
    }

    return sheet;
}

}
} // namespace MusicCore
