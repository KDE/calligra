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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION

    This file defines KWord tables. A table is a KWFrameSet, thus allowing its
    internal structure to be invisible to the rest of kword. This is useful
    since the internal structure of a table is itself a series of KWFrameSets
    (one for each cell).
*/

#include <kwframe.h>
#include <kwtextframeset.h>
#include <qptrlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qrichtext_p.h>

class KWDocument;
class KWTableFrameSetEdit;
class QPainter;
class KWAnchor;
class KWordFrameSetIface;

/**
 * Class: KWTableFrameSet
 *
 * This class implements tables by acting as the manager for
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

    // A cell can be any type of FrameSet, but for now, we only support text!
    class Cell : public KWTextFrameSet
    {
    public:
        // constructor
        Cell( KWTableFrameSet *table, unsigned int row, unsigned int col, const QString & name = QString::null );
        Cell( KWTableFrameSet *table, const Cell &original );
        // destructor
        virtual ~Cell();

        unsigned int m_row, m_col;
        unsigned int m_rows, m_cols;
        virtual void addFrame(KWFrame *_frame, bool recalc = true);

        uint getRow() const {return m_row;}
        uint getColumn() const {return m_col;}
        KWTableFrameSet *table() const { return m_table;}
        bool isAboveOrLeftOf( unsigned row, unsigned col );

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
    private:
        KWTableFrameSet *m_table;
    };
    friend class Cell;

    /** The type of frameset. Use this to differentiate between different instantiations of
     *  the framesets. Each implementation will return a different frameType.
     */
    virtual FrameSetType type() { return FT_TABLE; }

    virtual KWordFrameSetIface* dcopObject();

    virtual void addTextFrameSets( QPtrList<KWTextFrameSet> & /*lst*/, bool forceAllTextFrameSet =false  );

    // constructor
    KWTableFrameSet( KWDocument *_doc, const QString & name );
    // destructor
    virtual ~KWTableFrameSet();

    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * canvas );

    /**
     * @param emptyRegion The region is modified to subtract the areas painted, thus
     *                    allowing the caller to determine which areas remain to be painted.
     */
    virtual void createEmptyRegion( const QRect & crect, QRegion & emptyRegion, KWViewMode *viewMode );
    void drawBorders( QPainter& painter, const QRect &crect, KWViewMode *viewMode );
    virtual void drawContents( QPainter * painter, const QRect & crect,
                               const QColorGroup & cg, bool onlyChanged, bool resetChanged,
                               KWFrameSetEdit *edit, KWViewMode *viewMode );
    // Dummy since we reimplement drawContents
    virtual void drawFrame(KWFrame *, QPainter *, const QRect &, const QRect&,
                           const QPoint&,
                           KWFrame *, const QColorGroup &, bool, bool,
                           KWFrameSetEdit *, KWViewMode *, bool ) {}

    // Frameset management
    Cell *getCell( int i ) { return m_cells.at( i ); }
    Cell *getCell( unsigned int row, unsigned int col );
    Cell *getCellByPos( double x, double y );

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
    void recalcCols(int _col=-1,int _row=-1);
    void recalcRows(int _col=-1,int _row=-1);

    /** returns the number of rows */
    unsigned int getRows()const { return m_rows; }
    /** returns the number of columns */
    unsigned int getCols()const { return m_colPositions.count()-1; }

    /** returns the number of cells the table contains, this includes
     * temporary headers. */
    unsigned int getNumCells()const { return m_cells.count(); }

    /** returns the fact if one cell (==frame) has been selected */
    bool hasSelectedFrame();

    /** select a row */
    void selectRow(uint number);

    /** select a column */
    void selectCol(uint number);

    /** deselect all frames */
    void deselectAll();

    /** refresh resize handle */
    void refreshSelectedCell();

    /** move the whole of the table, this is mainly for anchored frames. */
    void moveBy( double dx, double dy );
    /** point the outlines of all the cells */
    //void drawAllRects( QPainter &p, int xOffset, int yOffset );

    /** select all frames from the first selected to the argument frameset. */
    void selectUntil( Cell *cell );
    /** convenience method */
    void selectUntil( double x, double y);

    bool getFirstSelected(unsigned int &row, unsigned int &col);

    bool isRowSelected(uint row);
    bool isColSelected(uint column);

    /** is at least one row selected on the whole table?*/
    bool isRowsSelected();

    /** is at least one col selected on the whole table?*/
    bool isColsSelected();

    /** Return true if exactly one frame is selected. The parameters row
    *  and col will receive the values of the active row and col.<br>
    *  When no frame or more then one frame is selected row and col will
    *  stay unchanged (and false is returned).
    */
    bool isOneSelected( unsigned int &row, unsigned int &col );

    /** insert a row of new cells, use the getCols() call to decide how many cells are created */
    void insertRow( unsigned int _idx,QPtrList<KWFrameSet> listFrameSet=QPtrList<KWFrameSet>(),QPtrList<KWFrame>listFrame=QPtrList<KWFrame>(), bool _recalc = true, bool _removeable = false );
    /** insert a column of new cells use the getRows() call to decide how many cells are created */
    void insertCol( unsigned int _idx,QPtrList<KWFrameSet> listFrameSet=QPtrList<KWFrameSet>(), QPtrList<KWFrame> listFrame=QPtrList<KWFrame>(), double width = KWTableFrameSet::m_sDefaultColWidth);

    /** remove all the cells in a certain row */
    void deleteRow( unsigned int _idx, bool _recalc = true );
    /** remove all the cells in a certain col */
    void deleteCol( unsigned int _idx);

    // the boolean actually works, but is not saved (to xml) yet :(
    void setShowHeaderOnAllPages( bool s ) { m_showHeaderOnAllPages = s; }
    bool getShowHeaderOnAllPages()const { return m_showHeaderOnAllPages; }

    /** redraw contents of temp headers. */
    bool hasTempHeaders()const { return m_hasTmpHeaders; }

    /** release the constrains of the table and allow all frames to be
     * edited apart from each other. (ps. there is no way back..) */
    void ungroup();

    void group();

    bool isActive()const { return m_active; }

    /** merge cells to one cell. Will loose all text not in top-left cell */
    KCommand *joinCells(unsigned int colBegin=0,unsigned int rowBegin=0, unsigned int colEnd=0,unsigned int rowEnd=0);
    /** split selected cell into a number of cells */
    KCommand * splitCell(unsigned int intoRows, unsigned int intoCols, int _col=-1, int _row=-1,QPtrList<KWFrameSet> listFrameSet=QPtrList<KWFrameSet>(),QPtrList<KWFrame>listFrame=QPtrList<KWFrame>());

    /** display formatting information */
    void viewFormatting( QPainter &painter, int zoom );
    /** do a number of complex tests to test the validity of the table. Missing/duplicate cells
    * and wrong values will be detected (and corrected) */
    void validate();

    /** returns true if we have a cell occupying that position */
    bool contains( double mx, double my );

    /** override save so we save in table style.. */
    virtual QDomElement save( QDomElement &parentElem, bool saveFrames = true );
    /** load one cell */
    Cell* loadCell( QDomElement &frameElem, bool loadFrames = true, bool useNames = true );

    /** from and to XML - used when copying and pasting a table */
    virtual QDomElement toXML( QDomElement &parentElem, bool saveFrames = true );
    virtual void fromXML( QDomElement &framesetElem, bool loadFrames = true, bool useNames = true );

    virtual void zoom( bool forPrint );
    //virtual void preparePrinting( QPainter *painter, QProgressDialog *progress, int &processedParags );

    /** Contribute to the document statistics */
    virtual int paragraphs();
    virtual int paragraphsSelected();
    virtual bool statistics( QProgressDialog *progress, ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words,
        ulong & sentences, ulong & syllables, bool selected );

    virtual void finalize();

    virtual void updateFrames( int flags = 0xff );

    /* Overloaded methods, look for docu in kwframe.h */
    virtual void moveFloatingFrame( int frameNum, const KoPoint &position );
    virtual KoSize floatingFrameSize( int frameNum = 0 );
    virtual KCommand * anchoredObjectCreateCommand( int frameNum );
    virtual KCommand * anchoredObjectDeleteCommand( int frameNum );
    virtual KWAnchor * createAnchor( KoTextDocument *txt, int frameNum );

    virtual bool canRemovePage( int num );

    void showPopup( KWFrame *frame, KWFrameSetEdit *edit, KWView *view, const QPoint &point );
    /** Add a cell to this table, the cell should allready have info like row, col and should
     * allready have a frame.
     */
    void addCell( Cell *cell );

    /** set left border. Uses all selected frames in this table to set the left border. Will
        only set the left border to frames that are selected and don't have a frame to their
        left that is also selected.
    */
    void setLeftBorder(KoBorder newBorder);

    /** set top border. Uses all selected frames in this table to set the top border. Will
        only set the top border to frames that are selected and don't have a frame to their
        top that is also selected.
    */
    void setTopBorder(KoBorder newBorder);

    /** set bottom border. Uses all selected frames in this table to set the bottom border. Will
        only set the bottom border to frames that are selected and don't have a frame to their
        bottom that is also selected.
    */
    void setBottomBorder(KoBorder newBorder);

    /** set right border. Uses all selected frames in this table to set the right border. Will
        only set the right border to frames that are selected and don't have a frame to their
        right that is also selected.
    */
    void setRightBorder(KoBorder newBorder);

    KCommand *setProtectContent ( bool _protect );

    virtual KWTextFrameSet* nextTextObject( KWFrameSet * );
    /**
     * Sets zOrder for all cells in this table
     */
    void setZOrder();

    void convertTableToText();

#ifndef NDEBUG
    virtual void printDebug( KWFrame * frame );
    virtual void printDebug();
#endif
    static const uint m_sDefaultColWidth = 60;
protected:
    /* Overloaded methods, look for docu in kwframe.h */
    virtual void deleteAnchors();
    virtual void createAnchors( KWTextParag * parag, int index, bool placeHolderExists = false );

private:
    /** position an individual cell in the grid */
    void position(Cell *theCell, bool setMinFrameHeight=false);
    /** returns the absolute top-position of the row in the grid */
    double getPositionOfRow(unsigned int row, bool bottom=false);

    unsigned int m_rows;
    bool m_showHeaderOnAllPages;
    bool m_hasTmpHeaders;
    bool m_active;
    QPtrList<Cell> m_cells;
    /** The list of page boundaries.
    *   Each page the table spans has an entry in this list which points to the last _line_
    *   on a page.
    *   For a 1 page table there is no page break; and therefor for such tables this list is
    *   empty.
    */
    QValueList<unsigned int> m_pageBoundaries;
    unsigned int redrawFromCol;
    QValueList<double> m_rowPositions, m_colPositions;
};

/**
 * The object created to edit this table - in fact at a given moment,
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
    virtual void dropEvent( QDropEvent * e, const QPoint &n, const KoPoint &d )
    { if ( m_currentCell ) m_currentCell->dropEvent( e, n, d ); } // TODO check current cell

    virtual void focusInEvent() { if ( m_currentCell ) m_currentCell->focusInEvent(); }
    virtual void focusOutEvent() { if ( m_currentCell ) m_currentCell->focusOutEvent(); }
    virtual void copy() { if ( m_currentCell ) m_currentCell->copy(); }
    virtual void cut() { if ( m_currentCell ) m_currentCell->cut(); }
    virtual void paste() { if ( m_currentCell ) m_currentCell->paste(); }
    // should selectAll select all cells ? etc.
    virtual void selectAll() { if ( m_currentCell ) m_currentCell->selectAll(); }

    // Set the cell which is currently being edited
    void setCurrentCell( KWFrameSet * fs, bool eraseSelection=true );
    void setCurrentCell( const KoPoint & dPoint );

    void showPopup( KWFrame* frame, KWView* view, const QPoint & _point );

protected:
    KWFrameSetEdit * m_currentCell;

};
#endif
