#include "kwframelayout.h"
#include "kwtextframeset.h"
#include "kwdoc.h"
#include <qtimer.h>

//#define DEBUG_FRAMELAYOUT

#ifdef NDEBUG
#undef DEBUG_FRAMELAYOUT
#endif

KWFrameLayout::HeaderFooterFrameset::HeaderFooterFrameset( KWTextFrameSet* fs, int start, int end,
                                                           double spacing, OddEvenAll oea )
    : m_frameset(fs), m_startAtPage(start), m_endAtPage(end), m_oddEvenAll(oea),
      m_spacing(spacing), m_minY( 0 ), m_positioned( false )
{
    if ( fs->getNumFrames() > 0 )
        m_height = fs->frame(0)->height();
    else
        m_height = 20; // whatever. The text layout will resize it.
}


void KWFrameLayout::HeaderFooterFrameset::debug()
{
#ifdef DEBUG_FRAMELAYOUT
    HeaderFooterFrameset* hff = this;
    kdDebug(32002) << " * " << hff->m_frameset->getName()
                   << " pages:" << hff->m_startAtPage << "-" << (hff->m_endAtPage==-1?QString("(all)"):QString::number(hff->m_endAtPage))
                   << " page-selection:" << (hff->m_oddEvenAll==HeaderFooterFrameset::Odd ? "Odd" :
                                             hff->m_oddEvenAll==HeaderFooterFrameset::Even ? "Even" : "All")
                   << " frames:" << hff->m_frameset->getNumFrames()
                   << " height:" << hff->m_height
                   << " spacing:" << hff->m_spacing << endl;
#endif
}

void KWFrameLayout::HeaderFooterFrameset::deleteFramesAfterLast( int lastPage )
{
    int lastFrame = lastFrameNumber( lastPage );
#ifdef DEBUG_FRAMELAYOUT
    //kdDebug(32002) << " Final cleanup: frameset " << m_frameset->getName() << ": lastFrame=" << lastFrame << endl;
#endif
    KWTextFrameSet* fs = m_frameset;
    while ( (int)fs->getNumFrames() - 1 > lastFrame ) {
#ifdef DEBUG_FRAMELAYOUT
        kdDebug(32002) << "  Final cleanup: deleting frame " << fs->getNumFrames() - 1 << " of " << fs->getName() << endl;
#endif
        fs->delFrame( fs->getNumFrames() - 1 );
    }
}

/////

void KWFrameLayout::layout( KWFrameSet* mainTextFrameSet, int numColumns,
                            int fromPage, int toPage )
{
    // Just debug stuff
#ifdef DEBUG_FRAMELAYOUT
    kdDebug(32002) << "KWFrameLayout::layout " << fromPage << " to " << toPage << endl;
    Q_ASSERT( toPage >= fromPage );
    QPtrListIterator<HeaderFooterFrameset> itdbg( m_headersFooters );
    for ( ; itdbg.current() ; ++itdbg )
        itdbg.current()->debug();
    QPtrListIterator<HeaderFooterFrameset> itdbg2( m_footnotes );
    for ( ; itdbg2.current() ; ++itdbg2 )
        itdbg2.current()->debug();
#endif
    double ptColumnWidth = m_doc->ptColumnWidth();
    bool mainTextFrameResized = false;

    // The main loop is: "for each page". We lay out each page separately.
    for ( int pageNum = fromPage ; pageNum <= toPage ; ++pageNum )
    {
        double top = pageNum * m_doc->ptPaperHeight() + m_doc->ptTopBorder();
        double bottom = ( pageNum + 1 ) * m_doc->ptPaperHeight() - m_doc->ptBottomBorder();
        double left = m_doc->ptLeftBorder();
        double right = m_doc->ptPaperWidth() - m_doc->ptRightBorder();
        Q_ASSERT( left < right );
        KoRect oldColumnRect = firstColumnRect( mainTextFrameSet, pageNum, numColumns );

        // For each header/footer....
        for ( QPtrListIterator<HeaderFooterFrameset> it( m_headersFooters ); it.current() ; ++it )
        {
            int frameNum = it.current()->frameNumberForPage( pageNum );
            if ( frameNum != -1 )
            {
                it.current()->m_positioned = true;
                KWTextFrameSet* fs = it.current()->m_frameset;
#ifdef DEBUG_FRAMELAYOUT
                kdDebug(32002) << " Page " << pageNum << ": adding frame " << frameNum << " from " << fs->getName() << endl;
#endif
                KoRect rect;
                if ( fs->isAHeader() ) // add on top
                {
                    rect.setRect( left, top, right - left, it.current()->m_height );
                    top += it.current()->m_height + it.current()->m_spacing;
                } else // footer, add at bottom
                {
                    double frameHeight = it.current()->m_height;
                    double frameTop = bottom - frameHeight;
                    rect.setRect( left, frameTop, right - left, frameHeight );
                    bottom -= frameHeight + it.current()->m_spacing;
                }
                Q_ASSERT( bottom > 0 );
                Q_ASSERT( top < bottom );
#ifdef DEBUG_FRAMELAYOUT
                kdDebug(32002) << "     rect:" << rect << "   - new_top:" << top << " new_bottom:" << bottom << endl;
#endif
                resizeOrCreateHeaderFooter( fs, frameNum, rect );
            }
        }

        // All headers/footers for this page have been done,
        // now resize the frame from the main textframeset (if any)
        // the first time _before_ doing the footnotes.
        resizeMainTextFrame( mainTextFrameSet, pageNum, numColumns, ptColumnWidth, m_doc->ptColumnSpacing(), left, top, bottom, true );

        // Recalc footnote pages
        checkFootNotes();

        bool firstFootNote = true;
        // For each footnote (called sorted them from bottom to top)
        for ( QPtrListIterator<HeaderFooterFrameset> it( m_footnotes ); it.current() ; ++it )
        {
            int frameNum = it.current()->frameNumberForPage( pageNum );
            if ( frameNum != -1 )
            {
                it.current()->m_positioned = true;
                KWTextFrameSet* fs = it.current()->m_frameset;
#ifdef DEBUG_FRAMELAYOUT
                kdDebug(32002) << " Page " << pageNum << ": adding footnote frame " << frameNum << " from " << fs->getName() << endl;
#endif
                KoRect rect;

                // When two footnotes are in the same page there should be 0 spacing between them
                // Yeah, write a generic frame layouter and then realize it's not flexible enough :(
                if ( fs->isFootEndNote() && !firstFootNote )
                {
                    // Undo "bottom -= spacing". This assumes equal spacing for all footnotes
                    bottom += it.current()->m_spacing;
                    bottom -= 1; // keep them one pixel apart though
                }
                double frameTop = bottom - it.current()->m_height;
                double frameHeight = it.current()->m_height;

                Q_ASSERT ( fs->isFootEndNote() );

                //KWFrame::FrameBehavior fnFrameBehavior = KWFrame::AutoExtendFrame;
                KWFootNoteFrameSet* fnfs = static_cast<KWFootNoteFrameSet *>( fs );
                KWFootNoteVariable* fnvar = fnfs->footNoteVariable();

                // The varY() call here is what depends on the maintextframe size
                /*double minY = fnvar->varY() + it.current()->m_spacing */ /* some spacing */;
                double minY = it.current()->m_minY;
                kdDebug(32002) << "   footnote: frameHeight=" << frameHeight << " frameTop (" << frameTop << ") <? minY (" << minY << ")" << endl;
                if ( frameTop < minY )
                {
                    // Ok, this is the complex case of a footnote var too far down in the page,
                    // and its footnote text is too big, so both won't fit.
                    // We do like other WPs: we create a frame on the next page
                    it.current()->m_endAtPage++; // this will do so

                    // In the current page we stop at minY.
                    frameTop = minY;
                    frameHeight = bottom - frameTop;
                    kdDebug(32002) << "   footnote: new top=" << frameTop << " new height=" << frameHeight << " remaining height=" << it.current()->m_height - frameHeight << endl;
                    Q_ASSERT( frameHeight < it.current()->m_height );
                    it.current()->m_height -= frameHeight; // calculate what remains to be done in the next frame
                    //fnFrameBehavior = KWFrame::Ignore;
                }

                rect.setRect( left, frameTop, right - left, frameHeight );
                bottom -= frameHeight + it.current()->m_spacing;
                Q_ASSERT( bottom > 0 );
                Q_ASSERT( top < bottom );
#ifdef DEBUG_FRAMELAYOUT
                kdDebug(32002) << "     footnote rect:" << rect << "   - new_top:" << top << " new_bottom:" << bottom << endl;
#endif
                resizeOrCreateHeaderFooter( fs, frameNum, rect );
                firstFootNote = false;

                // We added a footnote, update main text frame size
#ifdef DEBUG_FRAMELAYOUT
                kdDebug(32002) << "   Laid out a footnote -> call resizeMainTextFrame/checkFootNotes again" << endl;
#endif
                resizeMainTextFrame( mainTextFrameSet, pageNum, numColumns, ptColumnWidth, m_doc->ptColumnSpacing(), left, top, bottom, true );
                checkFootNotes();
            }
        } // for all footnotes

        if ( !mainTextFrameResized ) {
            // Test if the main text frame for this page was really resized or not.
            KoRect newColumnRect = firstColumnRect( mainTextFrameSet, pageNum, numColumns );
#ifdef DEBUG_FRAMELAYOUT
            kdDebug(32002) << "  Comparing old=" << oldColumnRect << " and new=" << newColumnRect << endl;
#endif
            mainTextFrameResized = ( oldColumnRect != newColumnRect );
        }

    } // for all pages

    // Final cleanup: delete all frames after lastFrameNumber in each frameset
    QPtrListIterator<HeaderFooterFrameset> it( m_headersFooters );
    for ( ; it.current() ; ++it )
        it.current()->deleteFramesAfterLast( m_doc->getPages() - 1 );
    QPtrListIterator<HeaderFooterFrameset> it2( m_footnotes );
    for ( ; it2.current() ; ++it2 )
        it2.current()->deleteFramesAfterLast( m_doc->getPages() - 1 );

    if ( mainTextFrameResized ) {
#ifdef DEBUG_FRAMELAYOUT
        kdDebug(32002) << " Done. mainTextFrameResized is true, calling invalidate (delayed)." << endl;
#endif
        // m_doc->updateAllFrames(); // done by resizeMainTextFrame

        // Not right now, this could be called during formatting...
        //m_doc->invalidate();
        // ### This means the layout will be done during painting. Not good.
        // What about mainTextFrameResized->invalidate() or even layout()?
        QTimer::singleShot( 0, m_doc, SLOT( invalidate() ) );
    }
}

void KWFrameLayout::resizeOrCreateHeaderFooter( KWTextFrameSet* headerFooter, uint frameNumber, const KoRect& rect )
{
    if ( frameNumber < headerFooter->getNumFrames() ) {
        headerFooter->frame( frameNumber )->setRect( rect );
    }
    else
    {
#ifdef DEBUG_FRAMELAYOUT
        kdDebug(32002) << "KWFrameLayout::resizeOrCreateHeaderFooter creating frame for " << headerFooter->getName() << endl;
#endif
        KWFrame* frame = new KWFrame( headerFooter, rect.x(), rect.y(), rect.width(), rect.height() );
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        if ( headerFooter->isHeaderOrFooter() ) // not for footnotes!
        {
            frame->setNewFrameBehavior( KWFrame::Copy );
            frame->setCopy( true );
        }
        else
            frame->setNewFrameBehavior( KWFrame::NoFollowup );
        headerFooter->addFrame( frame );
    }
    // This updates e.g. availableHeight. Very important in the case
    // of the footnote frameset with 2 frames.
    headerFooter->updateFrames();
}

// Called at beginning and end of the layout for a given page,
// to determine if the main-text-frame layout really changed or not.
// Testing in resizeMainTextFrame doesn't work, we call it several times,
// once for each footnote, so it can't detect the "no change" case.
KoRect KWFrameLayout::firstColumnRect( KWFrameSet* mainTextFrameSet, int pageNum, int numColumns ) const
{
    uint frameNum = pageNum * numColumns /*+ col  0 here*/;
    if ( mainTextFrameSet && frameNum < mainTextFrameSet->getNumFrames() )
        return * mainTextFrameSet->frame( frameNum );
    else
        return KoRect();
}

bool KWFrameLayout::resizeMainTextFrame( KWFrameSet* mainTextFrameSet, int pageNum, int numColumns, double ptColumnWidth, double ptColumnSpacing, double left, double top, double bottom, bool updateFrames )
{
    if ( !mainTextFrameSet )
        return false;
    bool mainTextFrameResized = false;
    for ( int col = 0; col < numColumns; col++ ) {
        Q_ASSERT( bottom > top );
        // Calculate wanted rect for this frame
        KoRect rect( left + col * ( ptColumnWidth + ptColumnSpacing ),
                     top, ptColumnWidth, bottom - top );
        uint frameNum = pageNum * numColumns + col;
#ifdef DEBUG_FRAMELAYOUT
        kdDebug(32002) << " Page " << pageNum << ": resizing main text frame " << frameNum << " to " << rect << endl;
#endif
        if ( frameNum < mainTextFrameSet->getNumFrames() ) {
            // Resize existing frame
            KWFrame* frame = mainTextFrameSet->frame( frameNum );
            bool resized = (rect != *frame);
            if ( resized ) {
                frame->setRect( rect );
                frame->updateRulerHandles();
                mainTextFrameResized = true;
                if ( updateFrames )
                    mainTextFrameSet->updateFrames();
            }
        } else {
            // Create new frame
            KWFrame * frame = new KWFrame( mainTextFrameSet, rect.x(), rect.y(), rect.width(), rect.height() );
            mainTextFrameSet->addFrame( frame );
            Q_ASSERT( frameNum == mainTextFrameSet->getNumFrames()-1 );
            mainTextFrameResized = true;
            if ( updateFrames )
                mainTextFrameSet->updateFrames();
        }
    }
    return mainTextFrameResized;
}

void KWFrameLayout::checkFootNotes()
{
    // We recalculate all footnotes pages, but we return true
    // if those on pageNum have changed.
    QPtrListIterator<HeaderFooterFrameset> it( m_footnotes );
    for ( ; it.current() ; ++it )
    {
        HeaderFooterFrameset* hff = it.current();
        if ( ! hff->m_positioned )
        {
            Q_ASSERT ( hff->m_frameset->isFootEndNote() );
            KWFootNoteFrameSet* fnfs = static_cast<KWFootNoteFrameSet *>( hff->m_frameset );
            KWFootNoteVariable* fnvar = fnfs->footNoteVariable();
            double varY = fnvar->varY();
            hff->m_minY = varY + hff->m_spacing /* some spacing */;
            int pageNum = static_cast<int>(varY / m_doc->ptPaperHeight());
            //int pageNum = fnvar->pageNum(); // faster to deduce it from varY
            if ( pageNum != hff->m_startAtPage ) {
#ifdef DEBUG_FRAMELAYOUT
                kdDebug(32002) << " checkFootNotes: found minY=" << hff->m_minY << " start/end=" << pageNum << " for footnote " << fnvar->text() << endl;
#endif
                hff->m_startAtPage = pageNum;
                hff->m_endAtPage = pageNum;
            }
        }
    }
}
