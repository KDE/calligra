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

#include <QPainter>

#include <kdebug.h>

#include <KoOdfLoadingContext.h>
#include <KoShapeContainer.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlNS.h>

#include <CellView.h>
#include <Damages.h>
#include <GenValidationStyle.h>
#include <Map.h>
#include <OdfLoadingContext.h>
#include <Region.h>
#include <RowColumnFormat.h>
#include <Sheet.h>
#include <SheetView.h>
#include <StyleManager.h>

using namespace KSpread;

class TableShape::Private
{
public:
    int         columns;
    int         rows;
    Sheet*      sheet; // owned by "TableMap" data center
    SheetView*  sheetView;

public:
    void adjustColumnDimensions( double factor );
    void adjustRowDimensions( double factor );
};

void TableShape::Private::adjustColumnDimensions( double factor )
{
    for (int col = 1; col <= columns; ++col) {
        ColumnFormat* const columnFormat = sheet->nonDefaultColumnFormat(col);
        columnFormat->setWidth(columnFormat->width() * factor);
    }
}

void TableShape::Private::adjustRowDimensions( double factor )
{
    for (int row = 1; row <= rows; ++row) {
        RowFormat* const rowFormat = sheet->nonDefaultRowFormat(row);
        rowFormat->setHeight(rowFormat->height() * factor);
    }
}



TableShape::TableShape( int columns, int rows )
    : d( new Private )
{
    setObjectName("TableShape");
    d->columns = columns;
    d->rows = rows;
    d->sheet = 0;
    d->sheetView = 0;
}

TableShape::~TableShape()
{
    delete d->sheetView;
    if (d->sheet) {
        d->sheet->map()->removeSheet(d->sheet); // declare the sheet as deleted
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

void TableShape::setColumns( int columns )
{
    Q_ASSERT( columns > 0 );
    const double factor = (double) d->columns / columns;
    d->columns = columns;
    d->adjustColumnDimensions( factor );
    d->sheetView->invalidate();
}

void TableShape::setRows( int rows )
{
    Q_ASSERT( rows > 0 );
    const double factor = (double) d->rows / rows;
    d->rows = rows;
    d->adjustRowDimensions( factor );
    d->sheetView->invalidate();
}

void TableShape::paint( QPainter& painter, const KoViewConverter& converter )
{
    if (KoShape::parent()) {
        kDebug() << KoShape::parent()->name() <<  KoShape::parent()->shapeId() << KoShape::parent()->boundingRect();
    }
    const QRectF paintRect = QRectF( QPointF( 0.0, 0.0 ), size() );

    applyConversion( painter, converter );
    painter.setClipRect( paintRect, Qt::IntersectClip );

    // painting cell contents
    d->sheetView->setPaintCellRange( QRect( 1, 1, d->columns, d->rows ) );
    d->sheetView->setPaintDevice( painter.device() );
    d->sheetView->setViewConverter( &converter );
    d->sheetView->paintCells( 0 /*paintDevice*/, painter, paintRect, QPointF( 0.0, 0.0 ) );
}

bool TableShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    kDebug() << "LOADING TABLE SHAPE";
    if (element.namespaceURI() == KoXmlNS::table && element.localName() == "table") {
        // pre-load auto styles
        KoOdfLoadingContext& odfContext = context.odfLoadingContext();
        OdfLoadingContext tableContext(odfContext);
        QHash<QString, Conditions> conditionalStyles;
        Styles autoStyles = sheet()->map()->styleManager()->loadOasisAutoStyles(odfContext.stylesReader(), conditionalStyles);

        if (!element.attributeNS(KoXmlNS::table, "name", QString()).isEmpty()) {
            sheet()->setSheetName(element.attributeNS(KoXmlNS::table, "name", QString()), true);
        }
        const bool result = sheet()->loadOasis(element, tableContext, autoStyles, conditionalStyles);

        // delete any styles which were not used
        sheet()->map()->styleManager()->releaseUnusedAutoStyles(autoStyles);

        if (!result) {
            return false;
        }

        const QRect usedArea = sheet()->usedArea();
        d->columns = usedArea.width();
        d->rows = usedArea.height();

        QSizeF size(0.0, 0.0);
        for (int col = 1; col <= d->columns; ++col) {
            size.rwidth() += sheet()->columnFormat(col)->visibleWidth();
        }
        for (int row = 1; row <= d->rows; ++row) {
            size.rheight() += sheet()->rowFormat(row)->visibleHeight();
        }
        KoShape::setSize(size);
        return true;
    }
    return false;
}

void TableShape::saveOdf( KoShapeSavingContext & context ) const
{
    const Map* map = sheet()->map();
    // Saving the custom cell styles including the default cell style.
    map->styleManager()->saveOasis(context.mainStyles());

    // Saving the default column style
    KoGenStyle defaultColumnStyle(KoGenStyle::StyleTableColumn, "table-column");
    defaultColumnStyle.addPropertyPt("style:column-width", map->defaultColumnFormat()->width());
    defaultColumnStyle.setDefaultStyle(true);
    context.mainStyles().lookup(defaultColumnStyle, "Default", KoGenStyles::DontForceNumbering);

    // Saving the default row style
    KoGenStyle defaultRowStyle(KoGenStyle::StyleTableRow, "table-row");
    defaultRowStyle.addPropertyPt("style:row-height", map->defaultRowFormat()->height());
    defaultRowStyle.setDefaultStyle(true);
    context.mainStyles().lookup(defaultRowStyle, "Default", KoGenStyles::DontForceNumbering);

    GenValidationStyles valStyle;
    sheet()->saveOasis(context, valStyle);
    valStyle.writeStyle(context.xmlWriter());
}

void TableShape::init(QMap<QString, KoDataCenter*> dataCenterMap)
{
    Map* map = dynamic_cast<Map*>(dataCenterMap["TableMap"]);
    Q_CHECK_PTR(map);
    d->sheet = map->addNewSheet();
    d->sheetView = new SheetView(d->sheet);

    connect(map, SIGNAL(damagesFlushed(const QList<Damage*>&)),
            this, SLOT(handleDamages(const QList<Damage*>&)));

    // Initialize the size using the default column/row dimensions.
    QSize size;
    for (int col = 1; col <= d->columns; ++col) {
        size.rwidth() += sheet()->columnFormat(col)->visibleWidth();
    }
    for (int row = 1; row <= d->rows; ++row) {
        size.rheight() += sheet()->rowFormat(row)->visibleHeight();
    }
    KoShape::setSize(size);
}

void TableShape::setSize( const QSizeF& newSize )
{
    const QSizeF oldSize = size();
    if ( oldSize == newSize )
        return;

    QSizeF size2 = oldSize;
    const qreal cellWidth = d->sheet->map()->defaultColumnFormat()->width();
    const qreal cellHeight = d->sheet->map()->defaultRowFormat()->height();

    // Note that the following four variables can also be negative
    const qreal dx = newSize.width() - oldSize.width();
    const qreal dy = newSize.height() - oldSize.height();
    int numAddedCols = 0;
    int numAddedRows = 0;

    if( qAbs(dx) >= cellWidth ) {
        numAddedCols = int( dx / cellWidth );
        size2.rwidth() += cellWidth * numAddedCols;
    }
    if ( qAbs(dy) >= cellHeight ) {
        numAddedRows = int( dy / cellHeight );
        size2.rheight() += cellHeight * numAddedRows;
    }
    if ( qAbs(dx) >= cellWidth || qAbs(dy) >= cellHeight ) {
        d->columns += numAddedCols;
        d->rows += numAddedRows;
        d->sheetView->invalidate();
        KoShape::setSize( size2 );
    }
}

Map* TableShape::map() const
{
    return d->sheet->map();
}

Sheet* TableShape::sheet() const
{
    return d->sheet;
}

void TableShape::setSheet(const QString& sheetName)
{
    Sheet* sheet = d->sheet->map()->findSheet(sheetName);
    if ( ! sheet )
        return;
    delete d->sheetView;
    d->sheetView = new SheetView(sheet);
    setColumns(d->columns);
    setRows(d->rows);
    update();
}

void TableShape::handleDamages( const QList<Damage*>& damages )
{
    QList<Damage*>::ConstIterator end(damages.end());
    for( QList<Damage*>::ConstIterator it = damages.begin(); it != end; ++it )
    {
        Damage* damage = *it;
        if( !damage ) continue;

        if( damage->type() == Damage::Cell )
        {
            CellDamage* cellDamage = static_cast<CellDamage*>( damage );
            const Region region = cellDamage->region();

            if ( cellDamage->changes() & CellDamage::Appearance )
                d->sheetView->invalidateRegion( region );
            continue;
        }

        if( damage->type() == Damage::Sheet )
        {
            SheetDamage* sheetDamage = static_cast<SheetDamage*>( damage );

            if ( sheetDamage->changes() & SheetDamage::PropertiesChanged )
                d->sheetView->invalidate();
            continue;
        }
    }

    update();
}

#include "TableShape.moc"
