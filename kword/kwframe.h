/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef frame_h
#define frame_h

#include "defs.h"
#include "kwunit.h"

#include <qrect.h>
#include <qpicture.h>
#include <qlist.h>

#include "border.h"
#include <koMainWindow.h>

class KWFrame;
class KWDocument;
class KWChild;
class KWordFrame;
class KWGroupManager;
class KFormulaEdit;
class KWImage;
class KWResizeHandle;
class KWCanvas;
class QPoint;
class QSize;
class QCursor;
class QRegion;
class QPainter;
class QTextStream;
namespace Qt3 {
class QTextCursor;
}
using namespace Qt3;
class KWTextDocument;

class KWDocument;
class KWFrameSet;

/**
 * This class represents a single frame.
 * A frame belongs to a frameset which states its contents.
 * A frame does NOT have contents, the frameset stores that.
 * A frame is really just a square that is used to place the content
 * of a frameset.
 */
class KWFrame : public QRect
{
public:

    /**
     * Constructor
     * @param fs parent frameset
     * @param left, top, width, height coordinates of the frame
     * The page number will be automatically determined from the position of the frame.
     * @param ra ...
     * @param gap ...
     */
    KWFrame(KWFrameSet *fs, int left, int top, int width, int height, RunAround ra = RA_NO, KWUnit gap = 1.0 /*mm*/);
    /* Destructor */
    virtual ~KWFrame();

    /** a frame can be selected by the user clicking on it. The frame
     * remembers if it is selected
     * - which is actually pretty bad in terms of doc/view design (DF)
     */
    void setSelected( bool _selected );
    bool isSelected() { return selected; }

    /* Run around stuff */
    /** add an intersection to the list of intersections of this frame.
    * A frame can intersect another frame, since we don't want to print
    * such comments on top of each other we keep a list of intersections and use that
    * together with runaround setting to decide which of the 2 frames can use that space.
    * @param QRect the intersecting rectangle (can be a frame)
    */
    void addIntersect( QRect &_r );

    /** Returns if this frame has any intersections
    * @return boolean true if we have intersections.
    */
    bool hasIntersections() { return !intersections.isEmpty(); }

    /** Removes all intersections
    */
    void clearIntersects() { intersections.clear(); emptyRegionDirty = TRUE; }

    QRegion getEmptyRegion( bool useCached = TRUE );

    unsigned int getNextFreeYPos( unsigned int _y, unsigned int _h );

    QCursor getMouseCursor( int mx, int my, bool table );

    /** Calculate the left indent we have to make for a free spot to render in.
    * The test will be limited to a square (mostly one line) inside the frame. The
    * y provides the offset, the h the height of the line.
    * @param y absolute y coordinate. The y coordinate from where we wil start to test.
    * @param h height. The height in which we will test.
    */
    int getLeftIndent( int y, int h );

    /** Calculate the right indent we have to make for a free spot to render in.
    * The test will be limited to a square (mostly one line) inside the frame. The
    * y provides the offset, the h the height of the line.
    * @param y absolute y coordinate. The y coordinate from where we wil start to test.
    * @param h height. The height in which we will test.
    */
    int getRightIndent( int y, int h );

    KWUnit getRunAroundGap() { return runAroundGap; }
    void setRunAroundGap( KWUnit gap ) { runAroundGap = gap; }

    void setRunAround( RunAround _ra ) { runAround = _ra; }
    RunAround getRunAround() { return runAround; }

    FrameBehaviour getFrameBehaviour() { return frameBehaviour; }
    void setFrameBehaviour( FrameBehaviour fb ) { frameBehaviour = fb; }

    FrameInfo getFrameInfo();
    FrameType getFrameType();

    /* frame duplication properties */
    SheetSide getSheetSide() { return sheetSide; }
    void setSheetSide( SheetSide ss ) { sheetSide = ss; }

    NewFrameBehaviour getNewFrameBehaviour() { return newFrameBehaviour; }
    void setNewFrameBehaviour( NewFrameBehaviour nfb ) { newFrameBehaviour = nfb; }

    /* Data stucture methods */
    KWFrameSet *getFrameSet() const { return frameSet; }
    void setFrameSet( KWFrameSet *fs ) { frameSet = fs; }

    bool isMostRight() { return mostRight; }
    void setMostRight( bool _mr ) { mostRight = _mr; }

    void setPageNum( int i ) { pageNum = i; }
    int getPageNum() { return pageNum; }

    /* All borders can be custum drawn with their own colors etc. */
    Border &getLeftBorder() { return brd_left; }
    Border &getRightBorder() { return brd_right; }
    Border &getTopBorder() { return brd_top; }
    Border &getBottomBorder() { return brd_bottom; }

    Border getLeftBorder2() { return brd_left; }
    Border getRightBorder2() { return brd_right; }
    Border getTopBorder2() { return brd_top; }
    Border getBottomBorder2() { return brd_bottom; }

    void setLeftBorder( Border _brd ) { brd_left = _brd; }
    void setRightBorder( Border _brd ) { brd_right = _brd; }
    void setTopBorder( Border _brd ) { brd_top = _brd; }
    void setBottomBorder( Border _brd ) { brd_bottom = _brd; }

    /* Resize handles (in kwcanvas.h) are the dots that are drawn on selected
       frames, this creates and deletes then */
    void createResizeHandles();
    void createResizeHandlesForPage(KWCanvas *canvas);
    void removeResizeHandlesForPage(KWCanvas *canvas);
    void removeResizeHandles();
    void updateResizeHandles();

    QBrush getBackgroundColor() { return backgroundColor; }
    void setBackgroundColor( QBrush _color ) { backgroundColor = _color; }

    /** set left border size */
    void setBLeft( KWUnit b ) { bleft = b; }
    /** set right border size */
    void setBRight( KWUnit b ) { bright = b; }
    /** set top border size */
    void setBTop( KWUnit b ) { btop = b; }
    /** set bottom border size */
    void setBBottom( KWUnit b ) { bbottom = b; }

    /** get left border size */
    KWUnit getBLeft() { return bleft; }
    /** get right border size */
    KWUnit getBRight() { return bright; }
    /** get top border size */
    KWUnit getBTop() { return btop; }
    /** get bottom border size */
    KWUnit getBBottom() { return bbottom; }

    /** returns a copy of self */
    KWFrame *getCopy();

protected:
    SheetSide sheetSide;
    RunAround runAround;
    FrameBehaviour frameBehaviour;
    NewFrameBehaviour newFrameBehaviour;
    bool selected;
    KWUnit runAroundGap;
    bool mostRight;
    int pageNum;

    QList<QRect> intersections;
    QRegion emptyRegion;
    bool emptyRegionDirty;

    Border brd_left, brd_right, brd_top, brd_bottom;
    QBrush backgroundColor;

    KWUnit bleft, bright, btop, bbottom;

private:
    QList<KWResizeHandle> handles;
    KWFrame &operator=( const KWFrame &_frame );
    KWFrame ( const KWFrame &_frame );
    KWFrameSet *frameSet;
};

/**
 * This object is created to edit a particular frameset in a particular view
 * The view's canvas creates it, and destroys it.
 */
class KWFrameSetEdit
{
public:
    KWFrameSetEdit( KWFrameSet * fs, KWCanvas * canvas ) : m_fs(fs), m_canvas(canvas) {}
    virtual ~KWFrameSetEdit() {}

    KWFrameSet * frameSet() const { return m_fs; }
    KWCanvas * canvas() const { return m_canvas; }

    /**
     * Paint this frameset in "has focus" mode (e.g. with a cursor)
     */
    virtual void drawContents( QPainter *, int /*cx*/, int /*cy*/, int /*cw*/, int /*ch*/,
                               QColorGroup &, bool /*onlyChanged*/ ) {}

    // Events forwarded by the canvas (when being in "edit" mode)
    virtual void keyPressEvent( QKeyEvent * ) {}
    virtual void mousePressEvent( QMouseEvent * ) {}
    virtual void mouseMoveEvent( QMouseEvent * ) {} // only called if button is pressed
    virtual void mouseReleaseEvent( QMouseEvent * ) {}
    virtual void mouseDoubleClickEvent( QMouseEvent * ) {}
    virtual void dragEnterEvent( QDragEnterEvent * ) {}
    virtual void dragMoveEvent( QDragMoveEvent * ) {}
    virtual void dragLeaveEvent( QDragLeaveEvent * ) {}
    virtual void dropEvent( QDropEvent * ) {}
    virtual void focusInEvent() {}
    virtual void focusOutEvent() {}
    virtual void doAutoScroll( QPoint ) {}
    virtual void copy() {}
    virtual void cut() {}
    virtual void paste() {}
    virtual void selectAll() {}

protected:
    KWFrameSet * m_fs;
    KWCanvas * m_canvas;
};

/**
 * Class: KWFrameSet
 * Base type, a frameset holds content as well as frames to show that
 * content.
 * The different types of content are implemented in the different
 * types of frameSet implementations (see below)
 * @see KWTextFrameSet, KWPartFramSet, KWPictureFrameSet,
 *      KWPartFrameSet, KWFormulaFrameSet
 */
class KWFrameSet : public QObject
{
    Q_OBJECT
public:
    // constructor
    KWFrameSet( KWDocument *_doc );
    // destructor
    virtual ~KWFrameSet() {; }

    virtual FrameType getFrameType() { return FT_BASE; }
    virtual FrameInfo getFrameInfo() { return frameInfo; }
    void setFrameInfo( FrameInfo fi ) { frameInfo = fi; }

    // frame management
    //virtual void addFrame( KWFrame _frame );
    virtual void addFrame( KWFrame *_frame );
    virtual void delFrame( unsigned int _num );
    virtual void delFrame( KWFrame *frm, bool remove = TRUE );

    /** retrieve frame from x and y coords (absolute coords) */
    KWFrame *getFrame( int _x, int _y );
    KWFrame *getFrame( unsigned int _num );
    QListIterator<KWFrame> frameIterator() { return QListIterator<KWFrame>(frames); }
    const QListIterator<KWFrame> frameIterator() const { return QListIterator<KWFrame>(frames); }
    int getFrameFromPtr( KWFrame *frame );
    unsigned int getNumFrames() { return frames.count(); }

    virtual bool isPTYInFrame( unsigned int /*_frame*/, unsigned int /*_ypos */ )
        { return true; }

    /** Create a framesetedit object to edit this frameset in @p canvas */
    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * ) { return 0L; }

    /**
     * Paint this frameset
     * When the frameset is being edited, KWFrameSetEdit's drawContents is called instead.
     */
    virtual void drawContents( QPainter *, int /*cx*/, int /*cy*/, int /*cw*/, int /*ch*/,
                               QColorGroup &, bool /*onlyChanged*/ )
    {}

    /** reshuffle frames so text is always displayed from top-left down and then right. */
    virtual void update() {; }

    /** relayout text in frames, so that it flows correctly around other frames */
    virtual void layout() {}

    /** returns true if we have a frame occupying that position */
    virtual bool contains( unsigned int mx, unsigned int my );

    /**
     * Return 1, if a frame gets selected which was not selected before,<br>
     * 2, if a frame gets selected which was already selected<br>
     * Also select the frame if simulate==false.
     */
    virtual int selectFrame( unsigned int mx, unsigned int my, bool simulate = false );
    virtual void deSelectFrame( unsigned int mx, unsigned int my );
    virtual QCursor getMouseCursor( unsigned int mx, unsigned int my );

    /** create XML to describe yourself */
    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

    int getNext( QRect _rect );
    /** returns page number of the numbered frame */
    int getPageOfFrame( int i ) { return frames.at( i )->getPageNum(); }

    KWDocument* kWordDocument() const { return doc; }

    // only used for headers and footers...
    void setCurrent( int i ) { current = i; }
    int getCurrent() { return current; }

    /** make this frameset part of a groupmanager
     * @see KWGroupManager
     */
    void setGroupManager( KWGroupManager *gm ) { grpMgr = gm; }
    KWGroupManager *getGroupManager() { return grpMgr; }

    /** table headers can created by the groupmanager, we store the fact that
     this is one in here. */
    void setIsRemoveableHeader( bool _h ) { removeableHeader = _h; }
    bool isRemoveableHeader() { return removeableHeader; }

    /** returns if one of our frames has been selected. */
    bool hasSelectedFrame();

    /** returns the visibility of the frameset. */
    bool isVisible() { return visible; }
    /** get the visibility of the frameset. */
    void setVisible( bool v ) { visible = v; }

    QString getName() const { return name; }
    void setName( const QString &_name ) { name = _name; }

protected:
    // document
    KWDocument *doc;

    // frames
    QList<KWFrame> frames;

    FrameInfo frameInfo;
    int current;
    KWGroupManager *grpMgr;
    bool removeableHeader, visible;
    QString name;

};

/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/

class KWPictureFrameSet : public KWFrameSet
{
public:
    KWPictureFrameSet( KWDocument *_doc )
        : KWFrameSet( _doc )
    { image = 0L; }
    virtual ~KWPictureFrameSet();

    virtual FrameType getFrameType() { return FT_PICTURE; }

    virtual void setImage( KWImage *_image )
    { image = _image; }
    void setFileName( QString _filename );
    void setFileName( QString _filename, QSize _imgSize );
    void setSize( QSize _imgSize );

    virtual KWImage* getImage()
    { return image; }
    QString getFileName() { return filename; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

    virtual void drawContents( QPainter *painter, int cx, int cy, int cw, int ch,
                               QColorGroup &, bool /*onlyChanged*/ );

protected:
    KWImage *image;
    QString filename;

};

/******************************************************************/
/* Class: KWPartFrameSet                                          */
/******************************************************************/

class KWPartFrameSet : public KWFrameSet
{
public:
    KWPartFrameSet( KWDocument *_doc, KWChild *_child );
    virtual ~KWPartFrameSet();

    virtual FrameType getFrameType() { return FT_PART; }

    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * );

    virtual QPicture *getPicture();

    void activate( QWidget *_widget );
    void deactivate();

    KWChild *getChild() { return child; }

    virtual void update();

    void drawContents( QPainter * p, int cx, int cy, int cw, int ch,
                       QColorGroup &, bool onlyChanged );

    //void enableDrawing( bool f ) { _enableDrawing = f; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

protected:
    KWordFrame *frame;
    KWChild *child;
    QPicture pic;

    //bool _enableDrawing;
};

class KWPartFrameSetEdit : public KWFrameSetEdit
{
public:
    KWPartFrameSetEdit( KWPartFrameSet * fs, KWCanvas * canvas )
        : KWFrameSetEdit( fs, canvas ) {}
    virtual ~KWPartFrameSetEdit();

    KWPartFrameSet * partFrameSet() const
    {
        return static_cast<KWPartFrameSet*>(frameSet());
    }

    // Events forwarded by the canvas (when being in "edit" mode)
    virtual void mousePressEvent( QMouseEvent * );
    virtual void mouseDoubleClickEvent( QMouseEvent * );
};

/******************************************************************/
/* Class: KWFormulaFrameSet                                       */
/******************************************************************/

class KWFormulaFrameSet : public KWFrameSet
{
public:
    KWFormulaFrameSet( KWDocument *_doc );
    KWFormulaFrameSet( KWDocument *_doc, QWidget *parent );
    virtual ~KWFormulaFrameSet();

    void create( QWidget *parent );

    virtual FrameType getFrameType() { return FT_FORMULA; }

    virtual QPicture *getPicture();

    virtual void activate( QWidget *_widget );
    virtual void deactivate();

    virtual void update();

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

    void insertChar( int c );
    void setFormat( const QFont &f, const QColor &c );

protected:
    KFormulaEdit *formulaEdit;
    QPicture *pic;
    QString text;
    QFont font;
    QColor color;

};

bool isAHeader( FrameInfo fi );
bool isAFooter( FrameInfo fi );
bool isAWrongHeader( FrameInfo fi, KoHFType t );
bool isAWrongFooter( FrameInfo fi, KoHFType t );

#endif
