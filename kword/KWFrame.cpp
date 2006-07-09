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

#include "KWFrame.h"
#include "KWFrameSet.h"
#include "KWFrameList.h"
#include "KWDocument.h"
#include "KWPageManager.h"
#include "KWTextFrameSet.h"
#include "KWViewMode.h"
#include "KWCanvas.h"

#include <KoOasisContext.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoStyleStack.h>

#include <kcommand.h>
#include <kdebug.h>

#include <float.h> // for DBL_DIG

//#define DEBUG_DRAW

/******************************************************************/
/* Class: ZOrderedFrameList                                       */
/******************************************************************/

int ZOrderedFrameList::compareItems(QPtrCollection::Item a, QPtrCollection::Item b)
{
    int za = ((KWFrame *)a)->zOrder();
    int zb = ((KWFrame *)b)->zOrder();
    if (za == zb) return 0;
    if (za < zb) return -1;
    return 1;
}


/******************************************************************/
/* Class: KWFrame                                                 */
/******************************************************************/

KWFrame::KWFrame(KWFrame * frame)
{
    m_runAround = RA_NO;
    //kdDebug(32001) << "KWFrame::KWFrame this=" << this << " frame=" << frame << endl;
    copySettings( frame );
    m_minFrameHeight=0;
    m_frameStack = 0; // lazy initialisation.
}

KWFrame::KWFrame(KWFrameSet *fs, double left, double top, double width, double height, RunAround ra )
    : KoRect( left, top, width, height ),
      // Initialize member vars here. This ensures they are all initialized, since it's
      // easier to compare this list with the member vars list (compiler ensures order).
      m_sheetSide( AnySide ),
      m_runAround( ra ),
      m_runAroundSide( RA_BIGGEST ),
      m_frameBehavior( AutoExtendFrame ),
      m_newFrameBehavior( ( fs && fs->type() == FT_TEXT ) ? Reconnect : NoFollowup ),
      m_bCopy( false ),
      m_drawFootNoteLine( false ),
      m_runAroundLeft( 1.0 ),
      m_runAroundRight( 1.0 ),
      m_runAroundTop( 1.0 ),
      m_runAroundBottom( 1.0 ),
      m_paddingLeft( 0 ),
      m_paddingRight( 0 ),
      m_paddingTop( 0 ),
      m_paddingBottom( 0 ),
      m_minFrameHeight( 0 ),
      m_internalY( 0 ),
      m_zOrder( 0 ),
      m_backgroundColor( (fs && (fs->type() == FT_PICTURE || fs->type() == FT_PART)) ? QBrush( QColor(), Qt::NoBrush) : QBrush( QColor() ) ), // valid brush with invalid color ( default )
      m_borderLeft( QColor(), KoBorder::SOLID, 0 ),
      m_borderRight( QColor(), KoBorder::SOLID, 0 ),
      m_borderTop( QColor(), KoBorder::SOLID, 0 ),
      m_borderBottom( QColor(), KoBorder::SOLID, 0 ),
      m_frameSet( fs )
{
    //kdDebug(32001) << "KWFrame::KWFrame " << this << " left=" << left << " top=" << top << endl;
    m_frameStack = 0; // lazy initialisation.
}

KWFrame::~KWFrame()
{
    //kdDebug(32001) << "KWFrame::~KWFrame " << this << endl;
    delete m_frameStack;
    m_frameStack = 0;
}

void KWFrame::setBackgroundColor( const QBrush &color )
{
    m_backgroundColor = color;
}


int KWFrame::pageNumber() const
{
    Q_ASSERT( m_frameSet );
    if( !m_frameSet ) {
        kdDebug() << k_funcinfo << this << " has no frameset!" << endl;
        return 0;
    }
    if( !m_frameSet->pageManager() ) {
        kdWarning() << k_funcinfo << this << " is not a frame that is in use; misses a pageManager!" << endl;
        return -1;
    }
    return frameSet()->pageManager()->pageNumber(this);
}

int KWFrame::pageNumber( KWDocument* doc ) const
{
    return doc->pageManager()->pageNumber(this);
}

KWFrame *KWFrame::getCopy() {
    /* returns a deep copy of self */
    return new KWFrame(this);
}

void KWFrame::copySettings(KWFrame *frm)
{
    setFrameSet( frm->frameSet() ); // do this first in case of debug output in the methods below
    setRect(frm->x(), frm->y(), frm->width(), frm->height());
    // Keep order identical as member var order (and init in ctor)
    setSheetSide(frm->sheetSide());
    setRunAround(frm->runAround());
    setRunAroundSide(frm->runAroundSide());
    setFrameBehavior(frm->frameBehavior());
    setNewFrameBehavior(frm->newFrameBehavior());
    setRunAroundGap(frm->runAroundLeft(), frm->runAroundRight(), frm->runAroundTop(), frm->runAroundBottom());
    setPaddingLeft(frm->paddingLeft());
    setPaddingRight(frm->paddingRight());
    setPaddingTop(frm->paddingTop());
    setPaddingBottom(frm->paddingBottom());
    setMinimumFrameHeight(frm->minimumFrameHeight());
    m_internalY = 0; // internal Y is recalculated
    setZOrder(frm->zOrder());
    setCopy(frm->isCopy());
    m_drawFootNoteLine = false; // recalculated
    setBackgroundColor( frm->backgroundColor() );
    setLeftBorder(frm->leftBorder());
    setRightBorder(frm->rightBorder());
    setTopBorder(frm->topBorder());
    setBottomBorder(frm->bottomBorder());
}

void KWFrame::frameBordersChanged() {
    if (frameSet()->isFloating())
        frameSet()->anchorFrameset()->invalidate();
}


void KWFrame::updateRulerHandles(){
// TODO
#if 0
    if(! isSelected())
    {
        KWDocument *doc = frameSet()->kWordDocument();
        if(doc)
            doc->updateRulerFrameStartEnd();
    }
#endif
}

QRect KWFrame::outerRect( KWViewMode* viewMode ) const
{
    KWDocument *doc = m_frameSet->kWordDocument();
    QRect outerRect( doc->zoomRect( *this ) );
    if ( viewMode && !m_frameSet->groupmanager() ) {
        int minBorder = viewMode->drawFrameBorders() ? 1 : 0;
        KWFrame* settingsFrame = m_frameSet->settingsFrame( this );
        outerRect.rLeft() -= KoBorder::zoomWidthX( settingsFrame->leftBorder().width(), doc, minBorder );
        outerRect.rTop() -= KoBorder::zoomWidthY( settingsFrame->topBorder().width(), doc, minBorder );
        outerRect.rRight() += KoBorder::zoomWidthX( settingsFrame->rightBorder().width(), doc, minBorder );
        outerRect.rBottom() += KoBorder::zoomWidthY( settingsFrame->bottomBorder().width(), doc, minBorder );
    }
    return outerRect;
}

KoRect KWFrame::outerKoRect() const
{
    KoRect outerRect = *this;
    KWDocument *doc = m_frameSet->kWordDocument();
    KWFrame* settingsFrame = m_frameSet->settingsFrame( this );
    outerRect.rLeft() -= KoBorder::zoomWidthX( settingsFrame->leftBorder().width(), doc, 1 ) / doc->zoomedResolutionX();
    outerRect.rTop() -= KoBorder::zoomWidthY( settingsFrame->topBorder().width(), doc, 1 ) / doc->zoomedResolutionY();
    outerRect.rRight() += KoBorder::zoomWidthX( settingsFrame->rightBorder().width(), doc, 1 ) / doc->zoomedResolutionX();
    outerRect.rBottom() += KoBorder::zoomWidthY( settingsFrame->bottomBorder().width(), doc, 1 ) / doc->zoomedResolutionY();
    return outerRect;
}

KoRect KWFrame::runAroundRect() const
{
    KoRect raRect = outerKoRect();
    raRect.rLeft() -= m_runAroundLeft;
    raRect.rRight() += m_runAroundRight;
    raRect.rTop() -= m_runAroundTop;
    raRect.rBottom() += m_runAroundBottom;
    return raRect;
}

void KWFrame::save( QDomElement &frameElem )
{
    // setAttribute( double ) uses a default precision of 6, and this seems
    // to be 6 digits, even like '123.123' !
    frameElem.setAttribute( "left", QString::number( left(), 'g', DBL_DIG ) );
    frameElem.setAttribute( "top", QString::number( top(), 'g', DBL_DIG ) );
    frameElem.setAttribute( "right", QString::number( right(), 'g', DBL_DIG ) );
    frameElem.setAttribute( "bottom", QString::number( bottom(), 'g', DBL_DIG ) );
    if ( minimumFrameHeight() > 0 )
        frameElem.setAttribute( "min-height", QString::number( minimumFrameHeight(), 'g', DBL_DIG ) );

    if ( !m_frameSet->isHeaderOrFooter() && !m_frameSet->isMainFrameset() )
    {
        if(runAround()!=RA_NO)
        {
            frameElem.setAttribute( "runaround", static_cast<int>( runAround() ) );
            if (runAround() == RA_BOUNDINGRECT)
            {
                if (runAroundSide()==RA_LEFT)
                    frameElem.setAttribute( "runaroundSide", "left" );
                else if (runAroundSide()==RA_RIGHT)
                    frameElem.setAttribute( "runaroundSide", "right" );
                else
                    frameElem.setAttribute( "runaroundSide", "biggest" );
            }
        }
        if(runAroundLeft()!=0 || runAroundRight()!=0 || runAroundTop()!=0 || runAroundBottom()!=0) {
            frameElem.setAttribute( "runaroundLeft", m_runAroundLeft );
            frameElem.setAttribute( "runaroundRight", m_runAroundRight );
            frameElem.setAttribute( "runaroundTop", m_runAroundTop );
            frameElem.setAttribute( "runaroundBottom", m_runAroundBottom );
            // The old file format had only one value, keep compat
            double runAroundGap = QMAX( QMAX( m_runAroundLeft, m_runAroundRight ), QMAX( m_runAroundTop, m_runAroundBottom ) );
            frameElem.setAttribute( "runaroundGap", runAroundGap );
        }
    }

    if(leftBorder().penWidth()!=0)
        frameElem.setAttribute( "lWidth", leftBorder().penWidth() );

    if(leftBorder().color.isValid())
    {
        frameElem.setAttribute( "lRed", leftBorder().color.red() );
        frameElem.setAttribute( "lGreen", leftBorder().color.green() );
        frameElem.setAttribute( "lBlue", leftBorder().color.blue() );
    }
    if(leftBorder().getStyle() != KoBorder::SOLID)
        frameElem.setAttribute( "lStyle", static_cast<int>( leftBorder().getStyle()) );

    if(rightBorder().penWidth()!=0)
        frameElem.setAttribute( "rWidth", rightBorder().penWidth() );

    if(rightBorder().color.isValid())
    {
        frameElem.setAttribute( "rRed", rightBorder().color.red() );
        frameElem.setAttribute( "rGreen", rightBorder().color.green() );
        frameElem.setAttribute( "rBlue", rightBorder().color.blue() );
    }
    if(rightBorder().getStyle() != KoBorder::SOLID)
        frameElem.setAttribute( "rStyle", static_cast<int>( rightBorder().getStyle() ) );

    if(topBorder().penWidth()!=0)
        frameElem.setAttribute( "tWidth", topBorder().penWidth() );

    if(topBorder().color.isValid())
    {
        frameElem.setAttribute( "tRed", topBorder().color.red() );
        frameElem.setAttribute( "tGreen", topBorder().color.green() );
        frameElem.setAttribute( "tBlue", topBorder().color.blue() );
    }
    if(topBorder().getStyle() != KoBorder::SOLID)
        frameElem.setAttribute( "tStyle", static_cast<int>( topBorder().getStyle() ) );

    if(bottomBorder().penWidth()!=0) {
        frameElem.setAttribute( "bWidth", bottomBorder().penWidth() );
    }
    if(bottomBorder().color.isValid()) {
        frameElem.setAttribute( "bRed", bottomBorder().color.red() );
        frameElem.setAttribute( "bGreen", bottomBorder().color.green() );
        frameElem.setAttribute( "bBlue", bottomBorder().color.blue() );
    }
    if(bottomBorder().getStyle() != KoBorder::SOLID)
        frameElem.setAttribute( "bStyle", static_cast<int>( bottomBorder().getStyle() ) );

    if(backgroundColor().color().isValid())
    {
        frameElem.setAttribute( "bkRed", backgroundColor().color().red() );
        frameElem.setAttribute( "bkGreen", backgroundColor().color().green() );
        frameElem.setAttribute( "bkBlue", backgroundColor().color().blue() );
        frameElem.setAttribute( "bkStyle", (int)backgroundColor().style ());
    }
    if(paddingLeft() != 0)
        frameElem.setAttribute( "bleftpt", paddingLeft() );

    if(paddingRight()!=0)
        frameElem.setAttribute( "brightpt", paddingRight() );

    if(paddingTop()!=0)
        frameElem.setAttribute( "btoppt", paddingTop() );

    if(paddingBottom()!=0)
        frameElem.setAttribute( "bbottompt", paddingBottom() );

    if(frameBehavior()!=AutoCreateNewFrame)
        frameElem.setAttribute( "autoCreateNewFrame", static_cast<int>( frameBehavior()) );

    //if(newFrameBehavior()!=Reconnect) // always save this one, since the default value depends on the type of frame, etc.
    frameElem.setAttribute( "newFrameBehavior", static_cast<int>( newFrameBehavior()) );

    //same reason
    frameElem.setAttribute( "copy", static_cast<int>( m_bCopy ) );

    if(sheetSide()!= AnySide)
        frameElem.setAttribute( "sheetSide", static_cast<int>( sheetSide()) );

    frameElem.setAttribute( "z-index", zOrder() );
}

void KWFrame::load( QDomElement &frameElem, KWFrameSet* frameSet, int syntaxVersion )
{
    m_minFrameHeight = KWDocument::getAttribute( frameElem, "min-height", 0.0 );
    m_runAround = static_cast<RunAround>( KWDocument::getAttribute( frameElem, "runaround", RA_NO ) );
    QString str = frameElem.attribute( "runaroundSide" );
    if ( str == "left" )
        m_runAroundSide = RA_LEFT;
    else if ( str == "right" )
        m_runAroundSide = RA_RIGHT;
    else
        m_runAroundSide = RA_BIGGEST;

    double runAroundGap = ( frameElem.hasAttribute( "runaroundGap" ) )
                          ? frameElem.attribute( "runaroundGap" ).toDouble()
                          : frameElem.attribute( "runaGapPT" ).toDouble();
    setRunAroundGap( KWDocument::getAttribute( frameElem, "runaroundLeft", runAroundGap ),
                     KWDocument::getAttribute( frameElem, "runaroundRight", runAroundGap ),
                     KWDocument::getAttribute( frameElem, "runaroundTop", runAroundGap ),
                     KWDocument::getAttribute( frameElem, "runaroundBottom", runAroundGap ) );

    m_sheetSide = static_cast<SheetSide>( KWDocument::getAttribute( frameElem, "sheetSide", AnySide ) );
    m_frameBehavior = static_cast<FrameBehavior>( KWDocument::getAttribute( frameElem, "autoCreateNewFrame", AutoCreateNewFrame ) );
    // Old documents had no "NewFrameBehavior" for footers/headers -> default to Copy.
    NewFrameBehavior defaultValue = frameSet->isHeaderOrFooter() ? Copy : Reconnect;
    // for old document we used the British spelling (newFrameBehaviour), so this is for backwards compatibility.
    defaultValue = static_cast<NewFrameBehavior>( KWDocument::getAttribute( frameElem, "newFrameBehaviour", defaultValue ) );
    m_newFrameBehavior = static_cast<NewFrameBehavior>( KWDocument::getAttribute( frameElem, "newFrameBehavior", defaultValue ) );
    if ( frameSet->isFootEndNote() ) // note that isFootNote/isEndNote are not possible yet
        m_newFrameBehavior = NoFollowup;

    KoBorder l, r, t, b;
    l.setPenWidth( KWDocument::getAttribute( frameElem, "lWidth", 0.0 ));
    r.setPenWidth(KWDocument::getAttribute( frameElem, "rWidth", 0.0 ));
    t.setPenWidth(KWDocument::getAttribute( frameElem, "tWidth", 0.0 ));
    b.setPenWidth(KWDocument::getAttribute( frameElem, "bWidth", 0.0 ));
    if ( frameElem.hasAttribute("lRed") )
        l.color.setRgb(
            KWDocument::getAttribute( frameElem, "lRed", 0 ),
            KWDocument::getAttribute( frameElem, "lGreen", 0 ),
            KWDocument::getAttribute( frameElem, "lBlue", 0 ) );
    if ( frameElem.hasAttribute("rRed") )
        r.color.setRgb(
            KWDocument::getAttribute( frameElem, "rRed", 0 ),
            KWDocument::getAttribute( frameElem, "rGreen", 0 ),
            KWDocument::getAttribute( frameElem, "rBlue", 0 ) );
    if ( frameElem.hasAttribute("tRed") )
        t.color.setRgb(
            KWDocument::getAttribute( frameElem, "tRed", 0 ),
            KWDocument::getAttribute( frameElem, "tGreen", 0 ),
            KWDocument::getAttribute( frameElem, "tBlue", 0 ) );
    if ( frameElem.hasAttribute("bRed") )
        b.color.setRgb(
            KWDocument::getAttribute( frameElem, "bRed", 0 ),
            KWDocument::getAttribute( frameElem, "bGreen", 0 ),
            KWDocument::getAttribute( frameElem, "bBlue", 0 ) );
    l.setStyle(static_cast<KoBorder::BorderStyle>( KWDocument::getAttribute( frameElem, "lStyle", KoBorder::SOLID ) ));
    r.setStyle(static_cast<KoBorder::BorderStyle>( KWDocument::getAttribute( frameElem, "rStyle", KoBorder::SOLID ) ));
    t.setStyle(static_cast<KoBorder::BorderStyle>( KWDocument::getAttribute( frameElem, "tStyle", KoBorder::SOLID ) ));
    b.setStyle( static_cast<KoBorder::BorderStyle>( KWDocument::getAttribute( frameElem, "bStyle", KoBorder::SOLID ) ));
    QColor c;
    if ( frameElem.hasAttribute("bkRed") )
        c.setRgb(
            KWDocument::getAttribute( frameElem, "bkRed", 0 ),
            KWDocument::getAttribute( frameElem, "bkGreen", 0 ),
            KWDocument::getAttribute( frameElem, "bkBlue", 0 ) );

    if ( syntaxVersion < 2 ) // Activate old "white border == no border" conversion
    {
        if(c==l.color && l.penWidth()==1 && l.getStyle()==0 )
            l.setPenWidth(0);
        if(c==r.color  && r.penWidth()==1 && r.getStyle()==0)
            r.setPenWidth(0);
        if(c==t.color && t.penWidth()==1 && t.getStyle()==0 )
            t.setPenWidth(0);
        if(c==b.color && b.penWidth()==1 && b.getStyle()==0 )
            b.setPenWidth(0);
    }
    m_borderLeft = l;
    m_borderRight = r;
    m_borderTop = t;
    m_borderBottom = b;
    m_backgroundColor = QBrush( c );


    if( frameElem.hasAttribute("bkStyle"))
        m_backgroundColor.setStyle (static_cast<Qt::BrushStyle>(KWDocument::getAttribute( frameElem, "bkStyle", Qt::SolidPattern )));

    m_paddingLeft = frameElem.attribute( "bleftpt" ).toDouble();
    m_paddingRight = frameElem.attribute( "brightpt" ).toDouble();
    m_paddingTop = frameElem.attribute( "btoppt" ).toDouble();
    m_paddingBottom = frameElem.attribute( "bbottompt" ).toDouble();
    m_bCopy = KWDocument::getAttribute( frameElem, "copy", frameSet->isHeaderOrFooter() /* default to true for h/f */ );
    m_zOrder = frameElem.attribute( "z-index" ).toInt();
}

// This is shared with table cells - so, no runaround and newframebehavior etc.
// Only background, borders, padding.
void KWFrame::loadBorderProperties( KoStyleStack& styleStack )
{
    // padding. fo:padding for 4 values or padding-left/right/top/bottom
    m_paddingLeft = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "padding", "left" ) );
    m_paddingRight = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "padding", "right" ) );
    m_paddingTop = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "padding", "top" ) );
    m_paddingBottom = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "padding", "bottom" ) );

    // background color (3.11.25)
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "background-color" ) ) {
        QString color = styleStack.attributeNS( KoXmlNS::fo, "background-color" );
        if ( color == "transparent" )
            m_backgroundColor = QBrush( QColor(), Qt::NoBrush );
        else
        {
            m_backgroundColor = QBrush( QColor( color ) /*, brush style is a dead feature, ignored */ );
        }
    }
    // OOo compatibility: it uses background-transparency=100% instead of background-color="transparent"
    if ( styleStack.hasAttributeNS( KoXmlNS::fo, "background-transparency" ) ) {
        QString transp = styleStack.attributeNS( KoXmlNS::fo, "background-transparency" );
        if ( transp == "100%" )
            m_backgroundColor.setStyle( Qt::NoBrush );
    }

    // borders (3.11.27)
    // can be none/hidden, solid and double. General form is the XSL/FO "width|style|color"
    {
        m_borderLeft.loadFoBorder( styleStack.attributeNS( KoXmlNS::fo, "border", "left") );
        m_borderRight.loadFoBorder( styleStack.attributeNS( KoXmlNS::fo, "border", "right") );
        m_borderTop.loadFoBorder( styleStack.attributeNS( KoXmlNS::fo, "border", "top") );
        m_borderBottom.loadFoBorder( styleStack.attributeNS( KoXmlNS::fo, "border", "bottom") );
    }
    // TODO more refined border spec for double borders (3.11.28)
}

void KWFrame::loadCommonOasisProperties( KoOasisContext& context, KWFrameSet* frameSet, const char* typeProperties )
{
    KoStyleStack& styleStack = context.styleStack();
    styleStack.setTypeProperties( typeProperties );

    loadBorderProperties( styleStack );

#if 0 // not allowed in the current OASIS spec
    // margins, i.e. runAroundGap. fo:margin for 4 values or padding-left/right/top/bottom
    m_runAroundLeft = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin", "left" ) );
    m_runAroundRight = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin", "right" ) );
    m_runAroundTop = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin", "top" ) );
    m_runAroundBottom = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin", "bottom" ) );
#endif
    // margins, i.e. runAroundGap. fo:margin-left/right/top/bottom
    m_runAroundLeft = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin-left" ) );
    m_runAroundRight = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin-right" ) );
    m_runAroundTop = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin-top" ) );
    m_runAroundBottom = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::fo, "margin-bottom" ) );

    // This attribute isn't part of the OASIS spec. Doesn't matter since it doesn't affect rendering
    // of existing documents, only editing (and only KWord has this kind of option until now).
    const QCString frameBehaviorOnNewPage = styleStack.attributeNS( KoXmlNS::koffice, "frame-behavior-on-new-page" ).latin1();
    if ( frameBehaviorOnNewPage == "followup" )
        m_newFrameBehavior = Reconnect;
    else if ( frameBehaviorOnNewPage == "copy" )
        m_newFrameBehavior = Copy;
    else if ( frameBehaviorOnNewPage == "none" )
        m_newFrameBehavior = NoFollowup;
    else { // Defaults for OASIS documents not created by KWord
        m_newFrameBehavior = frameSet->isHeaderOrFooter() ? Copy : NoFollowup;
        if ( !frameBehaviorOnNewPage.isEmpty() )
            kdWarning(32001) << "Unknown value for koffice:frame-behavior-on-new-page: " << frameBehaviorOnNewPage << endl;
    }
    // Footnotes and endnotes are handled in a special way.
    if ( frameSet->isFootEndNote() ) // note that isFootNote/isEndNote are not possible yet
        m_newFrameBehavior = NoFollowup;

    KWFrame::RunAround runAround = KWFrame::RA_BOUNDINGRECT;
    KWFrame::RunAroundSide runAroundSide = KWFrame::RA_BIGGEST;
    const QCString oowrap = styleStack.attributeNS( KoXmlNS::style, "wrap" ).latin1();
    if ( oowrap == "none" )        // 'no wrap' means 'avoid horizontal space'
        runAround = KWFrame::RA_SKIP;
    else if ( oowrap == "left" )
        runAroundSide = KWFrame::RA_LEFT;
    else if ( oowrap == "right" )
        runAroundSide= KWFrame::RA_RIGHT;
    else if ( oowrap == "run-through" )
        runAround = KWFrame::RA_NO;
    //if ( oowrap == "biggest" ) // OASIS extension
    // ->( KWFrame::RA_BOUNDINGRECT, KWFrame::RA_BIGGEST ), already set above
    //if ( oowrap == "parallel" || oowrap == "dynamic" )
    // dynamic is called "optimal" in the OO GUI. It's different from biggest because it can lead to parallel.
    // Those are not supported in KWord, let's use biggest instead
    setRunAround( runAround );
    setRunAroundSide( runAroundSide );
}

void KWFrame::startOasisFrame( KoXmlWriter &writer, KoGenStyles& mainStyles, const QString& name, const QString& lastFrameName ) const
{
    writer.startElement( "draw:frame" );
    writer.addAttribute( "draw:name", name );
    writer.addAttribute( "draw:style-name", saveOasisFrameStyle( mainStyles ) );

    if ( !frameSet()->isFloating() )
    { // non-inline frame, anchored to page
        const int pgNum = pageNumber();
        const double yInPage = top() - frameSet()->pageManager()->topOfPage(pgNum);
        writer.addAttributePt( "svg:x", left() );
        writer.addAttributePt( "svg:y", yInPage );
        writer.addAttribute( "text:anchor-type", "page" );
        writer.addAttribute( "text:anchor-page-number", pgNum );
        writer.addAttribute( "draw:z-index", zOrder() );
    }
    writer.addAttributePt( "svg:width", width() );
    writer.addAttributePt( "svg:height", height() );
    if ( isCopy() )
        writer.addAttribute( "draw:copy-of", lastFrameName );
}

// shared between startOasisFrame and table cells.
// Only background, borders, padding.
void KWFrame::saveBorderProperties( KoGenStyle& frameStyle ) const
{
    // Background: color and transparency
    // OOo seems to use style:background-transparency="100%", but the schema allows background-color=transparent
    if ( m_backgroundColor.style() == Qt::NoBrush )
        frameStyle.addProperty( "fo:background-color", "transparent" );
    else if ( m_backgroundColor.color().isValid() )
        frameStyle.addProperty( "fo:background-color", m_backgroundColor.color().name() );

    // Borders
    if (  ( m_borderLeft == m_borderRight )
          && ( m_borderLeft == m_borderTop )
          && ( m_borderLeft == m_borderBottom ) )
    {
        frameStyle.addProperty( "fo:border", m_borderLeft.saveFoBorder() );
    }
    else
    {
        frameStyle.addProperty( "fo:border-left", m_borderLeft.saveFoBorder() );
        frameStyle.addProperty( "fo:border-right", m_borderRight.saveFoBorder() );
        frameStyle.addProperty( "fo:border-top", m_borderTop.saveFoBorder() );
        frameStyle.addProperty( "fo:border-bottom", m_borderBottom.saveFoBorder() );
    }

    if ( m_paddingLeft != 0 && ( ( m_paddingLeft == m_paddingRight )
                                 && ( m_paddingLeft == m_paddingTop )
                                 && ( m_paddingLeft == m_paddingBottom ) ) )
        frameStyle.addPropertyPt( "fo:padding", m_paddingLeft );
    else
    {
        if ( m_paddingLeft != 0 )
            frameStyle.addPropertyPt( "fo:padding-left", m_paddingLeft );
        if ( m_paddingRight != 0 )
            frameStyle.addPropertyPt( "fo:padding-right", m_paddingRight );
        if ( m_paddingTop != 0 )
            frameStyle.addPropertyPt( "fo:padding-top", m_paddingTop );
        if ( m_paddingBottom != 0 )
            frameStyle.addPropertyPt( "fo:padding-bottom", m_paddingBottom );
    }
}

void KWFrame::saveMarginAttributes( KoXmlWriter &writer ) const
{
    if ( m_runAroundLeft != 0 )
        writer.addAttributePt( "fo:margin-left", m_runAroundLeft );
    if ( m_runAroundRight != 0 )
        writer.addAttributePt( "fo:margin-right", m_runAroundRight );
    if ( m_runAroundTop != 0 )
        writer.addAttributePt( "fo:margin-top", m_runAroundTop );
    if ( m_runAroundBottom != 0 )
        writer.addAttributePt( "fo:margin-bottom", m_runAroundBottom );
}

void KWFrame::saveMarginProperties( KoGenStyle& frameStyle ) const
{
#if 0 // not allowed in the current OASIS spec
    if ( m_runAroundLeft != 0 && ( ( m_runAroundLeft == m_runAroundRight )
                                 && ( m_runAroundLeft == m_runAroundTop )
                                 && ( m_runAroundLeft == m_runAroundBottom ) ) )
        frameStyle.addPropertyPt( "fo:margin", m_runAroundLeft );
    else
    {
#endif
        if ( m_runAroundLeft != 0 )
            frameStyle.addPropertyPt( "fo:margin-left", m_runAroundLeft );
        if ( m_runAroundRight != 0 )
            frameStyle.addPropertyPt( "fo:margin-right", m_runAroundRight );
        if ( m_runAroundTop != 0 )
            frameStyle.addPropertyPt( "fo:margin-top", m_runAroundTop );
        if ( m_runAroundBottom != 0 )
            frameStyle.addPropertyPt( "fo:margin-bottom", m_runAroundBottom );
#if 0 // not allowed in the current OASIS spec
    }
#endif
}

QString KWFrame::saveOasisFrameStyle( KoGenStyles& mainStyles ) const
{
    KoGenStyle frameStyle( KWDocument::STYLE_FRAME_AUTO, "graphic" );
    QString protect;
    if ( frameSet()->protectContent() )
        protect = "content";
    if ( frameSet()->isProtectSize() ) // ## should be moved for frame
    {
        if ( !protect.isEmpty() )
            protect+=" ";
        protect+="size";
    }
    if ( !protect.isEmpty() )
        frameStyle.addProperty( "style:protect", protect );

    if ( !frameSet()->isFloating() )
    { // non-inline frame, anchored to page
        frameStyle.addProperty( "style:horizontal-rel", "page" );
        frameStyle.addProperty( "style:vertical-rel", "page" );
        frameStyle.addProperty( "style:horizontal-pos", "from-left" );
        frameStyle.addProperty( "style:vertical-pos", "from-top" );
    }

    saveBorderProperties( frameStyle );
    saveMarginProperties( frameStyle );

    if ( runAround() == KWFrame::RA_SKIP )
        frameStyle.addProperty( "style:wrap", "none" );
    else if ( runAround() == KWFrame::RA_NO )
        frameStyle.addProperty( "style:wrap", "run-through" );
    else // RA_BOUNDINGRECT
    {
        if ( runAroundSide() ==  KWFrame::RA_LEFT )
            frameStyle.addProperty( "style:wrap", "left" );
        else if ( runAroundSide() == KWFrame::RA_RIGHT )
            frameStyle.addProperty( "style:wrap", "right" );
        else if ( runAroundSide() == KWFrame::RA_BIGGEST )
            frameStyle.addProperty( "style:wrap", "biggest" );
    }

    // This attribute isn't part of the OASIS spec. Doesn't matter since it doesn't affect rendering
    // of existing documents, only editing (and only KWord has this kind of option until now).
    NewFrameBehavior defaultNfb = frameSet()->isHeaderOrFooter() ? Copy : NoFollowup;
    if ( m_newFrameBehavior != defaultNfb ) {
        const char* value = "none";
        if ( m_newFrameBehavior == Reconnect )
            value = "followup";
        else if ( m_newFrameBehavior == Copy )
            value = "copy";
        else if ( m_newFrameBehavior == NoFollowup )
            value = "none";
        frameStyle.addProperty( "koffice:frame-behavior-on-new-page", value );
    }

    // The loading code for this one is in kwtextframeset, maybe this should be moved there too
    const char* frameBehav = 0;
    if ( m_frameBehavior == KWFrame::Ignore )
        frameBehav = "clip";
    else if ( m_frameBehavior == KWFrame::AutoCreateNewFrame )
        frameBehav = "auto-create-new-frame";
    // the third case, AutoExtendFrame is handled by min-height
    if ( frameBehav )
        frameStyle.addProperty( "style:overflow-behavior", frameBehav );

    return mainStyles.lookup( frameStyle, "fr" );
}

bool KWFrame::frameAtPos( const QPoint& point, bool borderOfFrameOnly) const {
    // Forwarded to KWFrameSet to make it virtual
    return frameSet()->isFrameAtPos( this, point, borderOfFrameOnly );
}

KoRect KWFrame::innerRect() const
{
    KoRect inner( this->normalize());
    inner.moveBy( paddingLeft(), paddingTop() );
    inner.setWidth( innerWidth() );
    inner.setHeight( innerHeight() );
    return inner;
}

double KWFrame::innerWidth() const
{
    return KMAX( 0.0, width() - m_paddingLeft - m_paddingRight );
}

double KWFrame::innerHeight() const
{
    return KMAX( 0.0, height() - m_paddingTop - m_paddingBottom );
}

void KWFrame::setFramePadding( double left, double top, double right, double bottom)
{
    m_paddingLeft = left;
    m_paddingTop = top;
    m_paddingRight = right;
    m_paddingBottom = bottom;
}

bool KWFrame::compareFrameZOrder(KWFrame *f1, KWFrame *f2)
{
    return f1->zOrder() < f2->zOrder();
}
