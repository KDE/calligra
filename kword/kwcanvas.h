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
class KWViewMode;
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

    KWDocument * kWordDocument() const { return m_doc; }
    KWGUI * gui() const { return m_gui; }
    KWFrameSetEdit *currentFrameSetEdit() const { return m_currentFrameSetEdit; }

    void switchViewMode( KWViewMode * newViewMode );
    KWViewMode *viewMode() const { return m_viewMode; }

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
    void copySelectedFrames();
    void cutSelectedFrames();

    void pasteFrames();

    // Mouse press
    void mpEditFrame( QMouseEvent *e, const QPoint& nPoint );
    void mpCreate( const QPoint& normalPoint );
    void mpCreatePixmap( const QPoint& normalPoint );
    // Mouse move
    void mmEditFrameResize( bool top, bool bottom, bool left, bool right, bool noGrid );
    void mmEditFrameMove( const QPoint &normalPoint, bool shiftPressed );
    void mmCreate( const QPoint& normalPoint, bool shiftPressed );
    // Mouse release
    void mrEditFrame( QMouseEvent *e, const QPoint &nPoint );
    void mrCreateText();
    void mrCreatePixmap();
    void mrCreatePart();
    void mrCreateFormula();
    void mrCreateTable();

    enum MouseMode {
        MM_EDIT = 0,
        MM_CREATE_TEXT = 2,
        MM_CREATE_PIX = 3,
        MM_CREATE_TABLE = 5,
        MM_CREATE_FORMULA = 6,
        MM_CREATE_PART = 7
    };
    void setMouseMode( MouseMode _mm );
    MouseMode mouseMode() { return m_mouseMode; }

    void insertPicture( const QString &filename, bool isClipart, QSize pixmapSize, bool _keepRatio );
    void insertPart( const KoDocumentEntry &entry );
    void pasteImage( QMimeSource *e, const KoPoint &docPoint );

    void updateCurrentFormat();

    void updateFrameFormat();

    // Table creation support - methods used by KWView to reuse the last settings
    unsigned int tableRows() { return m_table.rows; }
    unsigned int tableCols() { return m_table.cols; }
    int tableWidthMode() { return m_table.width; }
    int tableHeightMode() { return m_table.height; }
    bool tableIsFloating() { return m_table.floating; }

    void createTable( unsigned int rows, unsigned int cols,
                      int /*KWTableFrameSet::CellSize*/ wid, int /*KWTableFrameSet::CellSize*/ hei,
                      bool isFloating );
    KWTableFrameSet * getTable();

    KWTableFrameSet *getCurrentTable() { return curTable; }

    //move canvas to show point dPoint (in doc coordinates)
    void scrollToOffset( const KoPoint & dPoint );

    //for KWTextFrameSetEdit
    void dragStarted() { m_mousePressed = false; }

    void refreshViewMode();

    void emitFrameSelectedChanged();

    void setXimPosition( int x, int y, int w, int h );

    QPoint pageCorner();
    void updateRulerOffsets( int cx = -1, int cy = -1 );

protected:
    void applyGrid( KoPoint &p );
    void applyAspectRatio( double ratio, KoRect& insRect );

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
    /**
     * Draw page borders, but also clear up the space between the frames and the page borders,
     * draw the page shadow, and the gray area.
     */
    void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );

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

    bool selectAllFrames( bool select );
    void selectFrame( KWFrame* frame, bool select );

    KWTableFrameSet * createTable(); // uses m_insRect and m_table to create the table

    void terminateCurrentEdit();
    bool checkCurrentEdit( KWFrameSet * fs );

signals:
    // Emitted when the current frameset edit changes
    void currentFrameSetEditChanged();
    // Emitted by the current frameset edit when its selection changes
    void selectionChanged( bool hasSelection );
    // Emitted when Mouse Mode changed
    void currentMouseModeChanged(int newMouseMode);
    // Emitted when frames have been selected or unselected (to disable/enable the UI in kwview)
    void frameSelectedChanged();
    // Emitted when the document structure has changed
    // ### DF: IMHO this should be only emitted by KWDocument (e.g. addFrameSet)
    void docStructChanged(int _type);

    void updateRuler();
private slots:
    void slotContentsMoving( int, int );
    void slotNewContentsSize();
    void doAutoScroll();

    //Terminate editing this frameset, if we were editing it.
    void terminateEditing( KWFrameSet *fs );

private:
    /**
     * Draw the contents of one frameset
     * @param resetChanged whether the changed flag should be reset to false while redrawing
     */
    void drawFrameSet( KWFrameSet * frameset, QPainter * painter,
                       const QRect & crect, bool onlyChanged, bool resetChanged );

    void drawMovingRect( QPainter & p );
    void deleteMovingRect();

#ifndef NDEBUG
    void printRTDebug( int );
#endif

    KWDocument *m_doc;
    KWFrameSetEdit *m_currentFrameSetEdit;
    KWGUI *m_gui;
    QTimer *m_scrollTimer;
    bool m_mousePressed;
    bool m_printing;
    bool m_imageDrag;
    KWViewMode *m_viewMode;

    // Frame stuff
    MouseMode m_mouseMode;
    KoRect m_resizedFrameInitialSize; // when resizing a frame
    KoRect m_insRect;  // when creating a new frame
    KoRect m_boundingRect; // when moving frame(s)
    KoPoint m_hotSpot; // when moving frame(s)
    bool m_deleteMovingRect, m_frameMoved, m_frameResized;
    bool m_ctrlClickOnSelectedFrame;
    bool m_isClipart; // when inserting a picture
    QString m_pictureFilename; // when inserting a picture
    QSize m_pixmapSize; // when inserting a picture (pixmaps only)
    bool m_keepRatio;//when inserting a picture
    KoDocumentEntry m_partEntry; // when inserting a part

    // Table creation support.
    // Having this as a member variable allows to remember and reuse the last settings
    struct
    {
        unsigned int cols;
        unsigned int rows;
        int /*KWTableFrameSet::CellSize*/ width;
        int /*KWTableFrameSet::CellSize*/ height;
        bool floating;
    } m_table;
    KWTableFrameSet *curTable;
    KWFrameMoveCommand *cmdMoveFrame;
};

#endif
