/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
#include "TableShape.h"

#include "TablePageManager.h"

#include <QPainter>

#include <KoShapeContainer.h>
#include <KoXmlNS.h>

#include <SheetsDebug.h>
#include <CellView.h>
#include <Damages.h>
#include <Condition.h>
#include <Map.h>
#include <PrintSettings.h>
#include <Region.h>
#include <RowColumnFormat.h>
#include <RowFormatStorage.h>
#include <Sheet.h>
#include <SheetView.h>
#include <Value.h>
#include <odf/SheetsOdf.h>

using namespace Calligra::Sheets;

class TableShape::Private
{
public:
    int         columns;
    int         rows;
    SheetView*  sheetView;
    bool        isMaster;
    TablePageManager* pageManager;

public:
    void adjustColumnDimensions(Sheet* sheet, double factor);
    void adjustRowDimensions(Sheet* sheet, double factor);
};

void TableShape::Private::adjustColumnDimensions(Sheet* sheet, double factor)
{
    for (int col = 1; col <= columns; ++col) {
        ColumnFormat* const columnFormat = sheet->nonDefaultColumnFormat(col);
        columnFormat->setWidth(columnFormat->width() * factor);
    }
}

void TableShape::Private::adjustRowDimensions(Sheet* sheet, double factor)
{
    for (int row = 1; row <= rows; ++row) {
        sheet->rowFormats()->setRowHeight(row, row, sheet->rowFormats()->rowHeight(row) * factor);
    }
}



TableShape::TableShape(int columns, int rows)
        : d(new Private)
{
    setObjectName(QLatin1String("TableShape"));
    d->columns = columns;
    d->rows = rows;
    d->sheetView = 0;
    d->isMaster = false;
    d->pageManager = 0;
}

TableShape::~TableShape()
{
    delete d->pageManager;
    delete d->sheetView;
    if (KoShape::userData()) {
        map()->removeSheet(qobject_cast<Sheet*>(KoShape::userData())); // declare the sheet as deleted
    }
    delete d;
}

int TableShape::columns() const
{
    return d->columns;
}

int TableShape::rows() const
{
    return d->rows;
}

void TableShape::setColumns(int columns)
{
    Q_ASSERT(columns > 0);
    if(!sheet())
        return;
    const double factor = (double) d->columns / columns;
    d->columns = columns;
    d->adjustColumnDimensions(qobject_cast<Sheet*>(KoShape::userData()), factor);
    setVisibleCellRange(QRect(1, 1, d->columns, d->rows));
    d->sheetView->invalidate();
    if (!d->pageManager) {
        return;
    }
    PrintSettings settings = *sheet()->printSettings();
    settings.setPrintRegion(Region(1, 1, d->columns, d->rows, sheet()));
    d->pageManager->setPrintSettings(settings);
}

void TableShape::setRows(int rows)
{
    Q_ASSERT(rows > 0);
    if(!sheet())
        return;
    const double factor = (double) d->rows / rows;
    d->rows = rows;
    d->adjustRowDimensions(qobject_cast<Sheet*>(KoShape::userData()), factor);
    setVisibleCellRange(QRect(1, 1, d->columns, d->rows));
    d->sheetView->invalidate();
    if (!d->pageManager) {
        return;
    }
    PrintSettings settings = *sheet()->printSettings();
    settings.setPrintRegion(Region(1, 1, d->columns, d->rows, sheet()));
    d->pageManager->setPrintSettings(settings);
}

void TableShape::paint(QPainter& painter, const KoViewConverter& converter, KoShapePaintingContext &)
{
#ifndef NDEBUG
    if (KoShape::parent()) {
        debugSheets << KoShape::parent()->name() <<  KoShape::parent()->shapeId() << KoShape::parent()->boundingRect();
    }
#endif
    const QRectF paintRect = QRectF(QPointF(0.0, 0.0), size());

    applyConversion(painter, converter);
    painter.setClipRect(paintRect, Qt::IntersectClip);

    // painting cell contents
    d->sheetView->setViewConverter(&converter);
    d->sheetView->paintCells(painter, paintRect, QPointF(0.0, 0.0));
}

bool TableShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    //debugSheets << "LOADING TABLE SHAPE";
    if (sheet() && element.namespaceURI() == KoXmlNS::table && element.localName() == "table") {
        if (!Odf::loadTableShape(sheet(), element, context)) return false;

        const QRect usedArea = sheet()->usedArea();
        d->columns = usedArea.width();
        d->rows = usedArea.height();

        QSizeF size(0.0, 0.0);
        for (int col = 1; col <= d->columns; ++col) {
            size.rwidth() += sheet()->columnFormat(col)->visibleWidth();
        }
        size.rheight() = sheet()->rowFormats()->totalVisibleRowHeight(1, d->rows);
        KoShape::setSize(size);
        return true;
    }
    return false;
}

void TableShape::saveOdf(KoShapeSavingContext & context) const
{
    if (!sheet())
        return;
    Odf::saveTableShape(sheet(), context);
}

void TableShape::setMap(Map *map)
{
    if (map == 0)
        return;
    Sheet* const sheet = map->addNewSheet();
    d->sheetView = new SheetView(sheet);
    KoShape::setUserData(sheet);
    d->isMaster = true;
    setVisibleCellRange(QRect(1, 1, d->columns, d->rows));

    connect(map, SIGNAL(damagesFlushed(QList<Damage*>)),
            this, SLOT(handleDamages(QList<Damage*>)));

    // Initialize the size using the default column/row dimensions.
    QSize size;
    for (int col = 1; col <= d->columns; ++col) {
        size.rwidth() += sheet->columnFormat(col)->visibleWidth();
    }
    size.rheight() = sheet->rowFormats()->totalVisibleRowHeight(1, d->rows);
    KoShape::setSize(size);
}

void TableShape::setSize(const QSizeF& newSize)
{
    const QSizeF oldSize = size();
    if (oldSize == newSize)
        return;

    QSizeF size2 = oldSize;
    const qreal cellWidth = map()->defaultColumnFormat()->width();
    const qreal cellHeight = map()->defaultRowFormat()->height();

    // Note that the following four variables can also be negative
    const qreal dx = newSize.width() - oldSize.width();
    const qreal dy = newSize.height() - oldSize.height();
    int numAddedCols = 0;
    int numAddedRows = 0;

    if (qAbs(dx) >= cellWidth) {
        numAddedCols = int(dx / cellWidth);
        size2.rwidth() += cellWidth * numAddedCols;
    }
    if (qAbs(dy) >= cellHeight) {
        numAddedRows = int(dy / cellHeight);
        size2.rheight() += cellHeight * numAddedRows;
    }
    if (qAbs(dx) >= cellWidth || qAbs(dy) >= cellHeight) {
        d->columns += numAddedCols;
        d->rows += numAddedRows;
        setVisibleCellRange(QRect(1, 1, d->columns, d->rows));
        d->sheetView->invalidate();
        KoShape::setSize(size2);
    }
}

Map* TableShape::map() const
{
    return qobject_cast<Sheet*>(KoShape::userData())->map();
}

Sheet* TableShape::sheet() const
{
    return qobject_cast<Sheet*>(KoShape::userData());
}

SheetView* TableShape::sheetView() const
{
    return d->sheetView;
}

void TableShape::setSheet(const QString& sheetName)
{
    Sheet* const sheet = map()->findSheet(sheetName);
    if (! sheet)
        return;
    delete d->sheetView;
    d->sheetView = new SheetView(sheet);
    KoShape::setUserData(sheet);
    setColumns(d->columns);
    setRows(d->rows);
    setVisibleCellRange(QRect(1, 1, d->columns, d->rows));
    update();
}

void TableShape::setVisibleCellRange(const QRect& cellRange)
{
    Q_ASSERT(KoShape::userData());
    if (!d->sheetView) {
        d->sheetView = new SheetView(sheet());
    }
    d->sheetView->setPaintCellRange(cellRange & QRect(1, 1, d->columns, d->rows));
}

void TableShape::shapeChanged(ChangeType type, KoShape *shape)
{
    Q_UNUSED(shape);
    // If this is a master table shape, the parent changed and we have no parent yet...
    if (d->isMaster && type == ParentChanged && !d->pageManager) {
        d->pageManager = new TablePageManager(this);
        return;
    }
    // Not the master table shape? Not embedded into a container?
    if (!d->isMaster || !KoShape::parent()) {
        return;
    }
    // Not the changes, we want to react on?
    if (type != SizeChanged) {
        return;
    }
    d->pageManager->layoutPages();
}

void TableShape::handleDamages(const QList<Damage*>& damages)
{
    QList<Damage*>::ConstIterator end(damages.end());
    for (QList<Damage*>::ConstIterator it = damages.begin(); it != end; ++it) {
        Damage* damage = *it;
        if (!damage) continue;

        if (damage->type() == Damage::Cell) {
            CellDamage* cellDamage = static_cast<CellDamage*>(damage);
            const Region region = cellDamage->region();

            if (cellDamage->changes() & CellDamage::Appearance)
                d->sheetView->invalidateRegion(region);
            continue;
        }

        if (damage->type() == Damage::Sheet) {
            SheetDamage* sheetDamage = static_cast<SheetDamage*>(damage);

            if (sheetDamage->changes() & SheetDamage::PropertiesChanged)
                d->sheetView->invalidate();
            continue;
        }
    }

    update();
}
