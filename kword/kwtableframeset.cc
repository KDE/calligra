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
#include <kotextobject.h> // for customItemChar !
#include <qpopupmenu.h>
#include <dcopobject.h>
#include "KWordFrameSetIface.h"
#include "KWordTableFrameSetIface.h"
#include <kmessagebox.h>

KWTableFrameSet::KWTableFrameSet( KWDocument *doc, const QString & name ) :
    KWFrameSet( doc )
{
    m_rows = 0;
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

KWTableFrameSet::~KWTableFrameSet()
{
    //delete m_anchor;
    //m_anchor = 0L;
    m_doc = 0L;
}

KWordFrameSetIface* KWTableFrameSet::dcopObject()
{
    if ( !m_dcop )
    m_dcop = new KWordTableFrameSetIface( this );

    return m_dcop;
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
        //This is wrong (reason for famous infinite loop bug)
        //Do this in KWTableFrameSet::invalidate
        //m_cells.at(i)->invalidate();
    }
    KWFrameSet::updateFrames();
}

void KWTableFrameSet::moveFloatingFrame( int /*frameNum TODO */, const KoPoint &position )
{
    double dx = position.x() - m_colPositions[0];
    double dy = position.y() - m_rowPositions[0];

    moveBy( dx, dy);

    if ( dx || dy )
        m_doc->updateAllFrames();
}

KoRect KWTableFrameSet::floatingFrameKoRect( int frameNum )
{
    return boundingRect();
}

QRect KWTableFrameSet::floatingFrameRect( int /*frameNum TODO */ )
{
    KoRect r = boundingRect();
    if ( r.isNull() )
        return QRect();
    QRect outerRect( m_doc->zoomRect( r ) );
    //kdDebug(32004) << "floatingFrameSize outerRect initially " << DEBUGRECT( outerRect ) << endl;
    Q_ASSERT( m_anchorTextFs );
    // Need to convert back to internal coords (in case of page breaking)
#if 0  // TODO
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
            //kdDebug(32004) << "floatingFrameSize outerRect now " << DEBUGRECT( outerRect ) << endl;
        }
    }
#endif

    return outerRect;
}

KCommand * KWTableFrameSet::anchoredObjectCreateCommand( int /*frameNum*/ )
{
    return new KWCreateTableCommand( i18n("Create Table"), this );
}

KCommand * KWTableFrameSet::anchoredObjectDeleteCommand( int /*frameNum*/ )
{
    return new KWDeleteTableCommand( i18n("Delete Table"), this );
}

KWAnchor * KWTableFrameSet::createAnchor( KoTextDocument *txt, int frameNum )
{
    //kdDebug(32004) << "KWTableFrameSet::createAnchor" << endl;
    return new KWAnchor( txt, this, frameNum );
}

void KWTableFrameSet::createAnchors( KWTextParag * parag, int index, bool placeHolderExists /*= false */ /*only used when loading*/ )
{
    //kdDebug(32004) << "KWTableFrameSet::createAnchors" << endl;
    // TODO make one rect per page, and create one anchor per page
    //if ( !m_anchor )
    {
        // Anchor this frame, after the previous one
        KWAnchor * anchor = createAnchor( m_anchorTextFs->textDocument(), 0 );
        if ( !placeHolderExists )
            parag->insert( index, KoTextObject::customItemChar() );
        parag->setCustomItem( index, anchor, 0 );
        kdDebug(32004) << "KWTableFrameSet::createAnchors setting anchor" << endl;
    }
    parag->setChanged( true );
    emit repaintChanged( m_anchorTextFs );
}

void KWTableFrameSet::deleteAnchors()
{
    KWAnchor * anchor = findAnchor( 0 );
    kdDebug(32004) << "KWTableFrameSet::deleteAnchors anchor=" << anchor << endl;
    deleteAnchor( anchor );
}

void KWTableFrameSet::addCell( Cell *cell )
{
    unsigned int i;

    m_rows = QMAX( cell->m_row + 1, m_rows );

    // Find the insertion point in the list.
    for ( i = 0; i < m_cells.count() && m_cells.at( i )->isAboveOrLeftOf( cell->m_row, cell->m_col ); i++ ) ;
    m_cells.insert( i, cell );

}

KoRect KWTableFrameSet::boundingRect() {
    return KoRect(m_colPositions[0],    // left
                m_rowPositions[0],      // top
                m_colPositions.last()-m_colPositions[0], // width
                m_rowPositions.last()-m_rowPositions[0]);// height
}

double KWTableFrameSet::topWithoutBorder()
{
    KWTableFrameSet::Cell *cell;
    double top = 0.0;
    int i;
    for (i=0; cell = getCell(0,i); i++)
    {
        top = QMAX( top, m_rowPositions[0] + cell->topBorder() );
    }
    return top;
}


double KWTableFrameSet::leftWithoutBorder()
{
    KWTableFrameSet::Cell *cell;
    double left = 0.0;
    int i;
    for (i=0; cell = getCell(i,0); i++)
    {
        left = QMAX( left, m_rowPositions[0] + cell->leftBorder() );
    }
    return left;
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
    if(f) return static_cast<KWTableFrameSet::Cell *> (f->frameSet());
    return 0L;
}

void KWTableFrameSet::recalcCols(int _col,int _row) {
    //kdDebug(32004) << "KWTableFrameSet::recalcCols" << endl;
    if(m_cells.isEmpty()) return; // assertion

    // check/set sizes of frames
    unsigned int row=0,col=0;
    if(_col!=-1 && _row!=-1)
    {
        row=(unsigned int)_row;
        col=(unsigned int)_col;
    }
    else
        isOneSelected(row,col);

    Cell *activeCell = getCell(row,col);
    double difference = 0;

    if(activeCell->frame(0)->left() - activeCell->leftBorder() != m_colPositions[activeCell->m_col]) {
        // left border moved.
        col = activeCell->m_row;
        difference = 0-(activeCell->frame(0)->left() - activeCell->leftBorder() - m_colPositions[activeCell->m_col]);
    }

    if(activeCell->frame(0)->right() - activeCell->rightBorder() !=
            m_colPositions[activeCell->m_col + activeCell->m_cols-1]) { // right border moved

        col = activeCell->m_col + activeCell->m_cols;
        double difference2 = activeCell->frame(0)->right() + activeCell->rightBorder() - m_colPositions[activeCell->m_col + activeCell->m_cols];

        double moved=difference2+difference;
        if(moved > -0.01 && moved < 0.01) { // we were simply moved.
                col=0;
                difference = difference2;
            } else if(difference2!=0)
                difference = difference2;
    }

    redrawFromCol=getCols(); // possible reposition col starting with this one, done in recalcRows
    if(difference!=0) {
        double last=col==0?0:m_colPositions[col-1];
        for(unsigned int i=col; i < m_colPositions.count(); i++) {
            double &colPos = m_colPositions[i];
            colPos = colPos + difference;
            if(colPos-last < minFrameWidth) { // Never make it smaller then allowed!
                difference += minFrameWidth - colPos;
                colPos = minFrameWidth + last;
            }
            last=colPos;
        }
        redrawFromCol=col;
        if(col>0) redrawFromCol--;
        //if(activeCell) activeCell->frame(0)->setMinFrameHeight(0);
    }

    //kdDebug(32004) << "end KWTableFrameSet::recalcCols" << endl;
}

void KWTableFrameSet::recalcRows(int _col, int _row) {
    kdDebug(32004) << "KWTableFrameSet::recalcRows ("<< _col <<"," << _row << ")" << endl;
    //for(unsigned int i=0; i < m_rowPositions.count() ; i++) kdDebug(32004) << "row: " << i << " = " << m_rowPositions[i] << endl;

    Q_ASSERT( !m_cells.isEmpty() );

    // check/set sizes of frames
    unsigned int row=0,col=0;
    if(_col!=-1 && _row!=-1)
    {
        row=(unsigned int)_row;
        col=(unsigned int)_col;
    }
    else
        isOneSelected(row,col);

    Cell *activeCell = getCell(row,col);
    double difference = 0;

    if(activeCell->frame(0)->height() != activeCell->frame(0)->minFrameHeight() &&
          activeCell->type() == FT_TEXT) {
        // when the amount of text changed and the frame has to be rescaled we are also called.
        // lets check the minimum size for all the cells in this row.

        // Take a square of table cells which depend on each others height. It is always the full
        // width of the table and the height is determened by joined cells, the minimum is one row.
        double minHeightOtherCols=0;    // The minimum height which the whole square of table cells can take
        double minHeightActiveRow=0;    // The minimum height our cell can get because of cells in his row
        double minHeightMyCol=0;        // The minimum height our column can get in the whole square
        unsigned int rowSpan=activeCell->m_rows;
        unsigned int startRow=activeCell->m_row;
        int colCount=0;
        do {
            unsigned int rowCount=startRow;
            double thisColHeight=0;     // the total height of this column
            double thisColActiveRow=0;  // the total height of all cells in this col, completely in the
                                        // row of the activeCell
            do {
                Cell *thisCell=getCell(rowCount,colCount);
                if(thisCell->m_row < startRow) {
                    rowSpan+=startRow-thisCell->m_row;
                    startRow=thisCell->m_row;
                    colCount=-1;
                    break;
                }
                if(thisCell->m_row + thisCell->m_rows > startRow + rowSpan) {
                    rowSpan = thisCell->m_row + thisCell->m_rows - startRow;
                    colCount=-1;
                    break;
                }
                thisColHeight+=thisCell->frame(0)->minFrameHeight();
                thisColHeight+=thisCell->topBorder();
                thisColHeight+=thisCell->bottomBorder();
                if(thisCell->m_row >= activeCell->m_row && thisCell->m_row + thisCell->m_rows <= activeCell->m_row + activeCell->m_rows)
                    thisColActiveRow+=thisCell->frame(0)->minFrameHeight();
                rowCount+=thisCell->m_rows;
            } while (rowCount < rowSpan+startRow);

            if(static_cast<unsigned int>(colCount) >= activeCell->m_col &&
                  static_cast<unsigned int>(colCount) < activeCell->m_col + activeCell->m_cols)
                minHeightMyCol=thisColHeight;
            else {
                minHeightOtherCols = QMAX(minHeightOtherCols, thisColHeight);
                minHeightActiveRow = QMAX(minHeightActiveRow, thisColActiveRow);
            }
            colCount++;
        } while(static_cast<unsigned int>(colCount) < getCols());

#if 0
    kdDebug(32004) << "activeCell: " << activeCell->m_row << ","<< activeCell->m_col << endl;
    kdDebug(32004) << "activeCell height. Cur:  " << activeCell->frame(0)->height() << ", new "<< activeCell->frame(0)->minFrameHeight() << endl;
    kdDebug(32004) << "minHeightOtherCols: " << minHeightOtherCols << endl;
    kdDebug(32004) << "minHeightActiveRow: " << minHeightActiveRow << endl;
    kdDebug(32004) << "minHeightMyCol: " << minHeightMyCol << endl;
    kdDebug(32004) << "rowSpan: " << rowSpan << endl;
    kdDebug(32004) << "startRow: " << startRow << endl;
#endif

        bool bottomRow=startRow+rowSpan == activeCell->m_row+activeCell->m_rows;
        if(!bottomRow) {
            Cell *bottomCell=getCell(startRow+rowSpan-1, activeCell->m_col);
            bottomCell->frame(0)->setHeight(bottomCell->frame(0)->minFrameHeight() +
                    minHeightOtherCols - minHeightMyCol);
            recalcRows(bottomCell->m_col, bottomCell->m_row);
        }
        if(activeCell->frame(0)->minFrameHeight() > activeCell->frame(0)->height()) { // wants to grow
            activeCell->frame(0)->setHeight(activeCell->frame(0)->minFrameHeight());
        } else { // wants to shrink
            double newHeight=QMAX(activeCell->frame(0)->minFrameHeight(),minHeightActiveRow);
            if(bottomRow) // I'm a strech cell
                newHeight=QMAX(newHeight, minHeightOtherCols - (minHeightMyCol - activeCell->frame(0)->minFrameHeight()));
            activeCell->frame(0)->setHeight(newHeight);
        }
    }

    if(activeCell->frame(0)->top() - activeCell->topBorder() != getPositionOfRow(activeCell->m_row)) {
        // top moved.
        row = activeCell->m_row;
        difference = 0 - (activeCell->frame(0)->top() - activeCell->topBorder() - getPositionOfRow(row));
    }


    if(activeCell->frame(0)->bottom() + activeCell->bottomBorder() !=
            getPositionOfRow(activeCell->m_row + activeCell->m_rows)) { // bottom moved

        row = activeCell->m_row + activeCell->m_rows;
        double difference2 = activeCell->frame(0)->bottom() + activeCell->bottomBorder() - getPositionOfRow(row);
        double moved=difference2+difference;
        if(moved > -0.01 && moved < 0.01) { // we were simply moved.
            row=0;
            difference = difference2;
        } else if( difference2!=0)
            difference = difference2;
    }

    unsigned int fromRow=m_rows; // possible reposition rows starting with this one, default to no repositioning
    unsigned int untilRow=0;     // possible reposition rows ending with this one
    if(difference!=0) {
        QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
        QValueList<double>::iterator j = m_rowPositions.begin();
        double last=0.0;
        int lineNumber=-1;
        while(j != m_rowPositions.end()) {
            lineNumber++;
            if(pageBound!=m_pageBoundaries.end()) {
                if(*pageBound == lineNumber) { // next page
                    if(lineNumber >= row) { // then delete line j
                        // TODO remove headers and  m_hasTmpHeaders = false;
                        QValueList<double>::iterator nextJ = j;
                        ++nextJ;
                        difference -= *(nextJ)-*(j);
                        kdDebug(32004) << "Deleting line with old pos: " << *j << endl;
                        j=m_rowPositions.remove(j);
                        j--;
                        QValueList<unsigned int>::iterator tmp = pageBound;
                        ++pageBound;
                        m_pageBoundaries.remove(tmp);
                        j++;
                        continue;
                    }
                    ++pageBound;
                    lineNumber--;
                }
            }
            if(lineNumber >= row)  { // below changed row
                if(*(j)-last < minFrameHeight) // Never make it smaller then allowed!
                    difference += minFrameHeight - *(j) + last;
                last=*(j);
                kdDebug(32004) << "moving " << *(j) << " by " << difference << "; to " << (*j) + difference << endl;
                (*j) = (*j) + difference; // move line.
            }
            j++;
        }
        fromRow=row;
        if(row>0) fromRow--;
    } else {
        row=0;
    }
#if 0
{ QValueList<unsigned int>::iterator pb = m_pageBoundaries.begin();
  unsigned int i=0;
  double last=0;
  do {
      double cur=m_rowPositions[i];
      if(pb!=m_pageBoundaries.end() && *(pb)==i) {
        kdDebug(32004) << "line: " << i << ": " << cur << " *" << (last>cur?" (ALERT)":"") << endl;
        ++pb;
      } else
        kdDebug(32004) << "line: " << i << ": " << cur << (last>cur?" (ALERT)":"") << endl;
      last=cur;
      i++;
  } while( i<m_rowPositions.count());
}
#endif

    //double pageHeight = m_doc->ptPaperHeight() - m_doc->ptBottomBorder() - m_doc->ptTopBorder();
    unsigned int pageNumber=getCell(0,0)->frame(0)->pageNum() +1;
    unsigned int lineNumber=1;
    QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
    QValueList<double>::iterator j = m_rowPositions.begin();

    double diff=0.0;
    double pageBottom = pageNumber * m_doc->ptPaperHeight() - m_doc->ptBottomBorder();
//kdDebug(32004) << "pageBottom; " << pageBottom << endl;
    while(++j!=m_rowPositions.end()) { // stuff for multipage tables.
        if(pageBound!=m_pageBoundaries.end() && *pageBound == lineNumber ) {
            if(*j > pageNumber * m_doc->ptPaperHeight() - m_doc->ptBottomBorder() ) { // next page marker exists, and is accurate...
                pageNumber++;
                pageBottom = pageNumber * m_doc->ptPaperHeight() - m_doc->ptBottomBorder();
//kdDebug(32004) << "pageBottom; " << pageBottom << endl;
                untilRow=QMAX(untilRow, *pageBound);
                pageBound++;
            }
        }

//kdDebug() << "checking; " << lineNumber << ", " << (*j) << endl;
        if((*j) + diff > pageBottom) { // a row falls off the page.
//kdDebug(32004) << "row falls off of page"<< endl;
            untilRow=m_rows;
            bool hugeRow = false;
            unsigned int breakRow = lineNumber-1;
            // find out of no cells are spanning multiple rows meaning we have to break higher.
#if 0
    I did not get a chance do debug this yet!  TZ.
            for(int i=0; i < getCols() ; i++) {
kdDebug() << "i: " << i<< endl;
Cell *c= getCell(breakRow, i);
kdDebug() << "c: " << c->m_row << "," << c->m_col << " w: " << c->m_cols << ", h: " << c->m_rows << endl;
                if(getCell(breakRow,i)->m_row < breakRow) {
                    breakRow = getCell(breakRow,i)->m_row;
                    i=-1;
                }
            }
kdDebug() << "breakRow: " << breakRow<< endl;
            fromRow=QMIN(fromRow, breakRow);
if(breakRow < lineNumber+1) {
    for(unsigned int i=lineNumber+1; i > breakRow;i--)
kdDebug() << "j--";
    for(unsigned int i=lineNumber+1; i > breakRow;i--)
        --j;
    lineNumber=breakRow+1;
}

            // find out if the next row (the new one on the page) does not contain cells higher then page.
            for(unsigned int i=0; i < getCols() ; i++) {
                if(getCell(breakRow+1,i) && getCell(breakRow+1,i)->frame(0)->height() > pageHeight)
                    hugeRow=true;
            }
            //if((*pageBound) != breakRow) { // ik denk dat dit wel moet..
                // voeg top in in rowPositions
#endif
            double topOfPage = m_doc->ptPaperHeight() * pageNumber + m_doc->ptTopBorder();
            QValueList<double>::iterator tmp = m_rowPositions.at(breakRow);
            diff += topOfPage - (*tmp); // diff between bottom of last row on page and top of new page
//kdDebug() << "diff += " <<  topOfPage  << " - " << (*tmp) << ". diff += " << topOfPage - (*tmp) <<" ="<< diff  << endl;
            lineNumber++;
            m_rowPositions.insert(j, topOfPage);

            // insert new pageBound. It points to last LINE on previous page
            pageBound = m_pageBoundaries.insert(pageBound, breakRow);
            //kdDebug(32004) << "inserting new pageBound: " << breakRow  << " at " << m_rowPositions[breakRow] << endl;
            pageBound++;
            if(!hugeRow) {
                // add header-rij toe. (en zet bool) TODO
                //j++;
                //lineNumber++;
                // m_hasTmpHeaders = true;
            }
            pageNumber++;
            pageBottom = pageNumber * m_doc->ptPaperHeight() - m_doc->ptBottomBorder();
            //kdDebug(32004) << "pageBottom; " << pageBottom << endl;
            if(pageNumber > m_doc->getPages()) m_doc->appendPage();
        }
        //if(diff > 0)  kdDebug(32004) << "   adding " << diff << ", line " << lineNumber << " " << *(j) <<" -> " << *(j)+diff << endl;
        if(diff > 0)
            (*j) = (*j) + diff;

        lineNumber++;
    }
#if 0
{ QValueList<unsigned int>::iterator pb = m_pageBoundaries.begin();
  unsigned int i=0;
  double last=0;
  do {
      double cur=m_rowPositions[i];
      if(pb!=m_pageBoundaries.end() && *(pb)==i) {
        kdDebug(32004) << "line: " << i << ": " << cur << " *" << (last>cur?" (ALERT)":"") << endl;
        ++pb;
      } else
        kdDebug(32004) << "line: " << i << ": " << cur << (last>cur?" (ALERT)":"") << endl;
      last=cur;
      i++;
  } while( i<m_rowPositions.count());
}
#endif
//for (unsigned int i=0; i < getRows(); kdDebug(32004)<<" pos of row["<<i<<"] = "<<getPositionOfRow(i)<<"/"<<getPositionOfRow(i,true)<<endl,i++);
    //kdDebug () << "Repositioning from row : " << fromRow << " until: " << untilRow << endl;
    //kdDebug () << "Repositioning from col > " << redrawFromCol << endl;
    // do positioning.
    Cell *cell;
    //bool setMinFrameSize= activeCell->frame(0)->isSelected();
    for(cell=m_cells.first();cell;cell=m_cells.next()) {
        if((cell->m_row + cell->m_rows > fromRow && cell->m_row < untilRow) || cell->m_col + cell->m_cols > redrawFromCol)
            position(cell, (cell==activeCell && cell->frame(0)->isSelected()));
    }
    redrawFromCol=getCols();

    // check if any rowPosition entries are unused

    //   first create a hash of all row entries
    QMap<unsigned int,int> rows;        // rownr, count
    unsigned int top=m_rowPositions.count() - m_pageBoundaries.count()-1;
    for(unsigned int i=0; i < top; rows[i++]=0);

    //   fill hash with data
    for (cell = m_cells.first(); cell; cell = m_cells.next()) {
        rows[cell->m_row]+=1;
    }

    //   check if some entries have stayed unused.
    unsigned int counter=top;
    int adjustment=m_pageBoundaries.count()-1;

    do {
        counter--;
        if(adjustment >= 0 && counter == m_pageBoundaries[adjustment])
            adjustment--;
        if(rows[counter]==0) {
            m_rows--;
            m_rowPositions.erase(m_rowPositions.at(counter+(adjustment>0?adjustment:0)));
            for (cell = m_cells.first(); cell; cell = m_cells.next()) {
                if(cell->m_row < counter && cell->m_rows + cell->m_row > counter)
                    cell->m_rows--;
                if(cell->m_row > counter)
                    cell->m_row--;
            }

            if(adjustment >= -1) {
                pageBound = m_pageBoundaries.at(adjustment+1);
                while(pageBound!=m_pageBoundaries.end()) {
                    (*pageBound)= (*pageBound)-1;
                    pageBound++;
                }
            }
        }
    } while(counter!=0);


    redrawFromCol=0;
    for(cell=m_cells.first();cell;cell=m_cells.next()) {
        if((cell->m_row + cell->m_rows > fromRow && cell->m_row < untilRow) || cell->m_col + cell->m_cols > redrawFromCol)
            position(cell);
    }
    redrawFromCol=getCols();
    kdDebug(32004) << "KWTableFrameSet::recalcRows done" << endl;
}

void KWTableFrameSet::resizeWidth( double width ) {
    Q_ASSERT(width != 0);
    Q_ASSERT(boundingRect().width() != 0);
    kdDebug() << "bounding width before resize " << boundingRect().width() << endl;
    double growth = width / boundingRect().width();

    // since we move all the columns, we also move the 1st one,
    // depending where it is on the page.
    // just compensate by substracting that offset.
    double moveOffset = m_colPositions[0] * growth - m_colPositions[0];

    for (uint i=0; i<m_colPositions.count(); i++) {
        m_colPositions[i] = m_colPositions[i] * growth - moveOffset;
    }
    finalize();
    kdDebug() << "bounding width after resize" << boundingRect().width() << endl;
    Q_ASSERT(boundingRect().width() - width < 0.01);
}

void KWTableFrameSet::setBoundingRect( KoRect rect, CellSize widthMode, CellSize heightMode ) {
   // Column postions..
    m_rowPositions.clear();
    unsigned int cols=0;
    for(QPtrListIterator<Cell> c(m_cells); c.current();  ++c)
        cols=QMAX(cols, c.current()->m_col + c.current()->m_cols);
    double colWidth = rect.width() / cols;
    if ( widthMode == TblAuto ) {
        rect.setLeft( m_doc->ptLeftBorder() );
        colWidth = (m_doc->ptPaperWidth() - m_doc->ptLeftBorder() - m_doc->ptRightBorder()) / cols;
    }

    for(unsigned int i=0; i <= cols;i++) {
        m_colPositions.append(rect.x() + colWidth * i);
    }

    // Row positions..
    m_rowPositions.clear();
    m_pageBoundaries.clear();
    double rowHeight = 0;
    if( heightMode != TblAuto )
        rowHeight = rect.height() / m_rows;
    rowHeight=QMAX(rowHeight, 22); // m_doc->getDefaultParagLayout()->getFormat().ptFontSize()) // TODO use table style font-size

    for(unsigned int i=0; i <= m_rows;i++) {
        m_rowPositions.append(rect.y() + rowHeight * i);
    }

    Cell *theCell;
    double oneMm = MM_TO_POINT( 1.0 );
    for(theCell=m_cells.first(); theCell; theCell=m_cells.next()) {
        KWFrame *frame = theCell->frame(0);
        frame->setBLeft( oneMm );
        frame->setBRight( oneMm );
        frame->setBTop( oneMm );
        frame->setBBottom( oneMm );
        frame->setNewFrameBehavior( KWFrame::NoFollowup );
        position(theCell, true);
    }
}

void KWTableFrameSet::position( Cell *theCell, bool setMinFrameHeight ) {
    if(!theCell->frame(0)) { // sanity check.
        kdDebug(32004) << "errorous table cell!! row:" << theCell->m_row << ", col: " << theCell->m_col << endl;
        return;
    }
    double x= *m_colPositions.at(theCell->m_col);
    double y= getPositionOfRow(theCell->m_row);
    double width = (*m_colPositions.at(theCell->m_col + theCell->m_cols)) - x;
    double height  = getPositionOfRow(theCell->m_row + theCell->m_rows-1, true) - y;

#if 0
    if(theCell->m_col==0) {
        kdDebug(32004) << "row "  << theCell->m_row << " has top: " << y << ", and bottom: " << y + height << endl;
    }
#endif

    // Now take the border sizes and make the cell smaller so it still fits inside the grid.
    KWFrame *theFrame = theCell->frame(0);
    x+=theCell->leftBorder();
    width-=theCell->leftBorder();
    width-=theCell->rightBorder();
    y+=theCell->topBorder();
    height-=theCell->topBorder();
    height-=theCell->bottomBorder();

    theFrame->setRect( x,y,width,height);
    if( setMinFrameHeight ) {
        theFrame->setMinFrameHeight(height);
    }

    if(!theCell->isVisible())
        theCell->setVisible(true);
}

double KWTableFrameSet::getPositionOfRow( unsigned int row, bool bottom ) {
    unsigned int adjustment=0;
    QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
    while(pageBound != m_pageBoundaries.end() && (*pageBound) <= row + adjustment) {
        adjustment++;
        pageBound++;
    }
    if(m_rowPositions.count() < row+adjustment+(bottom?1:0))  // Requested row does not exist.
        return 0;
    return m_rowPositions[row+adjustment+(bottom?1:0)];
}

bool KWTableFrameSet::hasSelectedFrame()
{
    unsigned int a=0,b=0;
    return getFirstSelected(a,b);
}

void KWTableFrameSet::moveBy( double dx, double dy ) {
    bool redraw=false;
kdDebug(32004) << "KWTableFrameSet::moveBy(" << dx<<","<<dy<<")\n";
//for(unsigned int i=0; i < m_rowPositions.count() ; kdDebug(32004) << "row " << i << ": " << m_rowPositions[i++] << endl);
    if(!(dy > -0.001 && dy < 0.001)) {
        redraw=true;
        QValueList<double>::iterator row = m_rowPositions.begin();
        while(row != m_rowPositions.end()) {
            (*row)= (*row)+dy;
            row++;
        }
    }
    if(!(dx > -0.001 && dx < 0.001)) {
        redraw=true;
        QValueList<double>::iterator col = m_colPositions.begin();
        while(col != m_colPositions.end()) {
            (*col)= (*col)+dx;
            col++;
        }
    }

    if(redraw) {
        Cell *cell;
        for(cell=m_cells.first();cell;cell=m_cells.next())
            position(cell);
    }
}

void KWTableFrameSet::selectRow(uint row)
{
    Q_ASSERT(row < m_rows);

    for ( uint i = 0; i < getCols(); i++ )
        getCell(row, i)->frame(0)->setSelected( true );
}

void KWTableFrameSet::selectCol(uint col)
{
    Q_ASSERT(col < m_colPositions.count()-1);

    for ( uint i = 0; i < getRows(); i++ )
        getCell(i, col)->frame(0)->setSelected( true );
}

void KWTableFrameSet::deselectAll()
{
    for ( unsigned int i = 0; i < m_cells.count(); i++ )
        m_cells.at( i )->frame( 0 )->setSelected( false );
}

void KWTableFrameSet::refreshSelectedCell()
{
    unsigned int row,col;
    if ( !isOneSelected( row, col ) )
        return;

    Cell *cell=getCell(row,col);
    if(cell)
        cell->frame( 0 )->updateResizeHandles();
}

void KWTableFrameSet::selectUntil( double x, double y) {
    KWFrame *f = frameAtPos(x,y);
    if(f) selectUntil(static_cast<KWTableFrameSet::Cell *> (f->frameSet()));
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
            cell->frame( 0 )->setSelected( true );
            cell->frame(0)->createResizeHandles();
            cell->frame(0)->updateResizeHandles();
        }
        else
        {
            if(cell->frame( 0 )->isSelected())
            {
                cell->frame( 0 )->setSelected( false );
                cell->frame(0)->removeResizeHandles();
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
        if(m_cells.at(i)->frame(0)->isSelected())  {
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

bool KWTableFrameSet::isRowSelected(uint row) {
    Q_ASSERT(row <= getRows());
    // if just one cell of the row is not selected, the row is not selected
    for ( uint i = 0; i < m_cells.count(); i++ ) {
        if (!m_cells.at( i )->frame( 0 )->isSelected()) {
            if (m_cells.at( i )->m_row == row)
            {
                kdDebug() << "row " << row << " row is not selected" << endl;
                return false;
            }
        }
    }
    kdDebug() << "row " << row << " row is selected" << endl;
    return true;
}

bool KWTableFrameSet::isColSelected(uint column) {
    Q_ASSERT(column <= getCols());
    // if just one cell of the col is not selected, the col is not selected
    for ( uint i = 0; i < m_cells.count(); i++ ) {
        if (!m_cells.at( i )->frame( 0 )->isSelected()) {
            if (m_cells.at( i )->m_col == column)
            {
                kdDebug() << "column " << column << " column is not selected" << endl;
                return false;
            }
        }
    }
    kdDebug() << "column " << column << " column is selected" << endl;
    return true;
}

bool KWTableFrameSet::isRowsSelected() {
    for (uint i=0;i<getRows();i++) {
        if (isRowSelected(i))
            return true;
    }
    return false;
}

bool KWTableFrameSet::isColsSelected() {
    for (uint i=0;i<getCols();i++) {
        if (isColSelected(i))
            return true;
    }
    return false;
}

bool KWTableFrameSet::getFirstSelected( unsigned int &row, unsigned int &col )
{
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        if (m_cells.at( i )->frame( 0 )->isSelected()) {
            row = m_cells.at( i )->m_row;
            col = m_cells.at( i )->m_col;
            return true;
        }
    }
    return false;
}

void KWTableFrameSet::insertRow( unsigned int newRowNumber,QPtrList<KWFrameSet> redoFrameset, QPtrList<KWFrame> redoFrame,bool recalc, bool isAHeader ) {

    unsigned int copyFromRow = newRowNumber==0?0:newRowNumber-1;
    if(newRowNumber==0)
        copyFromRow=1;

    // What height to use for the new row
    double height = 0;
    if(redoFrame.isEmpty())
        height = getPositionOfRow(copyFromRow,true) - getPositionOfRow(copyFromRow);
    else {
        KWFrame *f = redoFrame.at(0);
        height = f->height() + f->topBorder().width() + f->bottomBorder().width();;
    }

    // Calculate offset in QValueList because of page breaks.
    unsigned int adjustment=0;
    unsigned int untilRow=m_rows;
    QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
    while(pageBound != m_pageBoundaries.end() && (*pageBound) <= newRowNumber) {
        // Find out how many pages we allready had.
        adjustment++;
        pageBound++;
    }

    // Move all rows down from newRow to bottom of page
    QValueList<double>::iterator tmp = m_rowPositions.at(newRowNumber);
    double newPos = *tmp + height;
    tmp++;
    m_rowPositions.insert(tmp, newPos);
    for(unsigned int i=newRowNumber+adjustment+2; i < m_rowPositions.count(); i++) {
        double &rowPos = m_rowPositions[i];
kdDebug(32004) << "adjusting " << rowPos << " -> " << rowPos + height << endl;
        rowPos = rowPos + height;
        if(*pageBound == i) {
            untilRow= *pageBound;
            break;              // stop at pageBreak.
        }
    }

    // Position all changed cells.
    unsigned int newRows=++m_rows;
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        Cell *cell = m_cells.at(i);
        if ( cell->m_row >= newRowNumber ) { // move all cells beneath the new row.
            cell->m_row++;
            position(cell);
        }
    }

    unsigned int i =0;
    double oneMm = MM_TO_POINT( 1.0 );
    while(i < getCols()) {
        KWFrame *theFrame = 0L;
        if(redoFrame.isEmpty())
        {
            theFrame=new KWFrame(0L, 1, 1, 100, 20, KWFrame::RA_NO); // use dummy values here...
            theFrame->setFrameBehavior(KWFrame::AutoExtendFrame);
            theFrame->setNewFrameBehavior(KWFrame::NoFollowup);
            // copying the zorder of the neighbouring cell in the table :
            theFrame->setZOrder(getCell(newRowNumber+(newRowNumber==0?1:-1), i)->frame(0)->zOrder());
            theFrame->setMinFrameHeight(theFrame->height());
        }
        else
        {
            if(i<redoFrame.count())
                theFrame=redoFrame.at(i)->getCopy();
        }

        Cell *newCell=0L;
        if(redoFrameset.isEmpty())
            newCell=new Cell( this, newRowNumber, i, QString::null );
        else
        {
            if( i<redoFrameset.count())
            {
                newCell = static_cast<KWTableFrameSet::Cell*> (redoFrameset.at(i));
                addCell( newCell );
            }
            else
            {
                newCell =getCell(newRowNumber-1,i);
                newCell->m_rows=newCell->m_rows+1;
                continue;
            }
        }

        newCell->m_cols=getCell(copyFromRow,i)->m_cols;
        newCell->setIsRemoveableHeader( isAHeader );
        newCell->addFrame( theFrame, false );
        theFrame->setBLeft( oneMm );
        theFrame->setBRight( oneMm );
        theFrame->setBTop( oneMm );
        theFrame->setBBottom( oneMm );
        position(newCell);

        i+=newCell->m_cols;
    }

    m_rows=newRows;
    if ( recalc )
        finalize();
}

void KWTableFrameSet::insertCol( unsigned int newColNumber,QPtrList<KWFrameSet> redoFrameset, QPtrList<KWFrame>redoFrame, double width /* =KWTableFrameSet::ms_defaultWidth.*/ ) {
    width=QMAX(width,minFrameWidth);

    if(! redoFrame.isEmpty()) {
        KWFrame *f=redoFrameset.at(0)->frame(0);
        width=f->width() + f->leftBorder().width() + f->rightBorder().width();
    }

    QValueList<double>::iterator tmp = m_colPositions.at(newColNumber);

    tmp=m_colPositions.insert(tmp, *tmp);
    tmp++;
    while(tmp!=m_colPositions.end()) {
        (*tmp)= (*tmp)+width;
        tmp++;
    }

    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        Cell *cell = m_cells.at(i);
        if ( cell->m_col >= newColNumber) { // move all cells right of the new col.
            cell->m_col++;
            position(cell);
        }
    }

    for( unsigned int i = 0; i < getRows(); i++ ) {
        int rows;
        Cell *cell;
        if(newColNumber > 0 ) {
            cell = getCell(i, newColNumber-1);
            if(cell->m_col + cell->m_cols > newColNumber) {
                // cell overlaps the new column
                cell->m_cols++;
                continue;
            }
            rows = cell->m_rows;
        } else {
            rows = 1;
            cell = getCell(i, newColNumber+1);
        }
        Cell *newCell=0L;
        if(redoFrameset.isEmpty())
            newCell = new Cell( this, i, newColNumber, QString::null );
        else
        {
            if( i<redoFrameset.count())
            {
                newCell = static_cast<KWTableFrameSet::Cell*> (redoFrameset.at(i));
                addCell( newCell );
            }
            else
            {
                newCell =getCell(i,newColNumber-1);
                newCell->m_cols=newCell->m_cols+1;
                continue;
            }
        }
        KWFrame *theFrame = 0L;
        if(redoFrame.isEmpty())
        {
            theFrame=new KWFrame(newCell, 1, 1, width, 20, KWFrame::RA_NO ); // dummy values..
            theFrame->setFrameBehavior(KWFrame::AutoExtendFrame);
            theFrame->setNewFrameBehavior(KWFrame::NoFollowup);
            // copying the zorder of the neighbouring cell in the table :
            theFrame->setZOrder(getCell(i, newColNumber+(newColNumber==0?1:-1))->frame(0)->zOrder());
            theFrame->setMinFrameHeight(theFrame->height());
        }
        else
        {
            theFrame=redoFrame.at(i)->getCopy();
        }
        newCell->addFrame( theFrame,false );
        if(cell->m_rows >1) {
            newCell->m_rows = cell->m_rows;
            i+=cell->m_rows -1;
        }
        position(newCell);
    }
    finalize();
}

/* Delete all cells that are completely in this row.              */
void KWTableFrameSet::deleteRow( unsigned int row, bool _recalc )
{
    unsigned int rowspan=0;
    // I want to know the amount of the row(s) I am removing.
    for (unsigned int rs=1; rs < m_rows && rowspan == 0; rs++) {
        for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
            if(m_cells.at(i)->m_row == row && m_cells.at(i)->m_rows==rs) {
                rowspan=rs;
                break;
            }
        }
    }

    double height= getPositionOfRow(row+rowspan-1,true) - getPositionOfRow(row);
    QValueList<double>::iterator tmp = m_rowPositions.at(row+rowspan);
    tmp=m_rowPositions.erase(tmp);
    while(tmp!=m_rowPositions.end()) {
        (*tmp)= (*tmp)-height;
        tmp++;
    }

    // move/delete cells.
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        Cell *cell = m_cells.at(i);
        if ( row >= cell->m_row  && row < cell->m_row + cell->m_rows) { // cell is indeed in row
            if(cell->m_rows == 1) { // lets remove it
                frames.remove( cell->frame(0) );
                cell->delFrame( cell->frame(0));
                m_cells.take(i);
                i--;
            } else { // make cell span rowspan less rows
                cell->m_rows -= rowspan;
                position(cell);
            }
        } else if ( cell->m_row > row ) {
            // move cells to the left
            cell->m_row -= rowspan;
            position(cell);
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
    for (unsigned int colspan=1; colspan < getCols() && width==0; colspan++) {
        for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
            if(m_cells.at(i)->m_col == col && m_cells.at(i)->m_cols==colspan) {
                width=m_cells.at(i)->frame(0)->width();
                break;
            }
        }
    }

    QValueList<double>::iterator tmp = m_colPositions.at(col+1);
    tmp=m_colPositions.erase(tmp);
    while(tmp!=m_colPositions.end()) {
        (*tmp)= (*tmp)-width;
        tmp++;
    }

    // move/delete cells.
    for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
        Cell *cell = m_cells.at(i);
        if ( col >= cell->m_col  && col < cell->m_col + cell->m_cols) { // cell is indeed in col
            if(cell->m_cols == 1) { // lets remove it
                frames.remove( cell->frame(0) );
                cell->delFrame( cell->frame(0));
                m_cells.take(i);
                i--;
            } else { // make cell span colspan less cols
                cell->m_cols -= colspan;
                position(cell);
            }
        } else if ( cell->m_col > col ) {
            // move cells to the left
            cell->m_col -= colspan;
            position(cell);
        }
    }

    recalcCols();
    recalcRows();
}

void KWTableFrameSet::ungroup()
{
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
            if(cell->frame(0)->isSelected()) {
                colEnd+=cell->m_cols;
            } else
                break;
        }

        while(rowEnd+1 < getRows()) { // count all vertical selected cells
            Cell *cell = getCell(rowEnd+1, colBegin);
            if(cell->frame(0)->isSelected()) {
                for(unsigned int j=1; j <= cell->m_rows; j++) {
                    for(unsigned int i=colBegin; i<=colEnd; i++) {
                        if(! getCell(rowEnd+j,i)->frame(0)->isSelected())
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

    QPtrList<KWFrameSet> listFrameSet;
    QPtrList<KWFrame> listCopyFrame;

    // do the actual merge.
    for(unsigned int i=colBegin; i<=colEnd;i++) {
        for(unsigned int j=rowBegin; j<=rowEnd;j++) {
            Cell *cell = getCell(j,i);
            if(cell && cell!=firstCell) {
                listFrameSet.append(cell);
                listCopyFrame.append(cell->frame(0)->getCopy());
                frames.remove( cell->frame(0) );
                cell->delFrame( cell->frame(0));
                m_cells.take(m_cells.find(cell));
            }
        }
    }

    Q_ASSERT(firstCell);
    // update firstcell properties to reflect the merge
    firstCell->m_cols=colEnd-colBegin+1;
    firstCell->m_rows=rowEnd-rowBegin+1;
    position(firstCell);
    firstCell->frame(0)->updateResizeHandles();

    m_doc->updateAllFrames();
    m_doc->repaintAllViews();
    return new KWJoinCellCommand( i18n("Join Cells"), this,colBegin,rowBegin, colEnd,rowEnd,listFrameSet,listCopyFrame);
}

KCommand *KWTableFrameSet::splitCell(unsigned int intoRows, unsigned int intoCols, int _col, int _row,QPtrList<KWFrameSet> listFrameSet, QPtrList<KWFrame>listFrame) {
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
    int rowsDiff = intoRows-cell->m_rows;
    int colsDiff = ((int) intoCols)-cell->m_cols;

    if(rowsDiff >0) {
        unsigned int adjustment=0;
        QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
        while(pageBound != m_pageBoundaries.end() && (*pageBound) <= row) {
            adjustment++;
            pageBound++;
        }
        double height = (m_rowPositions[row+adjustment+1] - m_rowPositions[row+adjustment])/intoRows;

        QValueList<double>::iterator iRow = m_rowPositions.at(adjustment+row);
        for (int i=0; i < rowsDiff; i++) {
            double newPos = *iRow + height;
            iRow++;
            iRow=m_rowPositions.insert(iRow, newPos);
        }

//for(unsigned int i=0; i < m_rowPositions.count() ; kdDebug(32004) << "row " << i << ": " << m_rowPositions[i++] << endl);
    }
    if(colsDiff >0) {
        double width = (m_colPositions[col+1] - m_colPositions[col])/intoCols;

        QValueList<double>::iterator iCol = m_colPositions.at(col);
        for (int i=0; i < colsDiff; i++) {
            double newPos = *iCol + width;
            iCol++;
            iCol=m_colPositions.insert(iCol, newPos);
        }

//for(unsigned int i=0; i < m_colPositions.count() ; kdDebug(32004) << "col " << i << ": " << m_colPositions[i++] << endl);
    }

    KWFrame *firstFrame = cell->frame(0);

    // adjust cellspan and rowspan on other cells.
    for (unsigned int i=0; i< m_cells.count() ; i++) {
        Cell *theCell = m_cells.at(i);
        if(cell == theCell) continue;

        if(rowsDiff>0) {
            if(row >= theCell->m_row && row < theCell->m_row + theCell->m_rows)
                theCell->m_rows+=rowsDiff;
            if(theCell->m_row > row) {
                theCell->m_row+=rowsDiff;
            //    theCell->frame(0)->setTop(theCell->frame(0)->top()+extraHeight);
            }
        }
        if(colsDiff>0) {
            if(col >= theCell->m_col && col < theCell->m_col + theCell->m_cols)
                theCell->m_cols+=colsDiff;
            if(theCell->m_col > col) theCell->m_col+=colsDiff;
        }
        /*if(extraHeight != 0 && theCell->m_row == row) {
            theCell->frame(0)->setHeight(theCell->frame(0)->height()+extraHeight);
        } */
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

            KWFrame *theFrame=0L;
            if(listFrame.isEmpty())
            {
                theFrame=firstFrame->getCopy();
                theFrame->setRunAround( KWFrame::RA_NO );
                theFrame->setFrameBehavior(KWFrame::AutoExtendFrame);
                theFrame->setNewFrameBehavior(KWFrame::NoFollowup);
                lastFrameSet->addFrame( theFrame,false );
            }
            else
                lastFrameSet->addFrame( listFrame.at(i)->getCopy(),false );
            i++;

            // if the orig cell spans more rows/cols than it is split into, make first col/row wider.
            if(rowsDiff <0 && y==0)
                lastFrameSet->m_rows -=rowsDiff;
            if(colsDiff <0 && x==0)
                lastFrameSet->m_cols -=colsDiff;

            position(lastFrameSet);
        }
    }

    position(cell);
    firstFrame->setSelected(true);
    firstFrame->createResizeHandles();

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
        KWFrame *theFrame = getCell(j)->frame(0);
        if(theFrame->frameBehavior()==KWFrame::AutoCreateNewFrame) {
            theFrame->setFrameBehavior(KWFrame::AutoExtendFrame);
            kdWarning() << "Table cell property frameBehavior was incorrect; fixed" << endl;
        }
        if(theFrame->newFrameBehavior()!=KWFrame::NoFollowup) {
            kdWarning() << "Table cell property newFrameBehavior was incorrect; fixed" << endl;
            theFrame->setNewFrameBehavior(KWFrame::NoFollowup);
        }
    }

    QPtrList<Cell> misplacedCells;

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
                QString name = m_doc->generateFramesetName( i18n( "1 is table name, 2 is a number", "%1 Auto Added Cell %2" ).arg( getName() ) );
                Cell *cell = new Cell( this, row, col, name );
                KWFrame *theFrame = new KWFrame(cell, 10, 10, 20, 20, KWFrame::RA_NO );
                theFrame->setFrameBehavior(KWFrame::AutoExtendFrame);
                theFrame->setNewFrameBehavior(KWFrame::NoFollowup);
                cell->addFrame( theFrame,false );
                cell->m_rows = 1;
                cell->m_cols = 1;
                position(cell);
            }
        }
    }
    while (! misplacedCells.isEmpty()) {
        // append cell at bottom of table.
        Cell *cell = misplacedCells.take(0);
        cell->m_row = m_rows++;
        cell->m_col = 0;
        cell->m_cols = getCols();
        cell->m_rows = 1;
        m_cells.append(cell);
        position(cell);
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

        first = m_cells.at((m_pageBoundaries[i-1]))->frame( 0 );
        if(m_pageBoundaries[i] != 0)
        {
            KWTableFrameSet::Cell *cell=m_cells.at(m_pageBoundaries[i] -1);
            //not cell at right
            if((cell->m_cols+cell->m_col<getCols()-1) || (cell->m_col<getCols()-1) )
                cell=getCell(cell->m_row, getCols()-1);
            last = cell->frame( 0 );
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
    QPtrListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
    {
        QRect outerRect( viewMode->normalToView( frameIt.current()->outerRect() ) );
        //kdDebug(32004) << "KWTableFrameSet::createEmptyRegion outerRect=" << DEBUGRECT( outerRect )
        //          << " crect=" << DEBUGRECT( crect ) << endl;
        outerRect &= crect;
        if ( !outerRect.isEmpty() )
            emptyRegion = emptyRegion.subtract( outerRect );
    }
}

void KWTableFrameSet::drawBorders( QPainter& painter, const QRect &crect, KWViewMode *viewMode, KWCanvas *canvas ) {

    /*  Draw the borders on top of the lines stores in the m_rowPositions and m_colPositions arrays.
     *  check the relevant cells for borders and thus line thickness.
     *  We move the outer lines (on row==0 and col==0 plus on col=getCols() etc) a bit so they will stay
     *  inside the boundry of the table!
     */
    painter.save();
    QPen previewLinePen( lightGray ); // TODO use qcolorgroup
    QPen minsizeLinePen( red ); // TODO use qcolorgroup
    QColor defaultBorderColor = KoTextFormat::defaultTextColor( &painter );
    const int minborder = 1;
    bool drawPreviewLines = !(painter.device()->devType() == QInternal::Printer ||
         !canvas || !canvas->gui()->getView()->viewFrameBorders());

    // *** draw horizontal lines *** //
    unsigned int row=0;
    QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
    for (unsigned int i=0 ; i < m_rowPositions.count() ; i++) {
        bool bottom=false;
        if(pageBound!=m_pageBoundaries.end() && (*pageBound) == row || i == m_rowPositions.count()-1)
            bottom=true;  // at end of page or end of table draw bottom border of cell.

        const KoBorder *border=0;
        double startPos =0;
        for(unsigned int col=0; col <= getCols();) {
            Cell *cell = getCell(bottom?row-1:row, col);
            //if(cell) kdDebug(32004) << "cell (" << cell->m_row << "," << cell->m_col << ")" << endl;
            //else kdDebug(32004) << "cell: " << cell << endl;
            if(cell && cell->m_row != (bottom?row-1:row))
                cell=0;

            if(startPos!=0 && (!cell || col == getCols() || (
                    bottom && cell->frame(0)->bottomBorder()!=*border ||
                    !bottom && cell->frame(0)->topBorder()!=*border
                    ))) {
                if(border->width() > 0 || drawPreviewLines) {
                    double y = m_rowPositions[i];
                    if(row==0)
                        y+=border->width() / 2; // move slightly down.
                    else if (row == getRows())
                        y-=border->width() / 2; // move slightly up.
                    int ypix = m_doc->zoomItY(y);
                    double offset=0.0;
                    if(border->width() > 0 && col!=getCols()) { // offset border when not at right most cell.
                        if(cell) offset=cell->leftBorder();
                        Cell *c = getCell(row-1, col);
                        if(c) offset=QMAX(offset, c->leftBorder());
                    }
                    double x = m_colPositions[col] + offset;
                    QPoint topLeft = viewMode->normalToView(QPoint(m_doc->zoomItX(startPos), ypix));
                    QPoint bottomRight = viewMode->normalToView(QPoint(m_doc->zoomItX(x), ypix));
                    QRect line = QRect(topLeft, bottomRight);
                    if(crect.intersects( line )) {
                        //if(border->width() <= 0) kdDebug(32004) << "preview line" << endl;
                        if(border->width() <= 0)
                            painter.setPen( previewLinePen );
                        else {
                            int borderWidth = KoBorder::zoomWidthY( border->width(), m_doc, minborder );
                            painter.setPen( KoBorder::borderPen( *border, borderWidth, defaultBorderColor ) );
                        }
                        //kdDebug(32004) << "Paint: painter.drawHorizontalLine(" << line.left() << ","  << line.top() << "," <<  line.right() << ","  << line.bottom() << ")\n";
                        painter.drawLine( line.left(), line.top(), line.right(), line.bottom());
                    }
                }
                // reset startPos
                startPos = 0;
            }
            if(cell && startPos==0) {
                if(bottom)
                    border=&(cell->frame(0)->bottomBorder());
                else
                    border=&(cell->frame(0)->topBorder());

                if(col==0) // left most cell
                    startPos = m_colPositions[col];
                else {
                    double offset=0.0;
                    if(border->width() > 0) { // move line to the left a bit to compensate for the left border
                        if(cell) offset=cell->leftBorder();
                        Cell *c = getCell(row-1, col);
                        if(c) offset=QMAX(offset, c->leftBorder());
                    }
                    startPos = m_colPositions[col] - offset;
                }
            }
            col+=cell?cell->m_cols:1;
        }
        if(pageBound!=m_pageBoundaries.end() && (*pageBound) == row)
            pageBound++;
        else
            row++;
    }

    // *** draw vertical lines *** //
    for (unsigned int col=0 ; col < m_colPositions.count(); col++) {
        //kdDebug(32004) << "col: " << col << endl;
        bool right=false; // draw right border of cell.
        if(col == m_colPositions.count()-1)
            right=true;

        const KoBorder *border=0;
        int startRow =-1;
        for(unsigned int row=0; row <= getRows();) {
            //kdDebug(32004) << "row: " << row << endl;
            Cell *cell = getCell(row, right?col-1:col);
            if(cell && cell->m_col != (right?col-1:col))
                cell=0;

            if(startRow!=-1 && (!cell || cell->frame(0)->leftBorder()!=*border || row == getRows())) {
                if(border->width() > 0 || drawPreviewLines) {
                    double x = m_colPositions[col];
                    if(col==0) {
                        x+=border->width() / 2;
                    } else if(col==getCols()) {
                        x-=border->width() / 2;
                    }
                    int xpix = m_doc->zoomItX(x);
                    QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
                    unsigned int topRow=startRow;
                    do { // draw minimum of one line per page.
                        while(pageBound!=m_pageBoundaries.end() && *(pageBound) < topRow )
                            pageBound++;

                        unsigned int bottomRow;
                        if(pageBound==m_pageBoundaries.end()) bottomRow=m_rowPositions.count()-1;
                        else bottomRow=*(pageBound++);

                        //kdDebug(32004) << "from: " << topRow << " to: " << QMIN(row, bottomRow) << endl;
                        //kdDebug(32004) << "from: " << m_rowPositions[topRow] << " to: " << m_rowPositions[QMIN(row, bottomRow)] << endl;
                        double offset=0.0;
                        if(border->width() > 0) {
                            Cell *c=getCell(topRow,col);
                            if(c) offset=c->topBorder();
                            c=getCell(topRow,col-1);
                            if(c) offset=QMAX(offset,c->topBorder());
                            if(topRow==0) offset=0.0;
                        }
                        double top=m_rowPositions[topRow]-offset;

                        unsigned int toRow=QMIN(row,bottomRow);
                        offset=0.0;
                        if(border->width() > 0 && toRow!=bottomRow) {
                            if(cell) offset=cell->topBorder();
                            Cell *c=getCell(toRow,col-1);
                            if(c) offset=QMAX(offset,c->topBorder());
                        }
                        double bottom=m_rowPositions[toRow] + offset;

                        QPoint topLeft = viewMode->normalToView(QPoint(xpix, m_doc->zoomItY(top)));
                        QPoint bottomRight = viewMode->normalToView(QPoint(xpix, m_doc->zoomItY(bottom)));
                        QRect line = QRect(topLeft, bottomRight);
                        if(crect.intersects( line )) {
                            //if(border->width() <= 0) kdDebug(32004) << "preview line" << endl;
                            if(border->width() <= 0)
                                painter.setPen( previewLinePen );
                            else {
                                int borderWidth = KoBorder::zoomWidthX( border->width(), m_doc, minborder );
                                painter.setPen(KoBorder::borderPen( *border, borderWidth, defaultBorderColor ));
                            }
                            //kdDebug(32004) << "Paint: painter.drawVerticalLine(" << line.left() << ","  << line.top() << "," <<  line.right() << ","  << line.bottom() << ")\n";
                            painter.drawLine( line.left(), line.top(), line.right(), line.bottom());
                        }

                        topRow=bottomRow+1;
                    } while(topRow < row && topRow != m_rowPositions.count());
                }
                // reset startRow
                startRow = -1;
            }
            if(cell && startRow==-1) {
                startRow = row;
                if(right)
                    border=&(cell->frame(0)->rightBorder());
                else
                    border=&(cell->frame(0)->leftBorder());
            }
            row+=cell?cell->m_rows:1;
        }
    }

    if(drawPreviewLines) {
        painter.setPen( minsizeLinePen );
        for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
            Cell *cell = m_cells.at( i );
            double y = cell->frame(0)->top() + cell->frame(0)->minFrameHeight() + 1.5;
            if(y >= cell->frame(0)->bottom()) continue;
            int ypix=m_doc->zoomItY(y);
            QPoint topLeft = viewMode->normalToView(QPoint(m_doc->zoomItX(cell->frame(0)->left()), ypix));
            QPoint bottomRight = viewMode->normalToView(QPoint(m_doc->zoomItX(cell->frame(0)->right()), ypix));
            QRect line = QRect(topLeft, bottomRight);
            if(crect.intersects( line )) {
                painter.drawLine( line.left(), line.top(), line.right(), line.bottom());
            }
        }
    }

    painter.restore();
}

void KWTableFrameSet::drawContents( QPainter * painter, const QRect & crect,
                                    QColorGroup & cg, bool onlyChanged, bool resetChanged,
                                    KWFrameSetEdit * edit, KWViewMode * viewMode, KWCanvas *canvas )
{
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
    drawBorders( *painter, crect, viewMode, canvas );
}

void KWTableFrameSet::zoom( bool forPrint ) {
    for (unsigned int i =0; i < m_cells.count(); i++) {
        m_cells.at(i)->zoom( forPrint );
    }
}

#if 0
void KWTableFrameSet::preparePrinting( QPainter *painter, QProgressDialog *progress, int &processedParags ) {
    for (unsigned int i =0; i < m_cells.count(); i++) {
        m_cells.at(i)->preparePrinting( painter, progress, processedParags );
    }
}
#endif

QDomElement KWTableFrameSet::save( QDomElement &parentElem, bool saveFrames ) {
    // When saving to a file, we don't have anything specific to the frameset to save.
    // Save the cells only.
    for (unsigned int i =0; i < m_cells.count(); i++) {
        m_cells.at(i)->save(parentElem, saveFrames);
    }
    return QDomElement(); // No englobing element for tables...
}

QDomElement KWTableFrameSet::toXML( QDomElement &parentElem, bool saveFrames )
{
    QDomElement framesetElem = parentElem.ownerDocument().createElement( "FRAMESET" );
    parentElem.appendChild( framesetElem );
    KWFrameSet::saveCommon( framesetElem, false ); // Save the frameset attributes
    // Save the cells
    save( framesetElem, saveFrames );
    return framesetElem;
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
    if(_row <0) _row =0;
    unsigned int row=_row;
    int _col = KWDocument::getAttribute( framesetElem, "col", 0 );
    if(_col <0) _col =0;
    int _rows = KWDocument::getAttribute( framesetElem, "rows", 1 );
    if(_rows <0) _rows =0;
    int _cols = KWDocument::getAttribute( framesetElem, "cols", 1 );
    if(_cols <0) _cols =0;

    while(m_rowPositions.count() <= static_cast<unsigned int>(row + _rows + m_pageBoundaries.count())) {
        QValueList<double>::iterator tmp = m_rowPositions.end();
        m_rowPositions.insert(tmp,0);
    }
    while(m_colPositions.count() <= static_cast<unsigned int>(_col + _cols)) {
        QValueList<double>::iterator tmp = m_colPositions.end();
        m_colPositions.insert(tmp,0);
    }

    Cell *cell = new Cell( this, row, _col, QString::null /*unused*/ );
    QString autoName = cell->getName();
    //kdDebug(32004) << "KWTableFrameSet::loadCell autoName=" << autoName << endl;
    cell->load( framesetElem, loadFrames );
    cell->m_rows = _rows;
    cell->m_cols = _cols;

    if(m_pageBoundaries.count() > 0) {
        unsigned int adjustment=0;
        QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
        while(pageBound != m_pageBoundaries.end() && (*pageBound) <= row + adjustment) {
            adjustment++;
            pageBound++;
        }
        row+=adjustment;
    }

    //kdDebug(32004) << "loading cell (" << row << "," << _col << ")\n";
    if(cell->frame(0)) {
        cell->frame(0)->setMinFrameHeight(cell->frame(0)->height()); // TODO run the formatter over the text here
        QValueList<double>::iterator tmp = m_colPositions.at(_col);
        if(*tmp == 0) (*tmp) = cell->frame(0)->left();
        else (*tmp) = (cell->frame(0)->left() + *tmp) / 2;

        tmp = m_colPositions.at(_col+_cols);
        if(*tmp == 0) (*tmp) = cell->frame(0)->right();
        else (*tmp) = (cell->frame(0)->right() + *tmp) / 2;

        tmp = m_rowPositions.at(row);
        if(*tmp == 0) (*tmp) = cell->frame(0)->top();
        else {
            if (static_cast<int>(*tmp/m_doc->ptPaperHeight()) < static_cast<int>(cell->frame(0)->top()/m_doc->ptPaperHeight())) {
                //kdDebug(32004) << "This cell is on a new page" << endl;
                QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
                while(pageBound != m_pageBoundaries.end() && (*pageBound) < row) ++pageBound;
                if(*pageBound!=row) {
                    m_pageBoundaries.insert(pageBound,row++);
                    ++tmp;
                    m_rowPositions.insert(tmp,cell->frame(0)->top());
                }
            } else
                (*tmp) = (cell->frame(0)->top() + *tmp) / 2;
        }

        tmp = m_rowPositions.at(row+_rows);
        if(*tmp == 0) (*tmp) = cell->frame(0)->bottom();
        else { // untested...
            if (static_cast<int>(*tmp/m_doc->ptPaperHeight()) > static_cast<int>(cell->frame(0)->top()/m_doc->ptPaperHeight())) {
                //kdDebug(32004) << "next cell is on a new page" << endl;
                QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
                while(pageBound != m_pageBoundaries.end() && (*pageBound) < row) ++pageBound;
                if(*pageBound!=row) {
                    m_pageBoundaries.insert(pageBound,row++);
                    m_rowPositions.insert(tmp,cell->frame(0)->bottom());
                }
            } else
                (*tmp) = (cell->frame(0)->bottom() + *tmp) / 2;
        }
    }
    if ( !useNames )
        cell->setName( autoName );

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

int KWTableFrameSet::paragraphsSelected()
{
    int paragraphs = 0;
    for (unsigned int i =0; i < m_cells.count(); i++)
        paragraphs += m_cells.at(i)->paragraphsSelected();
    return paragraphs;
}

bool KWTableFrameSet::statistics( QProgressDialog *progress, ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words,
    ulong & sentences, ulong & syllables, bool selected )
{
    for (unsigned int i =0; i < m_cells.count(); i++) {
        if( ! m_cells.at(i)->statistics( progress, charsWithSpace, charsWithoutSpace, words, sentences, syllables, selected ) )
        {
            return false;
        }
    }
    return true;
}

void KWTableFrameSet::finalize( ) {
    //kdDebug(32004) << "KWTableFrameSet::finalize" << endl;

    for ( unsigned int i = 0; i < m_cells.count(); i++ )
        position(m_cells.at( i ));

    recalcCols();
    recalcRows();
    KWFrameSet::finalize();
}

bool KWTableFrameSet::canRemovePage( int num ) {
    /*  This one is a lot simpler then the one it overrides, we simply don't have
        to check if the frame contains something, the simple existence of a frame
        is enough
    */
    QPtrListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt ) {
        if ( frameIt.current()->pageNum() == num ) {
            return false;
        }
    }
    return true;
}

void KWTableFrameSet::addTextFrameSets( QPtrList<KWTextFrameSet> & lst )
{
    for (unsigned int i =0; i < m_cells.count(); i++) {
        if (!m_cells.at(i)->textObject()->protectContent())
            lst.append(m_cells.at(i));
    }
}

void KWTableFrameSet::setLeftBorder(KoBorder newBorder) {
    QPtrListIterator<Cell> it( m_cells );
    Cell *cell;
    while ( (cell = it.current()) != 0 ) {
        ++it;
        if (cell->frame( 0 )->isSelected()) {
            Cell *cellLeft= getCell(cell->m_row, cell->m_col-1);
            if(!(cellLeft && cellLeft->frame(0)->isSelected())) {
                cell->setLeftBorder(newBorder);
            }
        }
    }
}

void KWTableFrameSet::setTopBorder(KoBorder newBorder) {
    QPtrListIterator<Cell> it( m_cells );
    Cell *cell;
    while ( (cell = it.current()) != 0 ) {
        ++it;
        if (cell->frame( 0 )->isSelected()) {
            Cell *otherCell= getCell(cell->m_row-1, cell->m_col);
            if(!(otherCell && otherCell->frame(0)->isSelected())) {
                cell->setTopBorder(newBorder);
            }
        }
    }
    recalcRows();
}

void KWTableFrameSet::setBottomBorder(KoBorder newBorder) {
    QPtrListIterator<Cell> it( m_cells );
    Cell *cell;
    while ( (cell = it.current()) != 0 ) {
        ++it;
        if (cell->frame( 0 )->isSelected()) {
            Cell *otherCell= getCell(cell->m_row+1, cell->m_col);
            if(!(otherCell && otherCell->frame(0)->isSelected())) {
                cell->setBottomBorder(newBorder);
            }
        }
    }
    recalcRows();
}

void KWTableFrameSet::setRightBorder(KoBorder newBorder) {
    QPtrListIterator<Cell> it( m_cells );
    Cell *cell;
    while ( (cell = it.current()) != 0 ) {
        ++it;
        if (cell->frame( 0 )->isSelected()) {
            Cell *otherCell= getCell(cell->m_row, cell->m_col+1);
            if(!(otherCell && otherCell->frame(0)->isSelected())) {
                cell->setRightBorder(newBorder);
            }
        }
    }
}

KCommand *KWTableFrameSet::setProtectContent ( bool _protect )
{
    QPtrListIterator<Cell> it( m_cells );
    Cell *cell;
    bool createMacro = false;
    KMacroCommand *macro = new KMacroCommand( i18n("Protect Content"));
    while ( (cell = it.current()) != 0 ) {
        ++it;
        if (cell->frame( 0 )->isSelected()) {
            if ( cell->protectContent() != _protect )
            {
                KWProtectContentCommand *cmd = new KWProtectContentCommand( i18n("Protect Content"), cell , _protect);
                cell->setProtectContent( _protect );
                macro->addCommand( cmd );
                createMacro = true;
            }
        }
    }
    if ( createMacro )
        return macro;
    else
        delete macro;
    return 0L;
}

KWTextFrameSet* KWTableFrameSet::nextTextObject( KWFrameSet *obj )
{
    int pos = -1;
    KWTableFrameSet::Cell *tmp = dynamic_cast<KWTableFrameSet::Cell *>(obj);
    if ( tmp )
        pos=m_cells.findNextRef(tmp);
    if(pos !=-1)
    {
        KWFrameSet *frm=0L;
        for ( frm=m_cells.at(pos); frm != 0; frm=m_cells.next() ){
            KWTextFrameSet *newFrm = frm->nextTextObject( obj );
            if(newFrm && newFrm->textObject()->needSpellCheck())
                return newFrm;
       }
    }
    else
    {
        KWFrameSet *frm=0L;
        for ( frm=m_cells.first(); frm != 0; frm=m_cells.next() ){
            KWTextFrameSet *newFrm = frm->nextTextObject( obj );
            if(newFrm && newFrm->textObject()->needSpellCheck())
                return newFrm;
        }
    }
    return 0L;
}

#ifndef NDEBUG
void KWTableFrameSet::printDebug( KWFrame * theFrame )
{
    KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell *>( theFrame->frameSet() );
    Q_ASSERT( cell );
    if ( cell ) {
        kdDebug(32004) << " |  |- row :" << cell->m_row << endl;
        kdDebug(32004) << " |  |- col :" << cell->m_col << endl;
        kdDebug(32004) << " |  |- rows:" << cell->m_rows << endl;
        kdDebug(32004) << " |  +- cols:" << cell->m_cols << endl;
    }
}

void KWTableFrameSet::printDebug() {
    kdDebug(32004) << " |  Table size (" << m_rows << "x" << getCols() << ")" << endl;
    kdDebug(32004) << " |  col  " << 0 << ": " << m_colPositions[0] << endl;
    for(unsigned int i=1;i<m_colPositions.count();
        kdDebug(32004) << " |    |  " << i++ << ": " << m_colPositions[i] << endl);
    kdDebug(32004) << " |  row  " << 0 << ": " << m_rowPositions[0] << endl;
    for(unsigned int i=1;i<m_rowPositions.count();
        kdDebug(32004) << " |    |  " << i++ << ": " << m_rowPositions[i] << endl);
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

double KWTableFrameSet::Cell::leftBorder() {
    double b = frame(0)->leftBorder().width();
    if(b==0.0)
        return 0.0;
    if(m_col==0) // left most cell
        return b;
    return (b / 2);
}

double KWTableFrameSet::Cell::rightBorder() {
    double b=frame(0)->rightBorder().width();
    if(b==0.0)
        return 0.0;
    if(m_col+m_cols==m_table->getCols()) // right most cell
        return b;
    return (b / 2);
}

double KWTableFrameSet::Cell::topBorder() {
    double b = frame(0)->topBorder().width();
    if(b==0.0)
        return 0.0;
    if(m_row==0) // top most cell
        return b;
    return (b / 2);
}

double KWTableFrameSet::Cell::bottomBorder() {
    double b = frame(0)->bottomBorder().width();
    if(b==0.0)
        return 0.0;
    if(m_row+m_rows==m_table->m_rows) // bottom most cell
        return b;
    return (b / 2);
}

void KWTableFrameSet::Cell::setLeftBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->leftBorder().width() - newBorder.width();
    f->setLeftBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && m_col!=0) {
        diff = diff / 2; // if not outer edge only use halve
        m_table->getCell(m_row, m_col-1)->setRightBorder(newBorder);
    }
    f->setLeft(f->left() - diff);
}

void KWTableFrameSet::Cell::setRightBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->rightBorder().width() - newBorder.width();
    f->setRightBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && m_col+m_cols!=m_table->getCols()) {
        diff = diff / 2; // if not outer edge only use halve
        m_table->getCell(m_row, m_col+1)->setLeftBorder(newBorder);
    }
    f->setRight(f->right() + diff);
}

void KWTableFrameSet::Cell::setTopBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->topBorder().width() - newBorder.width();
    f->setTopBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && m_row!=0) {
        diff = diff / 2; // if not outer edge only use halve
        m_table->getCell(m_row-1, m_col)->setBottomBorder(newBorder);
    }
    f->setTop(f->top() - diff);
}

void KWTableFrameSet::Cell::setBottomBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->bottomBorder().width() - newBorder.width();
    f->setBottomBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && m_row+m_rows!=m_table->m_rows) {
        diff = diff / 2; // if not outer edge only use halve
        m_table->getCell(m_row+1, m_col)->setTopBorder(newBorder);
    }
    f->setBottom(f->bottom() + diff);
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

void KWTableFrameSetEdit::setCurrentCell( KWFrameSet * fs, bool eraseSelection )
{
    if ( m_currentCell )
        m_currentCell->terminate(eraseSelection);
    delete m_currentCell;
    m_currentCell =  fs->createFrameSetEdit( m_canvas );
    m_currentFrame = fs->frame( 0 );
    //refresh koruler
    m_canvas->gui()->getView()->slotUpdateRuler();
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
        KoTextDocument * textdoc = textframeSet->textDocument();
        if(textdoc->hasSelection( KoTextDocument::Standard ))
            moveToOtherCell=false;
    }
    KWTableFrameSet::Cell *fs = 0L;

    if(moveToOtherCell)
    {
        switch( e->key() ) {
            case QKeyEvent::Key_Up:
            {
                if(!(static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor()->parag()->prev())
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    int row=cell->m_row-1;
                    int col=cell->m_col;
                    do {
                        if(row < 0) {
                            col--;
                            row=tableFrame->getRows()-1;
                        }
                        if(col < 0) {
                            col=tableFrame->getCols()-1;
                            row=tableFrame->getRows()-1;
                        }
                        fs=tableFrame->getCell(row,col);
                        if(fs && fs->m_row != static_cast<unsigned int>(row)) {
                            col=fs->m_col -1;
                            fs=0;
                        }
                    } while(fs==0);

                }
            }
            break;
            case QKeyEvent::Key_Down:
            {
                if(!(static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor()->parag()->next())
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    unsigned int row=cell->m_row+cell->m_rows;
                    unsigned int col=cell->m_col;
                    do {
                        if(row >= tableFrame->getRows()) {
                            row=0;
                            col++;
                        }
                        if(col >= tableFrame->getCols()) {
                            col=0;
                            row=0;
                        }
                        fs=tableFrame->getCell(row,col);
                        if(fs && fs->m_row != row) {
                            col+=fs->m_cols;
                            fs=0;
                        }
                    } while(fs==0);

                }
            }
            break;
            case QKeyEvent::Key_Left:
            {
                KoTextCursor *cur = (static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor();
                if(!cur->parag()->prev()&&cur->index()==0)
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    int row=cell->m_row;
                    int col=cell->m_col-cell->m_cols;
                    do {
                        if(col < 0) {
                            col= (int)tableFrame->getCols() -1;
                            row--;
                        }
                        if(row <0) {
                            col= (int)tableFrame->getCols() -1;
                            row= (int)tableFrame->getRows() -1;
                        }
                        fs=tableFrame->getCell(row,col);
                        if(fs && fs->m_row != row) {
                            col+=fs->m_cols;
                            fs=0;
                        }
                    } while(fs==0);
                }
            }
            break;
            case QKeyEvent::Key_Right:
            {
                KoTextCursor *cur = (static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor();
                if(!cur->parag()->next()&&cur->index()==cur->parag()->string()->length()-1)
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    int row=cell->m_row;
                    int col=cell->m_col+cell->m_cols;
                    do {
                        if(col >= tableFrame->getCols()) {
                            col= 0;
                            row++;
                        }
                        if(row >= tableFrame->getRows()) {
                            col= 0;
                            row= 0;
                        }
                        fs=tableFrame->getCell(row,col);
                        if(fs && fs->m_row != row) {
                            col+=fs->m_cols;
                            fs=0;
                        }
                    } while(fs==0);
                }
            }
            break;
        }
    }
    if ( fs )
        setCurrentCell( fs );
    else
    {
        if ( !textframeSet->textObject()->protectContent() )
            m_currentCell->keyPressEvent( e );
        else
            KMessageBox::information(0L, i18n("Read-only content cannot be changed. No modifications will be accepted"));
    }
}

void KWTableFrameSetEdit::keyReleaseEvent( QKeyEvent * e )
{
    if ( m_currentCell )
        m_currentCell->keyReleaseEvent( e );
}

void KWTableFrameSetEdit::dragMoveEvent( QDragMoveEvent * e, const QPoint &n, const KoPoint &d )
{
    kdDebug(32004)<<"m_currentCell :"<<m_currentCell<<endl;
    if ( m_currentCell )
    {
        KWFrameSet *fs = tableFrameSet()->getCellByPos( d.x(), d.y() );
        kdDebug(32004)<<"fs :"<<fs <<endl;
        if(fs && fs != m_currentCell->frameSet())
            setCurrentCell(fs, false);
        if(m_currentCell)
            m_currentCell->dragMoveEvent( e, n, d );
    }
    else
    {
        setCurrentCell( d );
        kdDebug(32004)<<"after m_currentCell :"<<m_currentCell<<endl;
        if(m_currentCell)
            m_currentCell->dragMoveEvent( e, n, d );
    }
}

void KWTableFrameSetEdit::showPopup( KWFrame *frame, KWView *view, const QPoint &point )
{
    if (m_currentCell)
    {
        KWTextFrameSetEdit *fse = dynamic_cast<KWTextFrameSetEdit *>(m_currentCell);
        Q_ASSERT( fse ); // For now: m_currentCell always KWTextFrameSetEdit
        if ( fse )
        {
            fse->showPopup( frame, view, point );
        }
    }

}

void KWTableFrameSet::showPopup( KWFrame *theFrame, KWFrameSetEdit *edit, KWView *view, const QPoint &point )
{
    KWTextFrameSetEdit * textedit = dynamic_cast<KWTextFrameSetEdit *>(edit);
    Q_ASSERT( textedit ); // is it correct that this is always set ?
    if (textedit)
        textedit->showPopup( theFrame, view, point );
    else
    {
        QPopupMenu * popup = view->popupMenu("text_popup");
        Q_ASSERT(popup);
        if (popup)
            popup->popup( point );
    }
}

#include "kwtableframeset.moc"
