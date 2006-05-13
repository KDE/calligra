/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#ifndef kwviewmode_h
#define kwviewmode_h

#include <QRect>
#include <QString>
class KWDocument;
class QPainter;
class QRegion;
class KWTextFrameSet;
class KWFrameSet;
class KWCanvas;
class KoRuler;
class KoPageLayout;
class KWPage;

/**
 * Abstract base class for KWCanvas's view modes.
 *
 * The 'normal' mode is the WYSIWYG mode ("page mode"), in which pages are one
 * below another. Since frames are in this coordinate system, the view modes
 * other than the normal mode implement a transformation between the normal mode's
 * coordinate system and its own.
 * When drawing, we use normalcoord->viewcoord and when handling mouse clicks
 * we use viewcoord->normalcoord.
 * Note that this all in zoomed (pixel) coordinates (see DESIGN file).
 *
 * The View Mode is obviously also responsible for drawing pages etc.
 */
class KWViewMode
{
protected:
    KWViewMode( KWDocument * doc, KWCanvas* canvas, bool drawFrameBorders )
        : m_doc( doc ), m_canvas( canvas ), m_drawFrameBorders( drawFrameBorders )
    {}
public:
    virtual ~KWViewMode() {}

    /** Normal coord -> view coord */
    virtual QPoint normalToView( const QPoint & nPoint ) = 0;

    /** Normal coord -> view coord */
    QRect normalToView( const QRect & nRect )
    { return QRect( normalToView( nRect.topLeft() ), nRect.size() ); }

    /** View coord -> normal coord */
    virtual QPoint viewToNormal( const QPoint & vPoint ) = 0;

    /** View coord -> normal coord */
    QRect viewToNormal( const QRect & nRect )
    { return QRect( viewToNormal( nRect.topLeft() ), viewToNormal( nRect.bottomRight() ) ); }

    /** Return view coord for page rect. Avoids rounding problems when doing normalToView(zoomedRect) */
    virtual QRect viewPageRect( int pgNum ) = 0;

    /** Size of the contents area, in pixels */
    virtual QSize contentsSize() = 0;

    /** Size (in pixels) of the total area available for text in a given textframeset
     * This is used by KWTextFrameSet::drawFrame to erase between the bottom of the
     * last paragraph and the bottom of the available area. */
    virtual QSize availableSizeForText( KWTextFrameSet* textfs );

    /** "Topleft of current page" - concept used by the rulers.
     * The default implementation is good enough for any page-based viewmode,
     * since it calls normalToView. But the textmode has no page concept. */
    virtual QPoint pageCorner();
    /** The result of this is passed to setFrameStartEnd for both rulers
     * (after adjustement with pageCorner()) */
    virtual QRect rulerFrameRect();
    /** Called when the page layout is set, or changes.
     * Usually this is directly passed to KoRuler (for page-based viewmodes) */
    virtual void setPageLayout( KoRuler* hRuler, KoRuler* vRuler, const KoPageLayout& layout );

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion ) = 0;

    /** Config option for KWViewModePreview (a bit of a hack) */
    virtual void setPagesPerRow(int) {}
    virtual int pagesPerRow() { return 0; }

    /** Should selected text be drawn as such? */
    virtual bool drawSelections() { return true; }

    /** Should frame background be drawn? Usually yes, but not for embedded docs with transparent=true */
    virtual bool drawFrameBackground() { return true; }

    /** Should we see frame borders? This setting doesn't always come from KWView... */
    bool drawFrameBorders() const { return m_drawFrameBorders; }
    void setDrawFrameBorders(bool b)  { m_drawFrameBorders = b; }

    /** Should this frameset be visible in this viewmode? True by default, all are shown. */
    virtual bool isFrameSetVisible( const KWFrameSet* /*frameset*/ ) { return true; }

    /** Should formatVertically() happen (to skip frame bottom, frames on top, etc.) */
    // TODO: maybe this should be more fine-grained.
    virtual bool shouldFormatVertically() { return true; }

    /** Should adjust[LR]Margin() happen (to run the text around frames on top etc.) */
    virtual bool shouldAdjustMargins() { return true; }

    /** Does this viewmode know anything about frames? */
    virtual bool hasFrames() { return true; }

    /** Does this viewmode know anything about pages? */
    virtual bool hasPages() { return true; }

    /** Return the name of the viewmode, used for loading/saving. */
    virtual const QString type() const = 0;

    /** Answers the question if argument frameset has to be drawn as a text-mode
     *  text area if true, or if false as a frame with its own contents. */
    virtual bool isTextModeFrameset(KWFrameSet *) const { return false; }

    static KWViewMode *create( const QString& viewModeType, KWDocument* doc, KWCanvas* canvas );

    KWCanvas * canvas() const { return m_canvas; }

protected:
    /**
     * Draw the border around one page, and clear up the empty space in that page
     * @param crect the rect we're interested in painting (in view pixels)
     * @param pageRect the rect for the page, in view pixels
     * @param emptySpaceRegion the empty space to clear up inside the page (in view pixels)
     * @param painter the painter to paint the border on.
     */
    void drawOnePageBorder( QPainter * painter, const QRect & crect, const QRect & pageRect,
                            const QRegion & emptySpaceRegion );
    QRect drawRightShadow( QPainter * painter, const QRect & crect, const QRect & pageRect, int topOffset );
    QRect drawBottomShadow( QPainter * painter, const QRect & crect, const QRect & pageRect, int leftOffset );
    static const unsigned short s_shadowOffset;

    KWDocument * m_doc;
    KWCanvas * m_canvas;
    bool m_drawFrameBorders;
};

/** The 'normal' view mode (pages below one another) */
class KWViewModeNormal : public KWViewMode
{
public:
    KWViewModeNormal( KWDocument * doc, KWCanvas* canvas, bool drawFrameBorders )
        : KWViewMode( doc, canvas, drawFrameBorders ) {}
    virtual ~KWViewModeNormal() {}

    virtual QPoint normalToView( const QPoint & nPoint );
    virtual QPoint viewToNormal( const QPoint & vPoint );
    virtual QRect viewPageRect( int pgNum );
    virtual QSize contentsSize();

    virtual const QString type() const { return "ModeNormal"; }

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );

protected:
    virtual int xOffset(KWPage *page, int canvasWidth = -1);
};

/** The view mode used when printing (pages under one another, no selections) */
class KWViewModePrint : public KWViewModeNormal // we inherit the "normal" viewmode
{
public:
    KWViewModePrint( KWDocument * doc, KWCanvas* canvas )
        : KWViewModeNormal( doc, canvas, false /*drawFrameBorders*/ ) {}
    virtual ~KWViewModePrint() {}
    virtual bool drawSelections() { return false; }
protected:
    virtual int xOffset(KWPage *page, int canvasWidth = -1);
};

/** The 'embedded' view mode (usually a single page, no selections) */
class KWViewModeEmbedded : public KWViewMode
{
public:
    KWViewModeEmbedded ( KWDocument * doc, KWCanvas* canvas )
        : KWViewMode( doc, canvas, false /*drawFrameBorders*/ ),
          mDrawFrameBackground( true ),
          mDrawSelections( true )
    {}
    virtual ~ KWViewModeEmbedded() {}

    void setDrawFrameBackground( bool b ) { mDrawFrameBackground = b; }
    void setDrawSelections( bool b ) { mDrawSelections = b; }

    // This view mode is very easy to implement ;-P
    virtual QPoint normalToView( const QPoint & nPoint ) { return nPoint; }
    virtual QPoint viewToNormal( const QPoint & vPoint ) { return vPoint; }
    virtual QRect viewPageRect( int pgNum );
    virtual QSize contentsSize() { return QSize(); }
    virtual bool hasPages() { return false; }

    virtual void drawPageBorders( QPainter *, const QRect &, const QRegion & ){}
    virtual bool drawSelections() { return mDrawSelections; }

    virtual const QString type() const { return "ModeEmbedded"; }
    virtual bool drawFrameBackground() { return mDrawFrameBackground; }

private:
    bool mDrawFrameBackground;
    bool mDrawSelections;
};


/** A mode for previewing the overall document
 Pages are organized in a grid (mostly useful when zooming out a lot) */
class KWViewModePreview : public KWViewMode
{
public:
    KWViewModePreview( KWDocument * doc, KWCanvas* canvas, bool drawFrameBorders, int _nbPagePerRow );
    virtual ~KWViewModePreview() {}

    virtual QPoint normalToView( const QPoint & nPoint );
    virtual QPoint viewToNormal( const QPoint & vPoint );
    virtual QRect viewPageRect( int pgNum );
    virtual QSize contentsSize();
    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );

    virtual void setPagesPerRow(int _nb) {m_pagesPerRow=_nb;}
    virtual int pagesPerRow() {return m_pagesPerRow;}
    virtual const QString type() const { return "ModePreview"; }

    int numRows() const;

//protected:
    /** Return the spacing between the left border of the canvas and the page. */
    int leftSpacing();
    /** Return the spacing between the top border of the canvas and the page. */
    int topSpacing();

private:
    int m_pagesPerRow;
    int m_spacing;
};

/** The 'edit only one text frameset' view mode */
class KWViewModeText : public KWViewMode
{
public:

    KWViewModeText( KWDocument * doc, KWCanvas* canvas, KWTextFrameSet* fs );
    virtual ~KWViewModeText() {}

    KWTextFrameSet *textFrameSet() const;
    /**
     * Return a text frameset that the user has either selected or is currently editing,
     * returning the main text frameset otherwise.
     * If the selected frame is not a text FS, and the user is not editing one in the
     * first view plus the document has no main text frameset then this method will return 0
     */
    static KWTextFrameSet *determineTextFrameSet( KWDocument* doc );

    virtual QPoint normalToView( const QPoint & nPoint );
    virtual QPoint viewToNormal( const QPoint & vPoint );
    virtual QSize contentsSize();
    virtual QRect viewPageRect( int ) { return QRect(); }
    virtual QSize availableSizeForText( KWTextFrameSet* textfs );
    // There is no page concept. Keep everything relative to (0,0)
    virtual QPoint pageCorner() { return QPoint( 0, 0 ); }
    virtual QRect rulerFrameRect();
    virtual void setPageLayout( KoRuler* hRuler, KoRuler* vRuler, const KoPageLayout& layout );

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );
    virtual const QString type() const { return "ModeText"; }
    virtual bool shouldFormatVertically() { return false; }
    virtual bool shouldAdjustMargins() { return false; }
    virtual bool hasFrames() { return false; }
    virtual bool hasPages() { return false; }

    virtual bool isFrameSetVisible( const KWFrameSet* fs );

    virtual bool isTextModeFrameset(KWFrameSet *fs) const;

    /// hardcoded distance from the left side this viewmode starts its rendering
    static const int OFFSET = 10;


private:
    KWTextFrameSet *m_textFrameSet;
};

#endif
