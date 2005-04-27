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

#include <qdom.h>

#include "defs.h"
#include <koPicture.h>
#include <koRect.h>
#include <qbrush.h>
#include <qptrlist.h>
#include <qptrvector.h>
#include "kwstyle.h"
#include "koborder.h"


class KoStyleStack;
class KCommand;
class KWAnchor;
class KWCanvas;
class KWChild;
class KWDocument;
class KWFrame;
class KWFrameSet;
class KWTextFrameSet;
class KWResizeHandle;
class KWTableFrameSet;
class KWTextDocument;
class KWTextParag;
class KWView;
class KWViewMode;
class QCursor;
class QPainter;
class QPoint;
class QRegion;
class QSize;
class QProgressDialog;
class KWTextFrameSet;
class KWFramePartMoveCommand;
class KWordFrameSetIface;
class DCOPObject;
class KoXmlWriter;
class KoSavingContext;
class KoTextDocument;

/**
 * small utility class representing a sortable (by z-order) list of frames
 * you can use sort() and inSort(item)
 **/
class KWFrameList: public QPtrList<KWFrame>
{
protected:
	virtual int compareItems(QPtrCollection::Item a, QPtrCollection::Item b);
};

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
    /** Runaround types
     * RA_NO = No run around, all text is just printed.
     * RA_BOUNDINGRECT = run around the square of this frame.
     * RA_SKIP = stop running text on the whole horizontal space this frame occupies.
     */
    enum RunAround { RA_NO = 0, RA_BOUNDINGRECT = 1, RA_SKIP = 2 };

    /** Runaround side - only meaningful when RA_BOUNDINGRECT is used
     */
    enum RunAroundSide { RA_BIGGEST = 0, RA_LEFT = 1, RA_RIGHT = 2 };

    /**
     * Constructor
     * @param fs parent frameset
     * @param left, top, width, height coordinates of the frame
     * The page number will be automatically determined from the position of the frame.
     * @param ra the "runaround" setting, i.e. whether text should flow below the frame,
     * around the frame, or avoiding the frame on the whole horizontal band.
     */
    KWFrame(KWFrameSet *fs, double left, double top, double width, double height,
            RunAround ra = RA_BOUNDINGRECT);
    KWFrame(KWFrame * frame);
    /** Destructor
     */
    virtual ~KWFrame();

    /** a frame can be selected by the user clicking on it. The frame
     * remembers if it is selected
     * - which is actually pretty bad in terms of doc/view design (DF)
     */
    void setSelected( bool _selected );
    bool isSelected()const { return m_selected; }

    MouseMeaning getMouseMeaning( const KoPoint& docPoint, MouseMeaning defaultMeaning );

    double runAroundLeft() const { return m_runAroundLeft; }
    double runAroundRight() const { return m_runAroundRight; }
    double runAroundTop() const { return m_runAroundTop; }
    double runAroundBottom() const { return m_runAroundBottom; }

    void setRunAroundGap( double left, double right, double top, double bottom ) {
        m_runAroundLeft = left;
        m_runAroundRight = right;
        m_runAroundTop = top;
        m_runAroundBottom = bottom;
    }

    RunAround runAround()const { return m_runAround; }
    void setRunAround( RunAround _ra ) { m_runAround = _ra; }

    RunAroundSide runAroundSide() const { return m_runAroundSide; }
    void setRunAroundSide( RunAroundSide rs ) { m_runAroundSide = rs; }

    /** what should happen when the frame is full
     */
    enum FrameBehavior { AutoExtendFrame=0 , AutoCreateNewFrame=1, Ignore=2 };

    FrameBehavior frameBehavior()const { return m_frameBehavior; }
    void setFrameBehavior( FrameBehavior fb ) { m_frameBehavior = fb; }

    /* Frame duplication properties */

    /** This frame will only be copied to:
     *   AnySide, OddSide or EvenSide
     */
    enum SheetSide { AnySide=0, OddSide=1, EvenSide=2};
    SheetSide sheetSide()const { return m_sheetSide; }
    void setSheetSide( SheetSide ss ) { m_sheetSide = ss; }

    /** What happens on new page
     * (create a new frame and reconnect, no followup, make copy) */
    enum NewFrameBehavior { Reconnect=0, NoFollowup=1, Copy=2 };
    NewFrameBehavior newFrameBehavior()const { return m_newFrameBehavior; }
    void setNewFrameBehavior( NewFrameBehavior nfb ) { m_newFrameBehavior = nfb; }

    /** Drawing property: if isCopy, this frame is a copy of the previous frame in the frameset
     */
    bool isCopy()const { return m_bCopy; }
    void setCopy( bool copy ) { m_bCopy = copy; }

    /** Data stucture methods
     */
    KWFrameSet *frameSet() const { return m_frameSet; }
    void setFrameSet( KWFrameSet *fs ) { m_frameSet = fs; }

    /** The page on which this frame is (0 based)
     */
    int pageNum() const;
    /** Same as pageNum(), but works if the frame hasn't been added to a frameset yet
     */
    int pageNum( KWDocument* doc ) const;

    /** The z-order of the frame, relative to the other frames on the same page
     */
    void setZOrder( int z ) { m_zOrder = z; }
    int zOrder() const { return m_zOrder; }

    /** For KWFrameSet::updateFrames only. Clear list of frames on top of this one.
     */
    void clearFramesOnTop() { m_framesOnTop.clear(); }
    void clearFramesBelow() { m_framesBelow.clear(); }

    /** For KWFrameSet::updateFrames only. Add a frame on top of this one.
     * Note that order doesn't matter in that list, it's for clipping only. */
    void addFrameOnTop( KWFrame* fot ) { m_framesOnTop.append( fot ); }

    /**
     * order DOES matter for this one tho. this one is for transparency & selection.
     **/
    void addFrameBelow( KWFrame* fbl ) { m_framesBelow.append( fbl ); }
    void sortFramesBelow() { m_framesBelow.sort(); }

    const QPtrList<KWFrame>& framesOnTop() const { return m_framesOnTop; }
    const QPtrList<KWFrame>& framesBelow() const { return m_framesBelow; }

    /** All borders can be custom drawn with their own colors etc.
     */
    const KoBorder &leftBorder() const { return m_borderLeft; }
    const KoBorder &rightBorder() const { return m_borderRight; }
    const KoBorder &topBorder() const { return m_borderTop; }
    const KoBorder &bottomBorder() const { return m_borderBottom; }


    void setLeftBorder( KoBorder _brd ) { m_borderLeft = _brd; }
    void setRightBorder( KoBorder _brd ) { m_borderRight = _brd; }
    void setTopBorder( KoBorder _brd ) { m_borderTop = _brd; }
    void setBottomBorder( KoBorder _brd ) { m_borderBottom = _brd; }

    void invalidateParentFrameset();

    /** Return the _zoomed_ rectangle for this frame, including the border - for drawing
     * @param viewMode needed to know if borders are visible or not
     */
    QRect outerRect( KWViewMode* viewMode ) const;

    /** Return the unzoomed rectangle, including the border, for the frames-on-top list.
     * The default border of size 1-pixel that is drawn on screen is _not_ included here
     * [since it depends on the zoom]
     */
    KoRect outerKoRect() const;

    /** Return the rectangle for this frame including the border and the runaround gap.
     * This is the rectangle that the surrounding text must run around.
     */
    KoRect runAroundRect() const;

    /** Resize handles (in kwcanvas.h) are the dots that are drawn on selected
       frames, this creates and deletes them */
    void createResizeHandles();
    void createResizeHandlesForPage(KWCanvas *canvas);
    void removeResizeHandlesForPage(KWCanvas *canvas);
    void removeResizeHandles();
    void frameBordersChanged();
    void updateResizeHandles();
    void repaintResizeHandles();
    void updateRulerHandles();
    void updateCursorType();

    QBrush backgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor( const QBrush &_color );

    KoRect innerRect() const;

    double innerWidth() const;
    double innerHeight() const;


    /** The "internal Y" is the offset (in pt) of the real frame showed in this one
     * ("real" means "the last that isn't a copy")
     * This offset in pt is the sum of the height of the frames before that one.
     * For text frames, this is equivalent to the layout units (after multiplication). */
    void setInternalY( double y ) { m_internalY = y; }
    double internalY() const { return m_internalY; }

    /// set left padding (distance between frame contents and frame border)
    void setPaddingLeft( double b ) { m_paddingLeft = b; }
    /// set right padding
    void setPaddingRight( double b ) { m_paddingRight = b; }
    /// set top padding
    void setPaddingTop( double b ) { m_paddingTop = b; }
    /// set bottom padding
    void setPaddingBottom( double b ) { m_paddingBottom = b; }

    /// get left padding
    double paddingLeft() const { return m_paddingLeft; }
    /// get right padding
    double paddingRight() const { return m_paddingRight; }
    /// get top padding
    double paddingTop() const { return m_paddingTop; }
    /// get bottom padding
    double paddingBottom() const { return m_paddingBottom; }

    void setFramePadding( double _left, double _top, double right, double bottom);
    /** returns a copy of self
     */
    KWFrame *getCopy();

    void copySettings(KWFrame *frm);

    /** create XML to describe yourself
     */
    void save( QDomElement &frameElem );
    /** read attributes from XML. @p headerOrFooter if true some defaults are different
     */
    void load( QDomElement &frameElem, KWFrameSet* frameSet, int syntaxVersion );
    void loadCommonOasisProperties( KoOasisContext& context, KWFrameSet* frameSet );
    void loadBorderProperties( KoStyleStack& styleStack );

    QString saveOasisFrameStyle( KoGenStyles& mainStyles ) const;
    void startOasisFrame( KoXmlWriter &xmlWriter, KoGenStyles& mainStyles, const QString& name ) const;
    void saveBorderProperties( KoGenStyle& frameStyle ) const;

    void setMinFrameHeight(double h);
    double minFrameHeight(void)const {return m_minFrameHeight;}

    /** Return if the point is on the frame.
        @param point the point in normal coordinates.
        @param borderOfFrameOnly when true an additional check is done if the point
          is on the border.  */
    bool frameAtPos( const QPoint& nPoint, bool borderOfFrameOnly=false ) const;

    /**
     * Only applicable to frames of the main text frameset.
     * Set to true by KWFrameLayout if the "footnote line" should be
     * drawn under this frame.
     */
    void setDrawFootNoteLine( bool b ) { m_drawFootNoteLine = b; }
    bool drawFootNoteLine()const { return m_drawFootNoteLine; }

private:
    SheetSide m_sheetSide : 2;
    RunAround m_runAround : 2;
    RunAroundSide m_runAroundSide : 2;
    FrameBehavior m_frameBehavior : 2;
    NewFrameBehavior m_newFrameBehavior : 2;
    bool m_bCopy;
    bool m_selected;
    bool m_drawFootNoteLine;

    double m_runAroundLeft, m_runAroundRight, m_runAroundTop, m_runAroundBottom;
    double m_paddingLeft, m_paddingRight, m_paddingTop, m_paddingBottom;
    double m_minFrameHeight;

    double m_internalY;
    int m_zOrder;

    QBrush m_backgroundColor;
    KoBorder m_borderLeft, m_borderRight, m_borderTop, m_borderBottom;

    QPtrList<KWResizeHandle> handles;
    KWFrameList m_framesOnTop; ///< List of frames on top of us, those we shouldn't overwrite
    KWFrameList m_framesBelow; ///< List of frames below us. needed for selection code & transparency
    KWFrameSet *m_frameSet;

    /** Prevent operator=
     */
    KWFrame &operator=( const KWFrame &_frame );
    /** Prevent copy constructor
     */
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

    /**
     * Called before destruction, when terminating edition - use to e.g. hide cursor
     */
    virtual void terminate(bool /*removeselection*/ = true) {}

    /**
     * Paint this frameset in "has focus" mode (e.g. with a cursor)
     * See KWFrameSet for explanation about the arguments.
     * Most framesets don't need to reimplement that (the KWFrameSetEdit gets passed to drawFrame)
     */
    virtual void drawContents( QPainter *, const QRect &,
                               const QColorGroup &, bool onlyChanged, bool resetChanged,
                               KWViewMode *viewMode );

    // Events forwarded by the canvas (when being in "edit" mode)
    virtual void keyPressEvent( QKeyEvent * ) {}
    virtual void keyReleaseEvent( QKeyEvent * ) {}
    virtual void imStartEvent( QIMEvent * ) {}
    virtual void imComposeEvent( QIMEvent * ) {}
    virtual void imEndEvent( QIMEvent * ) {}
    virtual void mousePressEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {}
    virtual void mouseMoveEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {} // only called if button is pressed
    virtual void mouseReleaseEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {}
    virtual void mouseDoubleClickEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {}
    virtual void dragEnterEvent( QDragEnterEvent * ) {}
    virtual void dragMoveEvent( QDragMoveEvent *, const QPoint &, const KoPoint & ) {}
    virtual void dragLeaveEvent( QDragLeaveEvent * ) {}
    virtual void dropEvent( QDropEvent *, const QPoint &, const KoPoint &, KWView* ) {}
    virtual void focusInEvent() {}
    virtual void focusOutEvent() {}
    virtual void copy() {}
    virtual void cut() {}
    virtual void paste() {}
    virtual void selectAll() {}
    virtual void pasteData( QMimeSource* /*data*/, int /*provides*/ ) {}

    /** Show a popup menu - called when right-clicking inside a frame of this frameset.
     * The default implementation calls the frameset's showPopup.
     * @param frame the frame which was clicked.
     * @param view the view - we use it to get the popupmenu by name
     * @param point the mouse position (at which to show the menu)
     */
    virtual void showPopup( KWFrame* frame, KWView* view, const QPoint & _point );

    /// Called if the cursor tries to leave the frameset at its beginning.
    /// Returns true if the frameset was inline, and we did exit into another frameset.
    bool exitLeft();
    /// Called if the cursor tries to leave the frameset at its end.
    /// Returns true if the frameset was inline, and we did exit into another frameset.
    bool exitRight();

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
 * @see KWTextFrameSet, KWPartFrameSet, KWPictureFrameSet,
 *      KWFormulaFrameSet, KWTableFrameSet
 */
class KWFrameSet : public QObject
{
    Q_OBJECT
public:
    /// constructor
    KWFrameSet( KWDocument *doc );
    /// destructor
    virtual ~KWFrameSet();

    virtual KWordFrameSetIface* dcopObject();

    /** The type of frameset. Use this to differentiate between different instantiations of
     *  the framesets. Each implementation will return a different frameType.
     */
    virtual FrameSetType type() { return FT_BASE; }

    /**
     *  Return the type of FrameSet that would have been used in KWord 1.1
     */
    virtual FrameSetType typeAsKOffice1Dot1(void) { return type(); }

    virtual void addTextFrameSets( QPtrList<KWTextFrameSet> & /*lst*/, bool /*onlyReadWrite*/ = false ) {};
    virtual bool ownLine() const { return FALSE;}

    /** The different types of textFramesets (that TEXT is important here!)
     * FI_BODY = normal text frames.<br>
     * FI_FIRST_HEADER = Header on page 1<br>
     * FI_EVEN_HEADER = header on any even page<br>
     * FI_ODD_HEADER = header on any odd page (can be including page 1)<br>
     * FI_FIRST_FOOTER = footer on page 1<br>
     * FI_EVEN_FOOTER = footer on any even page<br>
     * FI_ODD_FOOTER = footer on any odd page (can be including page 1)<br>
     * FI_FOOTNOTE = a footnote frame.
     */
    enum Info { FI_BODY = 0, FI_FIRST_HEADER = 1, FI_EVEN_HEADER = 2, FI_ODD_HEADER = 3,
                FI_FIRST_FOOTER = 4, FI_EVEN_FOOTER = 5, FI_ODD_FOOTER = 6,
                FI_FOOTNOTE = 7 };
    /** Returns the type of TextFrameSet this is */
    Info frameSetInfo()const { return m_info; }
    /** Set the type of TextFrameSet this is */
    void setFrameSetInfo( Info fi ) { m_info = fi; }

    bool isAHeader() const;
    bool isAFooter() const;
    bool isHeaderOrFooter() const { return isAHeader() || isAFooter(); }
    bool isFootEndNote() const;

    virtual bool isFootNote() const { return false; }
    virtual bool isEndNote() const { return false; }

    bool isMainFrameset() const;
    bool isMoveable() const;

    // frame management
    virtual void addFrame( KWFrame *_frame, bool recalc = true );

    /** Delete a frame from the set of frames this frameSet has.
    *   @param num The frameNumber to be removed.
    *   @param remove passing true means that there can not be an undo of the action.
    *   @param recalc do an updateFrames()
    */
    virtual void delFrame( unsigned int _num, bool remove = true, bool recalc = true );

    /** Delete a frame from the set of frames this frameSet has.
    *   @param frm. The frame that should be deleted
    *   @param remove passing true means that there can not be an undo of the action.
    *   @param recalc do an updateFrames()
    */
    void delFrame( KWFrame *frm, bool remove = true, bool recalc = true ); // calls the virtual one

    /// Called by delFrame when it really deletes a frame (remove=true), to remove it from the table too
    virtual void frameDeleted( KWFrame* /*frm*/, bool /*recalc*/ ) {}

    void deleteAllFrames();
    void deleteAllCopies(); /// \note for headers/footers only

    /** retrieve frame from x and y coords (unzoomed coords) */
    KWFrame *frameAtPos( double _x, double _y ) const;

    /** Return if the point is on the frame.
        @param point the point in normal coordinates.
        @param borderOfFrameOnly when true an additional check is done if the point
          is on the border.  */
    bool isFrameAtPos( const KWFrame* frame, const QPoint& nPoint, bool borderOfFrameOnly=false ) const;

    /** return a frame if nPoint in on one of its borders */
    KWFrame *frameByBorder( const QPoint & nPoint ) const;

    /** get a frame by number */
    KWFrame *frame( unsigned int _num ) const;

    /** get the frame whose settings apply for @p frame
        (usually @p frame, but can also be the real frame if frame is a copy) */
    static KWFrame * settingsFrame( const KWFrame* frame );

    /** Iterator over the child frames */
    const QPtrList<KWFrame> &frameIterator() const { return frames; }
    /** Get frame number */
    int frameFromPtr( KWFrame *frame );
    /** Get number of child frames */
    unsigned int getNumFrames() const { return frames.count(); }

    /** Called when the user resizes a frame. Calls resizeFrame. */
    void resizeFrameSetCoords( KWFrame* frame, double newLeft, double newTop, double newRight, double newBottom, bool finalSize );
    /** Called when the user resizes a frame. Reimplemented by KWPictureFrameSet. */
    virtual void resizeFrame( KWFrame* frame, double newWidth, double newHeight, bool finalSize );
    /** Called when the user moves a frame. */
    virtual void moveFrame( KWFrame* ) {}

    /** True if the frameset was deleted (but not destroyed, since it's in the undo/redo) */
    bool isDeleted() const { return frames.isEmpty(); }

    /** Create a framesetedit object to edit this frameset in @p canvas */
    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas *, bool /*temp*/ = false ) { return 0L; }

    /**
     * @param emptyRegion The region is modified to subtract the areas painted, thus
     *                    allowing the caller to determine which areas remain to be painted.
     * Framesets that can be transparent should reimplement this and make it a no-op,
     * so that the background is painted below the transparent frame.
     */
    virtual void createEmptyRegion( const QRect & crect, QRegion & emptyRegion, KWViewMode *viewMode );

    /**
     * Paint this frameset
     * @param painter The painter in which to draw the contents of the frameset
     * @param crect The rectangle (in scrollview "contents coordinates", i.e. "view coords")
     * to be painted
     * @param cg The colorgroup from which to get the colors
     * @param onlyChanged If true, only redraw what has changed (see KWCanvas::repaintChanged)
     * @param resetChanged If true, set the changed flag to false after drawing.
     * @param edit If set, this frameset is being edited, so a cursor is needed.
     * @param viewMode For coordinate conversion, always set.
     * @param canvas For view settings. WARNING: canvas can be 0 (e.g. in embedded documents).
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
                               const QColorGroup &cg, bool onlyChanged, bool resetChanged,
                               KWFrameSetEdit *edit, KWViewMode *viewMode );

    /**
     * This one is called by drawContents for each frame.
     * It sets up clip rect and painter translation, and calls drawFrame, drawFrameBorder and drawMargins
     *
     * @param drawUnderlyingFrames if the frame implements it, then it should draw underlying frames.
     * This is set to false by the default drawFrame implementation, so that the frames under a
     * transparent frame are simply drawn, without transparency handling (nor their own
     * double-buffering).
     */
    void drawFrameAndBorders( KWFrame *frame,
                              QPainter *painter, const QRect &crect,
                              const QColorGroup &cg, bool, bool,
                              KWFrameSetEdit *edit, KWViewMode *viewMode,
                              KWFrame *settingsFrame, bool drawUnderlyingFrames );

    /**
     * Paint the borders for one frame of this frameset.
     * @param painter The painter in which to draw the contents of the frameset
     * @param frame The frame to be drawn
     * @param settingsFrame The frame from which we take the settings (usually @p frame, but not with Copy behaviour)
     * @param crect The rectangle (in "contents coordinates") to be painted
     */
    void drawFrameBorder( QPainter *painter, KWFrame *frame, KWFrame *settingsFrame,
                          const QRect &crect, KWViewMode *viewMode );

    /**
     * Draw a particular frame of this frameset.
     * This is called by drawContents and is what framesets must reimplement.
     * @param fcrect rectangle to be repainted, in the _frame_'s coordinate system, in pixels.
     * Doesn't include padding.
     *
     * @param crect rectangle to be repainted, in view coordinates. Includes padding.
     * Default implementation does double-buffering and calls drawFrameContents.
     */
    virtual void drawFrame( KWFrame *frame, QPainter *painter, const QRect &fcrect, const QRect &crect,
                            const QPoint& translationOffset,
                            KWFrame *settingsFrame, const QColorGroup &cg, bool onlyChanged, bool resetChanged,
                            KWFrameSetEdit *edit, KWViewMode *viewMode, bool drawUnderlyingFrames );

    /**
     * Implement this one instead of drawFrame to benefit from double-buffering
     * AND transparency handling (painting frames below this one) automatically.
     * You MUST reimplement one or the other, or you'll get infinite recursion ;)
     *
     * In this method, the painter has been translated to the frame's coordinate system
     * @param fcrect rectangle to be repainted, in the _frame_'s coordinate system, in pixels.
     * Doesn't include padding.
     */
    virtual void drawFrameContents( KWFrame * frame, QPainter *painter, const QRect& fcrect,
                                    const QColorGroup &cg, bool onlyChanged, bool resetChanged,
                                    KWFrameSetEdit * edit, KWViewMode *viewMode );

    /**
     * Draw the padding area inside of a specific frame of this frameSet
     */
    virtual void drawPadding( KWFrame *frame, QPainter *p, const QRect &fcrect, const QColorGroup &cg, KWViewMode *viewMode);

    enum UpdateFramesFlags {
        UpdateFramesInPage = 1,
        SortFrames = 2 ///< kwtextframeset only
        // next one is 4, not 3 ;)
    };
    /**
     * Called when our frames change, or when another frameset's frames change.
     * Framesets can reimplement it, but should always call the parent method.
     */
    virtual void updateFrames( int flags = 0xff );

    /** Return list of frames in page @p pageNum.
     * This is fast since it uses the m_framesInPage array.*/
    const QPtrList<KWFrame> & framesInPage( int pageNum ) const;

    /** Allows to detect that updateFrames() hasn't been called yet (e.g. on loading) */
    bool hasFramesInPageArray() const { return !m_framesInPage.isEmpty(); }

    /** relayout text in frames, so that it flows correctly around other frames */
    virtual void layout() {}
    virtual void invalidate() {}

    /** returns true if we have a frame occupying that position */
    virtual bool contains( double mx, double my );

    /**
     * Return "what it means" to have the mouse at position nPoint,
     * (which kwdoc tests to be over one of our frames).
     * The meaning is "what will happen when clicking".
     */
    virtual MouseMeaning getMouseMeaning( const QPoint &nPoint, int keyState );
    /**
     * Return the meaning of clicking inside the frame (when neither Ctrl nor Shift is pressed)
     */
    virtual MouseMeaning getMouseMeaningInsideFrame( const KoPoint& );

    /** Show a popup menu - called when right-clicking inside a frame of this frameset.
     * The default implementation shows "frame_popup".
     * @param frame the frame which was clicked. Always one of ours.
     * @param view the view - we use it to get the popupmenu by name
     * @param point the mouse position (at which to show the menu)
     */
    virtual void showPopup( KWFrame *frame, KWView *view, const QPoint &point );

    /// save to XML - when saving
    virtual QDomElement save( QDomElement &parentElem, bool saveFrames = true ) = 0;
    /// save to XML - when copying to clipboard
    virtual QDomElement toXML( QDomElement &parentElem, bool saveFrames = true )
    { return save( parentElem, saveFrames ); }
    /// Save to OASIS format
    virtual void saveOasis( KoXmlWriter& writer, KoSavingContext& context, bool saveFrames ) const = 0;

    /// load from XML - when loading
    virtual void load( QDomElement &framesetElem, bool loadFrames = true );
    KWFrame* loadOasisFrame( const QDomElement& tag, KoOasisContext& context );
    /// load from XML - when pasting from clipboard
    virtual void fromXML( QDomElement &framesetElem, bool loadFrames = true, bool /*useNames*/ = true )
    { load( framesetElem, loadFrames ); }

    virtual QString toPlainText() const { return QString::null; }

    //virtual void preparePrinting( QPainter *, QProgressDialog *, int & ) { }

    /** Called once the frameset has been completely loaded or constructed.
     * The default implementation calls updateFrames() and zoom(). Call the parent :) */
    virtual void finalize();

    virtual int paragraphs() { return 0; }
    virtual int paragraphsSelected() { return 0; }
    virtual bool statistics( QProgressDialog */*progress*/,  ulong & /*charsWithSpace*/, ulong & /*charsWithoutSpace*/, ulong & /*words*/,
                             ulong & /*sentences*/, ulong & /*syllables*/, ulong & /*lines*/,  bool /*process only selected */ ) { return true; }

    KWDocument* kWordDocument() const { return m_doc; }

    /// Return true if page @p num can be removed, as far as this frameset is concerned
    virtual bool canRemovePage( int num );

    //Note: none of those floating-frameset methods creates undo/redo
    //They are _called_ by the undo/redo commands.

    /// Make this frameset floating (anchored), as close to its current position as possible.
    void setFloating();
    /**
     * Make this frameset anchored, with the anchor at @p paragId,@p index in the text frameset @p textfs.
     * Also used during OASIS loading (placeHolderExists=true)
     */
    void setAnchored( KWTextFrameSet* textfs, KoTextParag* parag, int index, bool placeHolderExists = false, bool repaint = true );
    /** Make this frameset floating, with the anchor at @p paragId,@p index in the text frameset @p textfs.
     * \deprecated
     */
    void setAnchored( KWTextFrameSet* textfs, int paragId, int index, bool placeHolderExists = false, bool repaint = true );
    /** Note that this frameset has been made floating already, and store anchor position */
    void setAnchored( KWTextFrameSet* textfs );
    /// Make this frameset fixed, i.e. not anchored
    void setFixed();
    /// Return true if this frameset is floating (inline), false if it's fixed
    bool isFloating() const { return m_anchorTextFs; }
    /// Return the frameset in which our anchor is - assuming isFloating()
    KWTextFrameSet * anchorFrameset() const { return m_anchorTextFs; }
    /// Sets the frameset in which we are about to become inline. Used for OASIS loading.
    void setAnchorFrameset(KWTextFrameSet * textfs) { m_anchorTextFs = textfs; }
    /// Return the anchor object for this frame number
    KWAnchor * findAnchor( int frameNum );
    /// Tell this frame the format of it's anchor
    virtual void setAnchorFormat( KoTextFormat* /*format*/, int /*frameNum*/ ) {}

    /// Create an anchor for the floating frame identified by frameNum
    virtual KWAnchor * createAnchor( KoTextDocument *txt, int frameNum );

    /** Move the frame frameNum to the given position - this is called when
        the frame is anchored and the anchor moves (see KWAnchor). */
    virtual void moveFloatingFrame( int frameNum, const KoPoint &position );
    /** Get the size of the "floating frame" identified by frameNum.
        By default a real frame but not for tables. */
    virtual KoSize floatingFrameSize( int frameNum = 0 );
    /** Get the rect of the "floating frame" identified by frameNum,
        in coordinates RELATIVE TO THE PARENT FRAMESET.
        This is especially important for multi-parent inline frames. */
    KoRect floatingFrameRect( int frameNum = 0 );
    /** Get the 'baseline' to use for the "floating frame" identified by frameNum.
        -1 means same as the height (usual case) */
    virtual int floatingFrameBaseline( int /*frameNum*/ ) { return -1; }
    /// Store command for creating an anchored object
    virtual KCommand * anchoredObjectCreateCommand( int frameNum );
    /// Store command for deleting an anchored object
    virtual KCommand * anchoredObjectDeleteCommand( int frameNum );

    /** make this frameset part of a groupmanager
     * @see KWTableFrameSet
     */
    void setGroupManager( KWTableFrameSet *gm ) { grpMgr = gm; }
    /** returns the groupManager this frameset belongs to. A Groupmanager is better known as a table */
    KWTableFrameSet *getGroupManager()const { return grpMgr; }

    /** table headers can created by the groupmanager, we store the fact that
     this is one in here. */
    void setIsRemoveableHeader( bool h ) { m_removeableHeader = h; }
    bool isRemoveableHeader()const { return m_removeableHeader; }

    /// returns if one of our frames has been selected.
    bool hasSelectedFrame();

    bool isProtectSize()const { return m_protectSize; }
    void setProtectSize( bool _b) { m_protectSize = _b;}

    /**
     * Returns true if the frameset is visible.
     * A frameset is visible if setVisible(false) wasn't called,
     * but also, for a header frameset, if m_doc->isHeaderVisible is true, etc.
     * For an "even pages header" frameset, the corresponding headerType setting
     * must be selected (i.e. different headers for even and odd pages).
     * @param viewMode pass the current viewmode when using this method for any visual
     * stuff (drawing, handling input etc.). Frameset visibility depends on the viewmode.
     */
    bool isVisible( KWViewMode* viewMode = 0L ) const;

    /// set the visibility of the frameset.
    virtual void setVisible( bool v );

    /// get/set frameset name. For tables in particular, this _must_ be unique
    QString getName() const { return m_name; }
    void setName( const QString &_name );

    /// set frameBehavior on all frames, see KWFrame for specifics
    void setFrameBehavior( KWFrame::FrameBehavior fb );

    /// set newFrameBehavior on all frames, see KWFrame for specifics
    void setNewFrameBehavior( KWFrame::NewFrameBehavior nfb );

#ifndef NDEBUG
    virtual void printDebug();
    virtual void printDebug( KWFrame * );
#endif

    virtual KWTextFrameSet* nextTextObject( KWFrameSet * ) { return 0L;}

    bool isPaintedBy( KWFrameSet* fs ) const;

    /// set z-order for all frames
    virtual void setZOrder();

    virtual void setProtectContent ( bool protect ) = 0;
    virtual bool protectContent() const = 0;

signals:

    /// Emitted when something has changed in this frameset,
    /// so that all views repaint it. KWDocument connects to it,
    /// and KWCanvas connects to KWDocument.
    void repaintChanged( KWFrameSet * frameset );

protected:

    /// save the common attributes for the frameset
    void saveCommon( QDomElement &parentElem, bool saveFrames );

    /**Determine the clipping rectangle for drawing the contents of @p frame with @p painter
     * in the rectangle delimited by @p crect.
     * This determines where to clip the painter to draw the contents of a given frame
     */
    QRegion frameClipRegion( QPainter * painter, KWFrame *frame, const QRect & crect,
                             KWViewMode * viewMode );

    void deleteAnchor( KWAnchor * anchor );
    virtual void deleteAnchors();
    virtual void createAnchors( KoTextParag * parag, int index, bool placeHolderExists = false, bool repaint = true );


    KWDocument *m_doc;            // Document
    QPtrList<KWFrame> frames;        // Our frames

    // Cached info for optimization
    /// This array provides a direct access to the frames on page N
    QPtrVector< QPtrList<KWFrame> > m_framesInPage;
    /// always equal to m_framesInPage[0].first()->pageNum() :)
    int m_firstPage;
    /// always empty, for convenience in @ref framesInPage
    QPtrList<KWFrame> m_emptyList; // ## make static pointer to save memory ?

    Info m_info;
    KWTableFrameSet *grpMgr;
    bool m_removeableHeader, m_visible;
    bool m_protectSize;
    QString m_name;
    KWTextFrameSet * m_anchorTextFs;
    KWordFrameSetIface *m_dcop;
};

/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/
class KWPictureFrameSet : public KWFrameSet
{
public:
    KWPictureFrameSet( KWDocument *_doc, const QString & name );
    /// Used for OASIS loading
    KWPictureFrameSet( KWDocument* doc, const QDomElement& frame, const QDomElement& imageTag, KoOasisContext& context );
    virtual ~KWPictureFrameSet();

    virtual KWordFrameSetIface* dcopObject();

    /**
     * The type of frameset. Use this to differentiate between different instantiations of
     *  the framesets. Each implementation will return a different frameType.
     */
    virtual FrameSetType type( void );

    virtual FrameSetType typeAsKOffice1Dot1( void );

    void setPicture( const KoPicture &picture ) { m_picture = picture; }
    KoPicture picture() const { return m_picture; }

    KoPictureKey key() const { return m_picture.getKey(); }

    void loadPicture( const QString &fileName );
    void insertPicture( const KoPicture& picture );

    /**
     * Reload a picture, which already exists in the picture collection
     * (if not, it gives a defaulft KoPicture() )
     */
    void reloadPicture( const KoPictureKey& key );

    /**
     * @deprecated
     * The size of the image is now only needed at drawing time, not before anymore.
     * However at drawing time, it is the frame's size that matters.
     */
    void setSize( const QSize & _imgSize );

    /**
     * Called when the user resizes a frame.
     */
    virtual void resizeFrame( KWFrame* frame, double newWidth, double newHeight, bool finalSize );

    virtual QDomElement save( QDomElement &parentElem, bool saveFrames = true );
    virtual void load( QDomElement &attributes, bool loadFrames = true );
    virtual void saveOasis( KoXmlWriter& writer, KoSavingContext& context, bool saveFrames ) const;

    virtual void drawFrameContents( KWFrame * frame, QPainter *painter, const QRect & crect,
                                    const QColorGroup &cg, bool onlyChanged, bool resetChanged,
                                    KWFrameSetEdit * edit, KWViewMode *viewMode );

    /// Pixmaps can be transparent
    virtual void createEmptyRegion( const QRect &, QRegion &, KWViewMode * ) { }

#ifndef NDEBUG
    virtual void printDebug( KWFrame* frame );
#endif

    bool keepAspectRatio() const;
    void setKeepAspectRatio( bool b );

    virtual void setProtectContent ( bool protect ) { m_protectContent = protect; }
    virtual bool protectContent() const { return m_protectContent; }

protected:
    void loadOasis( const QDomElement& frame, const QDomElement& imageTag, KoOasisContext& context );

    /// The picture
    KoPicture m_picture;
    bool m_keepAspectRatio;
    /// Cache the finalSize parameter of the method resizeFrame for drawFrame
    bool m_finalSize;
    bool m_protectContent;
};

#if 0 // KWORD_HORIZONTAL_LINE
// MOC_SKIP_BEGIN
/******************************************************************/
/* Class: KWHorzLineFrameSet                                      */
/******************************************************************/
class KWHorzLineFrameSet : public KWPictureFrameSet
{
public:
    KWHorzLineFrameSet( KWDocument *_doc, const QString & name );
    virtual ~KWHorzLineFrameSet();

    virtual KWordFrameSetIface* dcopObject();

    /**
     * The type of frameset. Use this to differentiate between different instantiations of
     *  the framesets. Each implementation will return a different frameType.
     */
    virtual FrameSetType type( void );
    virtual bool ownLine() const;
    void drawFrameContents( KWFrame *frame, QPainter *painter, const QRect &crect,
                            const QColorGroup &, bool, bool, KWFrameSetEdit *, KWViewMode * );

};
// MOC_SKIP_END
#endif

#endif
