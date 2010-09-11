/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2009,2010 Sebastian Sauer <sebsauer@kdab.com>

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
 * Boston, MA 02110-1301, USA
*/

#include "workbook.h"
#include "sheet.h"
#include "excel.h"

#include <iostream>
#include <vector>

using namespace Swinder;

class Workbook::Private
{
public:
    KoStore* store;
    std::vector<Sheet*> sheets;
    QHash<PropertyType, QVariant> properties;
    std::map<std::pair<unsigned, QString>, QString> namedAreas;
    std::map<unsigned, QList<QRect> > filterRanges;
    int activeTab;
    bool passwordProtected;
    unsigned long passwd;
    std::vector<Format*> formats;
    MSO::OfficeArtDggContainer* dggContainer;
    QList<QColor> colorTable;
};

Workbook::Workbook(KoStore* store)
{
    d = new Workbook::Private();
    d->store = store;
    d->passwordProtected = false;
    d->activeTab = -1;
    d->passwd = 0; // password protection disabled
    d->dggContainer = 0;

    // initialize palette
    static const char *const default_palette[64-8] = { // default palette for all but the first 8 colors
        "#000000", "#ffffff", "#ff0000", "#00ff00", "#0000ff", "#ffff00", "#ff00ff",
        "#00ffff", "#800000", "#008000", "#000080", "#808000", "#800080", "#008080",
        "#c0c0c0", "#808080", "#9999ff", "#993366", "#ffffcc", "#ccffff", "#660066",
        "#ff8080", "#0066cc", "#ccccff", "#000080", "#ff00ff", "#ffff00", "#00ffff",
        "#800080", "#800000", "#008080", "#0000ff", "#00ccff", "#ccffff", "#ccffcc",
        "#ffff99", "#99ccff", "#ff99cc", "#cc99ff", "#ffcc99", "#3366ff", "#33cccc",
        "#99cc00", "#ffcc00", "#ff9900", "#ff6600", "#666699", "#969696", "#003366",
        "#339966", "#003300", "#333300", "#993300", "#993366", "#333399", "#333333",
    };
    for (int i = 0; i < 64 - 8; i++) {
        d->colorTable.append(QColor(default_palette[i]));
    }

}

Workbook::~Workbook()
{
    clear();
    for (unsigned i = 0; i < d->formats.size(); i++)
        delete d->formats[i];
    delete d;
}

KoStore* Workbook::store() const
{
    return d->store;
}

void Workbook::clear()
{
    // FIXME use iterator
    for (unsigned i = 0; i < sheetCount(); i++) {
        Sheet* s = sheet(i);
        delete s;
    }
    d->sheets.clear();
    delete d->dggContainer; d->dggContainer = 0;
}

bool Workbook::load(const char* filename)
{
    ExcelReader* reader = new ExcelReader();
    bool result = reader->load(this, filename);
    delete reader;
    return result;
}

void Workbook::appendSheet(Sheet* sheet)
{
    d->sheets.push_back(sheet);
}

unsigned Workbook::sheetCount() const
{
    return d->sheets.size();
}

Sheet* Workbook::sheet(unsigned index)
{
    if (index >= sheetCount()) return (Sheet*)0;
    return d->sheets[index];
}

bool Workbook::hasProperty(PropertyType type) const
{
    return d->properties.contains(type);
}

QVariant Workbook::property(PropertyType type, const QVariant &defaultValue) const
{
    return d->properties.contains(type) ? d->properties[ type ] : defaultValue;
}

void Workbook::setProperty(PropertyType type, const QVariant &value)
{
    d->properties[ type ] = value;
}

std::map<std::pair<unsigned, QString>, QString>& Workbook::namedAreas()
{
    return d->namedAreas;
}

void Workbook::setNamedArea(unsigned sheet, QString name, QString formula)
{
    d->namedAreas[std::make_pair(sheet, name)] = formula;
}

QList<QRect> Workbook::filterRanges(unsigned sheet) const
{
    return d->filterRanges[sheet];
}

QList<QRect> Workbook::filterRanges(const Sheet* sheet) const
{
    for (unsigned i = 0; i < d->sheets.size(); i++) {
        if(d->sheets[i] == sheet) return filterRanges(i);
    }
    return QList<QRect>();
}


void Workbook::addFilterRange(unsigned sheet, const QRect& range)
{
    d->filterRanges[sheet].push_back(range);
}

int Workbook::activeTab() const
{
    return d->activeTab;
}

void Workbook::setActiveTab(int tab)
{
    d->activeTab = tab;
}

bool Workbook::isPasswordProtected() const
{
    return d->passwordProtected;
}

void Workbook::setPasswordProtected(bool p)
{
    d->passwordProtected = p;
}

unsigned long Workbook::password() const
{
    return d->passwd;
}

void Workbook::setPassword(unsigned long hash)
{
    d->passwd = hash;
}

void Workbook::emitProgress(int value)
{
    emit sigProgress(value);
}

int Workbook::addFormat(const Format& format)
{
    d->formats.push_back(new Format(format));
    return d->formats.size()-1;
}

Format* Workbook::format(int index) const
{
    Q_ASSERT(index >= 0 && index < d->formats.size());
    return d->formats[index];
}

int Workbook::formatCount() const
{
    return d->formats.size();
}

void Workbook::setOfficeArtDggContainer(const MSO::OfficeArtDggContainer& dggContainer)
{
    d->dggContainer = new MSO::OfficeArtDggContainer(dggContainer);
}

MSO::OfficeArtDggContainer* Workbook::officeArtDggContainer() const
{
    return d->dggContainer;
}

void Workbook::setColorTable(const QList<QColor> &colorTable)
{
    d->colorTable = colorTable;
}

QColor Workbook::customColor(unsigned index) const
{
    if (index < unsigned(d->colorTable.size()))
        return d->colorTable[index];
    else
        return QColor();
}

QColor Workbook::color(unsigned index) const
{
    if ((index >= 8) && (index < 0x40))
        return customColor(index - 8);

    // FIXME the following colors depend on system color settings
    // 0x0040  system window text color for border lines
    // 0x0041  system window background color for pattern background
    // 0x7fff  system window text color for fonts
    if (index == 0x40) return QColor(0, 0, 0);
    if (index == 0x41) return QColor(255, 255, 255);
    if (index == 0x7fff) return QColor(0, 0, 0);

    // fallback: just "black"
    QColor color;

    // standard colors: black, white, red, green, blue,
    // yellow, magenta, cyan
    switch (index) {
    case 0:   color = QColor(0, 0, 0); break;
    case 1:   color = QColor(255, 255, 255); break;
    case 2:   color = QColor(255, 0, 0); break;
    case 3:   color = QColor(0, 255, 0); break;
    case 4:   color = QColor(0, 0, 255); break;
    case 5:   color = QColor(255, 255, 0); break;
    case 6:   color = QColor(255, 0, 255); break;
    case 7:   color = QColor(0, 255, 255); break;
    default:  break;
    }

    return color;
}

#ifdef SWINDER_XLS2RAW
void Workbook::dumpStats()
{
    for (unsigned i = 0; i < d->sheets.size(); i++) {
        printf("Sheet %u\n", i+1);
        d->sheets[i]->dumpStats();
    }
}
#endif
