/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KWCANVAS_H
#define KWCANVAS_H

#include <koprinter.h>
#include <koRect.h>
#include <qscrollview.h>
#include <qstylesheet.h>
#include <qrichtext_p.h>
#include <koQueryTrader.h>
#include "kwtextparag.h"

class KWDocument;
class KWFrame;
class KWFrameSet;
class KWFrameSetEdit;
class KWTextFrameSet;
class KWTableFrameSet;
class KWFrameMoveCommand;
namespace Qt3 {
class QTextCursor;
class QTextParag;
class QTextFormat;
class QTextDocument;
}
using namespace Qt3;
class QTimer;
class KWGUI;
class KWStyle;
class KWAnchor;

/**
 * Class: KWCanvas
 * This class is responsible for the rendering of the frames to
 * the screen as well as the interaction with the user via mouse
 * and keyboard. There is one per view.
 */
class KWCanvas : public QScrollView
{
    Q_OBJECT

public:
    KWCanvas(QWidget *parent, KWDocument *d, KWGUI *lGui);
    virtual ~KWCanvas();

    KWDocument * kWordDocument() const { return doc; }
    KWGUI * gui() const { return m_gui; }
    //KWFrameSet *currentFrameSet() const { return m_currentFrameSet; }
    KWFrameSetEdit *currentFrameSetEdit() const { return m_currentFrameSetEdit; }

    void repaintAll( bool erase = false );
    /**
     * Only repaint the frameset @p fs.
     * @p resetChanged should only be true for the last view
     * (see KWFrameSet::drawContents)
     */
    void repaintChanged( KWFrameSet * fs, bool resetChanged );

    void print( QPainter *painter, KPrinter *printer );
    bool eventFilter( QObject *o, QEvent *e );
    bool focusNextPrevChild( bool );

    // --
    // frame border
    void setLeftFrameBorder( Border _frmBrd, bool _b );
    void setRightFrameBorder( Border _frmBrd, bool _b );
    void setTopFrameBorder( Border _frmBrd, bool _b );
    void setBottomFrameBorder( Border _frmBrd, bool _b );
    void setOutlineFrameBorder( Border _frmBrd, bool _b );

    void setFrameBackgroundColor( const QBrush &backColor );

    void editFrameProperties();

    // Mouse press
    void mpEditFrame( QMouseEvent *e, int mx, int my );
    void mpCreate( int mx, int my );
    void mpCreatePixmap( int mx, int my );
    // Mouse move
    void mmEdit( int /*mx*/, int /*my*/ );  // mouse move in edit mode (with button pressed)
    void mmEditFrameResize( bool top, bool bottom, bool left, bool right );
    void mmEditFrameMove( int mx, int my );
    void mmCreate( int mx, int my );
    // Mouse release
    void mrEditFrame();
    void mrCreateText();
    void mrCreatePixmap();
    void mrCreatePart();
    void mrCreateFormula();
    void mrCreateTable();
    // TODO Mouse double-click

    void setMouseMode( MouseMode _mm );
    MouseMode getMouseMode(){return m_mouseMode;}
    void setPixmapFilename( const QString & f ) { m_PixmapName = f; }
    void setPartEntry( const KoDocumentEntry & e ) { m_partEntry = e; }

    void updateCurrentFormat();

    void updateFrameFormat();

    // Table creation support - methods used by KWView to reuse the last settings
    unsigned int tableRows() { return m_table.rows; }
    unsigned int tableCols() { return m_table.cols; }
    KWTblCellSize tableWidthMode() { return m_table.width; }
    KWTblCellSize tableHeightMode() { return m_table.height; }
    bool tableIsFloating() { return m_table.floating; }

    void createTable( unsigned int rows, unsigned int cols,
                      KWTblCellSize wid, KWTblCellSize hei,
                      bool isFloating );
    KWTableFrameSet * getTable();

    KWTableFrameSet *getCurrentTable() { return curTable; }

    //delete frame
    void deleteTable( KWTableFrameSet *groupManager );
    void deleteFrame( KWFrame * frame );

    //delete currentFrameSetEdit when you remove table
    //column, row. Otherwise kword crashs.
    void deleteFrameSetEditTable( KWTableFrameSet *table );

    //move canvas
    void scrollToOffset( int _x, int _y );
    void setContentsPos( int x, int y );

    //koruler position
    int getVertRulerPos(int y=-1);
    int getHorzRulerPos(int x=-1);

protected:
    /** Set format changes on selection on current cursor */
    void setFormat( QTextFormat *, int flags);

    /**
     * Reimplemented from QScrollView, to draw the contents of the canvas
     */
    virtual void drawContents( QPainter *p, int cx, int cy, int cw, int ch );
    /**
     * The main drawing method.
     * @param painter
     * @param crect the area to be repainted, in contents coordinates
     */
    void drawDocument( QPainter *painter, const QRect &crect );

    virtual void keyPressEvent( QKeyEvent *e );
    virtual void contentsMousePressEvent( QMouseEvent *e );
    virtual void contentsMouseMoveEvent( QMouseEvent *e );
    virtual void contentsMouseReleaseEvent( QMouseEvent *e );
    virtual void contentsMouseDoubleClickEvent( QMouseEvent *e );
    virtual void contentsDragEnterEvent( QDragEnterEvent *e );
    virtual void contentsDragMoveEvent( QDragMoveEvent *e );
    virtual void contentsDragLeaveEvent( QDragLeaveEvent *e );
    virtual void contentsDropEvent( QDropEvent *e );
    virtual void resizeEvent( QResizeEvent *e );

    void selectAllFrames( bool select );
    void selectFrame( double mx, double my, bool select );

    KWTableFrameSet * createTable(); // uses m_insRect and m_table to create the table

signals:
    // Emitted when the current frameset edit changes
    void currentFrameSetEditChanged();
    // Emitted by the current frameset edit when its selection changes
    void selectionChanged( bool hasSelection );
    //Emitted when Mouse Mode changed
    void currentMouseModeChanged(MouseMode newMouseMode);
    //emit when you selected a other frame (disable/enable table button)
    void frameSelectedChanged();
    //emit when you want to refresh docstruct
    void docStructChanged();

private slots:
    void slotContentsMoving( int, int );
    void doAutoScroll();

private:
    /**
     * Draw the contents of one frameset
     * @param resetChanged whether the changed flag should be reset to false while redrawing
     */
    void drawFrameSet( KWFrameSet * frameset, QPainter * painter,
                       const QRect & crect, bool onlyChanged, bool resetChanged );

    void drawMovingRect( QPainter & p );

#ifndef NDEBUG
    void printRTDebug( int );
#endif

    KWDocument *doc;
    KWFrameSetEdit *m_currentFrameSetEdit;
    bool mousePressed;
    KWGUI *m_gui;
    QTimer *scrollTimer;

    // Frame stuff
    MouseMode m_mouseMode;
    KoRect m_resizedFrameInitialSize; // when resizing a frame
    KoRect m_insRect;  // when creating a new frame
    KoRect m_boundingRect; // when moving frame(s)
    KoPoint m_hotSpot; // when moving frame(s)
    bool deleteMovingRect, frameMoved, frameResized;
    QString m_PixmapName; // when inserting a pixmap
    KoDocumentEntry m_partEntry; // when inserting a part

    // Table creation support.
    // Having this as a member variable allows to remember and reuse the last settings
    struct
    {
        unsigned int cols;
        unsigned int rows;
        KWTblCellSize width;
        KWTblCellSize height;
        bool floating;
    } m_table;
    KWTableFrameSet *curTable;
    KWFrameMoveCommand *cmdMoveFrame;
};

#endif
