/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2003 Philipp Müller <philipp.mueller@gmx.de>
   Copyright 1998, 1999 Torben Weis <weis@kde.org>

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
#include "PrintSettings.h"

// KSpread
#include "Global.h"
#include "Region.h"

// KOffice
#include <KoPageLayout.h>
#include <KoUnit.h>

using namespace KSpread;

class PrintSettings::Private
{
public:
    KoPageLayout pageLayout;
    bool printGrid              : 1;
    bool printCharts            : 1;
    bool printObjects           : 1;
    bool printGraphics          : 1;
    bool printCommentIndicator  : 1;
    bool printFormulaIndicator  : 1;
    PageOrder pageOrder;
    Region printRegion;

public:
    void calculatePageDimensions();
};

void PrintSettings::Private::calculatePageDimensions()
{
    if (pageLayout.format != KoPageFormat::CustomSize)
    {
        pageLayout.width =  MM_TO_POINT(KoPageFormat::width(pageLayout.format, pageLayout.orientation));
        pageLayout.height = MM_TO_POINT(KoPageFormat::height(pageLayout.format, pageLayout.orientation));
    }
}

PrintSettings::PrintSettings()
    : d(new Private)
{
    d->pageLayout = KoPageLayout::standardLayout();
    d->printGrid = false;
    d->printCharts = true;
    d->printObjects = true;
    d->printGraphics = true;
    d->printCommentIndicator = false;
    d->printFormulaIndicator = false;
    d->pageOrder = LeftToRight;
    d->printRegion = Region(1, 1, KS_colMax, KS_rowMax);
}

PrintSettings::PrintSettings(const PrintSettings& other)
    : d(new Private)
{
    d->pageLayout = other.d->pageLayout;
    d->printGrid = other.d->printGrid;
    d->printCharts = other.d->printCharts;
    d->printObjects = other.d->printObjects;
    d->printGraphics = other.d->printGraphics;
    d->printCommentIndicator = other.d->printCommentIndicator;
    d->printFormulaIndicator = other.d->printFormulaIndicator;
    d->pageOrder = other.d->pageOrder;
    d->printRegion = other.d->printRegion;
}

PrintSettings::~PrintSettings()
{
    delete d;
}

const KoPageLayout& PrintSettings::pageLayout() const
{
    return d->pageLayout;
}

void PrintSettings::setPageLayout(const KoPageLayout& pageLayout)
{
    d->pageLayout = pageLayout;
}

void PrintSettings::setPageFormat(KoPageFormat::Format format)
{
    d->pageLayout.format = format;
    d->calculatePageDimensions();
}

void PrintSettings::setPageOrientation(KoPageFormat::Orientation orientation)
{
    d->pageLayout.orientation = orientation;
    d->calculatePageDimensions();
}

double PrintSettings::printWidth() const
{
     return d->pageLayout.width - d->pageLayout.left - d->pageLayout.right;
}

double PrintSettings::printHeight() const
{
    return d->pageLayout.height - d->pageLayout.top - d->pageLayout.bottom;
}

PrintSettings::PageOrder PrintSettings::pageOrder() const
{
    return d->pageOrder;
}

void PrintSettings::setPageOrder(PageOrder order)
{
    d->pageOrder = order;
}

bool PrintSettings::printGrid() const
{
    return d->printGrid;
}

void PrintSettings::setPrintGrid(bool printGrid)
{
    d->printGrid = printGrid;
}

bool PrintSettings::printCharts() const
{
    return d->printCharts;
}

void PrintSettings::setPrintCharts(bool printCharts)
{
    d->printCharts = printCharts;
}

bool PrintSettings::printObjects() const
{
    return d->printObjects;
}

void PrintSettings::setPrintObjects(bool printObjects)
{
    d->printObjects = printObjects;
}

bool PrintSettings::printGraphics() const
{
    return d->printGraphics;
}

void PrintSettings::setPrintGraphics(bool printGraphics)
{
    d->printGraphics = printGraphics;
}

bool PrintSettings::printCommentIndicator() const
{
    return d->printCommentIndicator;
}

void PrintSettings::setPrintCommentIndicator(bool printCommentIndicator)
{
    d->printCommentIndicator = printCommentIndicator;
}

bool PrintSettings::printFormulaIndicator() const
{
    return d->printFormulaIndicator;
}

void PrintSettings::setPrintFormulaIndicator(bool printFormulaIndicator)
{
    d->printFormulaIndicator = printFormulaIndicator;
}

const KSpread::Region& PrintSettings::printRegion() const
{
    return d->printRegion;
}

void PrintSettings::setPrintRegion(const Region& region)
{
    d->printRegion = region;
}

void PrintSettings::addPrintRange(const QRect& range)
{
    d->printRegion.add(range);
}

void PrintSettings::removePrintRange(const QRect& range)
{
    d->printRegion.sub(range, 0);
}

void PrintSettings::operator=(const PrintSettings& other)
{
    d->pageLayout = other.d->pageLayout;
    d->printGrid = other.d->printGrid;
    d->printCharts = other.d->printCharts;
    d->printObjects = other.d->printObjects;
    d->printGraphics = other.d->printGraphics;
    d->printCommentIndicator = other.d->printCommentIndicator;
    d->printFormulaIndicator = other.d->printFormulaIndicator;
    d->pageOrder = other.d->pageOrder;
}

bool PrintSettings::operator==(const PrintSettings& other) const
{
    if (d->pageLayout != other.d->pageLayout)
        return false;
    if (d->printGrid != other.d->printGrid)
        return false;
    if (d->printCharts != other.d->printCharts)
        return false;
    if (d->printObjects != other.d->printObjects)
        return false;
    if (d->printGraphics != other.d->printGraphics)
        return false;
    if (d->printCommentIndicator != other.d->printCommentIndicator)
        return false;
    if (d->printFormulaIndicator != other.d->printFormulaIndicator)
        return false;
    if (d->pageOrder != other.d->pageOrder)
        return false;
    return true;
}
