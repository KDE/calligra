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
#include <kwdoc.h>
#include <kwanchor.h>
#include <kwtableframeset.h>
#include "kwcanvas.h"

KWTableFrameSet::KWTableFrameSet( KWDocument *doc ) :
    KWFrameSet( doc )
{
    m_rows = 0;
    m_cols = 0;
    m_name = QString::null;
    m_showHeaderOnAllPages = true;
    m_hasTmpHeaders = false;
    m_active = true;
    m_isRendered = false;
    m_cells.setAutoDelete( true );
    frames.setAutoDelete(false);
    m_anchor = 0L;
}

/*================================================================*/
KWTableFrameSet::KWTableFrameSet( KWTableFrameSet &original ) :
    KWFrameSet( original.m_doc )
{
    m_doc = original.m_doc;
    m_rows = original.m_rows;
    m_cols = original.m_cols;
    m_name = original.m_name;
    m_showHeaderOnAllPages = original.m_showHeaderOnAllPages;
    m_hasTmpHeaders = original.m_hasTmpHeaders;
    m_active = original.m_active;
    m_isRendered = original.m_isRendered;
    m_cells.setAutoDelete( true );
    m_anchor = 0L;

    // Copy all cells.
    for ( unsigned int i = 0; i < original.m_cells.count(); i++ )
    {
        Cell *cell = new Cell( this, *original.m_cells.at( i ) );
        m_cells.append( cell );
    }
    m_doc->addFrameSet( this );
    frames.setAutoDelete(false);
}

/*================================================================*/
KWTableFrameSet::~KWTableFrameSet()
{
    delete m_anchor;
    m_anchor = 0L;
    if ( m_doc )
        m_doc->delFrameSet( this, false );
    m_doc = 0L;
}

KWFrameSetEdit * KWTableFrameSet::createFrameSetEdit( KWCanvas * canvas )
{
    return new KWTableFrameSetEdit( this, canvas );
}

void KWTableFrameSet::updateFrames()
{

    for (unsigned int i =0; i < m_cells.count(); i++)
    {
        m_cells.at(i)->updateFrames();
        m_cells.at(i)->layout();
    }
    KWFrameSet::updateFrames();
}

void KWTableFrameSet::moveFloatingFrame( int /*frameNum TODO */, const KoPoint &position )
{
    KoPoint currentPos = getCell( 0, 0 )->getFrame( 0 )->topLeft();
    KoPoint offset = position - currentPos;
    moveBy( offset.x(), offset.y() );
    // ## TODO apply page breaking
}

KoPoint KWTableFrameSet::floatingFrameSize( int /*frameNum TODO */ )
{
    // ## TODO cut into one rectangle per page
    KoRect r = boundingRect();
    return KoPoint( r.width(), r.height() );
}

void KWTableFrameSet::addDeleteAnchorCommand( int /*unused*/, KMacroCommand * /*macroCmd*/ )
{
    // Do we really want to delete the whole table with a single 'Del' keystroke ? (honest question)
    // If yes, implement this method
    // If not, we need an "allowed to delete" bool in KWAnchor and test for it in KWTextFrameSet.
}

void KWTableFrameSet::updateAnchors( bool placeHolderExists /*= false */ /*only used when loading*/ )
{
    int index = m_anchorPos.index;
    // TODO make one rect per page, and replace m_anchor with a QList<KWAnchor>
    if ( !m_anchor )
    {
        // Anchor this frame, after the previous one
        m_anchor = new KWAnchor( m_anchorPos.textfs->textDocument(), this, 0 /*....*/ );
        if ( !placeHolderExists )
            m_anchorPos.parag->insert( index, QChar('£') /*whatever*/ );
        m_anchorPos.parag->setCustomItem( index, m_anchor, 0 );
    }
    m_anchorPos.parag->setChanged( true );
    emit repaintChanged( m_anchorPos.textfs );
}

void KWTableFrameSet::deleteAnchors()
{
    findFirstAnchor();
    int index = m_anchorPos.index;
    if ( m_anchor )
    {
        // Delete anchor (after removing anchor char)
        m_anchorPos.parag->removeCustomItem( index );
        delete m_anchor;
        m_anchor = 0L;
    }
}

/*================================================================*/
void KWTableFrameSet::addCell( Cell *cell )
{
    unsigned int i;

    m_rows = QMAX( cell->m_row + 1, m_rows );
    m_cols = QMAX( cell->m_col + 1, m_cols );

    // Find the insertion point in the list.
    for ( i = 0; i < m_cells.count() && m_cells.at( i )->isAboveOrLeftOf( cell->m_row, cell->m_col ); i++ ) ;
    cell->setName( m_name + ' ' + cell->m_col + ',' + cell->m_row );
    m_cells.insert( i, cell );
}

/*================================================================*/
KoRect KWTableFrameSet::boundingRect()
{
    KWFrame *first = getCell( 0, 0 )->getFrame( 0 );
    assert(first);
    KWFrame *last = getCell( m_rows - 1, m_cols - 1 )->getFrame( 0 );
    assert(last);

    return first->unite( *last );
}

/*================================================================*/
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

/*================================================================*/
KWTableFrameSet::Cell *KWTableFrameSet::getCellByPos( int mx, int my )
{
    KWFrame *f = getFrame(mx,my);
    if(f) return static_cast<KWTableFrameSet::Cell *> (f->getFrameSet());
    return 0L;
}

/*================================================================*/
bool KWTableFrameSet::isTableHeader( Cell *cell )
{
    return cell->isRemoveableHeader() || ( cell->m_row == 0 );
}

/*================================================================*/
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
                if(i!=row) {
                    cell=getCell(i,col);
                    if(cell->m_col==col) {
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
                    if(cell==activeCell)
                        cell=0;
                    else
                        difference = -(activeCell->getFrame(0)->left() - coordinate);
                } else {
                    cell = getCell(i,col-1);
                    if(cell->m_row == i) // dont resize joined cells more then ones.
                        difference=activeCell->getFrame(0)->left() - coordinate;
                    else
                        cell=0;
                }
                if(cell) {
                    // rescale this cell with the calculated difference
                    double newWidth=cell->getFrame(0)->width() + difference;
                    if(newWidth<minFrameWidth) {
                        if(static_cast<int>(minFrameWidth-newWidth) > postAdjust)
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
                if(col==0) col++;
                cell = getCell(i,col-1);
                if(cell->m_row == i)
                    cell->getFrame(0)->setWidth( cell->getFrame(0)->width()+postAdjust);
            }
        } else {
            col+=activeCell->m_cols-1;
            // find old coord.
            coordinate=activeCell->getFrame(0)->right();
            bool found=false;
            for ( unsigned int i = 0; i < m_rows; i++) {
                if(i!=row) {
                    cell=getCell(i,activeCell->m_cols+activeCell->m_col-1);
                    if(cell->m_col+cell->m_cols==activeCell->m_cols+activeCell->m_col) {
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
                    if(cell != activeCell && cell->m_row == i) {
                        double newWidth = cell->getFrame(0)->width() +
                            activeCell->getFrame(0)->right() - coordinate;
                        if(newWidth<minFrameWidth) {
                            if(static_cast<int> (minFrameWidth-newWidth) > postAdjust)
                                postAdjust = minFrameWidth-newWidth;
                        }
                        cell->getFrame(0)->setWidth(newWidth);
                    }
                }
                for ( unsigned int i = 0; i < m_rows; i++) {
                    cell = getCell(i,col);
                    if(cell->m_row == i)
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
                if(cell->m_col==i && cell->m_row==j) {
                    cell->getFrame( 0 )->moveTopLeft( KoPoint( x, cell->getFrame( 0 )->y() ) );
                }
                if(cell->m_col + cell->m_cols -1 == i)
                    nextX=cell->getFrame(0) -> right() + tableCellSpacing;
            }
        }
    }
}

/*================================================================*/
void KWTableFrameSet::recalcRows(int _col, int _row)
{
    // remove automatically added headers
    for ( unsigned int j = 0; j < m_rows; j++ ) {
        if ( getCell( j, 0 )->isRemoveableHeader() ) {
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
            if(i!=col) {
                cell=getCell(row,i);
                if(cell->m_row==row) {
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
                        if(static_cast<int> (minFrameHeight-newHeight) > postAdjust)
                            postAdjust = minFrameHeight-newHeight;
                    }
                    cell->getFrame(0)->setHeight(newHeight);
                }
            }
            if(row!=0)
                activeCell->getFrame(0)->setHeight(
                    activeCell->getFrame(0)->height() +
                    activeCell->getFrame(0)->top()- coordinate);
            if(postAdjust!=0) {
                if(row==0) row++;
                for ( unsigned int i = 0; i < m_cols; i++) {
                    cell = getCell(row-1,i);
                    if(cell->m_col == i)
                        cell->getFrame(0)->setHeight(
                            cell->getFrame(0)->height() + postAdjust);
                }
            }
        } else { // bottom pos has changed
            row+=activeCell->m_rows-1;
            // find old coord.
            coordinate=activeCell->getFrame(0)->bottom();
            for ( unsigned int i = 0; i < m_cols; i++) {
                if(i!=col) {
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
                    if(cell != activeCell && cell->m_col == i) {
                        double newHeight= cell->getFrame(0)->height() +
                            activeCell->getFrame(0)->bottom() - coordinate;
                        if(newHeight<minFrameHeight) {
                            if(static_cast<int> (minFrameHeight-newHeight) > postAdjust)
                                postAdjust = minFrameHeight-newHeight;
                        }
                        cell->getFrame(0)->setHeight(newHeight);
                    }
                }
            }
            if(postAdjust!=0) {
                for ( unsigned int i = 0; i < m_cols; i++) {
                    cell = getCell(row,i);
                    if(cell->m_col == i) cell->getFrame(0)->setHeight(
                        cell->getFrame(0)->height() + postAdjust);
                }
            }
        }
    }

    // do positioning of frames
    double y, nextY = getCell( 0, 0 )->getFrame( 0 )->y();
    unsigned int doingPage = getCell(0,0)->getPageOfFrame(0);
    m_pageBoundaries.clear();
    m_pageBoundaries.append(0);
    for ( unsigned int j = 0; j < m_rows; j++ ) {
        y=nextY;
        unsigned int i = 0;
        bool _addRow = false;

        for ( i = 0; i < m_cols; i++ ) {
            Cell *cell = getCell(j,i);
            if(!(cell && cell->getFrame(0))) { // sanity check.
                kdDebug() << "screwy table cell!! row:" << cell->m_row << ", col: " << cell->m_col << endl;
                continue;
            }
            if(cell->m_col==i && cell->m_row==j) { // beware of multi cell frames.
                cell->getFrame( 0 )->moveTopLeft( KoPoint( cell->getFrame( 0 )->x(), y ) );
                cell->getFrame( 0 )->setPageNum(doingPage);
            }
            if(cell->m_row + cell->m_rows -1 == j)
                nextY=cell->getFrame(0) -> bottom() + tableCellSpacing;
        }

        // check all cells on this row if one might have fallen off the page.
        if( j == 0 ) continue;
        unsigned int fromRow=j;
        for(i = 0; i < m_cols; i++) {
            Cell *cell = getCell(j,i);
            KWFrameSet *fs=cell;
            if(cell->m_row < fromRow)
                fromRow = cell->m_row;
            if ( fs->getFrame( 0 )->bottom() >  // fits on page?
                  static_cast<int>((doingPage+1) * m_doc->ptPaperHeight() - m_doc->ptBottomBorder())) { // no
                y = (unsigned)( (doingPage+1) * m_doc->ptPaperHeight() + m_doc->ptTopBorder() );
                _addRow = true;
            }
        }
        if ( _addRow ) {
            j=fromRow;
            doingPage++;

            if ( y >=  m_doc->ptPaperHeight() * m_doc->getPages() )
                m_doc->appendPage( /*m_doc->getPages() - 1*/ );

            if ( m_showHeaderOnAllPages ) {
                m_hasTmpHeaders = true;
                insertRow( j, false, true );
            }
            for(i = 0; i < m_cells.count(); i++) {
                Cell *cell = m_cells.at(i);
                if(cell->m_row==j+1) cell->getFrame(0)->updateResizeHandles(); // reposition resize handles.
                if(cell->m_row!=j) continue; //  wrong row
                if(cell->m_col != 1) m_pageBoundaries.append(i); // new page boundary
                if ( m_showHeaderOnAllPages ) {
                    KWTextFrameSet *newFrameSet = dynamic_cast<KWTextFrameSet*>( cell );
                    KWTextFrameSet *baseFrameSet = dynamic_cast<KWTextFrameSet*>( getCell( 0, cell->m_col ) );
                    //newFrameSet->assign( baseFrameSet );

                    newFrameSet->getFrame(0)->setBackgroundColor( baseFrameSet->getFrame( 0 )->getBackgroundColor() );
                    newFrameSet->getFrame(0)->setLeftBorder( baseFrameSet->getFrame( 0 )->getLeftBorder() );
                    newFrameSet->getFrame(0)->setRightBorder( baseFrameSet->getFrame( 0 )->getRightBorder() );
                    newFrameSet->getFrame(0)->setTopBorder( baseFrameSet->getFrame( 0 )->getTopBorder() );
                    newFrameSet->getFrame(0)->setBottomBorder( baseFrameSet->getFrame( 0 )->getBottomBorder() );
                    newFrameSet->getFrame(0)->setBLeft( baseFrameSet->getFrame( 0 )->getBLeft() );
                    newFrameSet->getFrame(0)->setBRight( baseFrameSet->getFrame( 0 )->getBRight() );
                    newFrameSet->getFrame(0)->setBTop( baseFrameSet->getFrame( 0 )->getBTop() );
                    newFrameSet->getFrame(0)->setBBottom( baseFrameSet->getFrame( 0 )->getBBottom() );

                    newFrameSet->getFrame(0)->setHeight(baseFrameSet->getFrame(0)->height());
                }
                cell->getFrame( 0 )->moveTopLeft( KoPoint( cell->getFrame( 0 )->x(), y ) );
                cell->getFrame( 0 )->setPageNum(doingPage);
                if(cell->m_row + cell->m_rows -1 == j) {
                    nextY=cell->getFrame(0) -> bottom() + tableCellSpacing;
                }
            }
        }
    }
    m_pageBoundaries.append(m_cells.count());
}

/*================================================================*/
void KWTableFrameSet::setBoundingRect( KoRect rect )
{
    if ( m_widthMode == TblAuto )
    {
        rect.setLeft( m_doc->ptLeftBorder() );
        rect.setWidth( m_doc->ptPaperWidth() - ( m_doc->ptLeftBorder() + m_doc->ptRightBorder() ) );
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
            frame->setNewFrameBehaviour( NoFollowup );
            frame->setRect( rect.x() + j * (baseWidth + tableCellSpacing),
                rect.y() + i * (baseHeight + tableCellSpacing), baseWidth, baseHeight );
        }
    }
}

/*================================================================*/
void KWTableFrameSet::setHeightMode( KWTblCellSize mode )
{
    m_heightMode = mode;
}

/*================================================================*/
void KWTableFrameSet::setWidthMode( KWTblCellSize mode )
{
    m_widthMode = mode;
}

/*================================================================*/
bool KWTableFrameSet::hasSelectedFrame()
{
    unsigned int a=0,b=0;
    return getFirstSelected(a,b);
}

/*================================================================*/
void KWTableFrameSet::moveBy( double dx, double dy )
{
    // Hmm, don't want that I think....
//    dx = 0; // Ignore the x-offset.

    if(dx==0 && dy==0) return;
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        m_cells.at( i )->getFrame( 0 )->moveBy( dx, dy );
        m_cells.at( i )->setVisible(true);
    }
    preRender();
    m_doc->updateAllFrames();

    recalcCols();
    recalcRows();
}

/*================================================================*/
/*void KWTableFrameSet::drawAllRects( QPainter &p, int xOffset, int yOffset )
{
    KWFrame *frame;

    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        frame = m_cells.at( i )->getFrame( 0 );
        QRect tmpRect(frame->x() - xOffset,  frame->y() - yOffset, frame->width(), frame->height());
        p.drawRect( m_doc->zoomRect(tmpRect) );
    }
}*/


/*================================================================*/
void KWTableFrameSet::deselectAll()
{
    for ( unsigned int i = 0; i < m_cells.count(); i++ )
        m_cells.at( i )->getFrame( 0 )->setSelected( false );
}

/*================================================================*/
void KWTableFrameSet::refreshSelectedCell()
{
    unsigned int row,col;
    if ( !isOneSelected( row, col ) )
        return;

    Cell *cell=getCell(row,col);
    if(cell)
        cell->getFrame( 0 )->updateResizeHandles();
}

/*================================================================*/
void KWTableFrameSet::selectUntil( double x, double y) {
    KWFrame *f = getFrame(x,y);
    if(f) selectUntil(static_cast<KWTableFrameSet::Cell *> (f->getFrameSet()));
}

/*================================================================*/
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
        if(row >= fromRow && row <= toRow && col >= fromCol && col <= toCol) {
            cell->getFrame( 0 )->setSelected( true );
            cell->getFrame(0)->createResizeHandles();
            cell->getFrame(0)->updateResizeHandles();
        } else {
            cell->getFrame( 0 )->setSelected( false );
            cell->getFrame(0)->removeResizeHandles();
        }
    }
}

/*================================================================*/
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

/*================================================================*/
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

/*================================================================*/
void KWTableFrameSet::insertRow( unsigned int _idx, bool _recalc, bool isAHeader )
{
    unsigned int i = 0;
    unsigned int _rows = m_rows;

    QValueList<double> colStart;
    KoRect br = boundingRect();

    bool needFinetune=false;
    unsigned int copyFromRow=_idx-1;
    if(isAHeader) copyFromRow=0;

    // build a list of colStart positions.
    for ( i = 0; i < m_cells.count(); i++ ) {
        Cell *cell = m_cells.at(i);
        if ( cell->m_row == copyFromRow ) {
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
        if ( cell->m_row >= _idx ) cell->m_row++;
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

    colStart.append(br.right());

    QList<KWTableFrameSet::Cell> nCells;
    nCells.setAutoDelete( false );
    double height = getCell(copyFromRow,0)->getFrame(0)->height();

    for ( i = 0; i < getCols(); i++ ) {
        int colSpan = getCell(copyFromRow,i)->m_cols;
        double tmpWidth= colStart[i+colSpan] - colStart[i];
        if(i+colSpan != getCols())
            tmpWidth-=tableCellSpacing;
        else
            tmpWidth+=1;

        KWFrame *frame = new KWFrame(0L, colStart[i], br.y(), tmpWidth, height);
        frame->setFrameBehaviour(AutoExtendFrame);
        frame->setNewFrameBehaviour(NoFollowup);

        Cell *newCell = new Cell( this, _idx, i );
        newCell->m_cols=colSpan;
        newCell->setIsRemoveableHeader( isAHeader );
        newCell->addFrame( frame, _recalc );

        nCells.append( newCell );

        newCell->m_cols = getCell(copyFromRow,i)->m_cols;
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

/*================================================================*/
void KWTableFrameSet::insertCol( unsigned int col )
{
    double x=0, width = 60;
    if(col < m_cols) {
        // move others out of the way.
        for(unsigned int i = 0; i < m_cells.count(); i++) {
            Cell *cell = m_cells.at(i);
            if(cell->m_col == col) x= cell->getFrame(0)->x();
            if(cell->m_col >= col) cell->m_col++;
        }
    } else {
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
        Cell *newCell = new Cell( this, i, col );
        KWFrame *frame = new KWFrame(newCell, x, cell->getFrame(0)->y(), width, height );
        frame->setFrameBehaviour(AutoExtendFrame);
        newCell->addFrame( frame );
        if(cell->m_rows >1) {
            newCell->m_rows = cell->m_rows;
            i+=cell->m_rows -1;
        }
    }

    if(col < m_cols-1) m_cols++;

    recalcCols();
    finalize();
}

/*================================================================*/
/* Delete all cells that are completely in this row.              */
/*================================================================*/

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
                frames.remove(cell->getFrame(0));
                m_cells.remove( i );
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

/*================================================================*/
/* Delete all cells that are completely in this col.              */
/*================================================================*/
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
                m_cells.remove( i );
                frames.remove( i );
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

    recalcCols();
}

/*================================================================*/
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

/*================================================================*/
void KWTableFrameSet::ungroup()
{
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        m_cells.at( i )->setGroupManager( 0L );
        m_doc->addFrameSet(m_cells.at( i ));
    }

    m_cells.setAutoDelete( false );
    m_cells.clear();

    m_active = false;
}

/*================================================================*/
bool KWTableFrameSet::joinCells() {
    unsigned int colBegin, rowBegin, colEnd,rowEnd;
    if ( !getFirstSelected( rowBegin, colBegin ) ) return false;
    Cell *firstCell = getCell(rowBegin, colBegin);
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
                        return false; // can't use this selection..
                }
            }
            rowEnd+=cell->m_rows;
        } else
            break;
    }
    // if just one cell selected for joining; exit.
    if(rowBegin == rowEnd && colBegin == colEnd ||
            getCell(rowBegin,colBegin) == getCell(rowEnd,colEnd))
        return false;

    double bottom=getCell(rowEnd, colBegin)->getFrame(0)->bottom();
    double right=getCell(rowEnd, colEnd)->getFrame(0)->right();

    // do the actual merge.
    for(unsigned int i=colBegin; i<=colEnd;i++) {
        for(unsigned int j=rowBegin; j<=rowEnd;j++) {
            Cell *cell = getCell(j,i);
            if(cell && cell!=firstCell) {
                frames.remove(cell->getFrame(0));
                m_cells.remove(cell);
            }
        }
    }

    // update firstcell properties te reflect the merge
    firstCell->m_cols=colEnd-colBegin+1;
    firstCell->m_rows=rowEnd-rowBegin+1;
    firstCell->getFrame(0)->setRight(right);
    firstCell->getFrame(0)->setBottom(bottom);
    firstCell->getFrame(0)->updateResizeHandles();

    recalcCols();
    recalcRows();
    m_doc->updateAllFrames();
    m_doc->repaintAllViews();
    return true;
}

/*================================================================*/
bool KWTableFrameSet::splitCell(unsigned int intoRows, unsigned int intoCols)
{

    if(intoRows < 1 || intoCols < 1) return false; // assertion.

    unsigned int col, row;
    if ( !isOneSelected( row, col ) ) return false;

    Cell *cell=getCell(row,col);
    KWFrame *firstFrame = cell->getFrame(0);

    // unselect frame.
    firstFrame->setSelected(false);
    firstFrame->removeResizeHandles();

    double height = (firstFrame->height() -  tableCellSpacing * (intoRows-1)) / intoRows ;
    double width = (firstFrame->width() -  tableCellSpacing * (intoCols-1))/ intoCols  ;

    // will it fit?
    if(height < minFrameHeight) return false;
    if(width < minFrameWidth) return false;

    int newRows = intoRows-cell->m_rows;
    int newCols = intoCols-cell->m_cols;

    // adjust cellspan and rowspan on other cells.
    for (unsigned int i=0; i< m_cells.count() ; i++) {
        Cell *theCell = m_cells.at(i);
        if(cell == theCell) continue;

        if(newRows>0) {
            if(row >= theCell->m_row && row < theCell->m_row + theCell->m_rows)
                theCell->m_rows+=newRows;
            if(theCell->m_row > row) theCell->m_row+=newRows;
        }
        if(newCols>0) {
            if(col >= theCell->m_col && col < theCell->m_col + theCell->m_cols)
                theCell->m_cols+=newCols;
            if(theCell->m_col > col) theCell->m_col+=newCols;
        }
    }

    firstFrame->setWidth(static_cast<int>(width));
    firstFrame->setHeight(static_cast<int>(height));
    cell->m_rows = cell->m_rows - intoRows +1;
    if(cell->m_rows < 1)  cell->m_rows=1;
    cell->m_cols = cell->m_cols - intoCols +1;
    if(cell->m_cols < 1)  cell->m_cols=1;

    // create new cells
    for (unsigned int y = 0; y < intoRows; y++) {
        for (unsigned int x = 0; x < intoCols; x++){
            if(x==0 && y==0) continue; // the orig cell takes this spot.

            Cell *lastFrameSet= new Cell( this, y + row, x + col );
            lastFrameSet->setName(QString("split cell"));

            KWFrame *frame = new KWFrame(lastFrameSet,
                    firstFrame->left() + static_cast<int>((width+tableCellSpacing) * x),
                    firstFrame->top() + static_cast<int>((height+tableCellSpacing) * y),
                    width, height);
            frame->setFrameBehaviour(AutoExtendFrame);
            frame->setNewFrameBehaviour(NoFollowup);
            lastFrameSet->addFrame( frame );

            lastFrameSet->m_rows = 1;
            lastFrameSet->m_cols = 1;

            // if the orig cell spans more rows/cols than it is split into, make first col/row wider.
            if(newRows <0 && y==0)
                lastFrameSet->m_rows -=newRows;
            if(newCols <0 && x==0)
                lastFrameSet->m_cols -=newCols;
        }
    }

    // If we created extra rows/cols, adjust the groupmanager counters.
    if(newRows>0) m_rows+= newRows;
    if(newCols>0) m_cols+= newCols;

    finalize();

    return true;
}

/*================================================================*/
void KWTableFrameSet::viewFormatting( QPainter &/*painter*/, int )
{
}

/*================================================================*/
void KWTableFrameSet::preRender() {
#if 0
    for ( unsigned int i = 0; i < m_doc->getNumFrameSets(); i++ ) {
        if ( m_doc->getCell( i )->getGroupManager() == this) {
            KWFormatContext fc( doc, i + 1 );
            fc.init( m_doc->getFirstParag( i ) );

            // and render
/*
            if(!isRendered) {
                while ( fc.makeNextLineLayout());
                recalcRows();
            }*/
        }
    }
#endif
    m_isRendered=true;
}

/*================================================================*/
/* checks the cells for missing cells or duplicates, will correct
   mistakes.
*/
void KWTableFrameSet::validate()
{
    for (unsigned int j=0; j < getNumCells() ; j++) {
        KWFrame *frame = getCell(j)->getFrame(0);
        if(frame->getFrameBehaviour()==AutoCreateNewFrame) {
            frame->setFrameBehaviour(AutoExtendFrame);
            kdWarning() << "Table cell property frameBehaviour was incorrect; fixed" << endl;
        }
        if(frame->getNewFrameBehaviour()!=NoFollowup) {
            kdWarning() << "Table cell property newFrameBehaviour was incorrect; fixed" << endl;
            frame->setNewFrameBehaviour(NoFollowup);
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
                Cell *_frameSet = new Cell( this, row, col );
                _frameSet->setName(QString("Auto added cell"));
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
                KWFrame *frame = new KWFrame(_frameSet, x, y, width, height );
                frame->setFrameBehaviour(AutoExtendFrame);
                frame->setNewFrameBehaviour(NoFollowup);
                _frameSet->addFrame( frame );
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
    if(m_pageBoundaries.count() ==0)  recalcRows();
    KWFrame *first, *last;
    for (unsigned int i=1 ; i < m_pageBoundaries.count(); i++) {
        first = m_cells.at((m_pageBoundaries[i-1]))->getFrame( 0 );
        last = m_cells.at(m_pageBoundaries[i] -1)->getFrame( 0 );

        KoRect rect(first->x(), first->y(), last->right(), last->bottom());
        if(rect.contains(mx,my))
            return true;
    }

    return false;
}

void KWTableFrameSet::drawBorders( QPainter *painter, const QRect &crect, QRegion &region )
{
    painter->save();

    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        QRect frameRect( m_doc->zoomRect( *frame ) );
        QRect outerRect( frame->outerRect() );

        if ( !crect.intersects( outerRect ) )
            continue;

        region = region.subtract( outerRect );

        // Set the background color.
        painter->setBrush( frame->getBackgroundColor() );

        // Draw default borders using view settings except when printing, or disabled.
        QPen viewSetting( lightGray );
        if ( ( painter->device()->devType() == QInternal::Printer ) ||
            !m_doc->getViewFrameBorders() )
        {
            viewSetting.setColor( frame->getBackgroundColor().color() );
        }

        // Draw borders either as the user defined them, or using the view settings.
        // Borders should be drawn _outside_ of the frame area
        // otherwise the frames will erase the border when painting themselves.
        QPen pen;
        double width;
        int w;

        // Right
        width = frame->getRightBorder().ptWidth;
        if ( width > 0 )
        {
            w = QMAX( 1, (int)(m_doc->zoomItX( width ) + 0.5) );
            pen = Border::borderPen( frame->getRightBorder(), w );
        }
        else
        {
            w = 1;
            pen = viewSetting;
        }
        painter->setPen( pen );
        w = QMAX( w / 2, 1 );
        painter->drawLine( frameRect.right() + w, frameRect.y(),
                           frameRect.right() + w, frameRect.bottom() );

        // Bottom
        width = frame->getBottomBorder().ptWidth;
        if ( width > 0 )
        {
            w = QMAX( 1, (int)(m_doc->zoomItY( width ) + 0.5) );
            pen = Border::borderPen( frame->getBottomBorder(), w );
        }
        else
        {
            w = 1;
            pen = viewSetting;
        }
        painter->setPen( pen );
        w = QMAX( w / 2, 1 );
        painter->drawLine( frameRect.x(),     frameRect.bottom() + w,
                           frameRect.right(), frameRect.bottom() + w );

//        if ( cell->m_col == 0 ) // draw left only for 1st column.
        {
            // Left
            width = frame->getLeftBorder().ptWidth;
            if ( width > 0 )
            {
                w = QMAX( 1, (int)(m_doc->zoomItX( width ) + 0.5) );
                pen = Border::borderPen( frame->getLeftBorder(), w );
            }
            else
            {
                w = 1;
                pen = viewSetting;
            }
            painter->setPen( pen );
            w = QMAX( w / 2, 1 );
            painter->drawLine( frameRect.x() - w, frameRect.y(),
                               frameRect.x() - w, frameRect.bottom() );
        }
//        if ( cell->m_row == 0 ) // draw top only for 1st row.
        {
            // Top
            width = frame->getTopBorder().ptWidth;
            if ( width > 0 )
            {
                w = QMAX( 1, (int)(m_doc->zoomItY( width ) + 0.5) );
                pen = Border::borderPen( frame->getTopBorder(), w );
            }
            else
            {
                w = 1;
                pen = viewSetting;
            }
            painter->setPen( pen );
            w = QMAX( w / 2, 1 );
            painter->drawLine( frameRect.x(),     frameRect.y() - w,
                               frameRect.right(), frameRect.y() - w );
        }
    }
    painter->restore();
}

void KWTableFrameSet::drawContents( QPainter * painter, const QRect & crect,
        QColorGroup & cg, bool onlyChanged, bool resetChanged )
{
    for (unsigned int i=0; i < m_cells.count() ; i++)
        m_cells.at(i)->drawContents( painter, crect, cg, onlyChanged, resetChanged );

}

void KWTableFrameSet::zoom() {
    for (unsigned int i =0; i < m_cells.count(); i++) {
        m_cells.at(i)->zoom();
    }
}

void KWTableFrameSet::save( QDomElement &parentElem ) {
    for (unsigned int i =0; i < m_cells.count(); i++) {
        m_cells.at(i)->save(parentElem);
    }
}

void KWTableFrameSet::statistics( ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words, ulong & sentences )
{
    for (unsigned int i =0; i < m_cells.count(); i++) {
        m_cells.at(i)->statistics( charsWithSpace, charsWithoutSpace, words, sentences );
    }
}

void KWTableFrameSet::finalize( ) {
    recalcRows();
    recalcCols();
    KWFrameSet::finalize();
}


KWTableFrameSet::Cell::Cell( KWTableFrameSet *table, unsigned int row, unsigned int col ) :
    KWTextFrameSet( table->m_doc )
{
    m_table = table;
    m_row = row;
    m_col = col;
    m_rows = 1;
    m_cols = 1;
    setGroupManager( m_table );
    m_table->addCell( this );
    m_name = QString("table cell %1,%2").arg(m_row).arg(m_col);
}

KWTableFrameSet::Cell::Cell( KWTableFrameSet *table, const Cell &original ) :
    KWTextFrameSet( table->m_doc ) // TBD: turn getCopy into copy constructor, including setting
{
    m_table = table;
    m_row = original.m_row;
    m_col = original.m_col;
    m_rows = original.m_rows;
    m_cols = original.m_cols;
    setName(original.m_name);
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
    getGroupManager()->addFrame(_frame, recalc);
    KWTextFrameSet::addFrame(_frame, recalc);
}

KWTableFrameSetEdit::~KWTableFrameSetEdit()
{
    delete m_currentCell;
}

void KWTableFrameSetEdit::mousePressEvent( QMouseEvent * e )
{
    int mx = e->pos().x();
    int my = e->pos().y();
    setCurrentCell( mx,  my );
    if ( m_currentCell )
        m_currentCell->mousePressEvent( e );
}

void KWTableFrameSetEdit::setCurrentCell( int mx, int my )
{
    KWDocument * doc = m_fs->kWordDocument();
    int x = static_cast<int>( mx / doc->zoomedResolutionX() );
    int y = static_cast<int>( my / doc->zoomedResolutionY() );
    KWFrameSet *fs = tableFrameSet()->getCellByPos( x, y );
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

#include "kwtableframeset.moc"
