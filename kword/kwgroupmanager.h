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

#ifndef kwgroupmanager_h
#define kwgroupmanager_h

#include <defs.h>
#include <kwframe.h>
#include <qlist.h>
class KWDocument;
class Cell;
class QPainter;


/**
 * Class: KWGroupManager
 *
 * This class implements tables by acting as the manager for
 * the frame(set)s which make up the table cells (hence share a
 * common grpMgr attribute).
 *
 * A table can be anchored, in which case its frame(set)s are
 * located relative to the Y position of the anchor.
 * We have a cell structure which contains one frameset, because
 * of the nature of the table this frameset will always hold
 * exactly one frame. Therefore the terms cell, frameSet and frame
 * can be used to describe the same thing: one table-cell
 */
class KWGroupManager
//: public KWCharAnchor
{
public:
    struct Cell
    {
        KWFrameSet *frameSet;
        unsigned int row, col;
        unsigned int rows, cols;
        ~Cell() {if(frameSet) delete frameSet; }
    };

    // constructors
    KWGroupManager( KWDocument *_doc );
    KWGroupManager( const KWGroupManager &original );
    // destructor
    virtual ~KWGroupManager();

    // frameset management
    void addFrameSet( KWFrameSet *fs, unsigned int row, unsigned int col );
    KWFrameSet *getFrameSet( unsigned int row, unsigned int col );

    /** first row and auto-added rows are the table headers.
    * @returns if this frameset is either one.
    */
    bool isTableHeader( KWFrameSet *fs );

    /** put all the frames in the right location. */
    void init( unsigned int x, unsigned int y,
               unsigned int width, unsigned int height,
               KWTblCellSize wid, KWTblCellSize hei );
    /** announce all frames to the document */
    void init();
    /** resize and position all cells */
    void recalcCols();
    void recalcRows();

    /** returns the number of rows */
    unsigned int getRows() { return rows; }
    /** returns the number of columns */
    unsigned int getCols() { return cols; }

    /** returns a QRect which outlines the whole of the table. */
    QRect getBoundingRect();

    /** returns the number of cells the table contains, this includes
     * temporary headers. */
    unsigned int getNumCells() { return cells.count(); }

    /** returns a specific table cell. */
    Cell *getCell( int i ) { return cells.at( i ); }
    /** returns a specific table cell. */
    Cell *getCell( unsigned int row, unsigned int col );
    /** returns a specific table cell. */
    Cell *getCell( KWFrameSet * );

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
    void selectUntil( KWFrameSet *fs);
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
    void setShowHeaderOnAllPages( bool s ) { showHeaderOnAllPages = s; }
    bool getShowHeaderOnAllPages() { return showHeaderOnAllPages; }

    /** redraw contents of temp headers. */
    void updateTempHeaders();
    bool hasTempHeaders() { return hasTmpHeaders; }

    /** release the constrains of the table and allow all frames to be
     * edited apart from each other. (ps. there is no way back..) */
    void ungroup();

    bool isActive() { return active; }

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

    QString anchorType();
    QString anchorInstance();

protected:
    QList<Cell> cells;
    unsigned int rows, cols;
    KWDocument *doc;
    QString name;
    bool showHeaderOnAllPages, hasTmpHeaders;
    bool active;
    bool isRendered;
};

#endif
