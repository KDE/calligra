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
class QPainter;

/**
 * Class: KWTableFrameSet
 *
 * This class implements tables by acting as the manager for
 * the frame(set)s which make up the table cells.
 *
 * A table can be anchored, in which case its frame(set)s are
 * located relative to the Y position of the anchor.
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
        Cell( KWTableFrameSet *table, unsigned int row, unsigned int col );
        Cell( KWTableFrameSet *table, const Cell &original );
        // destructor
        virtual ~Cell();

        unsigned int m_row, m_col;
        unsigned int m_rows, m_cols;

        bool isAboveOrLeftOf( unsigned row, unsigned col );
    private:
        KWTableFrameSet *m_table;
    };
    friend class Cell;

    virtual FrameType getFrameType() { return FT_TABLE; }

    // constructors
    KWTableFrameSet( KWDocument *_doc );
    KWTableFrameSet( KWTableFrameSet &original );
    // destructor
    virtual ~KWTableFrameSet();

    virtual void drawContents( QPainter * painter, const QRect & crect,
                               QColorGroup & cg, bool onlyChanged, bool resetChanged );

    // frameset management
    Cell *getCell( int i ) { return m_cells.at( i ); }
    Cell *getCell( unsigned int row, unsigned int col );
    Cell *getCellByPos( int mx, int my ); // bad naming

    /** first row and auto-added rows are the table headers.
    * @returns if this frameset is either one.
    */
    bool isTableHeader( Cell *cell );

    /** put all the frames in the right location. */
    void init( unsigned int x, unsigned int y,
               unsigned int width, unsigned int height,
               KWTblCellSize wid, KWTblCellSize hei );
    /** resize and position all cells */
    void recalcCols();
    void recalcRows();

    /** returns the number of rows */
    unsigned int getRows() { return m_rows; }
    /** returns the number of columns */
    unsigned int getCols() { return m_cols; }

    /** returns a QRect which outlines the whole of the table. */
    QRect getBoundingRect();

    /** returns the number of cells the table contains, this includes
     * temporary headers. */
    unsigned int getNumCells() { return m_cells.count(); }

    /** returns the fact if one cell (==frame) has been selected */
    bool hasSelectedFrame();

    /** deselect all frames */
    void deselectAll();

    /** move the whole of the table, this is mainly for anchored frames. */
    void moveBy( int dx, int dy );
    /** point the outlines of all the cells */
    void drawAllRects( QPainter &p, int xOffset, int yOffset );

    /** sets the name of the table, this _must_ be a unique name! */
    void setName( QString _name ) { name = _name; }
    QString getName() { return name; }

    /** select all frames from the first selected to the argument frameset. */
    void selectUntil( Cell *cell );
    bool getFirstSelected(unsigned int &row, unsigned int &col );
    /** Return true if exactly one frame is selected. The parameters row
    *  and col will receive the values of the active row and col.<br>
    *  When no frame or more then one frame is selected row and col will
    *  stay unchanged (and false is returned).
    */
    bool isOneSelected( unsigned int &row, unsigned int &col );

    /** insert a row of new cells, use the getCols() call to decide how many cells are created */
    void insertRow( unsigned int _idx, bool _recalc = true, bool _removeable = false );
    /** insert a column of new cells use the getRows() call to decide how many cells are created */
    void insertCol( unsigned int _idx );

    /** remove all the cells in a certain row */
    void deleteRow( unsigned int _idx, bool _recalc = true );
    /** remove all the cells in a certain col */
    void deleteCol( unsigned int _idx );

    // the boolean actually works, but is not saved (to xml) yet :(
    void setShowHeaderOnAllPages( bool s ) { m_showHeaderOnAllPages = s; }
    bool getShowHeaderOnAllPages() { return m_showHeaderOnAllPages; }

    /** redraw contents of temp headers. */
    void updateTempHeaders();
    bool hasTempHeaders() { return m_hasTmpHeaders; }

    /** release the constrains of the table and allow all frames to be
     * edited apart from each other. (ps. there is no way back..) */
    void ungroup();

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
    bool contains( unsigned int mx, unsigned int my );

    QString anchorType();
    QString anchorInstance();

    virtual bool isVisible();

protected:
    KWDocument *m_doc;
    unsigned int m_rows, m_cols;
    QString m_name;
    bool m_showHeaderOnAllPages;
    bool m_hasTmpHeaders;
    bool m_active;
    bool m_isRendered;
    QList<Cell> m_cells;
    void addCell( Cell *cell );
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
    virtual ~KWTableFrameSetEdit() {}

    KWTableFrameSet * tableFrameSet() const {
        return static_cast<KWTableFrameSet *>( m_fs );
    }

    // Forward all events to the current cell
    virtual void keyPressEvent( QKeyEvent * e ) { m_currentCell->keyPressEvent( e ); }
    virtual void mousePressEvent( QMouseEvent * e );
    virtual void mouseMoveEvent( QMouseEvent * e ) { m_currentCell->mouseMoveEvent( e ); }
    virtual void mouseReleaseEvent( QMouseEvent * e ) { m_currentCell->mouseReleaseEvent( e ); }
    virtual void mouseDoubleClickEvent( QMouseEvent * e ) { m_currentCell->mouseDoubleClickEvent( e ); } // TODO check current cell
    virtual void dragEnterEvent( QDragEnterEvent * e ) { m_currentCell->dragEnterEvent( e ); }
    virtual void dragMoveEvent( QDragMoveEvent * e ) { m_currentCell->dragMoveEvent( e ); }
    virtual void dragLeaveEvent( QDragLeaveEvent * e ) { m_currentCell->dragLeaveEvent( e ); }
    virtual void dropEvent( QDropEvent * e ) { m_currentCell->dropEvent( e ); } // TODO check current cell
    virtual void focusInEvent() { m_currentCell->focusInEvent(); }
    virtual void focusOutEvent() { m_currentCell->focusOutEvent(); }
    virtual void doAutoScroll( QPoint p ) { m_currentCell->doAutoScroll( p ); }
    virtual void copy() { m_currentCell->copy(); }
    virtual void cut() { m_currentCell->cut(); }
    virtual void paste() { m_currentCell->paste(); }
    // should selectAll select all cells ? etc.
    virtual void selectAll() { m_currentCell->selectAll(); }

    // Set the cell which is currently being edited
    void setCurrentCell( KWFrameSet * fs );
    void setCurrentCell( int mx, int my );

protected:
    KWFrameSetEdit * m_currentCell;

};
#endif
