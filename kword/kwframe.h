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
#include "kwimage.h"

#include <qrect.h>
#include <qpicture.h>
#include <qlist.h>

#include "border.h"
//#include <koMainWindow.h>

class KFormulaContainer;
class KFormulaView;
class KWAnchor;
class KWCanvas;
class KWChild;
class KWDocument;
class KWFrame;
class KWFrameSet;
class KWResizeHandle;
class KWTableFrameSet;
class KWTextDocument;
class KWView;
class QCursor;
class QPainter;
class QPoint;
class QRegion;
class QSize;

/**
 * This class represents a single frame.
 * A frame belongs to a frameset which states its contents.
 * A frame does NOT have contents, the frameset stores that.
 * A frame is really just a square that is used to place the content
 * of a frameset.
 */
class KWFrame : public QRect
{   // Note, if we inherited KoChild, we could get rid of quite some code
    // But KoChild has a QRect instead of using double, for its geometry though.
    // OTOH graphite's FxRect could be everything we need.
public:

    /**
     * Constructor
     * @param fs parent frameset
     * @param left, top, width, height coordinates of the frame
     * The page number will be automatically determined from the position of the frame.
     * @param ra the "runaround" setting, i.e. whether text should flow below the frame,
     * around the frame, or avoiding the frame on the whole horizontal band.
     * @param gap ...
     */
    KWFrame(KWFrameSet *fs, int left, int top, int width, int height, RunAround ra = RA_BOUNDINGRECT, KWUnit gap = 1.0 /*mm*/);
    /* Destructor */
    virtual ~KWFrame();

    /** a frame can be selected by the user clicking on it. The frame
     * remembers if it is selected
     * - which is actually pretty bad in terms of doc/view design (DF)
     */
    void setSelected( bool _selected );
    bool isSelected() { return selected; }

#if 0
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

#endif

    QCursor getMouseCursor( int mx, int my, bool table );

    KWUnit getRunAroundGap() { return runAroundGap; }
    void setRunAroundGap( KWUnit gap ) { runAroundGap = gap; }

    void setRunAround( RunAround _ra ) { runAround = _ra; }
    RunAround getRunAround() { return runAround; }

    FrameBehaviour getFrameBehaviour() { return frameBehaviour; }
    void setFrameBehaviour( FrameBehaviour fb ) { frameBehaviour = fb; }

    /* Frame duplication properties */
    SheetSide getSheetSide() { return sheetSide; }
    void setSheetSide( SheetSide ss ) { sheetSide = ss; }

    NewFrameBehaviour getNewFrameBehaviour() { return newFrameBehaviour; }
    void setNewFrameBehaviour( NewFrameBehaviour nfb ) { newFrameBehaviour = nfb; }

    /* Data stucture methods */
    KWFrameSet *getFrameSet() const { return frameSet; }
    void setFrameSet( KWFrameSet *fs ) { frameSet = fs; }

    bool isMostRight() { return mostRight; }
    void setMostRight( bool _mr ) { mostRight = _mr; }

    void setPageNum( int i ) { m_pageNum = i; }
    int pageNum() { return m_pageNum; }

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
    int m_pageNum;

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
    KWFrameSetEdit( KWFrameSet * fs, KWCanvas * canvas );
    virtual ~KWFrameSetEdit() {}

    KWFrameSet * frameSet() const { return m_fs; }
    KWCanvas * canvas() const { return m_canvas; }
    KWFrame * currentFrame() const { return m_currentFrame; }

    virtual KWFrameSetEdit* currentTextEdit(){return 0L;}

    /**
     * Paint this frameset in "has focus" mode (e.g. with a cursor)
     * See KWFrameSet for explanation about the arguments.
     */
    virtual void drawContents( QPainter *, const QRect &,
                               QColorGroup &, bool onlyChanged, bool resetChanged ) = 0;

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
    /**
     * The Frameset-Edit implementation is responsible for updating that one
     * (to the frame where the current "cursor" is)
     */
    KWFrame * m_currentFrame;
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
    KWFrameSet( KWDocument *doc );
    // destructor
    virtual ~KWFrameSet() {}

    virtual FrameType getFrameType() { return FT_BASE; }
    FrameInfo getFrameInfo() { return frameInfo; }
    void setFrameInfo( FrameInfo fi ) { frameInfo = fi; }

    bool isAHeader() const;
    bool isAFooter() const;
    bool isAWrongHeader( KoHFType t ) const;
    bool isAWrongFooter( KoHFType t ) const;

    // frame management
    //virtual void addFrame( KWFrame _frame );
    virtual void addFrame( KWFrame *_frame );
    virtual void delFrame( unsigned int _num );
    virtual void delFrame( KWFrame *frm, bool remove = TRUE );

    /** retrieve frame from x and y coords (absolute coords) */
    KWFrame *getFrame( int _x, int _y );
    KWFrame *getFrame( unsigned int _num );
    virtual const QList<KWFrame> &frameIterator() const { return frames; }
    int getFrameFromPtr( KWFrame *frame );
    unsigned int getNumFrames() { return frames.count(); }

    /** Create a framesetedit object to edit this frameset in @p canvas */
    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * ) { return 0L; }

    /**
     * Paint the borders for this frameset.
     * @param painter The painter in which to draw the contents of the frameset
     * @param crect The rectangle (in "contents coordinates") to be painted
     * @param region The region is modified to subtract the areas painted, thus
     *               allowing the caller to detrmine which areas remain to be painted.
     */
    virtual void drawBorders( QPainter *painter, const QRect &crect, QRegion &region );

    /**
     * Paint this frameset
     * When the frameset is being edited, KWFrameSetEdit's drawContents is called instead.
     * @param painter The painter in which to draw the contents of the frameset
     * @param crect The rectangle (in "contents coordinates") to be painted
     * @param cg The colorgroup from which to get the colors
     * @param onlyChanged If true, only redraw what has changed (see KWCanvas::repaintChanged)
     * @param resetChanged If true, set the changed flag to false after drawing.
     *
     * The way this "onlyChanged/resetChanged" works is: when something changes,
     * all views are asked to redraw themselves with onlyChanged=true.
     * But all views except the last one shouldn't reset the changed flag to false,
     * otherwise the other views wouldn't repaint anything.
     * So resetChanged is called with "false" for all views except the last one,
     * and with "true" for the last one, so that it resets the flag.
     */
    virtual void drawContents( QPainter *painter, const QRect &crect,
                               QColorGroup &cg, bool onlyChanged, bool resetChanged ) = 0;

    /**
     * Called when our frames change, or when another frameset's frames change.
     * Framesets can reimplement it, but should always call the parent method.
     */
    virtual void updateFrames();

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

    /** returns page number of the numbered frame */
    int getPageOfFrame( int i ) { return frames.at( i )->pageNum(); }

    /** Apply the new zoom/resolution - values are to be taken from kWordDocument() */
    virtual void zoom() {}

    /** Called once the frameset has been completely loaded or constructed.
     * The default implementation calls updateFrames() and zoom(). Call the parent :) */
    virtual void finalize();

    KWDocument* kWordDocument() const { return m_doc; }

    // only used for headers and footers...
    void setCurrent( int i ) { current = i; }
    int getCurrent() { return current; }

    /** Make this frameset anchored (floating) */
    void setAnchor( KWAnchor * anchor ) { m_anchor = anchor; /* TODO... */ }

    /** make this frameset part of a groupmanager
     * @see KWTableFrameSet
     */
    void setGroupManager( KWTableFrameSet *gm ) { grpMgr = gm; }
    KWTableFrameSet *getGroupManager() { return grpMgr; }

    /** table headers can created by the groupmanager, we store the fact that
     this is one in here. */
    void setIsRemoveableHeader( bool _h ) { removeableHeader = _h; }
    bool isRemoveableHeader() { return removeableHeader; }

    /** returns if one of our frames has been selected. */
    bool hasSelectedFrame();

    /**
     * Returns true if the frameset is visible.
     * A frameset is visible if setVisible(false) wasn't called,
     * but also, for a header frameset, if m_doc->isHeaderVisible is true, etc.
     * For an "even pages header" frameset, the corresponding headerType setting
     * must be selected (i.e. different headers for even and odd pages).
     */
    virtual bool isVisible();

    /** get the visibility of the frameset. */
    void setVisible( bool v ) { visible = v; }

    QString getName() const { return name; }
    void setName( const QString &_name ) { name = _name; }

signals:

    // Emitted when something has changed in this frameset,
    // so that all views repaint it. KWDocument connects to it,
    // and KWCanvas connects to KWDocument.
    void repaintChanged( KWFrameSet * frameset );

protected:

    // Determine the clipping rectangle for drawing the contents of @p frame with @p painter
    // in the rectangle delimited by @p crect.
    QRegion frameClipRegion( QPainter * painter, KWFrame *frame, const QRect & crect );

    KWDocument *m_doc;            // Document
    QList<KWFrame> frames;        // Our frames
    QList<KWFrame> m_framesOnTop; // List of frames on top of us, those we shouldn't overwrite

    FrameInfo frameInfo;
    int current;
    KWTableFrameSet *grpMgr;
    bool removeableHeader, visible;
    QString name;
    KWAnchor * m_anchor;
};

/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/

class KWPictureFrameSet : public KWFrameSet
{
public:
    KWPictureFrameSet( KWDocument *_doc )
        : KWFrameSet( _doc ) {}
    virtual ~KWPictureFrameSet();

    virtual FrameType getFrameType() { return FT_PICTURE; }

    void setImage( const KWImage &image ) { m_image = image; }
    KWImage image() const { return m_image; }

    QString key() const { return m_image.key(); }

    void setFileName( const QString &_filename, const QSize &_imgSize );
    void setSize( QSize _imgSize );

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

    virtual void drawContents( QPainter *painter, const QRect & crect,
                               QColorGroup &, bool onlyChanged, bool resetChanged );

protected:
    KWImage m_image;
};

/******************************************************************/
/* Class: KWPartFrameSet                                          */
/******************************************************************/

class KWPartFrameSet : public KWFrameSet
{
    Q_OBJECT
public:
    KWPartFrameSet( KWDocument *_doc, KWChild *_child );
    virtual ~KWPartFrameSet();

    virtual FrameType getFrameType() { return FT_PART; }

    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * );

    KWChild *getChild() { return child; }

    virtual void updateFrames();

    void drawContents( QPainter * p, const QRect & crect,
                       QColorGroup &, bool onlyChanged, bool resetChanged );

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

protected slots:
    void slotChildChanged();

protected:
    KWChild *child;
    bool m_lock;
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

    void drawContents( QPainter *, const QRect &,
                       QColorGroup &, bool, bool );

    // Events forwarded by the canvas (when being in "edit" mode)
    virtual void mousePressEvent( QMouseEvent * );
    virtual void mouseDoubleClickEvent( QMouseEvent * );
};

/******************************************************************/
/* Class: KWFormulaFrameSet                                       */
/******************************************************************/

class KWFormulaFrameSet : public KWFrameSet
{
    Q_OBJECT
public:
    KWFormulaFrameSet( KWDocument *_doc );
    KWFormulaFrameSet( KWDocument *_doc, QWidget *parent );
    virtual ~KWFormulaFrameSet();

    void create( QWidget *parent );

    virtual FrameType getFrameType() { return FT_FORMULA; }

    virtual KWFrameSetEdit* createFrameSetEdit(KWCanvas*);

    /**
     * Paint this frameset
     * When the frameset is being edited, KWFrameSetEdit's drawContents is called instead.
     */
    virtual void drawContents(QPainter*, const QRect&,
                              QColorGroup&, bool onlyChanged, bool resetChanged);

    void drawContents(QPainter*, const QRect&,
                      QColorGroup&, bool onlyChanged, bool resetChanged,
                      KFormulaView* formulaView);

    virtual void updateFrames();

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

    /** Apply the new zoom/resolution - values are to be taken from kWordDocument() */
    virtual void zoom();

    KFormulaContainer* getFormula() const { return formula; }

    void setChanged() { m_changed = true; }

protected slots:

    void slotFormulaChanged(int width, int height);

private:
    KFormulaContainer* formula;
    bool m_changed;
};


class KWFormulaFrameSetEdit : public QObject, public KWFrameSetEdit
{
    Q_OBJECT
public:
    KWFormulaFrameSetEdit(KWFormulaFrameSet* fs, KWCanvas* canvas);
    virtual ~KWFormulaFrameSetEdit();

    KWFormulaFrameSet* formulaFrameSet() const
    {
        return static_cast<KWFormulaFrameSet*>(frameSet());
    }

    /**
     * Paint this frameset in "has focus" mode (e.g. with a cursor)
     */
    virtual void drawContents(QPainter*, const QRect&,
                              QColorGroup&, bool onlyChanged, bool resetChanged);

    // Events forwarded by the canvas (when being in "edit" mode)
    virtual void keyPressEvent(QKeyEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*); // only called if button is pressed
    virtual void mouseReleaseEvent(QMouseEvent*);
    //virtual void mouseDoubleClickEvent( QMouseEvent * ) {}
    //virtual void dragEnterEvent( QDragEnterEvent * ) {}
    //virtual void dragMoveEvent( QDragMoveEvent * ) {}
    //virtual void dragLeaveEvent( QDragLeaveEvent * ) {}
    //virtual void dropEvent( QDropEvent * ) {}
    virtual void focusInEvent();
    virtual void focusOutEvent();
    virtual void doAutoScroll(QPoint);
    virtual void copy();
    virtual void cut();
    virtual void paste();
    virtual void selectAll();

protected slots:

    /**
     * Make sure the cursor can be seen at its new position.
     */
    void cursorChanged( bool visible, bool selecting );

private:
    KFormulaView* formulaView;
};

#endif
