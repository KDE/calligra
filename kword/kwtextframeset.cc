/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qregexp.h>

#include "kwtextframeset.h"
#include "kwtableframeset.h"
#include "kwdoc.h"
#include "kwview.h"
#include "kwviewmode.h"
#include "kwcanvas.h"
#include "kwanchor.h"
#include "kwcommand.h"
#include "kwdrag.h"
#include <koparagcounter.h>
#include "contents.h"
#include <koVariableDlgs.h>
#include "mailmerge.h"
#include <koAutoFormat.h>
#include <qclipboard.h>
#include <qprogressdialog.h>
#include <qpopupmenu.h>
#include <kapplication.h>
#include <klocale.h>
#include <kaction.h>
#include <kotextobject.h>
#include <kocommand.h>
#include <kotextformatter.h>
#include <krun.h>
#include <kmessagebox.h>
#include "kwvariable.h"
#include <koChangeCaseDia.h>
#include "KWordTextFrameSetIface.h"
#include "KWordTextFrameSetEditIface.h"
#include "KWordFootNoteFrameSetIface.h"
#include "KWordFrameSetIface.h"
#include <kdebug.h>
#include <kdebugclasses.h>
#include <assert.h>
//#define DEBUG_MARGINS
//#define DEBUG_FORMATVERTICALLY
//#define DEBUG_FORMATS
//#define DEBUG_FORMAT_MORE
//#define DEBUG_VIEWAREA
//#define DEBUG_CURSOR

//#define DEBUG_DTI
//#define DEBUG_ITD

/**
 * KWord's text formatter.
 * It derives from KoTextFormatter and simply forwards formatVertically to KWTextFrameSet,
 * since only KWTextFrameSet knows about page-breaking, overlapping frames etc.
 */
class KWTextFormatter : public KoTextFormatter
{
public:
    KWTextFormatter( KWTextFrameSet *textfs ) : m_textfs( textfs ) {}
    virtual ~KWTextFormatter() {}

    virtual int formatVertically( KoTextDocument*, KoTextParag* parag )
    {
        return m_textfs->formatVertically( parag );
    }
private:
    KWTextFrameSet *m_textfs;
};

KWTextFrameSet::KWTextFrameSet( KWDocument *_doc, const QString & name )
    : KWFrameSet( _doc )
{
    //kdDebug() << "KWTextFrameSet::KWTextFrameSet " << this << endl;
    if ( name.isEmpty() )
        m_name = _doc->generateFramesetName( i18n( "Text Frameset %1" ) );
    else
        m_name = name;

    QObject::setName( m_name.utf8() ); // store name in the QObject, for DCOP users
    m_currentViewMode = 0L;
    m_currentDrawnFrame = 0L;
    m_lastTextDocHeight = 0;
    // Create the text document to set in the text object
    KWTextDocument* textdoc = new KWTextDocument( this,
        new KoTextFormatCollection( _doc->defaultFont(), QColor() ),
                                                  new KWTextFormatter( this ) );
    textdoc->setFlow( this );
    textdoc->setPageBreakEnabled( true );              // get verticalBreak to be called
    if ( _doc->tabStopValue() != -1 )
        textdoc->setTabStops( _doc->ptToLayoutUnitPixX( _doc->tabStopValue() ));

    m_textobj = new KoTextObject( textdoc, m_doc->styleCollection()->findStyle( "Standard" ),
                                  this, (m_name+"-textobj").utf8() );

    connect( m_textobj, SIGNAL( availableHeightNeeded() ),
             SLOT( slotAvailableHeightNeeded() ) );
    connect( m_textobj, SIGNAL( afterFormatting( int, KoTextParag*, bool* ) ),
             SLOT( slotAfterFormatting( int, KoTextParag*, bool* ) ) );
    //connect( m_textobj, SIGNAL( formattingFirstParag() ),
    //         SLOT( slotFormattingFirstParag() ) );
    //connect( m_textobj, SIGNAL( chapterParagraphFormatted( KoTextParag * ) ),
    //         SLOT( slotChapterParagraphFormatted( KoTextParag * ) ) );
    connect( m_textobj, SIGNAL( newCommand( KCommand * ) ),
             SLOT( slotNewCommand( KCommand * ) ) );
    connect( m_textobj, SIGNAL( repaintChanged( KoTextObject* ) ),
             SLOT( slotRepaintChanged() ) );
    connect( m_textobj, SIGNAL( paragraphDeleted( KoTextParag*) ),
             SLOT( slotParagraphDeleted(KoTextParag*) ));

    connect( m_textobj, SIGNAL( paragraphCreated( KoTextParag*) ),
             SLOT( slotParagraphCreated(KoTextParag*) ));
    connect( m_textobj, SIGNAL( paragraphModified( KoTextParag*, int, int, int) ),
             SLOT( slotParagraphModified(KoTextParag*, int, int, int) ));

}

void KWTextFrameSet::slotParagraphModified(KoTextParag* _parag, int /*KoTextParag::ParagModifyType*/ _type, int start, int lenght)
{
    kWordDocument()->paragraphModified(_parag, _type, start, lenght);
}

void KWTextFrameSet::slotParagraphCreated(KoTextParag* /*_parag*/)
{
    //todo
}

void KWTextFrameSet::slotParagraphDeleted(KoTextParag*_parag)
{
    kWordDocument()->spellCheckParagraphDeleted( _parag,  this);
    kWordDocument()->paragraphDeleted( _parag, this);
}

KWordFrameSetIface* KWTextFrameSet::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KWordTextFrameSetIface( this );

    return m_dcop;
}

KWFrameSetEdit * KWTextFrameSet::createFrameSetEdit( KWCanvas * canvas )
{
    return new KWTextFrameSetEdit( this, canvas );
}

KoTextDocument * KWTextFrameSet::textDocument() const
{
    return m_textobj->textDocument();
}

KWTextDocument * KWTextFrameSet::kwTextDocument() const
{
     return static_cast<KWTextDocument *>(m_textobj->textDocument());
}

void KWTextFrameSet::slotAvailableHeightNeeded()
{
    kdDebug() << "KWTextFrameSet::slotAvailableHeightNeeded " << getName() << endl;
    updateFrames( /* TODO try passing 0 for speedup */ );
}

KWFrame * KWTextFrameSet::documentToInternal( const KoPoint &dPoint, QPoint &iPoint ) const
{
#ifdef DEBUG_DTI
    kdDebug() << "KWTextFrameSet::documentToInternal dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
    if ( !m_doc->viewMode()->hasFrames() ) { // text viewmode
        iPoint = QPoint( m_doc->ptToLayoutUnitPixX( dPoint.x() ),
                         m_doc->ptToLayoutUnitPixY( dPoint.y() ) );
        return frames.getFirst();
    }
    // Find the frame that contains dPoint. To go fast, we look them up by page number.
    int pageNum = static_cast<int>( dPoint.y() / m_doc->ptPaperHeight() );
    QPtrListIterator<KWFrame> frameIt( framesInPage( pageNum ) );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *theFrame = frameIt.current();
        if ( theFrame->contains( dPoint ) )
        {
            iPoint.setX( m_doc->ptToLayoutUnitPixX( dPoint.x() - theFrame->innerRect().x() ) );
            iPoint.setY( m_doc->ptToLayoutUnitPixY( dPoint.y() - theFrame->innerRect().y() + theFrame->internalY() ) );
#ifdef DEBUG_DTI
            kdDebug() << "documentToInternal: returning " << iPoint.x() << "," << iPoint.y()
                      << " internalY=" << theFrame->internalY() << " because frame=" << theFrame
                      << " contains dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
            return theFrame;
        }
#ifdef DEBUG_DTI
        //else
        //  kdDebug() << "DTI: " << frameRect
        //            << " doesn't contain nPoint:" << nPoint.x() << "," << nPoint.y() << endl;
#endif
    }
    // Not found. This means the mouse isn't over any frame, in the page pageNum.
    iPoint = m_doc->ptToLayoutUnitPix( dPoint ); // bah
    return 0;
}

KWFrame * KWTextFrameSet::documentToInternalMouseSelection( const KoPoint &dPoint, QPoint &iPoint, RelativePosition& relPos ) const
{
#ifdef DEBUG_DTI
    kdDebug() << "KWTextFrameSet::documentToInternalMouseSelection dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
    if ( !m_doc->viewMode()->hasFrames() ) { // text viewmode
        relPos = InsideFrame;
        iPoint = QPoint( m_doc->ptToLayoutUnitPixX( dPoint.x() ),
                         m_doc->ptToLayoutUnitPixY( dPoint.y() ) );
        return frames.getFirst();
    }

    // Find the frame that contains dPoint. To go fast, we look them up by page number.
    int pageNum = static_cast<int>( dPoint.y() / m_doc->ptPaperHeight() );
    QPtrListIterator<KWFrame> frameIt( framesInPage( pageNum ) );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *theFrame = frameIt.current();
        if ( theFrame->contains( dPoint ) )
        {
            iPoint.setX( m_doc->ptToLayoutUnitPixX( dPoint.x() - theFrame->innerRect().x() ) );
            iPoint.setY( m_doc->ptToLayoutUnitPixY( dPoint.y() - theFrame->innerRect().y() + theFrame->internalY() ) );
#ifdef DEBUG_DTI
            kdDebug() << "documentToInternal: returning InsideFrame " << iPoint.x() << "," << iPoint.y()
                      << " internalY=" << theFrame->internalY() << " because frame=" << theFrame
                      << " contains dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
            relPos = InsideFrame;
            return theFrame;
        }
    }
    frameIt.toFirst();
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *theFrame = frameIt.current();
        KoRect openLeftRect( theFrame->innerRect() );
        openLeftRect.setLeft( theFrame->bLeft() );
#ifdef DEBUG_DTI
        kdDebug() << "documentToInternal: openLeftRect=" << openLeftRect << endl;
#endif
        if ( openLeftRect.contains( dPoint ) )
        {
            // We are at the left of this frame (and not in any other frame of this frameset)
            iPoint.setX( m_doc->ptToLayoutUnitPixX(theFrame->innerRect().left() ));
            iPoint.setY( m_doc->ptToLayoutUnitPixY( dPoint.y() - theFrame->top() + theFrame->internalY() ) );
#ifdef DEBUG_DTI
            kdDebug() << "documentToInternal: returning LeftOfFrame " << iPoint.x() << "," << iPoint.y()
                      << " internalY=" << theFrame->internalY() << " because openLeftRect=" << openLeftRect
                      << " contains dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
            relPos = LeftOfFrame;
            return theFrame;
        }
        KoRect openTopRect( KoPoint( 0, 0 ), theFrame->innerRect().bottomRight() );
#ifdef DEBUG_DTI
        kdDebug() << "documentToInternal: openTopRect=" << openTopRect << endl;
#endif
        if ( openTopRect.contains( dPoint ) )
        {
            // We are at the top of this frame (...)
            iPoint.setX( m_doc->ptToLayoutUnitPixX( dPoint.x() - theFrame->innerRect().left() ) );
            iPoint.setY( m_doc->ptToLayoutUnitPixY( theFrame->internalY() ) );
#ifdef DEBUG_DTI
            kdDebug() << "documentToInternal: returning " << iPoint.x() << "," << iPoint.y()
                      << " internalY=" << theFrame->internalY() << " because openTopRect=" << openTopRect
                      << " contains dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
            relPos = TopOfFrame;
            return theFrame;
        }
    }
    // Not found. This means we are under (or at the right of), the frames in pageNum.
    // In that case, go for the first frame in the next page.
    if ( pageNum + 1 >= (int)m_framesInPage.size() + m_firstPage )
    {
        // Under last frame of last page!
        KWFrame *theFrame = frames.getLast();
        iPoint.setX( m_doc->ptToLayoutUnitPixX( theFrame->innerWidth() ) );
        iPoint.setY( m_doc->ptToLayoutUnitPixY( theFrame->innerHeight() ) );
#ifdef DEBUG_DTI
        kdDebug() << "documentToInternal: returning AtEnd " << iPoint.x() << "," << iPoint.y()
                  << " because we are under all frames of the last page" << endl;
#endif
        relPos = AtEnd;
        return theFrame;
    }
    else
    {
        QPtrListIterator<KWFrame> frameIt( framesInPage( pageNum + 1 ) );
        if ( frameIt.current() )
        {
            // There is a frame on the next page
            KWFrame *theFrame = frameIt.current();
            KoRect openTopRect( theFrame->innerRect() );
            openTopRect.setTop( 0 );
            if ( openTopRect.contains( dPoint ) ) // We are at the top of this frame
                iPoint.setX( m_doc->ptToLayoutUnitPixX( dPoint.x() - theFrame->left() ) );
            else
                iPoint.setX( 0 ); // We are, hmm, on the left or right of the frames
            iPoint.setY( m_doc->ptToLayoutUnitPixY( theFrame->internalY() ) );
#ifdef DEBUG_DTI
            kdDebug() << "documentToInternal: returning TopOfFrame " << iPoint.x() << "," << iPoint.y()
                      << " because we are under all frames of page " << pageNum << endl;
#endif
            relPos = TopOfFrame;
            return theFrame;
        } // else there is a gap (no frames on that page, but on some other further down)
        // This case isn't handled (and should be VERY rare I think)
    }

    iPoint = m_doc->ptToLayoutUnitPix( dPoint ); // bah
#ifdef DEBUG_DTI
    kdDebug() << "documentToInternal: returning not found for " << iPoint.x() << "," << iPoint.y() << endl;
#endif
    return 0;
}

KWFrame * KWTextFrameSet::internalToDocumentWithHint( const QPoint &iPoint, KoPoint &dPoint, const KoPoint &hintDPoint ) const
{
#ifdef DEBUG_ITD
    kdDebug() << "KWTextFrameSet::internalToDocumentWithHint hintDPoint: " << hintDPoint.x() << "," << hintDPoint.y() << endl;
#endif
    if ( !m_doc->viewMode()->hasFrames() ) { // text viewmode
        dPoint = m_doc->layoutUnitPtToPt( m_doc->pixelToPt( iPoint ) );
        return frames.getFirst();
    }
    KWFrame *lastFrame = 0L;
    QPtrListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *theFrame = frameIt.current();
        QRect r( 0, m_doc->ptToLayoutUnitPixY( theFrame->internalY() ), m_doc->ptToLayoutUnitPixX( theFrame->innerWidth() )+1, m_doc->ptToLayoutUnitPixY( theFrame->innerHeight() )+1 );
#ifdef DEBUG_ITD
        kdDebug() << "ITD: r=" << r << " iPoint=" << iPoint.x() << "," << iPoint.y() << endl;
#endif
        // r is the frame in qrt coords
        if ( r.contains( iPoint ) ) // both r and p are in layout units (aka internal)
        {
            dPoint = internalToDocumentKnowingFrame( iPoint, theFrame );
#ifdef DEBUG_ITD
            kdDebug() << "copy: " << theFrame->isCopy() << " hintDPoint.y()=" << hintDPoint.y() << " dPoint.y()=" << dPoint.y() << endl;
#endif
            // First test: No "hintDPoint" specified, go for the first match
            // Second test: hintDPoint specified, check if we are far enough
            if ( hintDPoint.isNull() || hintDPoint.y() <= dPoint.y() )
                return theFrame;
            // Remember that this frame matched, in case we find no further frame that matches
            lastFrame = theFrame;
        }
        else if ( lastFrame )
        {
            return lastFrame;
        }
    }

    // This happens when the parag is on a not-yet-created page (formatMore will notice afterwards)
    // So it doesn't matter much what happens here, we'll redo it anyway.
#ifdef DEBUG_ITD
    kdDebug(32002) << "KWTextFrameSet::internalToDocumentWithHint " << iPoint.x() << "," << iPoint.y()
                   << " not in any frame of " << (void*)this << endl;
#endif
    dPoint = m_doc->layoutUnitPtToPt( m_doc->pixelToPt( iPoint ) ); // bah
    return 0L;
}

// relPoint is in relative coordinates (pt)
KoPoint KWTextFrameSet::internalToDocumentKnowingFrame( const KoPoint &relPoint, KWFrame* theFrame ) const
{
    // It's ok to have theFrame == 0 in the text viewmode, but not in other modes
    if ( m_doc->viewMode()->hasFrames() )
        Q_ASSERT( theFrame );
    if ( theFrame )
        return KoPoint( relPoint.x() + theFrame->innerRect().x(),
                        relPoint.y() - theFrame->internalY() + theFrame->innerRect().y() );
    else
        return relPoint;
}

KoPoint KWTextFrameSet::internalToDocumentKnowingFrame( const QPoint &iPoint, KWFrame* theFrame ) const
{
    // Convert LU to relative coordinates (pt), then call the real internalToDocumentKnowingFrame().
    return internalToDocumentKnowingFrame( m_doc->layoutUnitPtToPt( m_doc->pixelToPt( iPoint ) ), theFrame );
}

QPoint KWTextFrameSet::moveToPage( int currentPgNum, short int direction ) const
{
    if ( !isVisible() || frames.isEmpty() )
        return QPoint();
    //kdDebug() << "KWTextFrameSet::moveToPage currentPgNum=" << currentPgNum << " direction=" << direction << endl;
    int num = currentPgNum + direction;
    int pages = m_doc->getPages();
    for ( ; num >= 0 && num < pages ; num += direction )
    {
        //kdDebug() << "KWTextFrameSet::moveToPage num=" << num << " pages=" << pages << endl;
        // Find the first frame on page num
        if ( num < m_firstPage || num >= (int)m_framesInPage.size() + m_firstPage )
            continue; // No frame on that page

        //kdDebug() << "KWTextFrameSet::moveToPage ok for first frame in page " << num << endl;
        QPtrListIterator<KWFrame> frameIt( framesInPage( num ) );
        return QPoint( 0, m_doc->ptToLayoutUnitPixY( frameIt.current()->internalY() ) + 2 ); // found, ok.
    }
    // Not found. Go to top of first frame or bottom of last frame, depending on direction
    if ( direction < 0 )
        return QPoint( 0, m_doc->ptToLayoutUnitPixY( frames.getFirst()->internalY() ) + 2 );
    else
    {
        KWFrame * theFrame = frames.getLast();
        return QPoint( 0, m_doc->ptToLayoutUnitPixY( theFrame->internalY() + theFrame->innerHeight() ) );
    }
}

void KWTextFrameSet::drawContents( QPainter *p, const QRect & crect, const QColorGroup &cg,
                                   bool onlyChanged, bool resetChanged,
                                   KWFrameSetEdit *edit, KWViewMode *viewMode )
{
    m_currentViewMode = viewMode;
    KWFrameSet::drawContents( p, crect, cg, onlyChanged, resetChanged, edit, viewMode );

    // Main textframeset: draw the footnote line if there are footnotes
    if ( isMainFrameset() && viewMode->hasFrames() )
    {
        // We stored the info "there's a footnote in this page" in the frame[s]
        // of the maintextframeset for that page. Usually one, but could be more
        // if there are columns. However we want to draw the line only once, so we
        // do it here and not in drawFrame (we'd have problems with cliprect anyway).
        if ( m_doc->footNoteSeparatorLineWidth() ==0.0)
            return;

        int pages = m_doc->getPages();
        double left = m_doc->ptLeftBorder();
        double factor = 100.0/m_doc->footNoteSeparatorLineLength();
        double pageWidth = m_doc->ptPaperWidth() - m_doc->ptRightBorder() - left ;
        double width = pageWidth / factor; // ### is 1/5 ok?
        int numColumns = m_doc->getColumns();
        for ( int pageNum = 0; pageNum < pages; pageNum++ )
        {
            //if ( viewMode->isPageVisible( pageNum ) )
            {
                uint frameNum = pageNum * numColumns /*+ col  0 here*/;
                Q_ASSERT ( frameNum < getNumFrames() );
                if ( frameNum < getNumFrames() )
                {
                    KWFrame* frame = this->frame( frameNum ); // ## or use framesInPage ?
                    //kdDebug() << " Footnote line: page " << pageNum << " found frame " << frameNum << " drawFootNoteLine=" << frame->drawFootNoteLine() << endl;
                    if ( frame->drawFootNoteLine() )
                    {
                        double y = frame->bottomLeft().y() + m_doc->headerFooterInfo().ptFootNoteBodySpacing / 2;
                        KoRect rect( left, y, width, 0 ); // this rect is flat
                        switch( m_doc->footNoteSeparatorLinePosition())
                        {
                        case SLP_LEFT:
                            break;
                        case SLP_CENTERED:
                            rect = KoRect( pageWidth/2.0+left-width/2.0, y,width,0);
                            break;
                        case SLP_RIGHT:
                            rect = KoRect( pageWidth+left-width, y,width,0);
                            break;
                        }

                        QRect flatRect = viewMode->normalToView( m_doc->zoomRect( rect ) );
                        //kdDebug() << " KWTextFrameSet::drawContents rect=" << rect << " zoomed:" << flatRect << endl;
                        flatRect.setBottom( flatRect.top() + 1 ); // #!@!@!& QRect....
                        if ( flatRect.intersects( crect ) ) {
                            p->save();
                            QPen pen( KoTextFormat::defaultTextColor( p ),   // always in default fg color (and black when printing)
                                      KoBorder::zoomWidthY( m_doc->footNoteSeparatorLineWidth(), m_doc, 1 ) ); // penwidth = zoomIt( 2 pt )
                            switch( m_doc->footNoteSeparatorLineType())
                            {
                            case SLT_SOLID:
                                pen.setStyle( SolidLine );
                                break;
                            case SLT_DASH:
                                pen.setStyle( DashLine );
                                break;
                            case SLT_DOT:
                                pen.setStyle( DotLine );
                                break;
                            case SLT_DASH_DOT:
                                pen.setStyle( DashDotLine );
                                break;
                            case SLT_DASH_DOT_DOT:
                                pen.setStyle( DashDotDotLine );
                                break;
                            }
                            p->setPen( pen );
                            p->drawLine( flatRect.left(), flatRect.top(), flatRect.right(), flatRect.top() );
                            //kdDebug() << "  drawLine( " << flatRect.left() << ", " << flatRect.top() << ", " << flatRect.right() << ", " << flatRect.top() << endl;
                            p->restore();
                        }
                    }
                } else
                    kdWarning(32001) << "Frameset:" << getName() << " frameNum=" << frameNum << " getNumFrames()=" << getNumFrames() << endl;
            }
        }
    }
}

void KWTextFrameSet::drawFrame( KWFrame *theFrame, QPainter *painter, const QRect &fcrect, const QRect &crect,
                                const QPoint& translationOffset,
                                KWFrame *settingsFrame, const QColorGroup &cg, bool onlyChanged, bool resetChanged,
                                KWFrameSetEdit *edit, KWViewMode *viewMode, bool drawUnderlyingFrames )
{
    // Detect if text frame needs transparency painting, to save time if it's using SolidPattern
    // In theory this code should be in kwFrameSet, but currently only text frames obey m_backgroundColor.
    if ( theFrame )
    {
        bool transparent = theFrame->backgroundColor().style() != Qt::SolidPattern;
        drawUnderlyingFrames &= transparent;
    }
    KWFrameSet::drawFrame( theFrame, painter, fcrect, crect, translationOffset, settingsFrame, cg, onlyChanged, resetChanged, edit, viewMode, drawUnderlyingFrames );
}

void KWTextFrameSet::drawFrameContents( KWFrame *theFrame, QPainter *painter, const QRect &r,
                                        const QColorGroup &cg, bool onlyChanged, bool resetChanged,
                                        KWFrameSetEdit *edit, KWViewMode *viewMode )
{
    // In this method the painter is translated to the frame's coordinate system
    // (in the first frame (0,0) will be its topleft, in the second frame it will be (0,internalY) etc.

    //kdDebug(32001) << "KWTextFrameSet::drawFrameContents " << getName() << "(frame " << frameFromPtr( theFrame ) << ") crect(r)=" << r << " onlyChanged=" << onlyChanged << endl;
    m_currentDrawnFrame = theFrame;
    if ( theFrame ) // 0L in the text viewmode
    {
        // Update variables for each frame (e.g. for page-number)
        // If more than KWPgNumVariable need this functionality, create an intermediary base class
        QPtrListIterator<KoTextCustomItem> cit( textDocument()->allCustomItems() );
        for ( ; cit.current() ; ++cit )
        {
            KWPgNumVariable * var = dynamic_cast<KWPgNumVariable *>( cit.current() );
            if ( var && !var->isDeleted() )
            {
                if ( var->subtype() == KWPgNumVariable::VST_PGNUM_CURRENT )
                {
                    //kdDebug() << "KWTextFrameSet::drawFrame updating pgnum variable to " << theFrame->pageNum()+1
                    //          << " and invalidating parag " << var->paragraph() << endl;
                    var->setPgNum( theFrame->pageNum()  + kWordDocument()->getVariableCollection()->variableSetting()->startingPage());
                } else if ( var->subtype() == KWPgNumVariable::VST_CURRENT_SECTION )
                {
                    var->setSectionTitle( kWordDocument()->sectionTitle( theFrame->pageNum() ) );
                }
                var->resize();
                var->paragraph()->invalidate( 0 ); // size may have changed -> need reformatting !
                var->paragraph()->setChanged( true );
            }
        }
    }

    // Do we draw a cursor ?
    bool drawCursor = edit!=0L;
    // Only if it will blink, i.e. not in a 'copy frame' that's not the current one
    if ( drawCursor && viewMode->hasFrames() && edit->currentFrame() != theFrame /*always true if currentFrame()==0 */ )
        drawCursor = false;
    KoTextCursor * cursor = edit ? (dynamic_cast<KWTextFrameSetEdit *>(edit) ? static_cast<KWTextFrameSetEdit *>(edit)->cursor() : 0) : 0;
    uint drawingFlags = 0;
    if ( viewMode->drawSelections() )
        drawingFlags |= KoTextDocument::DrawSelections;
    if ( m_doc->backgroundSpellCheckEnabled() )
        drawingFlags |= KoTextDocument::DrawMisspelledLine;
    if ( m_doc->viewFormattingChars() )
        drawingFlags |= KoTextDocument::DrawFormattingChars;

    //kdDebug(32001) << "KWTextFrameSet::drawFrame calling drawWYSIWYG. cg base color:" << cg.brush( QColorGroup::Base) << endl;
    KoTextParag * lastFormatted = textDocument()->drawWYSIWYG(
        painter, r.x(), r.y(), r.width(), r.height(),
        cg, kWordDocument(),
        onlyChanged, drawCursor, cursor, resetChanged, drawingFlags );

    // The last paragraph of this frame might have a bit in the next frame too.
    // In that case, and if we're only drawing changed paragraphs, (and resetting changed),
    // we have to set changed to true again, to draw the bottom of the parag in the next frame.
    if ( onlyChanged && resetChanged )
    {
        // Finding the "last parag of the frame" is a bit tricky.
        // It's usually the one before lastFormatted, except if it's actually lastParag :}  [see KoTextDocument::draw]
        KoTextParag * lastDrawn = lastFormatted->prev();
        if ( lastFormatted == textDocument()->lastParag() && ( !lastDrawn || m_doc->layoutUnitToPixelY( lastDrawn->rect().bottom() ) < r.bottom() ) )
            lastDrawn = lastFormatted;

        //kdDebug(32002) << "KWTextFrameSet::drawFrame drawn. onlyChanged=" << onlyChanged << " resetChanged=" << resetChanged << " lastDrawn=" << lastDrawn->paragId() << " lastDrawn's bottom:" << lastDrawn->rect().bottom() << " r.bottom=" << r.bottom() << endl;
        if ( lastDrawn && m_doc->layoutUnitToPixelY( lastDrawn->rect().bottom() ) > r.bottom() )
        {
            //kdDebug(32002) << "KWTextFrameSet::drawFrame setting lastDrawn " << lastDrawn->paragId() << " to changed" << endl;
            lastDrawn->setChanged( true );
        }
    }

    // NOTE: QTextView sets m_lastFormatted to lastFormatted here
    // But when scrolling up, this causes to reformat a lot of stuff for nothing.
    // And updateViewArea takes care of formatting things before we even arrive here.

    // Blank area under the very last paragraph - QRT draws it up to textdoc->height,
    // we have to draw it from there up to the bottom of the last frame.
    if ( ( !lastFormatted || lastFormatted == textDocument()->lastParag() ) )
    {
        // This is drawing code, so we convert everything to pixels
        int docHeight = textDocument()->lastParag()->pixelRect(m_doc).bottom() + 1;
        //QRect frameRect = m_doc->zoomRect( (theFrame->innerRect()) );

        QSize availSize = viewMode->availableSizeForText( this );
        QRect blank( 0, docHeight, availSize.width(), availSize.height() /*+ frameRect.height() ?? */ - docHeight );
        //kdDebug(32002) << this << " Blank area: " << blank << endl;
        painter->fillRect( blank, cg.brush( QColorGroup::Base ) );
        // for debugging :)
        //painter->setPen( QPen(Qt::blue, 1, DashLine) );  painter->drawRect( blank );
    }
    m_currentDrawnFrame = 0L;
}

void KWTextFrameSet::drawCursor( QPainter *p, KoTextCursor *cursor, bool cursorVisible, KWCanvas *canvas, KWFrame *theFrame )
{
    // This redraws the paragraph where the cursor is - with a small clip region around the cursor
    KWViewMode *viewMode = canvas->viewMode();
    bool hasFrames = viewMode->hasFrames();
    m_currentViewMode = viewMode;
    m_currentDrawnFrame = theFrame;

    QRect normalFrameRect;
    if (hasFrames)
        normalFrameRect = m_doc->zoomRect( theFrame->innerRect() );
    else
        normalFrameRect = QRect( QPoint(0, 0), viewMode->contentsSize() );

    QPoint topLeft = cursor->topParag()->rect().topLeft();         // in QRT coords
    int lineY;
    // Cursor height, in pixels
    int cursorHeight = m_doc->layoutUnitToPixelY( topLeft.y(), cursor->parag()->lineHeightOfChar( cursor->index(), 0, &lineY ) );
    QPoint iPoint( topLeft.x() - cursor->totalOffsetX() + cursor->x(),
                   topLeft.y() - cursor->totalOffsetY() + lineY );
#ifdef DEBUG_CURSOR
    kdDebug() << "KWTextFrameSet::drawCursor topLeft=" << topLeft.x() << "," << topLeft.y()
              << " x=" << cursor->x() << " y=" << lineY << endl;
    kdDebug() << "KWTextFrameSet::drawCursor iPoint=" << iPoint.x() << "," << iPoint.y()
              << "   cursorHeight=" << cursorHeight << endl;
#endif
    KoPoint dPoint;
    KoPoint hintDPoint = theFrame ? theFrame->innerRect().topLeft() : KoPoint();
    if ( internalToDocumentWithHint( iPoint, dPoint, hintDPoint ) )
    {
#ifdef DEBUG_CURSOR
        kdDebug() << " dPoint(doc, pts)=" << dPoint.x() << "," << dPoint.y() << endl;
        QPoint debugPt = m_doc->zoomPoint( dPoint );
        kdDebug() << " zoomed dPoint(doc, pixels)=" << debugPt.x() << "," << debugPt.y() << endl;
#endif
        QPoint vPoint = viewMode->normalToView( m_doc->zoomPoint( dPoint ) ); // from doc to view contents
#ifdef DEBUG_CURSOR
        kdDebug() << " vPoint(view, pixels)=" << vPoint.x() << "," << vPoint.y() << endl;
#endif
        // from now on, iPoint will be in pixels
        iPoint = m_doc->layoutUnitToPixel( iPoint );
        //int xadj = cursor->parag()->at( cursor->index() )->pixelxadj;
#ifdef DEBUG_CURSOR
        //kdDebug() << "     iPoint in pixels : " << iPoint.x() << "," << iPoint.y() << "     will add xadj=" << xadj << endl;
#endif
        //iPoint.rx() += xadj;
        //vPoint.rx() += xadj;
        // very small clipping around the cursor
        QRect clip( vPoint.x() - 5, vPoint.y(), 10, cursorHeight );

#ifdef DEBUG_CURSOR
        kdDebug() << " clip(view, before intersect)=" << clip << endl;
#endif

        QRect viewFrameRect = viewMode->normalToView( normalFrameRect );
        clip &= viewFrameRect; // clip to frame
#ifdef DEBUG_CURSOR
        kdDebug() << "KWTextFrameSet::drawCursor normalFrameRect=" << normalFrameRect
                  << " clip(view, after intersect)=" << clip << endl;
#endif

        QRegion reg;
        if ( hasFrames ) {
            reg = frameClipRegion( p, theFrame, clip, viewMode );
            if ( !isFloating() ) // problem with multiparent inline frames
                reg &= p->xForm( viewFrameRect );
        }

        if ( !hasFrames || !reg.isEmpty() )
        {
#ifdef DEBUG_CURSOR
            // for debug only!
            //p->fillRect( clip, QBrush( Qt::red, QBrush::Dense3Pattern ) );
#endif

            p->save();
            QColorGroup cg = QApplication::palette().active();

            if ( hasFrames )
            {
                p->setClipRegion( reg );
                // translate to qrt coords - after setting the clip region !
                // see internalToDocumentWithHint
                int translationX = viewFrameRect.left();
                int translationY = viewFrameRect.top() - m_doc->zoomItY( theFrame->internalY() );
#ifdef DEBUG_CURSOR
                kdDebug() << "        translating Y by viewFrameRect.top()-internalY in pixelY= " << viewFrameRect.top() << "-" << m_doc->zoomItY( theFrame->internalY() ) << "=" << viewFrameRect.top() - m_doc->zoomItY( theFrame->internalY() ) << endl;
#endif
                p->translate( translationX, translationY );
                p->setBrushOrigin( p->brushOrigin().x() + translationX, p->brushOrigin().y() + translationY );

                // The settings come from this frame
                KWFrame * settings = settingsFrame( theFrame );

                QBrush bgBrush( settings->backgroundColor() );
                bgBrush.setColor( KWDocument::resolveBgColor( bgBrush.color(), p ) );
                cg.setBrush( QColorGroup::Base, bgBrush );
            }

            QPixmap *pix = 0;
            bool wasChanged = cursor->parag()->hasChanged();
            uint drawingFlags = KoTextDocument::DrawSelections;
            if ( m_doc->backgroundSpellCheckEnabled() )
                drawingFlags |= KoTextDocument::DrawMisspelledLine;
            if ( m_doc->viewFormattingChars() )
                drawingFlags |= KoTextDocument::DrawFormattingChars;
            cursor->parag()->setChanged( TRUE );      // To force the drawing to happen
            textDocument()->drawParagWYSIWYG(
                p, static_cast<KoTextParag *>(cursor->parag()),
                iPoint.x() - 5, iPoint.y(), clip.width(), clip.height(),
                pix, cg, m_doc, // TODO view's zoom handler
                cursorVisible, cursor, FALSE /*resetChanged*/, drawingFlags );
            cursor->parag()->setChanged( wasChanged );      // Maybe we have more changes to draw!

            p->restore();

            //XIM Position
            QPoint ximPoint = vPoint;
            int line;
            cursor->parag()->lineStartOfChar( cursor->index(), 0, &line );
            canvas->setXimPosition( ximPoint.x(), ximPoint.y(),
                                    0, cursorHeight - cursor->parag()->lineSpacing( line ) );
        }
    }
    m_currentDrawnFrame = 0L;
}

void KWTextFrameSet::layout()
{
    invalidate();
    // Get the thing going though, repainting doesn't call formatMore
    m_textobj->formatMore();
}

void KWTextFrameSet::invalidate()
{
    //kdDebug() << "KWTextFrameSet::invalidate " << getName() << endl;
    m_textobj->setLastFormattedParag( textDocument()->firstParag() );
    textDocument()->invalidate(); // lazy layout, real update follows upon next repaint
}

void KWTextFrameSet::slotRepaintChanged()
{
    emit repaintChanged( this );
}

int KWTextFrameSet::paragraphs()
{
    int paragraphs = 0;
    KoTextParag * parag = textDocument()->firstParag();
    for ( ; parag ; parag = parag->next() )
        paragraphs++;
    return paragraphs;
}

int KWTextFrameSet::paragraphsSelected()
{
    int paragraphs = 0;
    KoTextParag *parag = textDocument()->firstParag();
    for ( ; parag ; parag = parag->next() ) {
        if ( parag->hasSelection( KoTextDocument::Standard ) )
            paragraphs++;
    }
    return paragraphs;
}

bool KWTextFrameSet::statistics( QProgressDialog *progress, ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words,
    ulong & sentences, ulong & syllables, bool selected )
{
    // parts of words for better counting of syllables:
    // (only use reg exp if necessary -> speed up)

    QStringList subs_syl;
    subs_syl << "cial" << "tia" << "cius" << "cious" << "giu" << "ion" << "iou";
    QStringList subs_syl_regexp;
    subs_syl_regexp << "sia$" << "ely$";

    QStringList add_syl;
    add_syl << "ia" << "riet" << "dien" << "iu" << "io" << "ii";
    QStringList add_syl_regexp;
    add_syl_regexp << "[aeiouym]bl$" << "[aeiou]{3}" << "^mc" << "ism$"
        << "[^l]lien" << "^coa[dglx]." << "[^gq]ua[^auieo]" << "dnt$";

    QString s;
    KoTextParag * parag = textDocument()->firstParag();
    for ( ; parag ; parag = parag->next() )
    {
        progress->setProgress(progress->progress()+1);
        // MA: resizing if KWStatisticsDialog does not work correct with this enabled, don't know why
        kapp->processEvents();
        if ( progress->wasCancelled() )
            return false;
        // start of a table
        if ( parag->at(0)->isCustom())
        {
            KoLinkVariable *var=dynamic_cast<KoLinkVariable *>(parag->at(0)->customItem());
            if(!var)
                continue;
        }

        bool hasTrailingSpace = true;
        if ( !selected ) {
            s = parag->string()->toString();
        } else {
            if ( parag->hasSelection( 0 ) ) {
                hasTrailingSpace = false;
                s = parag->string()->toString();
                if ( !( parag->fullSelected( 0 ) ) ) {
                    s = s.mid( parag->selectionStart( 0 ), parag->selectionEnd( 0 ) - parag->selectionStart( 0 ) );
                }
            } else {
                continue;
            }
        }

        // Character count
        for ( uint i = 0 ; i < s.length() - ( hasTrailingSpace ? 1 : 0 ) ; ++i )
        {
            QChar ch = s[i];
            ++charsWithSpace;
            if ( !ch.isSpace() )
                ++charsWithoutSpace;
        }

        // Syllable and Word count
        // Algorithm mostly taken from Greg Fast's Lingua::EN::Syllable module for Perl.
        // This guesses correct for 70-90% of English words, but the overall value
        // is quite good, as some words get a number that's too high and others get
        // one that's too low.
        // IMPORTANT: please test any changes against demos/statistics.kwd
        QRegExp re("\\s+");
        QStringList wordlist = QStringList::split(re, s);
        words += wordlist.count();
        re.setCaseSensitive(false);
        for ( QStringList::Iterator it = wordlist.begin(); it != wordlist.end(); ++it ) {
            QString word = *it;
            re.setPattern("[!?.,:_\"-]");    // clean word from punctuation
            word.replace(re, "");
            if ( word.length() <= 3 ) {  // extension to the original algorithm
                syllables++;
                continue;
            }
            re.setPattern("e$");
            word.replace(re, "");
            re.setPattern("[^aeiouy]+");
            QStringList syls = QStringList::split(re, word);
            int word_syllables = 0;
            for ( QStringList::Iterator it = subs_syl.begin(); it != subs_syl.end(); ++it ) {
                if( word.find(*it, 0, false) != -1 )
                    word_syllables--;
            }
            for ( QStringList::Iterator it = subs_syl_regexp.begin(); it != subs_syl_regexp.end(); ++it ) {
                re.setPattern(*it);
                if( word.find(re) != -1 )
                    word_syllables--;
            }
            for ( QStringList::Iterator it = add_syl.begin(); it != add_syl.end(); ++it ) {
                if( word.find(*it, 0, false) != -1 )
                    word_syllables++;
            }
            for ( QStringList::Iterator it = add_syl_regexp.begin(); it != add_syl_regexp.end(); ++it ) {
                re.setPattern(*it);
                if( word.find(re) != -1 )
                    word_syllables++;
            }
            word_syllables += syls.count();
            if ( word_syllables == 0 )
                word_syllables = 1;
            syllables += word_syllables;
        }
        re.setCaseSensitive(true);

        // Sentence count
        // Clean up for better result, destroys the original text but we only want to count
        s = s.stripWhiteSpace();
        QChar lastchar = s.at(s.length());
        if( ! s.isEmpty() && ! KoAutoFormat::isMark( lastchar ) ) {  // e.g. for headlines
            s = s + ".";
        }
        re.setPattern("[.?!]+");         // count "..." as only one "."
        s.replace(re, ".");
        re.setPattern("\\d\\.\\d");      // don't count floating point numbers as sentences
        s.replace(re, "0,0");
        re.setPattern("[A-Z]\\.+");      // don't count "U.S.A." as three sentences
        s.replace(re, "*");
        for ( uint i = 0 ; i < s.length() ; ++i )
        {
            QChar ch = s[i];
            if ( KoAutoFormat::isMark( ch ) )
                ++sentences;
        }
    }
    return true;
}

// Only interested in the body textframeset, not in header/footer
#define kdDebugBody(area) if ( frameSetInfo() == FI_BODY ) kdDebug(area)

// Helper for adjust*. There are 3 ways to use this method.
// marginLeft set -> determination of left margin for adjustLMargin
// marginRight set -> determination of right margin for adjustRMargin
// breakBegin, breakEnd, and paragLeftMargin set -> check whether we should jump over some frames
//                                                  [when there is not enough space besides them]
void KWTextFrameSet::getMargins( int yp, int h, int* marginLeft, int* marginRight,
                                 int* breakBegin, int* breakEnd, KoTextParag* parag )
{
    int paragLeftMargin = parag ? QMAX( parag->firstLineMargin(), parag->leftMargin() ) : 0;
#ifdef DEBUG_MARGINS
    kdDebugBody(32002) << "  KWTextFrameSet " << this << "(" << getName() << ") getMargins yp=" << yp
                       << " h=" << h << " called by "
                       << (marginLeft?"adjustLMargin":marginRight?"adjustRMargin":"formatVertically")
                       << " paragLeftMargin=" << paragLeftMargin
                       << endl;
    // Both or none...
    if (breakBegin) assert(breakEnd);
    if (breakEnd) assert(breakBegin);
#endif
    KoPoint pt;
    // The +h in here is a little hack, for the case where this line is going to
    // be moved down by formatVertically. We anticipate, and look at the bottom of the
    // line rather than the top of it, in order to find the bottom frame (the one
    // in which we'll end up). See TODO file for a real solution.
    KWFrame * theFrame = internalToDocument( QPoint(0, yp+h), pt );
    if (!theFrame)
    {
#ifdef DEBUG_MARGINS
        kdDebug(32002) << "  getMargins: internalToDocument returned frame=0L for yp+h=" << yp+h << " ->aborting with 0 margins" << endl;
#endif
        // frame == 0 happens when the parag is on a not-yet-created page (formatMore will notice afterwards)
        // Abort then, no need to return precise values
        if ( marginLeft )
            *marginLeft = 0;
        if ( marginRight )
            *marginRight = 0;
        return;
    }
#ifdef DEBUG_MARGINS
    else
        kdDebugBody(32002) << "  getMargins: internalToDocument returned frame=" << *theFrame
                           << " and pt=" << pt.x() << "," << pt.y() << endl;
#endif
    // Everything from there is in layout units
    // Note: it is very important that this method works in internal coordinates.
    // Otherwise, parags broken at the line-level (e.g. between two columns) are seen
    // as still in one piece, and we miss the frames in the 2nd column.
    int from = 0;
    int to = m_doc->ptToLayoutUnitPixX( theFrame->innerWidth() );
    bool init = false;

#ifdef DEBUG_MARGINS
    kdDebugBody(32002) << "  getMargins: looking for frames between " << yp << " and " << yp+h << " (internal coords)" << endl;
#endif
    // Principle: for every frame on top at this height, we'll move from and to
    // towards each other. The text flows between 'from' and 'to'
    QPtrListIterator<KWFrame> fIt( theFrame->framesOnTop() );
    for ( ; fIt.current() && from < to ; ++fIt )
    {
        if ( (*fIt)->runAround() == KWFrame::RA_BOUNDINGRECT )
        {
            KoRect rectOnTop = theFrame->intersect( (*fIt)->runAroundRect() );
#ifdef DEBUG_MARGINS
            kdDebugBody(32002) << "   getMargins found rect-on-top at (normal coords) " << rectOnTop << endl;
#endif
            QPoint iTop, iBottom; // top and bottom of intersection in internal coordinates
            if ( documentToInternal( rectOnTop.topLeft(), iTop ) &&
                 iTop.y() <= yp + h && // optimization
                 documentToInternal( rectOnTop.bottomRight(), iBottom ) )
            {
#ifdef DEBUG_MARGINS
                kdDebugBody(32002) << "      in internal coords: " << QRect(iTop,iBottom) << endl;
#endif
                // Look for intersection between p.y() -- p.y()+h  and iTop -- iBottom
                if ( QMAX( yp, iTop.y() ) <= QMIN( yp+h, iBottom.y() ) )
                {
#ifdef DEBUG_MARGINS
                    kdDebugBody(32002) << "   getMargins iTop=" << iTop.x() << "," << iTop.y()
                                       << " iBottom=" << iBottom.x() << "," << iBottom.y() << endl;
#endif
                    int availLeft = QMAX( 0, iTop.x() - from );
                    int availRight = QMAX( 0, to - iBottom.x() );
#ifdef DEBUG_MARGINS
                    kdDebugBody(32002) << "   getMargins availLeft=" << availLeft
                                       << " availRight=" << availRight << endl;
#endif
                    bool chooseLeft = false;
                    switch ( (*fIt)->runAroundSide() ) {
                    case KWFrame::RA_LEFT:
                        chooseLeft = true;
                        break;
                    case KWFrame::RA_RIGHT:
                        break; // chooseLeft remains false
                    case KWFrame::RA_BIGGEST:
                        chooseLeft = ( availLeft > availRight ); // choose the max
                    };

                    if ( chooseLeft )
                        // flow text at the left of the frame
                        to = QMIN( to, from + availLeft - 1 );  // can only go left -> QMIN
                    else
                        // flow text at the right of the frame
                        from = QMAX( from, to - availRight + 1 ); // can only go right -> QMAX

#ifdef DEBUG_MARGINS
                    kdDebugBody(32002) << "   getMargins from=" << from << " to=" << to << endl;
#endif
                    // If the available space is too small, give up on it
                    if ( to - from < m_doc->ptToLayoutUnitPixX( 15 ) + paragLeftMargin )
                    {
#ifdef DEBUG_MARGINS
                        kdDebugBody(32002) << "   smaller than minimum=" << m_doc->ptToLayoutUnitPixX( 15 ) + paragLeftMargin << endl;
#endif
                        from = to;
                    }

                    if ( breakEnd && from == to ) // no-space case
                    {
                        if ( !init ) // first time
                        {
                            init = true;
                            *breakBegin = iTop.y();
                            *breakEnd = iBottom.y();
                        }
                        else
                        {
                            *breakBegin = QMIN( *breakBegin, iTop.y() );
                            *breakEnd = QMAX( *breakEnd, iBottom.y() );
                        }
#ifdef DEBUG_MARGINS
                        kdDebugBody(32002) << "   getMargins iBottom.y=" << iBottom.y()
                                           << " breakBegin=" << *breakBegin
                                           << " breakEnd=" << *breakEnd << endl;
#endif
                    }
                } // else no intersection
            }// else we got a 0L, or the iTop.y()<=yp+h test didn't work - wrong debug output
            // kdDebugBody(32002) << "   gerMargins: normalToInternal returned 0L" << endl;
        }
    }
#ifdef DEBUG_MARGINS
    kdDebugBody(32002) << "   getMargins done. from=" << from << " to=" << to << endl;
#endif
    if ( from == to ) {
        from = 0;
        to = m_doc->ptToLayoutUnitPixX( theFrame->innerWidth() );
    }

    if ( marginLeft )
        *marginLeft = from;
    if ( marginRight )
    {
#ifdef DEBUG_MARGINS
        kdDebug(32002) << "    getMargins " << getName()
                       << " textdoc's width=" << textDocument()->width()
                       << " to=" << to << endl;
#endif
        *marginRight = textDocument()->width() - to;
    }
}

int KWTextFrameSet::adjustLMargin( int yp, int h, int margin, int space, KoTextParag* parag )
{
    int marginLeft = 0;
    if ( m_doc->viewMode()->shouldAdjustMargins() )
    {
        getMargins( yp, h, &marginLeft, 0L, 0L, 0L, parag );
#ifdef DEBUG_MARGINS
        kdDebugBody(32002) << "KWTextFrameSet::adjustLMargin marginLeft=" << marginLeft << endl;
#endif
    }
    return KoTextFlow::adjustLMargin( yp, h, margin + marginLeft, space, parag );
}

int KWTextFrameSet::adjustRMargin( int yp, int h, int margin, int space, KoTextParag* parag )
{
    int marginRight = 0;
    if ( m_doc->viewMode()->shouldAdjustMargins() )
    {
        getMargins( yp, h, 0L, &marginRight, 0L, 0L, parag );
#ifdef DEBUG_MARGINS
        kdDebugBody(32002) << "KWTextFrameSet::adjustRMargin marginRight=" << marginRight << endl;
#endif
    }
    return KoTextFlow::adjustRMargin( yp, h, margin + marginRight, space, parag );
}

// helper for formatVertically
bool KWTextFrameSet::checkVerticalBreak( int & yp, int & h, KoTextParag * parag, bool linesTogether, int breakBegin, int breakEnd )
{
    // We need the "+1" here because when skipping a frame on top, we want to be _under_
    // its bottom. Without the +1, we hit the frame again on the next adjustLMargin call.

    // Check for intersection between the parag (yp -- yp+h) and the break area (breakBegin -- breakEnd)
    if ( QMAX( yp, breakBegin ) <= QMIN( yp+h, breakEnd ) )
    {
        if ( !parag || linesTogether ) // Paragraph-level breaking
        {
#ifdef DEBUG_FORMATVERTICALLY
            kdDebug(32002) << "checkVerticalBreak ADJUSTING yp=" << yp << " h=" << h
                           << " breakEnd+2 [new value for yp]=" << breakEnd+2 << endl;
#endif
            yp = breakEnd + 1;
            return true;
        }
        else // Line-level breaking
        {
            QMap<int, KoTextParagLineStart*>& lineStarts = parag->lineStartList();
#ifdef DEBUG_FORMATVERTICALLY
            kdDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                           << ". lineStarts has " << lineStarts.count()
                           << " items" << endl;
#endif
            int dy = 0;
            int line = 0;
            QMap<int, KoTextParagLineStart*>::Iterator it = lineStarts.begin();
            for ( ; it != lineStarts.end() ; ++it, ++line )
            {
                KoTextParagLineStart * ls = it.data();
                Q_ASSERT( ls );
                int y = parag->rect().y() + ls->y;
#ifdef DEBUG_FORMATVERTICALLY
                kdDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                               << " line " << line << " ls->y=" << ls->y
                               << " ls->h=" << ls->h << " y=" << y
                               << " breakBegin=" << breakBegin
                               << " breakEnd=" << breakEnd << endl;
#endif
                if ( !dy )
                {
                    if ( QMAX( y, breakBegin ) <= QMIN( y + ls->h, breakEnd ) )
                    {
                        if ( line == 0 ) // First line ? It's like a paragraph breaking then
                        {
#ifdef DEBUG_FORMATVERTICALLY
                            kdDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                                           << " BREAKING first line -> parag break" << endl;
#endif
                            yp = breakEnd + 1;
                            return true;
                        }
                        dy = breakEnd + 1 - y;
                        ls->y = breakEnd + 1 - parag->rect().y();
#ifdef DEBUG_FORMATVERTICALLY
                        kdDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                                       << " BREAKING at line " << line << " dy=" << dy << "  Setting ls->y to " << ls->y << ", y=" << breakEnd << endl;
#endif
                    }
                }
                else
                {
                    ls->y += dy;
#ifdef DEBUG_FORMATVERTICALLY
                    if ( dy )
                        kdDebug(32002) << "                   moving down to position ls->y=" << ls->y << endl;
#endif
                }
            }
            parag->setMovedDown( true );
            parag->setHeight( h + dy );
#ifdef DEBUG_FORMATVERTICALLY
            kdDebug(32002) << "Paragraph height set to " << h+dy << endl;
#endif
            h += dy;
            return true;
        }
    }
    return false;
}

int KWTextFrameSet::formatVertically( KoTextParag * _parag )
{
    KWTextParag *parag = static_cast<KWTextParag *>( _parag );
    if ( !m_doc->viewMode()->shouldFormatVertically() )
    {
        fixParagWidth( parag );
        return 0;
    }

    QRect paragRect( _parag->rect() );
    int yp = paragRect.y();
    int hp = paragRect.height();
    int oldHeight = hp;
    int oldY = yp;

    // This is called since the 'vertical break' QRT flag is true.
    // End of frames/pages lead to those "vertical breaks".
    // What we do, is adjust the Y accordingly,
    // to implement page-break at the paragraph level and at the line level.
    // It's cumulative (the space of one break will be included in the further
    // paragraph's y position), which makes it easy to implement.
    // But don't forget that formatVertically is called twice for every parag, since the formatting
    // is re-done after moving down.

    bool linesTogether = parag ? parag->linesTogether() : false;
    bool hardFrameBreak = parag ? parag->hardFrameBreakBefore() : false;
    if ( !hardFrameBreak && parag && parag->prev() )
        hardFrameBreak = static_cast<KWTextParag *>(parag->prev())->hardFrameBreakAfter();

#ifdef DEBUG_FORMATVERTICALLY
    kdDebugBody(32002) << "KWTextFrameSet::formatVertically parag=" << parag
                       << " linesTogether=" << linesTogether << " hardFrameBreak=" << hardFrameBreak
                       << " yp=" << yp
                       << " hp=" << hp << endl;
#endif

    int totalHeight = 0;
    QPtrListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt )
    {
        int frameHeight = kWordDocument()->ptToLayoutUnitPixY( frameIt.current()->innerHeight() );
        int bottom = totalHeight + frameHeight;
        // Only skip bottom of frame if there is a next one or if there'll be another one created.
        // ( Not for header/footer, for instance. )
        bool check = frameIt.atLast() && frameIt.current()->frameBehavior() == KWFrame::AutoCreateNewFrame;
        if ( !check )
        {
            // ## TODO optimize this [maybe we should simply start from the end in the main loop?]
            // Or cache the attribute ( e.g. "frame->hasCopy()" ).
            QPtrListIterator<KWFrame> nextFrame( frameIt );
            while ( !check && !nextFrame.atLast() )
            {
                ++nextFrame;
                if ( !nextFrame.current()->isCopy() )
                    check = true; // Found a frame after us that isn't a copy => we have somewhere for our overflow
            }
        }

        if ( check )
        {
            if ( hardFrameBreak && yp > totalHeight && yp < bottom && !parag->wasMovedDown() )
            {
                // The paragraph wants a frame break before it, and is in the current frame
                // The last check is for whether we did the frame break already
                // (formatVertically is called twice for each paragraph, if a break was done)
                yp = bottom /*+ 2*/;
#ifdef DEBUG_FORMATVERTICALLY
                kdDebug(32002) << "KWTextFrameSet::formatVertically -> HARD FRAME BREAK" << endl;
                kdDebug(32002) << "KWTextFrameSet::formatVertically yp now " << yp << endl;
#endif
                break;
            }

#ifdef DEBUG_FORMATVERTICALLY
            kdDebug(32002) << "KWTextFrameSet::formatVertically frameHeight=" << frameHeight << " bottom=" << bottom << endl;
#endif
            // don't move down parags that have only one line and are bigger than the page (e.g. floating tables)
            if ( hp < frameHeight || ( parag && parag->lineStartList().count() > 1 ) )
            {
                // breakBegin==breakEnd==bottom, since the next frame's top is the same as bottom, in QRT coords.
                (void) checkVerticalBreak( yp, hp, parag, linesTogether, bottom, bottom );
                // Some people write a single paragraph over 3 frames! So we have to keep looking, that's why we ignore the return value
            }

        }
        if ( yp+hp < bottom )
            break; // we've been past the parag, so stop here
        totalHeight = bottom;
    }

    // Another case for a vertical break is frames with the RA_SKIP flag
    // Currently looking at all frames on top of all of our frames... maybe optimize better
    frameIt.toFirst();
    for ( ; frameIt.current(); ++frameIt )
    {
        QPtrListIterator<KWFrame> fIt( frameIt.current()->framesOnTop() );
        for ( ; fIt.current() ; ++fIt )
        {
            if ( (*fIt)->runAround() == KWFrame::RA_SKIP )
            {
                KoRect rectOnTop = frameIt.current()->intersect( (*fIt)->runAroundRect() );
                QPoint iTop, iBottom; // top and bottom in internal coordinates
                if ( documentToInternal( rectOnTop.topLeft(), iTop ) &&
                     iTop.y() <= yp + hp &&
                     documentToInternal( rectOnTop.bottomLeft(), iBottom ) &&
                     checkVerticalBreak( yp, hp, parag, linesTogether,
                                         iTop.y(), iBottom.y() ) )
                {
                    kdDebug(32002) << "KWTextFrameSet::formatVertically breaking around RA_SKIP frame yp="<<yp<<" hp=" << hp << endl;
                    // We don't "break;" here because there could be another such frame below the first one
                    // We assume that the frames on top are in order ( top to bottom ), btw.
                    // They should be, since updateFrames reorders before updating frames-on-top
                }
            }
        }
    }

    // And the last case for a vertical break is RA_BOUNDINGRECT frames that
    // leave no space by their side for any text (e.g. most tables)
    int breakBegin = 0;
    int breakEnd = 0;
    getMargins( yp, hp, 0L, 0L, &breakBegin, &breakEnd, parag );
    if ( breakEnd )
    {
        kdDebug(32002) << "KWTextFrameSet("<<getName()<<")::formatVertically no-space case. breakBegin=" << breakBegin
                       << " breakEnd=" << breakEnd << " hp=" << hp << endl;
        Q_ASSERT( breakBegin <= breakEnd );
        if ( checkVerticalBreak( yp, hp, parag, linesTogether, breakBegin, breakEnd ) )
            ; //kdDebug(32002) << "checkVerticalBreak ok." << endl;
        else // shouldn't happen
            kdWarning(32002) << "checkVerticalBreak didn't find it" << endl;
    }

    // ## TODO loop around those three methods until we don't move anymore ?

    // We also use verticalBreak as a hook into the formatting algo, to fix the parag width if necessary.
    fixParagWidth( parag );

    if ( hp != oldHeight )
        parag->setHeight( hp );
    if ( yp != oldY ) {
        QRect r = parag->rect();
        r.moveBy( 0, yp - oldY );
        parag->setRect( r );
        parag->setMovedDown( true );
    }
#ifdef DEBUG_FORMATVERTICALLY
    kdDebug() << "KWTextFrameSet::formatVertically returning " << ( yp + hp ) - ( oldY + oldHeight ) << endl;
#endif
    return ( yp + hp ) - ( oldY + oldHeight );
}

void KWTextFrameSet::fixParagWidth( KWTextParag* parag )
{
    // Fixing the parag rect for the formatting chars (CR and frame break).
    if ( parag && m_doc->viewFormattingChars() && parag->rect().width() < textDocument()->width() )
    {
        if ( parag->hardFrameBreakAfter() )
        {
            KoTextFormat * lastFormat = parag->at( parag->length() - 1 )->format();
            // keep in sync with KWTextFrameSet::formatVertically
            QString str = i18n( "--- Frame Break ---" );
            int width = 0;
            for ( int i = 0 ; i < (int)str.length() ; ++i )
                width += lastFormat->width( str, i );
            parag->setWidth( parag->rect().width() + width ); // TODO QMIN( textDocument()->width(), ... ) ?
        }
        else if ( parag->lineStartList().count() == 1 ) // don't use lines() here, parag not formatted yet
        {
            KoTextFormat * lastFormat = parag->at( parag->length() - 1 )->format();
            parag->setWidth( parag->rect().width() + lastFormat->width('x') );
        }
    }
}

KWTextFrameSet::~KWTextFrameSet()
{
    textDocument()->takeFlow();
    //kdDebug(32001) << "KWTextFrameSet::~KWTextFrameSet" << endl;
    m_doc = 0L;
    delete m_textobj;
}

// This struct is used for sorting frames.
// Since pages are one below the other, simply sorting on (y, x) does what we want.
struct FrameStruct
{
    KWFrame * frame;
    bool operator < ( const FrameStruct & t ) const {
        return compare(frame, t.frame) < 0;
    }
    bool operator <= ( const FrameStruct & t ) const {
        return compare(frame, t.frame) <= 0;
    }
    bool operator > ( const FrameStruct & t ) const {
        return compare(frame, t.frame) > 0;
    }

    /*
    the sorting of all frames in the same frameset is done as all sorting
    based on a simple frameOne > frameTwo question.
    Frame frameOne is greater then frameTwo if the center point lies more down then (the whole of)
    frame frameTwo. When they are equal, the X position is considered. */
    int compare (const KWFrame *frameOne, const KWFrame *frameTwo) const {
        // The first criteria is the page number though!
        int pageOne = frameOne->pageNum();
        int pageTwo = frameTwo->pageNum();
        if ( pageOne > pageTwo ) return 4; // frameOne > frameTwo
        if ( pageOne < pageTwo ) return -4; // frameOne < frameTwo

        double centerX = frameOne->left() + (frameOne->width() /2);
        // reverse the return values of the next two for RTL
        if ( centerX > frameTwo->right()) return 3; // frameOne > frameTwo
        if ( centerX < frameTwo->left()) return -3; // frameOne < frameTwo

        // check the Y position. Y is greater only when it is below the other frame.
        double centerY = frameOne->top() + (frameOne->height() /2);
        if ( centerY > frameTwo->bottom() ) return 2; //  frameOne > frameTwo
        if ( centerY < frameTwo->top() ) return -2; //  frameOne < frameTwo

        // the center of frameOne lies inside frameTwo. Lets check the topleft pos.
        if (frameOne->top() > frameTwo->top()) return 1;
        return -1;
    }
};

void KWTextFrameSet::updateFrames( int flags )
{
    // Not visible ? Don't bother then.
    if ( !isVisible() ) {
        m_textobj->setVisible(false);
        return;
    }
    m_textobj->setVisible(true);

    //kdDebug(32002) << "KWTextFrameSet::updateFrames " << getName() << " frame-count=" << frames.count() << endl;

    // Sort frames of this frameset on (y coord, x coord)
    // Adjustment on 20-Jun-2002 which does not change the itent of this but moves the
    // sorting from top-left of frame to the whole frame area. (TZ)

    QValueList<FrameStruct> sortedFrames;

    int width = 0;
    QPtrListIterator<KWFrame> frameIter( frameIterator() );
    for ( ; frameIter.current(); ++frameIter )
    {
        // Calculate max width while we're at it
        //kdDebug(32002) << "KWTextFrameSet::updateFrames frame innerWidth=" << frameIt.current()->innerWidth() << endl;
        width = QMAX( width, m_doc->ptToLayoutUnitPixX( frameIter.current()->innerWidth()));
        if ( flags & SortFrames )
        {
            FrameStruct str;
            str.frame = frameIter.current();
            sortedFrames.append( str );
        }
    }
    if ( width != textDocument()->width() )
    {
        //kdDebug(32002) << "KWTextFrameSet::updateFrames setWidth " << width << endl;
        textDocument()->setMinimumWidth( -1, 0 );
        textDocument()->setWidth( width + 1 ); // QRect semantics problem (#32866)
    } //else kdDebug(32002) << "KWTextFrameSet::updateFrames width already " << width << endl;

    if ( flags & SortFrames )
    {
        qHeapSort( sortedFrames );

        // Re-fill the frames list with the frames in the right order
        frames.setAutoDelete( false );
        frames.clear();

        QValueList<FrameStruct>::Iterator it = sortedFrames.begin();
        for ( ; it != sortedFrames.end() ; ++it )
            frames.append( (*it).frame );
    }

    double availHeight = 0;
    double internalYpt = 0;
    double lastRealFrameHeight = 0;
    bool firstFrame = true;

    QPtrListIterator<KWFrame> frameIt( frames );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame* theFrame = frameIt.current();

        if ( !theFrame->isCopy() )
            internalYpt += lastRealFrameHeight;

        theFrame->setInternalY( internalYpt );

        // Update availHeight with the internal height of this frame - unless it's a copy
        if ( !theFrame->isCopy() || firstFrame )
        {
            lastRealFrameHeight = theFrame->innerHeight();
            availHeight += lastRealFrameHeight;
        }
        firstFrame = false;
    }

    m_textobj->setAvailableHeight( m_doc->ptToLayoutUnitPixY( availHeight ) );
    //kdDebug(32002) << this << " (" << getName() << ") KWTextFrameSet::updateFrames availHeight=" << availHeight << " (LU: " << availableHeight() << ")" << endl;
    frames.setAutoDelete( true );

    KWFrameSet::updateFrames( flags );
}

int KWTextFrameSet::availableHeight() const
{
    return m_textobj->availableHeight();
}

KWFrame * KWTextFrameSet::internalToDocument( const KoPoint &relPoint, KoPoint &dPoint ) const
{
#ifdef DEBUG_ITD
    kdDebug() << getName() << " ITD called for relPoint=" << relPoint.x() << "," << relPoint.y() << endl;
#endif
    if ( !m_doc->viewMode()->hasFrames() ) { // text viewmode
        dPoint = relPoint;
        return frames.getFirst();
    }
    // This does a binary search in the m_framesInPage array, with internalY as criteria
    // We only look at the first frame of each page. Refining is done later on.
    Q_ASSERT( !m_framesInPage.isEmpty() );
    int len = m_framesInPage.count();
    int n1 = 0;
    int n2 = len - 1;
    double internalY = 0.0;
    int mid = 0;
    bool found = FALSE;
    while ( n1 <= n2 ) {
        double res;
        mid = (n1 + n2)/2;
#ifdef DEBUG_ITD
        kdDebug() << "ITD: begin. mid=" << mid << endl;
#endif
        Q_ASSERT( m_framesInPage[mid] ); // We have no null items
        if ( m_framesInPage[mid]->isEmpty() )
            res = -1;
        else
        {
            KWFrame * theFrame = m_framesInPage[mid]->first();
            internalY = theFrame->internalY();
#ifdef DEBUG_ITD
            kdDebug() << "ITD: relPoint.y=" << relPoint.y() << " internalY=" << internalY << endl;
#endif
            res = relPoint.y() - internalY;
#ifdef DEBUG_ITD
            kdDebug() << "ITD: res=" << res << endl;
#endif
            // Anything between this internalY (top) and internalY+height (bottom) is fine
            // (Using the next page's first frame's internalY only works if there is a frame on the next page)
            if ( res >= 0 )
            {
                double height = theFrame->innerHeight();
#ifdef DEBUG_ITD
                kdDebug() << "ITD: height=" << height << " -> the bottom is at " << internalY+height << endl;
#endif
                if ( relPoint.y() < internalY + height )
                {
#ifdef DEBUG_ITD
                    kdDebug() << "ITD: found a match " << mid << endl;
#endif
                    found = true;
                    break;
                }
            }
        }
        Q_ASSERT( res != 0 ); // this should have been already handled !
        if ( res < 0 )
            n2 = mid - 1;
        else // if ( res > 0 )
            n1 = mid + 1;
#ifdef DEBUG_ITD
        kdDebug() << "ITD: End of loop. n1=" << n1 << " n2=" << n2 << endl;
#endif
    }
    if ( !found )
    {
        // Not found (n2 < n1)
        // We might have missed the frame because n2 has many frames
        // (and we only looked at the first one).
        mid = n2;
#ifdef DEBUG_ITD
        kdDebug() << "ITD: Setting mid to n2=" << mid << endl;
#endif
        if ( mid < 0 )
        {
#ifdef DEBUG_ITD
            kdDebug(32002) << "KWTextFrameSet::internalToDocument " << relPoint.x() << "," << relPoint.y()
                           << " before any frame of " << (void*)this << endl;
#endif
            dPoint = relPoint; // "bah", I said above :)
            return 0L;
        }
    }
    // search to first of equal items
    // This happens with copied frames, which have the same internalY
    int result = mid;
    while ( mid - 1 >= 0 )
    {
        mid--;
        if ( !m_framesInPage[mid]->isEmpty() )
        {
            KWFrame * theFrame = m_framesInPage[mid]->first();
#ifdef DEBUG_ITD
            kdDebug() << "KWTextFrameSet::internalToDocument going back to page " << mid << " - frame: " << theFrame->internalY() << endl;
#endif
            if ( theFrame->internalY() == internalY ) // same internalY as the frame we found before
                result = mid;
            else
                break;
        }
    }

    // Now iterate over the frames in page 'result' and find the right one
    QPtrListIterator<KWFrame> frameIt( *m_framesInPage[result] );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *theFrame = frameIt.current();
        KoRect relRect( 0, theFrame->internalY(), theFrame->innerWidth(), theFrame->innerHeight() );
#ifdef DEBUG_ITD
        kdDebug() << "KWTextFrameSet::internalToDocument frame's relative rect:" << relRect << endl;
#endif
        if ( relRect.contains( relPoint ) ) // both relRect and relPoint are in "relative coordinates"
        {
            dPoint = internalToDocumentKnowingFrame( relPoint, theFrame );
            return theFrame;
        }
    }
#ifdef DEBUG_ITD
    kdDebug(32002) << "KWTextFrameSet::internalToDocument " << relPoint.x() << "," << relPoint.y()
                   << " not in any frame of " << (void*)this << " (looked on page " << result << ")" << endl;
#endif
    dPoint = relPoint; // bah again
    return 0L;
}

// same but with iPoint in LU
KWFrame * KWTextFrameSet::internalToDocument( const QPoint &iPoint, KoPoint &dPoint ) const
{
    KoPoint relPoint = m_doc->layoutUnitPtToPt( m_doc->pixelToPt( iPoint ) );
    return internalToDocument( relPoint, dPoint );
}

#ifndef NDEBUG
void KWTextFrameSet::printDebug()
{
    KWFrameSet::printDebug();
    if ( !isDeleted() )
    {
        kdDebug() << "KoTextDocument width = " << textDocument()->width() << " height = " << textDocument()->height() << endl;
        kdDebug() << " -- Frames in page array -- " << endl;
        for ( uint i = 0 ; i < m_framesInPage.size() ; ++i )
        {
            QPtrListIterator<KWFrame> it( *m_framesInPage[i] );
            int pgNum = i + m_firstPage;
            for ( ; it.current() ; ++it )
                kdDebug() << "  " << pgNum << ": " << it.current() << "   " << *it.current()
                          << " internalY=" << it.current()->internalY() << "pt "
                          << " (in LU:" << m_doc->ptToLayoutUnitPixY( it.current()->internalY() )
                          << " height=" << m_doc->ptToLayoutUnitPixX( it.current()->innerHeight() ) << ")" << endl;
        }
    }

    QPtrListIterator<KoTextCustomItem> cit( textDocument()->allCustomItems() );
    for ( ; cit.current() ; ++cit )
    {
      KWAnchor *anc = dynamic_cast<KWAnchor *>( cit.current() );
      if (anc)
          kdDebug() << "Inline framesets: " << anc->frameSet()->getName() << endl;
    }
}
#endif

QDomElement KWTextFrameSet::saveInternal( QDomElement &parentElem, bool saveFrames, bool saveAnchorsFramesets )
{
    if ( frames.isEmpty() ) // Deleted frameset -> don't save
        return QDomElement();
    unzoom();

    QDomElement framesetElem = parentElem.ownerDocument().createElement( "FRAMESET" );
    parentElem.appendChild( framesetElem );

    if ( grpMgr ) {
        framesetElem.setAttribute( "grpMgr", grpMgr->getName() );

        KWTableFrameSet::Cell *cell = (KWTableFrameSet::Cell *)this;
        framesetElem.setAttribute( "row", cell->m_row );
        framesetElem.setAttribute( "col", cell->m_col );
        framesetElem.setAttribute( "rows", cell->m_rows );
        framesetElem.setAttribute( "cols", cell->m_cols );
        framesetElem.setAttribute( "removable", static_cast<int>( m_removeableHeader ) );
    }
    if ( protectContent() )
        framesetElem.setAttribute( "protectContent", static_cast<int>(protectContent()));

    KWFrameSet::saveCommon( framesetElem, saveFrames );

    // Save paragraphs
    KWTextParag *start = static_cast<KWTextParag *>( textDocument()->firstParag() );
    while ( start ) {
        start->save( framesetElem, saveAnchorsFramesets );
        start = static_cast<KWTextParag *>( start->next() );
    }

    zoom( false );
    return framesetElem;
}

void KWTextFrameSet::load( QDomElement &attributes, bool loadFrames )
{
    KWFrameSet::load( attributes, loadFrames );
    if ( attributes.hasAttribute( "protectContent"))
        setProtectContent((bool)attributes.attribute( "protectContent" ).toInt());

    textDocument()->clear(false); // Get rid of dummy paragraph (and more if any)
    m_textobj->setLastFormattedParag( 0L ); // no more parags, avoid UMR in next setLastFormattedParag call
    KWTextParag *lastParagraph = 0L;

    // <PARAGRAPH>
    QDomElement paragraph = attributes.firstChild().toElement();
    for ( ; !paragraph.isNull() ; paragraph = paragraph.nextSibling().toElement() )
    {
        if ( paragraph.tagName() == "PARAGRAPH" )
        {
            KWTextParag *parag = new KWTextParag( textDocument(), lastParagraph );
            parag->load( paragraph );
            if ( !lastParagraph )        // First parag
                textDocument()->setFirstParag( parag );
            lastParagraph = parag;
            m_doc->progressItemLoaded();
        }
    }

    if ( !lastParagraph )                // We created no paragraph
    {
        // Create an empty one, then. See KWTextDocument ctor.
        textDocument()->clear( true );
        static_cast<KWTextParag *>( textDocument()->firstParag() )->setStyle( m_doc->styleCollection()->findStyle( "Standard" ) );
    }
    else
        textDocument()->setLastParag( lastParagraph );

    m_textobj->setLastFormattedParag( textDocument()->firstParag() );
    //kdDebug(32001) << "KWTextFrameSet::load done" << endl;
}

void KWTextFrameSet::zoom( bool forPrint )
{
    KWFrameSet::zoom( forPrint );
}

void KWTextFrameSet::unzoom()
{
}

void KWTextFrameSet::setVisible(bool visible)
{
    setInlineFramesVisible( visible );
    KWFrameSet::setVisible( visible );
}

void KWTextFrameSet::setInlineFramesVisible(bool visible)
{
    QPtrListIterator<KoTextCustomItem> cit( textDocument()->allCustomItems() );
    for ( ; cit.current() ; ++cit )
    {
      KWAnchor *anc = dynamic_cast<KWAnchor *>( cit.current() );
      if (anc)
            anc->frameSet()->setVisible( visible );
    }
}

#if 0
// Currently not used (since the WYSIWYG switch)
void KWTextFrameSet::preparePrinting( QPainter *painter, QProgressDialog *progress, int &processedParags )
{
    //textDocument()->doLayout( painter, textDocument()->width() );
    textDocument()->setWithoutDoubleBuffer( painter != 0 );

    textDocument()->formatCollection()->setPainter( painter );
    KoTextParag *parag = textDocument()->firstParag();
    while ( parag ) {
        parag->invalidate( 0 );
        parag->setPainter( painter, true );
        if ( painter )
            parag->format();
        parag = parag->next();
        if ( progress )
            progress->setProgress( ++processedParags );
    }
}
#endif

void KWTextFrameSet::addTextFrameSets( QPtrList<KWTextFrameSet> & lst, bool forceAllTextFrameSet )
{
    if (!textObject()->protectContent() || forceAllTextFrameSet)
        lst.append(this);
}

void KWTextFrameSet::slotNewCommand( KCommand *cmd )
{
    m_doc->addCommand( cmd );
}

void KWTextFrameSet::ensureFormatted( KoTextParag * parag, bool emitAfterFormatting )
{
    if (!isVisible())
        return;
    m_textobj->ensureFormatted( parag, emitAfterFormatting );
}

void KWTextFrameSet::slotAfterFormatting( int bottom, KoTextParag *lastFormatted, bool* abort )
{
    int availHeight = availableHeight();
    if ( ( bottom > availHeight ) ||   // this parag is already off page
         ( lastFormatted && bottom + lastFormatted->rect().height() > availHeight ) ) // or next parag will be off page
    {
#ifdef DEBUG_FORMAT_MORE
        if(lastFormatted)
            kdDebug(32002) << "slotAfterFormatting We need more space in " << getName()
                           << " bottom=" << bottom + lastFormatted->rect().height()
                           << " availHeight=" << availHeight << endl;
        else
            kdDebug(32002) << "slotAfterFormatting We need more space in " << getName()
                           << " bottom2=" << bottom << " availHeight=" << availHeight << endl;
#endif
        if ( frames.isEmpty() )
        {
            kdWarning(32002) << "slotAfterFormatting no more space, but no frame !" << endl;
            *abort = true;
            return;
        }

        double wantedPosition = 0;
        KWFrame::FrameBehavior frmBehavior = frames.last()->frameBehavior();
        if ( frmBehavior == KWFrame::AutoExtendFrame && isProtectSize())
            frmBehavior = KWFrame::Ignore;
        if (  frmBehavior ==  KWFrame::AutoCreateNewFrame )
        {
            KWFrame *theFrame = settingsFrame( frames.last() );
            double minHeight = minFrameHeight + theFrame->bTop() + theFrame->bBottom()+5;
            if ( availHeight < minHeight )
                frmBehavior = KWFrame::Ignore;
        }

        switch ( frmBehavior ) {
        case KWFrame::AutoExtendFrame:
        {
            int difference = ( bottom + 2 ) - availHeight; // in layout unit pixels
#ifdef DEBUG_FORMAT_MORE
            kdDebug(32002) << "AutoExtendFrame bottom=" << bottom << " availHeight=" << availHeight
                           << " => difference = " << difference << endl;
#endif
            if( lastFormatted && bottom + lastFormatted->rect().height() > availHeight ) {
#ifdef DEBUG_FORMAT_MORE
                kdDebug(32002) << " next will be off -> adding " << lastFormatted->rect().height() << endl;
#endif
                difference += lastFormatted->rect().height();
            }

            if(difference > 0) {
                // There's no point in resizing a copy, so go back to the last non-copy frame
                KWFrame *theFrame = settingsFrame( frames.last() );

                // Footers and footnotes go up
                if ( theFrame->frameSet()->isAFooter() || theFrame->frameSet()->isFootEndNote() )
                {
                    // The Y position doesn't matter much, recalcFrames will reposition the frame
                    // But the point of this code is set the correct height for the frame.
                    double maxFooterSize = footerHeaderSizeMax( theFrame );
                    double diffPt = m_doc->layoutUnitPtToPt( m_doc->pixelYToPt( difference ) );
                    wantedPosition = theFrame->top() - diffPt;
#ifdef DEBUG_FORMAT_MORE
                    kdDebug() << "  diffPt=" << diffPt << " -> wantedPosition=" << wantedPosition << endl;
#endif
                    if ( wantedPosition != theFrame->top() &&
                         ( theFrame->frameSet()->isFootEndNote() ||
                           theFrame->bottom() - maxFooterSize <= wantedPosition ) ) // Apply maxFooterSize for footers only
                    {
                        theFrame->setTop( wantedPosition);
#ifdef DEBUG_FORMAT_MORE
                        kdDebug() << "  ok: frame=" << *theFrame << " bottom=" << theFrame->bottom() << " height=" << theFrame->height() << endl;
#endif
                        frameResized( theFrame, true );
                        // We only got room for the next paragraph, we still have to keep the formatting going...
                        m_textobj->formatMore();
                        *abort = true;
                        return;
                    }
                    break;
                }

                wantedPosition = m_doc->layoutUnitPtToPt( m_doc->pixelYToPt( difference ) ) + theFrame->bottom();
                double pageBottom = (double) (theFrame->pageNum()+1) * m_doc->ptPaperHeight();
                pageBottom -= m_doc->ptBottomBorder();
                double newPosition = QMIN( wantedPosition, pageBottom );

                if ( theFrame->frameSet()->isAHeader() )
                {
                    double maxHeaderSize=footerHeaderSizeMax(  theFrame );
                    newPosition = QMIN( newPosition, maxHeaderSize+theFrame->top() );
                }

                newPosition = QMAX( newPosition, theFrame->top() ); // avoid negative heights
                bool resized = theFrame->bottom() != newPosition;

                if ( resized )
                {
                    if(theFrame->frameSet()->getGroupManager()) {
#ifdef DEBUG_FORMAT_MORE
                        kdDebug(32002) << "is table cell; just setting new minFrameHeight" << endl;
#endif
                        theFrame->setMinFrameHeight(newPosition - theFrame->top());
                    } else {
#ifdef DEBUG_FORMAT_MORE
                        kdDebug(32002) << "slotAfterFormatting setting bottom to " << newPosition << endl;
#endif
                        theFrame->setBottom(newPosition);
                    }
                }

                if(newPosition < wantedPosition && (theFrame->newFrameBehavior() == KWFrame::NoFollowup)) {
                    if ( resized )
                        frameResized( theFrame, false );
                    m_textobj->setLastFormattedParag( 0 );
                    break;
                }
                if(newPosition < wantedPosition && theFrame->newFrameBehavior() == KWFrame::Reconnect) {
                    wantedPosition = wantedPosition - newPosition + theFrame->top() + m_doc->ptPaperHeight();
                    // fall through to AutoCreateNewFrame
                } else {
                    if ( resized )
                    {
                        frameResized( theFrame, true );
                        abort = false;
                    }
                    break;
                }
            }
        }
        case KWFrame::AutoCreateNewFrame:
        {
            // We need a new frame in this frameset.
//#ifdef DEBUG_FORMAT_MORE
            kdDebug(32002) << "slotAfterFormatting creating new frame in frameset " << getName() << endl;
//#endif
            uint oldCount = frames.count();
            kdDebug() << " last frame=" << frames.last() << " pagenum=" << frames.last()->pageNum() << " getpages-1=" << m_doc->getPages()-1 << "   frames count=" << oldCount << endl;

            // First create a new page for it if necessary
            if ( frames.last()->pageNum() == m_doc->getPages() - 1 )
            {
                m_doc->appendPage();
                kdDebug() << "now frames count=" << frames.count() << endl;
            }

            // Maybe this created the frame, then we're done
            if ( frames.count() == oldCount )
            {
                Q_ASSERT( !isMainFrameset() ); // ouch, should have gone to the appendPage case above...
                // Otherwise, create a new frame on next page
                KWFrame *theFrame = frames.last();
                kdDebug() << "KWTextFrameSet::slotAfterFormatting creating frame on page " << theFrame->pageNum()+1 << endl;
                KWFrame *frm = theFrame->getCopy();
                frm->moveBy( 0, m_doc->ptPaperHeight() );
                //frm->setPageNum( theFrame->pageNum()+1 );
                addFrame( frm );
            }

            if (wantedPosition > 0)
                frames.last()->setBottom( wantedPosition );

            m_doc->updateAllFrames();
            /// We don't want to start from the beginning every time !
            ////m_doc->invalidate();

            if ( lastFormatted )
            {
                // Reformat the last paragraph. If it's over the two pages, it will need
                // the new page (e.g. for inline frames that need internalToDocument to work)
                if ( lastFormatted->prev() )
                {
                    m_textobj->setLastFormattedParag( lastFormatted->prev() );
                    lastFormatted->invalidate( 0 );
                }

                m_textobj->formatMore();
                *abort = true;
                return;
            }
	    m_doc->delayedRepaintAllViews();
        } break;
        case KWFrame::Ignore:
#ifdef DEBUG_FORMAT_MORE
            kdDebug(32002) << "slotAfterFormatting frame behaviour is Ignore" << endl;
#endif
            m_textobj->setLastFormattedParag( 0 );
            break;
        }
    }
    // Handle the case where the last frame is empty, so we may want to
    // remove the last page.
    else if ( frames.count() > 1 && !lastFormatted && frameSetInfo() == KWFrameSet::FI_BODY
              && bottom < availHeight - m_doc->ptToLayoutUnitPixY( frames.last()->innerHeight() ) )
    {
#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "slotAfterFormatting too much space (" << bottom << ", " << availHeight << ") , trying to remove last frame" << endl;
#endif
        int lastPage = m_doc->getPages() - 1;
        if(frames.last()->frameBehavior() == KWFrame::AutoExtendFrame) {
            delFrame(frames.last(), true);
            m_doc->frameChanged( 0L );
        }
        bool removed = false;
        // Last frame is empty -> try removing last page, and more if necessary
        while ( lastPage > 0 && m_doc->canRemovePage( lastPage ) )
        {
            m_doc->removePage( lastPage );
            if ( lastPage <= m_doc->getPages() - 1 )
            {
                kdWarning() << "Didn't manage to remove page " << lastPage << " (still having " << m_doc->getPages() << " pages ). Aborting" << endl;
                break;
            }
            removed = true;
            lastPage = m_doc->getPages()-1;
        }
        // Do all the recalc in one go. Speeds up deleting many pages.
        if ( removed )
            m_doc->afterRemovePages();
    }
    // Handle the case where the last frame is in AutoExtendFrame mode
    // and there is less text than space
    else if ( !lastFormatted && bottom + 2 < availHeight &&
              (frames.last()->frameBehavior() == KWFrame::AutoExtendFrame&& !isProtectSize()) )
    {
        // The + 2 here leaves 2 pixels below the last line. Without it we hit
        // the "break at end of frame" case in formatVertically (!!).
        int difference = availHeight - ( bottom + 2 );
#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "slotAfterFormatting less text than space (AutoExtendFrame). Frameset " << getName() << " availHeight=" << availHeight << " bottom=" << bottom << " ->difference=" << difference << endl;
#endif
        // There's no point in resizing a copy, so go back to the last non-copy frame
        KWFrame *theFrame = settingsFrame( frames.last() );
#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "frame is " << *theFrame << " footer:" << ( theFrame->frameSet()->isAFooter() || theFrame->frameSet()->isFootEndNote() ) << endl;
#endif
        if ( theFrame->frameSet()->isAFooter() || theFrame->frameSet()->isFootEndNote() )
        {
            double wantedPosition = theFrame->top() + m_doc->layoutUnitPtToPt( m_doc->pixelYToPt( difference ) );
            Q_ASSERT( wantedPosition < theFrame->bottom() );
            if ( wantedPosition != theFrame->top() )
            {
                kdDebug() << "top= " << theFrame->top() << " setTop " << wantedPosition << endl;
                theFrame->setTop( wantedPosition );
                kdDebug() << " -> the footer is now " << *theFrame << endl;
                frameResized( theFrame, true );
            }
        }
        else
        {
            double wantedPosition = theFrame->bottom() - m_doc->layoutUnitPtToPt( m_doc->pixelYToPt( difference ) );
#ifdef DEBUG_FORMAT_MORE
            kdDebug() << "slotAfterFormatting wantedPosition=" << wantedPosition << " top+minheight=" << theFrame->top() + minFrameHeight << endl;
#endif
            wantedPosition = QMAX( wantedPosition, theFrame->top() + minFrameHeight );
            if ( wantedPosition != theFrame->bottom()) {
                if(theFrame->frameSet()->getGroupManager() ) {
                    // When a frame can be smaller we don't rescale it if it is a table, since
                    // we don't have the full picture of the change.
                    // We will set the minFramHeight to the correct value and let the tables code
                    // do the rescaling based on all the frames in the row. (see KWTableFrameSet::recalcRows())
                    if(wantedPosition != theFrame->top() + theFrame->minFrameHeight()) {
#ifdef DEBUG_FORMAT_MORE
                        kdDebug(32002) << "is table cell; only setting new minFrameHeight, recalcrows will do the rest" << endl;
#endif
                        theFrame->setMinFrameHeight(wantedPosition - theFrame->top());
                        frameResized( theFrame, false );
                    }
                } else {
                    kdDebug() << "setBottom " << wantedPosition << endl;
                    theFrame->setBottom( wantedPosition );
                    frameResized( theFrame, true );
                }
            }
        }
    }
    if ( m_doc->processingType() == KWDocument::WP
         && this == m_doc->frameSet( 0 ) )
    {
        if ( m_lastTextDocHeight != textDocument()->height() )
        {
            m_lastTextDocHeight = textDocument()->height();
            emit mainTextHeightChanged();
        }
    }
}


double KWTextFrameSet::footNoteSize( KWFrame *theFrame )
{
    double tmp =0.0;
    int page = theFrame->pageNum();
    QPtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        if((fit.current()->isFootNote() || fit.current()->isEndNote()) &&
           fit.current()->isVisible())
        {
            KWFrame * frm=fit.current()->frame( 0 );
            if(frm->pageNum()==page )
                tmp += frm->innerHeight()+m_doc->ptFootnoteBodySpacing();
        }
    }
    return tmp;
}


double KWTextFrameSet::footerHeaderSizeMax( KWFrame *theFrame )
{
    double tmp =m_doc->ptPaperHeight()-m_doc->ptBottomBorder()-m_doc->ptTopBorder()-40;//default min 40 for page size
    int page = theFrame->pageNum();
    bool header=theFrame->frameSet()->isAHeader();
    if( header ? m_doc->isHeaderVisible():m_doc->isFooterVisible() )
    {
        QPtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
        for ( ; fit.current() ; ++fit )
        {
            bool state = header ? fit.current()->isAFooter():fit.current()->isAHeader();
            if(fit.current()->isVisible() && state)
            {
                KWFrame * frm=fit.current()->frame( 0 );
                if(frm->pageNum()==page )
                {
                    return (tmp-frm->innerHeight()-footNoteSize( theFrame ));
                }
            }
        }
    }
    if (theFrame->frameSet()->isHeaderOrFooter())
        return (tmp-footNoteSize( theFrame ));

    return tmp;
}

void KWTextFrameSet::frameResized( KWFrame *theFrame, bool invalidateLayout )
{
    kdDebug(32002) << "KWTextFrameSet::frameResized " << theFrame << " " << *theFrame << " invalidateLayout=" << invalidateLayout << endl;

    m_doc->updateAllFrames();

    if ( theFrame->frameSet()->frameSetInfo() != KWFrameSet::FI_BODY )
        m_doc->recalcFrames();

    KWTableFrameSet *table = theFrame->frameSet()->getGroupManager();
    if ( table )
    {
        KWTableFrameSet::Cell *cell = (KWTableFrameSet::Cell *)this;
        table->recalcCols(cell->m_col,cell->m_row);
        table->recalcRows(cell->m_col,cell->m_row);
    }

    // m_doc->frameChanged( theFrame );
    // Warning, can't call layout() (frameChanged calls it)
    // from here, since it calls formatMore() !
    if ( invalidateLayout )
        m_doc->invalidate(this);
    theFrame->updateRulerHandles();

    // Can't repaint directly, we might be in a paint event already
    m_doc->delayedRepaintAllViews();
}

bool KWTextFrameSet::isFrameEmpty( KWFrame * theFrame )
{
    KoTextParag * lastParag = textDocument()->lastParag();
    ensureFormatted( lastParag, false );
    int bottom = lastParag->rect().top() + lastParag->rect().height();

    if ( theFrame->frameSet() == this ) // safety check
    {
        //kdDebug() << "KWTextFrameSet::isFrameEmpty text bottom=(LU) " << bottom << " theFrame=" << theFrame << " " << *theFrame << " its internalY(LU)=" << m_doc->ptToLayoutUnitPixY( theFrame->internalY() ) << endl;
        return bottom < m_doc->ptToLayoutUnitPixY( theFrame->internalY() );
    }

    kdWarning() << "KWTextFrameSet::isFrameEmpty called for frame " << theFrame << " which isn't a child of ours!" << endl;
    if ( theFrame->frameSet()!=0L && theFrame->frameSet()->getName()!=0L)
        kdDebug() << "(this is " << getName() << " and the frame belongs to " << theFrame->frameSet()->getName() << ")" << endl;
    return false;
}

bool KWTextFrameSet::canRemovePage( int num )
{
    //kdDebug() << "KWTextFrameSet(" << getName() << ")::canRemovePage " << num << endl;

    // No frame on that page ? ok for us then
    if ( num < m_firstPage || num >= (int)m_framesInPage.size() + m_firstPage )
        return true;

    QPtrListIterator<KWFrame> frameIt( framesInPage( num ) );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame * theFrame = frameIt.current();
        //kdDebug() << "canRemovePage: looking at " << theFrame << endl;
        Q_ASSERT( theFrame->pageNum() == num );
        Q_ASSERT( theFrame->frameSet() == this );
        bool isEmpty = isFrameEmpty( theFrame );
        //kdDebug() << "KWTextFrameSet(" << getName() << ")::canRemovePage"
        //          << " found a frame on page " << num << " empty:" << isEmpty << endl;
        // Ok, so we have a frame on that page -> we can't remove it unless it's a copied frame OR it's empty
        bool isCopy = theFrame->isCopy() && frameIt.current() != frames.first();
        if ( !isCopy && !isEmpty )
            return false;
    }
    return true;
}

void KWTextFrameSet::delFrame( unsigned int num, bool remove, bool recalc )
{
    KWFrame *frm = frames.at( num );
    kdDebug() << "KWTextFrameSet(" << this << ")::delFrame " << frm << " (" << num << ")" << endl;
    if ( frm )
        emit frameDeleted( frm );
    KWFrameSet::delFrame( num, remove, recalc );
}

void KWTextFrameSet::updateViewArea( QWidget * w, KWViewMode* viewMode, const QPoint & nPointBottom )
{
    if (!isVisible(viewMode))
        return;
    int ah = availableHeight(); // make sure that it's not -1
#ifdef DEBUG_VIEWAREA
    kdDebug(32002) << "KWTextFrameSet::updateViewArea " << (void*)w << " " << w->name()
                     << " nPointBottom=" << nPointBottom.x() << "," << nPointBottom.y()
                     << " availHeight=" << ah << " textDocument()->height()=" << textDocument()->height() << endl;
#endif

    // Find last page that is visible
    int maxPage = ( nPointBottom.y() + m_doc->paperHeight() /*equiv. to ceil()*/ ) / m_doc->paperHeight();
    int maxY = 0;
    if ( maxPage < m_firstPage || maxPage >= (int)m_framesInPage.size() + m_firstPage )
        maxY = ah;
    else
    {
        // Find frames on that page, and keep the max bottom, in internal coordinates
        QPtrListIterator<KWFrame> frameIt( framesInPage( maxPage ) );
        for ( ; frameIt.current(); ++frameIt )
        {
            maxY = QMAX( maxY, m_doc->ptToLayoutUnitPixY( frameIt.current()->internalY() + frameIt.current()->innerHeight() ) );
        }
    }
#ifdef DEBUG_VIEWAREA
    kdDebug(32002) << "KWTextFrameSet (" << getName() << ")::updateViewArea maxY now " << maxY << endl;
#endif
    m_textobj->setViewArea( w, maxY );
    m_textobj->formatMore();
}

KCommand * KWTextFrameSet::setPageBreakingCommand( KoTextCursor * cursor, int pageBreaking )
{
    if ( !textDocument()->hasSelection( KoTextDocument::Standard ) &&
         static_cast<KWTextParag *>(cursor->parag())->pageBreaking() == pageBreaking )
        return 0L; // No change needed.

    m_textobj->emitHideCursor();

    m_textobj->storeParagUndoRedoInfo( cursor );

    if ( !textDocument()->hasSelection( KoTextDocument::Standard ) ) {
        KWTextParag *parag = static_cast<KWTextParag *>( cursor->parag() );
        parag->setPageBreaking( pageBreaking );
        m_textobj->setLastFormattedParag( cursor->parag() );
    }
    else
    {
        KoTextParag *start = textDocument()->selectionStart( KoTextDocument::Standard );
        KoTextParag *end = textDocument()->selectionEnd( KoTextDocument::Standard );
        m_textobj->setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setPageBreaking( pageBreaking );
    }

    m_textobj->formatMore();
    emit repaintChanged( this );
    KoTextObject::UndoRedoInfo & undoRedoInfo = m_textobj->undoRedoInfoStruct();
    undoRedoInfo.newParagLayout.pageBreaking = pageBreaking;
    KoTextParagCommand *cmd = new KoTextParagCommand(
        textDocument(), undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::PageBreaking );
    textDocument()->addCommand( cmd );
    undoRedoInfo.clear();
    m_textobj->emitShowCursor();
    m_textobj->emitUpdateUI( true );
    m_textobj->emitEnsureCursorVisible();
    // ## find a better name for the command
    return new KoTextCommand( m_textobj, /*cmd, */i18n("Change Paragraph Attribute") );
}

KCommand * KWTextFrameSet::pasteKWord( KoTextCursor * cursor, const QCString & data, bool removeSelected )
{
    if (protectContent() )
        return 0L;
    // Having data as a QCString instead of a QByteArray seems to fix the trailing 0 problem
    // I tried using QDomDocument::setContent( QByteArray ) but that leads to parse error at the end

    //kdDebug(32001) << "KWTextFrameSet::pasteKWord" << endl;
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Paste Text") );
    if ( removeSelected && textDocument()->hasSelection( KoTextDocument::Standard ) )
        macroCmd->addCommand( m_textobj->removeSelectedTextCommand( cursor, KoTextDocument::Standard ) );
    m_textobj->emitHideCursor();
    m_textobj->setLastFormattedParag( cursor->parag()->prev() ?
                           cursor->parag()->prev() : cursor->parag() );

    // We have our own command for this.
    // Using insert() wouldn't help storing the parag stuff for redo
    KWPasteTextCommand * cmd = new KWPasteTextCommand( textDocument(), cursor->parag()->paragId(), cursor->index(), data );
    textDocument()->addCommand( cmd );

    macroCmd->addCommand( new KoTextCommand( m_textobj, /*cmd, */QString::null ) );

    *cursor = *( cmd->execute( cursor ) );

    m_textobj->formatMore();
    emit repaintChanged( this );
    m_textobj->emitEnsureCursorVisible();
    m_textobj->emitUpdateUI( true );
    m_textobj->emitShowCursor();
    m_textobj->selectionChangedNotify();
    return macroCmd;
}

void KWTextFrameSet::insertTOC( KoTextCursor * cursor )
{
    m_textobj->emitHideCursor();
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Insert Table of Contents") );

    // Remove old TOC

    KoTextCursor *cur= KWInsertTOCCommand::removeTOC( this, cursor, macroCmd );

    // Insert new TOC

    KoTextDocCommand * cmd = new KWInsertTOCCommand( this,cur ? cur->parag(): cursor->parag() );
    textDocument()->addCommand( cmd );
    macroCmd->addCommand( new KoTextCommand( m_textobj, QString::null ) );
    *cursor = *( cmd->execute( cursor ) );

    m_textobj->setLastFormattedParag( textDocument()->firstParag() );
    m_textobj->formatMore();
    emit repaintChanged( this );
    m_textobj->emitEnsureCursorVisible();
    m_textobj->emitUpdateUI( true );
    m_textobj->emitShowCursor();

    m_doc->addCommand( macroCmd );
}

void KWTextFrameSet::insertFrameBreak( KoTextCursor *cursor )
{
    clearUndoRedoInfo();
    KMacroCommand* macroCmd = new KMacroCommand( i18n( "Insert Break After Paragraph" ) );
    macroCmd->addCommand( m_textobj->insertParagraphCommand( cursor ) );
    KWTextParag *parag = static_cast<KWTextParag *>( cursor->parag() );
    if(parag->prev()) {
        parag=static_cast<KWTextParag *> (parag->prev());
        cursor->setParag( parag );
        cursor->setIndex( parag->length() - 1 );
    }
    macroCmd->addCommand( setPageBreakingCommand( cursor, parag->pageBreaking() | KoParagLayout::HardFrameBreakAfter ) );
    m_doc->addCommand( macroCmd );

    m_textobj->setLastFormattedParag( parag );
    m_textobj->formatMore();
    emit repaintChanged( this );
    m_textobj->emitEnsureCursorVisible();
    m_textobj->emitUpdateUI( true );
    m_textobj->emitShowCursor();
}

QRect KWTextFrameSet::paragRect( KoTextParag * parag ) const
{
    // ## Warning. Imagine a paragraph cut in two pieces (at the line-level),
    // between two columns. A single rect in internal coords, but two rects in
    // normal coords. QRect( topLeft, bottomRight ) is just plain wrong.
    // Currently this method is only used for "ensure visible" so that's fine, but
    // we shouldn't use it for more precise stuff.
    KoPoint p;
    (void)internalToDocument( parag->rect().topLeft(), p );
    QPoint topLeft = m_doc->zoomPoint( p );
    (void)internalToDocument( parag->rect().bottomRight(), p );
    QPoint bottomRight = m_doc->zoomPoint( p );
    return QRect( topLeft, bottomRight );
}

void KWTextFrameSet::findPosition( const KoPoint &dPoint, KoTextParag * & parag, int & index )
{
    KoTextCursor cursor( textDocument() );

    QPoint iPoint;
    if ( documentToInternal( dPoint, iPoint ) )
    {
        cursor.place( iPoint, textDocument()->firstParag() );
        parag = cursor.parag();
        index = cursor.index();
    }
    else
    {
        // Not found, maybe under everything ?
        parag = textDocument()->lastParag();
        if ( parag )
            index = parag->length() - 1;
    }
}

bool KWTextFrameSet::minMaxInternalOnPage( int pageNum, int& topLU, int& bottomLU ) const
{
    QPtrListIterator<KWFrame> frameIt( framesInPage( pageNum ) );
    if ( !frameIt.current() )
        return false;

    // Look at all frames in the page, and keep min and max "internalY" positions
    double topPt = frameIt.current()->internalY();
    double bottomPt = topPt + frameIt.current()->height();

    for ( ; frameIt.current(); ++frameIt )
    {
        double y = frameIt.current()->internalY();
        topPt = QMIN( topPt, y );
        bottomPt = QMAX( bottomPt, y + frameIt.current()->height() );
    }
    // Convert to layout units
    topLU = m_doc->ptToLayoutUnitPixY( topPt );
    bottomLU = m_doc->ptToLayoutUnitPixY( bottomPt );
    return true;
}

KoTextParag* KWTextFrameSet::paragAtLUPos( int yLU ) const
{
    KoTextParag* parag = textDocument()->firstParag();
    for ( ; parag ; parag = parag->next() )
    {
        if ( parag->rect().bottom() >= yLU )
            return parag;
    }
    return 0L;
}

KCommand * KWTextFrameSet::deleteAnchoredFrame( KWAnchor * anchor )
{
    kdDebug() << "KWTextFrameSet::deleteAnchoredFrame anchor->index=" << anchor->index() << endl;
    Q_ASSERT( anchor );
    KoTextCursor c( textDocument() );
    c.setParag( anchor->paragraph() );
    c.setIndex( anchor->index() );

    textDocument()->setSelectionStart( KoTextDocument::Temp, &c );
    c.setIndex( anchor->index() + 1 );
    textDocument()->setSelectionEnd( KoTextDocument::Temp, &c );
    KCommand *cmd = m_textobj->removeSelectedTextCommand( &c, KoTextDocument::Temp );

    m_doc->repaintAllViews();
    return cmd;
}

bool KWTextFrameSet::hasSelection() const
{
    return m_textobj->hasSelection();
}

QString KWTextFrameSet::selectedText() const
{
    return m_textobj->selectedText();
}

void KWTextFrameSet::highlightPortion( KoTextParag * parag, int index, int length, KWCanvas * canvas, bool repaint )
{
    m_textobj->highlightPortion( parag, index, length, repaint );
    if ( repaint ) {
        QRect expose = canvas->viewMode()->normalToView( paragRect( parag ) );
        canvas->ensureVisible( (expose.left()+expose.right()) / 2,  // point = center of the rect
                               (expose.top()+expose.bottom()) / 2,
                               (expose.right()-expose.left()) / 2,  // margin = half-width of the rect
                               (expose.bottom()-expose.top()) / 2);
    }
}

void KWTextFrameSet::removeHighlight( bool repaint )
{
    m_textobj->removeHighlight( repaint );
}

void KWTextFrameSet::clearUndoRedoInfo()
{
    m_textobj->clearUndoRedoInfo();
}

void KWTextFrameSet::applyStyleChange( KoStyle * changedStyle, int paragLayoutChanged, int formatChanged )
{
    m_textobj->applyStyleChange( changedStyle, paragLayoutChanged, formatChanged );
}

void KWTextFrameSet::showPopup( KWFrame *, KWView *view, const QPoint &point )
{
    QPopupMenu * popup = view->popupMenu("text_popup");
    Q_ASSERT(popup);
    if (popup)
        popup->popup( point );
}

// KoTextFormatInterface methods
KoTextFormat *KWTextFrameSet::currentFormat() const
{
    return m_textobj->currentFormat();
}

KCommand *KWTextFrameSet::setChangeCaseOfTextCommand(KoChangeCaseDia::TypeOfCase _type)
{
    KoTextDocument *textdoc = m_textobj->textDocument();
    textdoc->selectAll( KoTextDocument::Standard );
    KoTextCursor *cursor = new KoTextCursor( textDocument() );
    KCommand* cmd = m_textobj->changeCaseOfText(cursor, _type);
    textdoc->removeSelection( KoTextDocument::Standard );
    delete cursor;
    return cmd;
}


KCommand *KWTextFrameSet::setFormatCommand( KoTextFormat * newFormat, int flags, bool zoomFont )
{
    m_textobj->textDocument()->selectAll( KoTextDocument::Temp );
    KCommand *cmd = m_textobj->setFormatCommand( 0L, 0L, newFormat, flags, zoomFont, KoTextDocument::Temp );
    m_textobj->textDocument()->removeSelection( KoTextDocument::Temp );
    return cmd;
}

const KoParagLayout * KWTextFrameSet::currentParagLayoutFormat() const
{
    return m_textobj->currentParagLayoutFormat();
}

KCommand *KWTextFrameSet::setParagLayoutFormatCommand( KoParagLayout *newLayout,int flags, int marginIndex)
{
    return m_textobj->setParagLayoutFormatCommand(newLayout, flags, marginIndex);
}

class KWFootNoteVarList : public QPtrList< KWFootNoteVariable >
{
protected:
    virtual int compareItems(QPtrCollection::Item a, QPtrCollection::Item b)
    {
        KWFootNoteVariable* vara = ((KWFootNoteVariable *)a);
        KWFootNoteVariable* varb = ((KWFootNoteVariable *)b);
        if ( vara->paragraph() == varb->paragraph() ) {
            // index() is a bit slow. But this is only called when there are
            // two footnotes in the same paragraph.
            int indexa = vara->index();
            int indexb = varb->index();
            return indexa < indexb ? -1 : indexa == indexb ? 0 : 1;
        }
        if ( vara->paragraph()->paragId() < varb->paragraph()->paragId() )
            return -1;
        return 1;
    }
};

void KWTextFrameSet::renumberFootNotes( bool repaint )
{
    KWFootNoteVarList lst;
    QPtrListIterator<KoTextCustomItem> cit( textDocument()->allCustomItems() );
    for ( ; cit.current() ; ++cit )
    {
        KWFootNoteVariable *fnv = dynamic_cast<KWFootNoteVariable *>( cit.current() );
        if (fnv && !fnv->isDeleted() && (fnv->frameSet() && !fnv->frameSet()->isDeleted()))
            lst.append( fnv );
    }
    lst.sort();
    short int varNumber = 0; // absolute order number [internal, not saved nor displayed]
    short int numDisplay = 1; // the number being displayed
    bool needRepaint = false;
    QPtrListIterator< KWFootNoteVariable > vit( lst );
    for ( ; vit.current() ; ++vit, ++varNumber )
    {
        ++varNumber;
        bool changed = false;
        KWFootNoteVariable* var = vit.current();
        if ( varNumber != var->num() )
        {
            changed = true;
            var->setNum( varNumber );
        }
        if ( var->numberingType()==KWFootNoteVariable::Auto )
        {
            if ( numDisplay != var->numDisplay() )
            {
                changed = true;
                var->setNumDisplay( numDisplay );
            }
            numDisplay++;
        }
        if ( changed )
        {
            if ( var->frameSet() ) //safety
            {
                if ( var->numberingType()== KWFootNoteVariable::Manual)
                    var->frameSet()->setName( m_doc->generateFramesetName(i18n("Footnote %1")));
                else
                    var->frameSet()->setName( i18n("Footnote %1").arg( var->text() ) );
                var->frameSet()->setCounterText( var->text() );
            }
            var->resize();
            var->paragraph()->invalidate(0);
            var->paragraph()->setChanged( true );
            needRepaint = true;
        }
    }
    if ( needRepaint && repaint )
        m_doc->slotRepaintChanged( this );
}

KoTextDocCommand *KWTextFrameSet::deleteTextCommand( KoTextDocument *textdoc, int id, int index, const QMemArray<KoTextStringChar> & str, const CustomItemsMap & customItemsMap, const QValueList<KoParagLayout> & oldParagLayouts )
{
    return new KWTextDeleteCommand( textdoc, id, index, str, customItemsMap, oldParagLayouts );
}



///////////////////////////////////////////////////////////////////////////////

KWTextFrameSetEdit::KWTextFrameSetEdit( KWTextFrameSet * fs, KWCanvas * canvas )
    : KoTextView( fs->textObject() ), KWFrameSetEdit( fs, canvas )
{
    kdDebug(32001) << "KWTextFrameSetEdit::KWTextFrameSetEdit " << fs->getName() << endl;
    KoTextView::setReadWrite( fs->kWordDocument()->isReadWrite() );
    KoTextObject* textobj = fs->textObject();
    connect( textobj, SIGNAL( selectionChanged(bool) ), canvas, SIGNAL( selectionChanged(bool) ) );
    connect( fs, SIGNAL( frameDeleted(KWFrame *) ), this, SLOT( slotFrameDeleted(KWFrame *) ) );
    connect( textView(), SIGNAL( cut() ), SLOT( cut() ) );
    connect( textView(), SIGNAL( copy() ), SLOT( copy() ) );
    connect( textView(), SIGNAL( paste() ), SLOT( paste() ) );
    updateUI( true, true );

    if( canvas->gui() && canvas->gui()->getHorzRuler())
    {
        if ( !textobj->protectContent() )
            canvas->gui()->getHorzRuler()->changeFlags(KoRuler::F_INDENTS | KoRuler::F_TABS);
        else
            canvas->gui()->getHorzRuler()->changeFlags(0);
    }

}

KWTextFrameSetEdit::~KWTextFrameSetEdit()
{
    //kdDebug(32001) << "KWTextFrameSetEdit::~KWTextFrameSetEdit" << endl;
    m_canvas->gui()->getHorzRuler()->changeFlags(0);
}

KoTextViewIface* KWTextFrameSetEdit::dcopObject()
{
    if ( !dcop )
        dcop = new KWordTextFrameSetEditIface( this );

    return dcop;
}

void KWTextFrameSetEdit::terminate(bool removeSelection)
{
    disconnect( textView()->textObject(), SIGNAL( selectionChanged(bool) ), m_canvas, SIGNAL( selectionChanged(bool) ) );
    textView()->terminate(removeSelection);
}

void KWTextFrameSetEdit::slotFrameDeleted( KWFrame *frm )
{
    if ( m_currentFrame == frm )
        m_currentFrame = 0L;
}

void KWTextFrameSetEdit::paste()
{
    QMimeSource *data = QApplication::clipboard()->data();
    // Hmm, we could reuse the result of KWView::checkClipboard...
    if ( data->provides( KWTextDrag::selectionMimeType() ) )
    {
        QByteArray arr = data->encodedData( KWTextDrag::selectionMimeType() );
        if ( arr.size() )
        {
            KCommand *cmd =textFrameSet()->pasteKWord( cursor(), QCString( arr ), true );
            if ( cmd )
                frameSet()->kWordDocument()->addCommand(cmd);
        }
    }
    else
    {
        // Note: QClipboard::text() seems to do a better job than encodedData( "text/plain" )
        // In particular it handles charsets (in the mimetype).
        QString text = QApplication::clipboard()->text();
        if ( !text.isEmpty() )
            textObject()->pasteText( cursor(), text, currentFormat(), true );
    }
}

void KWTextFrameSetEdit::cut()
{
    if ( textDocument()->hasSelection( KoTextDocument::Standard ) ) {
        copy();
        textObject()->removeSelectedText( cursor() );
    }
}

void KWTextFrameSetEdit::copy()
{
    if ( textDocument()->hasSelection( KoTextDocument::Standard ) ) {
        KWTextDrag *kd = newDrag( 0L );
        QApplication::clipboard()->setData( kd );
    }
}

bool KWTextFrameSetEdit::doIgnoreDoubleSpace(KoTextParag * parag,
        int index,QChar ch )
{
    if( textFrameSet()->kWordDocument()->allowAutoFormat())
    {
        KoAutoFormat * autoFormat = textFrameSet()->kWordDocument()->getAutoFormat();
        if(  autoFormat )
        {
            return autoFormat->doIgnoreDoubleSpace( parag, index,ch );
        }
    }
    return false;

}


void KWTextFrameSetEdit::doAutoFormat( KoTextCursor* cursor, KoTextParag *parag, int index, QChar ch )
{
    if( textFrameSet()->kWordDocument()->allowAutoFormat() )
    {
        KoAutoFormat * autoFormat = textFrameSet()->kWordDocument()->getAutoFormat();
        if( autoFormat )
            autoFormat->doAutoFormat( cursor, parag, index, ch, textObject());
    }
}

void KWTextFrameSetEdit::doCompletion( KoTextCursor* cursor, KoTextParag *parag, int index )
{
    if( textFrameSet()->kWordDocument()->allowAutoFormat() )
    {
        KoAutoFormat * autoFormat = textFrameSet()->kWordDocument()->getAutoFormat();
        if( autoFormat )
            autoFormat->doCompletion(  cursor, parag, index, textObject());
    }
}

void KWTextFrameSetEdit::startDrag()
{
    textView()->dragStarted();
    m_canvas->dragStarted();
    KWTextDrag *drag = newDrag( m_canvas->viewport() );
    if ( !frameSet()->kWordDocument()->isReadWrite() )
        drag->dragCopy();
    else {
        if ( drag->drag() && QDragObject::target() != m_canvas && QDragObject::target() != m_canvas->viewport() ) {
            //This is when dropping text _out_ of KWord. Since we have Move and Copy
            //options (Copy being accessed by pressing CTRL), both are possible.
            //But is that intuitive enough ? Doesn't the user expect a Copy in all cases ?
            //Losing the selection when dropping out of kword seems quite unexpected to me.
            //Undecided about this........
            textObject()->removeSelectedText( cursor() );
        }
    }
}

KWTextDrag * KWTextFrameSetEdit::newDrag( QWidget * parent ) const
{
    textFrameSet()->unzoom();
    KoTextCursor c1 = textDocument()->selectionStartCursor( KoTextDocument::Standard );
    KoTextCursor c2 = textDocument()->selectionEndCursor( KoTextDocument::Standard );

    QString text;
    QDomDocument domDoc( "PARAGRAPHS" );
    QDomElement elem = domDoc.createElement( "PARAGRAPHS" );
    domDoc.appendChild( elem );
    if ( c1.parag() == c2.parag() )
    {
        text = c1.parag()->toString( c1.index(), c2.index() - c1.index() );
        static_cast<KWTextParag *>(c1.parag())->save( elem, c1.index(), c2.index()-1, true );
    }
    else
    {
        text += c1.parag()->toString( c1.index() ) + "\n";
        static_cast<KWTextParag *>(c1.parag())->save( elem, c1.index(), c1.parag()->length()-2, true );
        KoTextParag *p = c1.parag()->next();
        while ( p && p != c2.parag() ) {
            text += p->toString() + "\n";
            static_cast<KWTextParag *>(p)->save( elem, 0, p->length()-2, true );
            p = p->next();
        }
        text += c2.parag()->toString( 0, c2.index() );
        static_cast<KWTextParag *>(c2.parag())->save( elem, 0, c2.index()-1, true );
    }
    textFrameSet()->zoom( false );

    KWTextDrag *kd = new KWTextDrag( parent );
    kd->setPlain( text );
    //kd->setFrameSetNumber( textFrameSet()->kWordDocument()->frameSetNum(textFrameSet()) );
    kd->setFrameSetNumber( textFrameSet()->kWordDocument()->numberOfTextFrameSet( textFrameSet(), true) );
    kd->setKWord( domDoc.toCString() );
    kdDebug(32001) << "KWTextFrameSetEdit::newDrag " << domDoc.toCString() << endl;
    return kd;
}

void KWTextFrameSetEdit::ensureCursorVisible()
{
    //kdDebug() << "KWTextFrameSetEdit::ensureCursorVisible paragId=" << cursor()->parag()->paragId() << endl;
    KoTextParag * parag = cursor()->parag();
    textFrameSet()->ensureFormatted( parag );
    KoTextStringChar *chr = parag->at( cursor()->index() );
    int h = parag->lineHeightOfChar( cursor()->index() );
    int x = parag->rect().x() + chr->x + cursor()->offsetX();
    //kdDebug() << "parag->rect().x()=" << parag->rect().x() << " chr->x=" << chr->x
    //          << " cursor()->offsetX()=" << cursor()->offsetX() << endl;
    int y = 0; int dummy;
    parag->lineHeightOfChar( cursor()->index(), &dummy, &y );
    y += parag->rect().y() + cursor()->offsetY();
    //kdDebug() << "KWTextFrameSetEdit::ensureCursorVisible y=" << y << endl;
    int w = 1;
    KoPoint pt;
    KoPoint hintDPoint;
    if ( m_currentFrame )
        hintDPoint = m_currentFrame->topLeft();
    KWFrame * theFrame = textFrameSet()->internalToDocumentWithHint( QPoint(x, y), pt, hintDPoint );
    //kdDebug() << "KWTextFrameSetEdit::ensureCursorVisible frame=" << theFrame << " m_currentFrame=" << m_currentFrame << endl;
    if ( theFrame && m_currentFrame != theFrame )
    {
        m_currentFrame = theFrame;
        m_canvas->gui()->getView()->updatePageInfo();
    }
    QPoint p = textFrameSet()->kWordDocument()->zoomPoint( pt );
    p = m_canvas->viewMode()->normalToView( p );
    w = textFrameSet()->kWordDocument()->layoutUnitToPixelX( w );
    h = textFrameSet()->kWordDocument()->layoutUnitToPixelY( h );
    //kdDebug() << "KWTextFrameSetEdit::ensureCursorVisible pt=" << pt << " p=" << p << " w=" << w << " y=" << y << endl;
    m_canvas->ensureVisible( p.x(), p.y() + h / 2, w, h / 2 + 2 );
}

bool KWTextFrameSetEdit::enterCustomItem( KoTextCustomItem* customItem, bool fromRight )
{
    KWAnchor* anchor = dynamic_cast<KWAnchor*>( customItem );
    if ( anchor ) {
        KWFrameSet* frameSet = anchor->frameSet();
        if ( frameSet->type() == FT_FORMULA ) {

            // store the instance variable we need after "delete this"
            KWCanvas* canvas = m_canvas;

            // this will "delete this"!
            m_canvas->editFrameSet( frameSet );

            // We assume that `editFrameSet' succeeded.
            if ( fromRight ) {
                KWFrameSetEdit* edit = canvas->currentFrameSetEdit();
                static_cast<KWFormulaFrameSetEdit*>( edit )->moveEnd();
            }

            // A FormulaFrameSetEdit looks a little different from
            // a FormulaFrameSet. (Colors)
            static_cast<KWFormulaFrameSet*>( frameSet )->setChanged();
            canvas->repaintChanged( frameSet, true );
            return true;
        }
    }
    return false;
}

void KWTextFrameSetEdit::keyPressEvent( QKeyEvent* e )
{
    // Handle moving into foreign frames (formula frames).
    if ( !( e->state() & ControlButton ) && !( e->state() & ShiftButton ) ) {
        switch ( e->key() ) {
        case Key_Left: {
            KoTextCursor* cursor = textView()->cursor();
            KoTextParag* parag = cursor->parag();
            int index = cursor->index();
            if ( index > 0 ) {
                KoTextStringChar* ch = parag->at( index-1 );
                if ( ch->isCustom() ) {
                    KoTextCustomItem* customItem = ch->customItem();
                    if ( enterCustomItem( customItem, true ) ) {
                        return;
                    }
                }
            }
            break;
        }
        case Key_Right: {
            KoTextCursor* cursor = textView()->cursor();
            KoTextParag* parag = cursor->parag();
            int index = cursor->index();
            if ( index < parag->length() ) {
                KoTextStringChar* ch = parag->at( index );
                if ( ch->isCustom() ) {
                    KoTextCustomItem* customItem = ch->customItem();
                    if ( enterCustomItem( customItem, false ) ) {
                        return;
                    }
                }
            }
            break;
        }
        }
    }
    textView()->handleKeyPressEvent( e );
}

void KWTextFrameSetEdit::keyReleaseEvent( QKeyEvent* e )
{
    textView()->handleKeyReleaseEvent( e );
}

void KWTextFrameSetEdit::mousePressEvent( QMouseEvent *e, const QPoint &, const KoPoint & dPoint )
{
    if ( dPoint.x() < 0 || dPoint.y() < 0 )
        return; // Ignore clicks completely outside of the page (e.g. in the gray area, or ruler)

    textFrameSet()->textObject()->clearUndoRedoInfo();
    if ( m_currentFrame )
        hideCursor(); // Need to do that with the old m_currentFrame

    QPoint iPoint;
    KWTextFrameSet::RelativePosition relPos;
    KWFrame * theFrame = textFrameSet()->documentToInternalMouseSelection( dPoint, iPoint, relPos );
    if ( theFrame && m_currentFrame != theFrame )
    {
        m_currentFrame = theFrame;
        m_canvas->gui()->getView()->updatePageInfo();
    }

    if ( m_currentFrame )
    {
        // Let KoTextView handle the mousepress event - but don't let it start
        // a drag if clicking on the left of the text (out of the frame itself)
        textView()->handleMousePressEvent( e, iPoint, relPos != KWTextFrameSet::LeftOfFrame );

        // Clicked on the left of the text -> select the whole paragraph
        if ( relPos == KWTextFrameSet::LeftOfFrame )
            textView()->selectParagUnderCursor( *textView()->cursor() );
    }
    // else mightStartDrag = FALSE; necessary?
}

void KWTextFrameSetEdit::mouseMoveEvent( QMouseEvent * e, const QPoint & nPoint, const KoPoint & )
{
    if ( textView()->maybeStartDrag( e ) )
        return;
    if ( nPoint.x() < 0 || nPoint.y() < 0 )
        return; // Ignore clicks completely outside of the page (e.g. in the gray area, or ruler)

    QPoint iPoint;
    KoPoint dPoint = frameSet()->kWordDocument()->unzoomPoint( nPoint );
    KWTextFrameSet::RelativePosition relPos;
    if ( nPoint.y() > 0 && textFrameSet()->documentToInternalMouseSelection( dPoint, iPoint, relPos ) )
    {
        if ( relPos == KWTextFrameSet::LeftOfFrame )
            textView()->extendParagraphSelection( iPoint );
        else
            textView()->handleMouseMoveEvent( e, iPoint );
    }
}

void KWTextFrameSetEdit::mouseReleaseEvent( QMouseEvent *, const QPoint &, const KoPoint & )
{
    textView()->handleMouseReleaseEvent();
}

void KWTextFrameSetEdit::mouseDoubleClickEvent( QMouseEvent *e, const QPoint &, const KoPoint & )
{
    textView()->handleMouseDoubleClickEvent( e, QPoint() /* Currently unused */ );
}

void KWTextFrameSetEdit::dragEnterEvent( QDragEnterEvent * e )
{
    if ( !frameSet()->kWordDocument()->isReadWrite() || !KWTextDrag::canDecode( e ) )
    {
        e->ignore();
        return;
    }
    e->acceptAction();
}

void KWTextFrameSetEdit::dragMoveEvent( QDragMoveEvent * e, const QPoint &nPoint, const KoPoint & )
{
    if ( !frameSet()->kWordDocument()->isReadWrite() || !KWTextDrag::canDecode( e ) )
    {
        e->ignore();
        return;
    }
    QPoint iPoint;
    KoPoint dPoint = frameSet()->kWordDocument()->unzoomPoint( nPoint );
    if ( textFrameSet()->documentToInternal( dPoint, iPoint ) )
    {
        textObject()->emitHideCursor();
        placeCursor( iPoint );
        textObject()->emitShowCursor();
        e->acceptAction(); // here or out of the if ?
    }
}

void KWTextFrameSetEdit::dragLeaveEvent( QDragLeaveEvent * )
{
}

void KWTextFrameSetEdit::dropEvent( QDropEvent * e, const QPoint & nPoint, const KoPoint & )
{
    if ( frameSet()->kWordDocument()->isReadWrite() && KWTextDrag::canDecode( e ) )
    {
        e->acceptAction();
        KoTextCursor dropCursor( textDocument() );
        QPoint dropPoint;
        KoPoint dPoint = frameSet()->kWordDocument()->unzoomPoint( nPoint );
        if ( !textFrameSet()->documentToInternal( dPoint, dropPoint ) )
            return; // Don't know where to paste

        KMacroCommand *macroCmd=new KMacroCommand(i18n("Paste Text"));
        bool createMacro = false;
        dropCursor.place( dropPoint, textDocument()->firstParag() );
        kdDebug(32001) << "KWTextFrameSetEdit::dropEvent dropCursor at parag=" << dropCursor.parag()->paragId() << " index=" << dropCursor.index() << endl;

        if ( ( e->source() == m_canvas ||
               e->source() == m_canvas->viewport() ) &&
             e->action() == QDropEvent::Move ) {
            int numberFrameSet=-1;
            numberFrameSet=KWTextDrag::decodeFrameSetNumber( e );
            //kdDebug()<<"decodeFrameSetNumber( QMimeSource *e ) :"<<numberFrameSet<<endl;;
            //KWFrameSet *frameset= frameSet()->kWordDocument()->frameSet( numberFrameSet );
            KWFrameSet *frameset= frameSet()->kWordDocument()->textFrameSetFromIndex(  numberFrameSet,true );
            KWTextFrameSet *tmp=dynamic_cast<KWTextFrameSet*>(frameset);
            tmp=tmp ? tmp:textFrameSet();
            if( tmp )
            {
                bool dropInSameObj= ( tmp == textFrameSet());
                KCommand *cmd=textView()->dropEvent(tmp->textObject(), dropCursor, dropInSameObj);
                if(cmd)
                {
                    macroCmd->addCommand(cmd);
                    createMacro = true;
                    //relayout textframeset after a dnd otherwise autoextend
                    //frameset is not re-layout
                    tmp->layout();
                    textFrameSet()->layout();
                }
                else
                {
                    delete macroCmd;
                    return;
                }
            }
        }
        else
        {   // drop coming from outside -> forget about current selection
            textDocument()->removeSelection( KoTextDocument::Standard );
            textObject()->selectionChangedNotify();
        }

        if ( e->provides( KWTextDrag::selectionMimeType() ) )
        {
            QByteArray arr = e->encodedData( KWTextDrag::selectionMimeType() );
            if ( arr.size() )
            {
                KCommand *cmd=textFrameSet()->pasteKWord( cursor(), QCString(arr), false );
                if ( cmd )
                {
                    macroCmd->addCommand(cmd);
                    createMacro = true;
                }
            }
        }
        else
        {
            QString text;
            if ( QTextDrag::decode( e, text ) )
                textObject()->pasteText( cursor(), text, currentFormat(), false );
        }
        if ( createMacro )
            frameSet()->kWordDocument()->addCommand(macroCmd);
        else
            delete macroCmd;
    }
}

void KWTextFrameSetEdit::focusInEvent()
{
    textView()->focusInEvent();
}

void KWTextFrameSetEdit::focusOutEvent()
{
    textView()->focusOutEvent();
}

void KWTextFrameSetEdit::selectAll()
{
    textObject()->selectAll( true );
}

void KWTextFrameSetEdit::drawCursor( bool visible )
{
#ifdef DEBUG_CURSOR
    kdDebug() << "KWTextFrameSetEdit::drawCursor " << visible << endl;
#endif
    KoTextView::drawCursor( visible );
    if ( !cursor()->parag() )
        return;

    if ( !cursor()->parag()->isValid() )
        textFrameSet()->ensureFormatted( cursor()->parag() );

    if ( !frameSet()->kWordDocument()->isReadWrite() )
        return;
    if ( m_canvas->viewMode()->hasFrames() && !m_currentFrame )
        return;

    QPainter p( m_canvas->viewport() );
    p.translate( -m_canvas->contentsX(), -m_canvas->contentsY() );
    p.setBrushOrigin( -m_canvas->contentsX(), -m_canvas->contentsY() );

    textFrameSet()->drawCursor( &p, cursor(), visible, m_canvas, m_currentFrame );
}

void KWTextFrameSetEdit::pgUpKeyPressed()
{
    QRect crect( m_canvas->contentsX(), m_canvas->contentsY(),
                 m_canvas->visibleWidth(), m_canvas->visibleHeight() );
    crect = m_canvas->viewMode()->viewToNormal( crect );

#if 0
    // One idea would be: move up until first-visible-paragraph wouldn't be visible anymore
    // First find the first-visible paragraph...
    KoTextParag *firstVis = m_cursor->parag();
    while ( firstVis && crect.intersects( s->rect() ) )
        firstVis = firstVis->prev();
    if ( !firstVis )
        firstVis = textFrameSet()->textDocument()->firstParag();
    else if ( firstVis->next() )
        firstVis = firstVis->next();
#endif
    // Go up of 90% of crect.height()
    int h = frameSet()->kWordDocument()->pixelToLayoutUnitY( (int)( (double)crect.height() * 0.9 ) );
    KoTextParag *s = textView()->cursor()->parag();
    int y = s->rect().y();
    while ( s ) {
        if ( y - s->rect().y() >= h )
            break;
        s = s->prev();
    }

    if ( !s )
        s = textDocument()->firstParag();

    textView()->cursor()->setParag( s );
    textView()->cursor()->setIndex( 0 );
}

void KWTextFrameSetEdit::pgDownKeyPressed()
{
    QRect crect( m_canvas->contentsX(), m_canvas->contentsY(),
                 m_canvas->visibleWidth(), m_canvas->visibleHeight() );
    crect = m_canvas->viewMode()->viewToNormal( crect );
    // Go down of 90% of crect.height()
    int h = frameSet()->kWordDocument()->pixelToLayoutUnitY( (int)( (double)crect.height() * 0.9 ) );

    KoTextCursor *cursor = textView()->cursor();
    KoTextParag *s = cursor->parag();
    int y = s->rect().y();
    while ( s ) {
        if ( s->rect().y() - y >= h )
            break;
        s = s->next();
    }

    if ( !s ) {
        s = textDocument()->lastParag();
        cursor->setParag( s );
        cursor->setIndex( s->length() - 1 );
    } else {
        cursor->setParag( s );
        cursor->setIndex( 0 );
    }
}

void KWTextFrameSetEdit::ctrlPgUpKeyPressed()
{
    if ( m_currentFrame )
    {
        QPoint iPoint = textFrameSet()->moveToPage( m_currentFrame->pageNum(), -1 );
        if ( !iPoint.isNull() )
            placeCursor( iPoint );
    }
}

void KWTextFrameSetEdit::ctrlPgDownKeyPressed()
{
    if ( m_currentFrame )
    {
        QPoint iPoint = textFrameSet()->moveToPage( m_currentFrame->pageNum(), +1 );
        if ( !iPoint.isNull() )
            placeCursor( iPoint );
    }
}

void KWTextFrameSetEdit::setCursor( KoTextParag* parag, int index )
{
    cursor()->setParag( parag );
    cursor()->setIndex( index );
}

void KWTextFrameSetEdit::insertExpression(const QString &_c)
{
    if(textObject()->hasSelection() )
        frameSet()->kWordDocument()->addCommand(textObject()->replaceSelectionCommand(
            cursor(), _c, KoTextDocument::Standard , i18n("Insert Expression")));
    else
       textObject()->insert( cursor(), currentFormat(), _c, false /* no newline */, true, i18n("Insert Expression") );
}

void KWTextFrameSetEdit::insertFloatingFrameSet( KWFrameSet * fs, const QString & commandName )
{
    textObject()->clearUndoRedoInfo();
    CustomItemsMap customItemsMap;
    QString placeHolders;
    // TODO support for multiple floating items (like multiple-page tables)
    int frameNumber = 0;
    int index = 0;
    bool ownline = false;
    { // the loop will start here :)
        KWAnchor * anchor = fs->createAnchor( textFrameSet()->textDocument(), frameNumber );
        if ( frameNumber == 0 && anchor->ownLine() && cursor()->index() > 0 ) // enforce start of line - currently unused
        {
            kdDebug() << "ownline -> prepending \\n" << endl;
            placeHolders += QChar('\n');
            index++;
            ownline = true;
        }
        placeHolders += KoTextObject::customItemChar();
        customItemsMap.insert( index, anchor );
    }
    fs->setAnchored( textFrameSet() );
    textObject()->insert( cursor(), currentFormat(), placeHolders,
                          ownline, false, commandName,
                          customItemsMap );
}

void KWTextFrameSetEdit::insertLink(const QString &_linkName, const QString & hrefName)
{
    KWDocument * doc = frameSet()->kWordDocument();
    KoVariable * var = new KoLinkVariable( textFrameSet()->textDocument(), _linkName, hrefName, doc->variableFormatCollection()->format( "STRING" ), doc->getVariableCollection() );
    insertVariable( var );
}

void KWTextFrameSetEdit::insertComment(const QString &_comment)
{
    KWDocument * doc = frameSet()->kWordDocument();
    KoVariable * var = new KoNoteVariable( textFrameSet()->textDocument(), _comment, doc->variableFormatCollection()->format( "STRING" ), doc->getVariableCollection() );
    insertVariable( var, 0,false/*don't delete selected text*/ );
}


void KWTextFrameSetEdit::insertCustomVariable( const QString &name)
{
     KWDocument * doc = frameSet()->kWordDocument();
     KoVariable * var = new KoCustomVariable( textFrameSet()->textDocument(), name, doc->variableFormatCollection()->format( "STRING" ), doc->getVariableCollection());
     insertVariable( var );
}

void KWTextFrameSetEdit::insertFootNote( NoteType noteType,KWFootNoteVariable::Numbering _numType, const QString &_manualString)
{
     kdDebug() << "KWTextFrameSetEdit::insertFootNote " << endl;
     KWDocument * doc = frameSet()->kWordDocument();
     KWFootNoteVariable * var = new KWFootNoteVariable( textFrameSet()->textDocument() , doc->variableFormatCollection()->format( "NUMBER" ), doc->getVariableCollection(), doc);
     var->setNoteType( noteType );
     var->setNumberingType(_numType );
     if ( _numType ==KWFootNoteVariable::Manual)
         var->setManualString( _manualString );

     // Now create text frameset which will hold the variable's contents
     KWFootNoteFrameSet *fs = new KWFootNoteFrameSet( doc, i18n( "Footnotes" ) );
     fs->setFrameSetInfo( KWFrameSet::FI_FOOTNOTE );

     // Place the frame on the correct page, but the exact coordinates
     // will be determined by recalcFrames (KWFrameLayout)
     int pageNum = m_currentFrame->pageNum();
     fs->createInitialFrame( pageNum );

     doc->addFrameSet( fs );

     // Bind the footnote variable and its text frameset
     var->setFrameSet( fs );
     fs->setFootNoteVariable( var );

     insertVariable( var );

     // Re-number footnote variables
     textFrameSet()->renumberFootNotes();

     // Layout the footnote frame
     doc->recalcFrames( pageNum, -1 ); // we know that for sure nothing changed before this page.

     KWCanvas* canvas = m_canvas;
     // And now edit the footnote frameset - all WPs do that it seems.
     m_canvas->editFrameSet( fs );

     // --- from here, we are deleted! ---
     // (This is why we must use canvas and not m_canvas)
     // Ensure cursor is visible
     KWTextFrameSetEdit *textedit=dynamic_cast<KWTextFrameSetEdit *>(canvas->currentFrameSetEdit()->currentTextEdit());
     if ( textedit )
         textedit->ensureCursorVisible();
}

void KWTextFrameSetEdit::insertVariable( int type, int subtype )
{
    kdDebug() << "KWTextFrameSetEdit::insertVariable " << type << endl;
    KWDocument * doc = frameSet()->kWordDocument();

    KoVariable * var = 0L;
    bool refreshCustomMenu = false;
    if ( type == VT_CUSTOM )
    {
        KoCustomVarDialog dia( m_canvas );
        if ( dia.exec() == QDialog::Accepted )
        {
            KoCustomVariable *v = new KoCustomVariable( textFrameSet()->textDocument(), dia.name(), doc->variableFormatCollection()->format( "STRING" ),doc->getVariableCollection() );
            v->setValue( dia.value() );
            var = v;
            refreshCustomMenu = true;
        }
    }
    else if ( type == VT_MAILMERGE )
    {
        KWMailMergeVariableInsertDia dia( m_canvas, doc->getMailMergeDataBase() );
        if ( dia.exec() == QDialog::Accepted )
        {
            var = new KWMailMergeVariable( textFrameSet()->textDocument(), dia.getName(), doc->variableFormatCollection()->format( "STRING" ),doc->getVariableCollection(),doc );
        }
    }
    else
        var = doc->getVariableCollection()->createVariable( type, subtype,  doc->variableFormatCollection(), 0L, textFrameSet()->textDocument(),doc);
    if ( var)
        insertVariable( var, 0L /*means currentFormat()*/, true, refreshCustomMenu);
}

void KWTextFrameSetEdit::insertVariable( KoVariable *var, KoTextFormat *format /*=0*/, bool removeSelectedText, bool refreshCustomMenu )
{
    if ( var )
    {
        CustomItemsMap customItemsMap;
        customItemsMap.insert( 0, var );
        if (!format)
            format = currentFormat();
        kdDebug() << "KWTextFrameSetEdit::insertVariable inserting into paragraph" << endl;
#ifdef DEBUG_FORMATS
        kdDebug() << "KWTextFrameSetEdit::insertVariable format=" << format << endl;
#endif
        textObject()->insert( cursor(), format, KoTextObject::customItemChar(),
                                false, removeSelectedText, i18n("Insert Variable"),
                                customItemsMap );
        var->recalc();
        cursor()->parag()->invalidate(0);
        cursor()->parag()->setChanged( true );
        frameSet()->kWordDocument()->slotRepaintChanged( frameSet() );
        if ( var->type()==VT_CUSTOM && refreshCustomMenu)
            frameSet()->kWordDocument()->refreshMenuCustomVariable();
    }
}

// Update the GUI toolbar button etc. to reflect the current cursor position.
void KWTextFrameSetEdit::updateUI( bool updateFormat, bool force )
{
    // Update UI - only for those items which have changed
    KoTextView::updateUI( updateFormat, force );

    // Paragraph settings
    KWTextParag * parag = static_cast<KWTextParag *>(cursor()->parag());

    if ( m_paragLayout.alignment != parag->resolveAlignment() || force ) {
        m_paragLayout.alignment = parag->resolveAlignment();
        m_canvas->gui()->getView()->showAlign( m_paragLayout.alignment );
    }

    // Counter
    if ( !m_paragLayout.counter )
        m_paragLayout.counter = new KoParagCounter; // we can afford to always have one here
    KoParagCounter::Style cstyle = m_paragLayout.counter->style();
    if ( parag->counter() )
        *m_paragLayout.counter = *parag->counter();
    else
    {
        m_paragLayout.counter->setNumbering( KoParagCounter::NUM_NONE );
        m_paragLayout.counter->setStyle( KoParagCounter::STYLE_NONE );
    }
    if ( m_paragLayout.counter->style() != cstyle || force )
        m_canvas->gui()->getView()->showCounter( * m_paragLayout.counter );

    if(m_paragLayout.leftBorder!=parag->leftBorder() ||
       m_paragLayout.rightBorder!=parag->rightBorder() ||
       m_paragLayout.topBorder!=parag->topBorder() ||
       m_paragLayout.bottomBorder!=parag->bottomBorder() || force )
    {
        m_paragLayout.leftBorder = parag->leftBorder();
        m_paragLayout.rightBorder = parag->rightBorder();
        m_paragLayout.topBorder = parag->topBorder();
        m_paragLayout.bottomBorder = parag->bottomBorder();
        m_canvas->gui()->getView()->showParagBorders( m_paragLayout.leftBorder, m_paragLayout.rightBorder, m_paragLayout.topBorder, m_paragLayout.bottomBorder );
    }

    if ( !parag->style() )
        kdWarning() << "Paragraph " << parag->paragId() << " has no style" << endl;
    else if ( m_paragLayout.style != parag->style() || force )
    {
        m_paragLayout.style = parag->style();
        m_canvas->gui()->getView()->showStyle( m_paragLayout.style->name() );
    }

    if( m_paragLayout.margins[QStyleSheetItem::MarginLeft] != parag->margin(QStyleSheetItem::MarginLeft)
        || m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] != parag->margin(QStyleSheetItem::MarginFirstLine)
        || m_paragLayout.margins[QStyleSheetItem::MarginRight] != parag->margin(QStyleSheetItem::MarginRight)
        || force )
    {
        m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] = parag->margin(QStyleSheetItem::MarginFirstLine);
        m_paragLayout.margins[QStyleSheetItem::MarginLeft] = parag->margin(QStyleSheetItem::MarginLeft);
        m_paragLayout.margins[QStyleSheetItem::MarginRight] = parag->margin(QStyleSheetItem::MarginRight);
        m_canvas->gui()->getView()->showRulerIndent( m_paragLayout.margins[QStyleSheetItem::MarginLeft], m_paragLayout.margins[QStyleSheetItem::MarginFirstLine], m_paragLayout.margins[QStyleSheetItem::MarginRight] );
    }

    if( m_paragLayout.tabList() != parag->tabList() )
    {
        m_paragLayout.setTabList( parag->tabList() );
        KoRuler * hr = m_canvas->gui()->getHorzRuler();
        if ( hr )
            hr->setTabList( parag->tabList() );
    }
    // There are more paragraph settings, but those that are not directly
    // visible in the UI don't need to be handled here.
    // For instance parag and line spacing stuff, borders etc.
}

void KWTextFrameSetEdit::showFormat( KoTextFormat *format )
{
    m_canvas->gui()->getView()->showFormat( *format );
}


void KWTextFrameSetEdit::showPopup( KWFrame * /*frame*/, KWView *view, const QPoint &point )
{
    // Decided against that - no other word processor does it that way apparently
#if 0
    // Place a KoTextCursor where we clicked
    KoTextCursor cursor( textDocument() );
    KoPoint dPoint = frameSet()->kWordDocument()->unzoomPoint( m_canvas->viewMode()->viewToNormal( point ) );
    QPoint iPoint;
    if ( textFrameSet()->documentToInternal( dPoint, iPoint, true ) )
        cursor.place( iPoint, textDocument()->firstParag() );
    else
        kdWarning() << "documentToInternal couldn't place cursor for RMB position! " << point.x() << "," << point.y() << endl;
    QString word = wordUnderCursor( cursor );
#endif
    QString word = wordUnderCursor( *cursor() );

    // Removed previous stuff
    view->unplugActionList( "datatools" );
    view->unplugActionList( "variable_action" );
    view->unplugActionList( "datatools_link" );

    // Those lists are stored in the KWView. Grab a ref to them.
    QPtrList<KAction> &actionList = view->dataToolActionList();
    QPtrList<KAction> &variableList = view->variableActionList();

    actionList.clear();
    variableList.clear();

    KWDocument * doc = frameSet()->kWordDocument();
    actionList = dataToolActionList(doc->instance(), word);

    doc->getVariableCollection()->setVariableSelected(variable());
    if ( variable() )
    {
        variableList = doc->getVariableCollection()->variableActionList();
    }

    if( variableList.count()>0)
    {
        view->plugActionList( "variable_action", variableList );
        QPopupMenu * popup = view->popupMenu("variable_popup");
        Q_ASSERT(popup);
        if (popup)
            popup->popup( point ); // using exec() here breaks the spellcheck tool (event loop pb)
    }
    else
    {
        kdDebug() << "KWView::openPopupMenuInsideFrame plugging actionlist with " << actionList.count() << " actions" << endl;
        if(refLink().isNull())
        {
            QPopupMenu * popup;
            view->plugActionList( "datatools", actionList );
            KoNoteVariable * var = dynamic_cast<KoNoteVariable *>(variable());
            KoCustomVariable * varCustom = dynamic_cast<KoCustomVariable *>(variable());
            KWFootNoteVariable * varFootNote = dynamic_cast<KWFootNoteVariable *>(variable());

            if( var )
                popup = view->popupMenu("comment_popup");
            else if( varCustom )
                popup = view->popupMenu("custom_var_popup");
            else if ( varFootNote )
            {
                view->changeFootNoteMenuItem( varFootNote->noteType() == FootNote );
                popup = view->popupMenu("footnote_popup");
            }
            else
                popup = view->popupMenu("text_popup");
            Q_ASSERT(popup);
            if (popup)
                popup->popup( point ); // using exec() here breaks the spellcheck tool (event loop pb)
        }
        else
        {
            view->plugActionList( "datatools_link", actionList );
            QPopupMenu * popup = view->popupMenu("text_popup_link");
            Q_ASSERT(popup);
            if (popup)
                popup->popup( point ); // using exec() here breaks the spellcheck tool (event loop pb)
        }
    }
}

//////

bool KWFootNoteFrameSet::isFootNote() const
{
    if ( !m_footNoteVar )
        return false;
    //Q_ASSERT( m_footNoteVar );
    return (m_footNoteVar->noteType()==FootNote );
}

bool KWFootNoteFrameSet::isEndNote() const
{
    if ( !m_footNoteVar )
        return false;
    //Q_ASSERT( m_footNoteVar );
    return (m_footNoteVar->noteType()==EndNote );
}



void KWFootNoteFrameSet::createInitialFrame( int pageNum )
{
    KWFrame *frame = new KWFrame(this, 0, pageNum * m_doc->ptPaperHeight() + 1, 20, 20 );
    frame->setFrameBehavior(KWFrame::AutoExtendFrame);
    frame->setNewFrameBehavior(KWFrame::NoFollowup);
    addFrame( frame );
}

void KWFootNoteFrameSet::setCounterText( const QString& text )
{
    KoTextParag* parag = textDocument()->firstParag();
    Q_ASSERT( parag );
    if ( parag ) {
        KoParagCounter counter;
        counter.setNumbering( KoParagCounter::NUM_FOOTNOTE );
        counter.setPrefix( text );
        counter.setSuffix( QString::null );
        parag->setCounter( counter );
    }
}

KWordFrameSetIface* KWFootNoteFrameSet::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KWFootNoteFrameSetIface( this );

    return m_dcop;
}


#include "kwtextframeset.moc"
