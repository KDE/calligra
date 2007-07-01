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
#include "MusicXmlWriter.h"
#include "Sheet.h"
#include "Part.h"
#include "PartGroup.h"

#include <KoXmlWriter.h>
#include <kofficeversion.h>

using namespace MusicCore;

MusicXmlWriter::MusicXmlWriter()
{
}

MusicXmlWriter::~MusicXmlWriter()
{
}

static void writePartGroup(KoXmlWriter& w, int id, PartGroup* group)
{
    w.startElement("music:part-group");
    w.addAttribute("type", "start");
    w.addAttribute("number", id);

    if (!group->name().isNull()) {
        w.startElement("music:group-name");
        w.addTextNode(group->name());
        w.endElement(); // music:group-name
    }
    if (!group->shortName(false).isNull()) {
        w.startElement("music:group-abbreviation");
        w.addTextNode(group->shortName());
        w.endElement(); // music:group-abbreviation
    }

    if (group->symbol() != PartGroup::None) {
        w.startElement("music:group-symbol");
        switch (group->symbol()) {
            case PartGroup::None:       w.addTextNode("none");   break;
            case PartGroup::Brace:      w.addTextNode("brace");  break;
            case PartGroup::Line:       w.addTextNode("line");   break;
            case PartGroup::Bracket:    w.addTextNode("bracket"); break;
        }
        w.endElement(); // music:group-symbol
    }

    w.startElement("music:group-barline");
    w.addTextNode(group->commonBarLines() ? "yes" : "no");
    w.endElement(); // music:group-barline

    w.endElement(); // music:part-group
}

static void writePartDesc(KoXmlWriter& w, int id, Part* part)
{
    w.startElement("music:score-part");
    w.addAttribute("id", QString("P%1").arg(id));

    w.startElement("music:part-name");
    w.addTextNode(part->name());
    w.endElement(); // music:part-name

    QString abbr = part->shortName(false);
    if (!abbr.isNull()) {
        w.startElement("music:part-abbreviation");
        w.addTextNode(abbr);
        w.endElement(); // music:part-abbreviation
    }

    w.endElement(); // music:score-part
}

static void writePart(KoXmlWriter& w, int id, Part* part)
{
    w.startElement("music:part");
    w.addAttribute("id", QString("P%1").arg(id));

    for (int i = 0; i < part->sheet()->barCount(); i++) {
        w.startElement("music:measure");
        w.addAttribute("number", i+1);
        w.endElement(); // music:measure
    }

    w.endElement(); // music:part
}

void MusicXmlWriter::writeSheet(KoXmlWriter& w, Sheet* sheet)
{
//    w.startDocument("score-partwise", "-//Recordare//DTD MusicXML 1.1 Partwise//EN",
//        "http://www.musicxml.org/dtds/partwise.dtd");
    w.startElement("music:score-partwise");
    w.addAttribute("xmlns:music", "http://www.koffice.org/music");
    w.addAttribute("version", "1.1");

    w.startElement("music:part-list");
    for (int i = 0; i < sheet->partCount(); i++) {
        for (int pg = 0; pg < sheet->partGroupCount(); pg++) {
            if (sheet->partGroup(pg)->firstPart() == i) {
                writePartGroup(w, pg+1, sheet->partGroup(pg));
            }
        }
        writePartDesc(w, i, sheet->part(i));
        for (int pg = 0; pg < sheet->partGroupCount(); pg++) {
            if (sheet->partGroup(pg)->lastPart() == i) {
                w.startElement("music:part-group");
                w.addAttribute("type", "stop");
                w.addAttribute("number", pg+1);
                w.endElement(); // music:part-group
            }
        }
    }
    w.endElement(); // music:part-list

    for (int i = 0; i < sheet->partCount(); i++) {
        writePart(w, i, sheet->part(i));
    }
        
    w.endElement(); // music:score-partwise
//    w.endDocument();
}

