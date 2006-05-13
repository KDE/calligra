/*
    Copyright (C) 2001, S.R.Haque (srhaque@iee.org).
    This file is part of the KDE project
    Copyright (C) 2005 Thomas Zander <zander@kde.org>

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
 * Boston, MA 02110-1301, USA.

DESCRIPTION

    This file implements KWord tables.
*/

// ### TODO : multi page tables

#include "KWTableFrameSet.h"
#include "KWDocument.h"
#include "KWAnchor.h"
#include "KWCanvas.h"
#include "KWCommand.h"
#include "KWViewMode.h"
#include "KWView.h"
#include "KWordFrameSetIface.h"
#include "KWordTableFrameSetIface.h"
#include "KWFrameList.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWOasisSaver.h"

#include <KoOasisContext.h>
#include <KoXmlWriter.h>
#include <KoDom.h>
#include <KoXmlNS.h>
#include <KoTextObject.h> // for customItemChar !
#include <KoTextParag.h>

#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>
#include <dcopobject.h>
#include <QApplication>
#include <q3popupmenu.h>
#include <QClipboard>
//Added by qt3to4:
#include <Q3MemArray>
#include <Q3PtrList>
#include <QKeyEvent>
#include <QEvent>
#include <QDragMoveEvent>
#include <Q3ValueList>
#include <QMouseEvent>


KWTableFrameSet::KWTableFrameSet( KWDocument *doc, const QString & name ) :
    KWFrameSet( doc )
{
    m_rows = m_cols = m_nr_cells = 0;
    m_name = QString::null;
    m_active = true;
    m_frames.setAutoDelete(false);
    if ( name.isEmpty() )
        m_name = doc->generateFramesetName( i18n( "Table %1" ) );
    else
        m_name = name;
}

KWTableFrameSet::~KWTableFrameSet()
{
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

void KWTableFrameSet::updateFrames( int flags )
{
    for(TableIter c(this); c; ++c)
        c.current()->updateFrames( flags );
    if ( isFloating() )  {
        KWAnchor * anchor = findAnchor( 0 );
        if ( anchor )
             anchor->resize();
    }

    KWFrameSet::updateFrames( flags );
}

void KWTableFrameSet::moveFloatingFrame( int /*frameNum TODO */, const KoPoint &position )
{
    // TODO multi-page case

    double dx = position.x() - m_colPositions[0];
    double dy = position.y() - m_rowPositions[0];

    int oldPageNumber = cell(0,0)->frame(0)->pageNumber();
    // TODO multi-page case

    moveBy( dx, dy );

    if ( dx || dy ) {
        updateFrames();
        cell(0,0)->frame(0)->frameStack()->updateAfterMove( oldPageNumber );
    }
}

KoSize KWTableFrameSet::floatingFrameSize( int /*frameNum*/ )
{
    return boundingRect().size();
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
    //kDebug(32004) << "KWTableFrameSet::createAnchor" << endl;
    return new KWAnchor( txt, this, frameNum );
}

void KWTableFrameSet::createAnchors( KoTextParag * parag, int index, bool placeHolderExists /*= false */ /*only used when loading*/,
                                     bool repaint )
{
    //kDebug(32004) << "KWTableFrameSet::createAnchors" << endl;
    // TODO make one rect per page, and create one anchor per page
    // Anchor this frame, after the previous one
    KWAnchor * anchor = createAnchor( m_anchorTextFs->textDocument(), 0 );
    if ( !placeHolderExists )
        parag->insert( index, KoTextObject::customItemChar() );
    parag->setCustomItem( index, anchor, 0 );
    kDebug(32004) << "KWTableFrameSet::createAnchors setting anchor" << endl;
    parag->setChanged( true );
    if ( repaint )
        emit repaintChanged( m_anchorTextFs );
}

void KWTableFrameSet::deleteAnchors()
{
    KWAnchor * anchor = findAnchor( 0 );
    kDebug(32004) << "KWTableFrameSet::deleteAnchors anchor=" << anchor << endl;
    deleteAnchor( anchor );
}


void KWTableFrameSet::addCell( Cell* daCell ) // called add but also used to 'update'
{
    m_rows = qMax( daCell->rowAfter(), m_rows );
    m_cols = qMax( daCell->columnAfter(), m_cols );

    if ( m_rowArray.size() < daCell->rowAfter() )
        m_rowArray.resize( daCell->rowAfter() );
    for ( uint row = daCell->firstRow() ;row < daCell->rowAfter(); ++row )
    {
        if ( !m_rowArray[ row ] )
            m_rowArray.insert( row, new Row );
        m_rowArray[ row ]->addCell( daCell );
    }
}

void KWTableFrameSet::removeCell( Cell* daCell )
{
    for ( uint row = daCell->firstRow() ; row < daCell->rowAfter(); ++row )
        m_rowArray[ row ]->removeCell( daCell );
}

void KWTableFrameSet::insertRowVector(uint index, Row *r)
{
    if(m_rowArray.size() < m_rowArray.count() + 1)
        m_rowArray.resize(m_rowArray.count() + 1);

    for(uint i = m_rowArray.count(); i > index; i--)
        m_rowArray.insert(i, m_rowArray[i-1]);

    m_rowArray.insert(index, r);
}

/*
 * Inserts a new (empty) element into each row vector.
 * Elements in a row vector after index are moved back.
 */
void KWTableFrameSet::insertEmptyColumn(uint index)
{
    for(uint i = 0; i < m_rowArray.count(); ++i) {
       Row *r = m_rowArray[i];
       if(r->m_cellArray.size() < m_cols + 1)
           r->m_cellArray.resize(m_cols + 1);
       for(int j = m_cols - 1; j >= (int)index; --j)
           r->m_cellArray.insert(j + 1, r->m_cellArray[j]);
       r->m_cellArray.insert(index,  0);
    }
}

KWTableFrameSet::Row*
KWTableFrameSet::removeRowVector(uint index)
{
    Q_ASSERT(index < m_rowArray.count() );
    Row *ret = m_rowArray.at(index);
    Row *r;
    for(uint i = index; i < m_rowArray.size() - 1; ++i){
        r = m_rowArray.at(i+1);
        m_rowArray.remove(i+1);
        m_rowArray.insert(i,r);
    }
    return ret;
}


KoRect KWTableFrameSet::boundingRect() {
    KoRect outerRect(m_colPositions[0],    // left
                     m_rowPositions[0],      // top
                     m_colPositions.last()-m_colPositions[0], // width
                     m_rowPositions.last()-m_rowPositions[0]);// height

    // Add 1 'pixel' (at current zoom level) like KWFrame::outerKoRect(),
    // to avoid that the bottom line disappears due to rounding problems
    // (i.e. that it doesn't fit in the calculated paragraph height in pixels,
    // which depends on the paragraph's Y position)
    // Better have one pixel too much (caret looks too big by one pixel)
    // than one missing (bottom line of cell not painted)
    outerRect.rRight() += m_doc->zoomItX( 1 ) / m_doc->zoomedResolutionX();
    outerRect.rBottom() += m_doc->zoomItY( 1 ) / m_doc->zoomedResolutionY();

    return outerRect;

}

double KWTableFrameSet::topWithoutBorder()
{
    double top = 0.0;
    for (uint i = 0; i < getColumns(); i++)
    {
        KWTableFrameSet::Cell *daCell = cell( 0, i );
        top = qMax( top, m_rowPositions[0] + daCell->topBorder() );
    }
    return top;
}


double KWTableFrameSet::leftWithoutBorder()
{
    double left = 0.0;
    for (uint i=0; i < getRows(); i++)
    {
        KWTableFrameSet::Cell *daCell = cell( i, 0 );
        left = qMax( left, m_colPositions[0] + daCell->leftBorder() );
    }
    return left;
}

/* returns the cell that occupies row, col. */
KWTableFrameSet::Cell *KWTableFrameSet::cell( unsigned int row, unsigned int col ) const
{
    if ( row < m_rowArray.size() && col < m_rowArray[row]->size() ) {
        Cell* cell = (*m_rowArray[row])[col];
        if ( cell )
            return cell;
    }
//    kWarning() << name() << " cell " << row << "," << col << " => returning 0!" << kBacktrace( 3 ) << endl;
//#ifndef NDEBUG
//    validate();
//    printArrayDebug();
//#endif
    return 0L;
}

KWTableFrameSet::Cell *KWTableFrameSet::cellByPos( double x, double y ) const
{
    KWFrame *f = frameAtPos(x,y);
    if(f) return static_cast<KWTableFrameSet::Cell *> (f->frameSet());
    return 0L;
}

void KWTableFrameSet::recalcCols(unsigned int col,unsigned int row) {
    if(col >= getColumns())
        col = getColumns()-1;
    if(row >= getRows())
        row = getRows()-1;
    Cell *activeCell = cell(row,col);
    Q_ASSERT( activeCell );
    if ( !activeCell )
        return;
    double difference = 0;

    if(activeCell->frame(0)->left() - activeCell->leftBorder() != m_colPositions[activeCell->firstColumn()]) {
        // left border moved.
        col = activeCell->firstRow();
        difference = 0-(activeCell->frame(0)->left() - activeCell->leftBorder() - m_colPositions[activeCell->firstColumn()]);
    }

    if(activeCell->frame(0)->right() - activeCell->rightBorder() !=
            m_colPositions[activeCell->lastColumn()]) { // right border moved

        col = activeCell->columnAfter();
        double difference2 = activeCell->frame(0)->right() + activeCell->rightBorder() - m_colPositions[activeCell->columnAfter()];

        double moved=difference2+difference;
        if(moved > -0.01 && moved < 0.01) { // we were simply moved.
                col=0;
                difference = difference2;
            } else if(difference2!=0)
                difference = difference2;
    }

    m_redrawFromCol=getColumns(); // possible reposition col starting with this one, done in recalcRows
    if(difference!=0) {
        double last=col==0?0:m_colPositions[col-1];
        for(unsigned int i=col; i < m_colPositions.count(); i++) {
            double &colPos = m_colPositions[i];
            colPos = colPos + difference;
            if(colPos-last < s_minFrameWidth) { // Never make it smaller then allowed!
                difference += s_minFrameWidth - colPos;
                colPos = s_minFrameWidth + last;
            }
            last=colPos;
        }
        m_redrawFromCol=col;
        if(col>0) m_redrawFromCol--;
        //if(activeCell) activeCell->frame(0)->setMinimumFrameHeight(0);
    }
    updateFrames();
    //kDebug(32004) << "end KWTableFrameSet::recalcCols" << endl;
}


// Step through the whole table and recalculate the position and size
// of each cell.

void KWTableFrameSet::recalcRows(unsigned int col, unsigned int row) {
    kDebug(32004) << name() << " KWTableFrameSet::recalcRows ("<< col <<"," << row << ")" << endl;
    //for(unsigned int i=0; i < m_rowPositions.count() ; i++) kDebug(32004) << "row: " << i << " = " << m_rowPositions[i] << endl;

    Cell *activeCell = cell(row,col);
    Q_ASSERT( activeCell );
    if ( !activeCell ) // #122807
        return;
    double difference = 0;

    if(activeCell->frame(0)->height() != activeCell->frame(0)->minimumFrameHeight() &&
          activeCell->type() == FT_TEXT) {
        // when the amount of text changed and the frame has to be rescaled we are also called.
        // lets check the minimum size for all the cells in this row.

        // Take a square of table cells which depend on each others height. It is always the full
        // width of the table and the height is determined by joined cells, the minimum is one row.
        double minHeightOtherCols=0;    // The minimum height which the whole square of table cells can take
        double minHeightActiveRow=0;    // The minimum height our cell can get because of cells in his row
        double minHeightMyCol=0;        // The minimum height our column can get in the whole square
        unsigned int rowSpan = activeCell->rowSpan();
        unsigned int startRow = activeCell->firstRow();
        for (uint colCount = 0; colCount < getColumns(); ++colCount )
        {
            // for each column
            unsigned int rowCount=startRow;
            double thisColHeight=0;     // the total height of this column
            double thisColActiveRow=0;  // the total height of all cells in this col, completely in the
                                        // row of the activeCell
            do { // for each row (under startRow)
                Cell *thisCell=cell(rowCount,colCount);
                if ( !thisCell )
                    break; // ###
                if(thisCell->firstRow() < startRow) { // above -> set startRow and restart
                    rowSpan += startRow - thisCell->firstRow();
                    startRow = thisCell->firstRow();
                    break;
                }
                if(thisCell->rowAfter() > startRow + rowSpan) {
                    rowSpan = thisCell->rowAfter() - startRow;
                    break;
                }

                thisColHeight+=thisCell->frame(0)->minimumFrameHeight();
                thisColHeight+=thisCell->topBorder();
                thisColHeight+=thisCell->bottomBorder();

                if(thisCell->firstRow() >= activeCell->firstRow() && thisCell->rowAfter() <= activeCell->rowAfter())
                    thisColActiveRow+=thisCell->frame(0)->minimumFrameHeight();

                rowCount += thisCell->rowSpan();
            } while (rowCount < rowSpan+startRow);

            if(colCount >= activeCell->firstColumn() &&
                  colCount < activeCell->columnAfter() )
                minHeightMyCol = thisColHeight;
            else {
                minHeightOtherCols = qMax(minHeightOtherCols, thisColHeight);
                minHeightActiveRow = qMax(minHeightActiveRow, thisColActiveRow);
            }
        } // for each column

        bool bottomRow = (startRow+rowSpan == activeCell->rowAfter());
        if(!bottomRow) {
            Cell *bottomCell=cell(startRow+rowSpan-1, activeCell->firstColumn());
            bottomCell->frame(0)->setHeight(bottomCell->frame(0)->minimumFrameHeight() +
                    minHeightOtherCols - minHeightMyCol);
            // ### RECURSE ###
            recalcRows(bottomCell->firstColumn(), bottomCell->firstRow());
        }
        if(activeCell->frame(0)->minimumFrameHeight() > activeCell->frame(0)->height()) { // wants to grow
            activeCell->frame(0)->setHeight(activeCell->frame(0)->minimumFrameHeight());
            //kDebug(32004) << activeCell->name() << " grew to its minheight: " << activeCell->frame(0)->minimumFrameHeight() << endl;
        } else { // wants to shrink
            double newHeight=qMax(activeCell->frame(0)->minimumFrameHeight(),minHeightActiveRow);
            if(bottomRow) // I'm a strech cell
                newHeight=qMax(newHeight, minHeightOtherCols - (minHeightMyCol - activeCell->frame(0)->minimumFrameHeight()));
            activeCell->frame(0)->setHeight(newHeight);
            //kDebug(32004) << activeCell->name() << " shrunk to: " << newHeight << endl;
        }
    }

    if(activeCell->frame(0)->top() - activeCell->topBorder() != getPositionOfRow(activeCell->firstRow())) {
        // top moved.
        row = activeCell->firstRow();
        difference = 0 - (activeCell->frame(0)->top() - activeCell->topBorder() - getPositionOfRow(row));
    }


    if(activeCell->frame(0)->bottom() + activeCell->bottomBorder() !=
            getPositionOfRow(activeCell->rowAfter())) { // bottom moved

        row = activeCell->rowAfter();
        double difference2 = activeCell->frame(0)->bottom() + activeCell->bottomBorder() - getPositionOfRow(row);
        double moved=difference2+difference;
        if(moved > -0.01 && moved < 0.01) { // we were simply moved.
            row=0;
            difference = difference2;
        } else if( difference2!=0)
            difference = difference2;
    }

    unsigned int fromRow = m_rows; // possible reposition rows starting with this one, default to no repositioning
    unsigned int untilRow=0;     // possible reposition rows ending with this one
    if( QABS( difference ) > 1E-10 ) { // means "difference != 0.0"
        Q3ValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
        Q3ValueList<double>::iterator j = m_rowPositions.begin();
        double last=0.0;
        int lineNumber=-1;
        while(j != m_rowPositions.end()) {
            lineNumber++;
            if(pageBound!=m_pageBoundaries.end()) {
                if((int)*pageBound == lineNumber) { // next page
                    if(lineNumber >= (int)row) { // then delete line j
                        Q3ValueList<double>::iterator nextJ = j;
                        ++nextJ;
                        difference -= *(nextJ)-*(j);
                        kDebug(32004) << "Deleting line with old pos: " << *j << endl;
                        j=m_rowPositions.remove(j);
                        j--;
                        Q3ValueList<unsigned int>::iterator tmp = pageBound;
                        ++pageBound;
                        m_pageBoundaries.remove(tmp);
                        j++;
                        continue;
                    }
                    ++pageBound;
                    lineNumber--;
                }
            }
            if(lineNumber >= (int)row)  { // below changed row
                if(*(j)-last < s_minFrameHeight) // Never make it smaller then allowed!
                    difference += s_minFrameHeight - *(j) + last;
                last=*(j);
                kDebug(32004) << "moving " << *(j) << " by " << difference << "; to " << (*j) + difference << endl;
                (*j) += difference; // move line.
            }
            j++;
        }
        fromRow=row;
        if(row>0) fromRow--;
    } else {
        row=0;
    }
#if 0
{ Q3ValueList<unsigned int>::iterator pb = m_pageBoundaries.begin();
  unsigned int i=0;
  double last=0;
  do {
      double cur=m_rowPositions[i];
      if(pb!=m_pageBoundaries.end() && *(pb)==i) {
        kDebug(32004) << "line: " << i << ": " << cur << " *" << (last>cur?" (ALERT)":"") << endl;
        ++pb;
      } else
        kDebug(32004) << "line: " << i << ": " << cur << (last>cur?" (ALERT)":"") << endl;
      last=cur;
      i++;
  } while( i<m_rowPositions.count());
}
#endif
#if 0 // def SUPPORT_MULTI_PAGE_TABLES

    //double pageHeight = m_doc->ptPaperHeight() - m_doc->ptBottomBorder() - m_doc->ptTopBorder();
    unsigned int pageNumber=cell(0,0)->frame(0)->pageNumber() +1;
    unsigned int lineNumber=1;
    Q3ValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
    Q3ValueList<double>::iterator j = m_rowPositions.begin();

    double diff=0.0;
    double pageBottom = pageNumber * m_doc->ptPaperHeight() - m_doc->ptBottomBorder();
    // kDebug(32004) << "pageBottom; " << pageBottom << endl;
    while(++j!=m_rowPositions.end()) { // stuff for multipage tables.
        if(pageBound!=m_pageBoundaries.end() && *pageBound == lineNumber ) {
            if(*j > pageNumber * m_doc->ptPaperHeight() - m_doc->ptBottomBorder() ) { // next page marker exists, and is accurate...
                pageNumber++;
                pageBottom = pageNumber * m_doc->ptPaperHeight() - m_doc->ptBottomBorder();
                // kDebug(32004) << "pageBottom; " << pageBottom << endl;
                untilRow=qMax(untilRow, *pageBound);
                pageBound++;
            }
        }

//kDebug() << "checking; " << lineNumber << ", " << (*j) << endl;
        if((*j) + diff > pageBottom) { // a row falls off the page.
//kDebug(32004) << "row falls off of page"<< endl;
            untilRow = m_rows;
            bool hugeRow = false;
            unsigned int breakRow = lineNumber-1;
            // find out of no cells are spanning multiple rows meaning we have to break higher.
#if 0
    // ### TODO: I did not get a chance do debug this yet!  TZ.
            for(int i=0; i < getColumns() ; i++) {
                kDebug() << "i: " << i<< endl;
                Cell *c= cell(breakRow, i);
                kDebug() << "c: " << c->firstRow() << "," << c->m_col << " w: " << c->columnSpan() << ", h: " << c->rowSpan() << endl;
                if(c->firstRow() < breakRow) {
                    breakRow = c->firstRow();
                    i=-1;
                }
            }
            kDebug() << "breakRow: " << breakRow<< endl;
            fromRow=qMin(fromRow, breakRow);
            if(breakRow < lineNumber+1) {
                for(unsigned int i=lineNumber+1; i > breakRow;i--)
                    kDebug() << "j--";
                for(unsigned int i=lineNumber+1; i > breakRow;i--)
                    --j;
                lineNumber=breakRow+1;
            }

            // find out if the next row (the new one on the page) does not contain cells higher then page.
            for(unsigned int i=0; i < getColumns() ; i++) {
                if(cell(breakRow+1,i) && cell(breakRow+1,i)->frame(0)->height() > pageHeight)
                    hugeRow=true;
            }
            //if((*pageBound) != breakRow) { // I think that this has to be that way
                // voeg top in in rowPositions
#endif

            double topOfPage = m_doc->ptPaperHeight() * pageNumber + m_doc->ptTopBorder();

            Q3ValueList<double>::iterator tmp = m_rowPositions.at(breakRow);
            diff += topOfPage - (*tmp); // diff between bottom of last row on page and top of new page
//kDebug() << "diff += " <<  topOfPage  << " - " << (*tmp) << ". diff += " << topOfPage - (*tmp) <<" ="<< diff  << endl;
            lineNumber++;
            m_rowPositions.insert(j, topOfPage);

            // insert new pageBound. It points to last LINE on previous page
            pageBound = m_pageBoundaries.insert(pageBound, breakRow);
            //kDebug(32004) << "inserting new pageBound: " << breakRow  << " at " << m_rowPositions[breakRow] << endl;
            pageBound++;
            if(!hugeRow) {
                // add header-rij toe. (en zet bool) TODO
                //j++;
                //lineNumber++;
                // m_hasTmpHeaders = true;
            }
            pageNumber++;
            pageBottom = pageNumber * m_doc->ptPaperHeight() - m_doc->ptBottomBorder();
            //kDebug(32004) << " pageBottom: " << pageBottom << " pageNumber=" << pageNumber << endl;
            if((int)pageNumber > m_doc->numPages()) {
                int num = m_doc->appendPage();
                kDebug(32004) << "Have appended page: " << num << " (one page mode!)" << endl;
                m_doc->afterInsertPage( num );
            }
        }
        //if(diff > 0)  kDebug(32004) << "   adding " << diff << ", line " << lineNumber << " " << *(j) <<" -> " << *(j)+diff << endl;
        if(diff > 0)
            (*j) = (*j) + diff;
        lineNumber++;

#if 0 // def SUPPORT_MULTI_PAGE_TABLES
        // Note: produces much ouput!
        int i = 1; // DEBUG
        for ( Q3ValueList<double>::iterator itDebug = m_rowPositions.begin(); itDebug != m_rowPositions.end(); ++itDebug, ++i )
        {
            kDebug(32004) << "m_rowPosition[" << i << "]= " << (*itDebug) << endl;
        }
#endif

    }
#endif
#if 0
{ Q3ValueList<unsigned int>::iterator pb = m_pageBoundaries.begin();
  unsigned int i=0;
  double last=0;
  do {
      double cur=m_rowPositions[i];
      if(pb!=m_pageBoundaries.end() && *(pb)==i) {
        kDebug(32004) << "line: " << i << ": " << cur << " *" << (last>cur?" (ALERT)":"") << endl;
        ++pb;
      } else
        kDebug(32004) << "line: " << i << ": " << cur << (last>cur?" (ALERT)":"") << endl;
      last=cur;
      i++;
  } while( i<m_rowPositions.count());
}
#endif
//for (unsigned int i=0; i < getRows(); kDebug(32004)<<" pos of row["<<i<<"] = "<<getPositionOfRow(i)<<"/"<<getPositionOfRow(i,true)<<endl,i++);
    //kDebug () << "Repositioning from row : " << fromRow << " until: " << untilRow << endl;
    //kDebug () << "Repositioning from col > " << redrawFromCol << endl;
    // do positioning.
    //Cell *cell;
    //bool setMinFrameSize= activeCell->frame(0)->isSelected();

#if 0 // def SUPPORT_MULTI_PAGE_TABLES
    for(TableIter cell(this); cell; ++cell) {
        if((cell->rowAfter() > fromRow && cell->firstRow() < untilRow) || cell->columnAfter() > m_redrawFromCol)
            position(cell, (cell==activeCell && cell->frame(0)->isSelected()));
    }
    m_redrawFromCol = getColumns();

    // check if any rowPosition entries are unused

    //   first create a hash of all row entries
    QMap<unsigned int,int> rows;        // rownr, count
    unsigned int top=m_rowPositions.count() - m_pageBoundaries.count()-1;
    for(unsigned int i=0; i < top; rows[i++]=0);

    //   fill hash with data
    for(TableIter i(this); i; ++i) {
        rows[i->firstRow()] += 1;
    }
    //   check if some entries have stayed unused.
    unsigned int counter=top;
    int adjustment=m_pageBoundaries.count()-1;

    do {
        counter--;
        if(adjustment >= 0 && counter == m_pageBoundaries[adjustment])
            adjustment--;
        if(rows[counter]==0) {
            kDebug() << k_funcinfo << "no rows at counter=" << counter << " -> erasing" << endl;
            m_rows--;
            m_rowPositions.erase(m_rowPositions.at(counter+(adjustment>0?adjustment:0)));
            for (TableIter cell(this); cell; ++cell) {
                if(cell->firstRow() < counter && cell->rowAfter() > counter)
                    cell->setRowSpan(cell->rowSpan()-1);
                if(cell->firstRow() > counter)
                    cell->setFirstRow(cell->firstRow()-1);
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
#endif


    m_redrawFromCol = 0;
    for (TableIter cell(this); cell; ++cell) {
        if((cell->rowAfter() > fromRow && cell->firstRow() < untilRow)
           || cell->columnAfter() > m_redrawFromCol)
            position(cell);
    }
    m_redrawFromCol = getColumns();
    kDebug(32004) << name() << " KWTableFrameSet::recalcRows done" << endl;
    updateFrames();
}

int KWTableFrameSet::columnEdgeAt( double x ) const
{
    // We compare x with the middle of columns (left+right/2),
    // to find which column x is closest to.
    // m_colPositions is sorted, so we can remember the last column we looked at.
    double lastMiddlePos = 0;
    for ( uint i = 0; i < m_colPositions.count() - 1; i++ ) {
        double middlePos = ( m_colPositions[i] + m_colPositions[i+1] ) / 2;
        Q_ASSERT( lastMiddlePos < middlePos );
        if ( x > lastMiddlePos && x <= middlePos )
            return i;
        lastMiddlePos = middlePos;
    }
    return m_colPositions.count() - 1;
}

int KWTableFrameSet::rowEdgeAt( double y ) const
{
    double lastMiddlePos = 0;
    for ( uint i = 0; i < m_rowPositions.count() - 1; i++ ) {
        double middlePos = ( m_rowPositions[i] + m_rowPositions[i+1] ) / 2;
        Q_ASSERT( lastMiddlePos < middlePos );
        if ( y > lastMiddlePos && y <= middlePos )
            return i;
        lastMiddlePos = middlePos;
    }
    return m_rowPositions.count() - 1;
}

double KWTableFrameSet::columnSize( unsigned int col )
{
  return m_colPositions[ col ];
}

double KWTableFrameSet::rowSize( unsigned int row )
{
  return m_rowPositions[ row ];
}

void KWTableFrameSet::resizeColumn( unsigned int col, double x )
{
    kDebug() << k_funcinfo << col << "," << x << endl;
    if ((col != 0) && (x - m_colPositions[ col-1 ] < s_minFrameWidth))
      m_colPositions[ col ] = m_colPositions[ col-1 ] + s_minFrameWidth;
    else
      if ((col != getColumns()) && (m_colPositions[ col + 1 ] - x < s_minFrameWidth))
        m_colPositions[col] = m_colPositions[ col + 1 ] - s_minFrameWidth;
      else
        m_colPositions[ col ] = x;

    // move all cells right of 'col'
    for (TableIter cell(this); cell; ++cell) {
        if ( cell->columnAfter() >= col ) {
            position(cell);
        }
    }
    recalcCols( col-1, 0 );
}

void KWTableFrameSet::resizeRow( unsigned int row, double y )
{
    kDebug() << k_funcinfo << row << "," << y << endl;
    double difference = m_rowPositions[row];
    if ((row != 0) && (y - m_rowPositions[ row-1 ] < s_minFrameHeight))
      m_rowPositions[ row ] = m_rowPositions[ row-1 ] + s_minFrameHeight;
    else
      if ((row != getRows()) && (m_rowPositions[ row + 1 ] - y < s_minFrameHeight))
        m_rowPositions[row] = m_rowPositions[ row + 1 ] - s_minFrameHeight;
      else
        m_rowPositions[ row ] = y;
    difference = m_rowPositions[row] - difference;

    //move all rows under 'row'
    if (row != 0)
       for (unsigned int i=row+1; i<= getRows(); i++)
           m_rowPositions[i] = m_rowPositions[i] + difference;

    // move all cells under 'row'
    for (TableIter cell(this); cell; ++cell) {
        if ( cell->rowAfter() >= row ) {
            position(cell);
        }
    }
    recalcRows( 0, row-1 );
}

void KWTableFrameSet::resizeWidth( double width ) {
    Q_ASSERT(width != 0);
    Q_ASSERT(boundingRect().width() != 0);
    kDebug() << "bounding width before resize " << boundingRect().width() << endl;
    double growth = width / boundingRect().width();

    // since we move all the columns, we also move the 1st one,
    // depending where it is on the page.
    // just compensate by substracting that offset.
    double moveOffset = m_colPositions[0] * growth - m_colPositions[0];

    for (uint i=0; i<m_colPositions.count(); i++) {
        m_colPositions[i] = m_colPositions[i] * growth - moveOffset;
    }
    finalize();
    kDebug() << "bounding width after resize" << boundingRect().width() << endl;
    Q_ASSERT(boundingRect().width() - width < 0.01);
}

void KWTableFrameSet::setBoundingRect( KoRect rect, CellSize widthMode, CellSize heightMode ) {
   // Column positions..
    m_colPositions.clear();
    unsigned int cols=0;
    for (TableIter c(this); c; ++c)
        cols = qMax(cols, c.current()->columnAfter());
    double colWidth = rect.width() / cols;
    if ( widthMode == TblAuto ) {
        KWPage *page = pageManager()->page(rect);
        rect.setLeft( page->leftMargin() );
        colWidth = (page->width() - page->leftMargin() - page->rightMargin()) / cols;
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
    rowHeight=qMax(rowHeight, 22.0); // m_doc->getDefaultParagLayout()->getFormat().ptFontSize()) // TODO use table style font-size

    for(unsigned int i=0; i <= m_rows;i++) {
        m_rowPositions.append(rect.y() + rowHeight * i);
    }

    double oneMm = MM_TO_POINT( 1.0 );
    for (TableIter cell(this); cell; ++cell) {
        KWFrame *frame = cell->frame(0);
        frame->setPaddingLeft( oneMm );
        frame->setPaddingRight( oneMm );
        frame->setPaddingTop( oneMm );
        frame->setPaddingBottom( oneMm );
        frame->setNewFrameBehavior( KWFrame::NoFollowup );
        position(cell, true);
    }
}

void KWTableFrameSet::position( Cell *theCell, bool setMinFrameHeight ) {
    if(!theCell->frame(0)) { // sanity check.
        kDebug(32004) << "errorous table cell!! row:" << theCell->firstRow()
            << ", col: " << theCell->firstColumn() << endl;
        return;
    }
    double x = *m_colPositions.at(theCell->firstColumn());
    double y = getPositionOfRow(theCell->firstRow());
    double width = (*m_colPositions.at(theCell->columnAfter())) - x;
    double height  = getPositionOfRow(theCell->lastRow(), true) - y;

#if 0
    if(theCell->m_col==0) {
        kDebug(32004) << "row "  << theCell->firstRow() << " has top: "
            << y << ", and bottom: " << y + height << endl;
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
    if( setMinFrameHeight )
        theFrame->setMinimumFrameHeight(height);

    if(!theCell->isVisible())
        theCell->setVisible(true);
}

double KWTableFrameSet::getPositionOfRow( unsigned int row, bool bottom ) {
    unsigned int adjustment=0;
    Q3ValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
    while(pageBound != m_pageBoundaries.end() && (*pageBound) <= row + adjustment) {
        adjustment++;
        pageBound++;
    }
    if(m_rowPositions.count() < row+adjustment+(bottom?1:0))  // Requested row does not exist.
        return 0;
    return m_rowPositions[row+adjustment+(bottom?1:0)];
}

void KWTableFrameSet::moveBy( double dx, double dy ) {
    bool redraw=false;
    kDebug(32004) << "KWTableFrameSet(" << name() << ")::moveBy(" << dx<<","<<dy<<")\n";
    //for(unsigned int i=0; i < m_rowPositions.count() ; kDebug(32004) << "row " << i << ": " << m_rowPositions[i++] << endl);
    if(!(dy > -0.001 && dy < 0.001)) {
        redraw=true;
        Q3ValueList<double>::iterator row = m_rowPositions.begin();
        while(row != m_rowPositions.end()) {
            (*row)= (*row)+dy;
            row++;
        }
    }
    if(!(dx > -0.001 && dx < 0.001)) {
        redraw=true;
        Q3ValueList<double>::iterator col = m_colPositions.begin();
        while(col != m_colPositions.end()) {
            (*col)= (*col)+dx;
            col++;
        }
    }

    if(redraw) {
        for(TableIter cell(this);cell;++cell)
            position(cell);
    }
}

/* Delete all cells that are completely in this row.              */
void KWTableFrameSet::deleteRow( unsigned int row, RemovedRow &rr, bool _recalc)
{
    Q_ASSERT(row < m_rowArray.size());
    const unsigned int rowspan=1;

    double height= getPositionOfRow(row+rowspan-1,true) - getPositionOfRow(row);
    Q3ValueList<double>::iterator tmp = m_rowPositions.at(row+rowspan);
    tmp=m_rowPositions.erase(tmp);
    while(tmp!=m_rowPositions.end()) {
        (*tmp)= (*tmp)-height;
        tmp++;
    }

    rr.m_index = row;
    rr.m_rowHeight = height;
    rr.m_row = m_rowArray[row];

    // move/delete cells.
    for ( TableIter cell(this); cell; ++cell ) {
        if ( row >= cell->firstRow() && row < cell->rowAfter()) { // cell is indeed in row
            if(cell->rowSpan() == 1) { // cell is wholly contained within row
                m_frames.remove( cell->frame(0) );
            } else { // make cell span rowspan less rows
                cell->setRowSpan(cell->rowSpan()-rowspan);
                position(cell);
            }

        } else if ( cell->firstRow() > row ) {
            // move cell up
            cell->setFirstRow( cell->firstRow() - rowspan );
            position(cell);
        }
    }

    removeRowVector(row);
    m_rows -= rowspan;
    m_rowArray.resize( m_rows );
    validate();

    if ( _recalc )
        recalcRows( 0, row-1 );
}

void KWTableFrameSet::reInsertRow(RemovedRow &rr)
{
   uint row = rr.index();
   Row *r =  rr.row();
   uint rlen = r->count();

   // adjust cell positions & sizes
   for(MarkedIterator cell(this); cell; ++cell) {

        if ( cell->firstRow() < row && cell->lastRow() >= row ){ // cell is indeed in row
                cell->setRowSpan(cell->rowSpan() + 1);
        }
        else if(r->m_cellArray[cell->firstColumn()] == cell.current()) {
                cell->setRowSpan(cell->rowSpan() + 1);
        }
        else if ( cell->firstRow() >= row ) {
            // move cell down
            cell->setFirstRow( cell->firstRow() + 1);
        }
   }

   // put back m_frames that were removed
   for(uint i = 0; i < rlen; i++){
       if( m_frames.findRef((*r)[i]->frame(0)) == -1 )
        m_frames.append( (*r)[i]->frame(0) );
   }

   // adjust row positions (ignores page boundaries!)a
   if(row == m_rows) {  //reinserting at bottom of table
       double d = m_rowPositions.last() + rr.height();
       m_rowPositions.append(d);
   }
   else {
       Q3ValueList<double>::iterator top = m_rowPositions.at(row);
       Q3ValueList<double>::iterator i = m_rowPositions.at(row+1);
       i = m_rowPositions.insert(i, *top + rr.height());
       i++;
       for(; i != m_rowPositions.end(); ++i) {
           *i = *i+ rr.height();
       }
   }

   // reinsert row into array
   m_rows++;
   insertRowVector(rr.index(), rr.takeRow());

   // don't actually have to visit all the cells, this could be optimised
   for(TableIter i(this); i ; ++i)
      position(i.current());

   validate();
}

void KWTableFrameSet::insertNewRow( uint idx, bool recalc, bool _removeable)
{

    (void) _removeable; // unused parameter
    unsigned int copyFromRow = idx==0?0:idx-1;
    if(idx==0)
        copyFromRow=1;
    Row *copyRow = m_rowArray[copyFromRow];

    uint new_rows = m_rows + 1;
    // What height to use for the new row
    double height = getPositionOfRow(copyFromRow,true) - getPositionOfRow(copyFromRow);

    // Calculate offset in QValueList because of page breaks.
    unsigned int adjustment=0;
    unsigned int untilRow=m_rows;
    Q3ValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
    while(pageBound != m_pageBoundaries.end() && (*pageBound) <= idx) {
        // Find out how many pages we already had.
        adjustment++;
        pageBound++;
    }

    // Move all rows down from newRow to bottom of page
    Q3ValueList<double>::iterator tmp = m_rowPositions.at(idx);
    double newPos = *tmp + height;
    tmp++;
    m_rowPositions.insert(tmp, newPos);
    for(unsigned int i=idx+adjustment+2; i < m_rowPositions.count(); i++) {
        double &rowPos = m_rowPositions[i];
        kDebug(32004) << "adjusting " << rowPos << " -> " << rowPos + height << endl;
        rowPos = rowPos + height;
        if(*pageBound == i) {
            untilRow= *pageBound;
            break;              // stop at pageBreak.
        }
    }

    for ( MarkedIterator cells(this); cells; ++cells) {
        if ( cells->firstRow() >= idx ) { // move all cells beneath the new row.
            cells->setFirstRow(cells->firstRow()+1);
        }
    }

    insertRowVector(idx, new Row);

    unsigned int i = 0;
    while(i < getColumns()) {

        if(idx != 0 && (idx != m_rows)) {
            Cell *c = cell(idx - 1, i);
            if( c == cell(idx + 1, i) ) {
                m_rowArray[idx]->addCell(c);
                c->setRowSpan(c->rowSpan() + 1);
                i += c->columnSpan();
                continue; // don't need to make a new cell
            }
        }

        KWFrame *theFrame = new KWFrame(copyRow->m_cellArray[i]->frame(0));
        Cell *newCell=new Cell( this, idx, i, QString::null );
        newCell->setColumnSpan( cell(copyFromRow,i)->columnSpan() );
        addCell(newCell);
        newCell->addFrame( theFrame, false );
        position(newCell);
        i += newCell->columnSpan();
   }

    // Position all changed cells.

    m_rows = new_rows;
    validate();
    if ( recalc )
        finalize();
}

void KWTableFrameSet::deleteColumn(uint col, RemovedColumn &rc)
{
    // keep these values in case we have to put it back
    if(!rc.m_initialized) {
        rc.m_index = col;
        rc.m_width = m_colPositions[col+1] - m_colPositions[col];
    }

    // move the colomn positions
    Q3ValueList<double>::iterator tmp = m_colPositions.at(col+1);
    tmp = m_colPositions.erase(tmp);
    while(tmp != m_colPositions.end()) {
        (*tmp) = (*tmp) - rc.m_width;
        tmp++;
    }

    // remove cells that are wholy in this column,
    // otherwise reduce rowspan. pointers to all cells
    // are kept in the RemovedColumn
    CheckedIter iter(this);
    for(uint i = 0; i < m_rows; ++i) {
        Cell *daCell = cell(i, col);

        if(!rc.m_initialized) {
            rc.m_column.append(daCell);
            rc.m_removed.append(daCell->columnSpan() == 1);
        }

        if(daCell->columnSpan() == 1) { // lets remove it
            if(daCell->firstRow() == i) {
                m_frames.remove( daCell->frame(0) );
                m_nr_cells--;
            }
            m_rowArray[i]->m_cellArray.insert(col, 0);
        }
        else {                    // make cell span 1 less column
            if(daCell->firstRow() == i) {
                daCell->setColumnSpan( daCell->columnSpan() - 1 );
                position(daCell);
            }
        }
    }

    // adjust cells in a later column
    for(; iter; ++iter) {
        if (iter->firstColumn() > col ) {
            iter->setFirstColumn( iter->firstColumn() - 1);
            position(iter.current());
        }
    }

    // move pointers in 2d array back one column to occupy
    // removed column
    for(uint i = 0; i < m_rows; i++) {
        for(uint j = col + 1; j < m_cols; j++)
             m_rowArray[i]->m_cellArray.insert(j-1, m_rowArray[i]->m_cellArray[j]);
    }
    m_cols--;
    rc.m_initialized = true;

    validate();
    recalcCols( col, 0 );
    recalcRows( col, 0 );
}

void KWTableFrameSet::reInsertColumn(RemovedColumn &rc)
{
    Q3ValueList<double>::iterator tmp = m_colPositions.at(rc.m_index);

    tmp = m_colPositions.insert(tmp, *tmp);
    tmp++;
    while(tmp != m_colPositions.end()) {
        (*tmp) = (*tmp) + rc.m_width;
        tmp++;
    }

    // if a cell starts after the column we are inserting, it
    // must be moved to the right, except if it is going to
    // occury the reinserted column also.
    for ( MarkedIterator cells(this); cells ; ++cells ) {
        if ( cells->firstColumn() >= rc.m_index &&
            (rc.m_column.at(cells->firstRow()) != cells.current())) {

            cells->setFirstColumn(cells->firstColumn() + 1);
        }
    }
    insertEmptyColumn(rc.m_index);
    m_cols++;

    for(uint i = 0; i < m_rows; ++i) {
        bool removed = rc.m_removed[i];
        Cell *daCell = rc.m_column.at(i);
        if(i == daCell->firstRow()) {
            if(removed) {
                daCell->setColumnSpan(1);
                m_frames.append(daCell->frame(0));
                m_nr_cells++;
            }
            else  {
               daCell->setColumnSpan(daCell->columnSpan() + 1);
            }
            addCell(daCell);
        }
    }

    validate();
    finalize();
}

void KWTableFrameSet::insertNewColumn( uint idx, double width)
{
    Q3ValueList<double>::iterator tmp = m_colPositions.at(idx);

    tmp = m_colPositions.insert(tmp, *tmp);
    tmp++;
    while(tmp!=m_colPositions.end()) {
        (*tmp)= (*tmp)+width;
        tmp++;
    }

    for ( MarkedIterator cells(this); cells ; ++cells ) {
        if ( cells->firstColumn() >= idx) { // move all cells right of the new col.
            cells->setFirstColumn(cells->firstColumn() + 1);
        }
    }
    insertEmptyColumn(idx);
    m_cols++;
    uint copyCol = (idx == 0) ? 1 : idx - 1 ;

    // make the new cells
    // note that the loop counter is mucked with in the loop!
    for( unsigned int i = 0; i < getRows(); i++ ) {

        // can't break a cell in half, so if there is the same cell
        // on both sides of inserted column, it occupies the new
        // column as well
        if(idx != 0 && (idx != m_cols -1)) {
            Cell *c = cell(i, idx - 1);
            if( c == cell(i, idx + 1) ) {
//                m_rowArray[i]->m_cellArray.insert(idx, c);
                c->setColumnSpan(c->columnSpan() + 1);
                addCell(c);
                i += c->rowSpan() - 1;
                continue; // don't need to make a new cell
            }
        }

        Cell *newCell = new Cell( this, i, idx, QString::null );
        KWFrame *theFrame = new KWFrame(cell(i, copyCol)->frame(0));
        newCell->addFrame( theFrame, false );
        position(newCell);
        m_nr_cells++;
    }
    validate();
    finalize();
}

void KWTableFrameSet::ungroup()
{
//    m_cells.setAutoDelete( false );
//    m_cells.clear();
    m_nr_cells = 0;

    m_active = false;
}

void KWTableFrameSet::group()
{
//   m_cells.setAutoDelete( true );
//   m_cells.clear();

   m_nr_cells = 0;
   m_active = true;
}

KCommand *KWTableFrameSet::joinCells(unsigned int colBegin,unsigned int rowBegin, unsigned int colEnd,unsigned int rowEnd) {
    Cell *firstCell = cell(rowBegin, colBegin);
    // if just one cell selected for joining; exit.
    if(rowBegin == rowEnd && colBegin == colEnd || cell(rowBegin,colBegin) == cell(rowEnd,colEnd))
        return 0L;
    Q3PtrList<KWFrameSet> listFrameSet;
    Q3PtrList<KWFrame> listCopyFrame;

    // do the actual merge.
    for(unsigned int i=colBegin; i<=colEnd;i++) {
        for(unsigned int j=rowBegin; j<=rowEnd;j++) {
            Cell *daCell = cell(j,i);
            if(daCell && daCell!=firstCell) {
                listFrameSet.append(daCell);
                listCopyFrame.append(daCell->frame(0)->getCopy());
                daCell->deleteFrame( daCell->frame(0));
            }
        }
    }

    Q_ASSERT(firstCell);
    // update firstcell properties to reflect the merge
    firstCell->setColumnSpan(colEnd-colBegin+1);
    firstCell->setRowSpan(rowEnd-rowBegin+1);
    addCell(firstCell);
    position(firstCell);
    validate();

    m_doc->updateAllFrames(); // TODO: only fs->updateFrames() & m_doc->updateFramesOnTopOrBelow(pageNum)
    m_doc->repaintAllViews();
    return new KWJoinCellCommand( i18n("Join Cells"), this,colBegin,rowBegin, colEnd,rowEnd,listFrameSet,listCopyFrame);
}

KCommand *KWTableFrameSet::splitCell(unsigned int intoRows, unsigned int intoCols, unsigned int col, unsigned int row, Q3PtrList<KWFrameSet> listFrameSet, Q3PtrList<KWFrame>listFrame) {
    if(intoRows < 1 || intoCols < 1)
        return 0L;

    kDebug(32004) << "KWTableFrameSet::splitCell" << endl;
    Cell *daCell=cell(row,col);
    int rowsDiff = intoRows - daCell->rowSpan();
    int colsDiff = ((int) intoCols) - daCell->columnSpan();

    if(rowsDiff >0) {
        unsigned int adjustment=0;
        Q3ValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
        while(pageBound != m_pageBoundaries.end() && (*pageBound) <= row) {
            adjustment++;
            pageBound++;
        }
        double height = (m_rowPositions[row+adjustment+1] - m_rowPositions[row+adjustment])/intoRows;

        Q3ValueList<double>::iterator iRow = m_rowPositions.at(adjustment+row);
        for (int i=0; i < rowsDiff; i++) {
            double newPos = *iRow + height;
            iRow++;
            iRow=m_rowPositions.insert(iRow, newPos);
        }

        // insert more rows into m_rowArray
        for(int i = 0; i < rowsDiff; ++i) {
            insertRowVector(row+i+1, new Row);
            m_rows++;
        }

       // m_rows += rowsDiff;
        //for(unsigned int i=0; i < m_rowPositions.count() ; ++i)
        //    kDebug(32004) << "row " << i << ": " << m_rowPositions[i] << endl);
    }
    if(colsDiff >0) {
        double width = (m_colPositions[col+1] - m_colPositions[col])/intoCols;

        Q3ValueList<double>::iterator iCol = m_colPositions.at(col);
        for (int i=0; i < colsDiff; i++) {
            double newPos = *iCol + width;
            iCol++;
            iCol=m_colPositions.insert(iCol, newPos);
        }

        for(int i = 0; i < colsDiff; i++) {
            insertEmptyColumn(col+i+1);
            m_cols++;
        }
        //for(unsigned int i=0; i < m_colPositions.count(); ++i)
        //    kDebug(32004) << "col " << i << ": " << m_colPositions[i] << endl);
        //m_cols += colsDiff;
    }

    // adjust cellspan and rowspan on other cells.
    for (CheckedIter i(this); i ; ++i) {
        if(daCell == i) continue;

        if(rowsDiff>0) {
            if(row >= i->firstRow()&& row < i->firstRow()+ i->rowSpan())
                i->setRowSpan(i->rowSpan() + rowsDiff);
            if(i->firstRow() > row) {
                i->setFirstRow(i->firstRow() + rowsDiff);
            //    theCell->frame(0)->setTop(theCell->frame(0)->top()+extraHeight);
            }
        }
        if(colsDiff>0) {
            if(col >= i->firstColumn() && col < i->columnAfter())
                i->setColumnSpan(i->columnSpan() + colsDiff);
            if(i->firstColumn() > col)
                i->setFirstColumn(i->firstColumn() + colsDiff);
        }
        /*if(extraHeight != 0 && theCell->firstRow()== row) {
            theCell->frame(0)->setHeight(theCell->frame(0)->height()+extraHeight);
        } */
        if ( rowsDiff > 0 || colsDiff > 0 ) // something changed?
            addCell( i ); // update arrays
    }

    int i=0;
    KWFrame *firstFrame = daCell->frame(0);
    // create new cells
    for (unsigned int y = 0; y < intoRows; y++) {
        for (unsigned int x = 0; x < intoCols; x++){
            if(x==0 && y==0)
                continue; // the orig cell takes this spot.

            Cell *lastFrameSet=0L;

            if(listFrameSet.isEmpty())
                lastFrameSet = new Cell( this, y + row, x + col );
            else
                lastFrameSet = static_cast<KWTableFrameSet::Cell*> (listFrameSet.at(i));
            lastFrameSet->setGroupManager(this);

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
                lastFrameSet->setRowSpan(lastFrameSet->rowSpan() - rowsDiff);
            if(colsDiff <0 && x==0)
                lastFrameSet->setColumnSpan(lastFrameSet->columnSpan() - colsDiff);

            addCell( lastFrameSet );
            position(lastFrameSet);
        }
    }

    // set new row and col-span. Use intermediate ints otherwise we get strange results as the
    // intermediate result could be negative (which goes wrong with unsigned ints)
    int r = (daCell->rowSpan() +1) - intoRows;
    if(r < 1) r=1;
    daCell->setRowSpan(r);

    int c = (daCell->columnSpan() + 1) - intoCols;
    if(c < 1)  c=1;
    daCell->setColumnSpan(c);

    position(daCell);
    addCell(daCell);
    validate();

    finalize();

    return new KWSplitCellCommand(i18n("Split Cells"),this,col,row,intoCols, intoRows);
}

void KWTableFrameSet::viewFormatting( QPainter &/*painter*/, int )
{
}

void KWTableFrameSet::validate()
{
    for(CheckedIter cells(this); cells; ++cells) {
        if(cells->columnSpan() == 0 || cells->rowSpan() == 0)  {
            kDebug(32004) << " KWTableFrameSet::validate(): zero dimension" << endl;
            kDebug(32004) << cells->firstRow() << " " << cells->firstColumn() << " " << cells->rowSpan()
                << " " << cells->columnSpan() << endl;
        }

        for(uint i = cells->firstRow(); i < cells->rowAfter(); ++i) {

             for(uint j = cells->firstColumn(); j < cells->columnAfter(); ++j) {
                  if( cell(i,j) != cells.current() ) {

                      QString str = QString("| 0x%1 ").arg( (unsigned long)cells.current(), 0, 16 );
                      kDebug(32004) << " KWTableFrameSet::validate() failed " << endl;
                      kDebug(32004) << "at row:  "<< i << " col: "<< j << " cell: "<<  str << endl;
                      kDebug(32004) << cells->firstRow() << " " << cells->firstColumn() << " " << cells->rowSpan()
                           << " " << cells->columnSpan() << endl;
                      //printArrayDebug();
                  }
             }
        }
    }
}

void KWTableFrameSet::createEmptyRegion( const QRect & crect, QRegion & emptyRegion, KWViewMode *viewMode )
{
    // Avoid iterating over all cells if we are out of view
    if ( !viewMode->normalToView( m_doc->zoomRect( boundingRect() ) ).intersects( crect ) )
        return;

    QRect outerRect( viewMode->normalToView( m_doc->zoomRect( boundingRect() )));
    outerRect &= crect;
        if ( !outerRect.isEmpty() )
            emptyRegion = emptyRegion.subtract( outerRect );

    Q3PtrListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
    {
        QRect outerRect( viewMode->normalToView( frameIt.current()->outerRect(viewMode) ) );
        //kDebug(32004) << "KWTableFrameSet::createEmptyRegion outerRect=" << DEBUGRECT( outerRect )
        //          << " crect=" << DEBUGRECT( crect ) << endl;
        outerRect &= crect;
        if ( !outerRect.isEmpty() )
            emptyRegion = emptyRegion.subtract( outerRect );
    }
}

void KWTableFrameSet::drawBorders( QPainter& painter, const QRect &crect, KWViewMode *viewMode ) {

    /*  Draw the borders on top of the lines stores in the m_rowPositions and m_colPositions arrays.
     *  check the relevant cells for borders and thus line thickness.
     *  We move the outer lines (on row==0 and col==0 plus on col=getColumns() etc) a bit so they will stay
     *  inside the boundary of the table!
     */
    painter.save();
    QPen previewLinePen( QApplication::palette().color(  QPalette::Active, QColorGroup::Mid ) );
    QColor defaultBorderColor = KoTextFormat::defaultTextColor( &painter );
    const int minborder = 1;
    bool drawPreviewLines = viewMode && viewMode->drawFrameBorders();

    // *** draw horizontal lines *** //
    unsigned int row=0;
    Q3ValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
    for (unsigned int i=0 ; i < m_rowPositions.count() ; i++) {
        //kDebug(32004) << "Horizontal line code. i: " << i << endl;
        bool bottom=false;
        if( (pageBound!=m_pageBoundaries.end() && (*pageBound) == row)
            || i == m_rowPositions.count()-1)
            bottom=true;  // at end of page or end of table draw bottom border of cell.

        const KoBorder *border=0;
        double startPos =0;
        for(unsigned int col=0; col <= getColumns();) {
            //kDebug(32004) << "bottom=" << bottom << " row=" << row << " col=" << col << endl;
            Cell *daCell = col < getColumns() ? cell(bottom?row-1:row, col) : 0;
            //if(daCell) kDebug(32004) << "cell (" << daCell->firstRow()<< "," << daCell->firstColumn() << ")" << endl;
            //else kDebug(32004) << "cell: " << daCell << endl;

            if(daCell && daCell->firstRow() != (bottom?row-1:row))
                daCell=0;

            if(startPos!=0 && (!daCell || col == getColumns() || (
                    bottom && daCell->frame(0)->bottomBorder()!=*border ||
                    !bottom && daCell->frame(0)->topBorder()!=*border
                    ))) {
                if(border->width() > 0 || drawPreviewLines) {
                    double y = m_rowPositions[i];
                    if(row==0)
                        y+=border->width() / 2; // move slightly down.
                    else if (row == getRows())
                        y-=border->width() / 2; // move slightly up.
                    int ypix = m_doc->zoomItY(y);
                    double offset=0.0;
                    if(border->width() > 0 && col!=getColumns()) { // offset border when not at right most cell.
                        if(daCell) offset=daCell->leftBorder();
                        if ( row > 0 ) {
                            Cell *c = cell(row-1, col);
                            if(c) offset=qMax(offset, c->leftBorder());
                        }
                    }
                    double x = m_colPositions[col] + offset;
                    QPoint topLeft = viewMode->normalToView(QPoint(m_doc->zoomItX(startPos), ypix));
                    QPoint bottomRight = viewMode->normalToView(QPoint(m_doc->zoomItX(x), ypix));
                    QRect line = QRect(topLeft, bottomRight);
                    if(crect.intersects( line )) {
                        //if(border->width() <= 0) kDebug(32004) << "preview line" << endl;
                        if(border->width() <= 0)
                            painter.setPen( previewLinePen );
                        else {
                            int borderWidth = KoBorder::zoomWidthY( border->width(), m_doc, minborder );
                            painter.setPen( KoBorder::borderPen( *border, borderWidth, defaultBorderColor ) );
                        }
                        //kDebug(32004) << "Paint: painter.drawHorizontalLine(" << line.left() << ","  << line.top() << "," <<  line.right() << ","  << line.bottom() << ")\n";
                        painter.drawLine( line.left(), line.top(), line.right(), line.bottom());
                    }
                }
                // reset startPos
                startPos = 0;
            }
            if(daCell && startPos==0) {
                if(bottom)
                    border=&(daCell->frame(0)->bottomBorder());
                else
                    border=&(daCell->frame(0)->topBorder());

                if(col==0) // left most cell
                    startPos = m_colPositions[col];
                else {
                    double offset=0.0;
                    if(border->width() > 0) { // move line to the left a bit to compensate for the left border
                        if(daCell) offset=daCell->leftBorder();
                        if ( row > 0 ) {
                            Cell *c = cell(row-1, col);
                            if(c) offset=qMax(offset, c->leftBorder());
                        }
                    }
                    startPos = m_colPositions[col] - offset;
                }
            }
            col += daCell ? daCell->columnSpan() : 1;
        }
        if(pageBound!=m_pageBoundaries.end() && (*pageBound) == row)
            pageBound++;
        else
            row++;
    }

    // *** draw vertical lines *** //
    for (unsigned int col=0 ; col < m_colPositions.count(); col++) {
        //kDebug(32004) << "Vertical line code. col: " << col << endl;
        bool right = false;
        if(col == m_colPositions.count()-1)
            right = true; // draw right border of cell.
        int cellColumn = right?col-1:col; // the column we'll be looking for in the loop below
        Q_ASSERT( cellColumn >= 0 );

        const KoBorder *border = 0;
        int startRow = -1;
        for(unsigned int row=0; row <= getRows();) {
            //kDebug(32004) << "row=" << row << " cellColumn=" << cellColumn << endl;
            Cell *daCell = row < getRows() ? cell(row, cellColumn) : 0;

            //kDebug(32004) << "Drawing vert. Line for cell row: " << row << " col: " << cellColumn << endl;
            if(daCell && daCell->firstColumn() != (uint)cellColumn)
                daCell=0;

#if 0
            kDebug() << "Condition: startRow:" << (startRow!=-1) << endl;
            if ( startRow != -1 )  {
                Q_ASSERT( border );
                kDebug() << "Other conditions: cell:" << !daCell << endl;
                kDebug() << " or last row:" << ( row == ( int )getRows() ) << endl;
                if ( daCell )
                    kDebug() << "Different border:" <<
                ( ( right && daCell->frame(0)->rightBorder() != *border) ||
                ( !right && daCell->frame(0)->leftBorder() != *border) )
                    << endl;
            }
#endif

            // be sure that the right border of the table is drawn even for joined cells
            if ( !daCell && startRow == -1 && cellColumn == ((int)m_colPositions.count()-2 ) && right )
            {
              // find the joined cell
              int col = cellColumn;
              while ( !daCell && col>0 )
              {
                col--;
                daCell = cell(row, col);
              }
              if ( daCell && daCell->isJoinedCell() && ( (int)daCell->columnSpan() + col -1 ) == cellColumn )
              {
                border = &(daCell->frame(0)->rightBorder());
                startRow = row;
              }
              else
                daCell = 0;
            }

            // Draw when something changed (different kind of border) or we're at the end
            // This code could be rewritten in a more QRT-like way
            // (iterate and compare with next, instead of the startRow/cell/border hack...)
            if(startRow != -1 &&
               (!daCell || row == getRows() ||
                ( right && daCell->frame(0)->rightBorder() != *border) ||
                ( !right && daCell->frame(0)->leftBorder() != *border) )
                ) {
                if(border->width() > 0 || drawPreviewLines) {
                    double x = m_colPositions[col];
                    if(col==0) {
                        x+=border->width() / 2;
                    } else if(col==getColumns()) {
                        x-=border->width() / 2;
                    }
                    int xpix = m_doc->zoomItX(x);
                    Q3ValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
                    unsigned int topRow=startRow;
                    //kDebug(32004) << "Drawing from topRow=" << topRow << endl;
                    do { // draw minimum of one line per page.
                        while( pageBound != m_pageBoundaries.end() && *(pageBound) < topRow )
                            pageBound++;

                        unsigned int bottomRow;
                        if(pageBound == m_pageBoundaries.end())
                            bottomRow = m_rowPositions.count()-1;
                        else
                            bottomRow = *(pageBound++);

                        //kDebug(32004) << "from: " << topRow << " to: " << qMin((uint)row, bottomRow) << endl;
                        //kDebug(32004) << "from: " << m_rowPositions[topRow] << " to: " << m_rowPositions[qMin((uint)row, bottomRow)] << endl;
                        double offset=0.0;
                        if(border->width() > 0) {
                            //kDebug(32004) << "looking at topRow=" << topRow << " col=" << col << endl;
                            Cell *c=cell(topRow,col);
                            if(c) offset=c->topBorder();
                            if ( col > 0 ) {
                                c=cell(topRow,col-1);
                                if(c) offset=qMax(offset,c->topBorder());
                            }
                            if(topRow==0) offset=0.0;
                        }
                        double top=m_rowPositions[topRow]-offset;

                        unsigned int toRow=qMin((uint)row,bottomRow);
                        offset=0.0;
                        if(border->width() > 0 && toRow!=bottomRow) {
                            if(daCell) offset=daCell->topBorder();
                            Cell *c=cell(toRow,col-1);
                            if(c) offset=qMax(offset,c->topBorder());
                        }
                        double bottom=m_rowPositions[toRow] + offset;

                        QPoint topLeft = viewMode->normalToView(QPoint(xpix, m_doc->zoomItY(top)));
                        QPoint bottomRight = viewMode->normalToView(QPoint(xpix, m_doc->zoomItY(bottom)));
                        QRect line = QRect(topLeft, bottomRight);
                        if(crect.intersects( line )) {
                            if(border->width() <= 0)
                                painter.setPen( previewLinePen );
                            else {
                                int borderWidth = KoBorder::zoomWidthX( border->width(), m_doc, minborder );
                                painter.setPen(KoBorder::borderPen( *border, borderWidth, defaultBorderColor ));
                            }
                            //kDebug(32004) << "drawBorders(): painter.drawVerticalLine(" << line.left() << ","  << line.top() << "," <<  line.right() << ","  << line.bottom() << ")\n";
                            painter.drawLine( line.left(), line.top(), line.right(), line.bottom());
                        }

                        topRow=bottomRow+1;
                    } while(topRow < (uint)row && topRow != m_rowPositions.count());
                } // end "if border to be drawn"

                // reset startRow
                startRow = -1;
            }

            if(daCell && startRow == -1) {
                startRow = row;
                if(right)
                    border = &(daCell->frame(0)->rightBorder());
                else
                    border = &(daCell->frame(0)->leftBorder());
                //kDebug(32004) << "startRow set to " << row << endl;
            }
            row += daCell ? daCell->rowSpan() : 1;
            //kDebug(32004) << "End of loop, row=" << row << endl;
        }
    }

#if 0
    if(drawPreviewLines) {
        QPen minsizeLinePen( Qt::red );
        painter.setPen( minsizeLinePen );
        for ( unsigned int i = 0; i < m_cells.count(); i++ ) {
            Cell *daCell = m_cells.at( i );
            double y = daCell->frame(0)->top() + daCell->frame(0)->minimumFrameHeight() + 1.5;
            if(y >= daCell->frame(0)->bottom()) continue;
            int ypix=m_doc->zoomItY(y);
            QPoint topLeft = viewMode->normalToView(QPoint(m_doc->zoomItX(daCell->frame(0)->left()), ypix));
            QPoint bottomRight = viewMode->normalToView(QPoint(m_doc->zoomItX(daCell->frame(0)->right()), ypix));
            QRect line = QRect(topLeft, bottomRight);
            if(crect.intersects( line )) {
                painter.drawLine( line.left(), line.top(), line.right(), line.bottom());
            }
        }
    }
#endif

    painter.restore();
}

void KWTableFrameSet::drawContents( QPainter * painter, const QRect & crect,
                                    const QColorGroup & cg, bool onlyChanged, bool resetChanged,
                                    KWFrameSetEdit * edit, KWViewMode * viewMode,
                                    KWFrameViewManager *fvm )
{
    for (TableIter cells(this) ; cells ; ++cells)
    {
        if (edit)
        {
            KWTableFrameSetEdit * tableEdit = static_cast<KWTableFrameSetEdit *>(edit);
            if ( tableEdit->currentCell() && ((Cell*) cells) == tableEdit->currentCell()->frameSet() )
            {
                cells->drawContents( painter, crect, cg, onlyChanged, resetChanged, tableEdit->currentCell(), viewMode, fvm );
                continue;
            }
        }
        cells->drawContents( painter, crect, cg, onlyChanged, resetChanged, 0L, viewMode, fvm );
    }
    drawBorders( *painter, crect, viewMode );
    //kDebug(32004) << "drawContents()" << endl;
}

// Called by KWAnchor for inline tables
// TODO: for non-inline ones we need a text-box around us...
// Well, even for inline-as-char ones.... Currently being debated with OASIS.
void KWTableFrameSet::saveOasis( KoXmlWriter& writer, KoSavingContext& context, bool ) const
{
    writer.startElement( "table:table" );
    writer.addAttribute( "table:name", name() );
    KoGenStyle tableStyle( KWDocument::STYLE_TABLE, "table" );
    tableStyle.addProperty( "table:align", "margins" );
    tableStyle.addPropertyPt( "style:width", m_colPositions.last()-m_colPositions[0] );
    const QString tableStyleName = context.mainStyles().lookup( tableStyle, "table" );
    writer.addAttribute( "table:style-name", tableStyleName );

    // ### to minimize the XML, we could use table:number-columns-repeated here
    // when a number of consecutive columns have the exact same style.
    for ( uint colNr = 0; colNr < getColumns(); ++colNr )
    {
        writer.startElement( "table:table-column" );
        KoGenStyle columnStyle( KWDocument::STYLE_TABLE_COLUMN, "table-column" );
        columnStyle.addPropertyPt( "style:column-width", m_colPositions[colNr+1] - m_colPositions[colNr] );
        const QString colStyleName = context.mainStyles().lookup( columnStyle, "col" );
        writer.addAttribute( "table:style-name", colStyleName );
        writer.endElement(); // table:table-column
    }

    // TODO table-header-rows once supported

    for ( uint row = 0; row < getRows(); ++row )
    {
        writer.startElement( "table:table-row" );

        KoGenStyle rowStyle( KWDocument::STYLE_TABLE_ROW, "table-row" );
        rowStyle.addPropertyPt( "table:row-height", m_rowPositions[row+1] - m_rowPositions[row] );
        // TODO is min-row-height or use-optimal-row-height necessary?
        const QString rowStyleName = context.mainStyles().lookup( rowStyle, "row" );
        writer.addAttribute( "table:style-name", rowStyleName );

        for ( uint col = 0; col < getColumns(); ++col )
        {
            Cell* daCell = cell(row, col);
            Q_ASSERT( daCell );
            if ( !daCell )
                continue;

            if ( daCell->isFirstGridPosnFast( row, col ) )
            {
                writer.startElement( "table:table-cell" );

                // Style: background, border, padding.
                KoGenStyle cellStyle( KWDocument::STYLE_TABLE_CELL_AUTO, "table-cell" );
                daCell->frame( 0 )->saveBorderProperties( cellStyle );
                const QString colStyleName = context.mainStyles().lookup( cellStyle, "cell" );
                writer.addAttribute( "table:style-name", colStyleName );

                // Attributes
                if ( daCell->columnSpan() > 1 )
                    writer.addAttribute( "table:number-columns-spanned", daCell->columnSpan() );
                if ( daCell->rowSpan() > 1 )
                    writer.addAttribute( "table:number-row-spanned", daCell->rowSpan() );

                // Content
                daCell->saveOasisContent( writer, context );

                writer.endElement(); // table:table-cell
            }
            else
            {
                // Empty element for the covered cell
                writer.startElement( "table:covered-table-cell" );
                writer.endElement();
            }
        }
        writer.endElement(); // table:table-row
    }

    writer.endElement(); // table:table
}

void KWTableFrameSet::loadOasis( const QDomElement& tableTag, KoOasisContext& context )
{
    // Left position of each column. The last one defined is the right position of the last cell/column.
    Q3MemArray<double> columnLefts(4);
    uint maxColumns = columnLefts.size() - 1;

    uint col = 0;
    columnLefts[0] = 0.0; // Initialize left of first cell
    QDomElement elem;
    forEachElement( elem, tableTag )
    {
        if ( elem.localName() == "table-column" && elem.namespaceURI() == KoXmlNS::table )
        {
            uint repeat = elem.attributeNS( KoXmlNS::table, "number-columns-repeated", "1").toUInt(); // Default 1 time
            if (!repeat)
                repeat=1; // At least one column defined!
            KoStyleStack& styleStack = context.styleStack();
            styleStack.setTypeProperties( "table-column" );
            styleStack.save();
            context.fillStyleStack( elem, KoXmlNS::table, "style-name", "table-column" );

            QString strWidth = styleStack.attributeNS( KoXmlNS::style, "column-width" );
            double width = KoUnit::parseValue( strWidth );

            if ( width < 1.0 ) // Something is wrong with the width
            {
                kWarning(32004) << "Table column width ridiculous, assuming 1 inch!" << endl;
                width = 72.0;
            }
            else
                kDebug(32004) << "- style width " << width << endl;

            for ( uint j = 0; j < repeat; ++j )
            {
                ++col;
                if ( col >= maxColumns )
                {
                    // We need more columns
                    maxColumns += 4;
                    columnLefts.resize( maxColumns+1, Q3GArray::SpeedOptim );
                }
                columnLefts[col] = width + columnLefts[col-1];
                kDebug(32004) << "Cell column " << col-1 << " left " << columnLefts[col-1] << " right " << columnLefts[col] << endl;
            }
            styleStack.restore();
        }
    }

    uint row = 0;
    uint column = 0;
    parseInsideOfTable( tableTag, context, columnLefts, row, column, 0 );
}

void KWTableFrameSet::parseInsideOfTable( const QDomElement& parent, KoOasisContext& context,
                                          const Q3MemArray<double> & columnLefts, uint& row, uint& column,
                                          double currentRowHeight )
{
    kDebug(32004) << "parseInsideOfTable" << endl;
    KoStyleStack& styleStack = context.styleStack();

    QDomElement e;
    forEachElement( e, parent )
    {
        const QString localName = e.localName();
        const QString ns = e.namespaceURI();
        if ( ns != KoXmlNS::table ) {
            kWarning(32004) << "Skipping element " << e.tagName() << " (in parseInsideOfTable)" << endl;
            continue;
        }

        styleStack.save();
        if ( localName == "table-cell" )
        {
            loadOasisCell( e, context, columnLefts, row, column, currentRowHeight );
            ++column;
        }
        else if ( localName == "covered-table-cell" )
        {
            ++column;
        }
        else if ( localName == "table-row" )
        {
            context.fillStyleStack( e, KoXmlNS::table, "style-name", "table-row" );
            context.styleStack().setTypeProperties( "table-row" );

            // Load row height in case it was set - note that it might not be set (e.g. OOo)
            double rowHeight = styleStack.attributeNS( KoXmlNS::table, "row-height" ).toDouble();
            column = 0;
            parseInsideOfTable( e, context, columnLefts, row, column, rowHeight );
            ++row;
        }
        else if ( localName == "table-header-rows" ) // ###TODO
        {
            // TODO: do we need to fillStyleStack?
            parseInsideOfTable( e, context, columnLefts, row, column, currentRowHeight );
        }
        else if ( localName == "table-column" )
        {
            // Already treated in loadOasis, we do not need to do anything here!
        }
        // TODO sub-table [ add to stack and expand at end of table loading ]
        else
        {
            kWarning(32004) << "Skipping element " << localName << " (in parseInsideOfTable)" << endl;
        }

        styleStack.restore();
    }
}

void KWTableFrameSet::loadOasisCell( const QDomElement& element, KoOasisContext& context,
                                     const Q3MemArray<double> & columnLefts, uint row, uint column,
                                     double currentRowHeight )
{
    //kDebug(32004) << k_funcinfo << element.localName() << " " << row << "," << column << endl;

    KoStyleStack& styleStack = context.styleStack();
    uint rowSpan = element.attributeNS( KoXmlNS::table, "number-rows-spanned", QString::null ).toUInt();
    if ( rowSpan == 0 )
        rowSpan = 1;
    uint colSpan = element.attributeNS( KoXmlNS::table, "number-columns-spanned", QString::null ).toUInt();
    if ( colSpan == 0 )
        colSpan = 1;

    // m_rowPositions / m_colPositions could be QMemArrays, or QValueVectors...
    while(m_rowPositions.count() <= row + rowSpan + m_pageBoundaries.count()) {
        m_rowPositions.append(0);
    }
    while(m_colPositions.count() <= column + colSpan) {
        m_colPositions.append(0);
    }

    Cell *daCell = new Cell( this, row, column, QString::null /*unused*/ );

    daCell->setRowSpan( rowSpan );
    daCell->setColumnSpan( colSpan );
    addCell( daCell ); // rowSpan/colSpan have changed -> update array

    double width = columnLefts[ qMin( column+colSpan, columnLefts.size()-1 ) ] - columnLefts[column];
    double height = currentRowHeight > 0 ? currentRowHeight : 20;
    KWFrame* frame = new KWFrame( daCell, columnLefts[column], 0, width, height );
    if ( currentRowHeight > 0 )
        frame->setMinimumFrameHeight( height ); // ensure that text formatting won't resize it down
    frame->setRunAround( KWFrame::RA_NO );
    frame->setFrameBehavior( KWFrame::AutoExtendFrame );
    frame->setNewFrameBehavior( KWFrame::NoFollowup );
    daCell->addFrame( frame, false );

    context.fillStyleStack( element, KoXmlNS::table, "style-name", "table-cell" );
    styleStack.setTypeProperties( "table-cell" );

    daCell->frame( 0 )->loadBorderProperties( styleStack );

    daCell->loadOasisContent( element, context );
    afterLoadingCell( daCell );
}

// Old XML
QDomElement KWTableFrameSet::save( QDomElement &parentElem, bool saveFrames ) {
    // When saving to a file, we don't have anything specific to the frameset to save.
    // Save the cells only.
    for (TableIter cells(this) ; cells ; ++cells)
        cells->save(parentElem, saveFrames);
    return QDomElement(); // No englobing element for tables...
}

// Old XML
QDomElement KWTableFrameSet::toXML( QDomElement &parentElem, bool saveFrames )
{
    QDomElement framesetElem = parentElem.ownerDocument().createElement( "FRAMESET" );
    parentElem.appendChild( framesetElem );
    KWFrameSet::saveCommon( framesetElem, false ); // Save the frameset attributes
    // Save the cells
    save( framesetElem, saveFrames );
    return framesetElem;
}

// Old XML
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

// Old XML
KWTableFrameSet::Cell* KWTableFrameSet::loadCell( QDomElement &framesetElem, bool loadFrames, bool useNames )
{
    int _row = KWDocument::getAttribute( framesetElem, "row", 0 );
    if(_row <0) _row =0;
    unsigned int row=_row;
    int _col = KWDocument::getAttribute( framesetElem, "col", 0 );
    if(_col <0) _col =0;
    int _rows = KWDocument::getAttribute( framesetElem, "rows", 1 );
    if(_rows <0) _rows = 1;
    int _cols = KWDocument::getAttribute( framesetElem, "cols", 1 );
    if(_cols <0) _cols = 1;

    // m_rowPositions / m_colPositions could be QMemArrays, or QValueVectors...
    while(m_rowPositions.count() <= static_cast<unsigned int>(row + _rows + m_pageBoundaries.count())) {
        m_rowPositions.append(0);
    }
    while(m_colPositions.count() <= static_cast<unsigned int>(_col + _cols)) {
        m_colPositions.append(0);
    }

    Cell *daCell = new Cell( this, row, _col, QString::null /*unused*/ );
    QString autoName = daCell->name();
    //kDebug(32004) << "KWTableFrameSet::loadCell autoName=" << autoName << endl;
    daCell->load( framesetElem, loadFrames );
    daCell->setRowSpan(_rows);
    daCell->setColumnSpan(_cols);
    addCell( daCell ); // rowSpan/colSpan have changed -> update array
    afterLoadingCell( daCell );
    if ( !useNames )
        daCell->setName( autoName );
    return daCell;
}

// Shared between old xml and oasis
void KWTableFrameSet::afterLoadingCell( Cell* daCell )
{
    uint row = daCell->firstRow();
    uint col = daCell->firstColumn();
    uint rowSpan = daCell->rowSpan();
    uint colSpan = daCell->columnSpan();
    if(m_pageBoundaries.count() > 0) {
        unsigned int adjustment=0;
        Q3ValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
        while(pageBound != m_pageBoundaries.end() && (*pageBound) <= row + adjustment) {
            adjustment++;
            pageBound++;
        }
        row+=adjustment;
    }

    kDebug(32004) << "loading cell (" << row << "," << col << ")\n";
    if(daCell->frame(0)) {
        daCell->frame(0)->setMinimumFrameHeight(daCell->frame(0)->height()); // TODO run the formatter over the text here
        Q3ValueList<double>::iterator tmp = m_colPositions.at(col);
        if(*tmp == 0) (*tmp) = daCell->frame(0)->left();
        else (*tmp) = (daCell->frame(0)->left() + *tmp) / 2;

        tmp = m_colPositions.at(col+colSpan);
        if(*tmp == 0) (*tmp) = daCell->frame(0)->right();
        else (*tmp) = (daCell->frame(0)->right() + *tmp) / 2;

        tmp = m_rowPositions.at(row);
        if(*tmp == 0)
            (*tmp) = daCell->frame(0)->top();
        else {
            if (static_cast<int>(*tmp/m_doc->pageLayout().ptHeight) < static_cast<int>(daCell->frame(0)->top()/m_doc->pageLayout().ptHeight)) {
                kDebug(32004) << "This cell is on a new page" << endl;
                Q3ValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
                while(pageBound != m_pageBoundaries.end() && (*pageBound) < row) ++pageBound;
                if(*pageBound!=row) {
                    m_pageBoundaries.insert(pageBound,row++);
                    ++tmp;
                    m_rowPositions.insert(tmp,daCell->frame(0)->top());
                }
            } else
                (*tmp) = (daCell->frame(0)->top() + *tmp) / 2;
        }

        tmp = m_rowPositions.at( row + rowSpan );
        if(*tmp == 0)
            (*tmp) = daCell->frame(0)->bottom();
        else { // untested...
            if (static_cast<int>(*tmp/m_doc->pageLayout().ptHeight) > static_cast<int>(daCell->frame(0)->top()/m_doc->pageLayout().ptHeight)) {
                kDebug(32004) << "next cell is on a new page" << endl;
                Q3ValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
                while(pageBound != m_pageBoundaries.end() && (*pageBound) < row) ++pageBound;
                if(*pageBound!=row) {
                    m_pageBoundaries.insert(pageBound,row++);
                    m_rowPositions.insert(tmp,daCell->frame(0)->bottom());
                }
            } else
                (*tmp) = (daCell->frame(0)->bottom() + *tmp) / 2;
        }
    }

    if ( m_rowPositions.count() != m_rows + 1 ) {
        kDebug() << name() << " loadCell: m_rowPositions=" << m_rowPositions.count() << " m_rows= " << m_rows << endl;
    }
}

int KWTableFrameSet::paragraphs()
{
    int paragraphs = 0;
    for (TableIter cells(this) ; cells ; ++cells)
        paragraphs += cells->paragraphs();
    return paragraphs;
}

int KWTableFrameSet::paragraphsSelected()
{
    int paragraphs = 0;
    for (TableIter cells(this) ; cells ; ++cells)
        paragraphs += cells->paragraphsSelected();
    return paragraphs;
}

bool KWTableFrameSet::statistics( Q3ProgressDialog *progress, ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words,
    ulong & sentences, ulong & syllables, ulong & lines, bool selected )
{
    for (TableIter cells(this) ; cells ; ++cells)
        if( ! cells->statistics( progress, charsWithSpace, charsWithoutSpace, words, sentences, syllables, lines, selected ) )
        {
            return false;
        }
    return true;
}

void KWTableFrameSet::finalize( ) {
    kDebug(32004) << "KWTableFrameSet::finalize" << endl;

    for (TableIter cells(this) ; cells ; ++cells)
    {
        position( cells );
        cells->finalize();
    }

    recalcCols(0, 0);
    recalcRows(0, 0);
    KWFrameSet::finalize();
}

void KWTableFrameSet::layout()
{
    for (TableIter cells(this) ; cells ; ++cells)
        cells->layout();
}

void KWTableFrameSet::invalidate()
{
    for (TableIter cells(this) ; cells ; ++cells)
        cells->invalidate();
}

void KWTableFrameSet::setVisible( bool v )
{
    for (TableIter cells(this) ; cells ; ++cells)
        cells->setVisible( v );

    KWFrameSet::setVisible( v );
}

bool KWTableFrameSet::canRemovePage( int num ) {
    /*  This one is a lot simpler then the one it overrides, we simply don't have
        to check if the frame contains something, the simple existence of a frame
        is enough
    */
    Q3PtrListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt ) {
        if ( frameIt.current()->pageNumber() == num ) {
            return false;
        }
    }
    return true;
}

void KWTableFrameSet::addTextFrameSets( Q3PtrList<KWTextFrameSet> & lst, bool onlyReadWrite )
{
    for (TableIter cells(this) ; cells ; ++cells)
        if (!cells->textObject()->protectContent() || onlyReadWrite )
            lst.append(cells);
}

KWTextFrameSet* KWTableFrameSet::nextTextObject( KWFrameSet *obj )
{
    bool found = false;
    KWTableFrameSet::Cell *tmp = dynamic_cast<KWTableFrameSet::Cell *>(obj);

    // make sure we have this cell
    if ( tmp ) {
        for(TableIter i(this); i; ++i) {
            if(i.current() == tmp) {
                found = true;
                break;
            }
        }
    }

    TableIter iter(this);
    if(found)
        iter.goToCell(tmp);

    for(; iter; ++iter) {
        KWTextFrameSet *newFrm = iter->nextTextObject( obj );
        if(newFrm && newFrm->textObject()->needSpellCheck())
            return newFrm;
    }

    return 0L;
}

void KWTableFrameSet::setZOrder()
{
    for( TableIter cells(this) ; cells ; ++cells ) {
        cells->setZOrder();
    }

}

// TODO provide toPlainText() (reimplemented from KWFrameSet)

QByteArray KWTableFrameSet::convertTableToText() // should be const, but TableIter doesn't allow it
{
    KWOasisSaver oasisSaver( m_doc );
    for (TableIter cells(this); cells; ++cells)
    {
        cells->textObject()->saveOasisContent( oasisSaver.bodyWriter(), oasisSaver.savingContext() );
    }
    if ( !oasisSaver.finish() )
        return QByteArray();
    return oasisSaver.data();
}

#ifndef NDEBUG
void KWTableFrameSet::printDebug( KWFrame * theFrame )
{
    KWTableFrameSet::Cell *daCell = dynamic_cast<KWTableFrameSet::Cell *>( theFrame->frameSet() );
    Q_ASSERT( daCell );
    if ( daCell ) {
        kDebug(32004) << " |  |- row :" << daCell->firstRow() << endl;
        kDebug(32004) << " |  |- col :" << daCell->firstColumn() << endl;
        kDebug(32004) << " |  |- rows:" << daCell->rowSpan() << endl;
        kDebug(32004) << " |  +- cols:" << daCell->columnSpan() << endl;
    }
}

void KWTableFrameSet::printArrayDebug() {
    kDebug(32004) << " |  Row/Cell arrays" << endl;
    Q_ASSERT( m_rows == m_rowArray.size() );
    for ( unsigned int row = 0; row < m_rows; ++row )  {
        QString str = QString( " | Row %1: " ).arg( row );
        for ( unsigned int col = 0; col < getColumns(); ++col )
            str += QString("| 0x%1 ").arg( (unsigned long)(*m_rowArray[row])[col], 0, 16 );
        kDebug(32004) << str<< " |" << endl;
    }
}

void KWTableFrameSet::printDebug() {
    kDebug(32004) << " |  Table size (" << m_rows << "x" << getColumns() << ")" << endl;
    kDebug(32004) << " |  col  " << 0 << ": " << m_colPositions[0] << endl;
    for(unsigned int i=1;i<m_colPositions.count(); ++i)
        kDebug(32004) << " |    |  " << i << ": " << m_colPositions[i] << endl;
    kDebug(32004) << " |  row  " << 0 << ": " << m_rowPositions[0] << endl;
    for(unsigned int i=1;i<m_rowPositions.count(); ++i)
        kDebug(32004) << " |    |  " << i << ": " << m_rowPositions[i] << endl;

    printArrayDebug();
    KWFrameSet::printDebug();
}

#endif

// ===

KWTableFrameSet::Cell::Cell( KWTableFrameSet *table, unsigned int row, unsigned int col, const QString &/*name*/ ) :
    KWTextFrameSet( table->m_doc,
                    // Generate frameset name from table_name+row+col
                    i18nc("Hello dear translator :), 1 is the table name, 2 and 3 are row and column", "%1 Cell %2,%3", table->name() , row, col) )
{
    m_row = row;
    m_col = col;
    m_rows = 1;
    m_cols = 1;
    m_isJoinedCell = false;
    setGroupManager( table );
    table->addCell( this );
}

KWTableFrameSet::Cell::Cell( KWTableFrameSet *table, const Cell &original ) :
    KWTextFrameSet( table->m_doc, original.m_name+'_' )
{
    m_row = original.m_row;
    m_col = original.m_col;
    m_rows = original.m_rows;
    m_cols = original.m_cols;
    m_isJoinedCell = original.m_isJoinedCell;
    setGroupManager( table );
    table->addCell( this );
}

KWTableFrameSet::Cell::~Cell()
{
}

bool KWTableFrameSet::Cell::isAboveOrLeftOf( unsigned row, unsigned col ) const
{
    return ( m_row < row ) || ( ( m_row == row ) && ( m_col < col ) );
}

bool KWTableFrameSet::Cell::containsCell( unsigned row, unsigned col ) const
{
    return ( m_row <= row &&
             m_col <= col &&
             rowAfter() > row &&
             columnAfter() > col );
}

void KWTableFrameSet::Cell::addFrame(KWFrame *_frame, bool recalc) {
    if(groupmanager())
        groupmanager()->addFrame(_frame, recalc);
    KWTextFrameSet::addFrame(_frame, recalc);
}

void KWTableFrameSet::Cell::frameDeleted( KWFrame* frm, bool recalc )
{
    if(groupmanager())
        groupmanager()->deleteFrame( frm, false, recalc );
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
    if(m_col+m_cols==m_groupmanager->getColumns()) // right most cell
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
    if(rowAfter() == m_groupmanager->m_rows) // bottom most cell
        return b;
    return (b / 2);
}

void KWTableFrameSet::Cell::setLeftBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->leftBorder().width() - newBorder.width();
    f->setLeftBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && m_col!=0) {
        diff = diff / 2; // if not outer edge only use halve
        m_groupmanager->cell(m_row, m_col-1)->setRightBorder(newBorder);
    }
    f->setLeft(f->left() - diff);
}

void KWTableFrameSet::Cell::setRightBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->rightBorder().width() - newBorder.width();
    f->setRightBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && m_col+m_cols!=m_groupmanager->getColumns()) {
        diff = diff / 2; // if not outer edge only use halve
        m_groupmanager->cell(m_row, m_col+1)->setLeftBorder(newBorder);
    }
    f->setRight(f->right() + diff);
}

void KWTableFrameSet::Cell::setTopBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->topBorder().width() - newBorder.width();
    f->setTopBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && m_row!=0) {
        diff = diff / 2; // if not outer edge only use halve
        m_groupmanager->cell(m_row-1, m_col)->setBottomBorder(newBorder);
    }
    f->setTop(f->top() - diff);
}

void KWTableFrameSet::Cell::setBottomBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->bottomBorder().width() - newBorder.width();
    f->setBottomBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && rowAfter() != m_groupmanager->m_rows) {
        diff = diff / 2; // if not outer edge only use halve
        m_groupmanager->cell(m_row+1, m_col)->setTopBorder(newBorder);
    }
    f->setBottom(f->bottom() + diff);
}

void KWTableFrameSet::Cell::setZOrder()
{
    Q3PtrListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
    {
        (*frameIt)->setZOrder( kWordDocument()->maxZOrder( (*frameIt)->pageNumber() ) + 1 );
    }
}

void KWTableFrameSet::Cell::drawContents( QPainter * painter, const QRect & crect,
        const QColorGroup & cg, bool onlyChanged, bool resetChanged,
        KWFrameSetEdit * edit, KWViewMode * viewMode, KWFrameViewManager *fvm )
{
    bool printing = painter->device()->devType() == QInternal::Printer;
    bool drawPreviewLines = viewMode && viewMode->drawFrameBorders();
    QRect cellRect = crect;
    if(!printing && drawPreviewLines) {
        // Make sure the clipping is changed so the preview lines (frame borders) are not overwritten.
        QRect zoomedRect( m_doc->zoomRect(*frame(0)) );
        QRect innerFrameRect( viewMode->normalToView( zoomedRect ) );
        innerFrameRect.addCoords(1, 1, -1, -1); // move and shrink
        cellRect = innerFrameRect.intersect(crect);
    }
    KWTextFrameSet::drawContents(painter, cellRect, cg, onlyChanged, resetChanged, edit, viewMode, fvm);
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
    KWFrameSet *fs = tableFrameSet()->cellByPos( dPoint.x(), dPoint.y() );
    KWTextFrameSet *textframeSet = dynamic_cast<KWTextFrameSet *>(fs);

    if ( textframeSet&& textframeSet->protectContent() && !tableFrameSet()->kWordDocument()->cursorInProtectedArea())
        return;

    if ( fs && ( !m_currentCell || fs != m_currentCell->frameSet() ) )
        setCurrentCell( fs );
}

void KWTableFrameSetEdit::setCurrentCell( KWFrameSet * fs, bool eraseSelection )
{
    bool oldProtectContent = false;
    KWTextFrameSet *textframeSet=0L;
    if ( m_currentCell )
        textframeSet = dynamic_cast<KWTextFrameSet *>(m_currentCell->frameSet());
    if ( textframeSet )
        oldProtectContent = textframeSet->protectContent();

    if ( m_currentCell )
    {
        m_currentCell->terminate(eraseSelection);
        delete m_currentCell;
    }
    m_currentCell = fs->createFrameSetEdit( m_canvas );
    textframeSet = dynamic_cast<KWTextFrameSet *>(m_currentCell->frameSet());
    if ( textframeSet )
    {
        if ( oldProtectContent != textframeSet->protectContent())
        {
            m_canvas->kWordDocument()->updateTextFrameSetEdit();
        }
    }


    m_currentFrame = fs->frame( 0 );
    KWTextFrameSetEdit *textframeSetEdit = dynamic_cast<KWTextFrameSetEdit *>(m_currentCell);
    if ( textframeSetEdit )
    {
        textframeSetEdit->ensureCursorVisible();
        //refresh koruler
        m_canvas->gui()->getView()->slotUpdateRuler();
    }
}

KWFrameSetEdit* KWTableFrameSetEdit::currentTextEdit()
{
    return m_currentCell;
}


void KWTableFrameSetEdit::keyPressEvent( QKeyEvent * e )
{
    // This method handles the up/left/down/right navigation keys in tables
    if ( !m_currentCell )
        return;
    KWTableFrameSet::Cell *cell = static_cast<KWTableFrameSet::Cell *>(m_currentCell->frameSet());
    KWTextFrameSet *textframeSet = dynamic_cast<KWTextFrameSet *>(m_currentCell->frameSet());
    bool moveToOtherCell = true;
    if(textframeSet)
    {
        // don't move to an adjacent cell when we are selecting text
        KoTextDocument * textdoc = textframeSet->textDocument();
        if(textdoc->hasSelection( KoTextDocument::Standard ))
            moveToOtherCell=false;
    }
    KWTableFrameSet::Cell *fs = 0L;

    bool tab=false; // No tab key pressed
    if(moveToOtherCell)
    {
        switch( e->key() ) {
            case Qt::Key_Up:
            {
                if(!(static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor()->parag()->prev())
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    int row = cell->firstRow() - 1;
                    int col = cell->firstColumn();
                    if (row < 0) {  // Wrap at top of table
                        col--; // Goes to column on the left
                        row = tableFrame->getRows() - 1;
                    }
                    if (col < 0) { // It was the first column
                        // Maybe exit the table instead?
                        col = tableFrame->getColumns() - 1;
                        row = tableFrame->getRows() - 1;
                    }
                    fs=tableFrame->cell(row,col);
                    // Not needed. cell gives us the right one already
                    //if (fs && fs->firstRow() != static_cast<unsigned int>(row)) { // Merged cell
                    //    fs=tableFrame->cell( row - fs->rowSpan() + 1, col );
                    //}
                }
            }
            break;
            case Qt::Key_Down:
            {
                if(!(static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor()->parag()->next())
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    unsigned int row = cell->rowAfter();
                    unsigned int col = cell->firstColumn();
                    if(row >= tableFrame->getRows()) { // Wrap at bottom of table
                        row=0;
                        col++; // Go to next column
                    }
                    if(col >= tableFrame->getColumns()) { // It was the last one
                        // Maybe exit the table instead?
                        col=0;
                        row=0;
                    }
                    fs=tableFrame->cell(row,col);
                    Q_ASSERT( fs );
                    Q_ASSERT( fs->firstRow() == row ); // We can't end up in the middle of a merged cell here.
                }
            }
            break;
            case Qt::Key_Backtab:
                tab=true;
                if (e->state() & Qt::ControlModifier)
                    break; // Break if tab was pressed with Control (in *any* key combination)
                // Do not break
            case Qt::Key_Left:
            {
                KoTextCursor *cur = (static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor();
                if ( tab || (!cur->parag()->prev()&&cur->index()==0) )
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    int row=cell->firstRow();
                    int col=cell->firstColumn() - 1;
                    if(col < 0) { // Wrap at first column
                        col = (int)tableFrame->getColumns()-1;
                        row--; // Go up
                    }
                    if(row < 0) { // It was the first row
                        // Maybe exit the table instead?
                        col = (int)tableFrame->getColumns()-1;
                        row = (int)tableFrame->getRows()-1;
                    }
                    fs=tableFrame->cell(row,col);
                    // Not needed. cell gives us the right one already
                    //if(fs && (int)fs->m_col != col) { // Merged cell
                    //    fs=tableFrame->cell( row, col - fs->columnSpan() + 1 );
                    //}
                }
            }
            break;
            case Qt::Key_Tab:
                tab=true;
                if (e->state() & Qt::ControlModifier)
                    break; // Break if tab was pressed with Control (in *any* key combination)
                // Do not break
            case Qt::Key_Right:
            {
                KoTextCursor *cur = (static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor();
                if( tab || (!cur->parag()->next()&&cur->index()==cur->parag()->string()->length()-1) )
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    unsigned int row = cell->firstRow();
                    unsigned int col = cell->columnAfter();
                    if(col >= tableFrame->getColumns()) { // Wrap after last column
                        col = 0;
                        row++; // Go down one row
                    }
                    if(row >= tableFrame->getRows()) { // It was the last row
                        // Maybe exit the table instead?
                        col = 0;
                        row = 0;
                    }
                    fs=tableFrame->cell(row,col);
                    Q_ASSERT( fs );
                    Q_ASSERT( fs->firstRow() == row ); // We can't end up in the middle of a merged cell here.
                }
            }
            break;
        }
    }
    if ( fs )
    {
        //don't switch to a protected cell protected when cursor in protected areas was disabled.
        if ( fs->textObject()->protectContent() && !tableFrameSet()->kWordDocument()->cursorInProtectedArea())
            return;
        setCurrentCell( fs );
    }
    else if ( textframeSet )
    {
        if ( !textframeSet->textObject()->protectContent() )
        {
            if (tab && (e->state() & Qt::ControlModifier) )
            {
                QKeyEvent event(QEvent::KeyPress, Qt::Key_Tab, 9, 0, QChar(9));
                m_currentCell->keyPressEvent( &event );
            }
            else
                m_currentCell->keyPressEvent( e );
        }
        else if(e->text().length() > 0)
            KMessageBox::information(0L, i18n("Read-only content cannot be changed. No modifications will be accepted."));
    }
}

void KWTableFrameSetEdit::keyReleaseEvent( QKeyEvent * e )
{
    if ( m_currentCell )
        m_currentCell->keyReleaseEvent( e );
}

void KWTableFrameSetEdit::inputMethodEvent( QInputMethodEvent* e )
{
    if ( m_currentCell )
        m_currentCell->inputMethodEvent( e );
}

void KWTableFrameSetEdit::dragMoveEvent( QDragMoveEvent * e, const QPoint &n, const KoPoint &d )
{
    kDebug(32004)<<"m_currentCell :"<<m_currentCell<<endl;
    if ( m_currentCell )
    {
        KWFrameSet *fs = tableFrameSet()->cellByPos( d.x(), d.y() );
        kDebug(32004)<<"fs :"<<fs <<endl;
        if(fs && fs != m_currentCell->frameSet())
            setCurrentCell(fs, false);
        if(m_currentCell)
            m_currentCell->dragMoveEvent( e, n, d );
    }
    else
    {
        setCurrentCell( d );
        kDebug(32004)<<"after m_currentCell :"<<m_currentCell<<endl;
        if(m_currentCell)
            m_currentCell->dragMoveEvent( e, n, d );
    }
}

void KWTableFrameSet::Row::addCell( Cell *cell )
{
    if ( m_cellArray.size() < cell->columnAfter())
        m_cellArray.resize( cell->columnAfter() );
    for ( uint col = cell->firstColumn() ; col < cell->columnAfter(); ++col )
        m_cellArray.insert( col, cell );
}

void KWTableFrameSet::Row::removeCell( Cell* cell )
{
    for ( uint col = cell->firstColumn() ; col <  cell->columnAfter(); ++col )
        m_cellArray.remove( col );
}

template<>
KWTableFrameSet::Cell*
KWTableFrameSet::TableIterator<KWTableFrameSet::VISIT_CELL>::operator++()
{
    if(!m_cell) return 0;

    Cell *ret = m_cell;

    do{
        // check for end of row first
        if(m_table->cell(m_row,m_col)->lastColumn() >= m_limit[RIGHT] ) {
            // now check for end of column
            if (m_row >= m_limit[LOW]){
                // at end of traversal
                m_cell = 0;
                break;
            }
            else {
                // goto first grid position in next row
                m_row += 1;
                m_col = m_limit[LEFT];
            }
        }
        else {
            // goto next cell in row
            m_col = m_table->cell(m_row, m_col)->columnAfter();
        }

        m_cell = m_table->cell(m_row,m_col);
    } while( m_cell && !m_cell->isFirstGridPosnFast(m_row,m_col) );

    return ret;
}

template<>
KWTableFrameSet::Cell*
KWTableFrameSet::TableIterator<KWTableFrameSet::VISIT_GRID>::operator++()
{
    if(!m_cell) return 0;

    Cell *ret = m_cell;
    // check for end of row
    if(m_col == m_limit[RIGHT]) {
        if(m_row == m_limit[LOW]) { // end of traversal
            m_row = 0;
            m_col = 0;
            m_cell = 0;
        }
        else { // go to next row
            m_row += 1;
            m_col = m_limit[LEFT];
            m_cell = m_table->cell(m_row, m_col);
        }
    }
    else { // move to next cell in row
        m_col += 1;
        m_cell = m_table->cell(m_row, m_col);
    }

    return ret;
}

KWTableFrameSet::MarkedIterator::MarkedIterator(KWTableFrameSet *table) :
    GridIter(table)
{
    // clear all the cell marks
    for(GridIter cell(table); cell; ++cell)
        cell->clearMark();

    if ( current() ) {
//      kDebug() << "MarkedIterator: visit: "
//          << QString("| 0x%1 ").arg((unsigned long)current(), 0, 16) << endl;
        current()->setMark();
    }
}

KWTableFrameSet::Cell *
KWTableFrameSet::MarkedIterator::operator++()
{
    Cell *ret = GridIter::operator++();

    while ( current() && current()->marked() ) {
        GridIter::operator++();
    }
    if ( current() ) {
//      kDebug() << "MarkedIterator: visit: "
//          << QString("| 0x%1 ").arg((unsigned long)current(), 0, 16) << endl;
        current()->setMark();
    }
    return ret;
}

template<>
KWTableFrameSet::TableIterator<KWTableFrameSet::CHECKED>::TableIterator(KWTableFrameSet *table):
    m_table(table)
{
    Q_ASSERT(m_table);
    set_limits(0, (int)m_table->getColumns() - 1, 0, (int)m_table->getRows() - 1);

    Cell *c = 0;
    for(uint i = m_limit[HIGH]; i <= m_limit[LOW]; ++i)
        for(uint j = m_limit[LEFT]; j <= m_limit[RIGHT]; ++j) {

            c = m_table->cell(i,j);
            if(c) c->clearMark();
        }
    toFirstCell();
}

template<>
KWTableFrameSet::Cell*
KWTableFrameSet::TableIterator<KWTableFrameSet::CHECKED>::operator++ ()
{

    Cell *ret = m_cell;
    if(!ret) return 0;

    ret->setMark();
    m_cell = 0;
    uint i = m_row; uint j = m_col;

    for(; i <= m_limit[LOW]; ++i) {

        for(j = 0; j <= m_limit[RIGHT]; ++j) {
            m_cell = m_table->cell(i,j);
            if( m_cell && !m_cell->marked() ){
                m_row = i; m_col = j;
                goto out;
            }
            else if(i == m_limit[LOW] && j == m_limit[RIGHT]){
                m_cell = 0;
                goto out;
            }
        }
    }

    out:
    return ret;
}

template<>
KWTableFrameSet::Cell*
KWTableFrameSet::TableIterator<KWTableFrameSet::CHECKED>::toFirstCell ()
{
    m_cell = 0;
    for(uint i = m_limit[HIGH]; i <= m_limit[LOW]; ++i)
        for(uint j = m_limit[LEFT]; j <= m_limit[RIGHT]; ++j) {
            m_cell = m_table->cell(i,j);
            if(m_cell) {
                m_row = i; m_col = j;
                goto out;
            }
        }

    out:
    return m_cell;
}

RemovedRow::RemovedRow() :
    m_row(0), m_index(0), m_rowHeight(0.0)
{

}

RemovedRow::~RemovedRow()
{
    // free cells as well ???
    delete m_row;
}

KWTableFrameSet::Row *RemovedRow::takeRow()
{
    Q_ASSERT(m_row);
    KWTableFrameSet::Row *ret = m_row;
    m_row = 0;
    return ret;
}

RemovedColumn::RemovedColumn()
    : m_column(), m_removed(), m_index(0), m_width(0), m_initialized(false){ }


#include "KWTableFrameSet.moc"
