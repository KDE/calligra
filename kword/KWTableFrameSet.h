#ifndef KWTABLEFRAMESET_H
#define KWTABLEFRAMESET_H
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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

DESCRIPTION

    This file defines KWord tables. A table is a KWFrameSet, thus allowing its
    internal structure to be invisible to the rest of kword. This is useful
    since the internal structure of a table is itself a series of KWFrameSets
    (one for each cell).
*/

#include "KWFrame.h"
#include "KWTextFrameSet.h"

#include <q3ptrlist.h>
#include <q3valuevector.h>
#include <qobject.h>
#include <QString>
//Added by qt3to4:
#include <QDragLeaveEvent>
#include <Q3MemArray>
#include <QKeyEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <Q3ValueList>
#include <QDragEnterEvent>
#include <QMouseEvent>
#include <KoRichText.h>

class KWDocument;
class KWTableFrameSetEdit;
class QPainter;
class KWAnchor;
class KWordFrameSetIface;
class KWFrameViewManager;

class RemovedRow;
class RemovedColumn;

/**
 * @brief This class implements tables by acting as the manager for
 * the frame(set)s which make up the table cells.
 *
 * We have a cell structure which contains one frameset, because
 * of the nature of the table this frameset will always hold
 * exactly one frame. Therefore the terms cell, frameSet and frame
 * can be used to describe the same thing: one table-cell
 */
class KWTableFrameSet : public KWFrameSet
{
    Q_OBJECT
public:

    /**
     * Table cell
     * @note A cell can be any type of FrameSet, but for now, we only support text!
     */
    class Cell : public KWTextFrameSet
    {
        unsigned int m_row, m_col;
        unsigned int m_rows, m_cols;

        bool m_isJoinedCell;
        bool m_marker;
        void calcIfJoinedCell() {
            m_isJoinedCell = ( (m_rows > 1) || (m_cols > 1) );
        }

    public:
        /// constructor
        Cell( KWTableFrameSet *table, unsigned int row, unsigned int col, const QString & name = QString::null );
        Cell( KWTableFrameSet *table, const Cell &original );
        // destructor
        virtual ~Cell();

        uint firstRow() const { return m_row; }
        uint firstColumn() const { return m_col; }
        uint rowSpan() const { return m_rows; }
        uint columnSpan() const { return m_cols; }

        uint lastRow() const { return m_row + m_rows - 1; }
        uint lastColumn() const { return m_col + m_cols - 1; }
        uint rowAfter() const { return m_row + m_rows; }
        uint columnAfter() const { return m_col + m_cols; }

        void setFirstRow(uint row) { m_row = row; }
        void setFirstColumn(uint col) { m_col = col; }
        void setRowSpan(uint rows) {
            m_rows = rows;
            calcIfJoinedCell();
        }
        void setColumnSpan(uint cols) {
            m_cols = cols;
            calcIfJoinedCell();
        }

        bool isFirstGridPosn(uint row, uint col) const {
            return (row == m_row) && (col == m_col);
        }

        bool isFirstGridPosnFast(uint row, uint col) const {
                if(!m_isJoinedCell) return true;
                return (row == m_row) && (col == m_col);
        }
        virtual void addFrame(KWFrame *_frame, bool recalc = true);
        /// Called by deleteFrame when it really deletes a frame (remove=true), to remove it from the table too
        virtual void frameDeleted( KWFrame* frm, bool recalc );

        bool isAboveOrLeftOf( unsigned row, unsigned column ) const;
        bool containsCell( unsigned row, unsigned column ) const;

        double leftBorder();
        double rightBorder();
        double topBorder();
        double bottomBorder();

        void setLeftBorder(KoBorder newBorder);
        void setRightBorder(KoBorder newBorder);
        void setTopBorder(KoBorder newBorder);
        void setBottomBorder(KoBorder newBorder);
        /**
         * set zorder for all frames in this cell
         */
        void setZOrder();
        bool isJoinedCell() const { return m_isJoinedCell; }
        void clearMark() { m_marker = false; }
        void setMark() { m_marker = true; }
        bool marked() const { return m_marker; }

        /** Overloaded to not clear any preview borders. */
        void drawContents( QPainter *painter, const QRect & crect,
                const QColorGroup & cg, bool onlyChanged, bool resetChanged,
                KWFrameSetEdit * edit, KWViewMode * viewMode,
                KWFrameViewManager *frameViewManager );

    };
    friend class Cell;

    /// Represents a row, for direct access to cells with m_rowArray[row][column]
    class Row {
    public:
        Cell* operator[] ( uint i ) const { return i < size() ? m_cellArray[i] : 0; }
        uint size() const { return m_cellArray.size(); }
        uint count() const { return m_cellArray.count(); }

        void addCell( Cell *cell );
        void removeCell( Cell* cell );

        /**
         * @brief Array of cells in the row.
         *
         * If a cell has m_cols = N, N values in this array point to the same cell.
         * (Same thing is a cell from a row above has m_rows > 1)
         */
        Q3PtrVector< Cell > m_cellArray;

    };

    /** The three different types of TableIterators */
    enum VisitType {
        /**
         * This iterator visits each grid position once, ie every
         * location in the m_rowArray rows. When some cells are joined, this
         * iterator will visit those cells more than once.
         */
        VISIT_GRID = 1,

        /**
         * This iterator visits each cell in the table once, whether
         * or not some of the cells are joined. If you want to visit all the cells
         * fast and perform some read-only operation, this is the one to use.
         */
        VISIT_CELL = 2,
        /**
         * Like VISIT_CELL it also visits each cell once, but has some other benefits. Slower.
         */
        CHECKED = 3
    };

    /**
      All the TableIterator templates are the same, except for the pre-increment
      operator (operator++). There is a specialised version of this
      operator method for each iterator type:

      VISIT_GRID: This iterator visits each grid position once, ie every
      location in the m_rowArray rows. When some cells are joined, this
      iterator will visit those cells more than once.

      VISIT_CELL: This iterator visits each cell in the table once, whether
      or not some of the cells are joined. If you want to visit all the cells
      fast and perform some read-only operation, this is the one to use.

      CHECKED: Also visits each cell once, but has some other benefits. Slower.

      Note that all the iterators have restrictions on the state that the
      table data structures are in before and while they are being used.
      This includes m_rowArray, the Rows and Cells but not m_colPositions and
      m_rowPositions, as they are not used during the traversal.
      The conditions include:
      <UL>
      <LI>( A ) All positions in m_rowArray must be occupied by a valid cell, ie must
      not be null.</LI>
      <LI>( B ) The Cell instance variables (m_row, m_rows, etc) must correctly
      correspond to where the cells are in m_rowArray.</LI>
      <LI>( C ) The m_rows and m_cols instance variables of the table are also correct.</LI>
      </UL>
      Taken together, these conditions are pretty much equivalent to the
      validate() function passing. These conditions may not hold in the middle
      of a method when table data structures are being manipulated.

      This table shows what conditions are necessary for each type of iterator:

      <TABLE>
      <TR><TD>                           </TD><TD>A</TD><TD>B</TD><TD>C</TD></TR>
      <TR><TD>TableIterator\<VISIT_GRID\></TD><TD>x</TD><TD> </TD><TD>x</TD></TR>
      <TR><TD>TableIterator\<VISIT_CELL\></TD><TD>x</TD><TD>x</TD><TD>x</TD></TR>
      <TR><TD>TableIterator\<CHECKED\>   </TD><TD> </TD><TD> </TD><TD>x</TD></TR>
      <TR><TD>MarkedIterator             </TD><TD>x</TD><TD> </TD><TD>x</TD></TR>
      </TABLE>

      The only iterator that can be used when there are null positions in the
      table is the checked iterator.
      Note that both the Checked and Marked Iterators traverse the table twice,
      once to clear the m_marked members in the cells (done in constructor) and
      then again to do the actual traversal. Because they use m_marked, only one
      of these iterators can be used at once.

      Don't use these classes directly in code, use the provided typedefs. That
      makes it easy to do more crazy template stuff later on. :-)  Templates
      are used here rather than inheritance to avoid virtual call overhead.
    */
    template<int VisitStyle = VISIT_CELL>
    class TableIterator {
    public:
        /**
         * @param table The table to iterate over. The current item is set to the Cell
         * at row 0, column 0
         */
        TableIterator (KWTableFrameSet *table);

        Cell* toFirstCell ();
        void goToCell(Cell*);

        operator Cell* () const { return m_cell; }
        Cell * current() const;
        Cell * operator->() { return m_cell; }
        Cell * operator++ ();

    protected:
        KWTableFrameSet *m_table;
        void set_limits(uint left, uint right, uint high, uint low)
        {
            m_limit[LEFT] = left;
            m_limit[RIGHT] = right;
            m_limit[HIGH] = high;
            m_limit[LOW] = low;
        }
    private:

        Cell *m_cell;
        uint m_row;
        uint m_col;

        enum Direction {LEFT, RIGHT, HIGH, LOW};
        static const uint DIRECTION_SIZE = 4;
        uint m_limit[DIRECTION_SIZE];
    };

    typedef TableIterator<VISIT_CELL> TableIter;
    typedef TableIterator<VISIT_GRID> GridIter;
    typedef TableIterator<CHECKED> CheckedIter;

    /**
     * This iterator does not look at the Cell instance variables
     * during traversal, (except m_marker), so they can be safely
     * changed during the traversal. However, to the user it does
     * not visit every grid position, it visits each cell once.
     * (in spite of the fact that it inherits from a grid-visiting
     * iterator).
     * Only one MarkedIterator can be used at once.  See TableIterator
     */
    class MarkedIterator : public GridIter {
    public:
        MarkedIterator(KWTableFrameSet *table);
        Cell *operator++();     // overridden from base but not virtual

    };


    /** The type of frameset. Use this to differentiate between different instantiations of
     *  the framesets. Each implementation will return a different frameType.
     */
    virtual FrameSetType type() const { return FT_TABLE; }

    virtual KWordFrameSetIface* dcopObject();

    virtual void addTextFrameSets( Q3PtrList<KWTextFrameSet> & lst, bool onlyReadWrite =false  );

    /// constructor
    KWTableFrameSet( KWDocument *_doc, const QString & name );
    /// destructor
    virtual ~KWTableFrameSet();

    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * canvas );

    /**
     * @param emptyRegion The region is modified to subtract the areas painted, thus
     *                    allowing the caller to determine which areas remain to be painted.
     * @param crect  the clip rect, outside which nothing is important
     * @param viewMode the current view mode
     */
    virtual void createEmptyRegion( const QRect & crect, QRegion & emptyRegion, KWViewMode *viewMode );
    void drawBorders( QPainter& painter, const QRect &crect, KWViewMode *viewMode );
    virtual void drawContents( QPainter * painter, const QRect & crect,
                               const QColorGroup & cg, bool onlyChanged, bool resetChanged,
                               KWFrameSetEdit *edit, KWViewMode *viewMode,
                               KWFrameViewManager *frameViewManager );
    /// Dummy since we reimplement drawContents
    virtual void drawFrame(KWFrame *, QPainter *, const QRect &, const QRect&,
                           const QPoint&,
                           KWFrame *, const QColorGroup &, bool, bool,
                           KWFrameSetEdit *, KWViewMode *, bool ) {}

    // Frameset management
    Cell *cell( unsigned int row, unsigned int column ) const;
    Cell *cellByPos( double x, double y ) const;

    enum CellSize {
        TblAuto = 0,
        TblManual
    };

    /**
     * Calculate the absolute size of the complete table.
     *  From the first cell to the last, including page breaks et.
     *  @return KoRect which outlines the whole of the table.
     */
    KoRect boundingRect();

    /**
     * Layout all cells to fit inside the rect, cells will however use a minimum size, so
     * the table might end up bigger.
     */
    void setBoundingRect( KoRect rect, CellSize widthMode, CellSize heightMode );

    /**
     * Calculate the top postion of the cell(s) in the leftmost column
     * @return double table leftmost position
     */
    double topWithoutBorder();

    /**
     * Calculate the top postion of the cell(s) in the top row
     * @return double table top position
     */
    double leftWithoutBorder();

    /**
     *  change the width of the table, keeping the proportions of the cells
     * (if one is wider than the others, it is still wider after resize)
     */
    void resizeWidth( double width );

    /** resize and position all cells */
    void recalcCols(uint column, uint row);
    void recalcRows(uint column, uint row);

    /** move a column edge (i.e. col can be 0 to getCols()+1) */
    void resizeColumn( unsigned int col, double x );
    /** move a row edge (i.e. row can be 0 to getRows()+1) */
    void resizeRow( unsigned int row, double y );

    double columnSize( unsigned int col );
    double rowSize( unsigned int col );

    /** return the number of the column edge closest to x (between 0 and getCols()+1) */
    int columnEdgeAt( double x ) const;
    /** return the number of the row edge closest to x (between 0 and getRows()+1) */
    int rowEdgeAt( double y ) const;

    /** returns the number of rows */
    unsigned int getRows() const { return m_rows; }
    /** returns the number of columns */
    unsigned int getColumns() const { return m_cols; }

    /** returns the number of cells the table contains, this includes
     * temporary headers. */
    unsigned int getNumCells()const { return m_nr_cells; }


    /** move the whole of the table, this is mainly for anchored frames. */
    void moveBy( double dx, double dy );

    /** insert a row of new cells, use the getCols() call to decide how many cells are created */
    void insertNewRow( uint _idx, bool _recalc = true, bool _removeable = false );
    /** insert a column of new cells use the getRows() call to decide how many cells are created */
    void insertNewColumn( uint _idx, double width = KWTableFrameSet::m_sDefaultColWidth);

    /** Remove all the cells in a certain row */
    void deleteRow( uint _idx, RemovedRow &rr, bool _recalc = true);

    /** remove all the cells in a certain column */
    void deleteColumn( uint _idx, RemovedColumn &rc);

    /** replace a row that was removed with deleteRow() */
    void reInsertRow(RemovedRow &row);
    /** replace a column that was removed with deleteColumn() */
    void reInsertColumn(RemovedColumn &col);

    /** release the constrains of the table and allow all frames to be
     * edited apart from each other. (ps. there is no way back..) */
    void ungroup();

    void group();

    bool isActive()const { return m_active; }

    /** merge cells to one cell. Will loose all text not in top-left cell
     * @param firstColumn the first column of the square of columns that will be used to merge
     * @param firstRow the first row
     * @param endColumn the last column that will end up in the merged cell
     * @param endRow last row
     */
    KCommand *joinCells(unsigned int firstColumn,unsigned int firstRow, unsigned int endColumn,unsigned int endRow);
    /**
     * split one cell into a number of cells and return a command for undo purposes.
     * @param intoRows the amount of rows the cell should be split into
     * @param intoColumns the amount of columns the cell should be split into
     * @param column the column of the cell to be split
     * @param row the row of the cell to be split
     * @param listFrameSet needed for undo reasons
     * @param listFrame needed for undo reasons
     */
    KCommand * splitCell(unsigned int intoRows, unsigned int intoColumns, unsigned int column,
            unsigned int row, Q3PtrList<KWFrameSet> listFrameSet=Q3PtrList<KWFrameSet>(),
            Q3PtrList<KWFrame>listFrame=Q3PtrList<KWFrame>() );

    /** display formatting information */
    void viewFormatting( QPainter &painter, int zoom );
    /** do a number of complex tests to test the validity of the table. Missing/duplicate cells
    * and wrong values will be detected (and corrected) */
    void validate();

    /** override save so we save in table style */
    virtual QDomElement save( QDomElement &parentElem, bool saveFrames = true );

    virtual void saveOasis( KoXmlWriter&, KoSavingContext&, bool saveFrames ) const;
    void loadOasis( const QDomElement& tag, KoOasisContext& context );
    virtual bool canBeSavedAsInlineCharacter() const { return false; }

    /** load one cell */
    Cell* loadCell( QDomElement &frameElem, bool loadFrames = true, bool useNames = true );

    /** from and to XML - used when copying and pasting a table */
    virtual QDomElement toXML( QDomElement &parentElem, bool saveFrames = true );
    virtual void fromXML( QDomElement &framesetElem, bool loadFrames = true, bool useNames = true );

    /** Contribute to the document statistics */
    virtual int paragraphs();
    virtual int paragraphsSelected();
    virtual bool statistics( Q3ProgressDialog *progress, ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words,
        ulong & sentences, ulong & syllables,ulong & lines, bool selected );

    virtual void finalize();
    virtual void invalidate();
    virtual void layout();

    virtual void updateFrames( int flags = 0xff );

    /* Overloaded methods, look for docu in kwframe.h */
    virtual void moveFloatingFrame( int frameNum, const KoPoint &position );
    virtual KoSize floatingFrameSize( int frameNum = 0 );
    virtual KCommand * anchoredObjectCreateCommand( int frameNum );
    virtual KCommand * anchoredObjectDeleteCommand( int frameNum );
    virtual KWAnchor * createAnchor( KoTextDocument *txt, int frameNum );

    virtual void setVisible( bool v );
    virtual bool canRemovePage( int num );

    /** Add a cell to this table, the cell should already have info like row, col and should
     * already have a frame.
     */
    void addCell( Cell *cell );

    /** Remove a cell from this table (either to delete it, or to move it)
     */
    void removeCell( Cell* cell );

    // The normal mechanism doesn't apply to tables; cells are protected individually
    // (in terms of data; the GUI has an item for protecting all cells at once)
    virtual void setProtectContent ( bool ) {}
    virtual bool protectContent() const { return false; }

    virtual KWTextFrameSet* nextTextObject( KWFrameSet * );
    /**
     * Sets zOrder for all cells in this table
     */
    void setZOrder();

    QByteArray convertTableToText();

#ifndef NDEBUG
    virtual void printDebug( KWFrame * frame );
    virtual void printDebug();
    void printArrayDebug();
#endif
    static const uint m_sDefaultColWidth = 60;
protected:
    /* Overloaded methods, look for docu in kwframe.h */
    /// \overload KWFrameSet::deleteAnchors
    virtual void deleteAnchors();
    /// \overload KWFrameSet::createAnchors
    virtual void createAnchors( KoTextParag * parag, int index, bool placeHolderExists = false, bool repaint = true );

private:
    void addCellToArray( Cell* cell );
    void afterLoadingCell( Cell* cell );
    void parseInsideOfTable( const QDomElement& parent, KoOasisContext& context,
                             const Q3MemArray<double> & columnLefts, uint& row, uint& column,
                             double currentRowHeight );
    void loadOasisCell( const QDomElement& element, KoOasisContext& context,
                        const Q3MemArray<double> & columnLefts, uint row, uint column,
                        double currentRowHeight );

   /**
     * @brief position an individual cell in the grid
     *
     * Adjusts the size of the cell frames.
     * It computes the sizes based on:
     * <ol>
     * <li>The values in the m_colPositions and m_rowPositions arrays.
     * <li>The width of the Cell borders.
     * </ol>
     */
    void position(Cell *theCell, bool setMinFrameHeight=false);

    /**
     * @brief Returns the absolute top-position of the row in the grid
     *
     * Returns a double value from m_rowPositions. This is either the bottom
     * or top position of the row of cells @p row. Note that you cannot index
     * directly into m_rowPositions from a row value obtained from a cell,
     * eg cell->firstRow(), because of the extra elements in m_rowPositions when
     * the table spans multiple pages.
     */
    double getPositionOfRow(unsigned int row, bool bottom=false);

    void insertEmptyColumn(uint index);
    /**
     *  insert a row in m_rowArray at position index. rows after
     *  the inserted row are moved back.
     */
    void insertRowVector(uint index, Row *row);
    /**
     *  remove the row from m_rowArray at position index.
     *  rows after are moved forward
     *  @return the removed row
     */
    Row* removeRowVector(uint index);

private:
    unsigned int m_rows, m_cols, m_nr_cells;
    bool m_active;
    Q3PtrVector< Row > m_rowArray;

    /** The list of page boundaries.
    *   Each page the table spans has an entry in this list which points to the last _line_
    *   on a page.
    *   For a 1 page table there is no page break; and therefor for such tables this list is
    *   empty.
    */
    Q3ValueList<unsigned int> m_pageBoundaries;
    unsigned int m_redrawFromCol;
    Q3ValueList<double> m_rowPositions, m_colPositions;
};


// all three templates specialise operator++
template<>
KWTableFrameSet::Cell*
KWTableFrameSet::TableIterator<KWTableFrameSet::VISIT_CELL>::operator++ ();

template<>
KWTableFrameSet::Cell*
KWTableFrameSet::TableIterator<KWTableFrameSet::VISIT_GRID>::operator++ ();

template<>
KWTableFrameSet::Cell*
KWTableFrameSet::TableIterator<KWTableFrameSet::CHECKED>::operator++ ();

template<int VisitStyle>
KWTableFrameSet::TableIterator<VisitStyle>::TableIterator(KWTableFrameSet *table) :
        m_table(table)
{
        Q_ASSERT(m_table);
        set_limits(0, m_table->getColumns() - 1, 0, m_table->getRows() - 1);
        toFirstCell();
}

// CHECKED specialises the constructor
template<>
KWTableFrameSet::TableIterator<KWTableFrameSet::CHECKED>::TableIterator(KWTableFrameSet *table);


template<int VisitStyle>
KWTableFrameSet::Cell*
KWTableFrameSet::TableIterator<VisitStyle>::toFirstCell (){
        m_cell = m_table->cell(m_limit[HIGH], m_limit[LEFT]);
        Q_ASSERT(m_cell);
        if ( !m_cell )
            return 0;
        m_row = m_cell->firstRow();
        m_col = m_cell->firstColumn();
        return m_cell;
}

template<int VisitStyle>
void
KWTableFrameSet::TableIterator<VisitStyle>::goToCell(KWTableFrameSet::Cell *cell)
{
        m_cell = cell;
        Q_ASSERT( m_cell );
        if ( m_cell )
        {
            m_row = m_cell->firstRow();
            m_col = m_cell->firstColumn();
        }
}

// CHECKED specialises to first cell
template<>
KWTableFrameSet::Cell*
KWTableFrameSet::TableIterator<KWTableFrameSet::CHECKED>::toFirstCell ();


template<int VisitStyle>
KWTableFrameSet::Cell*
KWTableFrameSet::TableIterator<VisitStyle>::current() const {
        return m_cell;
}

/**
 * RemovedRow and RemovedColumn implement the Memento design pattern
 */
class RemovedRow {

    KWTableFrameSet::Row *m_row;
    /// The row index that this row used to occupy
    uint m_index;
    double m_rowHeight;

    uint index() const { return m_index; }
    double height() const { return m_rowHeight; }
    KWTableFrameSet::Row *takeRow();
    KWTableFrameSet::Row *row() { return m_row; }

    friend class KWTableFrameSet;
public:
    RemovedRow();
    ~RemovedRow();
};

class RemovedColumn {

    Q3PtrList<KWTableFrameSet::Cell> m_column;
    Q3ValueList<bool> m_removed;
    uint m_index;
    double m_width;
    bool m_initialized;

    friend class KWTableFrameSet;
public:
    RemovedColumn();
};

/**
 * @brief The object created to edit this table
 *
 * In fact at a given moment,
 * it edits one cell (frameset) of the table, the one in which the cursor is.
 */
class KWTableFrameSetEdit : public KWFrameSetEdit
{
public:
    KWTableFrameSetEdit( KWTableFrameSet * fs, KWCanvas * canvas )
        : KWFrameSetEdit( fs, canvas ), m_currentCell( 0L ) {}
    virtual ~KWTableFrameSetEdit();

    KWTableFrameSet * tableFrameSet() const {
        return static_cast<KWTableFrameSet *>( m_fs );
    }

    virtual KWFrameSetEdit* currentTextEdit();

    KWFrameSetEdit* currentCell() const { return m_currentCell; }

    // Forward all events to the current cell
    virtual void keyPressEvent( QKeyEvent * e );
    virtual void keyReleaseEvent( QKeyEvent * e );
    virtual void inputMethodEvent( QInputMethodEvent* e );
    virtual void mousePressEvent( QMouseEvent * e, const QPoint &, const KoPoint & );
    virtual void mouseMoveEvent( QMouseEvent * e, const QPoint & n, const KoPoint & d )
    { if ( m_currentCell ) m_currentCell->mouseMoveEvent( e, n, d ); }
    virtual void mouseReleaseEvent( QMouseEvent * e, const QPoint & n, const KoPoint & d )
    { if ( m_currentCell ) m_currentCell->mouseReleaseEvent( e, n, d ); }
    virtual void mouseDoubleClickEvent( QMouseEvent * e, const QPoint & n, const KoPoint & d )
    { if ( m_currentCell ) m_currentCell->mouseDoubleClickEvent( e, n, d ); } // TODO check current cell

    virtual void dragEnterEvent( QDragEnterEvent * e )
    { if ( m_currentCell ) m_currentCell->dragEnterEvent( e ); }
    virtual void dragMoveEvent( QDragMoveEvent * e, const QPoint &n, const KoPoint &d );
    virtual void dragLeaveEvent( QDragLeaveEvent * e )
    { if ( m_currentCell ) m_currentCell->dragLeaveEvent( e ); }
    virtual void dropEvent( QDropEvent * e, const QPoint &n, const KoPoint &d, KWView* view )
    { if ( m_currentCell ) m_currentCell->dropEvent( e, n, d, view ); } // TODO check current cell

    virtual void focusInEvent() { if ( m_currentCell ) m_currentCell->focusInEvent(); }
    virtual void focusOutEvent() { if ( m_currentCell ) m_currentCell->focusOutEvent(); }
    virtual void copy() { if ( m_currentCell ) m_currentCell->copy(); }
    virtual void cut() { if ( m_currentCell ) m_currentCell->cut(); }
    virtual void paste() { if ( m_currentCell ) m_currentCell->paste(); }
    /// \note should selectAll select all cells ? etc.
    virtual void selectAll() { if ( m_currentCell ) m_currentCell->selectAll(); }

    /// Set the cell which is currently being edited
    void setCurrentCell( KWFrameSet * fs, bool eraseSelection=true );
    /// Set the cell which is currently being edited
    void setCurrentCell( const KoPoint & dPoint );

    void showPopup( KWFrame* frame, KWView* view, const QPoint & _point );

protected:
    KWFrameSetEdit * m_currentCell;

};
#endif
