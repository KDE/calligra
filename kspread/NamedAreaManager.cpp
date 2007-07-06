/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2002 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Phillip Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "NamedAreaManager.h"

// Qt
#include <QHash>

// KOffice
#include <KoDom.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

// KSpread
#include "Doc.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"
#include "Util.h"

using namespace KSpread;

struct NamedArea
{
    QString name;
    Sheet* sheet;
    QRect range;
};

class NamedAreaManager::Private
{
public:
    const Doc* doc;
    QHash<QString, NamedArea> namedAreas;
};

NamedAreaManager::NamedAreaManager(const Doc* doc)
    : d(new Private)
{
    d->doc = doc;
}

NamedAreaManager::~NamedAreaManager()
{
    delete d;
}

void NamedAreaManager::insert(Sheet* sheet, const QRect& range, const QString& name)
{
    NamedArea namedArea;
    namedArea.range = range;
    namedArea.sheet = sheet;
    namedArea.name = name;
    d->namedAreas[name] = namedArea;
    emit namedAreaAdded(name);
}

void NamedAreaManager::remove(const QString& name)
{
    if (!d->namedAreas.contains(name))
        return;
    const Map* map = d->namedAreas.value(name).sheet->map();
    d->namedAreas.remove(name);
    emit namedAreaRemoved(name);
    foreach (Sheet* sheet, map->sheetList())
        sheet->refreshRemoveAreaName(name);
}

void NamedAreaManager::remove(Sheet* sheet)
{
    const QList<NamedArea> namedAreas = d->namedAreas.values();
    for (int i = 0; i < namedAreas.count(); ++i)
    {
        if (namedAreas[i].sheet == sheet)
            remove(namedAreas[i].name);
    }
}

KSpread::Region NamedAreaManager::namedArea(const QString& name) const
{
    if (!d->namedAreas.contains(name))
        return Region();
    const NamedArea namedArea = d->namedAreas.value(name);
    return Region(namedArea.range, namedArea.sheet);
}

Sheet* NamedAreaManager::sheet(const QString& name) const
{
    if (!d->namedAreas.contains(name))
        return 0;
    return d->namedAreas.value(name).sheet;
}

bool NamedAreaManager::contains(const QString& name) const
{
    return d->namedAreas.contains(name);
}

QList<QString> NamedAreaManager::areaNames() const
{
    return d->namedAreas.keys();
}

void NamedAreaManager::loadOdf(const KoXmlElement& body)
{
    KoXmlNode namedAreas = KoDom::namedItemNS(body, KoXmlNS::table, "named-expressions");
    if (!namedAreas.isNull())
    {
        kDebug(36003) << "Loading named areas..." << endl;
        KoXmlElement element;
        forEachElement (element, namedAreas)
        {
            if (element.namespaceURI() != KoXmlNS::table)
                continue;
            if (element.localName() == "named-range")
            {
                if (!element.hasAttributeNS(KoXmlNS::table, "name"))
                    continue;
                if (!element.hasAttributeNS(KoXmlNS::table, "cell-range-address"))
                    continue;

                // TODO: what is: sheet:base-cell-address
                const QString name = element.attributeNS(KoXmlNS::table, "name", QString());
                const QString range = element.attributeNS(KoXmlNS::table, "cell-range-address", QString());
                kDebug(36003) << "Named area found, name: " << name << ", area: " << range << endl;

                Region region(Region::loadOdf(range), d->doc->map());
                if (!region.isValid())
                {
                    kDebug(36003) << "invalid area" << endl;
                    continue;
                }

                insert(region.firstSheet(), region.firstRange(), name);
            }
            else if (element.localName() == "named-expression")
            {
                kDebug(36003) << "Named expression found." << endl;
                // TODO
            }
        }
    }
}

void NamedAreaManager::saveOdf(KoXmlWriter& xmlWriter) const
{
    if (d->namedAreas.isEmpty())
        return;
    QString sheetName;
    Region region;
    xmlWriter.startElement("table:named-expressions");
    const QList<NamedArea> namedAreas = d->namedAreas.values();
    for (int i = 0; i < namedAreas.count(); ++i)
    {
        region = Region(namedAreas[i].range, namedAreas[i].sheet);
        xmlWriter.startElement("table:named-range");
        xmlWriter.addAttribute("table:name", namedAreas[i].name);
        xmlWriter.addAttribute("table:base-cell-address", region.saveOdf());
        xmlWriter.addAttribute("table:cell-range-address", region.saveOdf());
        xmlWriter.endElement();
    }
    xmlWriter.endElement();
}

void NamedAreaManager::loadXML(const KoXmlElement& parent)
{
    KoXmlElement element;
    forEachElement(element, parent)
    {
        if (element.tagName() == "reference")
        {
            Sheet* sheet = 0;
            QString tabname;
            QString refname;
            int left = 0;
            int right = 0;
            int top = 0;
            int bottom = 0;
            KoXmlElement sheetName = element.namedItem("tabname").toElement();
            if (!sheetName.isNull())
                sheet = d->doc->map()->findSheet(sheetName.text());
            if (!sheet)
                continue;
            KoXmlElement referenceName = element.namedItem("refname").toElement();
            if (!referenceName.isNull())
                refname = referenceName.text();
            KoXmlElement rect = element.namedItem("rect").toElement();
            if (!rect.isNull())
            {
                bool ok;
                if (rect.hasAttribute("left-rect"))
                    left = rect.attribute("left-rect").toInt(&ok);
                if (rect.hasAttribute("right-rect"))
                    right = rect.attribute("right-rect").toInt(&ok);
                if (rect.hasAttribute("top-rect"))
                    top = rect.attribute("top-rect").toInt(&ok);
                if (rect.hasAttribute("bottom-rect"))
                    bottom = rect.attribute("bottom-rect").toInt(&ok);
            }
            insert(sheet, QRect(QPoint(left, top), QPoint(right, bottom)), refname);
        }
    }
}

QDomElement NamedAreaManager::saveXML(QDomDocument& doc) const
{
    QDomElement element = doc.createElement("areaname");
    const QList<NamedArea> namedAreas = d->namedAreas.values();
    for (int i = 0; i < namedAreas.count(); ++i)
    {
        QDomElement e = doc.createElement("reference");
        QDomElement tabname = doc.createElement("tabname");
        tabname.appendChild(doc.createTextNode(namedAreas[i].sheet->sheetName()));
        e.appendChild(tabname);

        QDomElement refname = doc.createElement("refname");
        refname.appendChild(doc.createTextNode(namedAreas[i].name));
        e.appendChild(refname);

        QDomElement rect = doc.createElement("rect");
        rect.setAttribute("left-rect", (namedAreas[i].range).left());
        rect.setAttribute("right-rect",(namedAreas[i].range).right());
        rect.setAttribute("top-rect", (namedAreas[i].range).top());
        rect.setAttribute("bottom-rect", (namedAreas[i].range).bottom());
        e.appendChild(rect);
        element.appendChild(e);
    }
    return element;
}

#include "NamedAreaManager.moc"
