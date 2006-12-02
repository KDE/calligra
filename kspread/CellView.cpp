/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2004-2005 Tomas Mecir <mecirt@gmail.com>
   Copyright 2004-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 1999-2002,2004,2005 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Reinhart Geiser <geiseri@kde.org>
   Copyright 2003-2005 Meni Livne <livne@kde.org>
   Copyright 2003 Peter Simonsson <psn@linux.se>
   Copyright 1999-2002 David Faure <faure@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 1999,2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1998-2000 Torben Weis <weis@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Stephan Kulow <coolo@kde.org>
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1998-1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Qt
#include <QApplication>
#include <QColor>
#include <QPainter>
#include <QRectF>

// KOffice
#include <KoPoint.h>

// KSpread
#include "Canvas.h"
#include "Condition.h"
#include "Doc.h"
#include "Format.h"
#include "Selection.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "SheetView.h"
#include "StyleManager.h"
#include "View.h"

// Local
#include "CellView.h"
#include "CellPrivate.h"

using namespace KSpread;

const int s_borderSpace = 1;

class CellView::Private : public QSharedData
{
public:
    Private( Style* defaultStyle, double defaultWidth, double defaultHeight )
        : style( *defaultStyle )
        , width( defaultWidth )
        , height( defaultHeight )
        , hidden( false )
        , textX( 0.0 )
        , textY( 0.0 )
        , textWidth( 0.0 )
        , textHeight( 0.0 ) {}
    ~Private()
    {
        if ( s_empty == this )
            s_empty = 0;
    }

    Style style;
    Sheet::LayoutDirection layoutDirection;
    double  width;
    double  height;

    bool hidden;

    // Position and dimension of displayed text.
    // Doc coordinate system; points; no zoom
    double  textX;
    double  textY;
    double  textWidth;
    double  textHeight;

    // static empty data to be shared
    static Private* empty( Style* defaultStyle, double defaultWidth, double defaultHeight )
    {
        if( !s_empty)
            s_empty = new Private( defaultStyle, defaultWidth, defaultHeight );
        return s_empty;
    }

private:
    static Private* s_empty;
};

// create static pointer
CellView::Private* CellView::Private::s_empty = 0;


CellView::CellView( SheetView* sheetView, int col, int row )
    : d( Private::empty( sheetView->sheet()->doc()->styleManager()->defaultStyle(),
                         sheetView->sheet()->columnFormat( 0 )->dblWidth(),
                         sheetView->sheet()->rowFormat( 0 )->dblHeight() ) )
{
    Q_ASSERT( col > 0 && col <= KS_colMax );
    Q_ASSERT( row > 0 && row <= KS_rowMax );

    const Sheet* sheet = sheetView->sheet();
    Cell* const cell = sheet->cellAt( col, row );

    // create the effective style
    if ( cell->isPartOfMerged() )
    {
        const Cell* firstCell = cell->obscuringCells().first();
        d->style = sheetView->cellView( firstCell->column(), firstCell->row() ).style();
    }
    else
    {
        // lookup the 'normal' style
        Style style = sheet->style( col, row );
        if ( !style.isEmpty() && !style.isDefault() )
            d->style = style;

        // use conditional formatting attributes
        Conditions conditions = sheet->conditions( col, row );
        if ( Style* style = conditions.testConditions( cell ) )
            d->style.merge( *style );
    }

    if ( cell->dblWidth( col ) != sheetView->sheet()->columnFormat( 0 )->dblWidth() )
        d->width = cell->dblWidth( col );
    if ( cell->dblHeight( row ) != sheetView->sheet()->rowFormat( 0 )->dblHeight() )
        d->height = cell->dblHeight( row );

    // do not touch the other Private members, just return here.
    if ( cell->isDefault() ) return;

    d->layoutDirection = sheet->layoutDirection();
    d->hidden = sheet->columnFormat( col )->hidden() || ( sheet->rowFormat( row )->dblHeight() <= sheet->doc()->unzoomItY( 2 ) );

    // horizontal align
    if ( d->style.halign() == Style::HAlignUndefined )
    {
        //numbers should be right-aligned by default, as well as BiDi text
        if ( ( d->style.formatType() == Text_format) || cell->value().isString() )
            d->style.setHAlign( cell->d->strOutText.isRightToLeft() ? Style::Right : Style::Left );
        else
        {
            Value val = cell->value();
            while (val.isArray())
                val = val.element(0, 0);
            d->style.setHAlign( (val.isBoolean() || val.isNumber()) ? Style::Right : Style::Left );
        }
    }

    makeLayout( cell );
}

CellView::CellView( const CellView& other )
    : d( other.d )
{
}

CellView::~CellView()
{
}

Style CellView::style() const
{
    return d->style;
}

double CellView::textWidth() const
{
  return d->textWidth;
}

double CellView::textHeight() const
{
  return d->textHeight;
}

QString CellView::testAnchor( const Cell* cell, double x, double y ) const
{
  if( cell->link().isEmpty() )
    return QString();

  if( x > d->textX ) if( x < d->textX + d->textWidth )
      if( y > d->textY - d->textHeight ) if( y < d->textY )
          return cell->link();

  return QString();
}

// ================================================================
//                            Painting


// Paint the cell.  This is the main function that calls a lot of
//                  helper functions.
//
// `rect'       is the rectangle that we should paint on.  If the cell
//              does not overlap this rectangle, we can return immediately.
// `coordinate' is the origin (the upper left) of the cell in document
//              coordinates.
//
void CellView::paintCell( const QRectF& rect, QPainter& painter,
                          View* view, const KoPoint& coordinate,
                          const QPoint& cellRef,
                          QLinkedList<QPoint> &mergedCellsPainted, Cell* cell )
{
#if 0
    // If we are already painting this cell, then return immediately.
    // This avoids infinite recursion.
    if ( cell->testFlag( Cell::Flag_PaintingCell ) )
        return;

    // Indicate that we are painting this cell now.
    cell->setFlag( Cell::Flag_PaintingCell );
#endif

    // This flag indicates that we are working on drawing the cells that
    // another cell is obscuring.  The value is the number of levels down we
    // are currently working -- i.e. a cell obscured by a cell which is
    // obscured by a cell.
    static int  paintingObscured = 0;

#if 0
    if (paintingObscured == 0)
        kDebug(36004) << "painting cell " << cell->name() << endl;
    else
        kDebug(36004) << "  painting obscured cell " << cell->name() << endl;
#endif

    // Sanity check: If we're working on drawing an obscured cell, that
    // means this cell should have a cell that obscures it.
    Q_ASSERT(!(paintingObscured > 0 && cell->d->extra()->obscuringCells.isEmpty()));

    // The parameter cellref should be *this, unless this is the default cell.
    Q_ASSERT(cell->isDefault()
            || (((cellRef.x() == cell->column()) && (cellRef.y() == cell->row()))));

    double left = coordinate.x();

    // Handle right-to-left layout.
    // In an RTL sheet the cells have to be painted at their opposite horizontal
    // location on the canvas, meaning that column A will be the rightmost column
    // on screen, column B will be to the left of it and so on. Here we change
    // the horizontal coordinate at which we start painting the cell in case the
    // sheet's direction is RTL. We do this only if paintingObscured is 0,
    // otherwise the cell's painting location will flip back and forth in
    // consecutive calls to paintCell when painting obscured cells.
    if ( d->layoutDirection == Sheet::RightToLeft && paintingObscured == 0
         && view && view->canvasWidget() )
    {
        double  dwidth = view->doc()->unzoomItXOld(view->canvasWidget()->width());
        left = dwidth - coordinate.x() - d->width;
    }

    // See if this cell is merged or has overflown into neighbor cells.
    // In that case, the width/height is greater than just the cell
    // itself.
    if (cell->d->hasExtra()) {
        if (cell->mergedXCells() > 0 || cell->mergedYCells() > 0) {
            // merged cell extends to the left if sheet is RTL
            if ( d->layoutDirection == Sheet::RightToLeft ) {
                left -= cell->extraWidth() - d->width;
            }
            d->width  += cell->extraWidth();
            d->height += cell->extraHeight();
        }
        else {
#if 0
            width  += cell->extraXCells() ? cell->extraWidth()  : 0;
            height += cell->extraYCells() ? cell->extraHeight() : 0;
#else
            // FIXME: Make extraWidth/Height really contain the *extra* width/height.
            if ( cell->extraXCells() )
                d->width  += cell->extraWidth();
            if ( cell->extraYCells() )
                d->height += cell->extraHeight();
#endif
        }
    }

    // Check if the cell is "selected", i.e. it should be drawn with the
    // color that indicates selection (dark blue).  If more than one
    // square is selected, the last one uses the ordinary colors.  In
    // that case, "selected" will be set to false even though the cell
    // itself really is selected.
    bool  selected = false;
#if 0 // moved to Canvas::paintNormalMarker()
    if ( view != 0 ) {
        selected = view->selectionInfo()->contains( cellRef );

        // But the cell doesn't look selected if this is the marker cell.
        Cell* cell = this->cell->sheet()->cellAt( view->selectionInfo()->marker() );
        QPoint bottomRight( view->selectionInfo()->marker().x() + cell->extraXCells(),
                            view->selectionInfo()->marker().y() + cell->extraYCells() );
        QRect markerArea( view->selectionInfo()->marker(), bottomRight );
        selected = selected && !( markerArea.contains( cellRef ) );

        // Don't draw any selection at all when printing.
        if ( dynamic_cast<QPrinter*>(painter.device()) )
            selected = false;
    }
#endif

    // ----------------  Start the actual painting.  ----------------

    // If the rect of this cell doesn't intersect the rect that should
    // be painted, we can skip the rest and return. (Note that we need
    // to calculate `left' first before we can do this.)
    const QRectF  cellRect( left, coordinate.y(), d->width, d->height );
    if ( !cellRect.intersects( rect ) ) {
        cell->clearFlag( Cell::Flag_PaintingCell );
        return;
    }

    // 1. Paint the background.
    if ( !cell->isPartOfMerged() )
        paintBackground( painter, cellRect, selected );

#if 0
    // 3. Paint all the cells that this one obscures.  They may only be
    //    partially obscured.
    //
    // The `paintingObscured' variable is used to avoid infinite
    // recursion since cells sometimes paint their obscuring cell as
    // well.
    paintingObscured++;

    if (cell->d->hasExtra() && (cell->extraXCells() > 0 || cell->extraYCells() > 0)) {
        //kDebug(36004) << "painting obscured cells for " << name() << endl;

        paintObscuredCells( rect, painter, view, cellRect, cellRef,
                            mergedCellsPainted);

        // FIXME: Is this the right place for this?
        if ( cell->mergedXCells() > 0 || cell->mergedYCells() > 0 )
            mergedCellsPainted.prepend( cellRef );
    }
    paintingObscured--;
#endif

    // 6. Now paint the content, if this cell isn't obscured.
    if ( !cell->isObscured() ) {

        // 6a. Paint possible comment indicator.
        if ( !dynamic_cast<QPrinter*>(painter.device())
              || cell->sheet()->print()->printCommentIndicator() )
            paintCommentIndicator( painter, cellRect, cellRef, cell );

        // 6b. Paint possible formula indicator.
        if ( !dynamic_cast<QPrinter*>(painter.device())
              || cell->sheet()->print()->printFormulaIndicator() )
            paintFormulaIndicator( painter, cellRect, cell );

        // 6c. Paint possible indicator for clipped text.
        paintMoreTextIndicator( painter, cellRect, cell );

        //6c. Paint cell highlight
#if 0
        if (highlightBorder != None)
        paintCellHighlight ( painter, cellRect, cellRef, highlightBorder,
        rightHighlightPen, bottomHighlightPen,
        leftHighlightPen,  topHighlightPen );
#endif

        // 6d. Paint the text in the cell unless:
        //  a) it is empty
        //  b) something indicates that the text should not be painted
        //  c) the sheet is protected and the cell is hidden.
        if ( !cell->strOutText().isEmpty()
              && ( !dynamic_cast<QPrinter*>(painter.device()) || style().printText() )
              && !( cell->sheet()->isProtected()
              && style().hideAll() ) )
        {
            paintText( painter, cellRect, cellRef, cell );
        }
    }
#if 0
    // 7. If this cell is obscured and we are not already painting obscured
    //    cells, then paint the obscuring cell(s).  Otherwise don't do
    //    anything so that we don't cause an infinite loop.
    if ( cell->isObscured() && paintingObscured == 0 &&
         !( d->layoutDirection == Sheet::RightToLeft && dynamic_cast<QPrinter*>(painter.device()) ) )
    {

        //kDebug(36004) << "painting cells that obscure " << name() << endl;

        // Store the obscuringCells list in a list of QPoint(column, row)
        // This avoids crashes during the iteration through
        // obscuringCells, when the cells may get non valid or the list
        // itself gets changed during a call of obscuringCell->paintCell
        // (this happens e.g. when there is an updateDepend)
        if (cell->d->hasExtra()) {
            QLinkedList<QPoint>           listPoints;
            QList<Cell*>::iterator  it = cell->d->extra()->obscuringCells.begin();
            QList<Cell*>::iterator  end = cell->d->extra()->obscuringCells.end();
            for ( ; it != end; ++it ) {
                Cell *obscuringCell = *it;

                listPoints.append( QPoint( obscuringCell->column(), obscuringCell->row() ) );
            }

            QLinkedList<QPoint>::iterator  it1  = listPoints.begin();
            QLinkedList<QPoint>::iterator  end1 = listPoints.end();
            for ( ; it1 != end1; ++it1 ) {
                QPoint obscuringCellRef = *it1;

                // Only paint those obscuring cells that haven't been already
                // painted yet.
                //
                // This optimization removes an O(n^4) behaviour where n is
                // the number of cells on one edge in a merged cell.
                if ( mergedCellsPainted.contains( obscuringCellRef ) )
                    continue;

                Cell *obscuringCell = cell->sheet()->cellAt( obscuringCellRef.x(),
                obscuringCellRef.y() );

                if ( obscuringCell != 0 ) {
                    double x = cell->sheet()->dblColumnPos( obscuringCellRef.x() );
                    double y = cell->sheet()->dblRowPos( obscuringCellRef.y() );
                    if ( view != 0 ) {
                        x -= view->canvasWidget()->xOffset();
                        y -= view->canvasWidget()->yOffset();
                    }

                    KoPoint corner( x, y );
                    painter.save();

                    // Get the effective pens for the borders.  These are
                    // determined by possible conditions on the cell with
                    // associated styles.
                    QPen rp( d->style.rightBorderPen() );
                    QPen bp( d->style.bottomBorderPen() );
                    QPen lp( d->style.leftBorderPen() );
                    QPen tp( d->style.topBorderPen() );


                    //kDebug(36004) << "  painting obscuring cell "
                    //     << obscuringCell->name() << endl;
                    // QPen highlightPen;

                    //Note: Painting of highlight isn't quite right.  If several
                    //      cells are merged, then the whole merged cell will be
                    //      painted with the color of the last cell referenced
                    //      which is inside the merged range.
                    CellView tmpCellView( cell->sheet(), obscuringCellRef.x(), obscuringCellRef.y() );
                    CellView* cellView = &tmpCellView;
                    cellView->paintCell( rect, painter, view,
                                         corner, obscuringCellRef,
                                         mergedCellsPainted); // new pens
                    painter.restore();
                }
            }
        }
    }
#endif

#if 0
    // We are done with the painting, so remove the flag on the cell.
    cell->clearFlag( Cell::Flag_PaintingCell );
#endif
}

void CellView::paintCellBorders( const QRectF& paintRegion, QPainter& painter,
                                 const KoPoint& paintCoordinate,
                                 const QPoint& cellCoordinate, const QRect& cellRegion,
                                 QLinkedList<QPoint> &mergedCellsPainted, Cell* cell, SheetView* sheetView )
{
    Sheet* const sheet = cell->sheet();

#if 0
    // If we are already painting this cell, then return immediately.
    // This avoids infinite recursion.
    if ( cell->testFlag( Cell::Flag_PaintingCell ) )
        return;

    // Indicate that we are painting this cell now.
    cell->setFlag( Cell::Flag_PaintingCell );
#endif

    // This flag indicates that we are working on drawing the cells that
    // another cell is obscuring.  The value is the number of levels down we
    // are currently working -- i.e. a cell obscured by a cell which is
    // obscured by a cell.
    static int  paintingObscured = 0;

#if 0
    if (paintingObscured == 0)
        kDebug(36004) << "painting cell " << cell->name() << endl;
    else
        kDebug(36004) << "  painting obscured cell " << cell->name() << endl;
#endif

    // Sanity check: If we're working on drawing an obscured cell, that
    // means this cell should have a cell that obscures it.
    Q_ASSERT(!(paintingObscured > 0 && cell->d->extra()->obscuringCells.isEmpty()));

    // The parameter cellCoordinate should be *this, unless this is the default cell.
    Q_ASSERT(cell->isDefault() || ((cellCoordinate.x() == cell->column()) && (cellCoordinate.y() == cell->row())));

    const int col = cellCoordinate.x();
    const int row = cellCoordinate.y();

    // See if this cell is merged or has overflown into neighbor cells.
    // In that case, the width/height is greater than just the cell
    // itself.
    if (cell->mergedXCells() > 0 || cell->mergedYCells() > 0)
    {
        d->width   = cell->extraWidth();
        d->height  = cell->extraHeight();
    }
    else
    {
#if 0
        width  += cell->extraXCells() ? cell->extraWidth()  : 0;
        height += cell->extraYCells() ? cell->extraHeight() : 0;
#else
        // FIXME: Make extraWidth/Height really contain the *extra* width/height.
        if ( cell->extraXCells() )
            d->width  = cell->extraWidth();
        if ( cell->extraYCells() )
            d->height = cell->extraHeight();
#endif
    }

    CellView::Borders paintBorder = CellView::NoBorder;

    // borders
    // NOTE Stefan: the borders of the adjacent cells are taken for the case,
    //              that the cell is located on the edge of the cell range,
    //              that is painted.
    if ( col == 1 )
        paintBorder |= LeftBorder;
    else if ( d->style.leftPenValue() < sheetView->cellView( col - 1, row ).style().rightPenValue() )
        d->style.setLeftBorderPen( sheetView->cellView( col - 1, row ).style().rightBorderPen() );
    else // if ( d->style.leftPenValue() >= sheetView->cellView( col - 1, row ).style().rightPenValue() )
        paintBorder |= LeftBorder;
    if ( col == KS_colMax )
        paintBorder |= CellView::RightBorder;
    else if ( d->style.rightPenValue() < sheetView->cellView( col + 1, row ).style().leftPenValue() )
        d->style.setRightBorderPen( sheetView->cellView( col + 1, row ).style().leftBorderPen() );
    else if ( d->style.rightPenValue() > sheetView->cellView( col + 1, row ).style().leftPenValue() )
        paintBorder |= CellView::RightBorder;
    if ( row == 1 )
        paintBorder |= TopBorder;
    else if ( d->style.topPenValue() < sheetView->cellView( col, row - 1 ).style().bottomPenValue() )
        d->style.setTopBorderPen( sheetView->cellView( col, row - 1 ).style().bottomBorderPen() );
    else // if ( d->style.topPenValue() >= sheetView->cellView( col, row - 1 ).style().bottomPenValue() )
        paintBorder |= TopBorder;
    if ( row == KS_rowMax )
        paintBorder |= BottomBorder;
    else if ( d->style.bottomPenValue() < sheetView->cellView( col, row + 1 ).style().topPenValue() )
        d->style.setBottomBorderPen( sheetView->cellView( col, row + 1 ).style().topBorderPen() );
    else if ( d->style.bottomPenValue() >= sheetView->cellView( col, row + 1 ).style().topPenValue() )
        paintBorder |= BottomBorder;

    // Paint border if outermost cell or if the pen is more "worth"
    // than the border pen of the cell on the other side of the
    // border or if the cell on the other side is not painted. In
    // the latter case get the pen that is of more "worth"
    if ( col == cellRegion.right() )
        paintBorder |= CellView::RightBorder;
    if ( row == cellRegion.bottom() )
        paintBorder |= CellView::BottomBorder;
    if ( col == cellRegion.left() )
        paintBorder |= CellView::LeftBorder;
    if ( row == cellRegion.top() )
        paintBorder |= CellView::TopBorder;

    // ----------------  Start the actual painting.  ----------------

    // If the rect of this cell doesn't intersect the rect that should
    // be painted, we can skip the rest and return. (Note that we need
    // to calculate `left' first before we can do this.)
    const QRectF  cellRect( paintCoordinate.x(), paintCoordinate.y(), d->width, d->height );
    if ( !cellRect.intersects( paintRegion ) ) {
        cell->clearFlag( Cell::Flag_PaintingCell );
        return;
    }

    // 1. Paint the default borders if we are on screen or if we are printing
    //    and the checkbox to do this is checked.
    if ( painter.device()->devType() != QInternal::Printer ||
         sheet->print()->printGrid())
        paintDefaultBorders( painter, paintRegion, cellRect, cellCoordinate, paintBorder, cell );

    // 2. Paint the borders of the cell if no other cell is forcing this
    // one, i.e. this cell is not part of a merged cell.
    //

    // If we print pages, then we disable clipping, otherwise borders are
    // cut in the middle at the page borders.
    if ( dynamic_cast<QPrinter*>(painter.device()) )
        painter.setClipping( false );

    // Paint the borders if this cell is not part of another merged cell.
    if ( !cell->isPartOfMerged() )
    {
        paintCustomBorders( painter, paintRegion, cellRect, cellCoordinate, paintBorder, cell );
    }

    // Turn clipping back on.
    if ( dynamic_cast<QPrinter*>(painter.device()) )
        painter.setClipping( true );

    // 3. Paint diagonal lines and page borders.
    paintCellDiagonalLines( painter, cellRect, cell );
    paintPageBorders( painter, cellRect, cellCoordinate, paintBorder, cell );

#if 0
    // We are done with the painting, so remove the flag on the cell.
    cell->clearFlag( Cell::Flag_PaintingCell );
#endif
}


// The following code was commented out in the above function.  I'll
// leave it here in case this functionality is ever re-implemented and
// someone wants some code to start from.
//
#if 0

  /**
 * Modification for drawing the button
   */
  if ( d->style == Cell::ST_Button ) {
    QBrush fill( Qt::lightGray );
    QApplication::style().drawControl( QStyle::CE_PushButton, &_painter, this,
               QRect( _tx + 1, _ty + 1, w2 - 1, h2 - 1 ),
               defaultColorGroup ); //, selected, &fill );
}

    /**
 * Modification for drawing the combo box
     */
  else if ( d->style == Cell::ST_Select ) {
    QApplication::style().drawComboButton(  &_painter, _tx + 1, _ty + 1,
              w2 - 1, h2 - 1,
              defaultColorGroup, selected );
}
#endif


#if 0
 void CellView::paintCellHighlight(QPainter& painter,
             const QRectF& cellRect,
             const QPoint& cellRef,
             const int highlightBorder,
             const QPen& rightPen,
             const QPen& bottomPen,
             const QPen& leftPen,
             const QPen& topPen
             )
{
  //painter.drawLine(cellRect.left(),cellRect.top(),cellRect.right(),cellRect.bottom());
  //QPen pen(cell->d->extra()->highlight);
  //painter.setPen(highlightPen);

  QBrush nullBrush;
  painter.setBrush(nullBrush);

  QRect zoomedCellRect = cell->sheet()->doc()->zoomRect( cellRect );

  //The highlight rect is just inside the main cell rect
  //This saves the hassle of repainting nearby cells when the highlight is changed as the highlight areas
  //do not overlap
  zoomedCellRect.setLeft(zoomedCellRect.left()+1);
  //zoomedCellRect.setRight(zoomedCellRect.right()-1);
  zoomedCellRect.setTop(zoomedCellRect.top()+1);
  //zoomedCellRect.setBottom(zoomedCellRect.bottom()-1);

  if ( cellRef.x() != KS_colMax )
    zoomedCellRect.setWidth( zoomedCellRect.width() - 1 );
  if ( cellRef.y() != KS_rowMax )
  zoomedCellRect.setHeight( zoomedCellRect.height() - 1 );

  if (highlightBorder & Top)
{
    painter.setPen(topPen);
    painter.drawLine(zoomedCellRect.left(),zoomedCellRect.top(),zoomedCellRect.right(),zoomedCellRect.top());
}
  if (highlightBorder & Left)
{
    painter.setPen(leftPen);
    painter.drawLine(zoomedCellRect.left(),zoomedCellRect.top(),zoomedCellRect.left(),zoomedCellRect.bottom());
}
  if (highlightBorder & RightBorder)
{
    painter.setPen(rightPen);
    painter.drawLine(zoomedCellRect.right(),zoomedCellRect.top(),zoomedCellRect.right(),zoomedCellRect.bottom());
}
  if (highlightBorder & Bottom)
{
    painter.setPen(bottomPen);
    painter.drawLine(zoomedCellRect.left(),zoomedCellRect.bottom(),zoomedCellRect.right(),zoomedCellRect.bottom());
}

  if (highlightBorder & SizeGrip)
{
    QBrush brush(rightPen.color());
    painter.setBrush(brush);
    painter.setPen(rightPen);
    painter.drawRect(zoomedCellRect.right()-3,zoomedCellRect.bottom()-3,4,4);
}

  //painter.drawRect(zoomedCellRect.left(),zoomedCellRect.top(),zoomedCellRect.width(),zoomedCellRect.height());
}
#endif

#if 0
// Paint all the cells that this cell obscures (helper function to paintCell).
//
void CellView::paintObscuredCells(const QRectF& rect, QPainter& painter,
                                  View* view, const QRectF &cellRect,
                                  const QPoint &cellRef,
                                  QLinkedList<QPoint> &mergedCellsPainted)
{
    // If there are no obscured cells, return.
    if ( !cell->extraXCells() && !cell->extraYCells() )
        return;

    double  ypos = cellRect.y();
    int     maxY = cell->extraYCells();
    int     maxX = cell->extraXCells();

    // Loop through the rectangle of squares that we obscure and paint them.
    for ( int y = 0; y <= maxY; ++y ) {
        double xpos = cellRect.x();
        RowFormat* rl = cell->sheet()->rowFormat( cellRef.y() + y );

        for( int x = 0; x <= maxX; ++ x ) {
            ColumnFormat * cl = cell->sheet()->columnFormat( cellRef.x() + x );
            if ( y != 0 || x != 0 ) {
                uint  column = cellRef.x() + x;
                uint  row    = cellRef.y() + y;

                KoPoint corner( xpos, ypos );
#if 0
                Cell* cell = this->cell->sheet()->cellAt( column, row );

                // Check if the upper and lower borders should be painted, and
                // if so which pens we should use.  There used to be a nasty
                // bug here (#61452).
                // Check top pen.  Only check if this is not on the top row.
                topPen         = _topPen;
                if ( row > 1 && !cell->isPartOfMerged() ) {
                    Cell* cellUp = this->cell->sheet()->cellAt( column, row - 1 );

                    if ( cellUp->isDefault() )
                        paintBorder &= ~TopBorder;
                    else {
                        // If the cell towards the top is part of a merged cell, get
                        // the pointer to the master cell.
                        cellUp = cellUp->ultimateObscuringCell();

                        topPen = cellUp->effBottomBorderPen( cellUp->column(),
                        cellUp->row() );

#if 0
                        int  penWidth = qMax(1, cell->sheet()->doc()->zoomItYOld( topPen.width() ));
                        topPen.setWidth( penWidth );
#endif
                    }
                }

                // FIXME: I thought we had to check bottom pen as well.
                //        However, it looks as if we don't need to.  It works anyway.
                bottomPen         = _bottomPen;
#endif
                //kDebug(36004) << "calling paintcell for obscured cell "
                //       << cell->name() << endl;
                CellView tmpCellView( this->cell->sheet(), column, row  ); // FIXME
                CellView* cellView = &tmpCellView;
                cellView->paintCell( rect, painter, view, corner,
                                     QPoint( cellRef.x() + x, cellRef.y() + y ),
                                     mergedCellsPainted );
            }
            xpos += cl->dblWidth();
        }
        ypos += rl->dblHeight();
    }
}
#endif

//
// Paint the background of this cell.
//
void CellView::paintBackground( QPainter& painter, const QRectF& cellRect,
                                bool selected )
{
    Q_UNUSED( selected );

    // disable antialiasing
    painter.setRenderHint( QPainter::Antialiasing, false );

  // Handle printers separately.
  if ( dynamic_cast<QPrinter*>(painter.device()) )
  {
    //bad hack but there is a qt bug
    //so I can print backgroundcolor
    QBrush brush( d->style.backgroundColor() );
    if ( !d->style.backgroundColor().isValid() )
      brush.setColor( Qt::white );

    painter.fillRect( cellRect, brush );
    return;
  }

  if ( d->style.backgroundColor().isValid() )
    painter.setBackground( d->style.backgroundColor() );
  else
    painter.setBackground( QApplication::palette().base().color() );

  // Erase the background of the cell.
  painter.eraseRect( cellRect );

  // Get a background brush
  QBrush brush = d->style.backgroundBrush();

  // Draw background pattern if necessary.
  if ( brush.style() != Qt::NoBrush )
    painter.fillRect( cellRect, brush );

#if 0 // moved to Canvas::paintNormalMarker()
  // Draw alpha-blended selection
  if ( selected )
  {
    QColor selectionColor( QApplication::palette().highlight().color() );
    selectionColor.setAlpha( 127 );
    painter.fillRect( cellRect, selectionColor );
  }
#endif

    // restore antialiasing
    painter.setRenderHint( QPainter::Antialiasing, true );
}


// Paint the standard light grey borders that are always visible.
//
void CellView::paintDefaultBorders( QPainter& painter, const QRectF &rect,
                                    const QRectF &cellRect, const QPoint &cellRef,
                                    Borders paintBorder, Cell* cell )
{
    Q_UNUSED(cellRef);

    // disable antialiasing
    painter.setRenderHint( QPainter::Antialiasing, false );

    /*
    *** Notes about optimization ***

    This function was painting the top, left, right & bottom lines in almost
    all cells previously, contrary to what the comment below says should happen.
    There doesn't appear to be a UI option to enable or disable showing of the
    grid when printing at the moment, so I have disabled drawing of right and
    bottom borders for all cells.

    I also couldn't work out under what conditions the variables dt / db would
    come out as anything other than 0 in the code for painting the various borders.
    The cell->effTopBorderPen / cell->effBottomBorderPen calls were taking
    up a lot of time according some profiling I did.  If that code really is
    necessary, we need to find a more efficient way of getting the widths than
    grabbing the whole QPen object and asking it.

    --Robert Knight (robertknight@gmail.com)
    */
    bool paintingToExternalDevice = dynamic_cast<QPrinter*>(painter.device());

    // Each cell is responsible for drawing it's top and left portions
    // of the "default" grid. --Or not drawing it if it shouldn't be
    // there.  It's also responsible to paint the right and bottom, if
    // it is the last cell on a print out.

    const bool isMergedOrObscured = cell->isPartOfMerged() || cell->isObscured();

    const bool paintLeft    = ( paintBorder & LeftBorder &&
                                d->style.leftBorderPen().style() == Qt::NoPen &&
                                cell->sheet()->getShowGrid() &&
                                !isMergedOrObscured );
    const bool paintRight   = ( paintBorder & RightBorder &&
                                d->style.rightBorderPen().style() == Qt::NoPen &&
                                cell->sheet()->getShowGrid() &&
                                !isMergedOrObscured );
    const bool paintTop     = ( paintBorder & TopBorder &&
                                d->style.topBorderPen().style() == Qt::NoPen &&
                                cell->sheet()->getShowGrid() &&
                                !isMergedOrObscured );
    const bool paintBottom  = ( paintBorder & BottomBorder &&
                                cell->sheet()->getShowGrid() &&
                                d->style.bottomBorderPen().style() == Qt::NoPen &&
                                !isMergedOrObscured );

    // Set the single-pixel width pen for drawing the borders with.
    // NOTE Stefan: Use a cosmetic pen (width = 0), because we want the grid always one pixel wide
    painter.setPen( QPen( cell->sheet()->doc()->gridColor(), 0, Qt::SolidLine ) );

    QLineF line;

    // The left border.
    if ( paintLeft )
    {
        int dt = 0;
        int db = 0;

#if 0
        if ( cellRef.x() > 1 ) {
        Cell  *cell_west = cell->sheet()->cellAt( cellRef.x() - 1,
        cellRef.y() );
        QPen t = cell_west->effTopBorderPen( cellRef.x() - 1, cellRef.y() );
        QPen b = cell_west->effBottomBorderPen( cellRef.x() - 1, cellRef.y() );

        if ( t.style() != Qt::NoPen )
        dt = ( t.width() + 1 )/2;
        if ( b.style() != Qt::NoPen )
        db = ( t.width() / 2);
    }
#endif

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if ( paintingToExternalDevice ) {
                line = QLineF( qMax( rect.left(),   cellRect.x() ),
                               qMax( rect.top(),    cellRect.y() + dt ),
                               qMin( rect.right(),  cellRect.x() ),
                               qMin( rect.bottom(), cellRect.bottom() - db ) );
        }
        else {
                line = QLineF( cellRect.x(),
                               cellRect.y() + dt,
                               cellRect.x(),
                               cellRect.bottom() - db );
        }
        painter.drawLine( line );
    }


    // The top border.
    if ( paintTop ) {
        int dl = 0;
        int dr = 0;

#if 0
        if ( cellRef.y() > 1 ) {
            Cell  *cell_north = cell->sheet()->cellAt( cellRef.x(),
            cellRef.y() - 1 );

            QPen l = cell_north->effLeftBorderPen(  cellRef.x(), cellRef.y() - 1 );
            QPen r = cell_north->effRightBorderPen( cellRef.x(), cellRef.y() - 1 );

            if ( l.style() != Qt::NoPen )
                dl = ( l.width() - 1 ) / 2 + 1;
            if ( r.style() != Qt::NoPen )
                dr = r.width() / 2;
        }
#endif

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if ( paintingToExternalDevice ) {
            line = QLineF( qMax( rect.left(),   cellRect.x() + dl ),
                           qMax( rect.top(),    cellRect.y() ),
                           qMin( rect.right(),  cellRect.right() - dr ),
                           qMin( rect.bottom(), cellRect.y() ) );
        }
        else {
            line = QLineF( cellRect.x() + dl,
                           cellRect.y(),
                           cellRect.right() - dr,
                           cellRect.y() );
        }
        painter.drawLine( line );
    }


    // The right border.
    if ( paintRight ) {
        int dt = 0;
        int db = 0;

#if 0
        if ( cellRef.x() < KS_colMax ) {
            Cell  *cell_east = cell->sheet()->cellAt( cellRef.x() + 1,
            cellRef.y() );

            QPen t = cell_east->effTopBorderPen(    cellRef.x() + 1, cellRef.y() );
            QPen b = cell_east->effBottomBorderPen( cellRef.x() + 1, cellRef.y() );

            if ( t.style() != Qt::NoPen )
                dt = ( t.width() + 1 ) / 2;
            if ( b.style() != Qt::NoPen )
                db = ( t.width() / 2);
        }
#endif

        //painter.setPen( QPen( cell->sheet()->doc()->gridColor(), 1, Qt::SolidLine ) );

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if ( dynamic_cast<QPrinter*>(painter.device()) )
        {
                line = QLineF( qMax( rect.left(),   cellRect.right() ),
                               qMax( rect.top(),    cellRect.y() + dt ),
                               qMin( rect.right(),  cellRect.right() ),
                               qMin( rect.bottom(), cellRect.bottom() - db ) );
        }
        else
        {
                line = QLineF( cellRect.right(),
                               cellRect.y() + dt,
                               cellRect.right(),
                               cellRect.bottom() - db );
        }
        painter.drawLine( line );
    }

    // The bottom border.
    if ( paintBottom ) {
        int dl = 0;
        int dr = 0;
#if 0
        if ( cellRef.y() < KS_rowMax ) {
            Cell  *cell_south = cell->sheet()->cellAt( cellRef.x(),
            cellRef.y() + 1 );

            QPen l = cell_south->effLeftBorderPen(  cellRef.x(), cellRef.y() + 1 );
            QPen r = cell_south->effRightBorderPen( cellRef.x(), cellRef.y() + 1 );

            if ( l.style() != Qt::NoPen )
                dl = ( l.width() - 1 ) / 2 + 1;
            if ( r.style() != Qt::NoPen )
                dr = r.width() / 2;
        }
#endif

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if ( dynamic_cast<QPrinter*>(painter.device()) ) {
            line = QLineF( qMax( rect.left(),   cellRect.x() + dl ),
                           qMax( rect.top(),    cellRect.bottom() ),
                           qMin( rect.right(),  cellRect.right() - dr ),
                           qMin( rect.bottom(), cellRect.bottom() ) );
        }
        else {
            line = QLineF( cellRect.x() + dl,
                           cellRect.bottom(),
                           cellRect.right() - dr,
                           cellRect.bottom() );
        }
        painter.drawLine( line );
    }

    // restore antialiasing
    painter.setRenderHint( QPainter::Antialiasing, true );
}


// Paint a comment indicator if the cell has a comment.
//
void CellView::paintCommentIndicator( QPainter& painter,
                                      const QRectF &cellRect,
                                      const QPoint &cellRef,
                                      Cell* cell )
{
    // Point the little corner if there is a comment attached
    // to this cell.
    if ( ( !cell->comment( cellRef.x(), cellRef.y() ).isEmpty() )
            && cellRect.width() > 10.0
            && cellRect.height() > 10.0
            && ( cell->sheet()->print()->printCommentIndicator()
            || ( !dynamic_cast<QPrinter*>(painter.device()) && cell->sheet()->getShowCommentIndicator() ) ) )
    {
        QColor penColor = Qt::red;

        // If background has high red part, switch to blue.
        if ( qRed( d->style.backgroundColor().rgb() ) > 127 &&
             qGreen( d->style.backgroundColor().rgb() ) < 80 &&
             qBlue( d->style.backgroundColor().rgb() ) < 80 )
        {
            penColor = Qt::blue;
        }

        // Get the triangle.
        QPolygonF polygon( 3 );
        polygon.clear();
        if ( cell->sheet()->layoutDirection()==Sheet::RightToLeft ) {
            polygon << QPointF( cellRect.x() + 6.0, cellRect.y() );
            polygon << QPointF( cellRect.x(), cellRect.y() );
            polygon << QPointF( cellRect.x(), cellRect.y() + 6.0 );
        }
        else {
            polygon << QPointF( cellRect.right() - 5.0, cellRect.y() );
            polygon << QPointF( cellRect.right(), cellRect.y() );
            polygon << QPointF( cellRect.right(), cellRect.y() + 5.0 );
        }

        // And draw it.
        painter.setBrush( QBrush( penColor ) );
        painter.setPen( Qt::NoPen );
        painter.drawPolygon( polygon );
    }
}


// Paint a small rectangle if this cell holds a formula.
//
void CellView::paintFormulaIndicator( QPainter& painter,
                                      const QRectF &cellRect,
                                      Cell* cell )
{
  if ( cell->isFormula() &&
       cell->sheet()->getShowFormulaIndicator() &&
       cellRect.width()  > 10.0 &&
       cellRect.height() > 10.0 )
  {
    QColor penColor = Qt::blue;
    // If background has high blue part, switch to red.
    if ( qRed( d->style.backgroundColor().rgb() ) < 80 &&
         qGreen( d->style.backgroundColor().rgb() ) < 80 &&
         qBlue( d->style.backgroundColor().rgb() ) > 127 )
    {
      penColor = Qt::red;
    }

    // Get the triangle...
    QPolygonF polygon( 3 );
    polygon.clear();
    if ( cell->sheet()->layoutDirection()==Sheet::RightToLeft ) {
      polygon << QPointF( cellRect.right() - 6.0, cellRect.bottom() );
      polygon << QPointF( cellRect.right(), cellRect.bottom() );
      polygon << QPointF( cellRect.right(), cellRect.bottom() - 6.0 );
    }
    else {
      polygon << QPointF( cellRect.x(), cellRect.bottom() - 6.0 );
      polygon << QPointF( cellRect.x(), cellRect.bottom() );
      polygon << QPointF( cellRect.x() + 6.0, cellRect.bottom() );
    }

    // ...and draw it.
    painter.setBrush( QBrush( penColor ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( polygon );
  }
}


// Paint an indicator that the text in the cell is cut.
//
void CellView::paintMoreTextIndicator( QPainter& painter,
                                       const QRectF &cellRect,
                                       Cell* cell )
{
  // Show a red triangle when it's not possible to write all text in cell.
  // Don't print the red triangle if we're printing.
  if( cell->testFlag( Cell::Flag_CellTooShortX ) &&
      !dynamic_cast<QPrinter*>(painter.device()) &&
      cellRect.height() > 4.0  &&
      cellRect.width()  > 4.0 )
  {
    QColor penColor = Qt::red;
    // If background has high red part, switch to blue.
    if ( qRed( d->style.backgroundColor().rgb() ) > 127
         && qGreen( d->style.backgroundColor().rgb() ) < 80
         && qBlue( d->style.backgroundColor().rgb() ) < 80 )
    {
      penColor = Qt::blue;
    }

    // Get the triangle...
    QPolygonF polygon( 3 );
    polygon.clear();
    if ( cell->strOutText().isRightToLeft() ) {
      polygon << QPointF( cellRect.left() + 4.0, cellRect.y() + cellRect.height() / 2.0 - 4.0 );
      polygon << QPointF( cellRect.left(), cellRect.y() + cellRect.height() / 2.0 );
      polygon << QPointF( cellRect.left() + 4.0, cellRect.y() + cellRect.height() / 2.0 + 4.0 );
    }
    else {
      polygon << QPointF( cellRect.right() - 4.0, cellRect.y() + cellRect.height() / 2.0 - 4.0 );
      polygon << QPointF( cellRect.right(), cellRect.y() + cellRect.height() / 2.0 );
      polygon << QPointF( cellRect.right() - 4.0, cellRect.y() + cellRect.height() / 2.0 + 4.0 );
    }

    // ...and paint it.
    painter.setBrush( QBrush( penColor ) );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( polygon );
  }
}


// Paint the real contents of a cell - the text.
//
void CellView::paintText( QPainter& painter,
                          const QRectF &cellRect,
                          const QPoint &cellRef, Cell* cell )
{
  const ColumnFormat* columnFormat = cell->sheet()->columnFormat( cellRef.x() );
  QColor textColorPrint = d->style.fontColor();

  // Resolve the text color if invalid (=default).
  if ( !textColorPrint.isValid() ) {
    if ( dynamic_cast<QPrinter*>(painter.device()) )
      textColorPrint = Qt::black;
    else
      textColorPrint = QApplication::palette().text().color();
  }

  QPen tmpPen( textColorPrint );

  // Set the font according to the current zoom.
  painter.setFont( effectiveFont( cell ) );

    // Check for red font color for negative values.
    if ( cell->value().isNumber()
        && !( cell->sheet()->getShowFormula()
        && !( cell->sheet()->isProtected()
        && style().hideFormula() ) ) )
    {
        double value = cell->value().asFloat();
        if ( style().floatColor() == Style::NegRed && value < 0.0 )
            tmpPen.setColor( Qt::red );
    }

  // Check for blue color, for hyperlink.
  if ( !cell->link().isEmpty() ) {
    tmpPen.setColor( QApplication::palette().link().color() );
    QFont font = painter.font();
    font.setUnderline( true );
    painter.setFont( font );
  }

#if 0
/****

 For now I am commenting this out -- with the default color display you
 can read normal text through a highlighted background.  Maybe this isn't
 always the case, though, and we can put the highlighted text color back in.
 In that case, we need to somewhere in here figure out if the text overlaps
 another cell outside of the selection, otherwise that portion of the text
 will be printed white on white.  So just that portion would need to be
 painted again in the normal color.

 This should probably be done eventually, anyway, because I like using the
 reverse text color for highlighted cells.  I just don't like extending the
 cell 'highlight' background outside of the selection rectangle because it
 looks REALLY ugly.
*/

  if ( selected && ( cellRef.x() != marker.x() || cellRef.y() != marker.y() ) )
{
    QPen p( tmpPen );
    p.setColor( defaultColorGroup.highlightedText() );
    painter.setPen( p );
}
  else {
    painter.setPen(tmpPen);
}
#endif
  painter.setPen( tmpPen );

  QString  tmpText   = cell->strOutText();
  double   tmpHeight = d->textHeight;
  double   tmpWidth  = d->textWidth;

  // If the cell is to narrow to paint the whole contents, then pick
  // out a part of the content that we paint.  The result of this is
  // dependent on the data type of the content.
  //
  // FIXME: Make this dependent on the height as well.
  //
  if ( cell->testFlag( Cell::Flag_CellTooShortX ) ) {
    QFontMetrics fontMetrics( effectiveFont( cell ) );
    cell->d->strOutText = textDisplaying( fontMetrics, cell );

    // Recalculate the text dimensions and the offset.
    textSize( fontMetrics, cell );
    textOffset( fontMetrics, cell );
  }

  // Hide zero.
  if ( cell->sheet()->getHideZero()
       && cell->value().isNumber()
       && cell->value().asFloat() == 0 ) {
    cell->d->strOutText.clear();
  }

  // Clear extra cell if column or row is hidden
  //
  // FIXME: I think this should be done before the call to
  // textDisplaying() above.
  //
  if ( columnFormat->hidden() || ( cellRect.height() <= 2 ) ) {
    cell->freeAllObscuredCells();  /* TODO: This looks dangerous...must check when I have time */
    cell->d->strOutText = "";
  }

  double indent = 0.0;
  double offsetCellTooShort = 0.0;
  int a = d->style.halign();

    // Apply indent if text is align to left not when text is at right or middle.
    if (  a == Style::Left && !cell->isEmpty() )
    {
        indent = d->style.indentation();
    }

  // Made an offset, otherwise ### is under red triangle.
  if ( a == Style::Right && !cell->isEmpty() && cell->testFlag( Cell::Flag_CellTooShortX ) )
    offsetCellTooShort = 4;

  QFontMetrics fm2 = painter.fontMetrics();
  double offsetFont = 0.0;

  if ( style().valign() == Style::Bottom && style().underline() )
    offsetFont = fm2.underlinePos() + 1;

    int tmpAngle = d->style.angle();
    bool tmpVerticalText = d->style.verticalText();
    // force multiple rows on explicitly set line breaks
    bool tmpMultiRow = d->style.wrapText() || cell->strOutText().contains( '\n' );

  // Actually paint the text.
  //    There are 4 possible cases:
  //        - One line of text , horizontal
  //        - Angled text
  //        - Multiple rows of text , horizontal
  //        - Vertical text
  if ( !tmpMultiRow && !tmpVerticalText && !tmpAngle ) {
    // Case 1: The simple case, one line, no angle.

    const QPointF position( indent + cellRect.x() + d->textX - offsetCellTooShort,
                            cellRect.y() + d->textY - offsetFont );
    painter.drawText( position, cell->strOutText() );
  }
  else if ( tmpAngle != 0 ) {
    // Case 2: an angle.

    int angle = tmpAngle;
    QFontMetrics fm = painter.fontMetrics();

    painter.rotate( angle );
    double x;
    if ( angle > 0 )
      x = indent + cellRect.x() + d->textX;
    else
      x = indent + cellRect.x() + d->textX
          - ( fm.descent() + fm.ascent() ) * sin( angle * M_PI / 180 );
    double y;
    if ( angle > 0 )
      y = cellRect.y() + d->textY;
    else
      y = cellRect.y() + d->textY + d->textHeight;
    const QPointF position( x * cos( angle * M_PI / 180 ) + y * sin( angle * M_PI / 180 ),
                           -x * sin( angle * M_PI / 180 ) + y * cos( angle * M_PI / 180 ) );
    painter.drawText( position, cell->strOutText() );
    painter.rotate( -angle );
  }
  else if ( tmpMultiRow && !tmpVerticalText ) {
    // Case 3: Multiple rows, but horizontal.

    QString text;
    int i;
    int pos = 0;
    double dy = 0.0;
    QFontMetrics fm = painter.fontMetrics();
    do {
      i = cell->strOutText().indexOf( "\n", pos );
      if ( i == -1 )
        text = cell->strOutText().mid( pos, cell->strOutText().length() - pos );
      else {
        text = cell->strOutText().mid( pos, i - pos );
        pos = i + 1;
      }

      int align = d->style.halign();
      if ( cell->sheet()->getShowFormula()
          && !( cell->sheet()->isProtected()
          && style().hideFormula() ) )
        align = Style::Left;

      // ### Torben: This looks duplicated for me
      switch ( align ) {
        case Style::Left:
          d->textX = d->style.leftBorderPen().width() + s_borderSpace;
          break;

        case Style::Right:
          d->textX = cellRect.width() - s_borderSpace - fm.width( text )
                   - d->style.rightBorderPen().width();
          break;

        case Style::Center:
          d->textX = ( cellRect.width() - fm.width( text ) ) / 2;
      }

      const QPointF position( indent + cellRect.x() + d->textX, cellRect.y() + d->textY + dy );
      painter.drawText( position, text );
      dy += fm.descent() + fm.ascent();
    } while ( i != -1 );
  }
  else if ( tmpVerticalText && !cell->strOutText().isEmpty() ) {
    // Case 4: Vertical text.

    QString text;
    int i = 0;
    int len = 0;
    double dy = 0.0;
    QFontMetrics fm = painter.fontMetrics();
    do {
      len = cell->strOutText().length();
      text = cell->strOutText().at( i );
      const QPointF position( indent + cellRect.x() + d->textX,
                              cellRect.y() + d->textY + dy );
      painter.drawText( position, text );
      dy += fm.descent() + fm.ascent();
      i++;
    } while ( i != len );
  }

  // Check for too short cell and set the outText for future reference.
  if ( cell->testFlag( Cell::Flag_CellTooShortX ) ) {
    cell->d->strOutText = tmpText;
    d->textHeight = tmpHeight;
    d->textWidth  = tmpWidth;
  }

  if ( cell->sheet()->getHideZero() && cell->value().isNumber()
      && cell->value().asFloat() == 0 )
    cell->d->strOutText = tmpText;

  if ( columnFormat->hidden() || ( cellRect.height() <= 2 ) )
    cell->d->strOutText = tmpText;
}


// Paint page borders on the page.  Only do this on the screen.
//
void CellView::paintPageBorders( QPainter& painter, const QRectF &cellRect,
                                 const QPoint &cellRef, Borders paintBorder, Cell* cell )
{
  // Not screen?  Return immediately.
  if ( dynamic_cast<QPrinter*>(painter.device()) )
    return;

  if ( ! cell->sheet()->isShowPageBorders() )
    return;

  SheetPrint* print = cell->sheet()->print();

  // Draw page borders
  QLineF line;

  if ( cellRef.x() >= print->printRange().left()
       && cellRef.x() <= print->printRange().right() + 1
       && cellRef.y() >= print->printRange().top()
       && cellRef.y() <= print->printRange().bottom() + 1 )
  {
    if ( print->isOnNewPageX( cellRef.x() )
         && cellRef.y() <= print->printRange().bottom() )
    {
      painter.setPen( cell->sheet()->doc()->pageBorderColor() );

      if ( d->layoutDirection == Sheet::RightToLeft )
        line = QLineF( cellRect.right(), cellRect.top(),
                          cellRect.right(), cellRect.bottom() );
      else
        line = QLineF( cellRect.left(), cellRect.top(),
                          cellRect.left(), cellRect.bottom() );
      painter.drawLine( line );
    }

    if ( print->isOnNewPageY( cellRef.y() ) &&
         ( cellRef.x() <= print->printRange().right() ) )
    {
      painter.setPen( cell->sheet()->doc()->pageBorderColor() );
      line = QLineF( cellRect.left(),  cellRect.top(),
                        cellRect.right(), cellRect.top() );
      painter.drawLine( line );
    }

    if ( paintBorder & RightBorder ) {
      if ( print->isOnNewPageX( cellRef.x() + 1 )
           && cellRef.y() <= print->printRange().bottom() ) {
        painter.setPen( cell->sheet()->doc()->pageBorderColor() );

        if ( d->layoutDirection == Sheet::RightToLeft )
          line = QLineF( cellRect.left(), cellRect.top(),
                            cellRect.left(), cellRect.bottom() );
        else
          line = QLineF( cellRect.right(), cellRect.top(),
                            cellRect.right(), cellRect.bottom() );
        painter.drawLine( line );
           }
    }

    if ( paintBorder & BottomBorder ) {
      if ( print->isOnNewPageY( cellRef.y() + 1 )
           && cellRef.x() <= print->printRange().right() ) {
        painter.setPen( cell->sheet()->doc()->pageBorderColor() );
        line = QLineF( cellRect.left(),  cellRect.bottom(),
                          cellRect.right(), cellRect.bottom() );
        painter.drawLine( line );
           }
    }
  }
}


// Paint the cell borders.
//
void CellView::paintCustomBorders(QPainter& painter, const QRectF &rect,
                                  const QRectF &cellRect, const QPoint &cellRef,
                                  Borders paintBorder, Cell* cell )
{
    //Sanity check: If we are not painting any of the borders then the function
    //really shouldn't be called at all.
    if ( paintBorder == NoBorder )
        return;

    if (cell->d->hasExtra()) {
        QList<Cell*>::const_iterator it  = cell->d->extra()->obscuringCells.begin();
        QList<Cell*>::const_iterator end = cell->d->extra()->obscuringCells.end();
        for ( ; it != end; ++it ) {
            Cell* cell = *it;

            int xDiff = cellRef.x() - cell->column();
            int yDiff = cellRef.y() - cell->row();
            if (xDiff == 0)
                paintBorder |= LeftBorder;
            else
                paintBorder &= ~LeftBorder;
            if (yDiff == 0)
                paintBorder |= TopBorder;
            else
                paintBorder &= ~TopBorder;

            // Paint the border(s) if either this one should or if we have a
            // merged cell with this cell as its border.
            if (cell->mergedXCells() == xDiff)
                paintBorder |= RightBorder;
            else
                paintBorder &= ~RightBorder;
            if (cell->mergedYCells() == yDiff)
                paintBorder |= BottomBorder;
            else
                paintBorder &= ~BottomBorder;
        }
    }

    // Must create copies of these since otherwise the zoomIt()
    // operation will be performed on them repeatedly.
    QPen  leftPen  ( d->style.leftBorderPen() );
    QPen  rightPen ( d->style.rightBorderPen() );
    QPen  topPen   ( d->style.topBorderPen() );
    QPen  bottomPen( d->style.bottomBorderPen() );

    // Determine the pens that should be used for drawing
    // the borders.
    // NOTE Stefan: This prevents cosmetic pens (width==0).
    int left_penWidth   = qMax( 1, ( leftPen.width() ) );
    int right_penWidth  = qMax( 1, ( rightPen.width() ) );
    int top_penWidth    = qMax( 1, ( topPen.width() ) );
    int bottom_penWidth = qMax( 1, ( bottomPen.width() ) );

    leftPen.setWidth( left_penWidth );
    rightPen.setWidth( right_penWidth );
    topPen.setWidth( top_penWidth );
    bottomPen.setWidth( bottom_penWidth );

    QLineF line;

    if ( (paintBorder & LeftBorder) && leftPen.style() != Qt::NoPen )
    {
        painter.setPen( leftPen );

        //kDebug(36004) << "    painting left border of cell " << name() << endl;

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if ( dynamic_cast<QPrinter*>(painter.device()) ) {
            if ( cellRect.left() >= rect.left() + left_penWidth / 2)
                line = QLineF( cellRect.left() ,
                               qMax( rect.top(), cellRect.top() ),
                               cellRect.left(),
                               qMin( rect.bottom(), cellRect.bottom() ) );
        }
        else
        {
                line = QLineF( cellRect.left(), cellRect.top(), cellRect.left(), cellRect.bottom() );
        }
        painter.drawLine( line );
    }

    if ( (paintBorder & RightBorder) && rightPen.style() != Qt::NoPen )
    {
        painter.setPen( rightPen );

        //kDebug(36004) << "    painting right border of cell " << name() << endl;

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if ( dynamic_cast<QPrinter*>(painter.device()) ) {
                // Only print the right border if it is visible.
                if ( cellRect.right() <= rect.right() + right_penWidth / 2)
                    line = QLineF( cellRect.right(),
                                   qMax( rect.top(), cellRect.top() ),
                                   cellRect.right(),
                                   qMin( rect.bottom(), cellRect.bottom() ) );
        }
        else
        {
                line = QLineF( cellRect.right(), cellRect.top(), cellRect.right(), cellRect.bottom() );
        }
        painter.drawLine( line );
    }

    if ( (paintBorder & TopBorder) && topPen.style() != Qt::NoPen )
    {
        painter.setPen( topPen );

        //kDebug(36004) << "    painting top border of cell " << name()
        //       << " [" << cellRect.left() << "," << cellRect.right()
        //       << ": " << cellRect.right() - cellRect.left() << "]" << endl;

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if ( dynamic_cast<QPrinter*>(painter.device()) ) {
            if ( cellRect.top() >= rect.top() + top_penWidth / 2)
                line = QLineF( qMax( rect.left(),   cellRect.left() ),
                               cellRect.top(),
                               qMin( rect.right(),  cellRect.right() ),
                               cellRect.top() );
        }
        else
        {
            line = QLineF( cellRect.left(), cellRect.top(), cellRect.right(), cellRect.top() );
        }
        painter.drawLine( line );
    }

    if ( (paintBorder & BottomBorder) && bottomPen.style() != Qt::NoPen )
    {
        painter.setPen( bottomPen );

        //kDebug(36004) << "    painting bottom border of cell " << name()
        //       << " [" << cellRect.left() << "," << cellRect.right()
        //       << ": " << cellRect.right() - cellRect.left() << "]" << endl;

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if ( dynamic_cast<QPrinter*>(painter.device()) ) {
            if ( cellRect.bottom() <= rect.bottom() + bottom_penWidth / 2)
                line = QLineF( qMax( rect.left(),   cellRect.left() ),
                               cellRect.bottom(),
                               qMin( rect.right(),  cellRect.right() ),
                               cellRect.bottom() );
        }
        else {
            line = QLineF( cellRect.left(), cellRect.bottom(), cellRect.right(), cellRect.bottom() );
        }
        painter.drawLine( line );
    }

    // FIXME: Look very closely at when the following code is really needed.
    //        I can't really see any case, but I might be wrong.
    //        Since the code below is buggy, and incredibly complex,
    //        I am currently disabling it.  If somebody wants to enable
    //        it again, then please also solve bug 68977: "Embedded KSpread
    //        document printing problem" at the same time.
    return;

#if 0
    // Look at the cells on our corners. It may happen that we
    // just erased parts of their borders corner, so we might need
    // to repaint these corners.
    //
    QPen  vert_pen, horz_pen;
    int   vert_penWidth, horz_penWidth;

    // Some useful referenses.
    Cell  *cell_north     = cell->sheet()->cellAt( cellRef.x(),     cellRef.y() - 1 );
    Cell  *cell_northwest = cell->sheet()->cellAt( cellRef.x() - 1, cellRef.y() - 1 );
    Cell  *cell_west      = cell->sheet()->cellAt( cellRef.x() - 1, cellRef.y() );
    Cell  *cell_northeast = cell->sheet()->cellAt( cellRef.x() + 1, cellRef.y() - 1 );
    Cell  *cell_east      = cell->sheet()->cellAt( cellRef.x() + 1, cellRef.y() );
    Cell  *cell_south     = cell->sheet()->cellAt( cellRef.x(),     cellRef.y() + 1 );
    Cell  *cell_southwest = cell->sheet()->cellAt( cellRef.x() - 1, cellRef.y() + 1 );
    Cell  *cell_southeast = cell->sheet()->cellAt( cellRef.x() + 1, cellRef.y() + 1 );

    // Fix the borders which meet at the top left corner
    if ( cell_north->effLeftBorderValue( cellRef.x(), cellRef.y() - 1 )
         >= cell_northwest->effRightBorderValue( cellRef.x() - 1, cellRef.y() - 1 ) )
        vert_pen = cell_north->effLeftBorderPen( cellRef.x(), cellRef.y() - 1 );
    else
        vert_pen = cell_northwest->effRightBorderPen( cellRef.x() - 1, cellRef.y() - 1 );

    vert_penWidth = qMax( 1, doc->zoomItXOld( vert_pen.width() ) );
    vert_pen.setWidth( vert_penWidth );

    if ( vert_pen.style() != Qt::NoPen ) {
        if ( cell_west->effTopBorderValue( cellRef.x() - 1, cellRef.y() )
             >= cell_northwest->effBottomBorderValue( cellRef.x() - 1, cellRef.y() - 1 ) )
            horz_pen = cell_west->effTopBorderPen( cellRef.x() - 1, cellRef.y() );
        else
            horz_pen = cell_northwest->effBottomBorderPen( cellRef.x() - 1, cellRef.y() - 1 );

        horz_penWidth = qMax( 1, doc->zoomItYOld( horz_pen.width() ) );
        int bottom = ( qMax( 0, -1 + horz_penWidth ) ) / 2 + 1;

        painter.setPen( vert_pen );
        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if ( dynamic_cast<QPrinter*>(painter.device()) ) {
            if ( d->layoutDirection == Sheet::RightToLeft )
                painter.drawLine( qMax( rect.left(), cellRect.right() ),
                                  qMax( rect.top(), cellRect.top() ),
                                  qMin( rect.right(), cellRect.right() ),
                                  qMin( rect.bottom(), cellRect.top() + bottom ) );
            else
                painter.drawLine( qMax( rect.left(), cellRect.left() ),
                                  qMax( rect.top(), cellRect.top() ),
                                  qMin( rect.right(), cellRect.left() ),
                                  qMin( rect.bottom(), cellRect.top() + bottom ) );
        }
        else {
            if ( d->layoutDirection == Sheet::RightToLeft )
                painter.drawLine( cellRect.right(), cellRect.top(),
                                  cellRect.right(), cellRect.top() + bottom );
            else
                painter.drawLine( cellRect.left(), cellRect.top(),
                                  cellRect.left(), cellRect.top() + bottom );
        }
    }

    // Fix the borders which meet at the top right corner
    if ( cell_north->effRightBorderValue( cellRef.x(), cellRef.y() - 1 )
         >= cell_northeast->effLeftBorderValue( cellRef.x() + 1,
                 cellRef.y() - 1 ) )
        vert_pen = cell_north->effRightBorderPen( cellRef.x(), cellRef.y() - 1 );
    else
        vert_pen = cell_northeast->effLeftBorderPen( cellRef.x() + 1,
            cellRef.y() - 1 );

    // vert_pen = effRightBorderPen( cellRef.x(), cellRef.y() - 1 );
    vert_penWidth = qMax( 1, doc->zoomItXOld( vert_pen.width() ) );
    vert_pen.setWidth( vert_penWidth );
    if ( ( vert_pen.style() != Qt::NoPen ) && ( cellRef.x() < KS_colMax ) ) {
        if ( cell_east->effTopBorderValue( cellRef.x() + 1, cellRef.y() )
             >= cell_northeast->effBottomBorderValue( cellRef.x() + 1,
                     cellRef.y() - 1 ) )
            horz_pen = cell_east->effTopBorderPen( cellRef.x() + 1, cellRef.y() );
        else
            horz_pen = cell_northeast->effBottomBorderPen( cellRef.x() + 1,
                cellRef.y() - 1 );

        // horz_pen = cell->effTopBorderPen( cellRef.x() + 1, cellRef.y() );
        horz_penWidth = qMax( 1, doc->zoomItYOld( horz_pen.width() ) );
        int bottom = ( qMax( 0, -1 + horz_penWidth ) ) / 2 + 1;

        painter.setPen( vert_pen );
        //If we are on paper printout, we limit the length of the lines
        //On paper, we always have full cells, on screen not
        if ( dynamic_cast<QPrinter*>(painter.device()) ) {
            if ( d->layoutDirection == Sheet::RightToLeft )
                painter.drawLine( qMax( rect.left(), cellRect.left() ),
                                  qMax( rect.top(), cellRect.top() ),
                                  qMin( rect.right(), cellRect.left() ),
                                  qMin( rect.bottom(), cellRect.top() + bottom ) );
            else
                painter.drawLine( qMax( rect.left(), cellRect.right() ),
                                  qMax( rect.top(), cellRect.top() ),
                                  qMin( rect.right(), cellRect.right() ),
                                  qMin( rect.bottom(), cellRect.top() + bottom ) );
        }
        else {
            if ( d->layoutDirection == Sheet::RightToLeft )
                painter.drawLine( cellRect.left(), cellRect.top(),
                                  cellRect.left(), cellRect.top() + bottom );
            else
                painter.drawLine( cellRect.right(), cellRect.top(),
                                  cellRect.right(), cellRect.top() + bottom );
        }
    }

    // Bottom
    if ( cellRef.y() < KS_rowMax ) {
        // Fix the borders which meet at the bottom left corner
        if ( cell_south->effLeftBorderValue( cellRef.x(), cellRef.y() + 1 )
             >= cell_southwest->effRightBorderValue( cellRef.x() - 1,
                     cellRef.y() + 1 ) )
            vert_pen = cell_south->effLeftBorderPen( cellRef.x(), cellRef.y() + 1 );
        else
            vert_pen = cell_southwest->effRightBorderPen( cellRef.x() - 1,
                cellRef.y() + 1 );

        // vert_pen = effLeftBorderPen( cellRef.x(), cellRef.y() + 1 );
        vert_penWidth = qMax( 1, doc->zoomItYOld( vert_pen.width() ) );
        vert_pen.setWidth( vert_penWidth );
        if ( vert_pen.style() != Qt::NoPen ) {
            if ( cell_west->effBottomBorderValue( cellRef.x() - 1, cellRef.y() )
                 >= cell_southwest->effTopBorderValue( cellRef.x() - 1,
                         cellRef.y() + 1 ) )
                horz_pen = cell_west->effBottomBorderPen( cellRef.x() - 1,
                    cellRef.y() );
            else
                horz_pen = cell_southwest->effTopBorderPen( cellRef.x() - 1,
                    cellRef.y() + 1 );

            // horz_pen = cell->effBottomBorderPen( cellRef.x() - 1, cellRef.y() );
            horz_penWidth = qMax( 1, doc->zoomItXOld( horz_pen.width() ) );
            int bottom = ( qMax( 0, -1 + horz_penWidth ) ) / 2;

            painter.setPen( vert_pen );
            // If we are on paper printout, we limit the length of the lines.
            // On paper, we always have full cells, on screen not.
            if ( dynamic_cast<QPrinter*>(painter.device()) ) {
                if ( d->layoutDirection == Sheet::RightToLeft )
                    painter.drawLine( qMax( rect.left(), cellRect.right() ),
                                      qMax( rect.top(), cellRect.bottom() - bottom ),
                                      qMin( rect.right(), cellRect.right() ),
                                      qMin( rect.bottom(), cellRect.bottom() ) );
                else
                    painter.drawLine( qMax( rect.left(), cellRect.left() ),
                                      qMax( rect.top(), cellRect.bottom() - bottom ),
                                      qMin( rect.right(), cellRect.left() ),
                                      qMin( rect.bottom(), cellRect.bottom() ) );
            }
            else {
                if ( d->layoutDirection == Sheet::RightToLeft )
                    painter.drawLine( cellRect.right(), cellRect.bottom() - bottom,
                                      cellRect.right(), cellRect.bottom() );
                else
                    painter.drawLine( cellRect.left(), cellRect.bottom() - bottom,
                                      cellRect.left(), cellRect.bottom() );
            }
        }

        // Fix the borders which meet at the bottom right corner
        if ( cell_south->effRightBorderValue( cellRef.x(), cellRef.y() + 1 )
             >= cell_southeast->effLeftBorderValue( cellRef.x() + 1, cellRef.y() + 1 ) )
            vert_pen = cell_south->effRightBorderPen( cellRef.x(), cellRef.y() + 1 );
        else
            vert_pen = cell_southeast->effLeftBorderPen( cellRef.x() + 1, cellRef.y() + 1 );

            // vert_pen = effRightBorderPen( cellRef.x(), cellRef.y() + 1 );
        vert_penWidth = qMax( 1, doc->zoomItYOld( vert_pen.width() ) );
        vert_pen.setWidth( vert_penWidth );
        if ( ( vert_pen.style() != Qt::NoPen ) && ( cellRef.x() < KS_colMax ) ) {
            if ( cell_east ->effBottomBorderValue( cellRef.x() + 1, cellRef.y() )
                 >= cell_southeast->effTopBorderValue( cellRef.x() + 1,
                         cellRef.y() + 1 ) )

                horz_pen = cell->sheet()->cellAt( cellRef.x() + 1, cellRef.y() )
                        ->effBottomBorderPen( cellRef.x() + 1, cellRef.y() );
            else
                horz_pen = cell->sheet()->cellAt( cellRef.x() + 1, cellRef.y() + 1 )
                        ->effTopBorderPen( cellRef.x() + 1, cellRef.y() + 1 );

            // horz_pen = cell->effBottomBorderPen( cellRef.x() + 1, cellRef.y() );
            horz_penWidth = qMax( 1, doc->zoomItXOld( horz_pen.width() ) );
            int bottom = ( qMax( 0, -1 + horz_penWidth ) ) / 2;

            painter.setPen( vert_pen );
            // If we are on paper printout, we limit the length of the lines.
            // On paper, we always have full cells, on screen not.
            if ( dynamic_cast<QPrinter*>(painter.device()) )      {
                if ( d->layoutDirection == Sheet::RightToLeft )
                    painter.drawLine( qMax( rect.left(), cellRect.left() ),
                                      qMax( rect.top(), cellRect.bottom() - bottom ),
                                      qMin( rect.right(), cellRect.left() ),
                                      qMin( rect.bottom(), cellRect.bottom() ) );
                else
                    painter.drawLine( qMax( rect.left(), cellRect.right() ),
                                      qMax( rect.top(), cellRect.bottom() - bottom ),
                                      qMin( rect.right(), cellRect.right() ),
                                      qMin( rect.bottom(), cellRect.bottom() ) );
            }
            else {
                if ( d->layoutDirection == Sheet::RightToLeft )
                    painter.drawLine( cellRect.left(), cellRect.bottom() - bottom,
                                      cellRect.left(), cellRect.bottom() );
                else
                    painter.drawLine( cellRect.right(), cellRect.bottom() - bottom,
                                      cellRect.right(), cellRect.bottom() );
            }
        }
    }
#endif
}


// Paint diagonal lines through the cell.
//
void CellView::paintCellDiagonalLines( QPainter& painter, const QRectF &cellRect, Cell* cell )
{
    if ( cell->isPartOfMerged() )
        return;

    QPen fallDiagonalPen( d->style.fallDiagonalPen() );
    QPen goUpDiagonalPen( d->style.goUpDiagonalPen() );

    if ( fallDiagonalPen.style() != Qt::NoPen ) {
        painter.setPen( fallDiagonalPen );
        painter.drawLine( QLineF( cellRect.x(), cellRect.y(), cellRect.right(), cellRect.bottom() ) );
    }

    if ( goUpDiagonalPen.style() != Qt::NoPen ) {
        painter.setPen( goUpDiagonalPen );
        painter.drawLine( QLineF( cellRect.x(), cellRect.bottom(), cellRect.right(), cellRect.y() ) );
    }
}


// Cut strOutText, so that it only holds the part that can be displayed.
//
// Used in paintText().
//
QString CellView::textDisplaying( const QFontMetrics& fm, Cell* cell )
{
  Style::HAlign hAlign = style().halign();
  if ( cell->testFlag( Cell::Flag_CellTooShortX ) )
      hAlign = Style::Left; // force left alignment, if text does not fit

  bool isNumeric = cell->value().isNumber();

  if ( !style().verticalText() )
  {
    // Non-vertical text: the ordinary case.

    // Not enough space but align to left
    double  len = 0.0;
    int     extraXCells = cell->extraXCells();

    for ( int i = cell->column(); i <= cell->column() + extraXCells; i++ )
    {
      ColumnFormat *cl2 = cell->sheet()->columnFormat( i );
      len += cl2->dblWidth() - 1.0; //-1.0 because the pixel in between 2 cells is shared between both cells
    }

    QString  tmp;
    double   tmpIndent = 0.0;
    if ( !cell->isEmpty() )
      tmpIndent = style().indentation();

    // Estimate worst case length to reduce the number of iterations.
    int start = qRound( ( len - 4.0 - 1.0 - tmpIndent ) / fm.width( '.' ) );
    start = qMin( cell->strOutText().length(), start );
    // Start out with the whole text, cut one character at a time, and
    // when the text finally fits, return it.
    for ( int i = start; i != 0; i-- )
    {
      //Note that numbers are always treated as left-aligned since if we have to cut digits off, they should
      //always be the least significant ones at the end of the string
      if ( hAlign == Style::Left || hAlign == Style::HAlignUndefined || isNumeric )
        tmp = cell->strOutText().left(i);
      else if ( hAlign == Style::Right )
        tmp = cell->strOutText().right(i);
      else
        tmp = cell->strOutText().mid( ( cell->strOutText().length() - i ) / 2, i );

      if (isNumeric)
      {
        //For numeric values, we can cut off digits after the decimal point to make it fit,
        //but not the integer part of the number.
        //If this number still contains a fraction part then we don't need to do anything, if we have run
        //out of space to fit even the integer part of the number then display #########
        //TODO Perhaps try to display integer part in standard form if there is not enough room for it?

        if (!tmp.contains('.'))
          cell->d->strOutText = QString().fill( '#', 20 );
      }

      // 4 equal length of red triangle +1 point.
      if ( fm.width( tmp ) + tmpIndent < len - 4.0 - 1.0 )
      {
        if ( style().angle() != 0 )
        {
          QString tmp2;
          RowFormat *rl = cell->sheet()->rowFormat( cell->row() );
          if ( d->textHeight > rl->dblHeight() )
          {
            for ( int j = cell->strOutText().length(); j != 0; j-- )
            {
              tmp2 = cell->strOutText().left( j );
              if ( fm.width( tmp2 ) < rl->dblHeight() - 1.0 )
              {
                return cell->strOutText().left( qMin( tmp.length(), tmp2.length() ) );
              }
            }
          }
          else
            return tmp;

        }
        else
          return tmp;
      }
    }
    return QString( "" );
  }
  else if ( style().verticalText() )
  {
    // Vertical text.

    RowFormat  *rl = cell->sheet()->rowFormat( cell->row() );
    double      tmpIndent = 0.0;

    // Not enough space but align to left.
    double  len = 0.0;
    int     extraXCells = cell->extraXCells();

    for ( int i = cell->column(); i <= cell->column() + extraXCells; i++ ) {
      ColumnFormat  *cl2 = cell->sheet()->columnFormat( i );

      // -1.0 because the pixel in between 2 cells is shared between both cells
      len += cl2->dblWidth() - 1.0;
    }

    if ( !cell->isEmpty() )
      tmpIndent = style().indentation();

    if ( ( d->textWidth + tmpIndent > len ) || d->textWidth == 0.0 )
      return QString( "" );

    for ( int i = cell->strOutText().length(); i != 0; i-- ) {
      if ( fm.ascent() + fm.descent() * i < rl->dblHeight() - 1.0 )
        return cell->strOutText().left( i );
    }

    return QString( "" );
  }

  double         w = d->width + cell->extraWidth();

  QString tmp;
  for ( int i = cell->strOutText().length(); i != 0; i-- ) {
    tmp = cell->strOutText().left( i );

    // 4 equals length of red triangle +1 pixel
    if ( fm.width( tmp ) < w - 4.0 - 1.0 )
      return tmp;
  }

  return  QString();
}


//                        End of Painting
// ================================================================



// Get the effective font.
//
// Used in makeLayout() and calculateTextParameters().
//
QFont CellView::effectiveFont( Cell* cell ) const
{
    QFont tmpFont( d->style.font() );
    // Scale the font size according to the current zoom.
    tmpFont.setPointSizeF( tmpFont.pointSizeF() / cell->sheet()->doc()->resolutionY() );
    return tmpFont;
}



// ================================================================
//                              Layout


// Recalculate the entire layout.  This includes the following members:
//
//   d->textX,     d->textY
//   d->textWidth, d->textHeight
//   d->extraXCells, d->extraYCells
//   cell->extraWidth(),  d->extraHeight
//
// and, of course,
//
//   cell->strOutText
//
void CellView::makeLayout( Cell* cell )
{
  // Are _col and _row really needed ?
  //
  // Yes they are: they are useful if this is the default layout, in
  // which case cell->row() and cell->column() are 0 and 0, but _col and _row
  // are the real coordinates of the cell.

  // Some initializations.
  cell->clearFlag( Cell::Flag_CellTooShortX );
  cell->clearFlag( Cell::Flag_CellTooShortY );

  // Initiate the cells that this one is obscuring to the ones that
  // are actually merged.
  cell->freeAllObscuredCells();
  if (cell->d->hasExtra())
  {
    cell->mergeCells( cell->column(), cell->row(), cell->mergedXCells(), cell->mergedYCells() );
  }

  if ( d->hidden )
    return;

  // Recalculate the output text, cell->strOutText.
  cell->setOutputText();

  // Empty text?  Reset the outstring and, if this is the default
  // cell, return.
  if ( cell->d->strOutText.isEmpty() ) {
    cell->d->strOutText.clear();

    if ( cell->isDefault() ) {
      cell->clearFlag( Cell::Flag_LayoutDirty );
      return;
    }
  }

  // Up to here, we have just cared about the contents, not the
  // painting of it.  Now it is time to see if the contents fits into
  // the cell and, if not, maybe rearrange the outtext a bit.
  //
  // First, Determine the correct font with zoom taken into account.
  // Then calculate text dimensions, i.e. d->textWidth and d->textHeight.
  QFontMetrics fontMetrics( effectiveFont( cell ) );
  textSize( fontMetrics, cell );

  // Calculate the size of the cell.
  calculateCellDimension( cell );

  // Check, if we need to break the line into multiple lines and are
  // allowed to do so.
  breakLines( fontMetrics, cell );

  // Also recalculate text dimensions, i.e. d->textWidth and d->textHeight,
  // because of new line breaks.
  textSize( fontMetrics, cell );

  // Calculate text offset, i.e. d->textX and d->textY.
  textOffset( fontMetrics, cell );

  // Obscure cells, if necessary.
  obscureHorizontalCells( cell );
  obscureVerticalCells( cell );

  cell->clearFlag( Cell::Flag_LayoutDirty );
}


void CellView::calculateCellDimension( const Cell* cell )
{
  double width  = cell->sheet()->columnFormat( cell->column() )->dblWidth();
  double height = cell->sheet()->rowFormat( cell->row() )->dblHeight();

  // Calculate extraWidth and extraHeight if we have a merged cell.
  if ( cell->testFlag( Cell::Flag_Merged ) ) {
    int  extraXCells = cell->extraXCells();
    int  extraYCells = cell->extraYCells();

    // FIXME: Introduce double extraWidth/Height here and use them
    //        instead (see FIXME about this in paintCell()).

    for ( int x = cell->column() + 1; x <= cell->column() + extraXCells; x++ )
      width += cell->sheet()->columnFormat( x )->dblWidth();

    for ( int y = cell->row() + 1; y <= cell->row() + extraYCells; y++ )
      height += cell->sheet()->rowFormat( y )->dblHeight();
  }

  // Cache the newly calculated extraWidth and extraHeight if we have
  // already allocated a struct for it.  Otherwise it will be zero, so
  // don't bother.
  if (cell->d->hasExtra()) {
    cell->d->extra()->extraWidth  = width;
    cell->d->extra()->extraHeight = height;
  }
}


//used in Sheet::adjustColumnHelper and Sheet::adjustRow
void CellView::calculateTextParameters( Cell* cell )
{
  // Get the font metrics for the effective font.
  QFontMetrics fontMetrics( effectiveFont( cell ) );

  // Recalculate text dimensions, i.e. d->textWidth and d->textHeight
  textSize( fontMetrics, cell );

  // Recalculate text offset, i.e. d->textX and d->textY.
  textOffset( fontMetrics, cell );
}

// Recalculate d->textX and d->textY.
//
// Used in makeLayout() and calculateTextParameters().
//
void CellView::textOffset( const QFontMetrics& fontMetrics, Cell* cell )
{
    const double ascent = fontMetrics.ascent();
    int hAlign = d->style.halign();
    Style::VAlign vAlign = d->style.valign();
    int tmpAngle = d->style.angle();
    bool tmpVerticalText = d->style.verticalText();
    bool tmpMultiRow = d->style.wrapText();

  double  w = d->width;
  double  h = d->height;

  // doc coordinate system; no zoom applied
  const double effTop = s_borderSpace + 0.5 * d->style.topBorderPen().width();
  const double effBottom = h - s_borderSpace - 0.5 * d->style.bottomBorderPen().width();

  // Calculate d->textY based on the vertical alignment and a few
  // other inputs.
  switch( vAlign )
  {
    case Style::Top:
    {
      if ( tmpAngle == 0 )
      {
        d->textY = effTop + ascent;
      }
      else if ( tmpAngle < 0 )
      {
        d->textY = effTop;
      }
      else
      {
        d->textY = effTop + ascent * cos( tmpAngle * M_PI / 180 );
      }
      break;
    }
    case Style::Bottom:
    {
      if ( !tmpVerticalText && !tmpMultiRow && !tmpAngle )
      {
        d->textY = effBottom;
      }
      else if ( tmpAngle != 0 )
      {
      // Is enough place available?
        if ( effBottom - effTop - d->textHeight > 0 )
        {
          if ( tmpAngle < 0 )
          {
            d->textY = effBottom - d->textHeight;
          }
          else
          {
            d->textY = effBottom - d->textHeight + ascent * cos( tmpAngle * M_PI / 180 );
          }
        }
        else
        {
          if ( tmpAngle < 0 )
          {
            d->textY = effTop;
          }
          else
          {
            d->textY = effTop + ascent * cos( tmpAngle * M_PI / 180 );
          }
        }
      }
      else if ( (tmpMultiRow || cell->strOutText().contains( '\n' ) ) && !tmpVerticalText )
      {
      // Is enough place available?
        if ( effBottom - effTop - d->textHeight > 0 )
        {
          d->textY = effBottom - d->textHeight + ascent;
        }
        else
        {
          d->textY = effTop + ascent;
        }
      }
      else
      {
      // Is enough place available?
        if ( effBottom - effTop - d->textHeight > 0 )
        {
          d->textY = effBottom - d->textHeight + ascent;
        }
        else
        {
          d->textY = effTop + ascent;
        }
      }
      break;
    }
    case Style::Middle:
    case Style::VAlignUndefined:
    {
      if ( !tmpVerticalText && !tmpMultiRow && !tmpAngle )
      {
        d->textY = ( h - d->textHeight ) / 2 + ascent;
      }
      else if ( tmpAngle != 0 )
      {
      // Is enough place available?
        if ( effBottom - effTop - d->textHeight > 0 )
        {
          if ( tmpAngle < 0 )
          {
            d->textY = ( h - d->textHeight ) / 2;
          }
          else
          {
            d->textY = ( h - d->textHeight ) / 2 + ascent * cos( tmpAngle * M_PI / 180 );
          }
        }
        else
        {
          if ( tmpAngle < 0 )
          {
            d->textY = effTop;
          }
          else
          {
            d->textY = effTop + ascent * cos( tmpAngle * M_PI / 180 );
          }
        }
      }
      else if ( (tmpMultiRow || cell->strOutText().contains( '\n' ) ) && !tmpVerticalText )
      {
      // Is enough place available?
        if ( effBottom - effTop - d->textHeight > 0 )
        {
          d->textY = ( h - d->textHeight ) / 2 + ascent;
        }
        else
        {
          d->textY = effTop + ascent;
        }
      }
      else
      {
      // Is enough place available?
        if ( effBottom - effTop - d->textHeight > 0 )
        {
          d->textY = ( h - d->textHeight ) / 2 + ascent;
        }
        else
          d->textY = effTop + ascent;
      }
      break;
    }
  }

  hAlign = d->style.halign();
  if ( cell->sheet()->getShowFormula() && !( cell->sheet()->isProtected() && style().hideFormula() ) )
  {
    hAlign = Style::Left;
  }

  // Calculate d->textX based on alignment and textwidth.
  switch ( hAlign ) {
    case Style::Left:
      d->textX = 0.5 * d->style.leftBorderPen().width() + s_borderSpace;
      break;
    case Style::Right:
      d->textX = w - s_borderSpace - d->textWidth
          - 0.5 * d->style.rightBorderPen().width();
      break;
    case Style::Center:
      d->textX = 0.5 * ( w - s_borderSpace - d->textWidth -
          0.5 * d->style.rightBorderPen().width() );
      break;
  }
}


// Recalculate the current text dimensions, i.e. d->textWidth and
// d->textHeight.
//
// Used in makeLayout() and calculateTextParameters().
//
void CellView::textSize( const QFontMetrics& fm, const Cell* cell )
{
    Style::VAlign vAlign = d->style.valign();
    int tmpAngle = d->style.angle();
    bool tmpVerticalText = d->style.verticalText();
    bool fontUnderlined = d->style.underline();

    // Set d->textWidth and d->textHeight to correct values according to
    // if the text is horizontal, vertical or rotated.
    if ( !tmpVerticalText && !tmpAngle ) {
        // Horizontal text.

        d->textWidth = ( fm.size( 0, cell->strOutText() ).width() );
        int offsetFont = 0;
        if ( ( vAlign == Style::Bottom ) && fontUnderlined ) {
            offsetFont = fm.underlinePos() + 1;
        }

        d->textHeight = ( fm.ascent() + fm.descent() + offsetFont )
                * ( cell->strOutText().count( '\n' ) + 1 );
    }
    else if ( tmpAngle!= 0 ) {
        // Rotated text.

        const double height = fm.ascent() + fm.descent();
        const double width  = fm.width( cell->strOutText() );
        d->textHeight = height * cos( tmpAngle * M_PI / 180 ) + qAbs( width * sin( tmpAngle * M_PI / 180 ) );

        d->textWidth = qAbs( height * sin( tmpAngle * M_PI / 180 ) ) + width * cos( tmpAngle * M_PI / 180 );
    }
    else {
        // Vertical text.

        int width = 0;
        for ( int i = 0; i < cell->strOutText().length(); i++ )
            width = qMax( width, fm.width( cell->strOutText().at( i ) ) );

        d->textWidth  = width;
        d->textHeight = ( fm.ascent() + fm.descent() ) * cell->strOutText().length();
    }
}

void CellView::breakLines( const QFontMetrics& fontMetrics, Cell* cell )
{
  if ( style().wrapText() &&
       d->textWidth > ( d->width - 2 * s_borderSpace
           - 0.5 * style().leftBorderPen().width() - 0.5 * style().rightBorderPen().width() &&
       // FIXME Stefan: consider this later in the wrap algo.
       d->textHeight + fontMetrics.ascent() + fontMetrics.descent() < d->height ) )
  {
    // don't remove the existing LF, these are intended line wraps (whishlist #9881)
    QString outText = cell->strOutText();

    // Break the line at appropriate places, i.e. spaces, if
    // necessary.  This means to change the spaces where breaks occur
    // into newlines.
    if ( !outText.contains(' ') )
    {
      // no spaces -> impossible to wrap
      return;
    }
    else
    {
      cell->d->strOutText.clear();

      // Make sure that we have a space at the end.
      outText += ' ';

      int start = 0;    // Start of the line we are handling now
      int breakpos = 0;   // The next candidate pos to break the string
      int pos1 = 0;
      int availableWidth = (int) ( d->width - 2 * s_borderSpace
          - 0.5 * style().leftBorderPen().width() - 0.5 * style().rightBorderPen().width() );

      do {

        breakpos = outText.indexOf( ' ', breakpos );
        int linefeed = outText.indexOf( '\n', pos1 );

//         kDebug(36004) << "start: " << start << "; breakpos: " << breakpos << "; pos1: " << pos1 << "; linefeed: " << linefeed << endl;

        //don't miss LF as a position to calculate current lineWidth
        int work_breakpos = breakpos;
        if (pos1 < linefeed && linefeed < breakpos)
          work_breakpos = linefeed;

        double lineWidth = fontMetrics.width( cell->strOutText().mid( start, (pos1 - start) )
                                              + outText.mid( pos1, work_breakpos - pos1 ) );

        //linefeed could be -1 when no linefeed is found!
        if (breakpos > linefeed && linefeed > 0)
        {
//           kDebug(36004) << "applying linefeed to start;" << endl;
          start = linefeed;
        }

        if ( lineWidth <= availableWidth ) {
            // We have room for the rest of the line.  End it here.
          cell->d->strOutText += outText.mid( pos1, breakpos - pos1 );
          pos1 = breakpos;
        }
        else {
          // Still not enough room.  Try to split further.
          if ( outText.at( pos1 ) == ' ' )
            pos1++;

          if ( pos1 != 0 && breakpos != -1 ) {
            cell->d->strOutText += '\n' + outText.mid( pos1, breakpos - pos1 );
          }
          else
            cell->d->strOutText += outText.mid( pos1, breakpos - pos1 );

          start = pos1;
          pos1 = breakpos;
        }

        breakpos++;
      } while( outText.indexOf( ' ', breakpos ) != -1 );
    }
  }
}

void CellView::obscureHorizontalCells( Cell* masterCell )
{
  double width = masterCell->dblWidth( masterCell->column() );

  int align = d->style.halign();

  // Get indentation.  This is only used for left aligned text.
  double indent = 0.0;
  if ( align == Style::Left && !masterCell->isEmpty() )
  {
    indent = style().indentation();
  }

  // Set Cell::Flag_CellTooShortX if the text is vertical or angled, and too
  // high for the cell.
  if ( style().verticalText() || style().angle() != 0 )
  {
    if ( d->textHeight >= d->height )
      masterCell->setFlag( Cell::Flag_CellTooShortX );
  }


  // Do we have to occupy additional cells to the right?  This is only
  // done for cells that have no merged cells in the Y direction.
  //
  // FIXME: Check if all cells along the merged edge to the right are
  //        empty and use the extra space?  No, probably not.
  //
  if ( d->textWidth + indent > ( d->width - 2 * s_borderSpace
       - style().leftBorderPen().width() - style().rightBorderPen().width() ) &&
       masterCell->mergedYCells() == 0 )
  {
    int col = masterCell->column();

    // Find free cells to the right of this one.
    int end = 0;
    while ( !end )
    {
      ColumnFormat* cl2 = masterCell->sheet()->columnFormat( col + 1 );
      Cell* nextCell = masterCell->sheet()->visibleCellAt( col + 1, masterCell->row() );

      if ( nextCell->isEmpty() )
      {
        width += cl2->dblWidth() - 1;
        col++;

        // Enough space?
        if ( d->textWidth + indent <= ( width - 2 * s_borderSpace
             - style().leftBorderPen().width() - style().rightBorderPen().width() ) )
          end = 1;
      }
      else
        // Not enough space, but the next cell is not empty
        end = -1;
    }

    // Try to use additional space from the neighboring cells that
    // were calculated in the last step.
    //
    // Currently this is only done for left aligned cells. We have to
    // check to make sure we haven't already force-merged enough cells
    //
    // FIXME: Why not right/center aligned text?
    //
    // FIXME: Shouldn't we check to see if end == -1 here before
    //        setting Cell::Flag_CellTooShortX?
    //
    if ( style().halign() == Style::Left || ( style().halign() == Style::HAlignUndefined
         && !masterCell->value().isNumber() ) )
    {
      if ( col - masterCell->column() > masterCell->mergedXCells() ) {
        masterCell->d->extra()->extraXCells = col - masterCell->column();
        masterCell->d->extra()->extraWidth = width;
        for ( int i = masterCell->column() + 1; i <= col; ++i ) {
          Cell* cell = masterCell->sheet()->nonDefaultCell( i, masterCell->row() );
          cell->obscure( masterCell );
        }

        // Not enough space
        if ( end == -1 )
          masterCell->setFlag( Cell::Flag_CellTooShortX );
      }
      else
        masterCell->setFlag( Cell::Flag_CellTooShortX );
    }
    else
      masterCell->setFlag( Cell::Flag_CellTooShortX );
  }
}

void CellView::obscureVerticalCells( Cell* masterCell )
{
  double height = masterCell->dblHeight( masterCell->row() );

  // Do we have to occupy additional cells at the bottom ?
  //
  // FIXME: Setting to make the current cell grow.
  //
  if ( masterCell->strOutText().contains( '\n' ) &&
       d->textHeight > ( d->height - 2 * s_borderSpace
       - style().topBorderPen().width()
       - style().bottomBorderPen().width() ) )
  {
    int row = masterCell->row();
    int end = 0;

    // Find free cells bottom to this one
    while ( !end ) {
      RowFormat* rl2  = masterCell->sheet()->rowFormat( row + 1 );
      Cell* nextCell = masterCell->sheet()->visibleCellAt( masterCell->column(), row + 1 );

      if ( nextCell->isEmpty() ) {
        height += rl2->dblHeight() - 1.0;
        row++;

        // Enough space ?
        if ( d->textHeight <= ( height - 2 * s_borderSpace
             - style().topBorderPen().width() - style().bottomBorderPen().width() ) )
          end = 1;
      }
      else
        // Not enough space, but the next cell is not empty.
        end = -1;
    }

    // Check to make sure we haven't already force-merged enough cells.
    if ( row - masterCell->row() > masterCell->mergedYCells() )
    {
      masterCell->d->extra()->extraYCells = row - masterCell->row();
      masterCell->d->extra()->extraHeight = height;

      for ( int i = masterCell->row() + 1; i <= row; ++i )
      {
        Cell  *cell = masterCell->sheet()->nonDefaultCell( masterCell->column(), i );
        cell->obscure( masterCell );
      }

      // Not enough space
      if ( end == -1 )
        masterCell->setFlag( Cell::Flag_CellTooShortY );
    }
    else
      masterCell->setFlag( Cell::Flag_CellTooShortY );
  }
}
