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
#include <qlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qrichtext_p.h>

using namespace Qt3;

class KWDocument;
class KWTableFrameSetEdit;
class QPainter;
class KWAnchor;

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

        bool isAboveOrLeftOf( unsigned row, unsigned col );
    private:
        KWTableFrameSet *m_table;
    };
    friend class Cell;

    virtual FrameSetType type() { return FT_TABLE; }

    // constructors
    KWTableFrameSet( KWDocument *_doc, const QString & name );
    KWTableFrameSet( KWTableFrameSet &original );
    // destructor
    virtual ~KWTableFrameSet();

    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * canvas );

    /**
     * @param emptyRegion The region is modified to subtract the areas painted, thus
     *                    allowing the caller to determine which areas remain to be painted.
     */
    virtual void createEmptyRegion( const QRect & crect, QRegion & emptyRegion, KWViewMode *viewMode );
    void drawBorders( QPainter& painter, const QRect &crect, KWViewMode *viewMode, KWCanvas *canvas );
    virtual void drawContents( QPainter * painter, const QRect & crect,
                               QColorGroup & cg, bool onlyChanged, bool resetChanged,
                               KWFrameSetEdit *edit, KWViewMode *viewMode, KWCanvas *canvas );
    // Dummy since we reimplement drawContents
    virtual void drawFrame(KWFrame *, QPainter *, const QRect &, QColorGroup &,
        bool, bool, KWFrameSetEdit *) {}

    // Frameset management
    Cell *getCell( int i ) { return m_cells.at( i ); }
    Cell *getCell( unsigned int row, unsigned int col );
    Cell *getCellByPos( double x, double y );

    /** first row and auto-added rows are the table headers.
    * @returns if this frameset is either one.
    */
    bool isTableHeader( Cell *cell );

    enum CellSize {
        TblAuto = 0,
        TblManual
    };

    /**
     * Layout all cells:
     *
     *  setHeightMode() and setWidthMode() controls the layout process.
     *  setBoundingRect() causes the layout to recalculated.
     *  boundingRect() returns a KoRect which outlines the whole of the table.
     */
    CellSize heightMode() { return m_heightMode; }
    void setHeightMode( CellSize mode );
    CellSize widthMode() { return m_widthMode; }
    void setWidthMode( CellSize mode );
    KoRect boundingRect();
    void setBoundingRect( KoRect rect );

    /** resize and position all cells */
    void recalcCols(int _col=-1,int _row=-1);
    void recalcRows(int _col=-1,int _row=-1);

    /** returns the number of rows */
    unsigned int getRows() { return m_rows; }
    /** returns the number of columns */
    unsigned int getCols() { return m_cols; }

    /** returns the number of cells the table contains, this includes
     * temporary headers. */
    unsigned int getNumCells() { return m_cells.count(); }

    /** returns the fact if one cell (==frame) has been selected */
    bool hasSelectedFrame();

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

    bool getFirstSelected(unsigned int &row, unsigned int &col );
    /** Return true if exactly one frame is selected. The parameters row
    *  and col will receive the values of the active row and col.<br>
    *  When no frame or more then one frame is selected row and col will
    *  stay unchanged (and false is returned).
    */
    bool isOneSelected( unsigned int &row, unsigned int &col );

    /** insert a row of new cells, use the getCols() call to decide how many cells are created */
    void insertRow( unsigned int _idx,QList<KWFrameSet> listFrameSet=QList<KWFrameSet>(),QList<KWFrame>listFrame=QList<KWFrame>(), bool _recalc = true, bool _removeable = false );
    /** insert a column of new cells use the getRows() call to decide how many cells are created */
    void insertCol( unsigned int _idx,QList<KWFrameSet> listFrameSet=QList<KWFrameSet>(), QList<KWFrame> listFrame=QList<KWFrame>());

    /** remove all the cells in a certain row */
    void deleteRow( unsigned int _idx, bool _recalc = true );
    /** remove all the cells in a certain col */
    void deleteCol( unsigned int _idx);

    // the boolean actually works, but is not saved (to xml) yet :(
    void setShowHeaderOnAllPages( bool s ) { m_showHeaderOnAllPages = s; }
    bool getShowHeaderOnAllPages() { return m_showHeaderOnAllPages; }

    /** redraw contents of temp headers. */
    void updateTempHeaders();
    bool hasTempHeaders() { return m_hasTmpHeaders; }

    /** release the constrains of the table and allow all frames to be
     * edited apart from each other. (ps. there is no way back..) */
    void ungroup();

    void group();

    bool isActive() { return m_active; }

    /** merge cells to one cell. Will loose all text not in top-left cell */
    bool joinCells();
    /** split selected cell into a number of cells */
    bool splitCell(unsigned int intoRows, unsigned int intoCols);

    /** display formatting information */
    void viewFormatting( QPainter &painter, int zoom );
    /** do a number of complex tests to test the validity of the table. Missing/duplicate cells
    * and wrong values will be detected (and corrected) */
    void validate();

    /** Prerender the whole table to resize the table cells so all text will fit. */
    void preRender();

    /** returns true if we have a cell occupying that position */
    bool contains( double mx, double my );

    /** override save so we save in table style.. */
    virtual void save( QDomElement &parentElem, bool saveFrames = true );

    virtual void zoom( bool forPrint );
    virtual void preparePrinting( QPainter *painter, QProgressDialog *progress, int &processedParags );

    /** Contribute to the document statistics */
    virtual int paragraphs();
    virtual bool statistics( QProgressDialog *progress, ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words,
        ulong & sentences, ulong & syllables );

    virtual void finalize();

    virtual void updateFrames();

    virtual void moveFloatingFrame( int frameNum, const QPoint &position );
    virtual QSize floatingFrameSize( int frameNum );
    /** Store command for creating an anchored object */
    virtual KCommand * anchoredObjectCreateCommand( int frameNum );
    /** Store command for deleting an anchored object */
    virtual KCommand * anchoredObjectDeleteCommand( int frameNum );
    virtual KWAnchor * createAnchor( KWTextDocument * textdoc, int frameNum );

    virtual bool canRemovePage( int num );

#ifndef NDEBUG
    virtual void printDebug( KWFrame * frame );
    virtual void printDebug();
#endif
    void addCell( Cell *cell );
protected:
    virtual void deleteAnchors();
    virtual void createAnchors( KWTextParag * parag, int index, bool placeHolderExists = false );
    //void addCell( Cell *cell );

private:
    unsigned int m_rows, m_cols;
    CellSize m_widthMode;
    CellSize m_heightMode;
    bool m_showHeaderOnAllPages;
    bool m_hasTmpHeaders;
    bool m_active;
    bool m_isRendered;
    QList<Cell> m_cells;
    QValueList<int> m_pageBoundaries;
    static const unsigned int tableCellSpacing;
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

    virtual QString getPopupName() { return "text_popup";}

    // Forward all events to the current cell
    virtual void keyPressEvent( QKeyEvent * e );
    virtual void mousePressEvent( QMouseEvent * e, const QPoint &, const KoPoint & );
    virtual void mouseMoveEvent( QMouseEvent * e, const QPoint & n, const KoPoint & d )
    { if ( m_currentCell ) m_currentCell->mouseMoveEvent( e, n, d ); }
    virtual void mouseReleaseEvent( QMouseEvent * e, const QPoint & n, const KoPoint & d )
    { if ( m_currentCell ) m_currentCell->mouseReleaseEvent( e, n, d ); }
    virtual void mouseDoubleClickEvent( QMouseEvent * e, const QPoint & n, const KoPoint & d )
    { if ( m_currentCell ) m_currentCell->mouseDoubleClickEvent( e, n, d ); } // TODO check current cell

    virtual void dragEnterEvent( QDragEnterEvent * e )
    { if ( m_currentCell ) m_currentCell->dragEnterEvent( e ); }
    virtual void dragMoveEvent( QDragMoveEvent * e, const QPoint &n, const KoPoint &d )
    { if ( m_currentCell ) m_currentCell->dragMoveEvent( e, n, d ); }
    virtual void dragLeaveEvent( QDragLeaveEvent * e )
    { if ( m_currentCell ) m_currentCell->dragLeaveEvent( e ); }
    virtual void dropEvent( QDropEvent * e, const QPoint &n, const KoPoint &d )
    { if ( m_currentCell ) m_currentCell->dropEvent( e, n, d ); } // TODO check current cell

    virtual void focusInEvent() { if ( m_currentCell ) m_currentCell->focusInEvent(); }
    virtual void focusOutEvent() { if ( m_currentCell ) m_currentCell->focusOutEvent(); }
    virtual void doAutoScroll( QPoint p ) { if ( m_currentCell ) m_currentCell->doAutoScroll( p ); }
    virtual void copy() { if ( m_currentCell ) m_currentCell->copy(); }
    virtual void cut() { if ( m_currentCell ) m_currentCell->cut(); }
    virtual void paste() { if ( m_currentCell ) m_currentCell->paste(); }
    // should selectAll select all cells ? etc.
    virtual void selectAll() { if ( m_currentCell ) m_currentCell->selectAll(); }

    // Set the cell which is currently being edited
    void setCurrentCell( KWFrameSet * fs );
    void setCurrentCell( const KoPoint & dPoint );

protected:
    KWFrameSetEdit * m_currentCell;

};
#endif
