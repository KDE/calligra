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
    w.startElement("part-group");
    w.addAttribute("type", "start");
    w.addAttribute("number", id);

    if (!group->name().isNull()) {
        w.startElement("group-name");
        w.addTextNode(group->name());
        w.endElement(); // group-name
    }
    // TODO group-abbreviation

    if (group->symbol() != PartGroup::None) {
        w.startElement("group-symbol");
        switch (group->symbol()) {
            case PartGroup::None:       w.addTextNode("none");   break;
            case PartGroup::Brace:      w.addTextNode("brace");  break;
            case PartGroup::Line:       w.addTextNode("line");   break;
            case PartGroup::Bracket:    w.addTextNode("bracket"); break;
        }
        w.endElement(); // group-symbol
    }

    w.startElement("group-barline");
    w.addTextNode(group->commonBarLines() ? "yes" : "no");
    w.endElement(); // group-barline

    w.endElement(); // part-group
}

static void writePartDesc(KoXmlWriter& w, int id, Part* part)
{
    w.startElement("score-part");
    w.addAttribute("id", QString("P%1").arg(id));

    w.startElement("part-name");
    w.addTextNode(part->name());
    w.endElement(); // part-name

    QString abbr = part->shortName(false);
    if (!abbr.isNull()) {
        w.startElement("part-abbreviation");
        w.addTextNode(abbr);
        w.endElement(); // part-abbreviation
    }

    w.endElement(); // score-part
}

static void writePart(KoXmlWriter& w, int id, Part* part)
{
    w.startElement("part");
    w.addAttribute("id", QString("P%1").arg(id));

    for (int i = 0; i < part->sheet()->barCount(); i++) {
        w.startElement("measure");
        w.addAttribute("number", i+1);
        w.endElement(); // measure
    }

    w.endElement(); // part
}

void MusicXmlWriter::writeSheet(QIODevice* dev, Sheet* sheet)
{
    KoXmlWriter w(dev);
    w.startDocument("score-partwise", "-//Recordare//DTD MusicXML 1.1 Partwise//EN",
        "http://www.musicxml.org/dtds/partwise.dtd");
    w.startElement("score-partwise");
    w.addAttribute("version", "1.1");

    w.startElement("identification");
    w.startElement("encoding");
    w.startElement("software");
    w.addTextNode( QString("KOffice/%1").arg(KOFFICE_VERSION_STRING) );
    w.endElement(); // software
    w.endElement(); // encoding
    w.endElement(); // identification

    w.startElement("part-list");
    for (int i = 0; i < sheet->partCount(); i++) {
        for (int pg = 0; pg < sheet->partGroupCount(); pg++) {
            if (sheet->partGroup(pg)->firstPart() == i) {
                writePartGroup(w, pg+1, sheet->partGroup(pg));
            }
        }
        writePartDesc(w, i, sheet->part(i));
        for (int pg = 0; pg < sheet->partGroupCount(); pg++) {
            if (sheet->partGroup(pg)->lastPart() == i) {
                w.startElement("part-group");
                w.addAttribute("type", "stop");
                w.addAttribute("number", pg+1);
                w.endElement(); // part-group
            }
        }
    }
    w.endElement(); // part-list

    for (int i = 0; i < sheet->partCount(); i++) {
        writePart(w, i, sheet->part(i));
    }
        
    w.endElement(); // score-partwise
    w.endDocument();
}

