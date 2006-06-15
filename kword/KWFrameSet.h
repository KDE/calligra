/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef frame_set_h
#define frame_set_h

#include "KWFrame.h"
#include "KoPoint.h"
#include "KoSize.h"
#include "KoRect.h"
#include <q3ptrvector.h>
#include <q3ptrlist.h>
#include "qdom.h"

class KWAnchor;
class KWFrame;
class KWFrameSetEdit;
class KWPageManager;
class KWTableFrameSet;
class KWTextFrameSet;
class KWTextFrameSetEdit;
class KWTextParag;
class KWView;
class KWFrameViewManager;

class KoSavingContext;
class KoTextDocument;
class KoTextFormat;
class KoTextParag;
class KoXmlWriter;

class KCommand;
class QPoint;
class Q3ProgressDialog;


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


    /** The type of frameset. Use this to differentiate between different instantiations of
     *  the framesets. Each implementation will return a different frameType.
     */
    virtual FrameSetType type() const { return FT_BASE; }

    virtual void addTextFrameSets( Q3PtrList<KWTextFrameSet> & /*lst*/, bool /*onlyReadWrite*/ = false ) {};
    virtual bool ownLine() const { return false;}

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
    const char* headerFooterTag() const; // for OASIS saving

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
    virtual void deleteFrame( unsigned int num, bool remove = true, bool recalc = true );

    /** Delete a frame from the set of frames this frameSet has.
    *   @param frm The frame that should be deleted
    *   @param remove passing true means that there can not be an undo of the action.
    *   @param recalc do an updateFrames()
    */
    void deleteFrame( KWFrame *frm, bool remove = true, bool recalc = true ); // calls the virtual one

    /// Called by deleteFrame when it really deletes a frame (remove=true), to remove it from the table too
    virtual void frameDeleted( KWFrame* /*frm*/, bool /*recalc*/ ) {}

    void deleteAllFrames();
    void deleteAllCopies(); /// \note for headers/footers only

    /** retrieve frame from x and y coords (unzoomed coords) */
    KWFrame *frameAtPos( double _x, double _y ) const;

    /** Return if the point is on the frame.
     *  @param nPoint the point in normal coordinates.
     *  @param borderOfFrameOnly when true an additional check is done if the point
     *    is on the border.
     *  @param frame the frame to check inside of
     */
    bool isFrameAtPos( const KWFrame* frame, const QPoint& nPoint, bool borderOfFrameOnly=false ) const;

    /** get a frame by number */
    KWFrame *frame( unsigned int _num ) const;

    /**
     * Returns the original frame that the param frame is a copy off, or the frame itself.
     * For changing a frame that is a copy of another (as found in KWFrame::isCopy()) you
     * need a so called settings frame, or original frame.  This method can savely be called
     * on any frame to make sure you are dealing with the original frame.
     * @param frame the frame to find the settings frame for.
     * @return the frame itself if its not a copy, or the original frame if it is one.
     */
    static KWFrame * settingsFrame( const KWFrame* frame );

    /** Iterator over the child frames */
    const Q3PtrList<KWFrame> &frameIterator() const { return m_frames; }
    /** Get frame number */
    int frameFromPtr( KWFrame *frame );
    /** Get number of child frames */
    unsigned int frameCount() const { return m_frames.count(); }

    /** True if the frameset was deleted (but not destroyed, since it's in the undo/redo) */
    bool isDeleted() const { return m_frames.isEmpty(); }

    /** Create a framesetedit object to edit this frameset in @p canvas */
    virtual KWFrameSetEdit * createFrameSetEdit( KWCanvas * ) { return 0; }

    /**
     * Let the caller know which parts of @p emptyRegion we do not claim, and thus the caller is
     * free to paint on without interfering with later paints from us.
     * For painting all frames of all framesets the painter needs to find out what area we
     * will fill with a background color and possibly paint over.
     * What this method does is fill an QRegion with all the areas we are going to fill, which
     * typically means we will paint the background of a frame there.
     * @param emptyRegion The region is modified to subtract the areas painted, thus
     *                    allowing the caller to determine which areas remain to be painted.
     * Framesets that can be transparent should reimplement this and make it a no-op,
     * so that the background is painted below the transparent frame.
     * @param crect the cliprect; only parts inside this rect are of interrest to us
     * @param viewMode For coordinate conversion, always set.
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
     * @param frameViewManager the frameViewManager;
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
                               KWFrameSetEdit *edit, KWViewMode *viewMode,
                               KWFrameViewManager *frameViewManager );

    /**
     * This one is called by drawContents for each frame.
     * It sets up clip rect and painter translation, and calls drawFrame, drawFrameBorder and drawMargins
     *
     * @param drawUnderlyingFrames if the frame implements it, then it should draw underlying frames.
     * This is set to false by the default drawFrame implementation, so that the frames under a
     * transparent frame are simply drawn, without transparency handling (nor their own
     * double-buffering).
     * @param frame the frame to draw
     * @param painter the painter to draw to
     * @param settingsFrame The frame from which we take the settings (usually @p frame, but not with Copy behaviour)
     * @param cg The colorgroup from which to get the colors
     * @param edit If set, this frameset is being edited, so a cursor is needed.
     * @param viewMode For coordinate conversion, always set.
     * @param crect rectangle to be repainted, in view coordinates. Includes padding.
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
     * @param viewMode For coordinate conversion, always set.
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
     * @param frame the frame to draw
     * @param painter the painter to draw to
     * @param settingsFrame The frame from which we take the settings (usually @p frame, but not with Copy behaviour)
     * @param cg The colorgroup from which to get the colors
     * @param onlyChanged If true, only redraw what has changed (see KWCanvas::repaintChanged)
     * @param resetChanged If true, set the changed flag to false after drawing.
     * @param edit If set, this frameset is being edited, so a cursor is needed.
     * @param viewMode For coordinate conversion, always set.
     * @param drawUnderlyingFrames if the frame implements it, then it should draw underlying frames.
     * @param translationOffset offset this method will translate the paiter before doing its work.
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
     * @param frame the frame to draw
     * @param painter the painter to draw to
     * @param cg The colorgroup from which to get the colors
     * @param onlyChanged If true, only redraw what has changed (see KWCanvas::repaintChanged)
     * @param resetChanged If true, set the changed flag to false after drawing.
     * @param edit If set, this frameset is being edited, so a cursor is needed.
     * @param viewMode For coordinate conversion, always set.
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
    const Q3PtrList<KWFrame> & framesInPage( int pageNum ) const;

    /** Allows to detect that updateFrames() hasn't been called yet (e.g. on loading) */
    bool hasFramesInPageArray() const { return !m_framesInPage.isEmpty(); }

    /** relayout text in frames, so that it flows correctly around other frames */
    virtual void layout() {}
    virtual void invalidate() {}

    /// save to XML - when saving
    virtual QDomElement save( QDomElement &parentElem, bool saveFrames = true ) = 0;
    /// save to XML - when copying to clipboard
    virtual QDomElement toXML( QDomElement &parentElem, bool saveFrames = true )
    { return save( parentElem, saveFrames ); }
    /// Save to OASIS format
    virtual void saveOasis( KoXmlWriter& writer, KoSavingContext& context, bool saveFrames ) const = 0;
    /// If the frameset is inline, is it ok to save it inside an oasis <text:p>?
    /// If not, KWAnchor will create a textbox around it.
    virtual bool canBeSavedAsInlineCharacter() const { return true; }

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
    virtual bool statistics( Q3ProgressDialog * /*progress*/,  ulong & /*charsWithSpace*/, ulong & /*charsWithoutSpace*/, ulong & /*words*/,
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
    void setGroupManager( KWTableFrameSet *gm ) { m_groupmanager = gm; }
    /** returns the groupManager this frameset belongs to. A Groupmanager is better known as a table */
    KWTableFrameSet *groupmanager()const { return m_groupmanager; }

    bool isProtectSize()const { return m_protectSize; }
    void setProtectSize( bool _b);

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
    QString name() const { return m_name; }
    void setName( const QString &name );

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

    void setPageManager(KWPageManager *pm) { m_pageManager = pm; }
    KWPageManager* pageManager() const { return m_pageManager; }

signals:

    /// Emitted when something has changed in this frameset,
    /// so that all views repaint it. KWDocument connects to it,
    /// and KWCanvas connects to KWDocument.
    void repaintChanged( KWFrameSet * frameset );

    void sigFrameAdded(KWFrame*);
    void sigFrameRemoved(KWFrame*);

    void sigNameChanged(KWFrameSet*);

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
    Q3PtrList<KWFrame> m_frames;        // Our frames

    // Cached info for optimization
    /// This array provides a direct access to the frames on page N
    Q3PtrVector< Q3PtrList<KWFrame> > m_framesInPage;
    /// always equal to m_framesInPage[0].first()->pageNumber() :)
    int m_firstPage;
    /// always empty, for convenience in @ref framesInPage
    Q3PtrList<KWFrame> m_emptyList; // ## make static pointer to save memory ?

    Info m_info;
    KWTableFrameSet *m_groupmanager;
    bool m_visible;
    bool m_protectSize;
    QString m_name;
    KWTextFrameSet * m_anchorTextFs;
    KWPageManager *m_pageManager;
};

#endif
