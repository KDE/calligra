/* This file is part of the KDE project
   Copyright (C) 1998, 1999  Torben Weis <weis@kde.org>
   Copyright (C) 2000 - 2005 The KSpread Team <calligra-devel@kde.org>
   
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
#include "RowColumnFormat.h"

#include <float.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <KoXmlNS.h>
#include <KoUnit.h>

#include "SheetsDebug.h"
#include "CellStorage.h"
#include "Global.h"
#include "Map.h"
#include "Region.h"
#include "RowFormatStorage.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "Style.h"
#include "StyleManager.h"

using namespace std;
using namespace Calligra::Sheets;

/*****************************************************************************
 *
 * RowFormat
 *
 *****************************************************************************/

class Q_DECL_HIDDEN RowFormat::Private
{
public:
    Sheet*      sheet;
    RowFormat*  next;
    RowFormat*  prev;
    double      height;
    int         row;
    bool        hide : 1;
    bool        filtered : 1;
    bool        pageBreak : 1; // before row
};

RowFormat::RowFormat()
        : d(new Private)
{
    d->sheet    = 0;
    d->row      = 0;
    d->height   = 0.0;
    d->hide     = false;
    d->filtered = false;
    d->pageBreak = false;
    d->next     = 0;
    d->prev     = 0;
}

RowFormat::RowFormat(const RowFormat& other)
        : d(new Private(*other.d))
{
}

RowFormat::RowFormat(const RowFormatStorage *rows, int row)
        : d(new Private)
{
    d->sheet = rows->sheet();
    d->row = row;
    d->height = rows->rowHeight(row);
    d->hide = rows->isHidden(row);
    d->filtered = rows->isFiltered(row);
    d->pageBreak = rows->hasPageBreak(row);
    d->next = d->prev = 0;
}

RowFormat::~RowFormat()
{
    if (d->next)
        d->next->setPrevious(d->prev);
    if (d->prev)
        d->prev->setNext(d->next);
    delete d;
}

void RowFormat::setSheet(Sheet* sheet)
{
    d->sheet = sheet;
}

void RowFormat::setHeight(double height)
{
    // avoid unnecessary updates
    if (qAbs(height - this->height()) < DBL_EPSILON)
        return;

    // default RowFormat?
    if (!d->sheet) {
        d->height = height;
        return;
    }

    // Raise document height by new height and lower it by old height.
    if (!isHidden() && !isFiltered())
        d->sheet->adjustDocumentHeight(height - d->height);

    d->height = height;

    d->sheet->print()->updateVerticalPageParameters(row());
}

double RowFormat::height() const
{
    return d->height;
}

double RowFormat::visibleHeight() const
{
    if (d->hide || d->filtered)
        return 0.0;
    return d->height;
}

QDomElement RowFormat::save(QDomDocument& doc, int yshift) const
{
    Q_ASSERT(d->sheet);
    QDomElement row = doc.createElement("row");
    row.setAttribute("height", QString::number(d->height));
    row.setAttribute("row", QString::number(d->row - yshift));
    if (d->hide)
        row.setAttribute("hide", QString::number((int) d->hide));

    const Style style = d->sheet->cellStorage()->style(QRect(1, d->row, KS_colMax, 1));
    if (!style.isEmpty()) {
        debugSheetsODF << "saving cell style of row" << d->row;
        QDomElement format;
        style.saveXML(doc, format, d->sheet->map()->styleManager());
        row.appendChild(format);
    }

    return row;
}

bool RowFormat::load(const KoXmlElement & row, int yshift, Paste::Mode mode)
{
    Q_ASSERT(d->sheet);
    bool ok;

    d->row = row.attribute("row").toInt(&ok) + yshift;
    if (!ok)
        return false;

    if (row.hasAttribute("height")) {
        if (d->sheet->map()->syntaxVersion() < 1) //compatibility with old format - was in millimeter
            d->height = qRound(MM_TO_POINT(row.attribute("height").toDouble(&ok)));
        else
            d->height = row.attribute("height").toDouble(&ok);

        if (!ok) return false;
    }

    // Validation
    if (d->height < 0) {
        debugSheets << "Value height=" << d->height << " out of range";
        return false;
    }
    if (d->row < 1 || d->row > KS_rowMax) {
        debugSheets << "Value row=" << d->row << " out of range";
        return false;
    }

    if (row.hasAttribute("hide")) {
        setHidden((int) row.attribute("hide").toInt(&ok));
        if (!ok)
            return false;
    }

    KoXmlElement f(row.namedItem("format").toElement());

    if (!f.isNull() && (mode == Paste::Normal || mode == Paste::Format || mode == Paste::NoBorder)) {
        Style style;
        if (!style.loadXML(f, mode))
            return false;
        d->sheet->cellStorage()->setStyle(Region(QRect(1, d->row, KS_colMax, 1)), style);
        return true;
    }

    return true;
}

int RowFormat::row() const
{
    return d->row;
}

void RowFormat::setRow(int row)
{
    d->row = row;
}

RowFormat* RowFormat::next() const
{
    return d->next;
}

RowFormat* RowFormat::previous() const
{
    return d->prev;
}

void RowFormat::setNext(RowFormat* next)
{
    d->next = next;
}

void RowFormat::setPrevious(RowFormat* prev)
{
    d->prev = prev;
}

void RowFormat::setHidden(bool _hide, bool repaint)
{
    Q_UNUSED(repaint);
    Q_ASSERT(d->sheet);
    if (_hide != d->hide) { // only if we change the status
        if (_hide) {
            // Lower maximum size by height of row
            d->sheet->adjustDocumentHeight(- height());
            d->hide = _hide; //hide must be set after we requested the height
        } else {
            // Rise maximum size by height of row
            d->hide = _hide; //unhide must be set before we request the height
            d->sheet->adjustDocumentHeight(height());
        }
    }
}

bool RowFormat::isHidden() const
{
    return d->hide;
}

void RowFormat::setFiltered(bool filtered)
{
    d->filtered = filtered;
}

bool RowFormat::isFiltered() const
{
    return d->filtered;
}

bool RowFormat::isHiddenOrFiltered() const
{
    return d->hide || d->filtered;
}

bool RowFormat::isDefault() const
{
    return !d->sheet;
}

void RowFormat::setPageBreak(bool enable)
{
    d->pageBreak = enable;
}

bool RowFormat::hasPageBreak() const
{
    return d->pageBreak;
}

bool RowFormat::operator==(const RowFormat& other) const
{
    // NOTE Stefan: Don't compare sheet and cell.
    if (d->height != other.d->height)
        return false;
    if (d->hide != other.d->hide)
        return false;
    if (d->filtered != other.d->filtered)
        return false;
    if (d->pageBreak != other.d->pageBreak) {
        return false;
    }
    return true;
}


/*****************************************************************************
 *
 * ColumnFormat
 *
 *****************************************************************************/

class Q_DECL_HIDDEN ColumnFormat::Private
{
public:
    Sheet*          sheet;
    ColumnFormat*   next;
    ColumnFormat*   prev;
    double          width;
    int             column;
    bool            hide : 1;
    bool            filtered : 1;
    bool            pageBreak : 1; // before column
};

ColumnFormat::ColumnFormat()
        : d(new Private)
{
    d->sheet    = 0;
    d->column   = 0;
    d->width    = 0.0;
    d->hide     = false;
    d->filtered = false;
    d->pageBreak = false;
    d->next     = 0;
    d->prev     = 0;
}

ColumnFormat::ColumnFormat(const ColumnFormat& other)
        : d(new Private(*other.d))
{
}

ColumnFormat::~ColumnFormat()
{
    if (d->next)
        d->next->setPrevious(d->prev);
    if (d->prev)
        d->prev->setNext(d->next);
    delete d;
}

void ColumnFormat::setSheet(Sheet* sheet)
{
    d->sheet = sheet;
}

void ColumnFormat::setWidth(double width)
{
    // avoid unnecessary updates
    if (qAbs(width - this->width()) < DBL_EPSILON)
        return;

    // default ColumnFormat?
    if (!d->sheet) {
        d->width = width;
        return;
    }

    // Raise document width by new width and lower it by old width.
    if (!isHidden() && !isFiltered())
        d->sheet->adjustDocumentWidth(width - d->width);

    d->width = width;

    d->sheet->print()->updateHorizontalPageParameters(column());
}

double ColumnFormat::width() const
{
    return d->width;
}

double ColumnFormat::visibleWidth() const
{
    if (d->hide || d->filtered)
        return 0.0;
    return d->width;
}

QDomElement ColumnFormat::save(QDomDocument& doc, int xshift) const
{
    Q_ASSERT(d->sheet);
    QDomElement col(doc.createElement("column"));
    col.setAttribute("width", QString::number(d->width));
    col.setAttribute("column", QString::number(d->column - xshift));

    if (d->hide)
        col.setAttribute("hide", QString::number((int) d->hide));

    const Style style = d->sheet->cellStorage()->style(QRect(d->column, 1, 1, KS_rowMax));
    if (!style.isEmpty()) {
        debugSheetsODF << "saving cell style of column" << d->column;
        QDomElement format(doc.createElement("format"));
        style.saveXML(doc, format, d->sheet->map()->styleManager());
        col.appendChild(format);
    }

    return col;
}

bool ColumnFormat::load(const KoXmlElement & col, int xshift, Paste::Mode mode)
{
    Q_ASSERT(d->sheet);
    bool ok;
    if (col.hasAttribute("width")) {
        if (d->sheet->map()->syntaxVersion() < 1) //combatibility to old format - was in millimeter
            d->width = qRound(MM_TO_POINT(col.attribute("width").toDouble(&ok)));
        else
            d->width = col.attribute("width").toDouble(&ok);

        if (!ok)
            return false;
    }

    d->column = col.attribute("column").toInt(&ok) + xshift;

    if (!ok)
        return false;

    // Validation
    if (d->width < 0) {
        debugSheets << "Value width=" << d->width << " out of range";
        return false;
    }
    if (d->column < 1 || d->column > KS_colMax) {
        debugSheets << "Value col=" << d->column << " out of range";
        return false;
    }
    if (col.hasAttribute("hide")) {
        setHidden((int) col.attribute("hide").toInt(&ok));
        if (!ok)
            return false;
    }

    KoXmlElement f(col.namedItem("format").toElement());

    if (!f.isNull() && (mode == Paste::Normal || mode == Paste::Format || mode == Paste::NoBorder)) {
        Style style;
        if (!style.loadXML(f, mode))
            return false;
        d->sheet->cellStorage()->setStyle(Region(QRect(d->column, 1, 1, KS_rowMax)), style);
        return true;
    }

    return true;
}

int ColumnFormat::column() const
{
    return d->column;
}

void ColumnFormat::setColumn(int column)
{
    d->column = column;
}

ColumnFormat* ColumnFormat::next() const
{
    return d->next;
}

ColumnFormat* ColumnFormat::previous() const
{
    return d->prev;
}

void ColumnFormat::setNext(ColumnFormat* next)
{
    d->next = next;
}

void ColumnFormat::setPrevious(ColumnFormat* prev)
{
    d->prev = prev;
}

void ColumnFormat::setHidden(bool _hide)
{
    Q_ASSERT(d->sheet);
    if (_hide != d->hide) { // only if we change the status
        if (_hide) {
            // Lower maximum size by width of column
            d->sheet->adjustDocumentWidth(- width());
            d->hide = _hide; //hide must be set after we requested the width
        } else {
            // Rise maximum size by width of column
            d->hide = _hide; //unhide must be set before we request the width
            d->sheet->adjustDocumentWidth(width());
        }
    }
}

bool ColumnFormat::isHidden() const
{
    return d->hide;
}

void ColumnFormat::setFiltered(bool filtered)
{
    d->filtered = filtered;
}

bool ColumnFormat::isFiltered() const
{
    return d->filtered;
}

bool ColumnFormat::isHiddenOrFiltered() const
{
    return d->hide || d->filtered;
}

bool ColumnFormat::isDefault() const
{
    return !d->sheet;
}

void ColumnFormat::setPageBreak(bool enable)
{
    d->pageBreak = enable;
}

bool ColumnFormat::hasPageBreak() const
{
    return d->pageBreak;
}

bool ColumnFormat::operator==(const ColumnFormat& other) const
{
    // NOTE Stefan: Don't compare sheet and cell.
    if (d->width != other.d->width)
        return false;
    if (d->hide != other.d->hide)
        return false;
    if (d->filtered != other.d->filtered)
        return false;
    if (d->pageBreak != other.d->pageBreak) {
        return false;
    }
    return true;
}
