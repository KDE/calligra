/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001-2006 David Faure <faure@kde.org>

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

#include "KWTextFrameSet.h"
#include "KWTableFrameSet.h"
#include "KWDocument.h"
#include "KWView.h"
#include "KWViewMode.h"
#include "KWCanvas.h"
#include "KWAnchor.h"
#include "KWCommand.h"
#include "KWFormulaFrameSet.h"
#include "KWBgSpellCheck.h"
#include "KWordTextFrameSetIface.h"
#include "KWordTextFrameSetEditIface.h"
#include "KWordFootNoteFrameSetIface.h"
#include "KWordFrameSetIface.h"
#include "KWLoadingInfo.h"
#include "KWInsertTOCCommand.h"
#include "KWMailMergeDataBase.h"
#include "KoTextBookmark.h"
#include "KWVariable.h"
#include "KWOasisSaver.h"
#include "KWFrameList.h"
#include "KWPageManager.h"
#include "KWPage.h"

#include <KoParagCounter.h>
#include <KoCustomVariablesDia.h>
#include <KoAutoFormat.h>
#include <KoTextObject.h>
#include <KoTextCommand.h>
#include <KoTextFormatter.h>
#include <KoChangeCaseDia.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoOasisContext.h>
#include <KoStore.h>

#include <klocale.h>
#include <kaction.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include <qclipboard.h>
#include <q3dragobject.h>
#include <qcursor.h>
#include <qfile.h>
#include <q3progressdialog.h>
#include <qregexp.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <Q3MemArray>
#include <QKeyEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <Q3ValueList>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <Q3PtrList>
#include <QPixmap>

#include <assert.h>
#include <qapplication.h>
#include <q3tl.h>

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
        return m_textfs->formatVertically( parag, parag->rect() );
    }
    virtual void postFormat( KoTextParag* parag )
    {
        m_textfs->fixParagWidth( static_cast<KWTextParag *>( parag ) );
    }
private:
    KWTextFrameSet *m_textfs;
};

KWTextFrameSet::KWTextFrameSet( KWDocument *_doc, const QString & name )
    : KWFrameSet( _doc )
{
    //kDebug() << "KWTextFrameSet::KWTextFrameSet " << this << endl;
    if ( name.isEmpty() )
        m_name = _doc->generateFramesetName( i18n( "Text Frameset %1" ) );
    else
        m_name = name;

    QObject::setName( m_name.toUtf8() ); // store name in the QObject, for DCOP users
    init();
}

KWTextFrameSet::KWTextFrameSet( KWDocument* doc, const QDomElement& tag, KoOasisContext& /*context*/ )
    : KWFrameSet( doc )
{
    m_name = tag.attributeNS( KoXmlNS::draw, "name", QString::null );
    if ( doc->frameSetByName( m_name ) ) // already exists!
        m_name = doc->generateFramesetName( m_name + " %1" );
    init();
    // Note that we don't call loadOasis here. The caller wants to do it,
    // to get the frame it returns.
}

// protected constructor for testing purposes; does not do an init.
KWTextFrameSet::KWTextFrameSet( const QString &name ) : KWFrameSet(0) {
    m_name = name;

    QObject::setName( m_name.toUtf8() ); // store name in the QObject, for DCOP users
    m_currentViewMode = 0L;
    m_currentDrawnFrame = 0L;
    m_lastTextDocHeight = 0;
    m_textobj = 0;
}

void KWTextFrameSet::init()
{
    m_currentViewMode = 0L;
    m_currentDrawnFrame = 0L;
    m_lastTextDocHeight = 0;
    // Create the text document to set in the text object
    KWTextDocument* textdoc = new KWTextDocument( this,
        new KoTextFormatCollection( m_doc->defaultFont(), QColor(),
                                    m_doc->globalLanguage(),
                                    m_doc->globalHyphenation() ),
        new KWTextFormatter( this ) );
    textdoc->setFlow( this );
    textdoc->setPageBreakEnabled( true );              // get verticalBreak to be called
    if ( m_doc->tabStopValue() != -1 )
        textdoc->setTabStops( m_doc->ptToLayoutUnitPixX( m_doc->tabStopValue() ));

    m_textobj = new KoTextObject( textdoc, m_doc->styleCollection()->findStyle( "Standard" ),
                                  this, (m_name+"-textobj").toUtf8() );
    m_doc->backSpeller()->registerNewTextObject( m_textobj );
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
    Q_ASSERT( isVisible() );
    kDebug() << "KWTextFrameSet::slotAvailableHeightNeeded " << name() << endl;
    updateFrames( 0 ); // only do the available-height determination
}

KWFrame * KWTextFrameSet::documentToInternal( const KoPoint &dPoint, QPoint &iPoint ) const
{
#ifdef DEBUG_DTI
    kDebug() << "KWTextFrameSet::documentToInternal dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
    if ( !m_doc->layoutViewMode()->hasFrames() ) { // text viewmode
        iPoint = QPoint( m_doc->ptToLayoutUnitPixX( dPoint.x() ),
                         m_doc->ptToLayoutUnitPixY( dPoint.y() ) );
        return m_frames.getFirst();
    }
    // Find the frame that contains dPoint. To go fast, we look them up by page number.
    int pageNum = m_doc->pageManager()->pageNumber(dPoint);
    Q3PtrListIterator<KWFrame> frameIt( framesInPage( pageNum ) );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *theFrame = frameIt.current();
        if ( theFrame->contains( dPoint ) )
        {
            iPoint.setX( m_doc->ptToLayoutUnitPixX( dPoint.x() - theFrame->innerRect().x() ) );
            iPoint.setY( m_doc->ptToLayoutUnitPixY( dPoint.y() - theFrame->innerRect().y() + theFrame->internalY() ) );
#ifdef DEBUG_DTI
            kDebug() << "documentToInternal: returning " << iPoint.x() << "," << iPoint.y()
                      << " internalY=" << theFrame->internalY() << " because frame=" << theFrame
                      << " contains dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
            return theFrame;
        }
#ifdef DEBUG_DTI
        //else
        //  kDebug() << "DTI: " << frameRect
        //            << " doesn't contain nPoint:" << nPoint.x() << "," << nPoint.y() << endl;
#endif
    }
    // Not found. This means the mouse isn't over any frame, in the page pageNum.
    iPoint = m_doc->ptToLayoutUnitPix( dPoint ); // bah
    return 0;
}

KWFrame * KWTextFrameSet::documentToInternalMouseSelection( const KoPoint &dPoint, QPoint &iPoint, RelativePosition& relPos, KWViewMode *viewMode ) const
{
#ifdef DEBUG_DTI
    kDebug() << "KWTextFrameSet::documentToInternalMouseSelection dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
    if ( !viewMode->hasFrames() ) { // text viewmode
        relPos = InsideFrame;
        iPoint = QPoint( m_doc->ptToLayoutUnitPixX( dPoint.x() ),
                         m_doc->ptToLayoutUnitPixY( dPoint.y() ) );
        return m_frames.getFirst();
    }

    // Find the frame that contains dPoint. To go fast, we look them up by page number.
    int pageNum = m_doc->pageManager()->pageNumber(dPoint);
    Q3PtrListIterator<KWFrame> frameIt( framesInPage( pageNum ) );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *theFrame = frameIt.current();
        if ( theFrame->contains( dPoint ) )
        {
            iPoint.setX( m_doc->ptToLayoutUnitPixX( dPoint.x() - theFrame->innerRect().x() ) );
            iPoint.setY( m_doc->ptToLayoutUnitPixY( dPoint.y() - theFrame->innerRect().y() + theFrame->internalY() ) );
#ifdef DEBUG_DTI
            kDebug() << "documentToInternal: returning InsideFrame " << iPoint.x() << "," << iPoint.y()
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
        openLeftRect.setLeft( theFrame->paddingLeft() );
#ifdef DEBUG_DTI
        kDebug() << "documentToInternal: openLeftRect=" << openLeftRect << endl;
#endif
        if ( openLeftRect.contains( dPoint ) )
        {
            // We are at the left of this frame (and not in any other frame of this frameset)
            iPoint.setX( m_doc->ptToLayoutUnitPixX(theFrame->innerRect().left() ));
            iPoint.setY( m_doc->ptToLayoutUnitPixY( dPoint.y() - theFrame->top() + theFrame->internalY() ) );
#ifdef DEBUG_DTI
            kDebug() << "documentToInternal: returning LeftOfFrame " << iPoint.x() << "," << iPoint.y()
                      << " internalY=" << theFrame->internalY() << " because openLeftRect=" << openLeftRect
                      << " contains dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
            relPos = LeftOfFrame;
            return theFrame;
        }
        KoRect openTopRect( KoPoint( 0, 0 ), theFrame->innerRect().bottomRight() );
#ifdef DEBUG_DTI
        kDebug() << "documentToInternal: openTopRect=" << openTopRect << endl;
#endif
        if ( openTopRect.contains( dPoint ) )
        {
            // We are at the top of this frame (...)
            iPoint.setX( m_doc->ptToLayoutUnitPixX( dPoint.x() - theFrame->innerRect().left() ) );
            iPoint.setY( m_doc->ptToLayoutUnitPixY( theFrame->internalY() ) );
#ifdef DEBUG_DTI
            kDebug() << "documentToInternal: returning " << iPoint.x() << "," << iPoint.y()
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
        KWFrame *theFrame = m_frames.getLast();
        iPoint.setX( m_doc->ptToLayoutUnitPixX( theFrame->innerWidth() ) );
        iPoint.setY( m_doc->ptToLayoutUnitPixY( theFrame->innerHeight() ) );
#ifdef DEBUG_DTI
        kDebug() << "documentToInternal: returning AtEnd " << iPoint.x() << "," << iPoint.y()
                  << " because we are under all frames of the last page" << endl;
#endif
        relPos = AtEnd;
        return theFrame;
    }
    else
    {
        Q3PtrListIterator<KWFrame> frameIt( framesInPage( pageNum + 1 ) );
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
            kDebug() << "documentToInternal: returning TopOfFrame " << iPoint.x() << "," << iPoint.y()
                      << " because we are under all frames of page " << pageNum << endl;
#endif
            relPos = TopOfFrame;
            return theFrame;
        } // else there is a gap (no frames on that page, but on some other further down)
        // This case isn't handled (and should be VERY rare I think)
    }

    iPoint = m_doc->ptToLayoutUnitPix( dPoint ); // bah
#ifdef DEBUG_DTI
    kDebug() << "documentToInternal: returning not found for " << iPoint.x() << "," << iPoint.y() << endl;
#endif
    return 0;
}

KWFrame * KWTextFrameSet::internalToDocumentWithHint( const QPoint &iPoint, KoPoint &dPoint, const KoPoint &hintDPoint ) const
{
#ifdef DEBUG_ITD
    kDebug() << "KWTextFrameSet::internalToDocumentWithHint hintDPoint: " << hintDPoint.x() << "," << hintDPoint.y() << endl;
#endif
    if ( !m_doc->layoutViewMode()->hasFrames() ) { // text viewmode
        dPoint = m_doc->layoutUnitPtToPt( m_doc->pixelToPt( iPoint ) );
        return m_frames.getFirst();
    }
    KWFrame *lastFrame = 0L;
    Q3PtrListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *theFrame = frameIt.current();
        QRect r( 0, m_doc->ptToLayoutUnitPixY( theFrame->internalY() ), m_doc->ptToLayoutUnitPixX( theFrame->innerWidth() )+1, m_doc->ptToLayoutUnitPixY( theFrame->innerHeight() )+1 );
#ifdef DEBUG_ITD
        kDebug() << "ITD: r=" << r << " iPoint=" << iPoint.x() << "," << iPoint.y() << endl;
#endif
        // r is the frame in qrt coords
        if ( r.contains( iPoint ) ) // both r and p are in layout units (aka internal)
        {
            dPoint = internalToDocumentKnowingFrame( iPoint, theFrame );
#ifdef DEBUG_ITD
            kDebug() << "copy: " << theFrame->isCopy() << " hintDPoint.y()=" << hintDPoint.y() << " dPoint.y()=" << dPoint.y() << endl;
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
    kDebug(32002) << "KWTextFrameSet::internalToDocumentWithHint " << iPoint.x() << "," << iPoint.y()
                   << " not in any frame of " << (void*)this << endl;
#endif
    dPoint = m_doc->layoutUnitPtToPt( m_doc->pixelToPt( iPoint ) ); // bah
    return 0L;
}

// relPoint is in relative coordinates (pt)
KoPoint KWTextFrameSet::internalToDocumentKnowingFrame( const KoPoint &relPoint, KWFrame* theFrame ) const
{
    // It's ok to have theFrame == 0 in the text viewmode, but not in other modes
    if ( m_doc->layoutViewMode()->hasFrames() )
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
    if ( !isVisible() || m_frames.isEmpty() )
        return QPoint();
    //kDebug() << "KWTextFrameSet::moveToPage currentPgNum=" << currentPgNum << " direction=" << direction << endl;
    int num = currentPgNum + direction;
    int pages = m_doc->pageCount();
    for ( ; num >= 0 && num < pages ; num += direction )
    {
        //kDebug() << "KWTextFrameSet::moveToPage num=" << num << " pages=" << pages << endl;
        // Find the first frame on page num
        if ( num < m_firstPage || num >= (int)m_framesInPage.size() + m_firstPage )
            continue; // No frame on that page

        //kDebug() << "KWTextFrameSet::moveToPage ok for first frame in page " << num << endl;
        Q3PtrListIterator<KWFrame> frameIt( framesInPage( num ) );
        return QPoint( 0, m_doc->ptToLayoutUnitPixY( frameIt.current()->internalY() ) + 2 ); // found, ok.
    }
    // Not found. Go to top of first frame or bottom of last frame, depending on direction
    if ( direction < 0 )
        return QPoint( 0, m_doc->ptToLayoutUnitPixY( m_frames.getFirst()->internalY() ) + 2 );
    else
    {
        KWFrame * theFrame = m_frames.getLast();
        return QPoint( 0, m_doc->ptToLayoutUnitPixY( theFrame->internalY() + theFrame->innerHeight() ) );
    }
}

void KWTextFrameSet::drawContents( QPainter *p, const QRect & crect, const QColorGroup &cg,
                                   bool onlyChanged, bool resetChanged,
                                   KWFrameSetEdit *edit, KWViewMode *viewMode,
                                   KWFrameViewManager *fvm)
{
    m_currentViewMode = viewMode;
    KWFrameSet::drawContents( p, crect, cg, onlyChanged, resetChanged, edit, viewMode, fvm );

    // Main textframeset: draw the footnote line if there are footnotes
    if ( isMainFrameset() && viewMode->hasFrames() )
    {
        // We stored the info "there's a footnote in this page" in the frame[s]
        // of the maintextframeset for that page. Usually one, but could be more
        // if there are columns. However we want to draw the line only once, so we
        // do it here and not in drawFrame (we'd have problems with cliprect anyway).
        if ( m_doc->footNoteSeparatorLineWidth() ==0.0)
            return;

        int pages = m_doc->pageCount();
        KWPage *page = m_doc->pageManager()->page(m_doc->pageManager()->startPage());
        double left = page->leftMargin();
        double pageWidth = page->width() - page->rightMargin() - left;
        double width = pageWidth * m_doc->footNoteSeparatorLineLength() / 100.0;
        int numColumns = m_doc->numColumns();
        for ( int pageNum = 0; pageNum < pages; pageNum++ )
        {
            //if ( viewMode->isPageVisible( pageNum ) )
            {
                uint frameNum = pageNum * numColumns /*+ col  0 here*/;
                if ( frameNum < frameCount() ) // not true on the "endnotes-only" page
                {
                    KWFrame* frame = this->frame( frameNum ); // ## or use framesInPage ?
                    //kDebug() << " Footnote line: page " << pageNum << " found frame " << frameNum << " drawFootNoteLine=" << frame->drawFootNoteLine() << endl;
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
                        //kDebug() << " KWTextFrameSet::drawContents rect=" << rect << " zoomed:" << flatRect << endl;
                        flatRect.setBottom( flatRect.top() + 1 ); // #!@!@!& QRect....
                        if ( flatRect.intersects( crect ) ) {
                            p->save();
                            QPen pen( KoTextFormat::defaultTextColor( p ),   // always in default fg color (and black when printing)
                                      KoBorder::zoomWidthY( m_doc->footNoteSeparatorLineWidth(), m_doc, 1 ) ); // penwidth = zoomIt( 2 pt )
                            switch( m_doc->footNoteSeparatorLineType())
                            {
                            case SLT_SOLID:
                                pen.setStyle( Qt::SolidLine );
                                break;
                            case SLT_DASH:
                                pen.setStyle( Qt::DashLine );
                                break;
                            case SLT_DOT:
                                pen.setStyle( Qt::DotLine );
                                break;
                            case SLT_DASH_DOT:
                                pen.setStyle( Qt::DashDotLine );
                                break;
                            case SLT_DASH_DOT_DOT:
                                pen.setStyle( Qt::DashDotDotLine );
                                break;
                            }
                            p->setPen( pen );
                            p->drawLine( flatRect.left(), flatRect.top(), flatRect.right(), flatRect.top() );
                            //kDebug() << "  drawLine( " << flatRect.left() << ", " << flatRect.top() << ", " << flatRect.right() << ", " << flatRect.top() << endl;
                            p->restore();
                        }
                    }
                }
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
        drawUnderlyingFrames &= theFrame->isTransparent();
    }
    KWFrameSet::drawFrame( theFrame, painter, fcrect, crect, translationOffset, settingsFrame, cg, onlyChanged, resetChanged, edit, viewMode, drawUnderlyingFrames );
}

void KWTextFrameSet::drawFrameContents( KWFrame *theFrame, QPainter *painter, const QRect &r,
                                        const QColorGroup &cg, bool onlyChanged, bool resetChanged,
                                        KWFrameSetEdit *edit, KWViewMode *viewMode )
{
    Q_ASSERT( r.isValid() );
    // In this method the painter is translated to the frame's coordinate system
    // (in the first frame (0,0) will be its topleft, in the second frame it will be (0,internalY) etc.

    //kDebug(32001) << "KWTextFrameSet::drawFrameContents " << name() << "(frame " << frameFromPtr( theFrame ) << ") crect(r)=" << r << " onlyChanged=" << onlyChanged << endl;
    m_currentDrawnFrame = theFrame;
    if ( theFrame ) // 0L in the text viewmode
    {
        // Update variables for each frame (e.g. for page-number)
        // If more than KWPgNumVariable need this functionality, create an intermediary base class
        Q3PtrListIterator<KoTextCustomItem> cit( textDocument()->allCustomItems() );
        for ( ; cit.current() ; ++cit )
        {
            KWPgNumVariable * var = dynamic_cast<KWPgNumVariable *>( cit.current() );
            if ( var && !var->isDeleted() )
            {
                QSize oldSize( var->width, var->height );
                const int pageNumberOffset = kWordDocument()->variableCollection()->variableSetting()->startingPageNumber() - 1;
                switch ( var->subType() )
                {
                case KWPgNumVariable::VST_PGNUM_CURRENT:
                    //kDebug() << "KWTextFrameSet::drawFrame updating pgnum variable to " << theFrame->pageNumber()
                    //          << " and invalidating parag " << var->paragraph() << endl;
                    var->setPgNum( theFrame->pageNumber() + pageNumberOffset );
                    break;
                case KWPgNumVariable::VST_CURRENT_SECTION:
                    var->setSectionTitle( kWordDocument()->sectionTitle( theFrame->pageNumber() ) );
                    break;
                case KWPgNumVariable::VST_PGNUM_PREVIOUS:
                    var->setPgNum( qMax(theFrame->pageNumber()-1,0) + pageNumberOffset );
                    break;
                case KWPgNumVariable::VST_PGNUM_NEXT:
                    var->setPgNum( theFrame->pageNumber() + 1 + pageNumberOffset );
                    break;
                }

                var->resize();
                QSize newSize( var->width, var->height );
                if ( oldSize != newSize )
                    var->paragraph()->invalidate( 0 ); // size has changed -> need reformatting !
                var->paragraph()->setChanged( true );
            }
        }
    }

    KoTextCursor * cursor = edit ? (dynamic_cast<KWTextFrameSetEdit *>(edit) ? static_cast<KWTextFrameSetEdit *>(edit)->cursor() : 0) : 0;
    uint drawingFlags = 0;
    if ( viewMode->drawSelections() )
        drawingFlags |= KoTextDocument::DrawSelections;
    if ( !viewMode->drawFrameBackground() )
        drawingFlags |= KoTextDocument::TransparentBackground;
    if ( m_doc->backgroundSpellCheckEnabled() )
        drawingFlags |= KoTextDocument::DrawMisspelledLine;
    if ( m_doc->viewFormattingChars() )
        drawingFlags |= KoTextDocument::DrawFormattingChars;

    //kDebug(32001) << "KWTextFrameSet::drawFrame calling drawWYSIWYG. cg base color:" << cg.brush( QColorGroup::Base) << endl;
    KoTextParag * lastFormatted = textDocument()->drawWYSIWYG(
        painter, r.x(), r.y(), r.width(), r.height(),
        cg, kWordDocument(),
        onlyChanged, false, cursor, resetChanged, drawingFlags );

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

        //kDebug(32002) << "KWTextFrameSet::drawFrame drawn. onlyChanged=" << onlyChanged << " resetChanged=" << resetChanged << " lastDrawn=" << lastDrawn->paragId() << " lastDrawn's bottom:" << lastDrawn->rect().bottom() << " r.bottom=" << r.bottom() << endl;
        if ( lastDrawn && m_doc->layoutUnitToPixelY( lastDrawn->rect().bottom() ) > r.bottom() )
        {
            //kDebug(32002) << "KWTextFrameSet::drawFrame setting lastDrawn " << lastDrawn->paragId() << " to changed" << endl;
            lastDrawn->setChanged( true );
        }
    }

    // NOTE: QTextView sets m_lastFormatted to lastFormatted here
    // But when scrolling up, this causes to reformat a lot of stuff for nothing.
    // And updateViewArea takes care of formatting things before we even arrive here.

    // Blank area under the very last paragraph - QRT draws it up to textdoc->height,
    // we have to draw it from there up to the bottom of the last frame.
    if ( ( !lastFormatted || lastFormatted == textDocument()->lastParag() )
         && viewMode->drawFrameBackground() )
    {
        // This is drawing code, so we convert everything to pixels
        int docHeight = textDocument()->lastParag()->pixelRect(m_doc).bottom() + 1;
        QSize availSize = viewMode->availableSizeForText( this );
        QRect blank( 0, docHeight, availSize.width(), availSize.height() - docHeight );
        //kDebug(32002) << this << " Blank area: " << blank << endl;
        painter->fillRect( blank, cg.brush( QColorGroup::Base ) );
        // for debugging :)
        //painter->setPen( QPen(Qt::blue, 1, DashLine) );  painter->drawRect( blank );
    }
    m_currentDrawnFrame = 0L;
}

void KWTextFrameSet::drawCursor( QPainter *p, KoTextCursor *cursor, bool cursorVisible, KWCanvas *canvas, KWFrame *theFrame )
{
    // This redraws the paragraph where the cursor is - with a small clip region around the cursor
    m_currentViewMode = canvas->viewMode();
    bool hasFrames = m_currentViewMode->hasFrames();
    m_currentDrawnFrame = theFrame;

    QRect normalFrameRect;
    if (hasFrames)
        normalFrameRect = m_doc->zoomRect( theFrame->innerRect() );
    else
        normalFrameRect = QRect( QPoint(0, 0), m_currentViewMode->contentsSize() );

    KoTextParag* parag = cursor->parag();
    QPoint topLeft = parag->rect().topLeft();         // in QRT coords
    int lineY;
    // Cursor height, in pixels
    int cursorHeight = m_doc->layoutUnitToPixelY( topLeft.y(), parag->lineHeightOfChar( cursor->index(), 0, &lineY ) );
    QPoint iPoint( topLeft.x() + cursor->x(),
                   topLeft.y() + lineY );

#ifdef DEBUG_CURSOR
    kDebug() << "KWTextFrameSet::drawCursor topLeft=" << topLeft.x() << "," << topLeft.y()
              << " x=" << cursor->x() << " y=" << lineY << endl;
    kDebug() << "KWTextFrameSet::drawCursor iPoint=" << iPoint.x() << "," << iPoint.y()
              << "   cursorHeight=" << cursorHeight << endl;
#endif
    KoPoint dPoint;
    KoPoint hintDPoint = theFrame ? theFrame->innerRect().topLeft() : KoPoint();
    if ( internalToDocumentWithHint( iPoint, dPoint, hintDPoint ) )
    {
#ifdef DEBUG_CURSOR
        kDebug() << " dPoint(doc, pts)=" << dPoint.x() << "," << dPoint.y() << endl;
        QPoint debugPt = m_doc->zoomPoint( dPoint );
        kDebug() << " zoomed dPoint(doc, pixels)=" << debugPt.x() << "," << debugPt.y() << endl;
#endif
        QPoint vPoint = m_currentViewMode->normalToView( m_doc->zoomPoint( dPoint ) ); // from doc to view contents
#ifdef DEBUG_CURSOR
        kDebug() << " vPoint(view, pixels)=" << vPoint.x() << "," << vPoint.y() << endl;
#endif
        // from now on, iPoint will be in pixels
        iPoint = m_doc->layoutUnitToPixel( iPoint );
        //int xadj = parag->at( cursor->index() )->pixelxadj;
#ifdef DEBUG_CURSOR
        //kDebug() << "     iPoint in pixels : " << iPoint.x() << "," << iPoint.y() << "     will add xadj=" << xadj << endl;
#endif
        //iPoint.rx() += xadj;
        //vPoint.rx() += xadj;
        // very small clipping around the cursor
        QRect clip( vPoint.x() - 5, vPoint.y(), 10, cursorHeight );

#ifdef DEBUG_CURSOR
        kDebug() << " clip(view, before intersect)=" << clip << endl;
#endif

        QRect viewFrameRect = m_currentViewMode->normalToView( normalFrameRect );
        clip &= viewFrameRect; // clip to frame
#ifdef DEBUG_CURSOR
        kDebug() << "KWTextFrameSet::drawCursor normalFrameRect=" << normalFrameRect
                  << " clip(view, after intersect)=" << clip << endl;
#endif

        QRegion reg;
        if ( hasFrames ) {
            reg = frameClipRegion( p, theFrame, clip, m_currentViewMode );
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
                kDebug() << "        translating Y by viewFrameRect.top()-internalY in pixelY= " << viewFrameRect.top() << "-" << m_doc->zoomItY( theFrame->internalY() ) << "=" << viewFrameRect.top() - m_doc->zoomItY( theFrame->internalY() ) << endl;
#endif
                p->translate( translationX, translationY );
                p->setBrushOrigin( p->brushOrigin().x() + translationX, p->brushOrigin().y() + translationY );

                // The settings come from this frame
                KWFrame * settings = settingsFrame( theFrame );

                QBrush bgBrush( settings->backgroundColor() );
                bgBrush.setColor( KWDocument::resolveBgColor( bgBrush.color(), p ) );
                cg.setBrush( QColorGroup::Base, bgBrush );
                // color of cursor, the inverse of the frame background
                QColor background = bgBrush.color();
                cg.setColor(QColorGroup::Text, QColor( 255 - background.red(),
                    255 - background.green(), 255 - background.blue()) );
            }
            else if(dynamic_cast<KWViewModeText *>(m_currentViewMode) != 0)
                p->translate( KWViewModeText::OFFSET, 0 );

            QPixmap *pix = 0;
            uint drawingFlags = KoTextDocument::DrawSelections;
            if ( m_doc->backgroundSpellCheckEnabled() )
                drawingFlags |= KoTextDocument::DrawMisspelledLine;
            if ( m_doc->viewFormattingChars() )
                drawingFlags |= KoTextDocument::DrawFormattingChars;

            // To force the drawing to happen:
            bool wasChanged = parag->hasChanged();
            int oldLineChanged = parag->lineChanged();
            int line; // line number
            parag->lineStartOfChar( cursor->index(), 0, &line );
            parag->setChanged( false ); // not all changed, only from a given line
            parag->setLineChanged( line );

            textDocument()->drawParagWYSIWYG(
                p, parag,
                qMax(0, iPoint.x() - 5), // negative values create problems
                iPoint.y(), clip.width(), clip.height(),
                pix, cg, m_doc, // TODO view's zoom handler
                cursorVisible, cursor, false /*resetChanged*/, drawingFlags );

            if ( wasChanged )      // Maybe we have more changes to draw, than those in the small cliprect
                parag->setLineChanged( oldLineChanged ); // -1 = all
            else
                parag->setChanged( false );

            p->restore();

            //XIM Position
            QPoint ximPoint = vPoint;
            canvas->setXimPosition( ximPoint.x(), ximPoint.y(),
                                    0, cursorHeight - parag->lineSpacing( line ) );
        }
    }
    m_currentDrawnFrame = 0L;
}

void KWTextFrameSet::layout()
{
    invalidate();
    // Get the thing going though, repainting doesn't call formatMore
    m_textobj->formatMore( 2 );
}

void KWTextFrameSet::invalidate()
{
    //kDebug() << "KWTextFrameSet::invalidate " << name() << endl;
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


bool KWTextFrameSet::statistics( Q3ProgressDialog *progress, ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words,
    ulong & sentences, ulong & syllables, ulong & lines, bool selected )
{
    return m_textobj->statistics( progress, charsWithSpace, charsWithoutSpace, words, sentences, syllables, lines, selected );
}

// Only interested in the body textframeset, not in header/footer
#define kDebugBody(area) if ( frameSetInfo() == FI_BODY ) kDebug(area)

Q3ValueList<KWFrame*> KWTextFrameSet::framesFromTo( int y1, int y2 ) const
{
    Q3ValueList<KWFrame*> framesList;
    KoPoint pt;
    KWFrame * firstFrame = internalToDocument( QPoint(0, y1), pt );
    if ( !firstFrame )
        return framesList;
    framesList.append( firstFrame );
    uint frameIndex = const_cast<KWTextFrameSet *>(this)->m_frames.findRef( firstFrame );
    while ( ++frameIndex < m_frames.count() ) {
        KWFrame* f = frame( frameIndex );
        if ( f->internalY() > y2 ) // too far down, we're done
            break;
        framesList.append( f );
    }
    return framesList;
}

// Helper for adjust*. There are 3 ways to use this method.
// 1) marginLeft and marginRight set -> determination of left and right margins for adjustMargins
// 2) marginRight set -> determination of right margin for adjustRMargin
// 3) breakBegin, breakEnd set -> check whether we should jump over some frames
//                                                  [when there is not enough space besides them]
// reqMinWidth is the width that the formatter wants to use (current char/item)
// validHeight is set to the height where marginLeft/marginRight applies (TODO)
void KWTextFrameSet::getMargins( int yp, int h, int reqMinWidth,
                                 int* marginLeft, int* marginRight, int* pageWidth,
                                 int* validHeight,
                                 int* breakBegin, int* breakEnd, KoTextParag* parag )
{
    // paragLeftMargin will be used as the minimum width needed for the parag,
    // to "see" the parag.
    // So we only apply the first line margin if it increases that width, i.e. if > 0.
    // Otherwise only the first line might be visible, in a narrow passage.
    int paragLeftMargin = parag ? parag->leftMargin() : 0;
    if ( parag && !parag->string()->isRightToLeft() && parag->firstLineMargin() > 0 )
        paragLeftMargin += parag->firstLineMargin();
#ifdef DEBUG_MARGINS
    kDebugBody(32002) << "  KWTextFrameSet " << this << "(" << name() << ") getMargins yp=" << yp
                       << " h=" << h << " called by "
                       << (marginLeft && marginRight ? "adjustMargins" : "formatVertically")
                       << " paragLeftMargin=" << paragLeftMargin
                       << endl;
    // Both or none...
    if (breakBegin) assert(breakEnd);
    if (breakEnd) assert(breakBegin);
    // Idem
    if ( marginLeft ) { assert( marginRight ); assert( pageWidth ); }
#endif

    // List of text frames holding the paragraph (yp,yp+h)
    // Usually there is only one, but you can have a paragraph
    // starting in one frame/column and ending in another one.
    Q3ValueList<KWFrame*> textFrames = framesFromTo( yp, yp + h );
    if (textFrames.isEmpty())
    {
#ifdef DEBUG_MARGINS
        kDebug(32002) << "  getMargins: internalToDocument returned no text frames for y1=" << yp << " y2=" << yp + h << " ->aborting with 0 margins" << endl;
#endif
        // frame == 0 happens when the parag is under the last frame.
        // On an auto-resizable frame, we know the frame will grow so we can go ahead
        // and use its width.
        if ( !m_frames.isEmpty() && m_frames.last()->frameBehavior() == KWFrame::AutoExtendFrame )
        {
            textFrames.append( m_frames.last() );
        }
        else
        {
            // On auto-create-new-frame, this means the parag is on a not-yet-created page
            // (formatMore will notice afterwards)
            // Abort then, no need to return precise values
            // We also abort in the third case (Ignore)
            if ( validHeight )
                *validHeight = 0;
            return;
        }
    }
    else
    {

#ifdef DEBUG_MARGINS
        kDebugBody(32002) << "  getMargins: internalToDocument returned " << textFrames.count() << " frames holding this paragraph" << endl;
#endif
    }
    if ( validHeight )
        *validHeight = h; // TODO

    // Everything from there is in layout units
    // Note: it is very important that this method works in internal coordinates.
    // Otherwise, parags broken at the line-level (e.g. between two columns) are seen
    // as still in one piece, and we miss the frames in the 2nd column.
    int from = 0;
    // TODO support for variable width... maybe it's enough to take the max here
    int to = m_doc->ptToLayoutUnitPixX( textFrames.first()->innerWidth() );
    if ( pageWidth )
        *pageWidth = to;
    bool init = false;

#ifdef DEBUG_MARGINS
    kDebugBody(32002) << "  getMargins: looking for frames between " << yp << " and " << yp+h << " (internal coords)" << endl;
#endif
    if ( m_doc->layoutViewMode()->shouldAdjustMargins() )
    {
        // Principle: for every frame on top at this height, we'll move from and to
        // towards each other. The text flows between 'from' and 'to'
        for ( Q3ValueList<KWFrame*>::const_iterator txtit = textFrames.begin(), txtend = textFrames.end() ; txtit != txtend ; ++txtit ) {
            KWFrame* theFrame = *txtit;
            Q_ASSERT( theFrame->frameStack() );
            Q3ValueList<KWFrame*> onTop = theFrame->frameStack()->framesOnTop();
            for (Q3ValueListIterator<KWFrame*> fIt = onTop.begin(); from < to && fIt != onTop.end(); ++fIt )
            {
                if ( (*fIt)->runAround() == KWFrame::RA_BOUNDINGRECT )
                {
                    KoRect rectOnTop = theFrame->intersect( (*fIt)->runAroundRect() );
#ifdef DEBUG_MARGINS
                    kDebugBody(32002) << "   getMargins found frame on top " << (*fIt)->frameSet()->name() << " with rect-on-top at (normal coords) " << rectOnTop << endl;
#endif
                    QPoint iTop, iBottom; // top and bottom of intersection in internal coordinates

                    if ( documentToInternal( rectOnTop.topLeft(), iTop ) &&
                         iTop.y() <= yp + h && // optimization
                         documentToInternal( rectOnTop.bottomRight(), iBottom ) )
                    {
#ifdef DEBUG_MARGINS
                        kDebugBody(32002) << "      in internal coords: " << QRect(iTop,iBottom) << endl;
#endif
                        // Look for intersection between yp -- yp+h  and iTop -- iBottom
                        if ( qMax( yp, iTop.y() ) <= qMin( yp+h, iBottom.y() ) )
                        {
#ifdef DEBUG_MARGINS
                            kDebugBody(32002) << "   getMargins iTop=" << iTop.x() << "," << iTop.y()
                                               << " iBottom=" << iBottom.x() << "," << iBottom.y() << endl;
#endif
                            int availLeft = qMax( 0, iTop.x() - from );
                            int availRight = qMax( 0, to - iBottom.x() );
#ifdef DEBUG_MARGINS
                            kDebugBody(32002) << "   getMargins availLeft=" << availLeft
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
                                to = qMin( to, from + availLeft - 1 );  // can only go left -> qMin
                            else
                                // flow text at the right of the frame
                                from = qMax( from, to - availRight + 1 ); // can only go right -> qMax

#ifdef DEBUG_MARGINS
                            kDebugBody(32002) << "   getMargins from=" << from << " to=" << to << endl;
#endif
                            // If the available space is too small, give up on it
                            if ( to - from < reqMinWidth + paragLeftMargin )
                            {
#ifdef DEBUG_MARGINS
                                kDebugBody(32002) << "   smaller than minimum=" << m_doc->ptToLayoutUnitPixX( 15 ) + paragLeftMargin << endl;
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
                                    *breakBegin = qMin( *breakBegin, iTop.y() );
                                    *breakEnd = qMax( *breakEnd, iBottom.y() );
                                }
#ifdef DEBUG_MARGINS
                                kDebugBody(32002) << "   getMargins iBottom.y=" << iBottom.y()
                                                   << " breakBegin=" << *breakBegin
                                                   << " breakEnd=" << *breakEnd << endl;
#endif
                            }
                        } // else no intersection
                    }// else we got a 0L, or the iTop.y()<=yp+h test didn't work - wrong debug output
                    // kDebugBody(32002) << "   gerMargins: normalToInternal returned 0L" << endl;
                }
            }
        }
    }
    if ( marginLeft /*&& marginRight && pageWidth  -- implicit*/ )
    {
#ifdef DEBUG_MARGINS
        kDebugBody(32002) << "   getMargins done. from=" << from << " to=" << to << endl;
#endif
        if ( from == to ) {
            from = 0;
            to = *pageWidth;
        }

        if ( marginLeft )
            *marginLeft += from;
        if ( marginRight )
        {
#ifdef DEBUG_MARGINS
            kDebug(32002) << "    getMargins " << name()
                           << " page width=" << *pageWidth
                           << " to=" << to << endl;
#endif
            *marginRight += *pageWidth - to;
        }
    }
}

void KWTextFrameSet::adjustMargins( int yp, int h, int reqMinWidth, int& leftMargin, int& rightMargin, int& pageWidth, KoTextParag* parag )
{
#ifdef DEBUG_MARGINS
    kDebugBody(32002) << "KWTextFrameSet::adjustMargins called for paragraph " << (parag?parag->paragId():-1) << endl;
#endif
    int validHeight; // currently ignored (TODO)
    getMargins( yp, h, reqMinWidth, &leftMargin, &rightMargin, &pageWidth, &validHeight, 0L, 0L, parag );
#ifdef DEBUG_MARGINS
    kDebugBody(32002) << "KWTextFrameSet::adjustMargins(yp=" << yp << " h=" << h << " reqMinWidth=" << reqMinWidth << " returning"
                       << " leftMargin=" << leftMargin << " rightMargin=" << rightMargin
                       << " valid from " << yp << " to " << yp+validHeight << endl;
#endif
}

// helper for formatVertically
bool KWTextFrameSet::checkVerticalBreak( int & yp, int & hp, KoTextParag * parag, bool linesTogether, int breakBegin, int breakEnd )
{
    // We need the "+1" here because when skipping a frame on top, we want to be _under_
    // its bottom. Without the +1, we hit the frame again on the next adjustLMargin call.

    // Check for intersection between the parag (yp -- yp+hp) and the break area (breakBegin -- breakEnd)
    if ( qMax( yp, breakBegin ) <= qMin( yp+hp, breakEnd ) )
    {
        if ( !parag || linesTogether ) // Paragraph-level breaking
        {
#ifdef DEBUG_FORMATVERTICALLY
            kDebug(32002) << "checkVerticalBreak ADJUSTING yp=" << yp << " hp=" << hp
                           << " breakEnd+2 [new value for yp]=" << breakEnd+2 << endl;
#endif
            yp = breakEnd + 1;
            return true;
        }
        else // Line-level breaking
        {
            QMap<int, KoTextParagLineStart*>& lineStarts = parag->lineStartList();
#ifdef DEBUG_FORMATVERTICALLY
            kDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
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
                kDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                               << " line " << line << " ls->y=" << ls->y
                               << " ls->h=" << ls->h << " y=" << y
                               << " breakBegin=" << breakBegin
                               << " breakEnd=" << breakEnd << endl;
#endif
                if ( !dy )
                {
                    if ( qMax( y, breakBegin ) <= qMin( y + ls->h, breakEnd ) )
                    {
                        if ( line == 0 ) // First line ? It's like a paragraph breaking then
                        {
#ifdef DEBUG_FORMATVERTICALLY
                            kDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                                           << " BREAKING first line -> parag break" << endl;
#endif
                            yp = breakEnd + 1;
                            return true;
                        }
                        dy = breakEnd + 1 - y;
                        ls->y = breakEnd + 1 - parag->rect().y();
#ifdef DEBUG_FORMATVERTICALLY
                        kDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                                       << " BREAKING at line " << line << " dy=" << dy << "  Setting ls->y to " << ls->y << ", y=" << breakEnd << endl;
#endif
                    }
                }
                else
                {
                    ls->y += dy;
#ifdef DEBUG_FORMATVERTICALLY
                    if ( dy )
                        kDebug(32002) << "                   moving down to position ls->y=" << ls->y << endl;
#endif
                }
            }
            parag->setMovedDown( true );
            parag->setHeight( hp + dy );
#ifdef DEBUG_FORMATVERTICALLY
            kDebug(32002) << "Paragraph height set to " << hp+dy << endl;
#endif
            hp += dy;
            return true;
        } // End of line-level breaking
    }
    return false;
}

int KWTextFrameSet::formatVertically( KoTextParag * _parag, const QRect& paragRect )
{
    // WARNING: in this whole method parag can be 0. See adjustFlow()
    KWTextParag *parag = static_cast<KWTextParag *>( _parag );
    if ( !m_doc->layoutViewMode()->shouldFormatVertically() )
    {
        return 0;
    }

#ifdef DEBUG_FORMATVERTICALLY
    kDebugBody(32002) << "KWTextFrameSet::formatVertically called for paragraph " << (parag?parag->paragId():-1) << endl;
#endif

    int yp = paragRect.y();
    int hp = paragRect.height();
    int oldHeight = hp;
    int oldY = yp;

    // This is called by KoTextFormatter to apply "vertical breaks".
    // End of frames/pages lead to those "vertical breaks".
    // What we do, is adjust the Y accordingly,
    // to implement page-break at the paragraph level and at the line level.
    // It's cumulative (the space of one break will be included in the further
    // paragraph's y position), which makes it easy to implement.
    // But don't forget that formatVertically is called twice for every parag, since the formatting
    // is re-done after moving down.

    bool linesTogether = parag ? parag->linesTogether() : true;
    bool hardFrameBreak = parag ? parag->hardFrameBreakBefore() : false;
    if ( !hardFrameBreak && parag && parag->prev() )
        hardFrameBreak = static_cast<KWTextParag *>(parag->prev())->hardFrameBreakAfter();

#ifdef DEBUG_FORMATVERTICALLY
    kDebugBody(32002) << "KWTextFrameSet::formatVertically parag=" << parag
                       << " linesTogether=" << linesTogether << " hardFrameBreak=" << hardFrameBreak
                       << " yp=" << yp
                       << " hp=" << hp << endl;
#endif

    int totalHeight = 0;
    Q3PtrListIterator<KWFrame> frameIt( frameIterator() );
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
            Q3PtrListIterator<KWFrame> nextFrame( frameIt );
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
                kDebug(32002) << "KWTextFrameSet::formatVertically -> HARD FRAME BREAK" << endl;
                kDebug(32002) << "KWTextFrameSet::formatVertically yp now " << yp << endl;
#endif
                break;
            }

#ifdef DEBUG_FORMATVERTICALLY
            kDebug(32002) << " formatVertically: frameHeight=" << frameHeight << " bottom=" << bottom << endl;
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

#ifdef DEBUG_FORMATVERTICALLY
    kDebug(32002) << " formatVertically: now looking at RA_SKIP" << endl;
#endif


    // Another case for a vertical break is frames with the RA_SKIP flag
    // Currently looking at all frames on top of all of our frames... maybe optimize better
    frameIt.toFirst();
    for ( ; frameIt.current(); ++frameIt )
    {
        Q_ASSERT( frameIt.current()->frameStack() );
        Q3ValueList<KWFrame*> onTop = frameIt.current()->frameStack()->framesOnTop();
        for (Q3ValueListIterator<KWFrame*> fIt = onTop.begin(); fIt != onTop.end(); ++fIt )
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
                    kDebug(32002) << "KWTextFrameSet::formatVertically breaking around RA_SKIP frame yp="<<yp<<" hp=" << hp << endl;
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
    int reqMinWidth = parag ? parag->string()->at( 0 ).width : 0;
    getMargins( yp, hp, reqMinWidth, 0L, 0L, 0L, 0L, &breakBegin, &breakEnd, parag );
    if ( breakEnd )
    {
        kDebug(32002) << "KWTextFrameSet("<<name()<<")::formatVertically no-space case. breakBegin=" << breakBegin
                       << " breakEnd=" << breakEnd << " hp=" << hp << endl;
        Q_ASSERT( breakBegin <= breakEnd );
        if ( checkVerticalBreak( yp, hp, parag, linesTogether, breakBegin, breakEnd ) )
            ; //kDebug(32002) << "checkVerticalBreak ok." << endl;
        else // shouldn't happen
            kWarning(32002) << "checkVerticalBreak didn't find it" << endl;
    }

    // ## TODO loop around those three methods until we don't move anymore ?

    if ( parag )
    {
        if ( hp != oldHeight )
            parag->setHeight( hp );
        if ( yp != oldY ) {
            QRect r = parag->rect();
            r.moveBy( 0, yp - oldY );
            parag->setRect( r );
            parag->setMovedDown( true );
        }
    }
#ifdef DEBUG_FORMATVERTICALLY
    kDebug() << "KWTextFrameSet::formatVertically returning " << ( yp + hp ) - ( oldY + oldHeight ) << endl;
#endif
    return ( yp + hp ) - ( oldY + oldHeight );
}

// adjustFlow is called e.g. to break the "top margin" of a paragraph.
// There is no parag pointer in that case.
int KWTextFrameSet::adjustFlow( int y, int w, int h )
{
    QRect r( 0, y, w, h );
    return formatVertically( 0L, r );
}

void KWTextFrameSet::fixParagWidth( KWTextParag* parag )
{
    // Fixing the parag rect for the formatting chars (CR and frame break).
    if ( parag && m_doc->viewFormattingChars() && parag->rect().width() < textDocument()->width() )
    {
        if ( parag->hardFrameBreakAfter() )
        {
            KoTextFormat * lastFormat = parag->at( parag->length() - 1 )->format();
            const QFontMetrics& refFontMetrics = lastFormat->refFontMetrics();
            // keep in sync with KWTextFrameSet::formatVertically
            QString str = i18n( "--- Frame Break ---" );
            int width = refFontMetrics.width( str );
            parag->setWidth( qMin( parag->rect().width() + width, textDocument()->width() ) );
        }
        else // default KoTextFormatter implementation
            parag->fixParagWidth( true );
    }
}

KWTextFrameSet::~KWTextFrameSet()
{
    textDocument()->takeFlow();
    //kDebug(32001) << "KWTextFrameSet::~KWTextFrameSet" << endl;
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
        int pageOne = frameOne->pageNumber();
        int pageTwo = frameTwo->pageNumber();
        if( (pageOne == -1) ^ (pageTwo == -1)) {
            if( pageOne == -1 )
                return 5; // undefined is higher than defined.
            return -5;
        }
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
        //kDebug(32002) << "KWTextFrameSet::updateFrames " << name() << " not visible" << endl;
        m_textobj->setVisible(false);
        return;
    }
    m_textobj->setVisible(true);

    //kDebug(32002) << "KWTextFrameSet::updateFrames " << name() << " frame-count=" << m_frames.count() << endl;

    // Sort frames of this frameset on (y coord, x coord)
    // Adjustment on 20-Jun-2002 which does not change the itent of this but moves the
    // sorting from top-left of frame to the whole frame area. (TZ)

    Q3ValueList<FrameStruct> sortedFrames;

    int width = 0;
    Q3PtrListIterator<KWFrame> frameIter( frameIterator() );
    for ( ; frameIter.current(); ++frameIter )
    {
        // Calculate max width while we're at it
        //kDebug(32002) << "KWTextFrameSet::updateFrames frame " << *frameIter.current() << " innerWidth=" << frameIter.current()->innerWidth() << "pt" << endl;
        width = qMax( width, m_doc->ptToLayoutUnitPixX( frameIter.current()->innerWidth()));
        if ( flags & SortFrames )
        {
            FrameStruct str;
            str.frame = frameIter.current();
            sortedFrames.append( str );
        }
    }
    if ( width != textDocument()->width() )
    {
        //kDebug(32002) << "KWTextFrameSet::updateFrames setWidth " << width << " LU pixels." << endl;
        //textDocument()->setMinimumWidth( -1, 0 );
        textDocument()->setWidth( width + 1 ); // QRect semantics problem (#32866)
    } //else kDebug(32002) << "KWTextFrameSet::updateFrames width already " << width << " LU pixels." << endl;

    if ( flags & SortFrames )
    {
        qHeapSort( sortedFrames );

        // Re-fill the frames list with the frames in the right order
        m_frames.setAutoDelete( false );
        m_frames.clear();

        Q3ValueList<FrameStruct>::Iterator it = sortedFrames.begin();
        for ( ; it != sortedFrames.end() ; ++it )
            m_frames.append( (*it).frame );
    }

    double availHeight = 0;
    double internalYpt = 0;
    double lastRealFrameHeight = 0;
    bool firstFrame = true;

    Q3PtrListIterator<KWFrame> frameIt( m_frames );
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
    //kDebug(32002) << this << " (" << name() << ") KWTextFrameSet::updateFrames availHeight=" << availHeight
    //               << " (LU: " << m_doc->ptToLayoutUnitPixY( availHeight ) << ")" << endl;
    m_frames.setAutoDelete( true );

    KWFrameSet::updateFrames( flags );
}

int KWTextFrameSet::availableHeight() const
{
    return m_textobj->availableHeight();
}

KWFrame * KWTextFrameSet::internalToDocument( const KoPoint &relPoint, KoPoint &dPoint ) const
{
#ifdef DEBUG_ITD
    kDebug() << name() << " ITD called for relPoint=" << relPoint.x() << "," << relPoint.y() << endl;
#endif
    if ( !m_doc->layoutViewMode()->hasFrames() ) { // text viewmode
        dPoint = relPoint;
        return m_frames.getFirst();
    }
    // This does a binary search in the m_framesInPage array, with internalY as criteria
    // We only look at the first frame of each page. Refining is done later on.
    Q_ASSERT( !m_framesInPage.isEmpty() );
    int len = m_framesInPage.count();
    int n1 = 0;
    int n2 = len - 1;
    double internalY = 0.0;
    int mid = 0;
    bool found = false;
    while ( n1 <= n2 ) {
        double res;
        mid = (n1 + n2)/2;
#ifdef DEBUG_ITD
        kDebug() << "ITD: begin. mid=" << mid << endl;
#endif
        Q_ASSERT( m_framesInPage[mid] ); // We have no null items
        if ( m_framesInPage[mid]->isEmpty() )
            res = -1;
        else
        {
            KWFrame * theFrame = m_framesInPage[mid]->first();
            internalY = theFrame->internalY();
#ifdef DEBUG_ITD
            kDebug() << "ITD: relPoint.y=" << relPoint.y() << " internalY=" << internalY << endl;
#endif
            res = relPoint.y() - internalY;
#ifdef DEBUG_ITD
            kDebug() << "ITD: res=" << res << endl;
#endif
            // Anything between this internalY (top) and internalY+height (bottom) is fine
            // (Using the next page's first frame's internalY only works if there is a frame on the next page)
            if ( res >= 0 )
            {
                double height = theFrame->innerHeight();
#ifdef DEBUG_ITD
                kDebug() << "ITD: height=" << height << " -> the bottom is at " << internalY+height << endl;
#endif
                if ( relPoint.y() < internalY + height )
                {
#ifdef DEBUG_ITD
                    kDebug() << "ITD: found a match " << mid << endl;
#endif
                    found = true;
                    break;
                }
            }
        }
        // res == 0 can't happen in theory, but in practice it happens when a frame has a height of 0
        // (e.g. newly imported table without correct row heights)
        if ( res < 0 )
            n2 = mid - 1;
        else // if ( res >= 0 )
            n1 = mid + 1;
#ifdef DEBUG_ITD
        kDebug() << "ITD: End of loop. n1=" << n1 << " n2=" << n2 << endl;
#endif
    }
    if ( !found )
    {
        // Not found (n2 < n1)
        // We might have missed the frame because n2 has many frames
        // (and we only looked at the first one).
        mid = n2;
#ifdef DEBUG_ITD
        kDebug() << "ITD: Setting mid to n2=" << mid << endl;
#endif
        if ( mid < 0 )
        {
#ifdef DEBUG_ITD
            kDebug(32002) << "KWTextFrameSet::internalToDocument " << relPoint.x() << "," << relPoint.y()
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
            kDebug() << "KWTextFrameSet::internalToDocument going back to page " << mid << " - frame: " << theFrame->internalY() << endl;
#endif
            if ( theFrame->internalY() == internalY ) // same internalY as the frame we found before
                result = mid;
            else
                break;
        }
    }

    // Now iterate over the frames in page 'result' and find the right one
    Q3PtrListIterator<KWFrame> frameIt( *m_framesInPage[result] );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *theFrame = frameIt.current();
        KoRect relRect( 0, theFrame->internalY(), theFrame->innerWidth(), theFrame->innerHeight() );
#ifdef DEBUG_ITD
        kDebug() << "KWTextFrameSet::internalToDocument frame's relative rect:" << relRect << endl;
#endif
        if ( relRect.contains( relPoint ) ) // both relRect and relPoint are in "relative coordinates"
        {
            dPoint = internalToDocumentKnowingFrame( relPoint, theFrame );
            return theFrame;
        }
    }
#ifdef DEBUG_ITD
    kDebug(32002) << "KWTextFrameSet::internalToDocument " << relPoint.x() << "," << relPoint.y()
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
        kDebug() << "KoTextDocument width = " << textDocument()->width() << " height = " << textDocument()->height() << endl;
    }

    Q3PtrListIterator<KoTextCustomItem> cit( textDocument()->allCustomItems() );
    for ( ; cit.current() ; ++cit )
    {
      KWAnchor *anc = dynamic_cast<KWAnchor *>( cit.current() );
      if (anc)
          kDebug() << "Inline framesets: " << anc->frameSet()->name() << endl;
    }
}
#endif

QDomElement KWTextFrameSet::saveInternal( QDomElement &parentElem, bool saveFrames, bool saveAnchorsFramesets )
{
    if ( m_frames.isEmpty() ) // Deleted frameset -> don't save
        return QDomElement();

    QDomElement framesetElem = parentElem.ownerDocument().createElement( "FRAMESET" );
    parentElem.appendChild( framesetElem );

    if ( m_groupmanager ) {
        framesetElem.setAttribute( "grpMgr", m_groupmanager->name() );

        KWTableFrameSet::Cell *cell = (KWTableFrameSet::Cell *)this;
        framesetElem.setAttribute( "row", cell->firstRow() );
        framesetElem.setAttribute( "col", cell->firstColumn() );
        framesetElem.setAttribute( "rows", cell->rowSpan() );
        framesetElem.setAttribute( "cols", cell->columnSpan() );
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

    return framesetElem;
}

KWFrame* KWTextFrameSet::loadOasisTextFrame( const QDomElement& frameTag, const QDomElement &tag, KoOasisContext& context )
{
    context.styleStack().save();
    context.fillStyleStack( frameTag, KoXmlNS::draw, "style-name", "graphic" ); // get the style for the graphics element

    KWFrame* frame = loadOasisFrame( frameTag, context );

    // Load minimum height - only available for text-box
    bool hasMinHeight = tag.hasAttributeNS( KoXmlNS::fo, "min-height" );
    if ( hasMinHeight ) {
        double height = KoUnit::parseValue( tag.attributeNS( KoXmlNS::fo, "min-height", QString::null ) );
        frame->setMinimumFrameHeight( height );
        if ( height > frame->height() || !tag.hasAttributeNS( KoXmlNS::fo, "height" ) )
            frame->setHeight( height );
    }

    // Load overflow behavior (OASIS 14.27.27, not in OO-1.1 DTD). This is here since it's only for text framesets.
    const QString overflowBehavior = context.styleStack().attributeNS( KoXmlNS::style, "overflow-behavior" );
    if ( frame->minimumFrameHeight() > 0 )
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
    else if ( overflowBehavior == "auto-create-new-frame" )
    {
        frame->setFrameBehavior( KWFrame::AutoCreateNewFrame );
        frame->setNewFrameBehavior( KWFrame::Reconnect ); // anything else doesn't make sense
    }
    else if ( overflowBehavior.isEmpty() || overflowBehavior == "clip" )
        frame->setFrameBehavior( KWFrame::Ignore );
    else
        kWarning(32001) << "Unknown value for style:overflow-behavior: " << overflowBehavior << endl;

    context.styleStack().restore();

    return frame;
}

void KWTextFrameSet::loadOasisContent( const QDomElement &bodyElem, KoOasisContext& context )
{
    return m_textobj->loadOasisContent( bodyElem, context, m_doc->styleCollection() );
}

KWFrame* KWTextFrameSet::loadOasis( const QDomElement& frameTag, const QDomElement &tag, KoOasisContext& context )
{
    KWFrame* frame = loadOasisTextFrame( frameTag, tag, context );
    loadOasisContent( tag, context );
    return frame;
}

static void finishTOC( KoXmlWriter& writer )
{
    writer.endElement(); // text:table-of-content
    writer.endElement(); // text:index-body
}

void KWTextFrameSet::saveOasisContent( KoXmlWriter& writer, KoSavingContext& context ) const
{
    // TODO save protectContent

    QMap<const KoTextParag*, KoTextBookmarkList> bookmarksPerParagraph;
    if ( m_doc->bookmarkList() )
        bookmarksPerParagraph = m_doc->bookmarkList()->bookmarksPerParagraph();

    // Basically just call saveOasis on every paragraph.
    // But we do table-of-contents-handling (for kword) in addition,
    // as well as bookmarks.
    KoTextParag* parag = textDocument()->firstParag();
    bool inTOC = false;
    while ( parag ) {
        bool tocParag = parag->partOfTableOfContents();
        if ( tocParag != inTOC ) {
            if ( tocParag ) { // first TOC paragraph
                writer.startElement( "text:table-of-content" );
                writer.addAttribute( "text:name", "Table Of Contents" );
                writer.addAttribute( "text:protected", "false" ); // true by default in OO, but we don't support that yet anyway
                writer.startElement( "text:table-of-content-source" );
                // TODO writer.addAttribute( "text:outline-level", ... );
                // TODO for each level writer.startElement( "text:table-of-content-entry-template" );
                // TODO writer.endElement(); // text:table-of-content-entry-template
                writer.endElement(); // text:table-of-content-source
                writer.startElement( "text:index-body" );
                writer.startElement( "text:index-title" );
                writer.addAttribute( "text:name", "Table Of Contents Heading" );
            } else {
                finishTOC( writer );
            }
        }


        // I want Qt4's QMap/QHash::value()!
        KoSavingContext::BookmarkPositions bookmarkStarts, bookmarkEnds;
        QMap<const KoTextParag*, KoTextBookmarkList>::const_iterator bkit = bookmarksPerParagraph.find( parag );
        if ( bkit != bookmarksPerParagraph.end() ) {
            // Massage a bit the bookmarks data; KoTextParag wants it ordered by position, for speed.
            const KoTextBookmarkList& bookmarks = *bkit;
            for ( KoTextBookmarkList::const_iterator it = bookmarks.begin(); it != bookmarks.end(); ++it )
            {
                const KoTextBookmark& bk = *it;
                if ( bk.startParag() == parag )
                    bookmarkStarts.append( KoSavingContext::BookmarkPosition(
                                               bk.bookmarkName(), bk.bookmarkStartIndex(),
                                               bk.isSimple() ) );
                if ( bk.endParag() == parag && !bk.isSimple() )
                    bookmarkEnds.append( KoSavingContext::BookmarkPosition( bk.bookmarkName(),
                                                                            bk.bookmarkEndIndex(), false ) );
            }
            qHeapSort( bookmarkStarts );
            qHeapSort( bookmarkEnds );
        }
        // should be done in all cases, even if both lists are empty
        context.setBookmarkPositions( bookmarkStarts, bookmarkEnds );

        // Save the whole parag, without the trailing space.
        parag->saveOasis( writer, context, 0, parag->lastCharPos() );

        if ( tocParag && !inTOC )
            writer.endElement(); // text:index-title
        inTOC = tocParag;

        parag = parag->next();
    }
    if ( inTOC )
        finishTOC( writer );
}

void KWTextFrameSet::saveOasis( KoXmlWriter& writer, KoSavingContext& context, bool saveFrames ) const
{
    // Save first frame with the whole contents
    KWFrame* frame = m_frames.getFirst();
    QString lastFrameName = name();
    frame->startOasisFrame( writer, context.mainStyles(), lastFrameName );

    QString nextFrameName = name() + '-';

    writer.startElement( "draw:text-box" );
    if ( frame->frameBehavior() == KWFrame::AutoExtendFrame )
        writer.addAttributePt( "fo:min-height", frame->minimumFrameHeight() );
    if ( m_frames.count() > 1 && saveFrames )
        writer.addAttribute( "draw:chain-next-name", nextFrameName + '2' );
    saveOasisContent( writer, context );
    writer.endElement(); // draw:text-box
    writer.endElement(); // draw:frame

    // Save other frames using chaining
    if ( saveFrames ) // false when called from KWDocument::saveSelectedFrames
    {
        int frameNumber = 2;
        Q3PtrListIterator<KWFrame> frameIter( frameIterator() );
        ++frameIter; // skip first frame, already saved
        for ( ; frameIter.current(); ++frameIter, ++frameNumber )
        {
            const QString frameName = nextFrameName + QString::number( frameNumber );
            frameIter.current()->startOasisFrame( writer, context.mainStyles(), frameName, lastFrameName );
            lastFrameName = frameName; // this is used for copy-frames
            writer.startElement( "draw:text-box" );
            if ( frame->frameBehavior() == KWFrame::AutoExtendFrame )
                writer.addAttributePt( "fo:min-height", frame->minimumFrameHeight() );
            if ( frameNumber < (int)m_frames.count() )
                writer.addAttribute( "draw:chain-next-name", nextFrameName + QString::number( frameNumber+1 ) );
            // No contents. Well, OOo saves an empty paragraph, but I'd say that's wrong.
            writer.endElement();
            writer.endElement(); // draw:frame
        }
    }
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
    //kDebug(32001) << "KWTextFrameSet::load done" << endl;
}

void KWTextFrameSet::finalize()
{
    KWFrameSet::finalize();
    m_textobj->formatMore( 0 ); // just to get the timer going
    // This is important in case of auto-resized frames or table cells,
    // which come from an import filter, which didn't give them the right size.
    // However it shouldn't start _now_ (so we use 0), because e.g. main frames
    // don't have the right size yet (KWFrameLayout not done yet).
}

void KWTextFrameSet::setVisible(bool visible)
{
    setInlineFramesVisible( visible );
    KWFrameSet::setVisible( visible );
}

void KWTextFrameSet::setInlineFramesVisible(bool visible)
{
    Q3PtrListIterator<KoTextCustomItem> cit( textDocument()->allCustomItems() );
    for ( ; cit.current() ; ++cit )
    {
      KWAnchor *anc = dynamic_cast<KWAnchor *>( cit.current() );
      if (anc)
            anc->frameSet()->setVisible( visible );
    }
}

void KWTextFrameSet::addTextFrameSets( Q3PtrList<KWTextFrameSet> & lst, bool onlyReadWrite )
{
    if (!textObject()->protectContent() || !onlyReadWrite)
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

bool KWTextFrameSet::slotAfterFormattingNeedMoreSpace( int bottom, KoTextParag *lastFormatted )
{
    int availHeight = availableHeight();
#ifdef DEBUG_FORMAT_MORE
    if(lastFormatted)
        kDebug(32002) << "slotAfterFormatting We need more space in " << name()
                       << " bottom=" << bottom + lastFormatted->rect().height()
                       << " availHeight=" << availHeight << endl;
    else
        kDebug(32002) << "slotAfterFormatting We need more space in " << name()
                       << " bottom2=" << bottom << " availHeight=" << availHeight << endl;
#endif
    if ( m_frames.isEmpty() )
    {
        kWarning(32002) << "slotAfterFormatting no more space, but no frame !" << endl;
        return true; // abort
    }

    KWFrame::FrameBehavior frmBehavior = m_frames.last()->frameBehavior();
    if ( frmBehavior == KWFrame::AutoExtendFrame && isProtectSize())
        frmBehavior = KWFrame::Ignore;
    if (  frmBehavior ==  KWFrame::AutoCreateNewFrame )
    {
        KWFrame *theFrame = settingsFrame( m_frames.last() );
        double minHeight = s_minFrameHeight + theFrame->paddingTop() + theFrame->paddingBottom() + 5;
        if ( availHeight < minHeight )
            frmBehavior = KWFrame::Ignore;
    }

    int difference = ( bottom + 2 ) - availHeight; // in layout unit pixels
#ifdef DEBUG_FORMAT_MORE
    kDebug(32002) << "AutoExtendFrame bottom=" << bottom << " availHeight=" << availHeight
                   << " => difference = " << difference << endl;
#endif
    if( lastFormatted && bottom + lastFormatted->rect().height() > availHeight ) {
#ifdef DEBUG_FORMAT_MORE
        kDebug(32002) << " next will be off -> adding " << lastFormatted->rect().height() << endl;
#endif
        difference += lastFormatted->rect().height();
    }

    switch ( frmBehavior ) {
    case KWFrame::AutoExtendFrame:
    {
        if(difference > 0) {
            // There's no point in resizing a copy, so go back to the last non-copy frame
            KWFrame *theFrame = settingsFrame( m_frames.last() );
            double wantedPosition = 0;

            // Footers and footnotes go up
            if ( theFrame->frameSet()->isAFooter() || theFrame->frameSet()->isFootNote() )
            {
                // The Y position doesn't matter much, recalcFrames will reposition the frame
                // But the point of this code is set the correct height for the frame.
                double maxFooterSize = footerHeaderSizeMax( theFrame );
                double diffPt = m_doc->layoutUnitPtToPt( m_doc->pixelYToPt( difference ) );
                wantedPosition = theFrame->top() - diffPt;
#ifdef DEBUG_FORMAT_MORE
                kDebug() << "  diffPt=" << diffPt << " -> wantedPosition=" << wantedPosition << endl;
#endif
                if ( wantedPosition < 0 )
                {
                    m_textobj->setLastFormattedParag( 0 );
                    return true; // abort
                }

                if ( wantedPosition != theFrame->top() &&
                     ( theFrame->frameSet()->isFootEndNote() ||
                       theFrame->bottom() - maxFooterSize <= wantedPosition ) ) // Apply maxFooterSize for footers only
                {
                    theFrame->setTop( wantedPosition );
#ifdef DEBUG_FORMAT_MORE
                    kDebug() << "  ok: frame=" << *theFrame << " bottom=" << theFrame->bottom() << " height=" << theFrame->height() << endl;
#endif
                    frameResized( theFrame, true );
                    // We only got room for the next paragraph, we still have to keep the formatting going...
                    return false; // keep going
                }
                kDebug() << "slotAfterFormatting didn't manage to get more space for footer/footnote, aborting" << endl;
                return true; // abort
            }
            // Other frames are resized by the bottom

            wantedPosition = m_doc->layoutUnitPtToPt( m_doc->pixelYToPt( difference ) ) + theFrame->bottom();
            KWPage *page = m_doc->pageManager()->page( theFrame );
            double pageBottom;
            if(page)
                pageBottom = page->offsetInDocument() + page->height() - page->bottomMargin();
            else
                pageBottom = theFrame->bottom();
            double newPosition = qMin( wantedPosition, pageBottom );
            kDebug(32002) << "wantedPosition=" << wantedPosition << " pageBottom=" << pageBottom
                           << " -> newPosition=" << newPosition << endl;

            if ( theFrame->frameSet()->isAHeader() )
            {
                double maxHeaderSize=footerHeaderSizeMax( theFrame );
                newPosition = qMin( newPosition, maxHeaderSize + theFrame->top() );
            }

            newPosition = qMax( newPosition, theFrame->top() ); // avoid negative heights
            kDebug(32002) << "newPosition=" << newPosition << endl;

            bool resized = false;
            if(theFrame->frameSet()->groupmanager()) {
                KWTableFrameSet *table = theFrame->frameSet()->groupmanager();
#ifdef DEBUG_FORMAT_MORE
                kDebug(32002) << "is table cell; just setting new minFrameHeight, to " << newPosition - theFrame->top() << endl;
#endif
                double newMinFrameHeight = newPosition - theFrame->top();
                resized = QABS( newMinFrameHeight - theFrame->minimumFrameHeight() ) > 1E-10;
                if ( resized ) {
                    theFrame->setMinimumFrameHeight( newMinFrameHeight );
                    KWTableFrameSet::Cell *cell = (KWTableFrameSet::Cell *)theFrame->frameSet();
                    table->recalcCols(cell->firstColumn(), cell->firstRow());
                    table->recalcRows(cell->firstColumn(), cell->firstRow());

                    if (!  table->anchorFrameset() )
                        ;// do nothing
                    else if ( table->anchorFrameset() && table->anchorFrameset()->isAHeader() ) //we must recalculate the header frame size
                    {
                      theFrame = table->anchorFrameset()->frameIterator().getLast();
                      theFrame->setBottom(newPosition);
                      frameResized( theFrame, false );
                    }
                    else if ( table->anchorFrameset()->isAFooter() || table->anchorFrameset()->isFootNote() ) //we must recalculate the footer frame size
                    {
                      theFrame = table->anchorFrameset()->frameIterator().getLast();
                      // The Y position doesn't matter much, recalcFrames will reposition the frame
                      // But the point of this code is set the correct height for the frame.
                      double maxFooterSize = footerHeaderSizeMax( theFrame );
                      double diffPt = m_doc->layoutUnitPtToPt( m_doc->pixelYToPt( difference ) );
                      wantedPosition = theFrame->top() - diffPt;
                      if ( wantedPosition < 0 )
                      {
                        m_textobj->setLastFormattedParag( 0 );
                        return true; // abort
                      }

                      if ( wantedPosition != theFrame->top() &&
                           ( theFrame->frameSet()->isFootEndNote() ||
                           theFrame->bottom() - maxFooterSize <= wantedPosition ) ) // Apply maxFooterSize for footers only
                      {
                        theFrame->setTop( wantedPosition );
                        frameResized( theFrame, true );
                        // We only got room for the next paragraph, we still have to keep the formatting going...
                      }
                    }

                    m_doc->delayedRepaintAllViews();
                }
                return true; // abort formatting for now (not sure this is correct)
            } else {
                resized = QABS( theFrame->bottom() - newPosition ) > 1E-10;
#ifdef DEBUG_FORMAT_MORE
                kDebug() << "  bottom=" << theFrame->bottom() << " new position:" << newPosition << " wantedPosition=" << wantedPosition << "  resized=" << resized << endl;
#endif

                if ( resized )
                {
#ifdef DEBUG_FORMAT_MORE
                    kDebug(32002) << "slotAfterFormatting changing bottom from " << theFrame->bottom() << " to " << newPosition << endl;
#endif
                    theFrame->setBottom(newPosition);
                    frameResized( theFrame, false );
                }
            }

            if(newPosition < wantedPosition &&
               (theFrame->newFrameBehavior() == KWFrame::Reconnect
                && !theFrame->frameSet()->isEndNote())) // end notes are handled by KWFrameLayout
            {
                wantedPosition = wantedPosition - newPosition + theFrame->top() + page->height();
#ifdef DEBUG_FORMAT_MORE
                kDebug(32002) << "Not enough room in this page -> creating new one, with a reconnect frame" << endl;
                kDebug(32002) << "new wantedPosition=" << wantedPosition << endl;
#endif

                // fall through to AutoCreateNewFrame
            }
            else if(newPosition < wantedPosition && (theFrame->newFrameBehavior() == KWFrame::NoFollowup)) {
                if ( theFrame->frameSet()->isEndNote() ) // we'll need a new page
                    m_doc->delayedRecalcFrames( theFrame->pageNumber() );

                m_textobj->setLastFormattedParag( 0 );
                return true; // abort
            } else {
                if ( resized ) // we managed to resize a frame
                    return false; // keep going
                return true; // abort
            }
        }
    }

    case KWFrame::AutoCreateNewFrame:
    {
        // We need a new frame in this frameset.
        return createNewPageAndNewFrame( lastFormatted, difference );
    }

    case KWFrame::Ignore:
#ifdef DEBUG_FORMAT_MORE
        kDebug(32002) << "slotAfterFormatting frame behaviour is Ignore" << endl;
#endif
        m_textobj->setLastFormattedParag( 0 );
        return true; // abort
    }
    kWarning() << "NEVERREACHED" << endl;
    // NEVERREACHED
    return true;
}

void KWTextFrameSet::slotAfterFormattingTooMuchSpace( int bottom )
{
    int availHeight = availableHeight();
    // The + 2 here leaves 2 pixels below the last line. Without it we hit
    // the "break at end of frame" case in formatVertically (!!).
    int difference = availHeight - ( bottom + 2 );
#ifdef DEBUG_FORMAT_MORE
    kDebug(32002) << "slotAfterFormatting less text than space (AutoExtendFrame). Frameset " << name() << " availHeight=" << availHeight << " bottom=" << bottom << " ->difference=" << difference << endl;
#endif
    // There's no point in resizing a copy, so go back to the last non-copy frame
    KWFrame *theFrame = settingsFrame( m_frames.last() );
#ifdef DEBUG_FORMAT_MORE
    kDebug(32002) << "   frame is " << *theFrame << " footer:" << ( theFrame->frameSet()->isAFooter() || theFrame->frameSet()->isFootEndNote() ) << endl;
#endif
    if ( theFrame->frameSet()->isAFooter() || theFrame->frameSet()->isFootEndNote() )
    {
        double wantedPosition = theFrame->top() + m_doc->layoutUnitPtToPt( m_doc->pixelYToPt( difference ) );
        Q_ASSERT( wantedPosition < theFrame->bottom() );
        if ( wantedPosition != theFrame->top() )
        {
#ifdef DEBUG_FORMAT_MORE
            kDebug() << "   top= " << theFrame->top() << " setTop " << wantedPosition << endl;
#endif
            theFrame->setTop( wantedPosition );
#ifdef DEBUG_FORMAT_MORE
            kDebug() << "    -> the frame is now " << *theFrame << endl;
#endif
            frameResized( theFrame, true );
        }
    }
    else // header or other frame: resize bottom
    {
        double wantedPosition = theFrame->bottom() - m_doc->layoutUnitPtToPt( m_doc->pixelYToPt( difference ) );
#ifdef DEBUG_FORMAT_MORE
        kDebug() << "slotAfterFormatting wantedPosition=" << wantedPosition << " top+minheight=" << theFrame->top() + s_minFrameHeight << endl;
#endif
        wantedPosition = qMax( wantedPosition, theFrame->top() + s_minFrameHeight );
        if( theFrame->frameSet()->groupmanager() ) {
            if ( wantedPosition != theFrame->bottom()) {
                KWTableFrameSet *table = theFrame->frameSet()->groupmanager();
                // When a frame can be smaller we don't rescale it if it is a table, since
                // we don't have the full picture of the change.
                // We will set the minFrameHeight to the correct value and let the tables code
                // do the rescaling based on all the frames in the row. (see KWTableFrameSet::recalcRows())
                if(wantedPosition != theFrame->top() + theFrame->minimumFrameHeight()) {
                    theFrame->setMinimumFrameHeight(wantedPosition - theFrame->top());
#ifdef DEBUG_FORMAT_MORE
                    kDebug(32002) << "is table cell; only setting new minFrameHeight to " << theFrame->minimumFrameHeight() << ", recalcrows will do the rest" << endl;
#endif
                    KWTableFrameSet::Cell *cell = (KWTableFrameSet::Cell *)theFrame->frameSet();
                    table->recalcCols(cell->firstColumn(), cell->firstRow());
                    table->recalcRows(cell->firstColumn(), cell->firstRow());

                    if (!  table->anchorFrameset() )
                        ;// do nothing
                    else if ( table->anchorFrameset() && table->anchorFrameset()->isAHeader()  )
                    {
                      theFrame = table->anchorFrameset()->frameIterator().getLast();
                      theFrame->setBottom(wantedPosition);
                      frameResized( theFrame, false );
                    }
                    else if ( table->anchorFrameset()->isAFooter() ||  table->anchorFrameset()->isFootEndNote() )
                    {
                      theFrame = table->anchorFrameset()->frameIterator().getLast();
                      double wantedPosition = theFrame->top() + m_doc->layoutUnitPtToPt( m_doc->pixelYToPt( difference ) );
                      Q_ASSERT( wantedPosition < theFrame->bottom() );
                      if ( wantedPosition != theFrame->top() )
                      {
                        theFrame->setTop( wantedPosition );
                        frameResized( theFrame, true );
                      }
                    }
                    m_doc->delayedRepaintAllViews();
                }
            }
        } else {
            // Also apply the frame's minimum height
            wantedPosition = qMax( wantedPosition, theFrame->top() + theFrame->minimumFrameHeight() );
            if ( wantedPosition != theFrame->bottom()) {
#ifdef DEBUG_FORMAT_MORE
                kDebug() << "    the frame was " << *theFrame << endl;
                kDebug() << "setBottom " << wantedPosition << endl;
#endif
                theFrame->setBottom( wantedPosition );
#ifdef DEBUG_FORMAT_MORE
                kDebug() << "    -> the frame is now " << *theFrame << endl;
#endif
                frameResized( theFrame, true );
            }
        }
    }
}

void KWTextFrameSet::slotAfterFormatting( int bottom, KoTextParag *lastFormatted, bool* abort )
{
    int availHeight = availableHeight();
    if ( ( bottom > availHeight ) ||   // this parag is already off page
         ( lastFormatted && bottom + lastFormatted->rect().height() > availHeight ) ) // or next parag will be off page
    {
        *abort = slotAfterFormattingNeedMoreSpace( bottom, lastFormatted );
    }
    // Handle the case where the last frame is empty, so we may want to
    // remove the last page.
    else if ( m_frames.count() > 1 && !lastFormatted && frameSetInfo() == KWFrameSet::FI_BODY
              && bottom < availHeight - m_doc->ptToLayoutUnitPixY( m_frames.last()->innerHeight() ) )
    {
#ifdef DEBUG_FORMAT_MORE
        kDebug(32002) << "slotAfterFormatting too much space (bottom=" << bottom << ", availHeight=" << availHeight << ") , trying to remove last frame" << endl;
#endif
        // Remove the empty last frame, if it's an auto-created one (e.g. a
        // continuation on the next page). Not when the user just created it!
        if(m_frames.last()->frameBehavior() == KWFrame::AutoExtendFrame
           && m_frames.last()->minimumFrameHeight() < 1E-10 ) { // i.e. equal to 0
            deleteFrame(m_frames.last(), true);
            m_doc->frameChanged( 0L );
        }
        if ( m_doc->processingType() == KWDocument::WP ) {
            bool removed = m_doc->tryRemovingPages();
            // Do all the recalc in one go. Speeds up deleting many pages.
            if ( removed )
                m_doc->afterRemovePages();
        }
    }
    // Handle the case where the last frame is in AutoExtendFrame mode
    // and there is less text than space
    else if ( !lastFormatted && bottom + 2 < availHeight &&
              (m_frames.last()->frameBehavior() == KWFrame::AutoExtendFrame&& !isProtectSize()) )
    {
        slotAfterFormattingTooMuchSpace( bottom );
        *abort = false;
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

// This is called when a text frame with behaviour AutoCreateNewFrame
// has more text than available frame height, so we need to create a new page
// so that a followup frame is created for this one
bool KWTextFrameSet::createNewPageAndNewFrame( KoTextParag* lastFormatted, int /*difference*/ )
{
    KWFrame* lastFrame = m_frames.last();
    // This is only going to help us if the new frame is reconnected. Otherwise bail out.
    if ( !lastFrame || lastFrame->newFrameBehavior() != KWFrame::Reconnect )  {
        kDebug(32002) << name() << " : frame is AutoCreateNewFrame but not Reconnect !?!? Aborting." << endl;
        m_textobj->setLastFormattedParag( 0 );
        return true; // abort
    }

//#ifdef DEBUG_FORMAT_MORE
    kDebug(32002) << "createNewPageAndNewFrame creating new frame in frameset " << name() << endl;
//#endif
    uint oldCount = m_frames.count();
    int lastPageNumber = m_doc->pageManager()->lastPageNumber();
    kDebug(32002) << " last frame=" << lastFrame << " pagenum=" << lastFrame->pageNumber() << " lastPageNumber=" << lastPageNumber << "   m_frames count=" << oldCount << endl;

    // First create a new page for it if necessary
    if ( lastFrame->pageNumber() == lastPageNumber )
    {
        // Let's first check if it will give us more space than we
        // already have left in this page. Otherwise we'll loop infinitely.

        int heightWeWillGet = 0; // in LU
        if(isMainFrameset()) // is never added in the framesToCopyOnNewPage
            heightWeWillGet += m_doc->ptToLayoutUnitPixY( m_frames.last()->height() );
        else {
            Q3PtrList<KWFrame> framesToCopy = m_doc->framesToCopyOnNewPage( lastPageNumber );
            Q3PtrListIterator<KWFrame> frameIt( framesToCopy );
            for ( ; frameIt.current(); ++frameIt )
                if (frameIt.current()->frameSet() == this &&
                    frameIt.current()->newFrameBehavior()==KWFrame::Reconnect)
                    heightWeWillGet += m_doc->ptToLayoutUnitPixY( frameIt.current()->height() );
        }

        // This logic doesn't applies to tables though, since they can be broken over multiple pages
        // TODO: lastFormatted->containsTable() or so (containsPageBreakableItem rather).

        // "difference" doesn't apply if we're pasting multiple paragraphs.
        // We want to compare the height of one paragraph, not all the missing height.
        KoTextParag* parag = lastFormatted ? lastFormatted : textDocument()->lastParag();
        // In fact the parag height isn't the right thing to test for - we should check
        // for the highest character that remains to be positioned.
        // Testcase: many big inline pictures in one paragraph.
        int paragHeight = parag->rect().height();
        kDebug(32002) << "height we will get in the new page:" << heightWeWillGet << " parag " << parag << " height:" << paragHeight << endl;
        if ( heightWeWillGet < paragHeight && !m_groupmanager )
        {
            kDebug(32002) << "not enough height on the new page, not worth it" << endl;
            m_textobj->setLastFormattedParag( 0 );
            return true; // abort
        }

        KWPage *page = m_doc->appendPage();
        if (  !m_doc->isLoading() )
            m_doc->afterInsertPage( page->pageNumber() );
        kDebug(32002) << "now frames count=" << m_frames.count() << endl;
    }

    // Maybe creating the new page created the frame in this frameset, then we're done
    // Otherwise let's create it ourselves:
    if ( m_frames.count() == oldCount )
    {
        Q_ASSERT( !isMainFrameset() ); // ouch, should have gone to the appendPage case above...
        // Otherwise, create a new frame on next page
        kDebug(32002) << "createNewPageAndNewFrame creating frame on page " << lastFrame->pageNumber()+1 << endl;
        KWFrame *frm = lastFrame->getCopy();
        frm->moveBy( 0, m_doc->pageManager()->page(frm)->height() );
        addFrame( frm );
    }

    updateFrames();
    Q_ASSERT(frame(0) && frame(0)->frameStack());
    frame(0)->frameStack()->update();
    /// We don't want to start from the beginning every time !
    ////m_doc->invalidate();

    // Reformat the last paragraph. If it's over the two pages, it will need
    // the new page (e.g. for inline frames that need internalToDocument to work)
    if ( lastFormatted )
        lastFormatted = lastFormatted->prev();
    else
        lastFormatted = textDocument()->lastParag();

    if ( lastFormatted )
    {
        m_textobj->setLastFormattedParag( lastFormatted );
        lastFormatted->invalidate( 0 );
        //This was a way to format the rest from here (recursively), but it didn't help much ensureCursorVisible()
        //So instead I fixed formatMore to return formatMore(2) itself.
        //m_textobj->formatMore( 2 );
        return false; // keep going
    }
    m_doc->delayedRepaintAllViews();
    return false; // all done
}

double KWTextFrameSet::footNoteSize( KWFrame *theFrame )
{
    double tmp =0.0;
    int page = theFrame->pageNumber();
    Q3PtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        if((fit.current()->isFootNote() || fit.current()->isEndNote()) &&
           fit.current()->isVisible())
        {
            KWFrame * frm=fit.current()->frame( 0 );
            if(frm->pageNumber()==page )
                tmp += frm->innerHeight()+m_doc->ptFootnoteBodySpacing();
        }
    }
    return tmp;
}


double KWTextFrameSet::footerHeaderSizeMax( KWFrame *theFrame )
{
    KWPage *page = m_doc->pageManager()->page(theFrame);
    Q_ASSERT( page );
    if ( !page )
        return 0;
    double tmp = page->height() - page->bottomMargin() - page->topMargin() - 40;//default min 40 for page size
    bool header=theFrame->frameSet()->isAHeader();
    if( header ? m_doc->isHeaderVisible():m_doc->isFooterVisible() )
    {
        Q3PtrListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
        for ( ; fit.current() ; ++fit )
        {
            bool state = header ? fit.current()->isAFooter():fit.current()->isAHeader();
            if(fit.current()->isVisible() && state)
            {
                KWFrame * frm=fit.current()->frame( 0 );
                if(frm->pageNumber()==page->pageNumber() )
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
    kDebug(32002) << "KWTextFrameSet::frameResized " << theFrame << " " << *theFrame << " invalidateLayout=" << invalidateLayout << endl;
    if ( theFrame->height() < 0 )
        return; // safety!

    KWFrameSet * fs = theFrame->frameSet();
    Q_ASSERT( fs == this );
    fs->updateFrames(); // update e.g. available height
    Q_ASSERT(frame(0) && frame(0)->frameStack());
    frame(0)->frameStack()->update();

    theFrame->updateRulerHandles();

    // Do a full KWFrameLayout if this will have influence on other frames, i.e.:
    // * if we resized the last main text frame (the one before the first endnote)
    // * if we resized an endnote
    // Delay it though, to get the full height first.
    if ( fs->isMainFrameset() || fs->isEndNote() )
        m_doc->delayedRecalcFrames( theFrame->pageNumber() );
    // * if we resized a header, footer, or footnote
    else if ( fs->frameSetInfo() != KWFrameSet::FI_BODY )
        m_doc->recalcFrames( theFrame->pageNumber(), -1 ); // warning this can delete theFrame!

    // m_doc->frameChanged( theFrame );
    // Warning, can't call layout() (frameChanged calls it)
    // from here, since it calls formatMore() !
    if ( invalidateLayout )
        m_doc->invalidate(this);

    // Can't repaint directly, we might be in a paint event already
    m_doc->delayedRepaintAllViews();
}

bool KWTextFrameSet::isFrameEmpty( KWFrame * theFrame )
{
    KoTextParag * lastParag = textDocument()->lastParag();
    // The problem is that if we format things here, and don't emit afterFormatting,
    // we won't resize autoresize frames properly etc. (e.g. endnotes)
    // Testcase for this problem: werner's footnote-1.doc
    //ensureFormatted( lastParag, false ); // maybe true here would do too? slow if maintextframeset though.
    if ( !lastParag->isValid() )
        return false; // we don't know yet
    int bottom = lastParag->rect().top() + lastParag->rect().height();

    if ( theFrame->frameSet() == this ) // safety check
    {
        //kDebug() << "KWTextFrameSet::isFrameEmpty text bottom=(LU) " << bottom << " theFrame=" << theFrame << " " << *theFrame << " its internalY(LU)=" << m_doc->ptToLayoutUnitPixY( theFrame->internalY() ) << endl;
        return bottom < m_doc->ptToLayoutUnitPixY( theFrame->internalY() );
    }

    kWarning() << "KWTextFrameSet::isFrameEmpty called for frame " << theFrame << " which isn't a child of ours!" << endl;
    if ( theFrame->frameSet()!=0L && theFrame->frameSet()->name()!=0L)
        kDebug() << "(this is " << name() << " and the frame belongs to " << theFrame->frameSet()->name() << ")" << endl;
    return false;
}

bool KWTextFrameSet::canRemovePage( int num )
{
    //kDebug() << "KWTextFrameSet(" << name() << ")::canRemovePage " << num << endl;

    // No frame on that page ? ok for us then
    if ( num < m_firstPage || num >= (int)m_framesInPage.size() + m_firstPage ) {
        //kDebug() << "No frame on that page. Number of frames: " << frameCount() << endl;
        return true;
    }

    Q3PtrListIterator<KWFrame> frameIt( framesInPage( num ) );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame * theFrame = frameIt.current();
        //kDebug() << "canRemovePage: looking at " << theFrame << " pageNum=" << theFrame->pageNumber() << endl;
        Q_ASSERT( theFrame->pageNumber() == num );
        Q_ASSERT( theFrame->frameSet() == this );
        bool isEmpty = isFrameEmpty( theFrame );
        //kDebug() << "KWTextFrameSet(" << name() << ")::canRemovePage"
        //          << " found a frame on page " << num << " empty:" << isEmpty << endl;
        // Ok, so we have a frame on that page -> we can't remove it unless it's a copied frame OR it's empty
        bool isCopy = theFrame->isCopy() && frameIt.current() != m_frames.first();
        if ( !isCopy && !isEmpty )
            return false;
    }
    return true;
}

void KWTextFrameSet::deleteFrame( unsigned int num, bool remove, bool recalc )
{
    KWFrame *frm = m_frames.at( num );
    kDebug() << "KWTextFrameSet(" << name() << ")::deleteFrame " << frm << " (" << num << ")" << endl;
    if ( frm )
        emit frameDeleted( frm );
    KWFrameSet::deleteFrame( num, remove, recalc );
}

void KWTextFrameSet::updateViewArea( QWidget * w, KWViewMode* viewMode, const QPoint & nPointBottom )
{
    if (!isVisible(viewMode))
        return;
    int ah = availableHeight(); // make sure that it's not -1
#ifdef DEBUG_VIEWAREA
    kDebug(32002) << "KWTextFrameSet::updateViewArea " << (void*)w << " " << w->name()
                     << " nPointBottom=" << nPointBottom.x() << "," << nPointBottom.y()
                     << " availHeight=" << ah << " textDocument()->height()=" << textDocument()->height() << endl;
#endif

    // Find last page that is visible
    int maxPage = m_doc->pageManager()->pageNumber(m_doc->unzoomItY( nPointBottom.y() ));
    int maxY = 0;
    if ( maxPage < m_firstPage || maxPage >= (int)m_framesInPage.size() + m_firstPage )
        maxY = ah;
    else
    {
        // Find frames on that page, and keep the max bottom, in internal coordinates
        Q3PtrListIterator<KWFrame> frameIt( framesInPage( maxPage ) );
        for ( ; frameIt.current(); ++frameIt )
        {
            maxY = qMax( maxY, m_doc->ptToLayoutUnitPixY( frameIt.current()->internalY() + frameIt.current()->innerHeight() ) );
        }
    }
#ifdef DEBUG_VIEWAREA
    kDebug(32002) << "KWTextFrameSet (" << name() << ")::updateViewArea maxY now " << maxY << endl;
#endif
    m_textobj->setViewArea( w, maxY );
    m_textobj->formatMore( 2 );
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

    m_textobj->formatMore( 2 );
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

KCommand * KWTextFrameSet::pasteOasis( KoTextCursor * cursor, const QByteArray & data, bool removeSelected )
{
    if (protectContent() )
        return 0;

    kDebug(32001) << "KWTextFrameSet::pasteOasis data:" << data.size() << " bytes" << endl;
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Paste") );
    if ( removeSelected && textDocument()->hasSelection( KoTextDocument::Standard ) )
        macroCmd->addCommand( m_textobj->removeSelectedTextCommand( cursor, KoTextDocument::Standard ) );
    m_textobj->emitHideCursor();
    m_textobj->setLastFormattedParag( cursor->parag()->prev() ?
                           cursor->parag()->prev() : cursor->parag() );

    KWOasisPasteCommand * cmd = new KWOasisPasteCommand( textDocument(), cursor->parag()->paragId(), cursor->index(), data );
    textDocument()->addCommand( cmd );

    macroCmd->addCommand( new KoTextCommand( m_textobj, /*cmd, */QString::null ) );

    *cursor = *( cmd->execute( cursor ) );

    // not enough when pasting many pages. We need the cursor's parag to be formatted.
    //m_textobj->formatMore( 2 );
    ensureFormatted( cursor->parag() );

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
    m_textobj->formatMore( 2 );
    emit repaintChanged( this );
    m_textobj->emitEnsureCursorVisible();
    m_textobj->emitUpdateUI( true );
    m_textobj->emitShowCursor();

    m_doc->addCommand( macroCmd );
}

KNamedCommand* KWTextFrameSet::insertFrameBreakCommand( KoTextCursor *cursor )
{
    KMacroCommand* macroCmd = new KMacroCommand( QString::null );
    macroCmd->addCommand( m_textobj->insertParagraphCommand( cursor ) );
    KWTextParag *parag = static_cast<KWTextParag *>( cursor->parag() );
    if(parag->prev()) {
        parag=static_cast<KWTextParag *> (parag->prev());
        cursor->setParag( parag );
        cursor->setIndex( parag->length() - 1 );
    }
    macroCmd->addCommand( setPageBreakingCommand( cursor, parag->pageBreaking() | KoParagLayout::HardFrameBreakAfter ) );
    Q_ASSERT( parag->next() );
    if ( parag->next() ) {
        cursor->setParag( parag->next() );
        cursor->setIndex( 0 );
    }
    return macroCmd;
}

void KWTextFrameSet::insertFrameBreak( KoTextCursor *cursor )
{
    clearUndoRedoInfo();
    m_textobj->emitHideCursor();
    KNamedCommand* cmd = insertFrameBreakCommand( cursor );
    cmd->setName( i18n( "Insert Break After Paragraph" ) );
    m_doc->addCommand( cmd );

    m_textobj->setLastFormattedParag( cursor->parag() );
    m_textobj->formatMore( 2 );
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
    Q3PtrListIterator<KWFrame> frameIt( framesInPage( pageNum ) );
    if ( !frameIt.current() )
        return false;

    // Look at all frames in the page, and keep min and max "internalY" positions
    double topPt = frameIt.current()->internalY();
    double bottomPt = topPt + frameIt.current()->height();

    for ( ; frameIt.current(); ++frameIt )
    {
        double y = frameIt.current()->internalY();
        topPt = qMin( topPt, y );
        bottomPt = qMax( bottomPt, y + frameIt.current()->height() );
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
    kDebug() << "KWTextFrameSet::deleteAnchoredFrame anchor->index=" << anchor->index() << endl;
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

QString KWTextFrameSet::toPlainText() const
{
    return m_textobj->textDocument()->plainText();
}

void KWTextFrameSet::highlightPortion( KoTextParag * parag, int index, int length, KWCanvas * canvas, bool repaint, KDialogBase* dialog )
{
    Q_ASSERT( isVisible() );
    Q_ASSERT( m_textobj->isVisible() );
    //kDebug() << "highlighting in " << name() << " parag=" << parag->paragId() << " index=" << index << " repaint=" << repaint << endl;
    m_textobj->highlightPortion( parag, index, length, repaint );
    if ( repaint ) {
        // Position the cursor
        canvas->editTextFrameSet( this, parag, index );
        // Ensure text is fully visible
        QRect expose = canvas->viewMode()->normalToView( paragRect( parag ) );
        canvas->ensureVisible( (expose.left()+expose.right()) / 2,  // point = center of the rect
                               (expose.top()+expose.bottom()) / 2,
                               (expose.right()-expose.left()) / 2,  // margin = half-width of the rect
                               (expose.bottom()-expose.top()) / 2);
        if ( dialog ) {
            //kDebug() << k_funcinfo << " dialog=" << dialog << " avoiding rect=" << expose << endl;
            QRect globalRect( expose );
            globalRect.moveTopLeft( canvas->mapToGlobal( globalRect.topLeft() ) );
            KDialog::avoidArea( dialog, globalRect );
        }
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

void KWTextFrameSet::applyStyleChange( KoStyleChangeDefMap changed )
{
    m_textobj->applyStyleChange( changed );
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


KCommand *KWTextFrameSet::setFormatCommand( const KoTextFormat * newFormat, int flags, bool zoomFont )
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

bool KWTextFrameSet::rtl() const
{
    return m_textobj->rtl();
}


KCommand *KWTextFrameSet::setParagLayoutFormatCommand( KoParagLayout *newLayout,int flags, int marginIndex)
{
    return m_textobj->setParagLayoutFormatCommand(newLayout, flags, marginIndex);
}

class KWFootNoteVarList : public Q3PtrList< KWFootNoteVariable >
{
protected:
    virtual int compareItems(Q3PtrCollection::Item a, Q3PtrCollection::Item b)
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
    Q3PtrListIterator<KoTextCustomItem> cit( textDocument()->allCustomItems() );
    for ( ; cit.current() ; ++cit )
    {
        KWFootNoteVariable *fnv = dynamic_cast<KWFootNoteVariable *>( cit.current() );
        if (fnv && !fnv->isDeleted() && (fnv->frameSet() && !fnv->frameSet()->isDeleted()))
            lst.append( fnv );
    }
    lst.sort();
    short int footNoteVarNumber = 0; // absolute order number [internal, not saved nor displayed]
    short int endNoteVarNumber = 0;
    short int footNoteNumDisplay = 1; // the number being displayed
    short int endNoteNumDisplay = 1;
    bool needRepaint = false;
    Q3PtrListIterator< KWFootNoteVariable > vit( lst );

    //create a list with all manual footnotes numbers
    Q3ValueList<int> addedNums;
    for ( ; vit.current() ; ++vit )
    {
        KWFootNoteVariable* var = vit.current();
        if ( var->numberingType()==KWFootNoteVariable::Manual )
        {
            uint const num = var->text().toUInt();
            if ( num != 0 )
                addedNums.append( num );
        }
    }

    for ( vit.toFirst() ; vit.current() ; )
    {
        KWFootNoteVariable* var = vit.current();
        bool endNote = var->noteType() == EndNote;
        short int & varNumber = endNote ? endNoteVarNumber : footNoteVarNumber;
        short int & numDisplay = endNote ? endNoteNumDisplay : footNoteNumDisplay;
        ++varNumber;
        bool changed = false;
        if ( varNumber != var->num() || var->numberingType()==KWFootNoteVariable::Manual )
        {
            changed = true;
            var->setNum( varNumber );
        }
        if ( var->numberingType()==KWFootNoteVariable::Auto )
        {
            if ( addedNums.contains( numDisplay ) != 0 ) // the automatic generated number should not be equal to a manual one
            {
                numDisplay++;
                continue; //try with the next number
            }
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
                QString fsName = endNote ? i18n("Endnote %1") : i18n("Footnote %1");
                if ( var->numberingType()== KWFootNoteVariable::Manual)
                    var->frameSet()->setName( m_doc->generateFramesetName(fsName));
                else
                    var->frameSet()->setName( fsName.arg( var->text() ) );
                var->frameSet()->setCounterText( var->text() );
            }
            var->resize();
            var->paragraph()->invalidate(0);
            var->paragraph()->setChanged( true );
            needRepaint = true;
        }
        ++vit;
    }
    if ( needRepaint && repaint )
        m_doc->slotRepaintChanged( this );
}

KoTextDocCommand *KWTextFrameSet::deleteTextCommand( KoTextDocument *textdoc, int id, int index, const Q3MemArray<KoTextStringChar> & str, const CustomItemsMap & customItemsMap, const Q3ValueList<KoParagLayout> & oldParagLayouts )
{
    return new KWTextDeleteCommand( textdoc, id, index, str, customItemsMap, oldParagLayouts );
}

QByteArray KWTextFrameSet::sortText(SortType type) const
{
    const KoTextCursor c1 = textDocument()->selectionStartCursor(KoTextDocument::Standard );
    const KoTextCursor c2 = textDocument()->selectionEndCursor( KoTextDocument::Standard );
    if ( c1.parag() == c2.parag() )
        return QByteArray();
    else
    {
        // ( paragraph text -> paragraph ) map. Note that this sorts on the key automatically.
        QMap<QString, const KoTextParag*> sortMap;
        sortMap.insert( c1.parag()->toString(0), c1.parag() );

        const KoTextParag *p = c1.parag()->next();
        while ( p && p != c2.parag() ) {
            sortMap.insert( p->toString(0), p );
            p = p->next();
        }
        sortMap.insert( c2.parag()->toString(0), c2.parag());

        typedef Q3ValueList<const KoTextParag *> ParagList;
        ParagList sortedParags = sortMap.values();
        if ( type == KW_SORTDECREASE )
        {
            // I could use an STL algorithm here, but only if Qt was compiled with STL support...
            ParagList newList;
            for ( ParagList::const_iterator it = sortedParags.begin(),
                                           end = sortedParags.end();
                  it != end ; ++it ) {
                newList.prepend( *it );
            }
            sortedParags = newList;
        }

        KWOasisSaver oasisSaver( m_doc );
        oasisSaver.saveParagraphs( sortedParags );
        if ( !oasisSaver.finish() )
            return QByteArray();
        return oasisSaver.data();
    }
}

// This is used when loading (KWTextDocument::loadOasisFootnote)
// and when inserting from the GUI (KWTextFrameSetEdit::insertFootNote),
// so don't add any 'repaint' or 'recalc' code here
KWFootNoteFrameSet * KWTextFrameSet::insertFootNote( NoteType noteType, KWFootNoteVariable::Numbering numType, const QString &manualString )
{
     kDebug() << "KWTextFrameSetEdit::insertFootNote " << endl;
     KWDocument * doc = m_doc;
     KWFootNoteVariable * var = new KWFootNoteVariable( textDocument(), doc->variableFormatCollection()->format( "NUMBER" ), doc->variableCollection(), doc );
     var->setNoteType( noteType );
     var->setNumberingType( numType );
     if ( numType == KWFootNoteVariable::Manual )
         var->setManualString( manualString );

     // Now create text frameset which will hold the variable's contents
     KWFootNoteFrameSet *fs = new KWFootNoteFrameSet( doc, i18n( "Footnotes" ) );
     fs->setFrameSetInfo( KWFrameSet::FI_FOOTNOTE );

     doc->addFrameSet( fs );

     // Bind the footnote variable and its text frameset
     var->setFrameSet( fs );
     fs->setFootNoteVariable( var );

     return fs;
}

KoVariable* KWTextFrameSet::variableUnderMouse( const KoPoint& dPoint )
{
    QPoint iPoint;
    if ( documentToInternal( dPoint, iPoint ) )
        return textObject()->variableAtPoint( iPoint );
    return 0;
}

KoLinkVariable* KWTextFrameSet::linkVariableUnderMouse( const KoPoint& dPoint )
{
    QPoint iPoint;
    if ( documentToInternal( dPoint, iPoint ) )
    {
        KoLinkVariable* linkVariable = dynamic_cast<KoLinkVariable *>( textObject()->variableAtPoint( iPoint ) );
        return linkVariable;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

KWTextFrameSetEdit::KWTextFrameSetEdit( KWTextFrameSet * fs, KWCanvas * canvas )
    : KoTextView( fs->textObject() ), KWFrameSetEdit( fs, canvas ), m_rtl( false )
{
    setBackSpeller( fs->kWordDocument()->backSpeller() );
    //kDebug(32001) << "KWTextFrameSetEdit::KWTextFrameSetEdit " << fs->name() << endl;
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

    setOverwriteMode( canvas->overwriteMode() );
}

KWTextFrameSetEdit::~KWTextFrameSetEdit()
{
    //kDebug(32001) << "KWTextFrameSetEdit::~KWTextFrameSetEdit" << endl;
    //m_canvas->gui()->getHorzRuler()->changeFlags(0);
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

void KWTextFrameSetEdit::paste(QClipboard::Mode mode)
{
    const QMimeData *data = QApplication::clipboard()->mimeData(mode);
    int provides = KWView::checkClipboard( data );
    pasteData( data, provides, false );
}

void KWTextFrameSetEdit::pasteData( const QMimeData* data, int provides, bool drop )
{
    if ( provides & KWView::ProvidesOasis )
    {
        KCommand* cmd = pasteOasisCommand( data );
        if ( cmd )
            frameSet()->kWordDocument()->addCommand(cmd);
    }
    else if ( provides & KWView::ProvidesPlainText )
    {
        // Note: QClipboard::text() seems to do a better job than encodedData( "text/plain" )
        // In particular it handles charsets (in the mimetype).
        const QString text = QApplication::clipboard()->text();
        const bool removeSelected = !drop;
        if ( !text.isEmpty() )
            textObject()->pasteText( cursor(), text, currentFormat(), removeSelected );
    }
    else {
        kWarning(32002) << "Unhandled case in KWTextFrameSetEdit::pasteData: provides=" << provides << endl;
    }
    // be sure that the footnote number didn't got erased
    KWFootNoteFrameSet *footNote = dynamic_cast<KWFootNoteFrameSet *>(textFrameSet());
    if ( footNote )
    {
        KoParagCounter *counter = footNote->textDocument()->firstParag()->counter();
        if ( !counter || ( counter->numbering() != KoParagCounter::NUM_FOOTNOTE ) )
            footNote->setCounterText( footNote->footNoteVariable()->text() );
            frameSet()->kWordDocument()->slotRepaintChanged( frameSet() );
    }
}

KCommand* KWTextFrameSetEdit::pasteOasisCommand( const QMimeData* mimeData )
{
    // Find which mimetype it was (could be oasis text, oasis presentation etc.)
    QString returnedTypeMime = KoTextObject::providesOasis( mimeData );
    if ( !returnedTypeMime.isEmpty() )
    {
        QByteArray arr = mimeData->data( returnedTypeMime );
        Q_ASSERT( !arr.isEmpty() );
        if ( arr.size() )
            return textFrameSet()->pasteOasis( cursor(), arr, true );
    }
    return 0;
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
        Q3DragObject *drag = newDrag( 0 );
        QApplication::clipboard()->setData( drag, QClipboard::Clipboard );
    }
}

bool KWTextFrameSetEdit::doIgnoreDoubleSpace(KoTextParag * parag,
        int index,QChar ch )
{
    if( textFrameSet()->kWordDocument()->allowAutoFormat())
    {
        KoAutoFormat * autoFormat = textFrameSet()->kWordDocument()->autoFormat();
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
        KoAutoFormat * autoFormat = textFrameSet()->kWordDocument()->autoFormat();
        if( autoFormat )
            autoFormat->doAutoFormat( cursor, parag, index, ch, textObject());
    }
}

bool KWTextFrameSetEdit::doCompletion( KoTextCursor* cursor, KoTextParag *parag, int index )
{
    if( textFrameSet()->kWordDocument()->allowAutoFormat() )
    {
        KoAutoFormat * autoFormat = textFrameSet()->kWordDocument()->autoFormat();
        if( autoFormat )
            return autoFormat->doCompletion(  cursor, parag, index, textObject());
    }
    return false;
}

bool KWTextFrameSetEdit::doToolTipCompletion( KoTextCursor* cursor, KoTextParag *parag, int index, int keyPressed )
{
    if( textFrameSet()->kWordDocument()->allowAutoFormat() )
    {
        KoAutoFormat * autoFormat = textFrameSet()->kWordDocument()->autoFormat();
        if( autoFormat )
            return autoFormat->doToolTipCompletion(  cursor, parag, index, textObject(), keyPressed);
    }
    return false;
}

void KWTextFrameSetEdit::showToolTipBox(KoTextParag *parag, int index, QWidget *widget, const QPoint &pos)
{
    if( textFrameSet()->kWordDocument()->allowAutoFormat() )
    {
        KoAutoFormat * autoFormat = textFrameSet()->kWordDocument()->autoFormat();
        if( autoFormat )
            autoFormat->showToolTipBox(parag, index, widget, pos);
    }
}

void KWTextFrameSetEdit::removeToolTipCompletion()
{
    if( textFrameSet()->kWordDocument()->allowAutoFormat() )
    {
        KoAutoFormat * autoFormat = textFrameSet()->kWordDocument()->autoFormat();
        if( autoFormat )
            autoFormat->removeToolTipCompletion();
    }
}

void KWTextFrameSetEdit::textIncreaseIndent()
{
    kDebug(32001) << "Increasing list" << endl;
    m_canvas->gui()->getView()->textIncreaseIndent();
}

bool KWTextFrameSetEdit::textDecreaseIndent()
{
    if (currentLeftMargin()>0)
    {
        kDebug(32001) << "Decreasing list" << endl;
        m_canvas->gui()->getView()->textDecreaseIndent();
        return true;
    }
    else
        return false;
}

void KWTextFrameSetEdit::startDrag()
{
    textView()->dragStarted();
    m_canvas->dragStarted();
    Q3DragObject *drag = newDrag( m_canvas->viewport() );
    if ( !frameSet()->kWordDocument()->isReadWrite() )
        drag->dragCopy();
    else {
        bool move = ( drag->drag() );
        if ( move )
        {
#if 0
            if ( Q3DragObject::target() != m_canvas && Q3DragObject::target() != m_canvas->viewport() ) {
                //This is when dropping text _out_ of KWord. Since we have Move and Copy
                //options (Copy being accessed by pressing CTRL), both are possible.
                //But is that intuitive enough ? Doesn't the user expect a Copy in all cases ?
                //Losing the selected text when dropping out of kword seems quite unexpected to me.
                //Undecided about this........
                textObject()->removeSelectedText( cursor() );
            }
#endif
        }
    }
}

Q3DragObject * KWTextFrameSetEdit::newDrag( QWidget * parent )
{
    KWTextFrameSet* fs = textFrameSet();
    return fs->kWordDocument()->dragSelected( parent, fs );
}

void KWTextFrameSetEdit::ensureCursorVisible()
{
    //kDebug() << "KWTextFrameSetEdit::ensureCursorVisible paragId=" << cursor()->parag()->paragId() << " cursor->index()=" << cursor()->index() << endl;
    KoTextParag * parag = cursor()->parag();
    int idx = cursor()->index();
    textFrameSet()->ensureFormatted( parag );
    KoTextStringChar *chr = parag->at( idx );
    int cursorHeight = parag->lineHeightOfChar( idx );
    int x = parag->rect().x() + cursor()->x(); // this includes +charwidth for an RTL char
    //kDebug() << "parag->rect().x()=" << parag->rect().x() << " x=" << cursor()->x() << endl;
    int y = 0; int dummy;
    parag->lineHeightOfChar( idx, &dummy, &y );
    y += parag->rect().y();
    //kDebug() << "KWTextFrameSetEdit::ensureCursorVisible y=" << y << endl;
    // make sure one char is visible before, and one after
    KoTextStringChar *chrLeft = idx > 0 ? chr-1 : chr;
    // which char is on the left and which one is on the right depends on chr->rightToLeft
    int areaLeft = chr->rightToLeft ? chr->width : chrLeft->width;
    int areaRight = chr->rightToLeft ? chrLeft->width : chr->width;
    KoPoint pt;
    KoPoint hintDPoint;
    if ( m_currentFrame )
        hintDPoint = m_currentFrame->topLeft();
    KWFrame * theFrame = textFrameSet()->internalToDocumentWithHint( QPoint(x, y), pt, hintDPoint );
    //kDebug() << "KWTextFrameSetEdit::ensureCursorVisible frame=" << theFrame << " m_currentFrame=" << m_currentFrame << endl;
    if ( theFrame && m_currentFrame != theFrame )
    {
        m_currentFrame = theFrame;
        m_canvas->gui()->getView()->updatePageInfo();
    }
    QPoint cursorPos = textFrameSet()->kWordDocument()->zoomPoint( pt );
    cursorPos = m_canvas->viewMode()->normalToView( cursorPos );
    areaLeft = textFrameSet()->kWordDocument()->layoutUnitToPixelX( areaLeft ) + 1;
    areaRight = textFrameSet()->kWordDocument()->layoutUnitToPixelX( areaRight ) + 1;
    cursorHeight = textFrameSet()->kWordDocument()->layoutUnitToPixelY( cursorHeight );
    //kDebug() << "KWTextFrameSetEdit::ensureCursorVisible pt=" << pt << " cursorPos=" << cursorPos
    //          << " areaLeft=" << areaLeft << " areaRight=" << areaRight << " y=" << y << endl;
    m_canvas->ensureVisible( cursorPos.x() - areaLeft, cursorPos.y() + cursorHeight / 2, areaLeft + areaRight, cursorHeight / 2 + 2 );
}

bool KWTextFrameSetEdit::enterCustomItem( KoTextCustomItem* customItem, bool fromRight )
{
    KWAnchor* anchor = dynamic_cast<KWAnchor*>( customItem );
    if ( anchor ) {
        KWFrameSet* frameSet = anchor->frameSet();
        if ( frameSet->type() == FT_FORMULA || frameSet->type() == FT_TEXT ) {

            // store the instance variable we need after "delete this"
            KWCanvas* canvas = m_canvas;

            // this will "delete this"!
            m_canvas->editFrameSet( frameSet );

            // We assume that `editFrameSet' succeeded.
            if ( fromRight ) {
                KWFrameSetEdit* edit = canvas->currentFrameSetEdit();
                if ( frameSet->type() == FT_FORMULA )
                    static_cast<KWFormulaFrameSetEdit*>( edit )->moveEnd();
                else
                    static_cast<KWTextFrameSetEdit*>( edit )->moveCursor( MoveEnd );
            }

            if ( frameSet->type() == FT_FORMULA )
            {
                // A FormulaFrameSetEdit looks a little different from
                // a FormulaFrameSet. (Colors)
                static_cast<KWFormulaFrameSet*>( frameSet )->setChanged();
                canvas->repaintChanged( frameSet, true );
            }
            return true;
        }
    }
    return false;
}

void KWTextFrameSetEdit::keyPressEvent( QKeyEvent* e )
{
    // Handle moving into inline frames (e.g. formula frames).
    if ( !( e->state() & Qt::ControlModifier ) && !( e->state() & Qt::ShiftModifier ) )
    {
        if (e->state() != Qt::NoButton)
                removeToolTipCompletion();
        switch ( e->key() ) {
        case Qt::Key_Left: {
            KoTextCursor* cursor = textView()->cursor();
            KoTextParag* parag = cursor->parag();
            int index = cursor->index();
            if ( index > 0 ) {
                KoTextStringChar* ch = parag->at( index-1 );
                if ( ch->isCustom() ) {
                    KoTextCustomItem* customItem = ch->customItem();
                    if ( enterCustomItem( customItem, true ) ) {
                        // Don't do anything here, "this" is deleted!
                        return;
                    }
                }
            }
            if ( index == 0 && !parag->prev() )
                if ( exitLeft() )
                    return;
            break;
        }
        case Qt::Key_Right: {
            KoTextCursor* cursor = textView()->cursor();
            KoTextParag* parag = cursor->parag();
            int index = cursor->index();
            if ( index < parag->length() - 1 ) {
                KoTextStringChar* ch = parag->at( index );
                if ( ch->isCustom() ) {
                    KoTextCustomItem* customItem = ch->customItem();
                    if ( enterCustomItem( customItem, false ) ) {
                        // Don't do anything here, "this" is deleted!
                        return;
                    }
                }
            } else if ( /*at end, covered by previous if, && */ !parag->next() )
                if ( exitRight() )
                    return;
            break;
        }
        }
    }
    // Calculate position of tooltip for autocompletion
    QPoint pos = textFrameSet()->cursorPos( cursor(), m_canvas, m_currentFrame );
    textView()->handleKeyPressEvent( e, m_canvas, pos );
}

void KWTextFrameSetEdit::keyReleaseEvent( QKeyEvent* e )
{
    textView()->handleKeyReleaseEvent( e );
}

void KWTextFrameSetEdit::inputMethodEvent( QInputMethodEvent* e )
{
    textView()->handleInputMethodEvent( e );
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
    KWFrame * theFrame = textFrameSet()->documentToInternalMouseSelection( dPoint, iPoint, relPos, m_canvas->viewMode() );
    if ( theFrame && m_currentFrame != theFrame )
    {
        m_currentFrame = theFrame;
        m_canvas->gui()->getView()->updatePageInfo();
    }

    if ( m_currentFrame )
    {
        // Let KoTextView handle the mousepress event - but don't let it start
        // a drag if clicking on the left of the text (out of the frame itself)
        bool addParag = textView()->handleMousePressEvent( e, iPoint, relPos != KWTextFrameSet::LeftOfFrame, frameSet()->kWordDocument()->insertDirectCursor() );


        // Clicked on the left of the text -> select the whole paragraph
        if ( relPos == KWTextFrameSet::LeftOfFrame )
            textView()->selectParagUnderCursor( *textView()->cursor() );
        if ( addParag )
            frameSet()->kWordDocument()->setModified(true );
    }
    // else mightStartDrag = false; necessary?

    if ( e->button() != Qt::LeftButton )
        return;
    KoVariable* var = variable();
    if ( var )
    {
        KWFootNoteVariable * footNoteVar = dynamic_cast<KWFootNoteVariable *>( var );
        if ( footNoteVar )
        {
            footNoteVar->frameSet()->startEditing( m_canvas );
            // --- and now we are deleted! ---
        }
    }
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
    if ( nPoint.y() > 0 && textFrameSet()->documentToInternalMouseSelection( dPoint, iPoint, relPos , m_canvas->viewMode()) )
    {
        if ( relPos == KWTextFrameSet::LeftOfFrame )
            textView()->extendParagraphSelection( iPoint );
        else
            textView()->handleMouseMoveEvent( e, iPoint );
    }

}

bool KWTextFrameSetEdit::openLink( KoLinkVariable* variable )
{
    KWTextFrameSet* fs = textFrameSet();
    KWDocument* doc = fs->kWordDocument();
    if ( doc->variableCollection()->variableSetting()->displayLink() ) {

        const QString url = variable->url();
        if( url.startsWith("bkm://") )
        {
            const KoTextBookmark* bookmark = doc->bookmarkByName(url.mid(6) );
            if ( bookmark )
            {
                cursor()->setParag( bookmark->startParag() );
                ensureCursorVisible();
                return true;
            }
        }
        KoTextView::openLink( variable );
        return true;
    }
    return false;
}

void KWTextFrameSetEdit::openLink()
{
    KoLinkVariable* v = linkVariable();
    if ( v )
        openLink( v );
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
    const QMimeData* mimeData = e->mimeData();
    int provides = KWView::checkClipboard( mimeData );
    if ( !frameSet()->kWordDocument()->isReadWrite() || provides == 0 )
    {
        e->ignore();
        return;
    }
    e->acceptAction();
}

void KWTextFrameSetEdit::dragMoveEvent( QDragMoveEvent * e, const QPoint &nPoint, const KoPoint & )
{
    const QMimeData* mimeData = e->mimeData();
    int provides = KWView::checkClipboard( mimeData );
    if ( !frameSet()->kWordDocument()->isReadWrite() || provides == 0 )
    {
        e->ignore();
        return;
    }
    // place cursor - unless dropping an image. well it's hard to know if the user
    // wants the dropped image to be inline or absolute positioned.
    if ( provides & ( KWView::ProvidesOasis | KWView::ProvidesPlainText | KWView::ProvidesFormula ) )
    {
        QPoint iPoint;
        KoPoint dPoint = frameSet()->kWordDocument()->unzoomPoint( nPoint );
        if ( textFrameSet()->documentToInternal( dPoint, iPoint ) )
        {
            textObject()->emitHideCursor();
            placeCursor( iPoint );
            textObject()->emitShowCursor();
        }
    }
    e->acceptAction();
}

void KWTextFrameSetEdit::dragLeaveEvent( QDragLeaveEvent * )
{
}

void KWTextFrameSetEdit::dropEvent( QDropEvent * e, const QPoint & nPoint, const KoPoint &, KWView* view )
{
    const QMimeData* mimeData = e->mimeData();
    int provides = KWView::checkClipboard( mimeData );
    if ( frameSet()->kWordDocument()->isReadWrite() && provides )
    {
        e->acceptAction();
        KoTextCursor dropCursor( textDocument() );
        QPoint dropPoint;
        KoPoint dPoint = frameSet()->kWordDocument()->unzoomPoint( nPoint );
        if ( !textFrameSet()->documentToInternal( dPoint, dropPoint ) )
            return; // Don't know where to paste

        dropCursor.place( dropPoint, textDocument()->firstParag() );
        kDebug(32001) << "KWTextFrameSetEdit::dropEvent dropCursor at parag=" << dropCursor.parag()->paragId() << " index=" << dropCursor.index() << endl;

        if ( ( e->source() == m_canvas ||
               e->source() == m_canvas->viewport() ) &&
               e->action() == QDropEvent::Move &&
              // this is the indicator that the source and dest text objects are the same
             textDocument()->hasSelection( KoTextDocument::Standard ) ) {

            KCommand *cmd = textView()->prepareDropMove( dropCursor );
            if(cmd)
            {
                KMacroCommand* macroCmd = new KMacroCommand( i18n( "Move Text" ) );
                macroCmd->addCommand(cmd);

                cmd = pasteOasisCommand( mimeData );
                if ( cmd )
                    macroCmd->addCommand(cmd);
                //relayout textframeset after a dnd otherwise autoextend
                //frameset is not re-layouted
                textFrameSet()->layout();
                frameSet()->kWordDocument()->addCommand( macroCmd );
            }
            return;
        }
        else
        {   // drop coming from outside -> forget about current selection
            textDocument()->removeSelection( KoTextDocument::Standard );
            textObject()->selectionChangedNotify();
        }

        // The cursor is already correctly positioned, all we need to do is to "paste" the dropped data.
        view->pasteData( mimeData, true );
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
    kDebug() << "KWTextFrameSetEdit::drawCursor " << visible << endl;
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

bool KWTextFrameSetEdit::pgUpKeyPressed()
{
    QRect crect( m_canvas->contentsX(), m_canvas->contentsY(),
                 m_canvas->visibleWidth(), m_canvas->visibleHeight() );
    crect = m_canvas->viewMode()->viewToNormal( crect );

    // Go up of 90% of crect.height()
    int h = frameSet()->kWordDocument()->pixelToLayoutUnitY( (int)( (double)crect.height() * 0.9 ) );
    KoTextParag *s = textView()->cursor()->parag();
    KoTextParag* oldParag = s;
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
    if ( s == oldParag )
    {
        m_canvas->viewportScroll( true );
        return false;
    }
    return true;
}

bool KWTextFrameSetEdit::pgDownKeyPressed()
{
    QRect crect( m_canvas->contentsX(), m_canvas->contentsY(),
                 m_canvas->visibleWidth(), m_canvas->visibleHeight() );
    crect = m_canvas->viewMode()->viewToNormal( crect );
    // Go down of 90% of crect.height()
    int h = frameSet()->kWordDocument()->pixelToLayoutUnitY( (int)( (double)crect.height() * 0.9 ) );

    KoTextCursor *cursor = textView()->cursor();
    KoTextParag *s = cursor->parag();
    KoTextParag* oldParag = s;
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
    if ( s == oldParag )
    {
        m_canvas->viewportScroll( false );
        return false;
    }
    return true;
}

void KWTextFrameSetEdit::ctrlPgUpKeyPressed()
{
    if ( m_currentFrame )
    {
        QPoint iPoint = textFrameSet()->moveToPage( m_currentFrame->pageNumber(), -1 );
        if ( !iPoint.isNull() )
            placeCursor( iPoint );
    }
}

void KWTextFrameSetEdit::ctrlPgDownKeyPressed()
{
    if ( m_currentFrame )
    {
        QPoint iPoint = textFrameSet()->moveToPage( m_currentFrame->pageNumber(), +1 );
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
            cursor(), _c, i18n("Insert Expression")));
    else
       textObject()->insert( cursor(), currentFormat(), _c, i18n("Insert Expression") );
}

void KWTextFrameSetEdit::insertFloatingFrameSet( KWFrameSet * fs, const QString & commandName )
{
    textObject()->clearUndoRedoInfo();
    CustomItemsMap customItemsMap;
    QString placeHolders;
    // TODO support for multiple floating items (like multiple-page tables)
    int frameNumber = 0;
    int index = 0;
    int insertFlags = KoTextObject::DoNotRemoveSelected;
    { // the loop will start here :)
        KWAnchor * anchor = fs->createAnchor( textFrameSet()->textDocument(), frameNumber );
        if ( frameNumber == 0 && anchor->ownLine() && cursor()->index() > 0 ) // enforce start of line - currently unused
        {
            kDebug() << "ownline -> prepending \\n" << endl;
            placeHolders += QChar('\n');
            index++;
            insertFlags |= KoTextObject::CheckNewLine;
        }
        placeHolders += KoTextObject::customItemChar();
        customItemsMap.insert( index, anchor );
    }
    fs->setAnchored( textFrameSet() );
    textObject()->insert( cursor(), currentFormat(), placeHolders,
                          commandName, KoTextDocument::Standard, insertFlags,
                          customItemsMap );
}

void KWTextFrameSetEdit::insertLink(const QString &_linkName, const QString & hrefName)
{
    KWDocument * doc = frameSet()->kWordDocument();
    KoVariable * var = new KoLinkVariable( textFrameSet()->textDocument(), _linkName, hrefName, doc->variableFormatCollection()->format( "STRING" ), doc->variableCollection() );
    insertVariable( var );
}

void KWTextFrameSetEdit::insertComment(const QString &_comment)
{
    KWDocument * doc = frameSet()->kWordDocument();
    KoVariable * var = new KoNoteVariable( textFrameSet()->textDocument(), _comment, doc->variableFormatCollection()->format( "STRING" ), doc->variableCollection() );
    insertVariable( var );
}


void KWTextFrameSetEdit::insertCustomVariable( const QString &name)
{
     KWDocument * doc = frameSet()->kWordDocument();
     KoVariable * var = new KoCustomVariable( textFrameSet()->textDocument(), name, doc->variableFormatCollection()->format( "STRING" ), doc->variableCollection());
     insertVariable( var );
}

void KWTextFrameSetEdit::insertFootNote( NoteType noteType, KWFootNoteVariable::Numbering numType, const QString &manualString )
{
    KWFootNoteFrameSet *fs = textFrameSet()->insertFootNote( noteType, numType, manualString );
    KWFootNoteVariable * var = fs->footNoteVariable();

    // Place the frame on the correct page, but the exact coordinates
    // will be determined by recalcFrames (KWFrameLayout)
    int pageNum = m_currentFrame->pageNumber();
    fs->createInitialFrame( pageNum );

    insertVariable( var );

    // Re-number footnote variables
    textFrameSet()->renumberFootNotes();

    // Layout the footnote frame
    textFrameSet()->kWordDocument()->recalcFrames( pageNum, -1 ); // we know that for sure nothing changed before this page.

    //KoTextParag* parag = fs->textDocument()->firstParag();
    //parag->truncate(0); // why? we just created it, anyway...

    // And now edit the footnote frameset - all WPs do that it seems.
    fs->startEditing( m_canvas );
    // --- and now we are deleted! ---
}

void KWTextFrameSetEdit::insertVariable( int type, int subtype )
{
    kDebug() << "KWTextFrameSetEdit::insertVariable " << type << endl;
    KWDocument * doc = frameSet()->kWordDocument();

    KoVariable * var = 0L;
    bool refreshCustomMenu = false;
    if ( type == VT_CUSTOM )
    {
        KoCustomVarDialog dia( m_canvas );
        if ( dia.exec() == QDialog::Accepted )
        {
            KoCustomVariable *v = new KoCustomVariable( textFrameSet()->textDocument(), dia.name(), doc->variableFormatCollection()->format( "STRING" ),doc->variableCollection() );
            v->setValue( dia.value() );
            var = v;
            refreshCustomMenu = true;
        }
    }
    else if ( type == VT_MAILMERGE )
    {
        KWMailMergeVariableInsertDia dia( m_canvas, doc->mailMergeDataBase() );
        if ( dia.exec() == QDialog::Accepted )
        {
            var = new KWMailMergeVariable( textFrameSet()->textDocument(), dia.getName(), doc->variableFormatCollection()->format( "STRING" ),doc->variableCollection(),doc );
        }
    }
    else
        var = doc->variableCollection()->createVariable( type, subtype, doc->variableFormatCollection(), 0L, textFrameSet()->textDocument(), doc, 0);
    if ( var)
        insertVariable( var, 0L /*means currentFormat()*/, refreshCustomMenu);
}

void KWTextFrameSetEdit::insertVariable( KoVariable *var, KoTextFormat *format /*=0*/, bool refreshCustomMenu )
{
    if ( var )
    {
        CustomItemsMap customItemsMap;
        customItemsMap.insert( 0, var );
        if (!format)
            format = currentFormat();
        kDebug() << "KWTextFrameSetEdit::insertVariable inserting into paragraph" << endl;
#ifdef DEBUG_FORMATS
        kDebug() << "KWTextFrameSetEdit::insertVariable format=" << format << endl;
#endif
        textObject()->insert( cursor(), format, KoTextObject::customItemChar(),
                              i18n("Insert Variable"),
                              KoTextDocument::Standard,
                              KoTextObject::DoNotRemoveSelected,
                              customItemsMap );
        frameSet()->kWordDocument()->slotRepaintChanged( frameSet() );
        if ( var->type()==VT_CUSTOM && refreshCustomMenu)
            frameSet()->kWordDocument()->refreshMenuCustomVariable();
    }
}

void KWTextFrameSetEdit::insertWPPage()
{
    KWTextFrameSet* textfs = textFrameSet();
    textfs->clearUndoRedoInfo();
    KoTextObject* textobj = textObject();
    KWDocument * doc = frameSet()->kWordDocument();
    int pages = doc->pageCount();
    int columns = doc->numColumns();
    // There could be N columns. In that case we may need to add up to N framebreaks.
    int inserted = 0;
    KMacroCommand* macroCmd = new KMacroCommand( i18n("Insert Page") );
    do {
        macroCmd->addCommand( textfs->insertFrameBreakCommand( cursor() ) );
        textobj->setLastFormattedParag( cursor()->parag() );
        textobj->formatMore( 2 );
    } while ( pages == doc->pageCount() && ++inserted <= columns );
    if ( pages == doc->pageCount() )
        kWarning(32002) << k_funcinfo << " didn't manage to insert a new page! inserted=" << inserted << " columns=" << columns << " pages=" << pages << endl;

    doc->addCommand( macroCmd );

    textfs->slotRepaintChanged();
    textobj->emitEnsureCursorVisible();
    textobj->emitUpdateUI( true );
    textobj->emitShowCursor();
}

KoBorder KWTextFrameSetEdit::border(KoBorder::BorderType type) {
    if(type == KoBorder::LeftBorder)
        return m_paragLayout.leftBorder;
    if(type == KoBorder::RightBorder)
        return m_paragLayout.rightBorder;
    if(type == KoBorder::TopBorder)
        return m_paragLayout.topBorder;
    return m_paragLayout.bottomBorder;
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
        m_canvas->gui()->getView()->updateBorderButtons( m_paragLayout.leftBorder, m_paragLayout.rightBorder, m_paragLayout.topBorder, m_paragLayout.bottomBorder );
    }

    if ( !parag->style() )
        kWarning() << "Paragraph " << parag->paragId() << " has no style" << endl;
    else if ( m_paragLayout.style != parag->style() || force )
    {
        m_paragLayout.style = parag->style();
        m_canvas->gui()->getView()->showStyle( m_paragLayout.style->name() );
    }

    if( m_paragLayout.margins[Q3StyleSheetItem::MarginLeft] != parag->margin(Q3StyleSheetItem::MarginLeft)
        || m_paragLayout.margins[Q3StyleSheetItem::MarginFirstLine] != parag->margin(Q3StyleSheetItem::MarginFirstLine)
        || m_paragLayout.margins[Q3StyleSheetItem::MarginRight] != parag->margin(Q3StyleSheetItem::MarginRight)
        || parag->string()->isRightToLeft() != m_rtl
        || force )
    {
        m_paragLayout.margins[Q3StyleSheetItem::MarginFirstLine] = parag->margin(Q3StyleSheetItem::MarginFirstLine);
        m_paragLayout.margins[Q3StyleSheetItem::MarginLeft] = parag->margin(Q3StyleSheetItem::MarginLeft);
        m_paragLayout.margins[Q3StyleSheetItem::MarginRight] = parag->margin(Q3StyleSheetItem::MarginRight);
        if ( m_rtl != parag->string()->isRightToLeft() && parag->counter() )
        {
            parag->counter()->invalidate();
            parag->setChanged( true ); // repaint
        }
        m_rtl = parag->string()->isRightToLeft();
        m_canvas->gui()->getView()->showRulerIndent( m_paragLayout.margins[Q3StyleSheetItem::MarginLeft], m_paragLayout.margins[Q3StyleSheetItem::MarginFirstLine], m_paragLayout.margins[Q3StyleSheetItem::MarginRight], m_rtl );
    }
    if( m_paragLayout.tabList() != parag->tabList() || force)
    {
        m_paragLayout.setTabList( parag->tabList() );
        KoRuler * hr = m_canvas->gui()->getHorzRuler();
        if ( hr )
            hr->setTabList( parag->tabList() );
    }
    if( m_paragLayout.lineSpacingType != parag->paragLayout().lineSpacingType || force)
    {
      m_paragLayout.lineSpacingType = parag->paragLayout().lineSpacingType;
      m_canvas->gui()->getView()->showSpacing( m_paragLayout.lineSpacingType );
    }
    // There are more paragraph settings, but those that are not directly
    // visible in the UI don't need to be handled here.
    // For instance parag stuff, borders etc.
}

void KWTextFrameSetEdit::showFormat( KoTextFormat *format )
{
    m_canvas->gui()->getView()->showFormat( *format );
}

QPoint KWTextFrameSet::cursorPos( KoTextCursor *cursor, KWCanvas* canvas, KWFrame* currentFrame )
{
    KWViewMode *viewMode = canvas->viewMode();

    KoTextParag* parag = cursor->parag();
    const QPoint topLeft = parag->rect().topLeft();         // in QRT coords
    int lineY;
    parag->lineHeightOfChar( cursor->index(), 0, &lineY );
    // iPoint is the topright corner of the current character
    QPoint iPoint( topLeft.x() + cursor->x() + parag->at( cursor->index() )->width, topLeft.y() + lineY );

    KoPoint dPoint;
    QPoint vPoint;
    KoPoint hintDPoint = currentFrame ? currentFrame->innerRect().topLeft() : KoPoint();
    if ( internalToDocumentWithHint( iPoint, dPoint, hintDPoint ) )
    {
        vPoint = viewMode->normalToView( m_doc->zoomPoint( dPoint ) ); // from doc to view contents
        vPoint.rx() -= canvas->contentsX();
        vPoint.ry() -= canvas->contentsY();
    } // else ... ?
    return vPoint;
}

//////

bool KWFootNoteFrameSet::isFootNote() const
{
    if ( !m_footNoteVar ) {
        kWarning() << k_funcinfo << " called too early? No footnote var." << endl;
        return false;
    }
    return ( m_footNoteVar->noteType() == FootNote );
}

bool KWFootNoteFrameSet::isEndNote() const
{
    if ( !m_footNoteVar ) {
        kWarning() << k_funcinfo << " called too early? No footnote var." << endl;
        return false;
    }
    return ( m_footNoteVar->noteType() == EndNote );
}



void KWFootNoteFrameSet::createInitialFrame( int pageNum )
{
    KWFrame *frame = new KWFrame(this, 0, m_doc->pageManager()->topOfPage(pageNum) + 1, 20, 20 );
    frame->setFrameBehavior(KWFrame::AutoExtendFrame);
    frame->setNewFrameBehavior(KWFrame::NoFollowup);
    addFrame( frame );
}

void KWFootNoteFrameSet::startEditing( KWCanvas* canvas )
{
    canvas->editFrameSet( this );

    // Ensure cursor is visible
    KWTextFrameSetEdit *textedit = dynamic_cast<KWTextFrameSetEdit *>(canvas->currentFrameSetEdit()->currentTextEdit());
    if ( textedit )
        textedit->ensureCursorVisible();
}

void KWFootNoteFrameSet::setFootNoteVariable( KWFootNoteVariable* var )
{
     m_footNoteVar = var;
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

#include "KWTextFrameSet.moc"
