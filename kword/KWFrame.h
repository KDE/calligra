/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2000-2006 David Faure <faure@kde.org>
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

#ifndef frame_h
#define frame_h

#include "defs.h"
#include <KoRect.h>
#include <qbrush.h>
#include "KoBorder.h"
#include <q3ptrlist.h>

class KWCanvas;
class KWDocument;
class KWFrame;
class KWFrameList;
class KWFrameSet;
class KWViewMode;

class KoGenStyle;
class KoGenStyles;
class KoOasisContext;
class KoStyleStack;
class KoXmlWriter;

class QDomElement;

/**
 * small utility class representing a sortable (by z-order) list of frames
 * you can use sort() and inSort(item)
 *
 * Deprecated, use QValueVector<KWFrame*> from now on, and sort it with
 * std::sort(frames.begin(),frames.end(),KWFrame::compareFrameZOrder);
 *
 * With Qt4 we'll be able to use qSort for lists.
 */
class ZOrderedFrameList : public Q3PtrList<KWFrame>
{
protected:
    virtual int compareItems(Q3PtrCollection::Item a, Q3PtrCollection::Item b);
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

    /** The page on which this frame is
     */
    int pageNumber() const;
    /** Same as pageNumber(), but works if the frame hasn't been added to a frameset yet
     */
    int pageNumber( KWDocument* doc ) const;

    /** The z-order of the frame, relative to the other frames on the same page
     */
    void setZOrder( int z ) { m_zOrder = z; }
    int zOrder() const { return m_zOrder; }

    /**
     * This is a method used to sort a list using the STL sorting methods.
     * @param f1 the first object
     * @param f2 the second object
     */
    static bool compareFrameZOrder(KWFrame *f1, KWFrame *f2);

    KWFrameList* frameStack() { return m_frameStack; }
    void setFrameStack(KWFrameList *fl) { m_frameStack = fl; }

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

    /** Return the rectangle for this frame.
     * This method is just so that new code doesn't rely on "KWFrame inherits KoRect",
     * which would be good to get rid of, at some point.
     */
    const KoRect& rect() const { return *this; }

    /** Marks a frame to have changed position/borders.
      Make sure you call this when the frame borders changed so when its inline it will be moved.
    */
    void frameBordersChanged();
    void updateRulerHandles();

    QBrush backgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor( const QBrush &_color );
    bool isTransparent() const { return m_backgroundColor.style() != Qt::SolidPattern; }

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
    void loadCommonOasisProperties( KoOasisContext& context, KWFrameSet* frameSet, const char* typeProperties );
    void loadBorderProperties( KoStyleStack& styleStack );

    QString saveOasisFrameStyle( KoGenStyles& mainStyles ) const;
    /**
     * Write out a draw:frame element and its style.
     * The caller is responsible for creating the child element and closing the draw:frame element.
     * @p lastFrameName is the name of the previous frame in the same frameset; this is only used for copy-frames
     */
    void startOasisFrame( KoXmlWriter &xmlWriter, KoGenStyles& mainStyles, const QString& name,
                          const QString& lastFrameName = QString::null ) const;
    void saveBorderProperties( KoGenStyle& frameStyle ) const;
    void saveMarginProperties( KoGenStyle& frameStyle ) const;
    void saveMarginAttributes( KoXmlWriter &writer ) const;

    /**
     * The property minimum frame height is used to make the automatic frame shrinking code stop.
     * Each frame has a height based on its frame dimentions, but changes in content may change
     * the sizes automatically.  If all text is removed from a text frame it will grow smaller
     * until the minimum frame height is reached, and it will not shrink below that.
     * @param h the new minimum height
     */
    void setMinimumFrameHeight(double h) { m_minFrameHeight = h; }
    /**
     * @return the minimum frame height.
     * @see setMinimumFrameHeight() for details.
     */
    double minimumFrameHeight(void)const {return m_minFrameHeight;}

    /** Return if the point is on the frame.
        @param nPoint the point in normal coordinates.
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
    bool m_drawFootNoteLine;

    double m_runAroundLeft, m_runAroundRight, m_runAroundTop, m_runAroundBottom;
    double m_paddingLeft, m_paddingRight, m_paddingTop, m_paddingBottom;
    double m_minFrameHeight;

    double m_internalY;
    int m_zOrder;

    QBrush m_backgroundColor;
    KoBorder m_borderLeft, m_borderRight, m_borderTop, m_borderBottom;

    /** List of frames we have below and on top of us.
     * Frames on top we should never overwrite.  Frames below us needd for selection code &
     * transparancy */
    KWFrameList *m_frameStack;
    KWFrameSet *m_frameSet;

    /** Prevent operator=
     */
    KWFrame &operator=( const KWFrame &_frame );
    /** Prevent copy constructor
     */
    KWFrame ( const KWFrame &_frame );
};

#endif
