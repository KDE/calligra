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

#include <qscrollview.h>
#include <qstylesheet.h>
#include <qrichtext_p.h>
#include <koQueryTrader.h>
#include "kwtextparag.h"

class KWDocument;
class KWFrameSet;
class KWFrameSetEdit;
class KWTextFrameSet;
class KWGroupManager;
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
    void setPixmapFilename( const QString & f ) { m_PixmapName = f; }
    void setPartEntry( const KoDocumentEntry & e ) { m_partEntry = e; }

    void updateCurrentFormat();

    //table
    unsigned int tableRows() {return trows;}
    unsigned int tableCols() {return tcols;};
    KWTblCellSize tableWidthMode(){return twid;}
    KWTblCellSize tableHeightMode(){return thei;}

    void setTableConfig( unsigned int rows, unsigned int cols,
                         KWTblCellSize wid, KWTblCellSize hei,
                         bool isFloating) {
        useAnchor = isFloating;
        trows = rows; tcols = cols;
        twid = wid; thei = hei;
    }

    KWGroupManager * getTable();

    KWGroupManager *getCurrentTable() { return curTable; }

    //delete frame
    void deleteTable( KWGroupManager *groupManager );
    void deleteFrame();

public slots:
    void printRTDebug();
    /**
     * Only repaint the frameset @p fs
     */
    void repaintChanged( KWFrameSet * fs );

protected:
    /** Set format changes on selection on current cursor */
    void setFormat( QTextFormat *, int flags);

    /**
     * Reimplemented from QScrollView, to draw the contents of the canvas
     */
    virtual void drawContents( QPainter *p, int cx, int cy, int cw, int ch );
    /**
     * The main drawing method.
     * @param onlyFrameset if 0L, repaint everything, otherwise repaint only what has changed inside this frameset.
     * @param painter
     * @param cx, cy, cw, ch the area to be repainted, in contents coordinates
     */
    void drawDocument( KWFrameSet * onlyFrameset, QPainter *painter, int cx, int cy, int cw, int ch );
    /**
     * Draw the borders of the frames
     * @param onlyFrameset if 0L, all frames of all frameset, otherwise only those of this particular frameset.
     * @param painter
     * @param crect the area to be repainted, in contents coordinates
     */
    void drawBorders( KWFrameSet * onlyFrameset, QPainter *painter, const QRect &crect );

    virtual void keyPressEvent( QKeyEvent *e );
    virtual void contentsMousePressEvent( QMouseEvent *e );
    virtual void contentsMouseMoveEvent( QMouseEvent *e );
    virtual void contentsMouseReleaseEvent( QMouseEvent *e );
    virtual void contentsMouseDoubleClickEvent( QMouseEvent *e );
    virtual void contentsDragEnterEvent( QDragEnterEvent *e );
    virtual void contentsDragMoveEvent( QDragMoveEvent *e );
    virtual void contentsDragLeaveEvent( QDragLeaveEvent *e );
    virtual void contentsDropEvent( QDropEvent *e );

    void selectAllFrames( bool select );
    void selectFrame( int mx, int my, bool select );
    void updateFrameFormat();

    bool isOutOfPage( QRect & r, int page ) const;

private slots:
    void frameDiaClosed();
    void slotContentsMoving( int, int );
    void doAutoScroll();

private:
    KWDocument *doc;
    KWFrameSetEdit *m_currentFrameSetEdit;
    bool mousePressed;
    KWGUI *m_gui;
    QTimer *scrollTimer;

    // Frame stuff
    MouseMode m_mouseMode;
    QRect m_insRect;
    int oldMx, oldMy;
    bool deleteMovingRect, frameResized;
    QString m_PixmapName; // when inserting a pixmap
    KoDocumentEntry m_partEntry; // when inserting a part

    // Anchor support
    bool useAnchor;

    //table
    unsigned int tcols, trows;
    KWTblCellSize twid, thei;
    KWGroupManager *curTable;
};

#endif
