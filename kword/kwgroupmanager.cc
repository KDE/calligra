/* This file is part of the KDE project
   Copyright (C) 2000, 2001 Thomas Zander <zander@earthling.net>

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
*/

#include "kwtextframeset.h"
#include "kwgroupmanager.h"
#include "kwdoc.h"
#include "kwunit.h"
#include <qvaluelist.h>
#include <kdebug.h>
#include <assert.h>

KWGroupManager::KWGroupManager( KWDocument *_doc )
 //: KWCharAnchor(), showHeaderOnAllPages( true ), hasTmpHeaders( false ), active( true )
{
    doc = _doc;
    cells.setAutoDelete( true );
    rows = 0;
    cols = 0;
    name = QString::null;
    isRendered=false;
    showHeaderOnAllPages=true;
    hasTmpHeaders= false;
    active=true;
}

/*================================================================*/
KWGroupManager::KWGroupManager( const KWGroupManager &original )// : KWCharAnchor(original)
{
    showHeaderOnAllPages = original.showHeaderOnAllPages;
    hasTmpHeaders = original.hasTmpHeaders;
    active = original.active;
    doc = original.doc;
    rows = original.rows;
    cols = original.cols;
    name = original.name;
    isRendered=original.isRendered;

    // copy all cells
    cells.setAutoDelete( true );
    QList<Cell> lCells = original.cells;
    lCells.setAutoDelete(false);
    for (unsigned int i=0; i< lCells.count();i++) {
        if(lCells.at(i))  {
            Cell *cell = new Cell;
            cell->row = lCells.at(i)->row;
            cell->col = lCells.at(i)->col;
            cell->rows = lCells.at(i)->rows;
            cell->cols = lCells.at(i)->cols;
            cell->frameSet= dynamic_cast<KWTextFrameSet*>(lCells.at(i)->frameSet)->getCopy();
            cell->frameSet->setGroupManager(this);
            cells.append( cell );
        }
    }

    doc->addGroupManager(this);
    init();
}

/*================================================================*/
KWGroupManager::~KWGroupManager() {
    if(doc) doc->delGroupManager(this, false);
    doc=0L;
}

/*================================================================*/
void KWGroupManager::addFrameSet( KWFrameSet *fs, unsigned int row, unsigned int col )
{
    unsigned int i;

    rows = QMAX( row + 1, rows );
    cols = QMAX( col + 1, cols );

    for (i = 0; i < cells.count() && (cells.at(i)->row < row ||
        cells.at(i)->row == row  && cells.at(i)->col < col) ; i++ );
    if(fs->getName().isEmpty())
        fs->setName(QString ("Cell %1,%2").arg(col+1).arg(row+1));

    // If the group is anchored, we must adjust the incoming frameset.
#if 0
    if ( anchored ) {
        KWFrame *newFrame = fs->getFrame( 0 );

        if (newFrame)
            newFrame->moveBy( origin.x(), origin.y() );
    }
#endif

    Cell *cell = new Cell;
    cell->frameSet = fs;
    cell->row = row;
    cell->col = col;
    cell->rows = 1;
    cell->cols = 1;

    cells.insert( i, cell );
}

/*================================================================*/
KWFrameSet *KWGroupManager::getFrameSet( unsigned int row, unsigned int col )
{
    Cell *cell=getCell(row,col);
    if(cell) return cell->frameSet;
    return 0L;
}

/*================================================================*/
/* returns the cell that occupies row, col. */
KWGroupManager::Cell *KWGroupManager::getCell( unsigned int row, unsigned int col )
{
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        if ( cells.at( i )->row <= row &&
                cells.at( i )->col <= col &&
                cells.at( i )->row+cells.at( i )->rows > row &&
                cells.at( i )->col+cells.at( i )->cols > col ) {
            return cells.at( i );
        }
    }
    return 0L;
}

/*================================================================*/
KWGroupManager::Cell *KWGroupManager::getCell( KWFrameSet *f ) {
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        if ( cells.at( i )->frameSet == f) {
            return cells.at(i);
        }
    }
    return 0L;
}

/*================================================================*/
bool KWGroupManager::isTableHeader( KWFrameSet *fs )
{
    Cell *cell = getCell(fs);
    if(cell)
        return fs->isRemoveableHeader() || cell->row==0;

    return false;
}

/*================================================================*/
void KWGroupManager::init( unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                           KWTblCellSize widthScaling, KWTblCellSize heightScaling )
{
    if ( widthScaling == TblAuto ) {
        x = doc->ptLeftBorder();
        width = doc->ptPaperWidth() -
                ( doc->ptLeftBorder() + doc->ptRightBorder() );
    }

    double baseWidth = (width - (cols-1) * tableCellSpacing) / cols;
    double baseHeight=0;
    if(heightScaling!=TblAuto)
        baseHeight = (height - (rows-1) * tableCellSpacing) / rows;

    // I will create 1 mm margins, this will recalculate the actual size needed for the frame.
    KWUnit oneMm;
    oneMm.setMM( 1 );
    double minBaseHeight = 22;// doc->getDefaultParagLayout()->getFormat().ptFontSize() + oneMm.pt() * 2; // TODO
    if(baseHeight < minBaseHeight + oneMm.pt() * 2)
        baseHeight =minBaseHeight + oneMm.pt() * 2;
    if(baseWidth < minFrameWidth + oneMm.pt() * 2)
        baseWidth = minFrameWidth +  oneMm.pt() * 2;
    // cast them only one time up here..
    unsigned int frameWidth = static_cast<unsigned int> (baseWidth + 0.5);
    unsigned int frameHeight = static_cast<unsigned int> (baseHeight + 0.5);
    // move/size the cells
    for ( unsigned int i = 0; i < rows; i++ ) {
        for ( unsigned int j = 0; j < cols; j++ ) {
            KWFrame *frame = getFrameSet( i, j )->getFrame( 0 );
            frame->setBLeft( oneMm );
            frame->setBRight( oneMm );
            frame->setBTop( oneMm );
            frame->setBBottom( oneMm );
            frame->setNewFrameBehaviour( NoFollowup );
            frame->setRect( x + j * (frameWidth + tableCellSpacing),
                y + i * (frameHeight + tableCellSpacing), baseWidth, baseHeight );
        }
    }

    for ( unsigned int k = 0; k < cells.count(); k++ )
        doc->addFrameSet( cells.at( k )->frameSet );
}

/*================================================================*/
void KWGroupManager::init()
{
    for ( unsigned int k = 0; k < cells.count(); k++ )
        doc->addFrameSet( cells.at( k )->frameSet );
}

/*================================================================*/
void KWGroupManager::recalcCols()
{
    unsigned int row=0,col=0;
    if(! cells.isEmpty() ) {
        //get selected cell
        isOneSelected(row,col);
        // ** check/set sizes of frames **
        // we assume only left or only right pos has changed.
        // check if leftCoordinate is same as rest of tableRow
        Cell *activeCell = getCell(row,col);
        Cell *cell;
        int coordinate;
        // find old coord.
        coordinate=activeCell->frameSet->getFrame(0)->left();
        if(col!=0) { // calculate the old position.
            coordinate = getCell(row, col-1)->frameSet->getFrame(0)->right() + tableCellSpacing;
        } else { // is leftmost, so lets look at other rows..
            for ( unsigned int i = 0; i < rows; i++) {
                if(i!=row) {
                    cell=getCell(i,col);
                    if(cell->col==col) {
                        coordinate=cell->frameSet->getFrame(0)->left();
                        break;
                    }
                }
            }
        }

        int postAdjust=0;
        if(coordinate != activeCell->frameSet->getFrame(0)->left()) { // left pos changed
            // we are now going to move the rest of the cells in this column as well.
            for ( unsigned int i = 0; i < rows; i++) {
                int difference=0;
                if(col==0) {// left most cell
                    cell = getCell(i,col);
                    if(cell==activeCell)
                        cell=0;
                    else
                        difference=(activeCell->frameSet->getFrame(0)->left() - coordinate) * -1;
                } else {
                    cell = getCell(i,col-1);
                    if(cell->row == i) // dont resize joined cells more then ones.
                        difference=activeCell->frameSet->getFrame(0)->left() - coordinate;
                    else
                        cell=0;
                }
                if(cell) {
                    // rescale this cell with the calculated difference
                    unsigned int newWidth=cell->frameSet->getFrame(0)->width() + difference;
                    if(newWidth<minFrameWidth) {
                        if(static_cast<int>(minFrameWidth-newWidth) > postAdjust)
                            postAdjust = minFrameWidth-newWidth;
                    }
                    cell->frameSet->getFrame(0)->setWidth(newWidth);
                }
            }

            // Because we are scaling the cells left of this one, the activeCell has to be
            // returned to its original size.
            if(col!=0)
                activeCell->frameSet->getFrame(0)->setWidth(
                  activeCell->frameSet->getFrame(0)->width() +
                  activeCell->frameSet->getFrame(0)->left() - coordinate);

            // if we found cells that ware made to small, we adjust them using the postAdjust var.
            for ( unsigned int i = 0; i < rows; i++) {
                if(col==0) col++;
                cell = getCell(i,col-1);
                if(cell->row == i)
                    cell->frameSet->getFrame(0)->setWidth( cell->frameSet->getFrame(0)->width()+postAdjust);
            }
        } else {
            col+=activeCell->cols-1;
            // find old coord.
            coordinate=activeCell->frameSet->getFrame(0)->right();
            bool found=false;
            for ( unsigned int i = 0; i < rows; i++) {
                if(i!=row) {
                    cell=getCell(i,activeCell->cols+activeCell->col-1);
                    if(cell->col+cell->cols==activeCell->cols+activeCell->col) {
                        coordinate=cell->frameSet->getFrame(0)->right();
                        found=true;
                        break;
                    }
                }
            }
            if(! found && activeCell->col + activeCell->cols < cols) { // if we did not find it and we are not on the right edge of the table.
               // use the position of the next cell.
               coordinate = getCell(activeCell->row, activeCell->col + activeCell->cols)->frameSet->getFrame(0)->left() - tableCellSpacing;
            }

            if(coordinate != activeCell->frameSet->getFrame(0)->right()) { // right pos changed.
                for ( unsigned int i = 0; i < rows; i++) {
                    Cell *cell = getCell(i,col);
                    if(cell != activeCell && cell->row == i) {
                        unsigned int newWidth= cell->frameSet->getFrame(0)->width() +
                            activeCell->frameSet->getFrame(0)->right() - coordinate;
                        if(newWidth<minFrameWidth) {
                            if(static_cast<int> (minFrameWidth-newWidth) > postAdjust)
                                postAdjust = minFrameWidth-newWidth;
                        }
                        cell->frameSet->getFrame(0)->setWidth(newWidth);
                    }
                }
                for ( unsigned int i = 0; i < rows; i++) {
                    cell = getCell(i,col);
                    if(cell->row == i)
                        cell->frameSet->getFrame(0)->setWidth( cell->frameSet->getFrame(0)->width()+postAdjust);
                }
            }
        }

        // Move cells
        unsigned int x, nextX=0;
        if(getFrameSet(0,0) &&  getFrameSet( 0, 0 )->getFrame( 0 ))
            nextX =getFrameSet( 0, 0 )->getFrame( 0 )->x();

        for ( unsigned int i = 0; i < cols; i++ ) {
            x=nextX;
            for ( unsigned int j = 0; j < rows; j++ ) {
                Cell *cell = getCell(j,i);
                if(cell->col==i && cell->row==j) {
                    cell->frameSet->getFrame( 0 )->moveTopLeft( QPoint( x, cell->frameSet->getFrame( 0 )->y() ) );
                }
                if(cell->col + cell->cols -1 == i)
                    nextX=cell->frameSet->getFrame(0) -> right() + tableCellSpacing;
            }
        }
    }
}

/*================================================================*/
void KWGroupManager::recalcRows()
{
    // remove automatically added headers
    for ( unsigned int j = 0; j < rows; j++ ) {
        if ( getFrameSet( j, 0 )->isRemoveableHeader() ) {
            deleteRow( j, false );
            j--;
        }
    }
    hasTmpHeaders = false;
    // check/set sizes of frames
    unsigned int row=0,col=0;
    if(! cells.isEmpty() && isOneSelected(row,col)) {
        // check if topCoordinate is same as rest of tableRow
        Cell *activeCell = getCell(row,col);
        Cell *cell;
        int coordinate;
        // find old coord.
        coordinate=activeCell->frameSet->getFrame(0)->top();
        for ( unsigned int i = 0; i < cols; i++) {
            if(i!=col) {
                cell=getCell(row,i);
                if(cell->row==row) {
                    coordinate=cell->frameSet->getFrame(0)->top();
                    break;
                }
            }
        }
        int postAdjust=0;
        if(coordinate != activeCell->frameSet->getFrame(0)->top()) { // top pos changed
            for ( unsigned int i = 0; i < cols; i++) {
                int difference=0;
                if(row==0) { // top cell
                    cell = getCell(0,i);
                    if(cell==activeCell)
                        cell=0;
                    else
                        difference= (activeCell->frameSet->getFrame(0)->top()- coordinate) * -1;
                } else {
                    cell = getCell(row-1,i);
                    if(cell->col == i) // dont resize joined cells more then ones.
                        difference= activeCell->frameSet->getFrame(0)->top()- coordinate;
                    else
                        cell=0;
                }
                if(cell) {
                    unsigned int newHeight= cell->frameSet->getFrame(0)->height() + difference;
                    if(newHeight<minFrameHeight) {
                        if(static_cast<int> (minFrameHeight-newHeight) > postAdjust)
                            postAdjust = minFrameHeight-newHeight;
                    }
                    cell->frameSet->getFrame(0)->setHeight(newHeight);
                }
            }
            if(row!=0)
                activeCell->frameSet->getFrame(0)->setHeight(
                    activeCell->frameSet->getFrame(0)->height() +
                    activeCell->frameSet->getFrame(0)->top()- coordinate);
            if(postAdjust!=0) {
                if(row==0) row++;
                for ( unsigned int i = 0; i < cols; i++) {
                    cell = getCell(row-1,i);
                    if(cell->col == i)
                        cell->frameSet->getFrame(0)->setHeight(
                            cell->frameSet->getFrame(0)->height() + postAdjust);
                }
            }
        } else { // bottom pos has changed
            row+=activeCell->rows-1;
            // find old coord.
            coordinate=activeCell->frameSet->getFrame(0)->bottom();
            for ( unsigned int i = 0; i < cols; i++) {
                if(i!=col) {
                    cell=getCell(activeCell->row+activeCell->rows-1,i);
                    if(cell->row+cell->rows==activeCell->row+activeCell->rows) {
                        coordinate=cell->frameSet->getFrame(0)->bottom();
                        break;
                    }
                }
            }
            if(coordinate != activeCell->frameSet->getFrame(0)->bottom()) {
                for ( unsigned int i = 0; i < cols; i++) {
                    cell = getCell(row,i);
                    if(cell != activeCell && cell->col == i) {
                        unsigned int newHeight= cell->frameSet->getFrame(0)->height() +
                            activeCell->frameSet->getFrame(0)->bottom() - coordinate;
                        if(newHeight<minFrameHeight) {
                            if(static_cast<int> (minFrameHeight-newHeight) > postAdjust)
                                postAdjust = minFrameHeight-newHeight;
                        }
                        cell->frameSet->getFrame(0)->setHeight(newHeight);
                    }
                }
            }
            if(postAdjust!=0) {
                for ( unsigned int i = 0; i < cols; i++) {
                    cell = getCell(row,i);
                    if(cell->col == i) cell->frameSet->getFrame(0)->setHeight(
                        cell->frameSet->getFrame(0)->height() + postAdjust);
                }
            }
        }
    }

    // do positioning of frames
    unsigned int y,nextY = getFrameSet( 0, 0 )->getFrame( 0 )->y();
    unsigned int doingPage = getFrameSet(0,0)->getPageOfFrame(0);
    for ( unsigned int j = 0; j < rows; j++ ) {
        y=nextY;
        unsigned int i = 0;
        bool _addRow = false;

        for ( i = 0; i < cols; i++ ) {
            Cell *cell = getCell(j,i);
            if(!(cell->frameSet && cell->frameSet->getFrame(0))) { // sanity check.
                kdDebug() << "screwy table cell!! row:" << cell->row << ", col: " << cell->col << endl;
                continue;
            }
            if(cell->col==i && cell->row==j) { // beware of multi cell frames.
                cell->frameSet->getFrame( 0 )->moveTopLeft( QPoint( cell->frameSet->getFrame( 0 )->x(), y ) );
                cell->frameSet->getFrame( 0 )->setPageNum(doingPage);
            }
            if(cell->row + cell->rows -1 == j)
                nextY=cell->frameSet->getFrame(0) -> bottom() + tableCellSpacing;
        }

        // check all cells on this row if one might have fallen off the page.
        if( j == 0 ) continue;
        unsigned int fromRow=j;
        for(i = 0; i < cols; i++) {
            Cell *cell = getCell(j,i);
            KWFrameSet *fs=cell->frameSet;
            if(cell->row < fromRow)
                fromRow = cell->row;
            if ( fs->getFrame( 0 )->bottom() >  // fits on page?
                  static_cast<int>((doingPage+1) * doc->ptPaperHeight() - doc->ptBottomBorder())) { // no
                y = (doingPage+1) * doc->ptPaperHeight() + doc->ptTopBorder();
                _addRow = true;
            }
        }
        if ( _addRow ) {
            j=fromRow;
            doingPage++;

            if ( y >=  doc->ptPaperHeight() * doc->getPages() )
                doc->appendPage( /*doc->getPages() - 1*/ );

            if ( showHeaderOnAllPages ) {
                hasTmpHeaders = true;
                insertRow( j, false, true );
            }
            for(i = 0; i < cols; i++) {
                Cell *cell = getCell (j,i);
                if ( showHeaderOnAllPages ) {
                    KWTextFrameSet *newFrameSet = dynamic_cast<KWTextFrameSet*>( cell->frameSet );
                    KWTextFrameSet *baseFrameSet = dynamic_cast<KWTextFrameSet*>( getFrameSet( 0, i ) );
                    //newFrameSet->assign( baseFrameSet );
                    newFrameSet->getFrame(0)->setHeight(baseFrameSet->getFrame(0)->height());
                }
                cell->frameSet->getFrame( 0 )->moveTopLeft( QPoint( cell->frameSet->getFrame( 0 )->x(), y ) );
                cell->frameSet->getFrame( 0 )->setPageNum(doingPage);
                if(cell->row + cell->rows -1 == j) {
                    nextY=cell->frameSet->getFrame(0) -> bottom() + tableCellSpacing;
                }
            }
        }
    }
}

/*================================================================*/
QRect KWGroupManager::getBoundingRect()
{
    QRect r1, r2;
    KWFrame *first = getFrameSet( 0, 0 )->getFrame( 0 );
    assert(first);
    KWFrame *last = getFrameSet( rows - 1, cols - 1 )->getFrame( 0 );
    assert(last);

    r1 = QRect( first->x(), first->y(), first->width(), first->height() );
    r2 = QRect( last->x(), last->y(), last->width(), last->height() );

    r1 = r1.unite( r2 );
    return QRect( r1 );
}

/*================================================================*/
bool KWGroupManager::hasSelectedFrame()
{
    unsigned int a=0,b=0;
    return getFirstSelected(a,b);
}

/*================================================================*/
void KWGroupManager::moveBy( int dx, int dy )
{
    dx = 0; // Ignore the x-offset.
    if(dy==0) return;
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        cells.at( i )->frameSet->getFrame( 0 )->moveBy( dx, dy );
        cells.at( i )->frameSet->setVisible(true);
    }
    preRender();
    doc->updateAllFrames();

    recalcCols();
    recalcRows();
}

/*================================================================*/
void KWGroupManager::drawAllRects( QPainter &p, int xOffset, int yOffset )
{
    KWFrame *frame;

    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        frame = cells.at( i )->frameSet->getFrame( 0 );
        p.drawRect( frame->x() - xOffset, frame->y() - yOffset, frame->width(), frame->height() );
    }
}


/*================================================================*/
void KWGroupManager::deselectAll()
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
        cells.at( i )->frameSet->getFrame( 0 )->setSelected( false );
}

/*================================================================*/
/* the selectUntil method will select all frames from the first
   selected to the frame of the argument frameset.
*/
void KWGroupManager::selectUntil( KWFrameSet *fs) {
    unsigned int toRow = 0, toCol = 0;
    Cell *cell = getCell(fs);
    toRow=cell->row + cell->rows -1;
    toCol=cell->col + cell->cols -1;

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


    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        cell = cells.at(i);
        // check if cell falls completely in square.
        unsigned int row = cell->row + cell->rows -1;
        unsigned int col = cell->col + cell->cols -1;
        if(row >= fromRow && row <= toRow && col >= fromCol && col <= toCol) {
            cell->frameSet->getFrame( 0 )->setSelected( true );
            cell->frameSet->getFrame(0)->createResizeHandles();
            cell->frameSet->getFrame(0)->updateResizeHandles();
        } else {
            cell->frameSet->getFrame( 0 )->setSelected( false );
            cell->frameSet->getFrame(0)->removeResizeHandles();
        }
    }
}

/*================================================================*/
/* Return true if exactly one frame is selected. The parameters row
   and col will receive the values of the active row and col.
   When no frame or more then one frame is selected row and col will
   stay unchanged (and false is returned).
*/
bool KWGroupManager::isOneSelected(unsigned int &row, unsigned int &col) {
    int selectedCell=-1;
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        if(cells.at(i)->frameSet->getFrame(0)->isSelected())  {
            if(selectedCell==-1)
                selectedCell=i;
            else
                selectedCell=cells.count()+1;
        }
    }
    if(selectedCell>=0 && selectedCell<= static_cast<int> (cells.count())) {
        row=cells.at(selectedCell)->row;
        col=cells.at(selectedCell)->col;
        return true;
    }
    return false;
}

/*================================================================*/
/* returns true if at least one is selected, excluding the argument frameset.
*/
bool KWGroupManager::getFirstSelected( unsigned int &row, unsigned int &col )
{
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        if (cells.at( i )->frameSet->getFrame( 0 )->isSelected()) {
            row = cells.at( i )->row;
            col = cells.at( i )->col;
            return true;
        }
    }
    return false;
}

/*================================================================*/
void KWGroupManager::insertRow( unsigned int _idx, bool _recalc, bool isAHeader )
{
    unsigned int i = 0;
    unsigned int _rows = rows;

    QValueList<int> colStart;
    QRect r = getBoundingRect();

    for ( i = 0; i < cells.count(); i++ ) {
        Cell *cell = cells.at(i);
        if ( cell->row == 0 ) {
            for( int rowspan=cell->cols; rowspan>0; rowspan--)
                colStart.append(cell->frameSet->getFrame( 0 )->width() / cell->cols );
        }
        if ( cell->row >= _idx ) cell->row++;
    }

    for( unsigned int col = 0; col < colStart.count(); col++) {
        for ( i = 0; i < cells.count(); i++ ) {
            if(cells.at(i)->col == col) {
                colStart[col]=cells.at(i)->frameSet->getFrame(0)->left();
                break;
            }
        }
    }

    colStart.append(r.right());

    QList<KWTextFrameSet> nCells;
    nCells.setAutoDelete( false );

    int ww = 0;
    for ( i = 0; i < getCols(); i++ ) {
        int tmpWidth= colStart[i+1] - colStart[i]-tableCellSpacing;
        if((i+1)==getCols())
            tmpWidth= colStart[i+1] - colStart[i]+tableCellSpacing-2;
        KWFrame *frame = new KWFrame(0L, colStart[i], r.y(), tmpWidth, 20); // TODO  doc->getDefaultParagLayout()->getFormat().ptFontSize() + 10 );
        frame->setFrameBehaviour(AutoExtendFrame);
        frame->setNewFrameBehaviour(NoFollowup);

        KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
        _frameSet->setGroupManager( this );
        _frameSet->setIsRemoveableHeader( isAHeader );
        _frameSet->addFrame( frame );

        // If the group is anchored, we must avoid double-application of
        // the anchor offset.
#if 0
        if ( anchored ) {
            KWFrame *newFrame = _frameSet->getFrame( 0 );

            if (newFrame)
                newFrame->moveBy( -origin.x(), -origin.y() );
        }
#endif
        addFrameSet( _frameSet, _idx, i );

        nCells.append( _frameSet );
        ww += colStart[ i ] + 2;

        if(isAHeader) {
            // copy behav from row0
            Cell *cell = getCell(0,i);
            _frameSet->getFrame(0)->setWidth(cell->frameSet->getFrame(0)->width());
            if(cell->cols>1) {
                getCell(_idx,i)->cols= cell->cols;
                i+=getCell(0,i)->cols -1 ;
            }
        }

    }

    rows = ++_rows;

    for ( i = 0; i < nCells.count(); i++ ) {
        KWUnit u;
        u.setMM( 1 );
        doc->addFrameSet( nCells.at( i ) );
        KWFrame *frame = nCells.at( i )->getFrame( 0 );
        frame->setBLeft( u );
        frame->setBRight( u );
        frame->setBTop( u );
        frame->setBBottom( u );

    }


    if ( _recalc )
        recalcRows();
}

/*================================================================*/
void KWGroupManager::insertCol( unsigned int _idx )
{
    unsigned int i = 0;
    unsigned int _cols = cols;

    QList<int> h;
    h.setAutoDelete( true );
    QRect r = getBoundingRect();

    for ( i = 0; i < cells.count(); i++ ) {
        Cell *cell = cells.at(i);
        if ( cell->col == 0 )
            for( int colspan=cell->rows; colspan>0; colspan--)
                h.append( new int( cell->frameSet->getFrame( 0 )->height() / cell->rows ) );
        if ( cell->col >= _idx ) cell->col++;
    }

    QList<KWTextFrameSet> nCells;
    nCells.setAutoDelete( false );

    int hh = 0;
    for ( i = 0; i < getRows(); i++ ) {
        KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
        _frameSet->setGroupManager( this );

        KWFrame *frame = new KWFrame(_frameSet, r.x(), r.y() + hh, 60, *h.at( i ) );
        frame->setFrameBehaviour(AutoExtendFrame);
        _frameSet->addFrame( frame );

        // If the group is anchored, we must avoid double-application of
        // the anchor offset.
#if 0
        if ( anchored ) {
            frame->moveBy( -origin.x(), -origin.y() );
        }
#endif
        addFrameSet( _frameSet, i, _idx );

        nCells.append( _frameSet );
        hh += *h.at( i ) + 2;
    }

    cols = ++_cols;

    for ( i = 0; i < nCells.count(); i++ ) {
        KWUnit u;
        u.setMM( 1 );
        doc->addFrameSet( nCells.at( i ) );
        KWFrame *frame = nCells.at( i )->getFrame( 0 );
        frame->setBLeft( u );
        frame->setBRight( u );
        frame->setBTop( u );
        frame->setBBottom( u );
    }

    recalcCols();
}

/*================================================================*/
/* Delete all cells that are completely in this row.              */
/*================================================================*/

void KWGroupManager::deleteRow( unsigned int row, bool _recalc )
{
    unsigned int height=0;
    unsigned int rowspan=1;
    // I want to know the height of the row(s) I am removing.
    for (unsigned int rowspan=1; rowspan < rows && height==0; rowspan++) {
        for ( unsigned int i = 0; i < cells.count(); i++ ) {
            if(cells.at(i)->row == row && cells.at(i)->rows==rowspan) {
                height=cells.at(i)->frameSet->getFrame(0)->height();
                break;
            }
        }
    }

    // move/delete cells.
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        Cell *cell = cells.at(i);
        if ( row >= cell->row  && row < cell->row + cell->rows) { // cell is indeed in row
            if(cell->rows == 1) { // lets remove it
                doc->delFrameSet( cells.at( i )->frameSet );
                cells.at( i )->frameSet = 0L;
                cells.remove( i );
                i--;
            } else { // make cell span rowspan less rows
                cell->rows -= rowspan;
                cell->frameSet->getFrame(0)->setHeight( cell->frameSet->getFrame(0)->height() - height - (rowspan -1) * tableCellSpacing);
            }
        } else if ( cell->row > row ) {
            // move cells to the left
            cell->row -= rowspan;
            cell->frameSet->getFrame(0)->moveBy( 0, -height);
        }
    }
    rows -= rowspan;

    if ( _recalc )
        recalcRows();
}

/*================================================================*/
/* Delete all cells that are completely in this col.              */
/*================================================================*/
void KWGroupManager::deleteCol( unsigned int col )
{
    unsigned int width=0;
    unsigned int colspan=1;
    // I want to know the width of the col(s) I am removing.
    for (unsigned int colspan=1; colspan < cols && width==0; colspan++) {
        for ( unsigned int i = 0; i < cells.count(); i++ ) {
            if(cells.at(i)->col == col && cells.at(i)->cols==colspan) {
                width=cells.at(i)->frameSet->getFrame(0)->width();
                break;
            }
        }
    }

    // move/delete cells.
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        Cell *cell = cells.at(i);
        if ( col >= cell->col  && col < cell->col + cell->cols) { // cell is indeed in col
            if(cell->cols == 1) { // lets remove it
                doc->delFrameSet( cells.at( i )->frameSet );
                cells.at( i )->frameSet = 0L;
                cells.remove( i );
                i--;
            } else { // make cell span colspan less cols
                cell->cols -= colspan;
                cell->frameSet->getFrame(0)->setWidth(
                        cell->frameSet->getFrame(0)->width() - width - (colspan-1) * tableCellSpacing);
            }
        } else if ( cell->col > col ) {
            // move cells to the left
            cell->col -= colspan;
            cell->frameSet->getFrame(0)->moveBy( -width, 0);
        }
    }
    cols -= colspan;

    recalcCols();
}

/*================================================================*/
void KWGroupManager::updateTempHeaders()
{
    if ( !hasTmpHeaders ) return;

    for ( unsigned int i = 1; i < rows; i++ ) {
        for ( unsigned int j = 0; j < cols; j++ ) {
            KWFrameSet *fs = getFrameSet( i, j );
            if ( fs->isRemoveableHeader() ) {
                //dynamic_cast<KWTextFrameSet*>( fs )->assign( dynamic_cast<KWTextFrameSet*>( getFrameSet( 0, j ) ) );

                QPainter p;
                QPicture pic;
                p.begin( &pic );
#if 0
                KWFormatContext fc( doc, doc->getFrameSetNum( fs ) + 1 );
                fc.init( dynamic_cast<KWTextFrameSet*>( fs )->getFirstParag(), true );

                bool bend = false;
                while ( !bend )
                    bend = !fc.makeNextLineLayout();
#endif

                p.end();
            }
        }
    }
}

/*================================================================*/
void KWGroupManager::ungroup()
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
        cells.at( i )->frameSet->setGroupManager( 0L );

    cells.setAutoDelete( false );
    cells.clear();

    active = false;
}

/*================================================================*/
bool KWGroupManager::joinCells() {
    unsigned int colBegin, rowBegin, colEnd,rowEnd;
    if ( !getFirstSelected( rowBegin, colBegin ) ) return false;
    Cell *firstCell = getCell(rowBegin, colBegin);
    colEnd=colBegin+firstCell->cols-1;
    rowEnd=rowBegin+firstCell->rows-1;

    while(colEnd+1 <getCols()) { // count all horizontal selected cells
        Cell *cell = getCell(rowEnd,colEnd+1);
        if(cell->frameSet->getFrame(0)->isSelected()) {
            colEnd+=cell->cols;
        } else
            break;
    }

    while(rowEnd+1 < getRows()) { // count all vertical selected cells
        Cell *cell = getCell(rowEnd+1, colBegin);
        if(cell->frameSet->getFrame(0)->isSelected()) {
            for(unsigned int j=1; j <= cell->rows; j++) {
                for(unsigned int i=colBegin; i<=colEnd; i++) {
                    if(! getCell(rowEnd+j,i)->frameSet->getFrame(0)->isSelected())
                        return false; // can't use this selection..
                }
            }
            rowEnd+=cell->rows;
        } else
            break;
    }
    // if just one cell selected for joining; exit.
    if(rowBegin == rowEnd && colBegin == colEnd ||
            getCell(rowBegin,colBegin) == getCell(rowEnd,colEnd))
        return false;

    int bottom=getCell(rowEnd, colBegin)->frameSet->getFrame(0)->bottom();
    int right=getCell(rowEnd, colEnd)->frameSet->getFrame(0)->right();

    // do the actual merge.
    for(unsigned int i=colBegin; i<=colEnd;i++) {
        for(unsigned int j=rowBegin; j<=rowEnd;j++) {
            Cell *cell = getCell(j,i);
            if(cell && cell!=firstCell) {
                doc->delFrameSet( cell->frameSet );
                cell->frameSet = 0L;
                cells.remove(cell);
            }
        }
    }

    // update firstcell properties te reflect the merge
    firstCell->cols=colEnd-colBegin+1;
    firstCell->rows=rowEnd-rowBegin+1;
    firstCell->frameSet->getFrame(0)->setRight(right);
    firstCell->frameSet->getFrame(0)->setBottom(bottom);
    firstCell->frameSet->getFrame(0)->updateResizeHandles();

    recalcCols();
    recalcRows();

    return true;
}

/*================================================================*/
bool KWGroupManager::splitCell(unsigned int intoRows, unsigned int intoCols)
{

    if(intoRows < 1 || intoCols < 1) return false; // assertion.

    unsigned int col, row;
    if ( !isOneSelected( row, col ) ) return false;

    Cell *cell=getCell(row,col);
    KWFrame *firstFrame = cell->frameSet->getFrame(0);

    // unselect frame.
    firstFrame->setSelected(false);
    firstFrame->removeResizeHandles();

    double height = (firstFrame->height() -  tableCellSpacing * (intoRows-1)) / intoRows ;
    double width = (firstFrame->width() -  tableCellSpacing * (intoCols-1))/ intoCols  ;

    // will it fit?
    if(height < minFrameHeight) return false;
    if(width < minFrameWidth) return false;

    int newRows = intoRows-cell->rows;
    int newCols = intoCols-cell->cols;

    // adjust cellspan and rowspan on other cells.
    for (unsigned int i=0; i< cells.count() ; i++) {
        Cell *theCell = cells.at(i);
        if(cell == theCell) continue;

        if(newRows>0) {
            if(row >= theCell->row && row < theCell->row + theCell->rows)
                theCell->rows+=newRows;
            if(theCell->row > row) theCell->row+=newRows;
        }
        if(newCols>0) {
            if(col >= theCell->col && col < theCell->col + theCell->cols)
                theCell->cols+=newCols;
            if(theCell->col > col) theCell->col+=newCols;
        }
    }

    firstFrame->setWidth(static_cast<int>(width));
    firstFrame->setHeight(static_cast<int>(height));
    cell->rows = cell->rows - intoRows +1;
    if(cell->rows < 1)  cell->rows=1;
    cell->cols = cell->cols - intoCols +1;
    if(cell->cols < 1)  cell->cols=1;

    // create new cells
    for (unsigned int y = 0; y < intoRows; y++) {
        for (unsigned int x = 0; x < intoCols; x++){
            if(x==0 && y==0) continue; // the orig cell takes this spot.

            Cell *newCell = new Cell;

            KWTextFrameSet *lastFrameSet= new KWTextFrameSet( doc );
            lastFrameSet->setName(QString("split cell"));
            lastFrameSet->setGroupManager( this );

            KWFrame *frame = new KWFrame(lastFrameSet,
                    firstFrame->left() + static_cast<int>((width+tableCellSpacing) * x),
                    firstFrame->top() + static_cast<int>((height+tableCellSpacing) * y),
                    width, height);
            frame->setFrameBehaviour(AutoExtendFrame);
            frame->setNewFrameBehaviour(NoFollowup);
            lastFrameSet->addFrame( frame );
#if 0
            if ( anchored ) { // is this needed?
                KWFrame *aFrame = lastFrameSet->getFrame( 0 );

                if (aFrame)
                    aFrame->moveBy( -origin.x(), -origin.y() );
            }
#endif
            doc->addFrameSet(lastFrameSet);

            newCell->frameSet = lastFrameSet;
            newCell->row = y + row;
            newCell->col = x + col;
            newCell->rows = 1;
            newCell->cols = 1;

            // if the orig cell spans more rows/cols than it is split into, make first col/row wider.
            if(newRows <0 && y==0)
                newCell->rows -=newRows;
            if(newCols <0 && x==0)
                newCell->cols -=newCols;

            unsigned int i;
            for (i = 0; i < cells.count() && ((cells.at(i)->row < newCell->row ||
                cells.at(i)->row == newCell->row) && cells.at(i)->col < newCell->col) ; i++ );
            cells.insert(i, newCell );
        }
    }

    // If we created extra rows/cols, adjust the groupmanager counters.
    if(newRows>0) rows+= newRows;
    if(newCols>0) cols+= newCols;
    recalcCols();

    // select all frames.
    firstFrame->setSelected(true);
    selectUntil(getCell(row+intoRows-1, col+intoCols-1)->frameSet);
    return true;
}

/*================================================================*/
QString KWGroupManager::anchorType()
{
    return "grpMgr";
}

QString KWGroupManager::anchorInstance()
{
    return name;
}

/*================================================================*/
void KWGroupManager::viewFormatting( QPainter &/*painter*/, int )
{
    KWFrame *frame;

    // If we have been populated, then draw a line from the origin to the
    // top left corner.
    if ( cells.count() > 0 )
    {
        frame = cells.at( 0 )->frameSet->getFrame( 0 );
       // painter.drawLine( origin.x(), origin.y(), frame->x(), frame->y());
    }
}

/*================================================================*/
void KWGroupManager::preRender() {
#if 0
    for ( unsigned int i = 0; i < doc->getNumFrameSets(); i++ ) {
        if ( doc->getFrameSet( i )->getGroupManager() == this) {
            KWFormatContext fc( doc, i + 1 );
            fc.init( doc->getFirstParag( i ) );

            // and render
/*
            if(!isRendered) {
                while ( fc.makeNextLineLayout());
                recalcRows();
            }*/
        }
    }
#endif
    isRendered=true;
}

/*================================================================*/
/* checks the cells for missing cells or duplicates, will correct
   mistakes.
*/
void KWGroupManager::validate()
{
    for (unsigned int j=0; j < getNumCells() ; j++) {
        KWFrame *frame = getCell(j)->frameSet->getFrame(0);
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
            for ( unsigned int i = 0; i < cells.count(); i++ )
            {
                if ( cells.at( i )->row <= row &&
                     cells.at( i )->col <= col &&
                     cells.at( i )->row+cells.at( i )->rows > row &&
                     cells.at( i )->col+cells.at( i )->cols > col )
                {
                    if(found==true)
                    {
                        kdWarning() << "Found duplicate cell, (" << cells.at(i)->row << ", " << cells.at(i)->col << ") moving one out of the way" << endl;
                        misplacedCells.append(cells.take(i--));
                    }
                    found=true;
                }
            }
            if(! found) {
                kdWarning() << "Missing cell, creating a new one; ("<< row << "," << col<<")" << endl;
                Cell *cell = new Cell;
                KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
                _frameSet->setName(QString("Auto added cell"));
                _frameSet->setGroupManager( this );
                int x=-1, y=-1, width=-1, height=-1;
                for (unsigned int i=0; i < cells.count(); i++) {
                    if(cells.at(i)->row==row)
                        y=cells.at(i)->frameSet->getFrame(0)->y();
                    if(cells.at(i)->col==col)
                        x=cells.at(i)->frameSet->getFrame(0)->x();
                    if(cells.at(i)->col==col && cells.at(i)->cols==1)
                        width=cells.at(i)->frameSet->getFrame(0)->width();
                    if(cells.at(i)->row==row && cells.at(i)->rows==1)
                        height=cells.at(i)->frameSet->getFrame(0)->height();
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
#if 0
                if ( anchored ) {
                    KWFrame *newFrame = _frameSet->getFrame( 0 );

                    if (newFrame)
                        newFrame->moveBy( -origin.x(), -origin.y() );
                }
#endif
                doc->addFrameSet(_frameSet);
                cell->frameSet = _frameSet;
                cell->row = row;
                cell->col = col;
                cell->rows = 1;
                cell->cols = 1;

                unsigned int i;
                for (i = 0; i < cells.count() && (cells.at(i)->row < row ||
                                                  cells.at(i)->row == row  && cells.at(i)->col < col) ; i++ );
                cells.insert(i, cell );
            }
        }
    }
    unsigned int bottom = getCell(rows-1,0)->frameSet->getFrame(0)->bottom();
    while (! misplacedCells.isEmpty()) {
        // append cell at botom of table.
        Cell *cell = misplacedCells.take(0);
        cell->frameSet->getFrame(0)->setWidth(getBoundingRect().width());
        cell->frameSet->getFrame(0)->moveBy( getBoundingRect().left() -
                                             cell->frameSet->getFrame(0)->left(),
                                             bottom - cell->frameSet->getFrame(0)->top() - tableCellSpacing);
        cell->row=rows++;
        cell->col=0;
        cell->cols=cols;
        cell->rows=1;
        bottom=cell->frameSet->getFrame(0)->bottom();
        cells.append(cell);
    }
}
