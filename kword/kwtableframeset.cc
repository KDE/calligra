/*
    Copyright (C) 2001, S.R.Haque (srhaque@iee.org).
    This file is part of the KDE project

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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION

    This file implements KWord tables.
*/

#include <kdebug.h>
#include <klocale.h>
#include "kwdoc.h"
#include "kwanchor.h"
#include "kwtableframeset.h"
#include "kwcanvas.h"
#include "kwcommand.h"
#include "kwviewmode.h"
#include "kwview.h"

// I changed this to 1 because it fixes cell border drawing in the normal case
// and because e.g. msword has no big cell-spacing by default.
// The real fix here would be to have a spacing per line and per row, that depends
// on the border (max of the borders of all cells on the line)
const unsigned int KWTableFrameSet::tableCellSpacing = 1; // 3;

KWTableFrameSet::KWTableFrameSet( KWDocument *doc, const QString & name ) :
    KWFrameSet( doc )
{
    m_rows = 0;
    m_cols = 0;
    m_name = QString::null;
    m_showHeaderOnAllPages = true;
    m_hasTmpHeaders = false;
    m_active = true;
    m_cells.setAutoDelete( true );
    frames.setAutoDelete(false);
    //m_anchor = 0L;
    if ( name.isEmpty() )
        m_name = doc->generateFramesetName( i18n( "Table %1" ) );
    else
        m_name = name;
}

KWTableFrameSet::KWTableFrameSet( KWTableFrameSet &original ) :
    KWFrameSet( original.m_doc )
{
    m_doc = original.m_doc;
    m_rows = original.m_rows;
    m_cols = original.m_cols;
    m_name = original.m_name + '_'; // unicity problem !
    m_showHeaderOnAllPages = original.m_showHeaderOnAllPages;
    m_hasTmpHeaders = original.m_hasTmpHeaders;
    m_active = original.m_active;
    m_cells.setAutoDelete( true );
    //m_anchor = 0L;

    // Copy all cells.
    for ( unsigned int i = 0; i < original.m_cells.count(); i++ )
    {
        Cell *cell = new Cell( this, *original.m_cells.at( i ) );
        m_cells.append( cell );
    }
    m_doc->addFrameSet( this );
    frames.setAutoDelete(false);
}

KWTableFrameSet::~KWTableFrameSet()
{
    //delete m_anchor;
    //m_anchor = 0L;
    m_doc = 0L;
}

KWFrameSetEdit * KWTableFrameSet::createFrameSetEdit( KWCanvas * canvas )
{
    return new KWTableFrameSetEdit( this, canvas );
}

void KWTableFrameSet::updateFrames()
{
    if(m_cells.count()==0)
        return;
    for (unsigned int i =0; i < m_cells.count(); i++)
    {
        m_cells.at(i)->updateFrames();
        m_cells.at(i)->invalidate();
    }
    KWFrameSet::updateFrames();
}

void KWTableFrameSet::moveFloatingFrame( int /*frameNum TODO */, const QPoint &position )
{
    Cell * cell = getCell( 0, 0 );
    ASSERT( cell );
    if ( !cell ) return;
    KoPoint currentPos = cell->getFrame( 0 )->topLeft();
    QPoint pos( position );
    // position includes the border, we need to adjust accordingly
    pos.rx() += Border::zoomWidthX( cell->getFrame( 0 )->leftBorder().ptWidth, m_doc, 1 );
    pos.ry() += Border::zoomWidthY( cell->getFrame( 0 )->topBorder().ptWidth, m_doc, 1 );
    // Now we can unzoom
    KoPoint kopos = m_doc->unzoomPoint( pos );
    if ( currentPos != kopos )
    {
        kdDebug() << "KWTableFrameSet::moveFloatingFrame " << kopos.x() << "," << kopos.y() << endl;
        KoPoint offset = kopos - currentPos;
        moveBy( offset.x(), offset.y() );

        // Recalc all "frames on top" everywhere
        kWordDocument()->updateAllFrames();
        // Don't call any drawing method from here.
        // We are called from KWAnchor::draw, inside a paintevent, so
        // we are not allowed to create a paint event ourselves.
        // KWAnchor draws the table anyway!
    }
}

QSize KWTableFrameSet::floatingFrameSize( int /*frameNum TODO */ )
{
    KoRect r = boundingRect();
    if ( r.isNull() )
        return QSize();
    QRect outerRect( m_doc->zoomRect( r ) );
    //kdDebug() << "floatingFrameSize outerRect initially " << DEBUGRECT( outerRect ) << endl;
    QSize sz = outerRect.size();
    ASSERT( m_anchorTextFs );
    // Need to convert back to internal coords (in case of page breaking)
    QPoint iPoint;
    if ( m_anchorTextFs->normalToInternal( outerRect.topLeft(), iPoint ) )
    {
        outerRect.setLeft( iPoint.x() );
        outerRect.setTop( iPoint.y() );
        QPoint brnPoint; // bottom right in internal coords
        if ( m_anchorTextFs->normalToInternal( outerRect.bottomRight(), brnPoint ) )
        {
            outerRect.setRight( brnPoint.x() );
            outerRect.setBottom( brnPoint.y() );
            //kdDebug() << "floatingFrameSize outerRect now " << DEBUGRECT( outerRect ) << endl;
            sz = outerRect.size();
        }
    }

    // TODO: in theory, we'd need to take the max of the borders of each cell
    // on the outside rect, to find the global rect needed. Well, if we assume constant
    // cell borders for now it's much simpler.
    KWFrame * firstCell = m_cells.getFirst()->getFrame( 0 );
    KWFrame * lastCell = m_cells.getLast()->getFrame( 0 );
    if ( firstCell && lastCell )
    {
        sz.rwidth() += Border::zoomWidthX( firstCell->leftBorder().ptWidth, m_doc, 1 )
                       + Border::zoomWidthX( lastCell->rightBorder().ptWidth, m_doc, 1 );
        sz.rheight() += Border::zoomWidthY( firstCell->topBorder().ptWidth, m_doc, 1 )
                        + Border::zoomWidthY( lastCell->bottomBorder().ptWidth, m_doc, 1 );
    }
    return sz;
}

KCommand * KWTableFrameSet::anchoredObjectCreateCommand( int /*frameNum*/ )
{
    return new KWCreateTableCommand( i18n("Create table"), this );
}

KCommand * KWTableFrameSet::anchoredObjectDeleteCommand( int /*frameNum*/ )
{
    return new KWDeleteTableCommand( i18n("Delete table"), this );
}

KWAnchor * KWTableFrameSet::createAnchor( KWTextDocument * textdoc, int frameNum )
{
    kdDebug() << "KWTableFrameSet::createAnchor" << endl;
    return new KWAnchor( textdoc, this, frameNum );
}

void KWTableFrameSet::createAnchors( KWTextParag * parag, int index, bool placeHolderExists /*= false */ /*only used when loading*/ )
{
    kdDebug() << "KWTableFrameSet::createAnchors" << endl;
    // TODO make one rect per page, and create one anchor per page
    //if ( !m_anchor )
    {
        // Anchor this frame, after the previous one
        KWAnchor * anchor = createAnchor( m_anchorTextFs->textDocument(), 0 );
        if ( !placeHolderExists )
            parag->insert( index, KWTextFrameSet::customItemChar() );
        parag->setCustomItem( index, anchor, 0 );
        kdDebug() << "KWTableFrameSet::createAnchors setting anchor" << endl;
    }
    parag->setChanged( true );
    emit repaintChanged( m_anchorTextFs );
}

void KWTableFrameSet::deleteAnchors()
{
    KWAnchor * anchor = findAnchor( 0 );
    kdDebug() << "KWTableFrameSet::deleteAnchors anchor=" << anchor << endl;
    deleteAnchor( anchor );
}

void KWTableFrameSet::addCell( Cell *cell )
{
    unsigned int i;

    m_rows = QMAX( cell->m_row + 1, m_rows );
    m_cols = QMAX( cell->m_col + 1, m_cols );

    // Find the insertion point in the list.
    for ( i = 0; i < m_cells.count() && m_cells.at( i )->isAboveOrLeftOf( cell->m_row, cell->m_col ); i++ ) ;
    // cell->setName( m_name + ' ' + cell->m_col + ',' + cell->m_row ); // already done in constructor
    m_cells.insert( i, cell );

}

KoRect KWTableFrameSet::boundingRect()
{
    KWFrame *first = getCell( 0, 0 )->getFrame( 0 );
    ASSERT(first);
    ASSERT(getCell( m_rows - 1, m_cols - 1 ));
    //laurent
    //when you insert a column or row
    //we test last cell but you resize anchor
    //each time that you add a cell you execute this function
    //but cell(m_rows - 1, m_cols - 1) doesn't exist
    //the first time.
    //So return koRect() to fix crash
    if(!getCell( m_rows - 1, m_cols - 1 ))
        return KoRect();

    KWFrame *last = getCell( m_rows - 1, m_cols - 1 )->getFrame( 0 );
    ASSERT(last);
    if (!first || !last)
        return KoRect();

    //kdDebug() << "KWTableFrameSet::boundingRect first=" << DEBUGRECT( *first ) << " last=" << DEBUGRECT( *last ) << endl;
    return first->unite( *last );
}

/* returns the cell that occupies row, col. */
KWTableFrameSet::Cell *KWTableFrameSet::getCell( unsigned int row, unsigned int col )
{
    for ( unsigned int i = 0; i < m_cells.count(); i++ )
    {
        Cell *cell = m_cells.at( i );
        if ( cell->m_row <= row &&
                cell->m_col <= col &&
                cell->m_row + cell->m_rows > row &&
                cell->m_col + cell->m_cols > col )
        {
            return cell;
        }
    }
    return 0L;
}

KWTableFrameSet::Cell *KWTableFrameSet::getCellByPos( double x, double y )
{
    KWFrame *f = frameAtPos(x,y);
    if(f) return static_cast<KWTableFrameSet::Cell *> (f->getFrameSet());
    return 0L;
}

bool KWTableFrameSet::isTableHeader( Cell *cell )
{
    return cell->isRemoveableHeader() || ( cell->m_row == 0 );
}

void KWTableFrameSet::recalcCols(int _col,int _row)
{
    unsigned int row=0,col=0;
    if(! m_cells.isEmpty() ) {
        //get selected cell
        if(_col!=-1 && _row!=-1)
        {
            row=(unsigned int)_row;
            col=(unsigned int)_col;
        }
        else
            isOneSelected(row,col);
        // ** check/set sizes of frames **
        // we assume only left or only right pos has changed.
        // check if leftCoordinate is same as rest of tableRow
        Cell *activeCell = getCell(row,col);
        Cell *cell;
        double coordinate;
        // find old coord.
        coordinate=activeCell->getFrame(0)->left();
        if(col!=0) { // calculate the old position.
            coordinate = getCell(row, col-1)->getFrame(0)->right() + tableCellSpacing;
        } else { // is leftmost, so lets look at other rows..
            for ( unsigned int i = 0; i < m_rows; i++) {
                if( !(i>=row &&i<=(activeCell->m_rows+activeCell->m_row-1))) {
                    cell=getCell(i,col);
                    if(cell && cell->m_col==col) {
                        coordinate=cell->getFrame(0)->left();
                        break;
                    }
                }
            }
        }
        double postAdjust=0;
        if(coordinate != activeCell->getFrame(0)->left()) { // left pos changed
            // we are now going to move the rest of the cells in this column as well.
            for ( unsigned int i = 0; i < m_rows; i++) {
                double difference=0;
                if(col==0) {// left most cell
                    cell = getCell(i,col);
                    if(!cell)
                        continue;
                    if(cell==activeCell)
                        cell=0;
                    else
                        difference = -(activeCell->getFrame(0)->left() - coordinate);
                } else {
                    cell = getCell(i,col-1);
                    if(!cell)
                        continue;
                    if(cell->m_row == i) // dont resize joined cells more then ones.
                        difference=activeCell->getFrame(0)->left() - coordinate;
                    else
                        cell=0;
                }
                if(cell) {
                    // rescale this cell with the calculated difference
                    double newWidth=cell->getFrame(0)->width() + difference;
                    if(newWidth<minFrameWidth) {
                        if(static_cast<double>(minFrameWidth-newWidth) > postAdjust)
                            postAdjust = minFrameWidth-newWidth;
                    }
                    cell->getFrame(0)->setWidth(newWidth);
                }
            }

            // Because we are scaling the cells left of this one, the activeCell has to be
            // returned to its original size.
            if(col!=0)
                activeCell->getFrame(0)->setWidth(
                  activeCell->getFrame(0)->width() +
                  activeCell->getFrame(0)->left() - coordinate);

            // if we found cells that ware made to small, we adjust them using the postAdjust var.
            for ( unsigned int i = 0; i < m_rows; i++) {
                if(col==0)
                    col++;
                cell = getCell(i,col-1);
                if(cell && cell->m_row == i)
                    cell->getFrame(0)->setWidth( cell->getFrame(0)->width()+postAdjust);
            }
        } else {
            col+=activeCell->m_cols-1;
            // find old coord.
            coordinate=activeCell->getFrame(0)->right();
            bool found=false;
            for ( unsigned int i = 0; i < m_rows; i++) {
                if(!((i>=row && i<=(activeCell->m_rows+activeCell->m_row-1)))) {
                    cell=getCell(i,activeCell->m_cols+activeCell->m_col-1);
                    if(cell && cell->m_col+cell->m_cols==activeCell->m_cols+activeCell->m_col) {
                        coordinate=cell->getFrame(0)->right();
                        found=true;
                        break;
                    }
                }
            }

            if(! found && activeCell->m_col + activeCell->m_cols < m_cols) { // if we did not find it and we are not on the right edge of the table.
               // use the position of the next cell.
               coordinate = getCell(activeCell->m_row, activeCell->m_col + activeCell->m_cols)->getFrame(0)->left() - tableCellSpacing;
            }
            if(coordinate != activeCell->getFrame(0)->right()) { // right pos changed.
                for ( unsigned int i = 0; i < m_rows; i++) {
                    Cell *cell = getCell(i,col);
                    if(cell && (cell != activeCell && cell->m_row == i)) {
                        double newWidth = cell->getFrame(0)->width() +
                            activeCell->getFrame(0)->right() - coordinate;
                        if(newWidth<minFrameWidth) {
                            if(minFrameWidth-newWidth > postAdjust)
                                postAdjust = minFrameWidth-newWidth;
                        }
                        cell->getFrame(0)->setWidth(newWidth);
                    }
                }
                for ( unsigned int i = 0; i < m_rows; i++) {
                    cell = getCell(i,col);
                    if(cell && (cell->m_row == i))
                        cell->getFrame(0)->setWidth( cell->getFrame(0)->width()+postAdjust);
                }
            }
        }
        // Move cells
        double x, nextX=0;
        if(getCell(0,0) &&  getCell( 0, 0 )->getFrame( 0 ))
            nextX =getCell( 0, 0 )->getFrame( 0 )->x();
        for ( unsigned int i = 0; i < m_cols; i++ ) {
            x=nextX;
            for ( unsigned int j = 0; j < m_rows; j++ ) {
                Cell *cell = getCell(j,i);
                if(!cell)
                    continue;
                if(cell->m_col==i && cell->m_row==j) {
                    cell->getFrame( 0 )->moveTopLeft( KoPoint( x, cell->getFrame( 0 )->y() ) );
                }
                if(cell->m_col + cell->m_cols -1 == i)
                    nextX=cell->getFrame(0) -> right() + tableCellSpacing;
            }
        }
    }
}

void KWTableFrameSet::recalcRows(int _col, int _row)
{
    //kdDebug() << "KWTableFrameSet::recalcRows" << endl;
    // remove automatically added headers
    for ( unsigned int j = 0; j < m_rows; j++ ) {
        Cell *tmp=getCell( j, 0 );
        ASSERT(tmp);
        if ( tmp && tmp->isRemoveableHeader() ) {
            //kdDebug() << "KWTableFrameSet::recalcRows removing temp row " << j << endl;
            deleteRow( j, false );
            j--;
        }
    }
    m_hasTmpHeaders = false;
    // check/set sizes of frames
    unsigned int row=0,col=0;
    if(!m_cells.isEmpty() /*&& isOneSelected(row,col)*/) {
        if(_col!=-1 && _row!=-1)
        {
            row=(unsigned int)_row;
            col=(unsigned int)_col;
        }
        else
            isOneSelected(row,col);

        // check if topCoordinate is same as rest of tableRow
        Cell *activeCell = getCell(row,col);
        Cell *cell;
        double coordinate;

        // find old coord.
        coordinate=activeCell->getFrame(0)->top();
        for ( unsigned int i = 0; i < m_cols; i++) {

            if( !(i>=col && i<=(activeCell->m_col+activeCell->m_cols-1))) {
                cell=getCell(row,i);
                ASSERT(cell);
                if(cell && cell->m_row==row) {
                    coordinate=cell->getFrame(0)->top();
                    break;
                }
            }
        }
        double postAdjust=0;
        if(coordinate != activeCell->getFrame(0)->top()) { // top pos changed
            for ( unsigned int i = 0; i < m_cols; i++) {
                double difference=0;
                if(row==0) { // top cell
                    cell = getCell(0,i);
                    if(cell==activeCell)
                        cell=0;
                    else
                        difference = - ( activeCell->getFrame(0)->top() - coordinate );
                } else {
                    cell = getCell(row-1,i);
                    if(cell->m_col == i) // dont resize joined cells more then ones.
                        difference = activeCell->getFrame(0)->top() - coordinate;
                    else
                        cell=0;
                }
                if(cell) {
                    double newHeight= cell->getFrame(0)->height() + difference;
                    if(newHeight<minFrameHeight) {
                        if(minFrameHeight-newHeight > postAdjust)
                            postAdjust = minFrameHeight-newHeight;
                    }
                    cell->getFrame(0)->setHeight(newHeight);
                    cell->getFrame(0)->setMinFrameHeight(newHeight);
                }
            }
            if(row!=0) {
                double newHeight = activeCell->getFrame(0)->height() +
                    activeCell->getFrame(0)->top()- coordinate;
                activeCell->getFrame(0)->setHeight(newHeight);
                activeCell->getFrame(0)->setMinFrameHeight(newHeight);
            }
            if(postAdjust!=0) {
                if(row==0) row++;
                for ( unsigned int i = 0; i < m_cols; i++) {
                    cell = getCell(row-1,i);
                    if(cell->m_col == i)
                        cell->getFrame(0)->setHeight(
                            cell->getFrame(0)->height() + postAdjust);
                        cell->getFrame(0)->setMinFrameHeight(
                            cell->getFrame(0)->height() + postAdjust);
                }
            }
        } else { // bottom pos has changed
            row+=activeCell->m_rows-1;
            // find old coord.
            coordinate=activeCell->getFrame(0)->bottom();
            for ( unsigned int i = 0; i < m_cols; i++) {
                if(!(i>=col && i<=(activeCell->m_col+activeCell->m_cols-1))) {
                    cell=getCell(activeCell->m_row+activeCell->m_rows-1,i);
                    if(cell->m_row+cell->m_rows==activeCell->m_row+activeCell->m_rows) {
                        coordinate=cell->getFrame(0)->bottom();
                        break;
                    }
                }
            }
            if(coordinate != activeCell->getFrame(0)->bottom()) {
                for ( unsigned int i = 0; i < m_cols; i++) {
                    cell = getCell(row,i);
                    ASSERT(cell);
                    ASSERT(activeCell);
                    if(cell && cell != activeCell && cell->m_col == i) {
                        double newHeight= cell->getFrame(0)->height() +
                            activeCell->getFrame(0)->bottom() - coordinate;
                        if(newHeight<minFrameHeight) {
                            if(minFrameHeight-newHeight > postAdjust)
                                postAdjust = minFrameHeight-newHeight;
                        }
                        cell->getFrame(0)->setHeight(newHeight);
                        cell->getFrame(0)->setMinFrameHeight(newHeight);
                    }
                }
            }
            if(postAdjust!=0) {
                for ( unsigned int i = 0; i < m_cols; i++) {
                    cell = getCell(row,i);
                    if(cell->m_col == i) {
                        cell->getFrame(0)->setHeight( cell->getFrame(0)->height() + postAdjust);
                        cell->getFrame(0)->setMinFrameHeight( cell->getFrame(0)->height() + postAdjust);
                    }
                }
            }
        }
    }

    // do positioning of frames
    double y, nextY = getCell( 0, 0 )->getFrame( 0 )->y();
    unsigned int doingPage = getCell(0,0)->getFrame(0)->pageNum();
    m_pageBoundaries.clear();
    m_pageBoundaries.append(0);
    for ( unsigned int j = 0; j < m_rows; j++ ) {
        y=nextY;
        unsigned int i = 0;

        for ( i = 0; i < m_cols; i++ ) {
            Cell *cell = getCell(j,i);
            if(!cell)
                continue;
            if(!(cell && cell->getFrame(0))) { // sanity check.
                kdDebug(32002) << "screwy table cell!! row:" << cell->m_row << ", col: " << cell->m_col << endl;
                continue;
            }
            if(cell->m_col==i && cell->m_row==j) { // beware of multi cell frames.
                cell->getFrame( 0 )->moveTopLeft( KoPoint( cell->getFrame( 0 )->x(), y ) );
                //cell->getFrame( 0 )->setPageNum(doingPage);
            }
            if(cell->m_row + cell->m_rows -1 == j)
                nextY=cell->getFrame(0) -> bottom() + tableCellSpacing;
        }
        // check all cells on this row if one might have fallen off the page.
        if( j == 0 ) continue;
        unsigned int fromRow=j;
        bool _addRow = false;
        bool hugeRow = false;
        double pageBottom = (doingPage+1) * m_doc->ptPaperHeight() - m_doc->ptBottomBorder();
        double pageHeight = m_doc->ptPaperHeight() - m_doc->ptBottomBorder() - m_doc->ptTopBorder();
        for(i = 0; i < m_cols; i++) {
            Cell *cell = getCell(j,i);
            if(!cell)
                continue;
            KWFrameSet *fs=cell;
            if(cell->m_row < fromRow)
                fromRow = cell->m_row;
            if ( fs->getFrame( 0 )->bottom() > pageBottom ) {
                 if ( fs->getFrame( 0 )->height() < pageHeight ) {
                     // doesn't fit on the page - but would fit on an empty page
                     kdDebug(32002) << "KWTableFrameSet::recalcRows cell " << j << " " << i << " doesn't fit on page " << doingPage << endl;
                     y = (unsigned)( (doingPage+1) * m_doc->ptPaperHeight() + m_doc->ptTopBorder() );
                     _addRow = true;
                 } else
                     hugeRow = true;
            }
        }
        if ( hugeRow ) {
            // Row is too big, we don't split it nor insert a temp header
            doingPage++;
        }
        else if ( _addRow ) {
            j=fromRow;
            doingPage++;

            if ( y >= m_doc->ptPaperHeight() * m_doc->getPages() )
                m_doc->appendPage();

            // No header rows for floating tables. If we want one,  then we have to
            // fix adjustFlow somehow, so that it doesn't move down tables if it can break them
            // between cells (!)
            bool addHeaderRow = m_showHeaderOnAllPages && !isFloating();
            if ( addHeaderRow )
            {
                // Refuse to create a huge header row, it makes no sense
                // When the first line is the one that's higher than the page, we end up with an infinite loop
                if ( !getCell( 0, 0 ) || getCell( 0, 0 )->getFrame(0)->height() > m_doc->ptPaperHeight() / 2 )
                    addHeaderRow = false;
            }
            if ( addHeaderRow ) {
                //kdDebug() << "KWTableFrameSet::recalcRows adding header at row " << j << endl;
                m_hasTmpHeaders = true;
                QList<KWFrameSet> listFrameSet=QList<KWFrameSet>();
                QList<KWFrame> listFrame=QList<KWFrame>();
                insertRow( j, listFrameSet, listFrame, false, true );
            }
            for(i = 0; i < m_cells.count(); i++) {
                Cell *cell = m_cells.at(i);
                if(cell->m_row==j+1) cell->getFrame(0)->updateResizeHandles(); // reposition resize handles.
                if(cell->m_row!=j) continue; //  wrong row
                if(cell->m_col != 1) m_pageBoundaries.append(i); // new page boundary
                if ( addHeaderRow ) {
                    KWTextFrameSet *baseFrameSet = getCell( 0, cell->m_col );
                    KWFrame *newFrame = cell->getFrame(0);
                    //newFrameSet->assign( baseFrameSet );

                    newFrame->setBackgroundColor( baseFrameSet->getFrame( 0 )->getBackgroundColor() );
                    newFrame->setLeftBorder( baseFrameSet->getFrame( 0 )->leftBorder() );
                    newFrame->setRightBorder( baseFrameSet->getFrame( 0 )->rightBorder() );
                    newFrame->setTopBorder( baseFrameSet->getFrame( 0 )->topBorder() );
                    newFrame->setBottomBorder( baseFrameSet->getFrame( 0 )->bottomBorder() );
                    newFrame->setBLeft( baseFrameSet->getFrame( 0 )->getBLeft() );
                    newFrame->setBRight( baseFrameSet->getFrame( 0 )->getBRight() );
                    newFrame->setBTop( baseFrameSet->getFrame( 0 )->getBTop() );
                    newFrame->setBBottom( baseFrameSet->getFrame( 0 )->getBBottom() );

                    newFrame->setHeight(baseFrameSet->getFrame(0)->height());
                    newFrame->setMinFrameHeight(baseFrameSet->getFrame(0)->minFrameHeight());
                    //kdDebug(32002) << "KWTableFrameSet::recalcRows header created, height=" << newFrame->height() << endl;
                }
                cell->getFrame( 0 )->moveTopLeft( KoPoint( cell->getFrame( 0 )->x(), y ) );
                if(cell->m_row + cell->m_rows -1 == j) {
                    nextY=cell->getFrame(0) -> bottom() + tableCellSpacing;
                }
            }
        }
    }
    m_pageBoundaries.append(m_cells.count());
    //kdDebug() << "KWTableFrameSet::recalcRows done" << endl;
}

void KWTableFrameSet::setBoundingRect( KoRect rect )
{
    //kdDebug() << "KWTableFrameSet::setBoundingRect" << endl;
    if ( m_widthMode == TblAuto )
    {
        rect.setLeft( m_doc->ptLeftBorder() );
        rect.setWidth( m_doc->ptPaperWidth() - ( m_doc->ptLeftBorder() + m_doc->ptRightBorder() ) - 5 /* hack */ );
    }

    double baseWidth = (rect.width() - (m_cols-1) * tableCellSpacing) / m_cols;
    double baseHeight=0;
    if( m_heightMode != TblAuto )
        baseHeight = (rect.height() - (m_rows-1) * tableCellSpacing) / m_rows;

    // I will create 1 mm margins, this will recalculate the actual size needed for the frame.
    double oneMm = MM_TO_POINT( 1.0 );
    double minBaseHeight = 22;// m_doc->getDefaultParagLayout()->getFormat().ptFontSize() + oneMm * 2; // TODO
    if(baseHeight < minBaseHeight + oneMm * 2)
        baseHeight =minBaseHeight + oneMm * 2;
    if(baseWidth < minFrameWidth + oneMm * 2)
        baseWidth = minFrameWidth +  oneMm * 2;
    // move/size the cells
    // TBD: is there a reason why this cannot be done as a linear scan of the list?
    for ( unsigned int i = 0; i < m_rows; i++ ) {
        for ( unsigned int j = 0; j < m_cols; j++ ) {
            KWFrame *frame = getCell( i, j )->getFrame( 0 );
            frame->setBLeft( oneMm );
            frame->setBRight( oneMm );
            frame->setBTop( oneMm );
            frame->setBBottom( oneMm );
            frame->setNewFrameBehaviour( KWFrame::NoFollowup );
            frame->setRect( rect.x() + j * (baseWidth + tableCellSpacing),
                rect.y() + i * (baseHeight + tableCellSpacing), baseWidth, baseHeight );
            //frame->setPageNum(m_doc->getPageOfRect( *frame ));
            frame->setMinFrameHeight(minBaseHeight);
        }
    }
}

void KWTableFrameSet::setHeightMode( CellSize mode )
{
    m_heightMode = mode;
}

void KWTableFrameSet::setWidthMode( CellSize mode )
{
    m_widthMode = mode;
}

bool KWTableFrameSet::hasSelectedFrame()
{
    unsigned int a=0,b=0;
    return getFirstSelected(a,b);
}

void KWTableFrameSet::moveBy( double dx, double dy )
{
    if(dx==0 && dy==0)
        return;
    kdDebug() << "KWTableFrameSet::moveBy " << dx << "," << dy << endl;
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        m_cells.at( i )->getFrame( 0 )->moveBy( dx, dy );
        if(!m_cells.at( i )->isVisible())
            m_cells.at( i )->setVisible(true);
    }
    m_doc->updateAllFrames();

/* leads to resizing while formatting, for inline tables, which qrt doesn't support
    recalcCols();
    recalcRows();
    */
}

/*void KWTableFrameSet::drawAllRects( QPainter &p, int xOffset, int yOffset )
{
    KWFrame *frame;

    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        frame = m_cells.at( i )->getFrame( 0 );
        QRect tmpRect(frame->x() - xOffset,  frame->y() - yOffset, frame->width(), frame->height());
        p.drawRect( m_doc->zoomRect(tmpRect) );
    }
}*/


void KWTableFrameSet::deselectAll()
{
    for ( unsigned int i = 0; i < m_cells.count(); i++ )
        m_cells.at( i )->getFrame( 0 )->setSelected( false );
}

void KWTableFrameSet::refreshSelectedCell()
{
    unsigned int row,col;
    if ( !isOneSelected( row, col ) )
        return;

    Cell *cell=getCell(row,col);
    if(cell)
        cell->getFrame( 0 )->updateResizeHandles();
}

void KWTableFrameSet::selectUntil( double x, double y) {
    KWFrame *f = frameAtPos(x,y);
    if(f) selectUntil(static_cast<KWTableFrameSet::Cell *> (f->getFrameSet()));
}

/* the selectUntil method will select all frames from the first
   selected to the frame of the argument frameset.
*/
void KWTableFrameSet::selectUntil( Cell *cell)
{
    unsigned int toRow = 0, toCol = 0;
    toRow=cell->m_row + cell->m_rows -1;
    toCol=cell->m_col + cell->m_cols -1;

    unsigned int fromRow = 0, fromCol = 0;
    getFirstSelected( fromRow, fromCol );
    if(cell->m_cols!=1)
        fromCol=QMIN(fromCol,cell->m_col);
    if(cell->m_rows!=1)
        fromRow=QMIN(fromRow,cell->m_row);



    if ( fromRow > toRow ) { // doSwap
        fromRow = fromRow^toRow;
        toRow = fromRow^toRow;
        fromRow = fromRow^toRow;
    }

    if ( fromCol > toCol ) { // doSwap
        fromCol = fromCol^toCol;
        toCol = fromCol^toCol;
        fromCol = fromCol^toCol;
    }

    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        cell = m_cells.at(i);
        // check if cell falls completely in square.
        unsigned int row = cell->m_row + cell->m_rows -1;
        unsigned int col = cell->m_col + cell->m_cols -1;
        if(row >= fromRow && row <= toRow && col >= fromCol && col <= toCol)
        {
            cell->getFrame( 0 )->setSelected( true );
            cell->getFrame(0)->createResizeHandles();
            cell->getFrame(0)->updateResizeHandles();
        }
        else
        {
            if(cell->getFrame( 0 )->isSelected())
            {
                cell->getFrame( 0 )->setSelected( false );
                cell->getFrame(0)->removeResizeHandles();
            }
        }
    }
}

/* Return true if exactly one frame is selected. The parameters row
   and col will receive the values of the active row and col.
   When no frame or more then one frame is selected row and col will
   stay unchanged (and false is returned).
*/
bool KWTableFrameSet::isOneSelected(unsigned int &row, unsigned int &col) {
    int selectedCell=-1;
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        if(m_cells.at(i)->getFrame(0)->isSelected())  {
            if(selectedCell==-1)
                selectedCell=i;
            else
                selectedCell=m_cells.count()+1;
        }
    }
    if(selectedCell>=0 && selectedCell<= static_cast<int> (m_cells.count())) {
        row=m_cells.at(selectedCell)->m_row;
        col=m_cells.at(selectedCell)->m_col;
        return true;
    }
    return false;
}

/* returns true if at least one is selected, excluding the argument frameset.
*/
bool KWTableFrameSet::getFirstSelected( unsigned int &row, unsigned int &col )
{
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        if (m_cells.at( i )->getFrame( 0 )->isSelected()) {
            row = m_cells.at( i )->m_row;
            col = m_cells.at( i )->m_col;
            return true;
        }
    }
    return false;
}

void KWTableFrameSet::insertRow( unsigned int _idx,QList<KWFrameSet> listFrameSet, QList<KWFrame> listFrame,bool _recalc, bool isAHeader )
{
    unsigned int i = 0;
    unsigned int _rows = m_rows;

    QValueList<double> colStart;
    KoRect br = boundingRect();

    bool needFinetune=false;
    //laurent
    //it's an unsigned so if idx==0 when you put before first row
    // you substract 1 to 0 to an unsigned value is bad
    unsigned int copyFromRow=(_idx==0)? 0 : _idx-1;
    int nbCols=0;
    if(isAHeader) copyFromRow=0;

    // build a list of colStart positions.
    for ( i = 0; i < m_cells.count(); i++ ) {
        Cell *cell = m_cells.at(i);
        if ( cell->m_row == copyFromRow ) {
            nbCols++;
            if(cell->m_cols==1)
                colStart.append(cell->getFrame( 0 )->left());
            else {
                needFinetune=true;
                for( int rowspan=cell->m_cols; rowspan>0; rowspan--)
                {
                    colStart.append(cell->getFrame( 0 )->left() + (cell->getFrame( 0 )->width() / cell->m_cols)*(rowspan - 1) );
                }
            }
        }
        // also move all cells beneath the new row.
        if ( cell->m_row >= _idx )
            cell->m_row++;
    }
    if(needFinetune) {
        for( unsigned int col = 0; col < colStart.count(); col++) {
            for ( i = 0; i < m_cells.count(); i++ ) {
                if(m_cells.at(i)->m_col == col) {
                    colStart[col]=m_cells.at(i)->getFrame(0)->left();
                    break;
                }
            }
        }
    }

    //add right position of table.
    colStart.append(br.right());
    QList<KWTableFrameSet::Cell> nCells;
    nCells.setAutoDelete( false );
    if(_idx==0)
        copyFromRow=1;

    double height = getCell(copyFromRow,0)->getFrame(0)->height();
    for ( i = 0; i < getCols(); i++ ) {
        int colSpan = getCell(copyFromRow,i)->m_cols;
        double tmpWidth= colStart[i+colSpan] - colStart[i];
        if(i+colSpan != getCols())
            tmpWidth-=tableCellSpacing;
        /*else
          tmpWidth+=1;*/

        KWFrame *frame = 0L;
        if(listFrame.isEmpty())
        {
            frame=new KWFrame(0L, colStart[i], br.y(), tmpWidth, height, KWFrame::RA_NO);
            frame->setFrameBehaviour(KWFrame::AutoExtendFrame);
            frame->setNewFrameBehaviour(KWFrame::NoFollowup);
        }
        else
            frame=listFrame.at(i)->getCopy();

        Cell *newCell=0L;
        if(listFrameSet.isEmpty())
            newCell=new Cell( this, _idx, i, QString::null );
        else
        {
            newCell = static_cast<KWTableFrameSet::Cell*> (listFrameSet.at(i));
            addCell( newCell );
        }
        newCell->m_cols=colSpan;
        newCell->setIsRemoveableHeader( isAHeader );
        newCell->addFrame( frame, /*_recalc*/false );
        nCells.append( newCell );
        newCell->m_cols = getCell(copyFromRow,i)->m_cols;


        if(colSpan>1)
            i+=colSpan-1;

    }
    m_rows = ++_rows;

    for ( i = 0; i < nCells.count(); i++ ) {
        double oneMm = MM_TO_POINT( 1.0 );
        KWFrame *frame = nCells.at( i )->getFrame( 0 );
        frame->setBLeft( oneMm );
        frame->setBRight( oneMm );
        frame->setBTop( oneMm );
        frame->setBBottom( oneMm );
    }

    if ( _recalc )
        finalize();
}

void KWTableFrameSet::insertCol( unsigned int col,QList<KWFrameSet> listFrameSet, QList<KWFrame>listFrame )
{
    unsigned int _cols = m_cols;
    double x=0, width = 60;
    if(col < m_cols)
    {
        // move others out of the way.
        for(unsigned int i = 0; i < m_cells.count(); i++)
        {
            Cell *cell = m_cells.at(i);
            if(cell->m_col == col)
                x= cell->getFrame(0)->x();
            if(cell->m_col >= col)
            {
                cell->m_col++;
            }
        }
    }
    else
    {
        x = boundingRect().right() + tableCellSpacing;
    }

    for( unsigned int i = 0; i < getRows(); i++ ) {
        int rows;
        double height;
        Cell *cell;
        if(col > 0 ) {
            cell = getCell(i, col-1);
            if(cell->m_col + cell->m_cols > col) {
                // cell overlaps the new column
                cell->m_cols++;
                cell->getFrame(0)->setWidth(cell->getFrame(0)->width() + width + tableCellSpacing - 1);
                continue;
            }
            rows = cell->m_rows;
            height = cell->getFrame(0)->height();
        } else {
            rows = 1;
            cell = getCell(i, col+1);
            height = cell->getFrame(0)->height();
        }
        Cell *newCell=0L;

        if(listFrameSet.isEmpty())
            newCell = new Cell( this, i, col, QString::null );
        else
        {
            newCell = static_cast<KWTableFrameSet::Cell*> (listFrameSet.at(i));
            addCell( newCell );
        }

        KWFrame *frame = 0L;
        if(listFrame.isEmpty())
        {
            frame=new KWFrame(newCell, x, cell->getFrame(0)->y(), width, height, KWFrame::RA_NO );
            frame->setFrameBehaviour(KWFrame::AutoExtendFrame);
        }
        else
        {
            frame=listFrame.at(i)->getCopy();
        }
        newCell->addFrame( frame,false );
        if(cell->m_rows >1) {
            newCell->m_rows = cell->m_rows;
            i+=cell->m_rows -1;
        }
    }

    m_cols = ++_cols;
    finalize();
}

/* Delete all cells that are completely in this row.              */

void KWTableFrameSet::deleteRow( unsigned int row, bool _recalc )
{
    double height=0;
    unsigned int rowspan=1;
    // I want to know the height of the row(s) I am removing.
    for (unsigned int rowspan=1; rowspan < m_rows && height==0; rowspan++) {
        for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
            if(m_cells.at(i)->m_row == row && m_cells.at(i)->m_rows==rowspan) {
                height=m_cells.at(i)->getFrame(0)->height();
                break;
            }
        }
    }

    // move/delete cells.
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        Cell *cell = m_cells.at(i);
        if ( row >= cell->m_row  && row < cell->m_row + cell->m_rows) { // cell is indeed in row
            if(cell->m_rows == 1) { // lets remove it
                frames.remove( cell->getFrame(0) );
                cell->delFrame( cell->getFrame(0));
                //m_cells.remove(  i);
                m_cells.take(i);
                i--;
            } else { // make cell span rowspan less rows
                cell->m_rows -= rowspan;
                cell->getFrame(0)->setHeight( cell->getFrame(0)->height() - height - (rowspan -1) * tableCellSpacing);
            }
        } else if ( cell->m_row > row ) {
            // move cells to the left
            cell->m_row -= rowspan;
            cell->getFrame(0)->moveBy( 0, -height);
        }
    }
    m_rows -= rowspan;

    if ( _recalc )
        recalcRows();
}

/* Delete all cells that are completely in this col.              */
void KWTableFrameSet::deleteCol( unsigned int col )
{
    double width=0;
    unsigned int colspan=1;
    // I want to know the width of the col(s) I am removing.
    for (unsigned int colspan=1; colspan < m_cols && width==0; colspan++) {
        for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
            if(m_cells.at(i)->m_col == col && m_cells.at(i)->m_cols==colspan) {
                width=m_cells.at(i)->getFrame(0)->width();
                break;
            }
        }
    }

    // move/delete cells.
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        Cell *cell = m_cells.at(i);
        if ( col >= cell->m_col  && col < cell->m_col + cell->m_cols) { // cell is indeed in col
            if(cell->m_cols == 1) { // lets remove it
                frames.remove( cell->getFrame(0) );
                cell->delFrame( cell->getFrame(0));
                //m_cells.remove(  i);
                m_cells.take(i);
                i--;
            } else { // make cell span colspan less cols
                cell->m_cols -= colspan;
                cell->getFrame(0)->setWidth(
                        cell->getFrame(0)->width() - width - (colspan-1) * tableCellSpacing);
            }
        } else if ( cell->m_col > col ) {
            // move cells to the left
            cell->m_col -= colspan;
            cell->getFrame(0)->moveBy( -width, 0);
        }
    }
    m_cols -= colspan;

    //laurent
    //it's necessary to recalc row, because when you used contain(...)
    //you used m_pageBoundaries.
    // But m_pageBoundaries is init in recalcRows
    //and  when you remove col, you must recreate m_pageBoundaries
    // otherwise kword crashs.
    recalcRows();
    recalcCols();
}

void KWTableFrameSet::updateTempHeaders()
{
#if 0
    if ( !m_hasTmpHeaders ) return;

    for ( unsigned int i = 1; i < m_rows; i++ ) {
        for ( unsigned int j = 0; j < m_cols; j++ ) {
            KWFrameSet *fs = getCell( i, j );
            if ( fs->isRemoveableHeader() ) {
                //dynamic_cast<KWTextFrameSet*>( fs )->assign( dynamic_cast<KWTextFrameSet*>( getCell( 0, j ) ) );

                QPainter p;
                QPicture pic;
                p.begin( &pic );
                KWFormatContext fc( doc, m_doc->getFrameSetNum( fs ) + 1 );
                fc.init( dynamic_cast<KWTextFrameSet*>( fs )->getFirstParag(), true );

                bool bend = false;
                while ( !bend )
                    bend = !fc.makeNextLineLayout();

                p.end();
            }
        }
    }
#endif
}

void KWTableFrameSet::ungroup()
{
    /*
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        m_cells.at( i )->setGroupManager( 0L );
        m_doc->addFrameSet(m_cells.at( i ));
        }
    */
    m_cells.setAutoDelete( false );
    m_cells.clear();

    m_active = false;
}
void KWTableFrameSet::group()
{
   m_cells.setAutoDelete( true );
   m_cells.clear();

   m_active = true;
}

KCommand *KWTableFrameSet::joinCells(unsigned int colBegin,unsigned int rowBegin, unsigned int colEnd,unsigned int rowEnd) {
    kdDebug()<<"colBegin :"<<colBegin<<" rowBegin :"<<rowBegin<<" colEnd :"<<colEnd<<" rowEnd :"<<rowEnd<<endl;
    Cell *firstCell = getCell(rowBegin, colBegin);
    if(colBegin==0 && rowBegin==0 && colEnd==0 && rowEnd==0)
    {

        if ( !getFirstSelected( rowBegin, colBegin ) )
            return 0L;

        firstCell = getCell(rowBegin, colBegin);
        colEnd=colBegin+firstCell->m_cols-1;
        rowEnd=rowBegin+firstCell->m_rows-1;

        while(colEnd+1 <getCols()) { // count all horizontal selected cells
            Cell *cell = getCell(rowEnd,colEnd+1);
            if(cell->getFrame(0)->isSelected()) {
                colEnd+=cell->m_cols;
            } else
                break;
        }

        while(rowEnd+1 < getRows()) { // count all vertical selected cells
            Cell *cell = getCell(rowEnd+1, colBegin);
            if(cell->getFrame(0)->isSelected()) {
                for(unsigned int j=1; j <= cell->m_rows; j++) {
                    for(unsigned int i=colBegin; i<=colEnd; i++) {
                        if(! getCell(rowEnd+j,i)->getFrame(0)->isSelected())
                            return 0L; // can't use this selection..
                    }
                }
                rowEnd+=cell->m_rows;
            } else
                break;
        }
        // if just one cell selected for joining; exit.
        if(rowBegin == rowEnd && colBegin == colEnd ||
           getCell(rowBegin,colBegin) == getCell(rowEnd,colEnd))
            return 0L;
    }
    double bottom=getCell(rowEnd, colBegin)->getFrame(0)->bottom();
    double right=getCell(rowEnd, colEnd)->getFrame(0)->right();

    QList<KWFrameSet> listFrameSet;
    QList<KWFrame> listCopyFrame;

    // do the actual merge.
    for(unsigned int i=colBegin; i<=colEnd;i++) {
        for(unsigned int j=rowBegin; j<=rowEnd;j++) {
            Cell *cell = getCell(j,i);
            if(cell && cell!=firstCell) {

                listFrameSet.append(cell);
                listCopyFrame.append(cell->getFrame(0)->getCopy());
                frames.remove( cell->getFrame(0) );
                cell->delFrame( cell->getFrame(0));
                //m_cells.remove(  cell);
                m_cells.take(m_cells.find(cell));
            }
        }
    }
    ASSERT(firstCell);
    // update firstcell properties te reflect the merge
    firstCell->m_cols=colEnd-colBegin+1;
    firstCell->m_rows=rowEnd-rowBegin+1;
    kdDebug()<<"=colEnd-colBegin+1; :"<<colEnd-colBegin+1<<endl;
    kdDebug()<<"=rowEnd-rowBegin+1  :"<<rowEnd-rowBegin+1<<endl;
    firstCell->getFrame(0)->setRight(right);
    firstCell->getFrame(0)->setBottom(bottom);
    firstCell->getFrame(0)->updateResizeHandles();

    recalcCols();
    recalcRows();
    m_doc->updateAllFrames();
    m_doc->repaintAllViews();
    return new KWJoinCellCommand( i18n("Join Cells"), this,colBegin,rowBegin, colEnd,rowEnd,listFrameSet,listCopyFrame);
}

KCommand *KWTableFrameSet::splitCell(unsigned int intoRows, unsigned int intoCols, int _col, int _row,QList<KWFrameSet> listFrameSet, QList<KWFrame>listFrame) {
    //kdDebug()<<"intoRows :"<<intoRows<<" intoCols :"<< intoCols <<" _col :"<<_col<<" _row "<<_row<<" listFrameSet :"<<listFrameSet.count()<<" listFrame :"<<listFrame.count()<<endl;
    if(intoRows < 1 || intoCols < 1)
        return 0L;

    unsigned int col, row;
    if(_col!=-1 && _row!=-1)
    {
        row=_row;
        col=_col;
    }
    else
    {
        if ( !isOneSelected( row, col ) )
            return 0L;
    }


    Cell *cell=getCell(row,col);
    KWFrame *firstFrame = cell->getFrame(0);
    // unselect frame.
    firstFrame->setSelected(false);
    firstFrame->removeResizeHandles();

    double height = (firstFrame->height() -  tableCellSpacing * (intoRows-1)) / intoRows ;
    double width = (firstFrame->width() -  tableCellSpacing * (intoCols-1))/ intoCols  ;
    double extraHeight = 0;

    // will it fit?
    if(width < minFrameWidth) return 0L;
    if(height < minFrameHeight) {
        extraHeight = minFrameHeight * intoRows - firstFrame->height();
        height = minFrameHeight;
    }

    firstFrame->setWidth(width);
    firstFrame->setHeight(height);

    int rowsDiff = intoRows-cell->m_rows;
    int colsDiff = ((int) intoCols)-cell->m_cols;

    // adjust cellspan and rowspan on other cells.
    for (unsigned int i=0; i< m_cells.count() ; i++) {
        Cell *theCell = m_cells.at(i);
        if(cell == theCell) continue;

        if(rowsDiff>0) {
            if(row >= theCell->m_row && row < theCell->m_row + theCell->m_rows)
                theCell->m_rows+=rowsDiff;
            if(theCell->m_row > row) {
                theCell->m_row+=rowsDiff;
                theCell->getFrame(0)->setTop(theCell->getFrame(0)->top()+extraHeight);
        }
        }
        if(colsDiff>0) {
            if(col >= theCell->m_col && col < theCell->m_col + theCell->m_cols)
                theCell->m_cols+=colsDiff;
            if(theCell->m_col > col) theCell->m_col+=colsDiff;
        }
        if(extraHeight != 0 && theCell->m_row == row) {
            theCell->getFrame(0)->setHeight(theCell->getFrame(0)->height()+extraHeight);
        }
    }

    // set new row and col-span. Use intermediate ints otherwise we get strange results as the
    // intermediate result could be negative (which goes wrong with unsigned ints)
    int r = (cell->m_rows +1) - intoRows;
    if(r < 1) r=1;
    cell->m_rows = r;

    int c = (cell->m_cols + 1) - intoCols;
    if(c < 1)  c=1;
    cell->m_cols = c;

    // If we created extra rows/cols, adjust the groupmanager counters.
    if(rowsDiff>0) m_rows+= rowsDiff;
    if(colsDiff>0) m_cols+= colsDiff;
    int i=0;
    // create new cells
    for (unsigned int y = 0; y < intoRows; y++) {
        for (unsigned int x = 0; x < intoCols; x++){
            if(x==0 && y==0)
                continue; // the orig cell takes this spot.

            Cell *lastFrameSet=0L;

            if(listFrameSet.isEmpty())
            {
                lastFrameSet= new Cell( this, y + row, x + col );
                lastFrameSet->m_rows = 1;
                lastFrameSet->m_cols = 1;
            }
            else
            {
                lastFrameSet = static_cast<KWTableFrameSet::Cell*> (listFrameSet.at(i));
                addCell( lastFrameSet );
            }

            KWFrame *frame=0L;
            if(listFrame.isEmpty())
            {
                frame=firstFrame->getCopy();
                frame->setRect(firstFrame->left() + static_cast<double>((width+tableCellSpacing) * x),
                               firstFrame->top() + static_cast<double>((height+tableCellSpacing) * y),
                               width, height);
                frame->setRunAround( KWFrame::RA_NO );
                frame->setFrameBehaviour(KWFrame::AutoExtendFrame);
                frame->setNewFrameBehaviour(KWFrame::NoFollowup);
                lastFrameSet->addFrame( frame,false );
            }
            else
                lastFrameSet->addFrame( listFrame.at(i)->getCopy(),false );
            i++;

            // if the orig cell spans more rows/cols than it is split into, make first col/row wider.
            if(rowsDiff <0 && y==0)
                lastFrameSet->m_rows -=rowsDiff;
            if(colsDiff <0 && x==0)
                lastFrameSet->m_cols -=colsDiff;
        }
    }

    finalize();

    return new KWSplitCellCommand(i18n("Split Cells"),this,col,row,intoCols, intoRows);
}

void KWTableFrameSet::viewFormatting( QPainter &/*painter*/, int )
{
}

/* checks the cells for missing cells or duplicates, will correct
   mistakes.
*/
void KWTableFrameSet::validate()
{
    for (unsigned int j=0; j < getNumCells() ; j++) {
        KWFrame *frame = getCell(j)->getFrame(0);
        if(frame->getFrameBehaviour()==KWFrame::AutoCreateNewFrame) {
            frame->setFrameBehaviour(KWFrame::AutoExtendFrame);
            kdWarning() << "Table cell property frameBehaviour was incorrect; fixed" << endl;
        }
        if(frame->getNewFrameBehaviour()!=KWFrame::NoFollowup) {
            kdWarning() << "Table cell property newFrameBehaviour was incorrect; fixed" << endl;
            frame->setNewFrameBehaviour(KWFrame::NoFollowup);
        }
    }

    QList<Cell> misplacedCells;

    for(unsigned int row=0; row < getRows(); row++) {
        for(unsigned int col=0; col <getCols(); col++) {
            bool found=false;
            for ( unsigned int i = 0; i < m_cells.count(); i++ )
            {
                if ( m_cells.at( i )->m_row <= row &&
                     m_cells.at( i )->m_col <= col &&
                     m_cells.at( i )->m_row+m_cells.at( i )->m_rows > row &&
                     m_cells.at( i )->m_col+m_cells.at( i )->m_cols > col )
                {
                    if(found==true)
                    {
                        kdWarning() << "Found duplicate cell, (" << m_cells.at(i)->m_row << ", " << m_cells.at(i)->m_col << ") moving one out of the way" << endl;
                        misplacedCells.append(m_cells.take(i--));
                    }
                    found=true;
                }
            }
            if(! found) {
                kdWarning() << "Missing cell, creating a new one; ("<< row << "," << col<<")" << endl;
                // worth it ?
                //QString name = m_doc->generateFramesetName( i18n( "1 is table name, 2 is a number", "%1 Auto Added Cell %2" ).arg( getName() ) );
                Cell *_frameSet = new Cell( this, row, col );
                double x=-1, y=-1, width=-1, height=-1;
                for (unsigned int i=0; i < m_cells.count(); i++) {
                    if(m_cells.at(i)->m_row==row)
                        y=m_cells.at(i)->getFrame(0)->y();
                    if(m_cells.at(i)->m_col==col)
                        x=m_cells.at(i)->getFrame(0)->x();
                    if(m_cells.at(i)->m_col==col && m_cells.at(i)->m_cols==1)
                        width=m_cells.at(i)->getFrame(0)->width();
                    if(m_cells.at(i)->m_row==row && m_cells.at(i)->m_rows==1)
                        height=m_cells.at(i)->getFrame(0)->height();
                    if(x!=-1 && y!=-1 && width!=-1 && height != -1)
                        break;
                }
                if(x== -1) x=0;
                if(y== -1) y=0;
                if(width== -1) width=minFrameWidth;
                if(height== -1) height=minFrameHeight;
                kdWarning() << " x: " << x << ", y:" << y << ", width: " << width << ", height: " << height << endl;
                KWFrame *frame = new KWFrame(_frameSet, x, y, width, height, KWFrame::RA_NO );
                frame->setFrameBehaviour(KWFrame::AutoExtendFrame);
                frame->setNewFrameBehaviour(KWFrame::NoFollowup);
                _frameSet->addFrame( frame,false );
                _frameSet->m_rows = 1;
                _frameSet->m_cols = 1;
            }
        }
    }
    double bottom = getCell(m_rows-1,0)->getFrame(0)->bottom();
    while (! misplacedCells.isEmpty()) {
        // append cell at bottom of table.
        Cell *cell = misplacedCells.take(0);
        cell->getFrame(0)->setWidth(boundingRect().width());
        cell->getFrame(0)->moveBy( boundingRect().left() -
                                             cell->getFrame(0)->left(),
                                             bottom - cell->getFrame(0)->top() - tableCellSpacing);
        cell->m_row = m_rows++;
        cell->m_col = 0;
        cell->m_cols = m_cols;
        cell->m_rows = 1;
        bottom=cell->getFrame(0)->bottom();
        m_cells.append(cell);
    }
}

bool KWTableFrameSet::contains( double mx, double my ) {
    /*  m_pageBoundaries is a list of integers.
        The list contains an entry for each page and that entry points to the first cell
        on that page.
        The list contains integers of the index of the cell in the m_cells list.

        the following points to the first cell of the first page.
          m_cells->at(m_pageBoundaries[0])

        The last entry in the list points to the last+1 cell (the cell on a non-existing
        extra page) Beware that this cell does no really exist! This entry is there so we
        can always get the last cell on a page by substracting 1 from the firs cell of the
        next page. (Well, in theory anyway, untill someone joins the last cell of a page ;)
    */
    if(m_cells.count()==0)
        return false;
    if(m_pageBoundaries.count() ==0)
        recalcRows();
    KWFrame *first, *last;
    for (unsigned int i=1 ; i < m_pageBoundaries.count(); i++) {

        first = m_cells.at((m_pageBoundaries[i-1]))->getFrame( 0 );
        if(m_pageBoundaries[i] != 0)
        {
            KWTableFrameSet::Cell *cell=m_cells.at(m_pageBoundaries[i] -1);
            //not cell at right
            if((cell->m_cols+cell->m_col<m_cols-1) || (cell->m_col<m_cols-1) )
                cell=getCell(cell->m_row, m_cols-1);
            last = cell->getFrame( 0 );
        }
        else
            last = first;


        KoRect rect( KoPoint( first->x(), first->y() ), KoPoint( last->right(), last->bottom() ) );
        if(rect.contains(mx,my)) {
            return true;
        }
    }

    return false;
}

void KWTableFrameSet::createEmptyRegion( const QRect & crect, QRegion & emptyRegion, KWViewMode *viewMode )
{
    // Avoid iterating over all cells if we are out of view
    if ( !viewMode->normalToView( m_doc->zoomRect( boundingRect() ) ).intersects( crect ) )
        return;
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
    {
        QRect outerRect( viewMode->normalToView( frameIt.current()->outerRect() ) );
        //kdDebug() << "KWTableFrameSet::createEmptyRegion outerRect=" << DEBUGRECT( outerRect )
        //          << " crect=" << DEBUGRECT( crect ) << endl;
        outerRect &= crect;
        if ( !outerRect.isEmpty() )
            emptyRegion = emptyRegion.subtract( outerRect );
    }
}

void KWTableFrameSet::drawBorders( QPainter& painter, const QRect &crect, KWViewMode *viewMode, KWCanvas *canvas )
{
    painter.save();

    // We need 'i'
    //QListIterator<KWFrame> frameIt = frameIterator();
    //for ( ; frameIt.current(); ++frameIt )
    bool topOfPage = true;
    for(uint i = 0; i < m_cells.count(); i++)
    {
        Cell *cell = m_cells.at(i);
        //KWFrame *frame = frameIt.current();
        KWFrame *frame = cell->getFrame( 0 );
        if ( !frame )
            continue;

        // This sets topOfPage for the first cell that is on top of a page,
        // _leaves_ it to true for the other cells in the same row,
        // and resets it to false at the next cell in the first column.
        if ( m_pageBoundaries.contains( i ) )
            topOfPage = true;
        else if ( cell->m_col == 0 )
            topOfPage = false;

        //kdDebug() << "KWTableFrameSet::drawBorders i=" << i << " row=" << cell->m_row << " col=" << cell->m_col
        //          << " topOfPage=" << topOfPage << endl;

        QRect outerRect( viewMode->normalToView( frame->outerRect() ) );
        if ( !crect.intersects( outerRect ) )
            continue;

        QRect rect( viewMode->normalToView( m_doc->zoomRect( *frame ) ) );
        // Set the background color.
        QBrush bgBrush( frame->getBackgroundColor() );
	bgBrush.setColor( KWDocument::resolveBgColor( bgBrush.color(), &painter ) );
        painter.setBrush( bgBrush );

        // Draw default borders using view settings...
        QPen defaultPen( lightGray ); // TODO use qcolorgroup
        // ...except when printing, or embedded doc, or disabled.
        if ( ( painter.device()->devType() == QInternal::Printer ) ||
             !canvas || !canvas->gui()->getView()->viewFrameBorders() )
        {
            defaultPen.setColor( bgBrush.color() );
        }

        // Draw borders either as the user defined them, or using the view settings.
        // Borders should be drawn _outside_ of the frame area
        // otherwise the frames will erase the border when painting themselves.

        //    Border::drawBorders( painter, m_doc, frameRect,
        //         frame->leftBorder(), frame->rightBorder(),
        //         frame->topBorder(), frame->bottomBorder(),
        //         1, viewSetting );

        const int minborder = 1;

        // ###### We'll need some code to ensure that this->rightborder == cell_on_right->leftborder etc.
        Border topBorder = frame->topBorder();
        Border bottomBorder = frame->bottomBorder();
        Border leftBorder = frame->leftBorder();
        Border rightBorder = frame->rightBorder();
        int topBorderWidth = Border::zoomWidthY( topBorder.ptWidth, m_doc, minborder );
        int bottomBorderWidth = Border::zoomWidthY( bottomBorder.ptWidth, m_doc, minborder );
        int leftBorderWidth = Border::zoomWidthX( leftBorder.ptWidth, m_doc, minborder );
        int rightBorderWidth = Border::zoomWidthX( rightBorder.ptWidth, m_doc, minborder );

        QColor defaultColor = KWDocument::defaultTextColor( &painter );

        if ( topOfPage )  // draw top only for 1st row on every page.
            if ( topBorderWidth > 0 )
            {
                if ( topBorder.ptWidth > 0 )
                    painter.setPen( Border::borderPen( topBorder, topBorderWidth, defaultColor ) );
                else
                    painter.setPen( defaultPen );
                int y = rect.top() - topBorderWidth + topBorderWidth/2;
                painter.drawLine( rect.left()-leftBorderWidth, y, rect.right()+rightBorderWidth, y );
            }
        if ( bottomBorderWidth > 0 )
        {
            if ( bottomBorder.ptWidth > 0 )
                painter.setPen( Border::borderPen( bottomBorder, bottomBorderWidth, defaultColor ) );
            else
                painter.setPen( defaultPen );
            int y = rect.bottom() + bottomBorderWidth - bottomBorderWidth/2;
            painter.drawLine( rect.left()-leftBorderWidth, y, rect.right()+rightBorderWidth, y );
        }
        if ( cell->m_col == 0 ) // draw left border only for 1st column.
            if ( leftBorderWidth > 0 )
            {
                if ( leftBorder.ptWidth > 0 )
                    painter.setPen( Border::borderPen( leftBorder, leftBorderWidth, defaultColor ) );
                else
                    painter.setPen( defaultPen );
                int x = rect.left() - leftBorderWidth + leftBorderWidth/2;
                painter.drawLine( x, rect.top()-topBorderWidth, x, rect.bottom()+bottomBorderWidth );
            }
        if ( rightBorderWidth > 0 )
        {
            if ( rightBorder.ptWidth > 0 )
                painter.setPen( Border::borderPen( rightBorder, rightBorderWidth, defaultColor ) );
            else
                painter.setPen( defaultPen );
            int x = rect.right() + rightBorderWidth - rightBorderWidth/2;
            painter.drawLine( x, rect.top()-topBorderWidth, x, rect.bottom()+bottomBorderWidth );
        }
    }
    painter.restore();
}

void KWTableFrameSet::drawContents( QPainter * painter, const QRect & crect,
                                    QColorGroup & cg, bool onlyChanged, bool resetChanged,
                                    KWFrameSetEdit * edit, KWViewMode * viewMode, KWCanvas *canvas )
{
    drawBorders( *painter, crect, viewMode, canvas );
    for (unsigned int i=0; i < m_cells.count() ; i++)
    {
        if (edit)
        {
            KWTableFrameSetEdit * tableEdit = static_cast<KWTableFrameSetEdit *>(edit);
            if ( tableEdit->currentCell() && m_cells.at(i) == tableEdit->currentCell()->frameSet() )
            {
                m_cells.at(i)->drawContents( painter, crect, cg, onlyChanged, resetChanged, tableEdit->currentCell(), viewMode, canvas );
                continue;
            }
        }
        m_cells.at(i)->drawContents( painter, crect, cg, onlyChanged, resetChanged, 0L, viewMode, canvas );
    }

}

void KWTableFrameSet::zoom( bool forPrint ) {
    for (unsigned int i =0; i < m_cells.count(); i++) {
        m_cells.at(i)->zoom( forPrint );
    }
}

void KWTableFrameSet::preparePrinting( QPainter *painter, QProgressDialog *progress, int &processedParags ) {
    for (unsigned int i =0; i < m_cells.count(); i++) {
        m_cells.at(i)->preparePrinting( painter, progress, processedParags );
    }
}

void KWTableFrameSet::save( QDomElement &parentElem, bool saveFrames ) {
    // When saving to a file, we don't have anything specific to the frameset to save.
    // Save the cells only.
    for (unsigned int i =0; i < m_cells.count(); i++) {
        m_cells.at(i)->save(parentElem, saveFrames);
    }
}

void KWTableFrameSet::toXML( QDomElement &parentElem, bool saveFrames )
{
    QDomElement framesetElem = parentElem.ownerDocument().createElement( "FRAMESET" );
    parentElem.appendChild( framesetElem );
    KWFrameSet::save( framesetElem, false ); // Save the frameset attributes
    // Save the cells
    save( framesetElem, saveFrames );
}

void KWTableFrameSet::fromXML( QDomElement &framesetElem, bool loadFrames, bool useNames )
{
    KWFrameSet::load( framesetElem, false ); // Load the frameset attributes
    // Load the cells
    QDomElement cellElem = framesetElem.firstChild().toElement();
    for ( ; !cellElem.isNull() ; cellElem = cellElem.nextSibling().toElement() )
    {
        if ( cellElem.tagName() == "FRAMESET" )
            loadCell( cellElem, loadFrames, useNames );
    }
}

KWTableFrameSet::Cell* KWTableFrameSet::loadCell( QDomElement &framesetElem, bool loadFrames, bool useNames )
{
    int _row = KWDocument::getAttribute( framesetElem, "row", 0 );
    int _col = KWDocument::getAttribute( framesetElem, "col", 0 );
    Cell *cell = new Cell( this, _row, _col, QString::null /*unused*/ );
    QString autoName = cell->getName();
    kdDebug() << "KWTableFrameSet::loadCell autoName=" << autoName << endl;
    cell->load( framesetElem, loadFrames );
    if ( !useNames )
        cell->setName( autoName );
    cell->m_rows = KWDocument::getAttribute( framesetElem, "rows", 1 );
    cell->m_cols = KWDocument::getAttribute( framesetElem, "cols", 1 );
    return cell;
}

int KWTableFrameSet::paragraphs()
{
    int paragraphs = 0;
    for (unsigned int i =0; i < m_cells.count(); i++) {
        paragraphs += m_cells.at(i)->paragraphs();
    }
    return paragraphs;
}

bool KWTableFrameSet::statistics( QProgressDialog *progress, ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words,
    ulong & sentences, ulong & syllables )
{
    for (unsigned int i =0; i < m_cells.count(); i++) {
        if( ! m_cells.at(i)->statistics( progress, charsWithSpace, charsWithoutSpace, words, sentences, syllables ) )
        {
            return false;
        }
    }
    return true;
}

void KWTableFrameSet::finalize( ) {
    //kdDebug() << "KWTableFrameSet::finalize" << endl;
    recalcRows();
    recalcCols();
    KWFrameSet::finalize();
}

bool KWTableFrameSet::canRemovePage( int num ) {
    /*  This one is a lot simpler then the one it overrides, we simply don't have
        to check if the frame contains something, the simple existence of a frame
        is enough
    */
    QListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt ) {
        if ( frameIt.current()->pageNum() == num ) {
            return false;
        }
    }
    return true;
}

void KWTableFrameSet::addTextFramesets( QList<KWTextFrameSet> & lst )
{
    for (unsigned int i =0; i < m_cells.count(); i++) {
        lst.append(m_cells.at(i));
    }
}

#ifndef NDEBUG
void KWTableFrameSet::printDebug( KWFrame * frame )
{
    KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell *>( frame->getFrameSet() );
    ASSERT( cell );
    if ( cell ) {
        kdDebug() << " |  |- row :" << cell->m_row << endl;
        kdDebug() << " |  |- col :" << cell->m_col << endl;
        kdDebug() << " |  |- rows:" << cell->m_rows << endl;
        kdDebug() << " |  +- cols:" << cell->m_cols << endl;
    }
}

void KWTableFrameSet::printDebug() {
    kdDebug() << " |  Table size (" << m_rows << "x" << m_cols << ")" << endl;
    KWFrameSet::printDebug();
}

#endif

/////

KWTableFrameSet::Cell::Cell( KWTableFrameSet *table, unsigned int row, unsigned int col, const QString &/*name*/ ) :
    KWTextFrameSet( table->m_doc, "." /*dummy and not empty, faster*/ )
{
    m_table = table;
    m_row = row;
    m_col = col;
    m_rows = 1;
    m_cols = 1;
    setGroupManager( m_table );
    m_table->addCell( this );
    m_name = i18n("Hello dear translator :), 1 is the table name, 2 and 3 are row and column", "%1 Cell %2,%3")
             .arg( table->getName() ).arg(m_row).arg(m_col);
}

KWTableFrameSet::Cell::Cell( KWTableFrameSet *table, const Cell &original ) :
    KWTextFrameSet( table->m_doc, "." )
{
    m_table = table;
    m_row = original.m_row;
    m_col = original.m_col;
    m_rows = original.m_rows;
    m_cols = original.m_cols;
    setName(original.m_name+'_'); // unicity problem !
    setGroupManager( m_table );
    m_table->addCell( this );
}

KWTableFrameSet::Cell::~Cell()
{
}

bool KWTableFrameSet::Cell::isAboveOrLeftOf( unsigned row, unsigned col )
{
    return ( m_row < row ) || ( ( m_row == row ) && ( m_col < col ) );
}

void KWTableFrameSet::Cell::addFrame(KWFrame *_frame, bool recalc) {
    if(getGroupManager())
        getGroupManager()->addFrame(_frame, recalc);
    KWTextFrameSet::addFrame(_frame, recalc);
}


KWTableFrameSetEdit::~KWTableFrameSetEdit()
{
    if ( m_currentCell )
        m_currentCell->terminate();
    delete m_currentCell;
}

void KWTableFrameSetEdit::mousePressEvent( QMouseEvent * e, const QPoint & nPoint, const KoPoint & dPoint )
{
    setCurrentCell( dPoint );
    if ( m_currentCell )
        m_currentCell->mousePressEvent( e, nPoint, dPoint );
}

void KWTableFrameSetEdit::setCurrentCell( const KoPoint & dPoint )
{
    KWFrameSet *fs = tableFrameSet()->getCellByPos( dPoint.x(), dPoint.y() );
    if ( fs && ( !m_currentCell || fs != m_currentCell->frameSet() ) )
        setCurrentCell( fs );
}

void KWTableFrameSetEdit::setCurrentCell( KWFrameSet * fs )
{
    if ( m_currentCell )
        m_currentCell->terminate();
    delete m_currentCell;
    m_currentCell =  fs->createFrameSetEdit( m_canvas );
    m_currentFrame = fs->getFrame( 0 );
}

KWFrameSetEdit* KWTableFrameSetEdit::currentTextEdit()
{
    return m_currentCell;
}


void KWTableFrameSetEdit::keyPressEvent( QKeyEvent * e )
{
    if ( !m_currentCell )
        return;
    KWTableFrameSet::Cell *cell = static_cast<KWTableFrameSet::Cell *>(m_currentCell->frameSet());
    KWTextFrameSet *textframeSet=dynamic_cast<KWTextFrameSet *>(m_currentCell->frameSet());
    bool moveToOtherCell=true;
    if(textframeSet)
    {
        //don't move to other cell when we try to select
        //a text
        QTextDocument * textdoc = textframeSet->textDocument();
        if(textdoc->hasSelection( QTextDocument::Standard ))
            moveToOtherCell=false;
    }
    KWFrameSet *fs = 0L;

    if(moveToOtherCell)
    {
        switch( e->key() ) {
            case QKeyEvent::Key_Up:
            {
                if(!(static_cast<KWTextFrameSetEdit *>(m_currentCell))->getCursor()->parag()->prev())
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    if ( cell->m_col > 0 )
                        fs = tableFrame->getCell(  cell->m_row, cell->m_col - 1 );
                    else if ( cell->m_row > 0 )
                        fs = tableFrame->getCell(  cell->m_row - 1,tableFrame->getCols() - 1 );
                    else
                        fs = tableFrame->getCell( tableFrame->getRows() - 1,tableFrame->getCols() - 1 );
                }
            }
            break;
            case QKeyEvent::Key_Down:
            {
                if(!(static_cast<KWTextFrameSetEdit *>(m_currentCell))->getCursor()->parag()->next())
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    if ( cell->m_col+cell->m_cols < tableFrame->getCols()  )
                        fs = tableFrame->getCell( cell->m_row+cell->m_rows-1, cell->m_col + cell->m_cols );
                    else if ( cell->m_row+cell->m_rows < tableFrame->getRows() )
                        fs = tableFrame->getCell( cell->m_row + cell->m_rows, 0 );
                    else
                        fs = tableFrame->getCell( 0, 0 );
                }
            }
            break;
        }
    }
    if ( fs )
        setCurrentCell( fs );
    else
        m_currentCell->keyPressEvent( e );
}

#include "kwtableframeset.moc"
