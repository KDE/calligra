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
#include "kwimage.h"

#include <koRect.h>
#include <qbrush.h>
#include <qlist.h>

#include "border.h"

namespace KFormula {
    class KFormulaContainer;
    class KFormulaView;
}

class KCommand;
class KWAnchor;
class KWCanvas;
class KWChild;
class KWDocument;
class KWFrame;
class KWFrameSet;
class KWResizeHandle;
class KWTableFrameSet;
class KWTextDocument;
class KWTextFrameSet;
class KWTextParag;
class KWView;
class KWViewMode;
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
class KWFrame : public KoRect
{
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
    KWFrame(KWFrameSet *fs, double left, double top, double width, double height,
            RunAround ra = RA_BOUNDINGRECT, double gap = MM_TO_POINT( 1.0 ));
    /* Destructor */
    virtual ~KWFrame();

    /** a frame can be selected by the user clicking on it. The frame
     * remembers if it is selected
     * - which is actually pretty bad in terms of doc/view design (DF)
     */
    void setSelected( bool _selected );
    bool isSelected() { return selected; }

    QCursor getMouseCursor( const KoPoint& docPoint, bool table, QCursor defaultCursor );

    double getRunAroundGap() { return runAroundGap; }
    void setRunAroundGap( double gap ) { runAroundGap = gap; }

    void setRunAround( RunAround _ra ) { runAround = _ra; }
    RunAround getRunAround() { return runAround; }

    /* What happens when the [text] frame is full (autoextend, create new frame, ignore) */
    FrameBehaviour getFrameBehaviour() { return frameBehaviour; }
    void setFrameBehaviour( FrameBehaviour fb ) { frameBehaviour = fb; }

    /* Frame duplication properties */
    SheetSide getSheetSide() { return sheetSide; }
    void setSheetSide( SheetSide ss ) { sheetSide = ss; }

    /* What happens on new page (reconnect, nofollowup, copy) */
    NewFrameBehaviour getNewFrameBehaviour() { return newFrameBehaviour; }
    void setNewFrameBehaviour( NewFrameBehaviour nfb ) { newFrameBehaviour = nfb; }

    /* Drawing property: if isCopy, this frame is a copy of the previous frame in the frameset */
    bool isCopy() { return m_bCopy; }
    void setCopy( bool copy ) { m_bCopy = copy; }

    /* Data stucture methods */
    KWFrameSet *getFrameSet() const { return frameSet; }
    void setFrameSet( KWFrameSet *fs ) { frameSet = fs; }

    /* The page on which this frame is */
    void setPageNum( int i ) { m_pageNum = i; }
    int pageNum() { return m_pageNum; }

    /* All borders can be custom drawn with their own colors etc. */
    const Border &getLeftBorder() const { return brd_left; }
    const Border &getRightBorder() const { return brd_right; }
    const Border &getTopBorder() const { return brd_top; }
    const Border &getBottomBorder() const { return brd_bottom; }

    void setLeftBorder( Border _brd ) { brd_left = _brd; }
    void setRightBorder( Border _brd ) { brd_right = _brd; }
    void setTopBorder( Border _brd ) { brd_top = _brd; }
    void setBottomBorder( Border _brd ) { brd_bottom = _brd; }

    /** Return the _zoomed_ rectangle for this frame, including the border - for drawing */
    QRect outerRect() const;
    /** Return the unzoomed rectangle, including the border, for the frames-on-top list.
        The default border of size 1-pixel that is drawn on screen is _not_ included here
        [since it depends on the zoom] */
    KoRect outerKoRect() const;

    /* Resize handles (in kwcanvas.h) are the dots that are drawn on selected
       frames, this creates and deletes them */
    void createResizeHandles();
    void createResizeHandlesForPage(KWCanvas *canvas);
    void removeResizeHandlesForPage(KWCanvas *canvas);
    void removeResizeHandles();
    void updateResizeHandles();

    QBrush getBackgroundColor() const { return backgroundColor; }
    void setBackgroundColor( QBrush _color ) { backgroundColor = _color; }

    // For text frames only
    void setInternalY( int y ) { m_internalY = y; }
    int internalY() const { return m_internalY; }

    /** set left margin size */
    void setBLeft( double b ) { bleft = b; }
    /** set right margin size */
    void setBRight( double b ) { bright = b; }
    /** set top margin size */
    void setBTop( double b ) { btop = b; }
    /** set bottom margin size */
    void setBBottom( double b ) { bbottom = b; }

    /** get left margin size */
    double getBLeft() { return bleft; }
    /** get right margin size */
    double getBRight() { return bright; }
    /** get top margin size */
    double getBTop() { return btop; }
    /** get bottom margin size */
    double getBBottom() { return bbottom; }

    /** returns a copy of self */
    KWFrame *getCopy();

private:
    SheetSide sheetSide;
    RunAround runAround;
    FrameBehaviour frameBehaviour;
    NewFrameBehaviour newFrameBehaviour;
    double runAroundGap;
    double bleft, bright, btop, bbottom; // margins

    bool m_bCopy;
    bool selected;
    int m_pageNum;
    int m_internalY; // for text frames only

    QBrush backgroundColor;
    Border brd_left, brd_right, brd_top, brd_bottom;

    QList<KWResizeHandle> handles;
    KWFrameSet *frameSet;

    // Prevent operator= and copy constructor
    KWFrame &operator=( const KWFrame &_frame );
    KWFrame ( const KWFrame &_frame );
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

    /**
     * Return the current most-low-level text edit object
     */
    virtual KWFrameSetEdit* currentTextEdit() { return 0L; }

    virtual QString getPopupName() { return QString();}

    /**
     * Called before destruction, when terminating edition - use to e.g. hide cursor
     */
    virtual void terminate() {}

    /**
     * Paint this frameset in "has focus" mode (e.g. with a cursor)
     * See KWFrameSet for explanation about the arguments.
     * Most framesets don't need to reimplement that (the KWFrameSetEdit gets passed to drawFrame)
     */
    virtual void drawContents( QPainter *, const QRect &,
                               QColorGroup &, bool onlyChanged, bool resetChanged,
                               KWViewMode *viewMode );

    // Events forwarded by the canvas (when being in "edit" mode)
    virtual void keyPressEvent( QKeyEvent * ) {}
    virtual void mousePressEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {}
    virtual void mouseMoveEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {} // only called if button is pressed
    virtual void mouseReleaseEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {}
    virtual void mouseDoubleClickEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {}
    virtual void dragEnterEvent( QDragEnterEvent * ) {}
    virtual void dragMoveEvent( QDragMoveEvent *, const QPoint &, const KoPoint & ) {}
    virtual void dragLeaveEvent( QDragLeaveEvent * ) {}
    virtual void dropEvent( QDropEvent *, const QPoint &, const KoPoint & ) {}
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
    virtual ~KWFrameSet();

    virtual FrameType getFrameType() { return FT_BASE; }
    FrameInfo getFrameInfo() { return frameInfo; }
    void setFrameInfo( FrameInfo fi ) { frameInfo = fi; }

    bool isAHeader() const;
    bool isAFooter() const;
    bool isHeaderOrFooter() const { return isAHeader() || isAFooter(); }

    bool isAWrongHeader( KoHFType t ) const;
    bool isAWrongFooter( KoHFType t ) const;

    // frame management
    virtual void addFrame( KWFrame *_frame, bool recalc = true );
    virtual void delFrame( unsigned int _num );
    virtual void delFrame( KWFrame *frm, bool remove = true );
    void deleteAllFrames();
    void deleteAllCopies(); // for headers/footers only

    /** retrieve frame from x and y coords (unzoomed coords) */
    KWFrame *frameAtPos( double _x, double _y );
    /** return a frame if nPoint in on one of its borders */
    KWFrame *frameByBorder( const QPoint & nPoint );

    /** get a frame by number */
    KWFrame *getFrame( unsigned int _num );

    /** get the frame whose settings apply for @p frame
        (usually @p frame, but can also be the real frame if frame is a copy) */
    static KWFrame * settingsFrame(KWFrame* frame);

    /* Iterator over the child frames */
    const QList<KWFrame> &frameIterator() const { return frames; }
    /* Get frame number */
    int getFrameFromPtr( KWFrame *frame );
    /* Get number of child frames */
    unsigned int getNumFrames() const { return frames.count(); }

    /** True if the frameset was deleted (but not destroyed, since it's in the undo/redo) */
    bool isDeleted() const { return frames.isEmpty(); }

    /** Create a framesetedit object to edit this frameset in @p canvas */
    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * ) { return 0L; }

    /**
     * @param emptyRegion The region is modified to subtract the areas painted, thus
     *                    allowing the caller to determine which areas remain to be painted.
     */
    virtual void createEmptyRegion( QRegion & emptyRegion, KWViewMode *viewMode );

    /**
     * Paint the borders for one frame of this frameset.
     * @param painter The painter in which to draw the contents of the frameset
     * @param frame The frame to be drawn
     * @param settingsFrame The frame from which we take the settings (usually @p frame, but not with Copy behaviour)
     * @param crect The rectangle (in "contents coordinates") to be painted
     * @param viewMode The current view mode ( for coord transformation )
     */
    virtual void drawFrameBorder( QPainter *painter, KWFrame *frame, KWFrame *settingsFrame, const QRect &crect, KWViewMode *viewMode );

    /**
     * Paint this frameset
     * @param painter The painter in which to draw the contents of the frameset
     * @param crect The rectangle (in "contents coordinates") to be painted
     * @param cg The colorgroup from which to get the colors
     * @param onlyChanged If true, only redraw what has changed (see KWCanvas::repaintChanged)
     * @param resetChanged If true, set the changed flag to false after drawing.
     * @param edit If set, this frameset is being edited, so a cursor is needed.
     *
     * The way this "onlyChanged/resetChanged" works is: when something changes,
     * all views are asked to redraw themselves with onlyChanged=true.
     * But all views except the last one shouldn't reset the changed flag to false,
     * otherwise the other views wouldn't repaint anything.
     * So resetChanged is called with "false" for all views except the last one,
     * and with "true" for the last one, so that it resets the flag.
     *
     * Framesets shouldn't reimplement this one in theory [but KWTableFrameSet has to].
     */
    virtual void drawContents( QPainter *painter, const QRect &crect,
                               QColorGroup &cg, bool onlyChanged, bool resetChanged,
                               KWFrameSetEdit *edit, KWViewMode *viewMode );

    /**
     * Draw a particular frame of this frameset.
     * This is called by drawContents and is what framesets must reimplement.
     */
    virtual void drawFrame( KWFrame *frame, QPainter *painter, const QRect &crect,
                            QColorGroup &cg, bool onlyChanged, bool resetChanged,
                            KWFrameSetEdit *edit ) = 0;

    /**
     * Called when our frames change, or when another frameset's frames change.
     * Framesets can reimplement it, but should always call the parent method.
     */
    virtual void updateFrames();

    /** relayout text in frames, so that it flows correctly around other frames */
    virtual void layout() {}
    virtual void invalidate() {}

    /** returns true if we have a frame occupying that position */
    virtual bool contains( double mx, double my );

    virtual bool getMouseCursor( const QPoint &nPoint, bool controlPressed, QCursor & cursor, bool canMove );

    /** create XML to describe yourself */
    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

    /** returns page number of the numbered frame */
    int getPageOfFrame( int i ) { return frames.at( i )->pageNum(); }

    /** Apply the new zoom/resolution - values are to be taken from kWordDocument() */
    virtual void zoom();

    /** Called once the frameset has been completely loaded or constructed.
     * The default implementation calls updateFrames() and zoom(). Call the parent :) */
    virtual void finalize();

    virtual void statistics( ulong & /*charsWithSpace*/, ulong & /*charsWithoutSpace*/, ulong & /*words*/, ulong & /*sentences*/ ) {}

    KWDocument* kWordDocument() const { return m_doc; }

    // Return true if page @p num can be removed, as far as this frameset is concerned
    virtual bool canRemovePage( int num );

    // only used for headers and footers...
    void setCurrent( int i ) { m_current = i; }
    int getCurrent() { return m_current; }

    //Note: none of those floating-frameset methods creates undo/redo
    //They are _called_ by the undo/redo commands.

    /** Make this frameset floating, as close to its current position as possible. */
    void setFloating();
    /** Make this frameset floating, with the anchor at @p paragId,@p index in the text frameset @p textfs */
    void setAnchored( KWTextFrameSet* textfs, int paragId, int index, bool placeHolderExists = false );
    /** Note that this frameset has been made floating already, and store anchor position */
    void setAnchored( KWTextFrameSet* textfs );
    /** Make this frameset fixed, i.e. not anchored */
    void setFixed();
    /** Return true if this frameset is floating, false if it's fixed */
    bool isFloating() { return m_anchorTextFs; }
    /** Return the frameset in which our anchor is - assuming isFloating() */
    KWTextFrameSet * anchorFrameset() const { return m_anchorTextFs; }
    /** Return the anchor object for this frame number */
    KWAnchor * findAnchor( int frameNum );

    /** Create an anchor for the floating frame identified by frameNum */
    virtual KWAnchor * createAnchor( KWTextDocument * textdoc, int frameNum );

    /** Move the frame frameNum to the given position - this is called when
        the frame is anchored and the anchor moves (see KWAnchor). */
    virtual void moveFloatingFrame( int frameNum, const KoPoint &position );
    /** Get the [zoomed] size of the "floating frame" identified by frameNum.
        By default a real frame but not for tables. */
    virtual QSize floatingFrameSize( int frameNum );
    /** Store command for creating an anchored object */
    virtual KCommand * anchoredObjectCreateCommand( int frameNum );
    /** Store command for deleting an anchored object */
    virtual KCommand * anchoredObjectDeleteCommand( int frameNum );

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
    bool isVisible() const;

    /** get the visibility of the frameset. */
    void setVisible( bool v );

    /** get/set frameset name. For tables in particular, this _must_ be unique */
    QString getName() const { return m_name; }
    void setName( const QString &_name ) { m_name = _name; }

#ifndef NDEBUG
    virtual void printDebug();
    virtual void printDebug( KWFrame * );
#endif

signals:

    // Emitted when something has changed in this frameset,
    // so that all views repaint it. KWDocument connects to it,
    // and KWCanvas connects to KWDocument.
    void repaintChanged( KWFrameSet * frameset );

protected:

    // Determine the clipping rectangle for drawing the contents of @p frame with @p painter
    // in the rectangle delimited by @p crect.
    QRegion frameClipRegion( QPainter * painter, KWFrame *frame, const QRect & crect,
                             KWViewMode * viewMode, bool onlyChanged );

    void deleteAnchor( KWAnchor * anchor );
    virtual void deleteAnchors();
    virtual void createAnchors( KWTextParag * parag, int index, bool placeHolderExists = false );

    KWDocument *m_doc;            // Document
    QList<KWFrame> frames;        // Our frames
    struct FrameOnTop {
        FrameOnTop() {} // for QValueList
        FrameOnTop( const KoRect & r, KWFrame * f )
            : intersection( r ), frame( f ) {}
        KoRect intersection;
        KWFrame * frame;
    };
    QValueList<FrameOnTop> m_framesOnTop; // List of frames on top of us, those we shouldn't overwrite

    FrameInfo frameInfo;
    int m_current; // used for headers and footers, not too sure what it means
    KWTableFrameSet *grpMgr;
    bool removeableHeader, visible;
    QString m_name;
    KWTextFrameSet * m_anchorTextFs;
};

/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/

class KWPictureFrameSet : public KWFrameSet
{
public:
    KWPictureFrameSet( KWDocument *_doc, const QString & name );
    virtual ~KWPictureFrameSet();

    virtual FrameType getFrameType() { return FT_PICTURE; }

    void setImage( const KWImage &image ) { m_image = image; }
    KWImage image() const { return m_image; }

    QString key() const { return m_image.key(); }

    void setFileName( const QString &_filename, const QSize &_imgSize );
    void setSize( QSize _imgSize );

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

    virtual void drawFrame( KWFrame *frame, QPainter *painter, const QRect & crect,
                            QColorGroup &, bool onlyChanged, bool resetChanged,
                            KWFrameSetEdit *edit = 0L );

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
    KWPartFrameSet( KWDocument *_doc, KWChild *_child, const QString & name );
    virtual ~KWPartFrameSet();

    virtual FrameType getFrameType() { return FT_PART; }

    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * );

    KWChild *getChild() { return child; }

    virtual void updateFrames();

    virtual void drawFrame( KWFrame * frame, QPainter * p, const QRect & crect,
                            QColorGroup &, bool onlyChanged, bool resetChanged,
                            KWFrameSetEdit *edit = 0L );

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

    // Events forwarded by the canvas (when being in "edit" mode)
    virtual void mousePressEvent( QMouseEvent *, const QPoint &, const KoPoint & );
    virtual void mouseDoubleClickEvent( QMouseEvent *, const QPoint &, const KoPoint & );
};

/******************************************************************/
/* Class: KWFormulaFrameSet                                       */
/******************************************************************/

class KWFormulaFrameSet : public KWFrameSet
{
    Q_OBJECT
public:
    KWFormulaFrameSet( KWDocument *_doc, const QString & name );
    virtual ~KWFormulaFrameSet();

    virtual FrameType getFrameType() { return FT_FORMULA; }

    virtual KWFrameSetEdit* createFrameSetEdit(KWCanvas*);

    /**
     * Paint this frameset
     */
    virtual void drawFrame(KWFrame *, QPainter*, const QRect&,
                           QColorGroup&, bool onlyChanged, bool resetChanged,
                           KWFrameSetEdit *edit = 0L);

    virtual void updateFrames();

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &attributes );

    /** Apply the new zoom/resolution - values are to be taken from kWordDocument() */
    virtual void zoom();

    KFormula::KFormulaContainer* getFormula() const { return formula; }

    void setChanged() { m_changed = true; }

protected slots:

    void slotFormulaChanged(int width, int height);

private:
    KFormula::KFormulaContainer* formula;
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

    KFormula::KFormulaView* getFormulaView() const
    {
        return formulaView;
    }

    virtual QString getPopupName() { return "Formula";}

    // Events forwarded by the canvas (when being in "edit" mode)
    virtual void keyPressEvent(QKeyEvent*);
    virtual void mousePressEvent(QMouseEvent*, const QPoint & n, const KoPoint & d );
    virtual void mouseMoveEvent(QMouseEvent*, const QPoint & n, const KoPoint & d); // only called if button is pressed
    virtual void mouseReleaseEvent(QMouseEvent*, const QPoint & n, const KoPoint & d);
    //virtual void mouseDoubleClickEvent( QMouseEvent *, const QPoint & n, const KoPoint & d ) {}
    //virtual void dragEnterEvent( QDragEnterEvent * ) {}
    //virtual void dragMoveEvent( QDragMoveEvent *, const QPoint &, const KoPoint & ) {}
    //virtual void dragLeaveEvent( QDragLeaveEvent * ) {}
    //virtual void dropEvent( QDropEvent *, const QPoint &, const KoPoint & ) {}
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
    KFormula::KFormulaView* formulaView;
};

#endif
