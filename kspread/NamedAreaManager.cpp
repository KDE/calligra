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
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

// KSpread
#include "CellStorage.h"
#include "FormulaStorage.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"
#include "Util.h"

using namespace KSpread;

struct NamedArea {
    QString name;
    Sheet* sheet;
    QRect range;
};

class NamedAreaManager::Private
{
public:
    const Map* map;
    QHash<QString, NamedArea> namedAreas;
};

NamedAreaManager::NamedAreaManager(const Map* map)
        : d(new Private)
{
    d->map = map;
    connect(this, SIGNAL(namedAreaAdded(const QString&)),
            this, SIGNAL(namedAreaModified(const QString&)));
    connect(this, SIGNAL(namedAreaRemoved(const QString&)),
            this, SIGNAL(namedAreaModified(const QString&)));
}

NamedAreaManager::~NamedAreaManager()
{
    delete d;
}

void NamedAreaManager::insert(const Region& region, const QString& name)
{
    // NOTE Stefan: Only contiguous regions are supported (OpenDocument compatibility).
    NamedArea namedArea;
    namedArea.range = region.lastRange();
    namedArea.sheet = region.lastSheet();
    namedArea.name = name;
    namedArea.sheet->cellStorage()->setNamedArea(Region(region.lastRange(), region.lastSheet()), name);
    d->namedAreas[name] = namedArea;
    emit namedAreaAdded(name);
}

void NamedAreaManager::remove(const QString& name)
{
    if (!d->namedAreas.contains(name))
        return;
    NamedArea namedArea = d->namedAreas.value(name);
    namedArea.sheet->cellStorage()->setNamedArea(Region(namedArea.range, namedArea.sheet), QString());
    d->namedAreas.remove(name);
    emit namedAreaRemoved(name);
    const QList<Sheet*> sheets = namedArea.sheet->map()->sheetList();
    foreach(Sheet* sheet, sheets) {
        const QString tmp = '\'' + name + '\'';
        const FormulaStorage* const storage = sheet->formulaStorage();
        for (int c = 0; c < storage->count(); ++c) {
            if (storage->data(c).expression().contains(tmp)) {
                Cell(sheet, storage->col(c), storage->row(c)).makeFormula();
            }
        }
    }
}

void NamedAreaManager::remove(Sheet* sheet)
{
    const QList<NamedArea> namedAreas = d->namedAreas.values();
    for (int i = 0; i < namedAreas.count(); ++i) {
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

void NamedAreaManager::regionChanged(const Region& region)
{
    Sheet* sheet;
    QList< QPair<QRectF, QString> > namedAreas;
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
        sheet = (*it)->sheet();
        namedAreas = sheet->cellStorage()->namedAreas(Region((*it)->rect(), sheet));
        for (int j = 0; j < namedAreas.count(); ++j) {
            d->namedAreas[namedAreas[j].second].range = namedAreas[j].first.toRect();
            emit namedAreaModified(namedAreas[j].second);
        }
    }
}

void NamedAreaManager::updateAllNamedAreas()
{
    QList< QPair<QRectF, QString> > namedAreas;
    const QRect rect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax));
    const QList<Sheet*> sheets = d->map->sheetList();
    for (int i = 0; i < sheets.count(); ++i) {
        namedAreas = sheets[i]->cellStorage()->namedAreas(Region(rect, sheets[i]));
        for (int j = 0; j < namedAreas.count(); ++j) {
            d->namedAreas[namedAreas[j].second].range = namedAreas[j].first.toRect();
            emit namedAreaModified(namedAreas[j].second);
        }
    }
}

void NamedAreaManager::loadOdf(const KoXmlElement& body)
{
    KoXmlNode namedAreas = KoXml::namedItemNS(body, KoXmlNS::table, "named-expressions");
    if (!namedAreas.isNull()) {
        kDebug(36003) << "Loading named areas...";
        KoXmlElement element;
        forEachElement(element, namedAreas) {
            if (element.namespaceURI() != KoXmlNS::table)
                continue;
            if (element.localName() == "named-range") {
                if (!element.hasAttributeNS(KoXmlNS::table, "name"))
                    continue;
                if (!element.hasAttributeNS(KoXmlNS::table, "cell-range-address"))
                    continue;

                // TODO: what is: table:base-cell-address
                const QString base = element.attributeNS(KoXmlNS::table, "base-cell-address", QString());

                // Handle the case where the table:base-cell-address does contain the referenced sheetname
                // while it's missing in the table:cell-range-address. See bug #194386 for an example.
                Sheet* fallbackSheet = 0;
                if (!base.isEmpty()) {
                    Region region(Region::loadOdf(base), d->map);
                    fallbackSheet = region.lastSheet();
                }
                
                const QString name = element.attributeNS(KoXmlNS::table, "name", QString());
                const QString range = element.attributeNS(KoXmlNS::table, "cell-range-address", QString());
                kDebug(36003) << "Named area found, name:" << name << ", area:" << range;

                Region region(Region::loadOdf(range), d->map, fallbackSheet);
                if (!region.isValid() || region.lastSheet()) {
                    kDebug(36003) << "invalid area";
                    continue;
                }

                insert(region, name);
            } else if (element.localName() == "named-expression") {
                kDebug(36003) << "Named expression found.";
                // TODO
            }
        }
    }
}

void NamedAreaManager::saveOdf(KoXmlWriter& xmlWriter) const
{
    if (d->namedAreas.isEmpty())
        return;
    Region region;
    xmlWriter.startElement("table:named-expressions");
    const QList<NamedArea> namedAreas = d->namedAreas.values();
    for (int i = 0; i < namedAreas.count(); ++i) {
        region = Region(namedAreas[i].range, namedAreas[i].sheet);
        xmlWriter.startElement("table:named-range");
        xmlWriter.addAttribute("table:name", namedAreas[i].name);
        xmlWriter.addAttribute("table:base-cell-address", Region(1, 1, namedAreas[i].sheet).saveOdf());
        xmlWriter.addAttribute("table:cell-range-address", region.saveOdf());
        xmlWriter.endElement();
    }
    xmlWriter.endElement();
}

void NamedAreaManager::loadXML(const KoXmlElement& parent)
{
    KoXmlElement element;
    forEachElement(element, parent) {
        if (element.tagName() == "reference") {
            Sheet* sheet = 0;
            QString tabname;
            QString refname;
            int left = 0;
            int right = 0;
            int top = 0;
            int bottom = 0;
            KoXmlElement sheetName = element.namedItem("tabname").toElement();
            if (!sheetName.isNull())
                sheet = d->map->findSheet(sheetName.text());
            if (!sheet)
                continue;
            KoXmlElement referenceName = element.namedItem("refname").toElement();
            if (!referenceName.isNull())
                refname = referenceName.text();
            KoXmlElement rect = element.namedItem("rect").toElement();
            if (!rect.isNull()) {
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
            insert(Region(QRect(QPoint(left, top), QPoint(right, bottom)), sheet), refname);
        }
    }
}

QDomElement NamedAreaManager::saveXML(QDomDocument& doc) const
{
    QDomElement element = doc.createElement("areaname");
    const QList<NamedArea> namedAreas = d->namedAreas.values();
    for (int i = 0; i < namedAreas.count(); ++i) {
        QDomElement e = doc.createElement("reference");
        QDomElement tabname = doc.createElement("tabname");
        tabname.appendChild(doc.createTextNode(namedAreas[i].sheet->sheetName()));
        e.appendChild(tabname);

        QDomElement refname = doc.createElement("refname");
        refname.appendChild(doc.createTextNode(namedAreas[i].name));
        e.appendChild(refname);

        QDomElement rect = doc.createElement("rect");
        rect.setAttribute("left-rect", (namedAreas[i].range).left());
        rect.setAttribute("right-rect", (namedAreas[i].range).right());
        rect.setAttribute("top-rect", (namedAreas[i].range).top());
        rect.setAttribute("bottom-rect", (namedAreas[i].range).bottom());
        e.appendChild(rect);
        element.appendChild(e);
    }
    return element;
}

#include "NamedAreaManager.moc"
