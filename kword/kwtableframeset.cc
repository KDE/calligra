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

// ### TODO : multi page tables
// ### TODO: Doxygen-ify the description above
// ### TODO (JJ:) : change the QMAX into kMax and QMIN into kMin, especially if one parameter is a function

#include <kdebug.h>
#include <klocale.h>
#include "kwdoc.h"
#include "kwanchor.h"
#include "kwtableframeset.h"
#include "kwcanvas.h"
#include "kwcommand.h"
#include "kwviewmode.h"
#include "kwview.h"
#include "kwdrag.h"
#include <kotextobject.h> // for customItemChar !
#include <qpopupmenu.h>
#include <dcopobject.h>
#include "KWordFrameSetIface.h"
#include "KWordTableFrameSetIface.h"
#include <kmessagebox.h>
#include <qclipboard.h>


KWTableFrameSet::KWTableFrameSet( KWDocument *doc, const QString & name ) :
    KWFrameSet( doc )
{
    m_rows = m_cols = m_nr_cells = 0;
    m_name = QString::null;
    m_showHeaderOnAllPages = true;
    m_hasTmpHeaders = false;
    m_active = true;
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

    int oldPageNumber = getCell(0,0)->frame(0)->pageNum();
    // TODO multi-page case

    moveBy( dx, dy );

    if ( dx || dy ) {
        updateFrames();
        int newPageNumber = getCell(0,0)->frame(0)->pageNum();
        m_doc->updateFramesOnTopOrBelow( newPageNumber );
        if ( oldPageNumber != newPageNumber )
            m_doc->updateFramesOnTopOrBelow( oldPageNumber );
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
    //kdDebug(32004) << "KWTableFrameSet::createAnchor" << endl;
    return new KWAnchor( txt, this, frameNum );
}

void KWTableFrameSet::createAnchors( KoTextParag * parag, int index, bool placeHolderExists /*= false */ /*only used when loading*/,
                                     bool repaint )
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
    if ( repaint )
        emit repaintChanged( m_anchorTextFs );
}

void KWTableFrameSet::deleteAnchors()
{
    KWAnchor * anchor = findAnchor( 0 );
    kdDebug(32004) << "KWTableFrameSet::deleteAnchors anchor=" << anchor << endl;
    deleteAnchor( anchor );
}


void KWTableFrameSet::addCell( Cell* cell ) // called add but also used to 'update'
{
    m_rows = QMAX( cell->rowAfter(), m_rows );
    m_cols = QMAX( cell->colAfter(), m_cols );

    if ( m_rowArray.size() < cell->rowAfter() )
        m_rowArray.resize( cell->rowAfter() );
    for ( uint row = cell->firstRow() ;row < cell->rowAfter(); ++row )
    {
        if ( !m_rowArray[ row ] )
            m_rowArray.insert( row, new Row );
        m_rowArray[ row ]->addCell( cell );
    }
}

void KWTableFrameSet::removeCell( Cell* cell )
{
    for ( uint row = cell->firstRow() ; row < cell->rowAfter(); ++row )
        m_rowArray[ row ]->removeCell( cell );
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
    return KoRect(m_colPositions[0],    // left
                m_rowPositions[0],      // top
                m_colPositions.last()-m_colPositions[0], // width
                m_rowPositions.last()-m_rowPositions[0]);// height
}

double KWTableFrameSet::topWithoutBorder()
{
    double top = 0.0;
    for (uint i = 0; i < getCols(); i++)
    {
        KWTableFrameSet::Cell *cell = getCell( 0, i );
        top = QMAX( top, m_rowPositions[0] + cell->topBorder() );
    }
    return top;
}


double KWTableFrameSet::leftWithoutBorder()
{
    double left = 0.0;
    for (uint i=0; i < getRows(); i++)
    {
        KWTableFrameSet::Cell *cell = getCell( i, 0 );
        left = QMAX( left, m_colPositions[0] + cell->leftBorder() );
    }
    return left;
}

/* returns the cell that occupies row, col. */
KWTableFrameSet::Cell *KWTableFrameSet::getCell( unsigned int row, unsigned int col )
{
    if ( row < m_rowArray.size() && col < m_rowArray[row]->size() ) {
        Cell* cell = (*m_rowArray[row])[col];
        if ( cell )
            return cell;
    }
//    kdWarning() << getName() << " getCell " << row << "," << col << " => returning 0!" << kdBacktrace( 3 ) << endl;
//#ifndef NDEBUG
//    validate();
//    printArrayDebug();
//#endif
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

    if(activeCell->frame(0)->left() - activeCell->leftBorder() != m_colPositions[activeCell->firstCol()]) {
        // left border moved.
        col = activeCell->firstRow();
        difference = 0-(activeCell->frame(0)->left() - activeCell->leftBorder() - m_colPositions[activeCell->firstCol()]);
    }

    if(activeCell->frame(0)->right() - activeCell->rightBorder() !=
            m_colPositions[activeCell->lastCol()]) { // right border moved

        col = activeCell->colAfter();
        double difference2 = activeCell->frame(0)->right() + activeCell->rightBorder() - m_colPositions[activeCell->colAfter()];

        double moved=difference2+difference;
        if(moved > -0.01 && moved < 0.01) { // we were simply moved.
                col=0;
                difference = difference2;
            } else if(difference2!=0)
                difference = difference2;
    }

    m_redrawFromCol=getCols(); // possible reposition col starting with this one, done in recalcRows
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
        //if(activeCell) activeCell->frame(0)->setMinFrameHeight(0);
    }
    updateFrames();
    //kdDebug(32004) << "end KWTableFrameSet::recalcCols" << endl;
}

void KWTableFrameSet::recalcRows(int _col, int _row) {
    kdDebug(32004) << getName() << " KWTableFrameSet::recalcRows ("<< _col <<"," << _row << ")" << endl;
    //for(unsigned int i=0; i < m_rowPositions.count() ; i++) kdDebug(32004) << "row: " << i << " = " << m_rowPositions[i] << endl;


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
        // width of the table and the height is determined by joined cells, the minimum is one row.
        double minHeightOtherCols=0;    // The minimum height which the whole square of table cells can take
        double minHeightActiveRow=0;    // The minimum height our cell can get because of cells in his row
        double minHeightMyCol=0;        // The minimum height our column can get in the whole square
        unsigned int rowSpan = activeCell->rowSpan();
        unsigned int startRow = activeCell->firstRow();
        for (uint colCount = 0; colCount < getCols(); ++colCount )
        {
            // for each column
            unsigned int rowCount=startRow;
            double thisColHeight=0;     // the total height of this column
            double thisColActiveRow=0;  // the total height of all cells in this col, completely in the
                                        // row of the activeCell
            do { // for each row (under startRow)
                Cell *thisCell=getCell(rowCount,colCount);
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

                thisColHeight+=thisCell->frame(0)->minFrameHeight();
                thisColHeight+=thisCell->topBorder();
                thisColHeight+=thisCell->bottomBorder();

                if(thisCell->firstRow() >= activeCell->firstRow() && thisCell->rowAfter() <= activeCell->rowAfter())
                    thisColActiveRow+=thisCell->frame(0)->minFrameHeight();

                rowCount += thisCell->rowSpan();
            } while (rowCount < rowSpan+startRow);

            if(colCount >= activeCell->firstCol() &&
                  colCount < activeCell->colAfter() )
                minHeightMyCol = thisColHeight;
            else {
                minHeightOtherCols = QMAX(minHeightOtherCols, thisColHeight);
                minHeightActiveRow = QMAX(minHeightActiveRow, thisColActiveRow);
            }
        } // for each column

        bool bottomRow = (startRow+rowSpan == activeCell->rowAfter());
        if(!bottomRow) {
            Cell *bottomCell=getCell(startRow+rowSpan-1, activeCell->firstCol());
            bottomCell->frame(0)->setHeight(bottomCell->frame(0)->minFrameHeight() +
                    minHeightOtherCols - minHeightMyCol);
	    // ### RECURSE ###
            recalcRows(bottomCell->firstCol(), bottomCell->firstRow());
        }
        if(activeCell->frame(0)->minFrameHeight() > activeCell->frame(0)->height()) { // wants to grow
            activeCell->frame(0)->setHeight(activeCell->frame(0)->minFrameHeight());
            //kdDebug(32004) << activeCell->getName() << " grew to its minheight: " << activeCell->frame(0)->minFrameHeight() << endl;
        } else { // wants to shrink
            double newHeight=QMAX(activeCell->frame(0)->minFrameHeight(),minHeightActiveRow);
            if(bottomRow) // I'm a strech cell
                newHeight=QMAX(newHeight, minHeightOtherCols - (minHeightMyCol - activeCell->frame(0)->minFrameHeight()));
            activeCell->frame(0)->setHeight(newHeight);
            //kdDebug(32004) << activeCell->getName() << " shrunk to: " << newHeight << endl;
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
        QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
        QValueList<double>::iterator j = m_rowPositions.begin();
        double last=0.0;
        int lineNumber=-1;
        while(j != m_rowPositions.end()) {
            lineNumber++;
            if(pageBound!=m_pageBoundaries.end()) {
                if((int)*pageBound == lineNumber) { // next page
                    if(lineNumber >= (int)row) { // then delete line j
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
            if(lineNumber >= (int)row)  { // below changed row
                if(*(j)-last < s_minFrameHeight) // Never make it smaller then allowed!
                    difference += s_minFrameHeight - *(j) + last;
                last=*(j);
                kdDebug(32004) << "moving " << *(j) << " by " << difference << "; to " << (*j) + difference << endl;
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
    // kdDebug(32004) << "pageBottom; " << pageBottom << endl;
    while(++j!=m_rowPositions.end()) { // stuff for multipage tables.
        if(pageBound!=m_pageBoundaries.end() && *pageBound == lineNumber ) {
            if(*j > pageNumber * m_doc->ptPaperHeight() - m_doc->ptBottomBorder() ) { // next page marker exists, and is accurate...
                pageNumber++;
                pageBottom = pageNumber * m_doc->ptPaperHeight() - m_doc->ptBottomBorder();
                // kdDebug(32004) << "pageBottom; " << pageBottom << endl;
                untilRow=QMAX(untilRow, *pageBound);
                pageBound++;
            }
        }

//kdDebug() << "checking; " << lineNumber << ", " << (*j) << endl;
        if((*j) + diff > pageBottom) { // a row falls off the page.
//kdDebug(32004) << "row falls off of page"<< endl;
            untilRow = m_rows;
            bool hugeRow = false;
            unsigned int breakRow = lineNumber-1;
            // find out of no cells are spanning multiple rows meaning we have to break higher.
#if 0
    // ### TODO: I did not get a chance do debug this yet!  TZ.
            for(int i=0; i < getCols() ; i++) {
                kdDebug() << "i: " << i<< endl;
                Cell *c= getCell(breakRow, i);
                kdDebug() << "c: " << c->firstRow() << "," << c->m_col << " w: " << c->colSpan() << ", h: " << c->rowSpan() << endl;
                if(getCell(breakRow,i)->firstRow() < breakRow) {
                    breakRow = getCell(breakRow,i)->firstRow();
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
            //kdDebug(32004) << " pageBottom: " << pageBottom << " pageNumber=" << pageNumber << endl;
            if((int)pageNumber > m_doc->numPages()) {
                int num = m_doc->appendPage();
                kdDebug(32004) << "Have appended page: " << num << " (one page mode!)" << endl;
                m_doc->afterAppendPage( num );
            }
        }
        //if(diff > 0)  kdDebug(32004) << "   adding " << diff << ", line " << lineNumber << " " << *(j) <<" -> " << *(j)+diff << endl;
        if(diff > 0)
            (*j) = (*j) + diff;
        lineNumber++;

#if 0 // def SUPPORT_MULTI_PAGE_TABLES
        // Note: produces much ouput!
        int i = 1; // DEBUG
        for ( QValueList<double>::iterator itDebug = m_rowPositions.begin(); itDebug != m_rowPositions.end(); ++itDebug, ++i )
        {
            kdDebug(32004) << "m_rowPosition[" << i << "]= " << (*itDebug) << endl;
        }
#endif

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
    //Cell *cell;
    //bool setMinFrameSize= activeCell->frame(0)->isSelected();

    for(TableIter cell(this); cell; ++cell) {
        if((cell->rowAfter() > fromRow && cell->firstRow() < untilRow) || cell->colAfter() > m_redrawFromCol)
            position(cell, (cell==activeCell && cell->frame(0)->isSelected()));
    }
    m_redrawFromCol = getCols();

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
            kdDebug() << k_funcinfo << "no rows at counter=" << counter << " -> erasing" << endl;
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


    m_redrawFromCol = 0;
    for (TableIter cell(this); cell; ++cell) {
        if((cell->rowAfter() > fromRow && cell->firstRow() < untilRow)
           || cell->colAfter() > m_redrawFromCol)
            position(cell);
    }
    m_redrawFromCol = getCols();
    kdDebug(32004) << getName() << " KWTableFrameSet::recalcRows done" << endl;
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

void KWTableFrameSet::resizeColumn( unsigned int col, double x )
{
    kdDebug() << k_funcinfo << col << "," << x << endl;
    if ((col != 0) && (x - m_colPositions[ col-1 ] < s_minFrameWidth))
      m_colPositions[ col ] = m_colPositions[ col-1 ] + s_minFrameWidth;
    else
      if ((col != getCols()) && (m_colPositions[ col + 1 ] - x < s_minFrameWidth))
        m_colPositions[col] = m_colPositions[ col + 1 ] - s_minFrameWidth;
      else
        m_colPositions[ col ] = x;

    // move all cells right of 'col'
    for (TableIter cell(this); cell; ++cell) {
        if ( cell->colAfter() >= col ) {
            position(cell);
        }
    }
    recalcCols( col );
}

void KWTableFrameSet::resizeRow( unsigned int row, double y )
{
    kdDebug() << k_funcinfo << row << "," << y << endl;
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
    recalcRows( row );
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
   // Column positions..
    m_colPositions.clear();
    unsigned int cols=0;
    for (TableIter c(this); c; ++c)
        cols = QMAX(cols, c.current()->colAfter());
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
        kdDebug(32004) << "errorous table cell!! row:" << theCell->firstRow()
		<< ", col: " << theCell->firstCol() << endl;
        return;
    }
    double x = *m_colPositions.at(theCell->firstCol());
    double y = getPositionOfRow(theCell->firstRow());
    double width = (*m_colPositions.at(theCell->colAfter())) - x;
    double height  = getPositionOfRow(theCell->lastRow(), true) - y;

#if 0
    if(theCell->m_col==0) {
        kdDebug(32004) << "row "  << theCell->firstRow() << " has top: "
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
        for(TableIter cell(this);cell;++cell)
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
    for (TableIter i(this) ; i; ++i )
        i->frame( 0 )->setSelected( false );
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

void KWTableFrameSet::selectUntil( Cell *cell)
{
    unsigned int toRow = 0, toCol = 0;
    toRow = cell->lastRow();
    toCol = cell->lastCol();

    unsigned int fromRow = 0, fromCol = 0;
    getFirstSelected( fromRow, fromCol );
    if(cell->colSpan() != 1 )
        fromCol = QMIN(fromCol, cell->firstCol());
    if(cell->rowSpan() != 1 )
        fromRow = QMIN(fromRow, cell->firstRow());



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

    for ( TableIter cell(this); cell; ++cell ) {
        // check if cell falls completely in square.
        unsigned int row = cell->lastRow();
        unsigned int col = cell->lastCol();
        if(row >= fromRow && row <= toRow && col >= fromCol && col <= toCol)
        {
            cell->frame(0)->setSelected( true );
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
    Cell *selected = 0;
    bool one = false;
    for ( TableIter cell(this); cell; ++cell) {
        if(cell->frame(0)->isSelected())  {
            if(one)
                selected = 0;
            else{
                selected = (Cell*) cell;
		one  = true;
            }
        }
    }
    if(selected) {
        row = selected->firstRow();
        col = selected->firstCol();
        return true;
    }
    return false;
}

bool KWTableFrameSet::isRowSelected(uint row) {
    Q_ASSERT(row < getRows());
    Row *r = m_rowArray[row];

    // if just one cell of the row is not selected, the row is not selected
    for ( uint i = 0; i < r->size(); i++ ) {
        if ( !((*r)[i]->frame(0)->isSelected()) ) {
//              kdDebug() << "row " << row << " row is not selected" << endl;
                return false;
        }
    }
//  kdDebug() << "row " << row << " row is selected" << endl;
    return true;
}

bool KWTableFrameSet::isColSelected(uint col) {
    Q_ASSERT(col <= getCols());
    Row *r;
    // if just one cell of the col is not selected, the col is not selected
    for ( uint i = 0; i < getRows(); i++ ) {
	r = m_rowArray[i];
        if ( !((*r)[col]->frame( 0 )->isSelected()) ) {
//            kdDebug() << "column " << col << " column is not selected" << endl;
            return false;
        }
    }
//    kdDebug() << "column " << col << " column is selected" << endl;
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
    for ( TableIter i(this); i ; ++i ) {
        if (i->frame( 0 )->isSelected()) {
            row = i->firstRow();
            col = i->firstCol();
            return true;
        }
    }
    return false;
}

/* Delete all cells that are completely in this row.              */
void KWTableFrameSet::deleteRow( unsigned int row, RemovedRow &rr, bool _recalc)
{
    Q_ASSERT(row < m_rowArray.size());
    const unsigned int rowspan=1;

    double height= getPositionOfRow(row+rowspan-1,true) - getPositionOfRow(row);
    QValueList<double>::iterator tmp = m_rowPositions.at(row+rowspan);
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
                frames.remove( cell->frame(0) );
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
        recalcRows();
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
        else if(r->m_cellArray[cell->firstCol()] == cell.current()) {
                cell->setRowSpan(cell->rowSpan() + 1);
        }
        else if ( cell->firstRow() >= row ) {
            // move cell down
            cell->setFirstRow( cell->firstRow() + 1);
        }
   }

   // put back frames that were removed
   for(uint i = 0; i < rlen; i++){
       if( frames.findRef((*r)[i]->frame(0)) == -1 )
        frames.append( (*r)[i]->frame(0) );
   }

   // adjust row positions (ignores page boundaries!)a
   if(row == m_rows) {  //reinserting at bottom of table
       double d = m_rowPositions.last() + rr.height();
       m_rowPositions.append(d);
   }
   else {
       QValueList<double>::iterator top = m_rowPositions.at(row);
       QValueList<double>::iterator i = m_rowPositions.at(row+1);
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

    (void) _removeable;	// unused parameter
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
    QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
    while(pageBound != m_pageBoundaries.end() && (*pageBound) <= idx) {
        // Find out how many pages we already had.
        adjustment++;
        pageBound++;
    }

    // Move all rows down from newRow to bottom of page
    QValueList<double>::iterator tmp = m_rowPositions.at(idx);
    double newPos = *tmp + height;
    tmp++;
    m_rowPositions.insert(tmp, newPos);
    for(unsigned int i=idx+adjustment+2; i < m_rowPositions.count(); i++) {
        double &rowPos = m_rowPositions[i];
        kdDebug(32004) << "adjusting " << rowPos << " -> " << rowPos + height << endl;
        rowPos = rowPos + height;
        if(*pageBound == i) {
            untilRow= *pageBound;
            break;              // stop at pageBreak.
        }
    }

    for ( MarkedIterator cell(this); cell; ++cell) {
        if ( cell->firstRow() >= idx ) { // move all cells beneath the new row.
            cell->setFirstRow(cell->firstRow()+1);
        }
    }

    insertRowVector(idx, new Row);

    unsigned int i = 0;
    while(i < getCols()) {

        if(idx != 0 && (idx != m_rows)) {
            Cell *c = getCell(idx - 1, i);
            if( c == getCell(idx + 1, i) ) {
                m_rowArray[idx]->addCell(c);
                c->setRowSpan(c->rowSpan() + 1);
                i += c->colSpan();
                continue; // don't need to make a new cell
            }
        }

        KWFrame *theFrame = new KWFrame(copyRow->m_cellArray[i]->frame(0));
        Cell *newCell=new Cell( this, idx, i, QString::null );
        newCell->setColSpan( getCell(copyFromRow,i)->colSpan() );
        addCell(newCell);
        newCell->addFrame( theFrame, false );
        position(newCell);
        i += newCell->colSpan();
   }

    // Position all changed cells.

    m_rows = new_rows;
    validate();
    if ( recalc )
        finalize();
}

void KWTableFrameSet::deleteCol(uint col, RemovedColumn &rc)
{
    // keep these values in case we have to put it back
    if(!rc.m_initialized) {
        rc.m_index = col;
        rc.m_width = m_colPositions[col+1] - m_colPositions[col];
    }

    // move the colomn positions
    QValueList<double>::iterator tmp = m_colPositions.at(col+1);
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
        Cell *cell = getCell(i, col);

        if(!rc.m_initialized) {
            rc.m_column.append(cell);
            rc.m_removed.append(cell->colSpan() == 1);
        }

        if(cell->colSpan() == 1) { // lets remove it
            if(cell->firstRow() == i) {
                frames.remove( cell->frame(0) );
                m_nr_cells--;
            }
            m_rowArray[i]->m_cellArray.insert(col, 0);
        }
        else {                    // make cell span 1 less column
            if(cell->firstRow() == i) {
                cell->setColSpan( cell->colSpan() - 1 );
                position(cell);
            }
        }
    }

    // adjust cells in a later column
    for(; iter; ++iter) {
        if (iter->firstCol() > col ) {
            iter->setFirstCol( iter->firstCol() - 1);
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
    recalcCols();
    recalcRows();
}

void KWTableFrameSet::reInsertCol(RemovedColumn &rc)
{
    QValueList<double>::iterator tmp = m_colPositions.at(rc.m_index);

    tmp = m_colPositions.insert(tmp, *tmp);
    tmp++;
    while(tmp != m_colPositions.end()) {
        (*tmp) = (*tmp) + rc.m_width;
        tmp++;
    }

    // if a cell starts after the column we are inserting, it
    // must be moved to the right, except if it is going to
    // occury the reinserted column also.
    for ( MarkedIterator cell(this); cell ; ++cell ) {
        if ( cell->firstCol() >= rc.m_index &&
            (rc.m_column.at(cell->firstRow()) != cell.current())) {

            cell->setFirstCol(cell->firstCol() + 1);
        }
    }
    insertEmptyColumn(rc.m_index);
    m_cols++;

    for(uint i = 0; i < m_rows; ++i) {
        bool removed = rc.m_removed[i];
        Cell *cell = rc.m_column.at(i);
        if(i == cell->firstRow()) {
            if(removed) {
                cell->setColSpan(1);
                frames.append(cell->frame(0));
                m_nr_cells++;
            }
            else  {
               cell->setColSpan(cell->colSpan() + 1);
            }
            addCell(cell);
        }
    }

    validate();
    finalize();
}

void KWTableFrameSet::insertNewCol( uint idx, double width)
{
    QValueList<double>::iterator tmp = m_colPositions.at(idx);

    tmp = m_colPositions.insert(tmp, *tmp);
    tmp++;
    while(tmp!=m_colPositions.end()) {
        (*tmp)= (*tmp)+width;
        tmp++;
    }

    for ( MarkedIterator cell(this); cell ; ++cell ) {
        if ( cell->firstCol() >= idx) { // move all cells right of the new col.
            cell->setFirstCol(cell->firstCol() + 1);
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
            Cell *c = getCell(i, idx - 1);
            if( c == getCell(i, idx + 1) ) {
//                m_rowArray[i]->m_cellArray.insert(idx, c);
                c->setColSpan(c->colSpan() + 1);
                addCell(c);
                i += c->rowSpan() - 1;
                continue; // don't need to make a new cell
            }
        }

        Cell *newCell = new Cell( this, i, idx, QString::null );
        KWFrame *theFrame = new KWFrame(getCell(i, copyCol)->frame(0));
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
    Cell *firstCell = getCell(rowBegin, colBegin);
    if(colBegin==0 && rowBegin==0 && colEnd==0 && rowEnd==0)
    {

        if ( !getFirstSelected( rowBegin, colBegin ) )
            return 0L;

        firstCell = getCell(rowBegin, colBegin);
        colEnd = colBegin + firstCell->colSpan() - 1;
        rowEnd = rowBegin + firstCell->rowSpan() - 1;

        while(colEnd+1 <getCols()) { // count all horizontal selected cells
            Cell *cell = getCell(rowEnd,colEnd+1);
            if(cell->frame(0)->isSelected()) {
                colEnd += cell->colSpan();
            } else
                break;
        }

        while(rowEnd+1 < getRows()) { // count all vertical selected cells
            Cell *cell = getCell(rowEnd+1, colBegin);
            if(cell->frame(0)->isSelected()) {
                for(unsigned int j=1; j <= cell->rowSpan(); j++) {
                    for(unsigned int i=colBegin; i<=colEnd; i++) {
                        if(! getCell(rowEnd+j,i)->frame(0)->isSelected())
                            return 0L; // can't use this selection..
                    }
                }
                rowEnd += cell->rowSpan();
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
            }
        }
    }

    Q_ASSERT(firstCell);
    // update firstcell properties to reflect the merge
    firstCell->setColSpan(colEnd-colBegin+1);
    firstCell->setRowSpan(rowEnd-rowBegin+1);
    addCell(firstCell);
    position(firstCell);
    validate();
    firstCell->frame(0)->updateResizeHandles();

    m_doc->updateAllFrames(); // TODO: only fs->updateFrames() & m_doc->updateFramesOnTopOrBelow(pageNum)
    m_doc->repaintAllViews();
    return new KWJoinCellCommand( i18n("Join Cells"), this,colBegin,rowBegin, colEnd,rowEnd,listFrameSet,listCopyFrame);
}

KCommand *KWTableFrameSet::splitCell(unsigned int intoRows, unsigned int intoCols, int _col, int _row,QPtrList<KWFrameSet> listFrameSet, QPtrList<KWFrame>listFrame) {
    if(intoRows < 1 || intoCols < 1)
        return 0L;

    kdDebug(32004) << "KWTableFrameSet::splitCell" << endl;
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
    int rowsDiff = intoRows - cell->rowSpan();
    int colsDiff = ((int) intoCols) - cell->colSpan();

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

        // insert more rows into m_rowArray
        for(int i = 0; i < rowsDiff; ++i) {
            insertRowVector(row+i+1, new Row);
            m_rows++;
        }

       // m_rows += rowsDiff;
        //for(unsigned int i=0; i < m_rowPositions.count() ; ++i)
        //    kdDebug(32004) << "row " << i << ": " << m_rowPositions[i] << endl);
    }
    if(colsDiff >0) {
        double width = (m_colPositions[col+1] - m_colPositions[col])/intoCols;

        QValueList<double>::iterator iCol = m_colPositions.at(col);
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
        //    kdDebug(32004) << "col " << i << ": " << m_colPositions[i] << endl);
        //m_cols += colsDiff;
    }

    // adjust cellspan and rowspan on other cells.
    for (CheckedIter i(this); i ; ++i) {
        if(cell == i) continue;

        if(rowsDiff>0) {
            if(row >= i->firstRow()&& row < i->firstRow()+ i->rowSpan())
                i->setRowSpan(i->rowSpan() + rowsDiff);
            if(i->firstRow() > row) {
                i->setFirstRow(i->firstRow() + rowsDiff);
            //    theCell->frame(0)->setTop(theCell->frame(0)->top()+extraHeight);
            }
        }
        if(colsDiff>0) {
            if(col >= i->firstCol() && col < i->colAfter())
                i->setColSpan(i->colSpan() + colsDiff);
            if(i->firstCol() > col)
                i->setFirstCol(i->firstCol() + colsDiff);
        }
        /*if(extraHeight != 0 && theCell->firstRow()== row) {
            theCell->frame(0)->setHeight(theCell->frame(0)->height()+extraHeight);
        } */
        if ( rowsDiff > 0 || colsDiff > 0 ) // something changed?
            addCell( i ); // update arrays
    }

    int i=0;
    KWFrame *firstFrame = cell->frame(0);
    // create new cells
    for (unsigned int y = 0; y < intoRows; y++) {
        for (unsigned int x = 0; x < intoCols; x++){
            if(x==0 && y==0)
                continue; // the orig cell takes this spot.

            Cell *lastFrameSet=0L;

            if(listFrameSet.isEmpty())
            {
                lastFrameSet= new Cell( this, y + row, x + col );
            }
            else
            {
                lastFrameSet = static_cast<KWTableFrameSet::Cell*> (listFrameSet.at(i));
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
                lastFrameSet->setRowSpan(lastFrameSet->rowSpan() - rowsDiff);
            if(colsDiff <0 && x==0)
                lastFrameSet->setColSpan(lastFrameSet->colSpan() - colsDiff);

            addCell( lastFrameSet );
            position(lastFrameSet);
        }
    }



    // set new row and col-span. Use intermediate ints otherwise we get strange results as the
    // intermediate result could be negative (which goes wrong with unsigned ints)
    int r = (cell->rowSpan() +1) - intoRows;
    if(r < 1) r=1;
    cell->setRowSpan(r);

    int c = (cell->colSpan() + 1) - intoCols;
    if(c < 1)  c=1;
    cell->setColSpan(c);

    position(cell);
    addCell(cell);
    validate();
    firstFrame->setSelected(true);
    firstFrame->createResizeHandles();

    finalize();

    return new KWSplitCellCommand(i18n("Split Cells"),this,col,row,intoCols, intoRows);
}

void KWTableFrameSet::viewFormatting( QPainter &/*painter*/, int )
{
}

void KWTableFrameSet::validate()
{
    for(CheckedIter cell(this); cell; ++cell) {
        if(cell->colSpan() == 0 || cell->rowSpan() == 0)  {
            kdDebug(32004) << " KWTableFrameSet::validate(): zero dimension" << endl;
            kdDebug(32004) << cell->firstRow() << " " << cell->firstCol() << " " << cell->rowSpan()
                << " " << cell->colSpan() << endl;
        }

        for(uint i = cell->firstRow(); i < cell->rowAfter(); ++i) {

             for(uint j = cell->firstCol(); j < cell->colAfter(); ++j) {
                  if( getCell(i,j) != cell.current() ) {

                      QString str = QString("| 0x%1 ").arg( (unsigned long)cell.current(), 0, 16 );
                      kdDebug(32004) << " KWTableFrameSet::validate() failed " << endl;
                      kdDebug(32004) << "at row:  "<< i << " col: "<< j << " cell: "<<  str << endl;
                      kdDebug(32004) << cell->firstRow() << " " << cell->firstCol() << " " << cell->rowSpan()
                           << " " << cell->colSpan() << endl;
                      //printArrayDebug();
                  }
             }
        }
    }
}

// not sure where this method is used, it will be fixed soon
bool KWTableFrameSet::contains( double mx, double my ) {

    // The default implementation looks into each cell, should be good enough (DF)
    return KWFrameSet::contains( mx, my );
/*
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
            if((cell->colAfter() < getCols()-1) || (cell->firstCol() < getCols()-1) )
                cell = getCell(cell->firstRow(),  getCols()-1);
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
*/
}

MouseMeaning KWTableFrameSet::getMouseMeaning( const QPoint &nPoint, int keyState )
{
    MouseMeaning m = MEANING_MOUSE_SELECT;
    KoPoint docPoint = m_doc->unzoomPoint( nPoint );
    double mx = docPoint.x();
    double my = docPoint.y();
    // See if we're over a frame border
    KWFrame * frame = frameByBorder( nPoint );
    if ( frame )
    {
        double hs = 6; // horizontal snap zone (in pt)
        double vs = 6; // vertical snap zone (in pt)
        // (see KWFrame::getMouseMeaning)

        if ( QABS( frame->x() - mx ) < hs/2
             && my >= frame->y() && my <= frame->bottom() )
            return MEANING_RESIZE_COLUMN;
        if ( QABS( frame->right() - mx ) < hs/2
             && my >= frame->y() && my <= frame->bottom() )
            return MEANING_RESIZE_COLUMN;
        if ( QABS( frame->y() - my ) < vs/2
             && mx >= frame->x() && mx <= frame->right() )
            return MEANING_RESIZE_ROW;
        if ( QABS( frame->bottom() - my ) < vs/2
             && mx >= frame->x() && mx <= frame->right() )
            return MEANING_RESIZE_ROW;
    }
    frame = frameAtPos( docPoint.x(), docPoint.y() );
    if ( frame == 0L )
        return MEANING_NONE;

    // Found a frame under the cursor
    // Ctrl -> select
    if ( keyState & ControlButton )
        return m;
    // Shift _and_ at least a frame is selected already
    // (shift + no frame selected is used to select text)
    if ( (keyState & ShiftButton) && (m_doc->getFirstSelectedFrame() != 0L) )
        return m;

    return MEANING_MOUSE_INSIDE_TEXT;
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

    QPtrListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
    {
        QRect outerRect( viewMode->normalToView( frameIt.current()->outerRect(viewMode) ) );
        //kdDebug(32004) << "KWTableFrameSet::createEmptyRegion outerRect=" << DEBUGRECT( outerRect )
        //          << " crect=" << DEBUGRECT( crect ) << endl;
        outerRect &= crect;
        if ( !outerRect.isEmpty() )
            emptyRegion = emptyRegion.subtract( outerRect );
    }
}

void KWTableFrameSet::drawBorders( QPainter& painter, const QRect &crect, KWViewMode *viewMode ) {

    /*  Draw the borders on top of the lines stores in the m_rowPositions and m_colPositions arrays.
     *  check the relevant cells for borders and thus line thickness.
     *  We move the outer lines (on row==0 and col==0 plus on col=getCols() etc) a bit so they will stay
     *  inside the boundary of the table!
     */
    painter.save();
    QPen previewLinePen( QApplication::palette().color(  QPalette::Active, QColorGroup::Mid ) );
    QColor defaultBorderColor = KoTextFormat::defaultTextColor( &painter );
    const int minborder = 1;
    bool drawPreviewLines = viewMode && viewMode->drawFrameBorders();

    // *** draw horizontal lines *** //
    unsigned int row=0;
    QValueList<unsigned int>::iterator pageBound = m_pageBoundaries.begin();
    for (unsigned int i=0 ; i < m_rowPositions.count() ; i++) {
        //kdDebug(32004) << "Horizontal line code. i: " << i << endl;
        bool bottom=false;
        if( (pageBound!=m_pageBoundaries.end() && (*pageBound) == row)
            || i == m_rowPositions.count()-1)
            bottom=true;  // at end of page or end of table draw bottom border of cell.

        const KoBorder *border=0;
        double startPos =0;
        for(unsigned int col=0; col <= getCols();) {
            //kdDebug(32004) << "bottom=" << bottom << " row=" << row << " col=" << col << endl;
            Cell *cell = col < getCols() ? getCell(bottom?row-1:row, col) : 0;
            //if(cell) kdDebug(32004) << "cell (" << cell->firstRow()<< "," << cell->firstCol() << ")" << endl;
            //else kdDebug(32004) << "cell: " << cell << endl;

            if(cell && cell->firstRow() != (bottom?row-1:row))
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
                        if ( row > 0 ) {
                            Cell *c = getCell(row-1, col);
                            if(c) offset=QMAX(offset, c->leftBorder());
                        }
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
                        if ( row > 0 ) {
                            Cell *c = getCell(row-1, col);
                            if(c) offset=QMAX(offset, c->leftBorder());
                        }
                    }
                    startPos = m_colPositions[col] - offset;
                }
            }
            col += cell ? cell->colSpan() : 1;
        }
        if(pageBound!=m_pageBoundaries.end() && (*pageBound) == row)
            pageBound++;
        else
            row++;
    }

    // *** draw vertical lines *** //
    for (unsigned int col=0 ; col < m_colPositions.count(); col++) {
        //kdDebug(32004) << "Vertical line code. col: " << col << endl;
        bool right = false;
        if(col == m_colPositions.count()-1)
            right = true; // draw right border of cell.
        int cellColumn = right?col-1:col; // the column we'll be looking for in the loop below
        Q_ASSERT( cellColumn >= 0 );

        const KoBorder *border = 0;
        int startRow = -1;
        for(unsigned int row=0; row <= getRows();) {
            //kdDebug(32004) << "row=" << row << " cellColumn=" << cellColumn << endl;
            Cell *cell = row < getRows() ? getCell(row, cellColumn) : 0;

            //kdDebug(32004) << "Drawing vert. Line for cell row: " << row << " col: " << cellColumn << endl;
            if(cell && cell->firstCol() != (uint)cellColumn)
                cell=0;

#if 0
            kdDebug() << "Condition: startRow:" << (startRow!=-1) << endl;
            if ( startRow != -1 )  {
                Q_ASSERT( border );
                kdDebug() << "Other conditions: cell:" << !cell << endl;
                kdDebug() << " or last row:" << ( row == ( int )getRows() ) << endl;
                if ( cell )
                    kdDebug() << "Different border:" <<
                ( ( right && cell->frame(0)->rightBorder() != *border) ||
                ( !right && cell->frame(0)->leftBorder() != *border) )
			    << endl;
            }
#endif
            // Draw when something changed (different kind of border) or we're at the end
            // This code could be rewritten in a more QRT-like way
            // (iterate and compare with next, instead of the startRow/cell/border hack...)
            if(startRow != -1 &&
               (!cell || row == getRows() ||
                ( right && cell->frame(0)->rightBorder() != *border) ||
                ( !right && cell->frame(0)->leftBorder() != *border) )
                ) {
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
                    //kdDebug(32004) << "Drawing from topRow=" << topRow << endl;
                    do { // draw minimum of one line per page.
                        while( pageBound != m_pageBoundaries.end() && *(pageBound) < topRow )
                            pageBound++;

                        unsigned int bottomRow;
                        if(pageBound == m_pageBoundaries.end())
                            bottomRow = m_rowPositions.count()-1;
                        else
                            bottomRow = *(pageBound++);

                        //kdDebug(32004) << "from: " << topRow << " to: " << QMIN((uint)row, bottomRow) << endl;
                        //kdDebug(32004) << "from: " << m_rowPositions[topRow] << " to: " << m_rowPositions[QMIN((uint)row, bottomRow)] << endl;
                        double offset=0.0;
                        if(border->width() > 0) {
                            //kdDebug(32004) << "looking at topRow=" << topRow << " col=" << col << endl;
                            Cell *c=getCell(topRow,col);
                            if(c) offset=c->topBorder();
                            if ( col > 0 ) {
                                c=getCell(topRow,col-1);
                                if(c) offset=QMAX(offset,c->topBorder());
                            }
                            if(topRow==0) offset=0.0;
                        }
                        double top=m_rowPositions[topRow]-offset;

                        unsigned int toRow=QMIN((uint)row,bottomRow);
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
                            if(border->width() <= 0)
                                painter.setPen( previewLinePen );
                            else {
                                int borderWidth = KoBorder::zoomWidthX( border->width(), m_doc, minborder );
                                painter.setPen(KoBorder::borderPen( *border, borderWidth, defaultBorderColor ));
                            }
                            //kdDebug(32004) << "drawBorders(): painter.drawVerticalLine(" << line.left() << ","  << line.top() << "," <<  line.right() << ","  << line.bottom() << ")\n";
                            painter.drawLine( line.left(), line.top(), line.right(), line.bottom());
                        }

                        topRow=bottomRow+1;
                    } while(topRow < (uint)row && topRow != m_rowPositions.count());
                } // end "if border to be drawn"

                // reset startRow
                startRow = -1;
            }

            if(cell && startRow == -1) {
                startRow = row;
                if(right)
                    border = &(cell->frame(0)->rightBorder());
                else
                    border = &(cell->frame(0)->leftBorder());
                //kdDebug(32004) << "startRow set to " << row << endl;
            }
            row += cell ? cell->rowSpan() : 1;
            //kdDebug(32004) << "End of loop, row=" << row << endl;
        }
    }

#if 0
    if(drawPreviewLines) {
        QPen minsizeLinePen( red );
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
#endif

    painter.restore();
}

void KWTableFrameSet::drawContents( QPainter * painter, const QRect & crect,
                                    const QColorGroup & cg, bool onlyChanged, bool resetChanged,
                                    KWFrameSetEdit * edit, KWViewMode * viewMode )
{
    for (TableIter cell(this) ; cell ; ++cell)
    {
        if (edit)
        {
            KWTableFrameSetEdit * tableEdit = static_cast<KWTableFrameSetEdit *>(edit);
            if ( tableEdit->currentCell() && ((Cell*) cell) == tableEdit->currentCell()->frameSet() )
            {
                cell->drawContents( painter, crect, cg, onlyChanged, resetChanged, tableEdit->currentCell(), viewMode );
                continue;
            }
        }
        cell->drawContents( painter, crect, cg, onlyChanged, resetChanged, 0L, viewMode );
    }
    drawBorders( *painter, crect, viewMode );
    //kdDebug(32004) << "drawContents()" << endl;
}

#if 0
void KWTableFrameSet::preparePrinting( QPainter *painter, QProgressDialog *progress, int &processedParags )
{
    for (TableIter cell(this) ; cell ; ++cell)
        cell->preparePrinting( painter, progress, processedParags );
    }
}
#endif

QDomElement KWTableFrameSet::save( QDomElement &parentElem, bool saveFrames ) {
    // When saving to a file, we don't have anything specific to the frameset to save.
    // Save the cells only.
    for (TableIter cell(this) ; cell ; ++cell)
        cell->save(parentElem, saveFrames);
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
    if(_rows <0) _rows = 1;
    int _cols = KWDocument::getAttribute( framesetElem, "cols", 1 );
    if(_cols <0) _cols = 1;

    // m_rowPositions / m_colPositions could be QMemArrays...
    while(m_rowPositions.count() <= static_cast<unsigned int>(row + _rows + m_pageBoundaries.count())) {
        m_rowPositions.append(0);
    }
    while(m_colPositions.count() <= static_cast<unsigned int>(_col + _cols)) {
        m_colPositions.append(0);
    }

    Cell *cell = new Cell( this, row, _col, QString::null /*unused*/ );
    QString autoName = cell->getName();
    //kdDebug(32004) << "KWTableFrameSet::loadCell autoName=" << autoName << endl;
    cell->load( framesetElem, loadFrames );
    cell->setRowSpan(_rows);
    cell->setColSpan(_cols);
    addCell( cell ); // rowSpan()/m_cols have changed -> update array

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
        if(*tmp == 0)
            (*tmp) = cell->frame(0)->top();
        else {
            if (static_cast<int>(*tmp/m_doc->ptPaperHeight()) < static_cast<int>(cell->frame(0)->top()/m_doc->ptPaperHeight())) {
                kdDebug(32004) << "This cell is on a new page" << endl;
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
        if(*tmp == 0)
            (*tmp) = cell->frame(0)->bottom();
        else { // untested...
            if (static_cast<int>(*tmp/m_doc->ptPaperHeight()) > static_cast<int>(cell->frame(0)->top()/m_doc->ptPaperHeight())) {
                kdDebug(32004) << "next cell is on a new page" << endl;
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

    if ( m_rowPositions.count() != m_rows + 1 ) {
        kdDebug() << getName() << " loadCell: m_rowPositions=" << m_rowPositions.count() << " m_rows= " << m_rows << endl;
    }
    return cell;
}

int KWTableFrameSet::paragraphs()
{
    int paragraphs = 0;
    for (TableIter cell(this) ; cell ; ++cell)
        paragraphs += cell->paragraphs();
    return paragraphs;
}

int KWTableFrameSet::paragraphsSelected()
{
    int paragraphs = 0;
    for (TableIter cell(this) ; cell ; ++cell)
        paragraphs += cell->paragraphsSelected();
    return paragraphs;
}

bool KWTableFrameSet::statistics( QProgressDialog *progress, ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words,
    ulong & sentences, ulong & syllables, ulong & lines, bool selected )
{
    for (TableIter cell(this) ; cell ; ++cell)
        if( ! cell->statistics( progress, charsWithSpace, charsWithoutSpace, words, sentences, syllables, lines, selected ) )
        {
            return false;
        }
    return true;
}

void KWTableFrameSet::finalize( ) {
    kdDebug(32004) << "KWTableFrameSet::finalize" << endl;

    for (TableIter cell(this) ; cell ; ++cell)
    {
        position( cell );
        cell->finalize();
    }

    recalcCols();
    recalcRows();
    KWFrameSet::finalize();
}

void KWTableFrameSet::layout()
{
    for (TableIter cell(this) ; cell ; ++cell)
        cell->layout();
}

void KWTableFrameSet::invalidate()
{
    for (TableIter cell(this) ; cell ; ++cell)
        cell->invalidate();
}

void KWTableFrameSet::setVisible( bool v )
{
    for (TableIter cell(this) ; cell ; ++cell)
        cell->setVisible( v );

    KWFrameSet::setVisible( v );
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

void KWTableFrameSet::addTextFrameSets( QPtrList<KWTextFrameSet> & lst, bool onlyReadWrite )
{
    for (TableIter cell(this) ; cell ; ++cell)
        if (!cell->textObject()->protectContent() || onlyReadWrite )
            lst.append(cell);
}

void KWTableFrameSet::setLeftBorder(KoBorder newBorder)
{
    for (TableIter cell(this) ; cell ; ++cell  ) {
        if (cell->frame( 0 )->isSelected()) {
            Cell *cellLeft = cell->firstCol() > 0 ? getCell(cell->firstRow(), cell->firstCol()-1) : 0;
            if(!(cellLeft && cellLeft->frame(0)->isSelected())) {
                cell->setLeftBorder(newBorder);
            }
        }
    }
}

void KWTableFrameSet::setTopBorder(KoBorder newBorder)
{
    for (TableIter cell(this) ; cell ; ++cell  ) {
        if (cell->frame( 0 )->isSelected()) {
            Cell *cellAbove = cell->firstRow() > 0 ? getCell(cell->firstRow()-1, cell->firstCol()) : 0;
            if(!(cellAbove && cellAbove->frame(0)->isSelected())) {
                cell->setTopBorder(newBorder);
            }
        }
    }
    recalcRows();
}

void KWTableFrameSet::setBottomBorder(KoBorder newBorder)
{
    for (TableIter cell(this) ; cell ; ++cell  ) {
        if (cell->frame( 0 )->isSelected()) {
            Cell *otherCell = cell->firstRow() < getRows() ? getCell(cell->firstRow()+1, cell->firstCol()) : 0;
            if(!(otherCell && otherCell->frame(0)->isSelected())) {
                cell->setBottomBorder(newBorder);
            }
        }
    }
    recalcRows();
}

void KWTableFrameSet::setRightBorder(KoBorder newBorder)
{
    for (TableIter cell(this) ; cell ; ++cell  ) {
        if (cell->frame( 0 )->isSelected()) {
            Cell *otherCell = cell->firstCol() < getCols() ? getCell(cell->firstRow(), cell->firstCol()+1) : 0;
            if(!(otherCell && otherCell->frame(0)->isSelected())) {
                cell->setRightBorder(newBorder);
            }
        }
    }
}

KCommand *KWTableFrameSet::setProtectContentCommand( bool _protect )
{
    KMacroCommand *macro = 0L;
    for (TableIter cell(this) ; cell ; ++cell  ) {
        if (cell->frame( 0 )->isSelected()) {
            if ( cell->protectContent() != _protect )
            {
                KWProtectContentCommand *cmd = new KWProtectContentCommand( i18n("Protect Content"), cell , _protect);
                cell->setProtectContent( _protect );
                if ( !macro )
                    macro = new KMacroCommand( i18n("Protect Content"));
                macro->addCommand( cmd );
            }
        }
    }
    return macro;
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
    for( TableIter cell(this) ; cell ; ++cell ) {
        cell->setZOrder();
    }

}

// TODO provide toPlainText() (reimplemented from KWFrameSet)

void KWTableFrameSet::convertTableToText()
{
    // TODO port to OASIS (see KWTextFrameSetEdit::newDrag)
    // and don't mess up the clipboard....

    // Copy/paste individual cells (without using the clipboard...)
    // into a hidden textframeset (or just textdoc)
    // then cut the whole text into a KoStore-in-QByteArray, return that.
    // In KWView we'd still build a command with delete_table+paste_text.
#if 0
    QDomDocument domDoc( "PARAGRAPHS" );
    QDomElement elem = domDoc.createElement( "PARAGRAPHS" );
    domDoc.appendChild( elem );
    QString text;
    for (TableIter cell(this); cell; ++cell)
    {
        cell->textDocument()->selectAll( KoTextDocument::Temp );
        text += cell->copyTextParag( elem, KoTextDocument::Temp );
        cell->textDocument()->removeSelection( KoTextDocument::Temp );
    }
    KWTextDrag *kd = new KWTextDrag( 0L );
    kd->setPlain( text );
    kd->setFrameSetNumber( -1 );
    kd->setKWord( domDoc.toCString() );
    kdDebug(32001) << "convertTableToText " << domDoc.toCString() << endl;
    QApplication::clipboard()->setData( kd );
#endif
}

#ifndef NDEBUG
void KWTableFrameSet::printDebug( KWFrame * theFrame )
{
    KWTableFrameSet::Cell *cell = dynamic_cast<KWTableFrameSet::Cell *>( theFrame->frameSet() );
    Q_ASSERT( cell );
    if ( cell ) {
        kdDebug(32004) << " |  |- row :" << cell->firstRow() << endl;
        kdDebug(32004) << " |  |- col :" << cell->firstCol() << endl;
        kdDebug(32004) << " |  |- rows:" << cell->rowSpan() << endl;
        kdDebug(32004) << " |  +- cols:" << cell->colSpan() << endl;
    }
}

void KWTableFrameSet::printArrayDebug() {
    kdDebug(32004) << " |  Row/Cell arrays" << endl;
    Q_ASSERT( m_rows == m_rowArray.size() );
    for ( unsigned int row = 0; row < m_rows; ++row )  {
        QString str = QString( " | Row %1: " ).arg( row );
        for ( unsigned int col = 0; col < getCols(); ++col )
            str += QString("| 0x%1 ").arg( (unsigned long)(*m_rowArray[row])[col], 0, 16 );
        kdDebug(32004) << str<< " |" << endl;
    }
}

void KWTableFrameSet::printDebug() {
    kdDebug(32004) << " |  Table size (" << m_rows << "x" << getCols() << ")" << endl;
    kdDebug(32004) << " |  col  " << 0 << ": " << m_colPositions[0] << endl;
    for(unsigned int i=1;i<m_colPositions.count(); ++i)
        kdDebug(32004) << " |    |  " << i << ": " << m_colPositions[i] << endl;
    kdDebug(32004) << " |  row  " << 0 << ": " << m_rowPositions[0] << endl;
    for(unsigned int i=1;i<m_rowPositions.count(); ++i)
        kdDebug(32004) << " |    |  " << i << ": " << m_rowPositions[i] << endl;

    printArrayDebug();
    KWFrameSet::printDebug();
}

#endif

// ===

KWTableFrameSet::Cell::Cell( KWTableFrameSet *table, unsigned int row, unsigned int col, const QString &/*name*/ ) :
    KWTextFrameSet( table->m_doc,
                    // Generate frameset name from table_name+row+col
                    i18n("Hello dear translator :), 1 is the table name, 2 and 3 are row and column", "%1 Cell %2,%3")
                    .arg( table->getName() ).arg(row).arg(col) )
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
             colAfter() > col );
}

void KWTableFrameSet::Cell::addFrame(KWFrame *_frame, bool recalc) {
    if(getGroupManager())
        getGroupManager()->addFrame(_frame, recalc);
    KWTextFrameSet::addFrame(_frame, recalc);
}

void KWTableFrameSet::Cell::frameDeleted( KWFrame* frm, bool recalc )
{
    if(getGroupManager())
        getGroupManager()->delFrame( frm, false, recalc );
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
    if(m_col+m_cols==grpMgr->getCols()) // right most cell
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
    if(rowAfter() == grpMgr->m_rows) // bottom most cell
        return b;
    return (b / 2);
}

void KWTableFrameSet::Cell::setLeftBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->leftBorder().width() - newBorder.width();
    f->setLeftBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && m_col!=0) {
        diff = diff / 2; // if not outer edge only use halve
        grpMgr->getCell(m_row, m_col-1)->setRightBorder(newBorder);
    }
    f->setLeft(f->left() - diff);
}

void KWTableFrameSet::Cell::setRightBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->rightBorder().width() - newBorder.width();
    f->setRightBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && m_col+m_cols!=grpMgr->getCols()) {
        diff = diff / 2; // if not outer edge only use halve
        grpMgr->getCell(m_row, m_col+1)->setLeftBorder(newBorder);
    }
    f->setRight(f->right() + diff);
}

void KWTableFrameSet::Cell::setTopBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->topBorder().width() - newBorder.width();
    f->setTopBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && m_row!=0) {
        diff = diff / 2; // if not outer edge only use halve
        grpMgr->getCell(m_row-1, m_col)->setBottomBorder(newBorder);
    }
    f->setTop(f->top() - diff);
}

void KWTableFrameSet::Cell::setBottomBorder(KoBorder newBorder) {
    KWFrame *f = frame(0);
    double diff = f->bottomBorder().width() - newBorder.width();
    f->setBottomBorder(newBorder);

    if((diff > 0.01 || diff < -0.01) && rowAfter() != grpMgr->m_rows) {
        diff = diff / 2; // if not outer edge only use halve
        grpMgr->getCell(m_row+1, m_col)->setTopBorder(newBorder);
    }
    f->setBottom(f->bottom() + diff);
}

void KWTableFrameSet::Cell::setZOrder()
{
    QPtrListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
    {
        (*frameIt)->setZOrder( kWordDocument()->maxZOrder( (*frameIt)->pageNum() ) + 1 );
    }
}

void KWTableFrameSet::Cell::drawContents( QPainter * painter, const QRect & crect,
        const QColorGroup & cg, bool onlyChanged, bool resetChanged,
        KWFrameSetEdit * edit, KWViewMode * viewMode )
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
    KWTextFrameSet::drawContents(painter, cellRect, cg, onlyChanged, resetChanged, edit, viewMode);
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
        m_currentCell->terminate(eraseSelection);
    delete m_currentCell;
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
            case QKeyEvent::Key_Up:
            {
                if(!(static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor()->parag()->prev())
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    int row = cell->firstRow() - 1;
                    int col = cell->firstCol();
                    if (row < 0) {  // Wrap at top of table
                        col--; // Goes to column on the left
                        row = tableFrame->getRows() - 1;
                    }
                    if (col < 0) { // It was the first column
                        // Maybe exit the table instead?
                        col = tableFrame->getCols() - 1;
                        row = tableFrame->getRows() - 1;
                    }
                    fs=tableFrame->getCell(row,col);
                    // Not needed. getCell gives us the right one already
                    //if (fs && fs->firstRow() != static_cast<unsigned int>(row)) { // Merged cell
                    //    fs=tableFrame->getCell( row - fs->rowSpan() + 1, col );
                    //}
                }
            }
            break;
            case QKeyEvent::Key_Down:
            {
                if(!(static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor()->parag()->next())
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    unsigned int row = cell->rowAfter();
                    unsigned int col = cell->firstCol();
                    if(row >= tableFrame->getRows()) { // Wrap at bottom of table
                        row=0;
                        col++; // Go to next column
                    }
                    if(col >= tableFrame->getCols()) { // It was the last one
                        // Maybe exit the table instead?
                        col=0;
                        row=0;
                    }
                    fs=tableFrame->getCell(row,col);
                    Q_ASSERT( fs );
                    Q_ASSERT( fs->firstRow() == row ); // We can't end up in the middle of a merged cell here.
                }
            }
            break;
            case QKeyEvent::Key_Backtab:
                tab=true;
                if (e->state() & QKeyEvent::ControlButton)
                    break; // Break if tab was pressed with Control (in *any* key combination)
                // Do not break
            case QKeyEvent::Key_Left:
            {
                KoTextCursor *cur = (static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor();
                if ( tab || (!cur->parag()->prev()&&cur->index()==0) )
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    int row=cell->firstRow();
                    int col=cell->firstCol() - 1;
                    if(col < 0) { // Wrap at first column
                        col = (int)tableFrame->getCols()-1;
                        row--; // Go up
                    }
                    if(row < 0) { // It was the first row
                        // Maybe exit the table instead?
                        col = (int)tableFrame->getCols()-1;
                        row = (int)tableFrame->getRows()-1;
                    }
                    fs=tableFrame->getCell(row,col);
                    // Not needed. getCell gives us the right one already
                    //if(fs && (int)fs->m_col != col) { // Merged cell
                    //    fs=tableFrame->getCell( row, col - fs->colSpan() + 1 );
                    //}
                }
            }
            break;
            case QKeyEvent::Key_Tab:
                tab=true;
                if (e->state() & QKeyEvent::ControlButton)
                    break; // Break if tab was pressed with Control (in *any* key combination)
                // Do not break
            case QKeyEvent::Key_Right:
            {
                KoTextCursor *cur = (static_cast<KWTextFrameSetEdit *>(m_currentCell))->cursor();
                if( tab || (!cur->parag()->next()&&cur->index()==cur->parag()->string()->length()-1) )
                {
                    KWTableFrameSet* tableFrame=tableFrameSet();
                    unsigned int row = cell->firstRow();
                    unsigned int col = cell->colAfter();
                    if(col >= tableFrame->getCols()) { // Wrap after last column
                        col = 0;
                        row++; // Go down one row
                    }
                    if(row >= tableFrame->getRows()) { // It was the last row
                        // Maybe exit the table instead?
                        col = 0;
                        row = 0;
                    }
                    fs=tableFrame->getCell(row,col);
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
    else
    {
        if ( !textframeSet->textObject()->protectContent() )
        {
            if (tab && (e->state() & QKeyEvent::ControlButton) )
            {
                QKeyEvent event(QEvent::KeyPress, QKeyEvent::Key_Tab, 9, 0, QChar(9));
                m_currentCell->keyPressEvent( &event );
            }
            else
                m_currentCell->keyPressEvent( e );
        }
        else
            KMessageBox::information(0L, i18n("Read-only content cannot be changed. No modifications will be accepted."));
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
            view->plugActionList( "tableactions", view->tableActions() );
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

void KWTableFrameSet::Row::addCell( Cell *cell )
{
    if ( m_cellArray.size() < cell->colAfter())
        m_cellArray.resize( cell->colAfter() );
    for ( uint col = cell->firstCol() ; col < cell->colAfter(); ++col )
        m_cellArray.insert( col, cell );
}

void KWTableFrameSet::Row::removeCell( Cell* cell )
{
    for ( uint col = cell->firstCol() ; col <  cell->colAfter(); ++col )
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
		if(m_table->getCell(m_row,m_col)->lastCol() >= m_limit[RIGHT] ) {
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
			m_col = m_table->getCell(m_row, m_col)->colAfter();
		}

		m_cell = m_table->getCell(m_row,m_col);
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
		if(m_row == m_limit[LOW]) {	// end of traversal
			m_row = 0;
			m_col = 0;
			m_cell = 0;
		}
		else {				// go to next row
			m_row += 1;
			m_col = m_limit[LEFT];
			m_cell = m_table->getCell(m_row, m_col);
		}
	}
	else {					// move to next cell in row
		m_col += 1;
		m_cell = m_table->getCell(m_row, m_col);
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
//      kdDebug() << "MarkedIterator: visit: "
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
//      kdDebug() << "MarkedIterator: visit: "
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
    set_limits(0, m_table->getCols() - 1, 0, m_table->getRows() - 1);

    Cell *c = 0;
    for(uint i = m_limit[HIGH]; i <= m_limit[LOW]; ++i)
        for(uint j = m_limit[LEFT]; j <= m_limit[RIGHT]; ++j) {

            c = m_table->getCell(i,j);
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
            m_cell = m_table->getCell(i,j);
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
            m_cell = m_table->getCell(i,j);
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



#include "kwtableframeset.moc"
