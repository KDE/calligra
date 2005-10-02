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

#include <qrect.h>
class KWDocument;
class QPainter;
class QRegion;
class KWTextFrameSet;
class KWFrameSet;
class KWCanvas;
class KoRuler;
class KoPageLayout;

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
    KWViewMode( KWDocument * doc, bool drawFrameBorders ) : m_doc( doc ), m_drawFrameBorders( drawFrameBorders ), m_currentCanvas( 0 ) {}
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

    /** Size of the contents area */
    virtual QSize contentsSize() = 0;

    /** Size (in pixels) of the total area available for text in a given textframeset
     * This is used by KWTextFrameSet::drawFrame to erase between the bottom of the
     * last paragraph and the bottom of the available area. */
    virtual QSize availableSizeForText( KWTextFrameSet* textfs );

    /** "Topleft of current page" - concept used by the rulers.
     * The default implementation is good enough for any page-based viewmode,
     * since it calls normalToView. But the textmode has no page concept. */
    virtual QPoint pageCorner( KWCanvas* canvas );
    /** The result of this is passed to setFrameStartEnd for both rulers
     * (after adjustement with pageCorner()) */
    virtual QRect rulerFrameRect( KWCanvas* canvas );
    /** Called when the page layout is set, or changes.
     * Usually this is directly passed to KoRuler (for page-based viewmodes) */
    virtual void setPageLayout( KoRuler* hRuler, KoRuler* vRuler, const KoPageLayout& layout );

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion ) = 0;

    /** Config option for KWViewModePreview (a bit of a hack) */
    virtual void setPagesPerRow(int) {}
    virtual int pagesPerRow() {return 0;}

    /** Should selected text be drawn as such? */
    virtual bool drawSelections() {return true;}

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

    /** Return the name of the viewmode, used for loading/saving. */
    virtual const QString type() = 0;

    /** Answers the question if argument frameset has to be drawn as a text-mode
     *  text area if true, or if false as a frame with its own contents. */
    virtual bool isTextModeFrameset(KWFrameSet *) const { return false; }

    static KWViewMode *create( const QString & viewModeType, KWDocument * );

    void        setCanvas( KWCanvas * canvas )  { m_currentCanvas = canvas; }
    KWCanvas *  canvas() { return m_currentCanvas; }

protected:
    // Draw the border around one page, and clear up the empty space in that page
    void drawOnePageBorder( QPainter * painter, const QRect & crect, const QRect & pageRect,
                            const QRegion & emptySpaceRegion );
    QRect drawRightShadow( QPainter * painter, const QRect & crect, const QRect & pageRect, int topOffset );
    QRect drawBottomShadow( QPainter * painter, const QRect & crect, const QRect & pageRect, int leftOffset );
    static const unsigned short s_shadowOffset;

    KWDocument * m_doc;
    bool m_drawFrameBorders;
    KWCanvas * m_currentCanvas;

};

/** The 'normal' view mode (pages below one another) */
class KWViewModeNormal : public KWViewMode
{
public:
    KWViewModeNormal( KWDocument * doc, bool drawFrameBorders ) : KWViewMode( doc, drawFrameBorders ) {}
    virtual ~KWViewModeNormal() {}

    // This view mode is very easy to implement ;-P
    virtual QPoint normalToView( const QPoint & nPoint ) { return nPoint; }
    virtual QPoint viewToNormal( const QPoint & vPoint ) { return vPoint; }
    virtual QSize contentsSize();

    virtual const QString type() {return "ModeNormal";}

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );
};

/** The view mode used when printing (pages under one another, no selections) */
class KWViewModePrint : public KWViewModeNormal // we inherit the "normal" viewmode
{
public:
    KWViewModePrint( KWDocument * doc ) : KWViewModeNormal( doc, false /*drawFrameBorders*/ ) {}
    virtual ~KWViewModePrint() {}
    virtual bool drawSelections() { return false; }
};

/** The 'embedded' view mode (usually a single page, no selections) */
class KWViewModeEmbedded : public KWViewMode
{
public:
    KWViewModeEmbedded ( KWDocument * doc ) : KWViewMode( doc, false /*drawFrameBorders*/ ) {}
    virtual ~ KWViewModeEmbedded() {}

    // This view mode is very easy to implement ;-P
    virtual QPoint normalToView( const QPoint & nPoint ) { return nPoint; }
    virtual QPoint viewToNormal( const QPoint & vPoint ) { return vPoint; }
    virtual QSize contentsSize() {return QSize();}

    virtual void drawPageBorders( QPainter *, const QRect &, const QRegion & ){}
    virtual bool drawSelections() { return false; }

    virtual const QString type() {return "ModeEmbedded";}
};


/** A mode for previewing the overall document
 Pages are organized in a grid (mostly useful when zooming out a lot) */
class KWViewModePreview : public KWViewMode
{
public:
    KWViewModePreview( KWDocument * doc, bool drawFrameBorders, int _nbPagePerRow=4 ) : KWViewMode( doc, drawFrameBorders ),
        m_pagesPerRow(_nbPagePerRow),
        m_spacing(10)
    {}
    virtual ~KWViewModePreview() {}

    virtual QPoint normalToView( const QPoint & nPoint );
    virtual QPoint viewToNormal( const QPoint & vPoint );
    virtual QSize contentsSize();
    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );

    virtual void setPagesPerRow(int _nb) {m_pagesPerRow=_nb;}
    virtual int pagesPerRow() {return m_pagesPerRow;}
    virtual const QString type() {return "ModePreview";}

    int numRows() const;

protected:
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

    KWViewModeText( KWDocument * doc, KWTextFrameSet* fs );
    virtual ~KWViewModeText() {}

    KWTextFrameSet *textFrameSet() const;
    static KWTextFrameSet *determineTextFrameSet( KWDocument* doc );

    virtual QPoint normalToView( const QPoint & nPoint );
    virtual QPoint viewToNormal( const QPoint & vPoint );
    virtual QSize contentsSize();
    virtual QSize availableSizeForText( KWTextFrameSet* textfs );
    // There is no page concept. Keep everything relative to (0,0)
    virtual QPoint pageCorner( KWCanvas* ) { return QPoint( 0, 0 ); }
    virtual QRect rulerFrameRect( KWCanvas* canvas );
    virtual void setPageLayout( KoRuler* hRuler, KoRuler* vRuler, const KoPageLayout& layout );

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );
    virtual const QString type() {return "ModeText";}
    virtual bool shouldFormatVertically() { return false; }
    virtual bool shouldAdjustMargins() { return false; }
    virtual bool hasFrames() { return false; }

    virtual bool isFrameSetVisible( const KWFrameSet* fs );

    virtual bool isTextModeFrameset(KWFrameSet *fs) const;

private:
    KWTextFrameSet *m_textFrameSet;
};

#endif
