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

#include <KoXmlReader.h>

#include <kdebug.h>

#include <QHash>

#define NS_MUSIC "http://www.koffice.org/music"

namespace MusicCore {
namespace MusicXmlReader {

static QString getProperty(const KoXmlElement& elem, const char *propName)
{
    KoXmlElement propElem = KoXml::namedItemNS(elem, NS_MUSIC, propName);
    return propElem.text();
}

static void loadPart(const KoXmlElement& partElement, Part* part)
{
    Sheet* sheet = part->sheet();

    KoXmlElement bar;
    int curBar = 0;
    forEachElement(bar, partElement) {
        if (bar.namespaceURI() != NS_MUSIC || bar.localName() != "measure") continue;

        Bar* bar = NULL;
        if (curBar >= sheet->barCount()) {
            bar = sheet->addBar();
        } else {
            bar = sheet->bar(curBar);
        }

        KoXmlElement e;
        forEachElement(e, bar) {
            if (e.namespaceURI() != NS_MUSIC) continue;

            if (e.localName() == "attributes") {
            } else if (e.localName() == "note") {
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
