/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <qcolor.h>
#include <koprinter.h>
#include <qevent.h>
#include <qregion.h>
#include <kmessagebox.h>
#include <qdict.h>
#include <qpixmap.h>
#include <qfileinfo.h>

#include <koTemplateChooseDia.h>
#include <koStore.h>
#include <koStoreDevice.h>
#include <koMainWindow.h>
#include <koQueryTrader.h>

#include <kapp.h> // for KDE_VERSION
#include <kurl.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kcharsets.h>

#include <kformuladocument.h>
#include <koDocumentInfo.h>
#include <unistd.h>
#include <math.h>

#include "kwframe.h"
#include "kwtextframeset.h"
#include "kwtableframeset.h"
#include "kwdoc.h"
#include "kwcanvas.h"
#include "defs.h"
#include "kwutils.h"
#include "kwstyle.h"
#include "autoformat.h"
#include "variable.h"
#include "serialletter.h"
#include "kwview.h"
#include "kwviewmode.h"
#include "kwfactory.h"
#include "kwcommand.h"
#include "kwtextimage.h"
#include <kdebug.h>
#include <kfontdialog.h>

#include <kspell.h>

//#define DEBUG_PAGES

#undef getPointBasedAttribute
#define getPointBasedAttribute(structure, attribute, element, attributeName, defaultValue) \
do \
{ \
    structure.pt##attribute = getAttribute( element, attributeName, defaultValue ); \
    structure.mm##attribute = POINT_TO_MM( structure.pt##attribute ); \
    structure.inch##attribute = POINT_TO_INCH( structure.pt##attribute ); \
} while (0)

/******************************************************************/
/* Class: KWChild                                              */
/******************************************************************/

KWChild::KWChild( KWDocument *_wdoc, const QRect& _rect, KoDocument *_doc )
    : KoDocumentChild( _wdoc, _doc, _rect )
{
}

KWChild::KWChild( KWDocument *_wdoc )
    : KoDocumentChild( _wdoc )
{
}

KWChild::~KWChild()
{
}

KoDocument *KWChild::hitTest( const QPoint &, const QWMatrix & )
{
  return 0L;
}

/******************************************************************/
/* Class: KWCommandHistory                                        */
/******************************************************************/
class KWCommandHistory : public KCommandHistory
{
public:
    KWCommandHistory( KWDocument * doc ) : KCommandHistory( doc->actionCollection(),  false ), m_pDoc( doc ) {}
public /*slots*/: // They are already slots in the parent. Running moc on the inherited class shouldn't be necessary AFAICS.
    virtual void undo();
    virtual void redo();
private:
    KWDocument * m_pDoc;
};

void KWCommandHistory::undo()
{
    m_pDoc->clearUndoRedoInfos();
    KCommandHistory::undo();
}

void KWCommandHistory::redo()
{
    m_pDoc->clearUndoRedoInfos();
    KCommandHistory::redo();
}

void KWDocument::clearUndoRedoInfos()
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet *>( fit.current() );
        if ( fs )
            fs->clearUndoRedoInfo();
    }
}

/******************************************************************/
/* Class: KWDocument                                              */
/******************************************************************/

KWDocument::KWDocument(QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
      m_unit( KWUnit::U_MM ), // footNoteManager( this ),
      urlIntern()
{
    m_lstViews.setAutoDelete( false );
    m_lstChildren.setAutoDelete( true );
    m_styleList.setAutoDelete( false );
    m_deletedStyles.setAutoDelete( false );
//    varFormats.setAutoDelete(true);
    frames.setAutoDelete( true );

    setInstance( KWFactory::global(), false );

    m_gridX = m_gridY = 10;
    m_indent = MM_TO_POINT( 10.0 );

    m_iNbPagePerRow = 4;

    m_bShowRuler = true;

    m_viewFormattingChars = false;

    m_bDontCheckUpperWord = false;
    m_bDontCheckTitleCase = false;
    //m_onlineSpellCheck = false;

    m_autoFormat = new KWAutoFormat(this);

    m_commandHistory = new KWCommandHistory( this );
#if KDE_VERSION >= 220 // kdelibs >= 2.2 only
    connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
#endif
    connect( m_commandHistory, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );

    connect( documentInfo(), SIGNAL( sigDocumentInfoModifed()),this,SLOT(slotDocumentInfoModifed() ) );

    m_formulaDocument = new KFormula::KFormulaDocument(actionCollection(), m_commandHistory);

    setEmpty();
    setModified(false);

    styleMask = U_FONT_FAMILY_ALL_SIZE | U_COLOR | U_BORDER | U_INDENT |
                         U_NUMBERING | U_ALIGN | U_TABS | U_SMART;
    m_headerVisible = false;
    m_footerVisible = false;

    m_lastStyle = 0L;
    m_pixmapMap = 0L;
    m_clipartMap = 0L;
    m_pasteFramesetsMap = 0L;

    slDataBase = new KWSerialLetterDataBase( this );
    slRecordNum = -1;

    // Get default font from KDE
    m_defaultFont = KGlobalSettings::generalFont();
    KGlobal::charsets()->setQFont(m_defaultFont, KGlobal::locale()->charset());

    getFormulaDocument()->setResolution( m_zoomedResolutionX, m_zoomedResolutionY );

    m_syntaxVersion = CURRENT_SYNTAX_VERSION;
    m_pKSpellConfig=0;

    // Some simple import filters don't define any style,
    // so let's have a Standard style at least
    KWStyle * standardStyle = new KWStyle( "Standard" ); // This gets translated later on
    addStyleTemplate( standardStyle );
}

void KWDocument::setZoomAndResolution( int zoom, int dpiX, int dpiY, bool updateViews, bool forPrint )
{
    KWZoomHandler::setZoomAndResolution( zoom, dpiX, dpiY, updateViews, forPrint );
    newZoomAndResolution( updateViews, forPrint );
}

void KWDocument::newZoomAndResolution( bool updateViews, bool forPrint )
{
    getFormulaDocument()->setResolution( m_zoomedResolutionX, m_zoomedResolutionY );

    // Update all fonts
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->zoom( forPrint );

    layout();
    updateAllFrames();
    if ( updateViews )
    {
        emit newContentsSize();
        repaintAllViews( true );
    }
}

bool KWDocument::initDoc()
{
    m_pageLayout.unit = PG_MM;
    m_pages = 1;

    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = s_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.inchHeaderBodySpacing = POINT_TO_INCH( 10 );
    m_pageHeaderFooter.inchFooterBodySpacing = POINT_TO_INCH( 10 );
    m_pageHeaderFooter.mmHeaderBodySpacing = POINT_TO_MM( 10 );
    m_pageHeaderFooter.mmFooterBodySpacing = POINT_TO_MM( 10 );

    QString _template;

    bool ok = FALSE;
    KoTemplateChooseDia::ReturnType ret = KoTemplateChooseDia::choose(
        KWFactory::global(), _template, "application/x-kword", "*.kwd", i18n("KWord"),
        KoTemplateChooseDia::Everything, "kword_template");
    if ( ret == KoTemplateChooseDia::Template ) {
        QFileInfo fileInfo( _template );
        QString fileName( fileInfo.dirPath( TRUE ) + "/" + fileInfo.baseName() + ".kwt" );
        resetURL();
        ok = loadNativeFormat( fileName );
    } else if ( ret == KoTemplateChooseDia::File ) {
        KURL url( _template);
        ok = openURL( url );
    } else if ( ret == KoTemplateChooseDia::Empty ) {
        QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::global() ) );
        resetURL();
        ok = loadNativeFormat( fileName );
    }
    setModified( FALSE );
    setEmpty();
    return ok;
}

void KWDocument::initEmpty()
{
    m_pageLayout.unit = PG_MM;
    m_pages = 1;

    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = s_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.inchHeaderBodySpacing = POINT_TO_INCH( 10 );
    m_pageHeaderFooter.inchFooterBodySpacing = POINT_TO_INCH( 10 );
    m_pageHeaderFooter.mmHeaderBodySpacing = POINT_TO_MM( 10 );
    m_pageHeaderFooter.mmFooterBodySpacing = POINT_TO_MM( 10 );

    QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWFactory::global() ) );
    /*bool ok = */loadNativeFormat( fileName );
    resetURL();
    setModified( FALSE );
    setEmpty();
}


void KWDocument::setPageLayout( KoPageLayout _layout, KoColumns _cl, KoKWHeaderFooter _hf )
{
    if ( m_processingType == WP ) {
        kdDebug() << "KWDocument::setPageLayout WP" << endl;
        m_pageLayout = _layout;
        m_pageColumns = _cl;
        m_pageHeaderFooter = _hf;
    } else {
        //kdDebug() << "KWDocument::setPageLayout NON-WP" << endl;
        m_pageLayout = _layout;
        m_pageLayout.ptLeft = 0;
        m_pageLayout.ptRight = 0;
        m_pageLayout.ptTop = 0;
        m_pageLayout.ptBottom = 0;
        m_pageLayout.mmLeft = 0;
        m_pageLayout.mmRight = 0;
        m_pageLayout.mmTop = 0;
        m_pageLayout.mmBottom = 0;
        m_pageLayout.inchLeft = 0;
        m_pageLayout.inchRight = 0;
        m_pageLayout.inchTop = 0;
        m_pageLayout.inchBottom = 0;
        m_pageHeaderFooter = _hf;
    }

    recalcFrames();

    updateAllFrames();
}

void KWDocument::updateRuler()
{
    kdDebug() << "KWDocument::updateRuler" << endl;
    // Invalidate document layout
    layout();
    //refresh koRuler in each view
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
    {
        viewPtr->getGUI()->getHorzRuler()->setPageLayout( m_pageLayout);
        viewPtr->getGUI()->getVertRuler()->setPageLayout( m_pageLayout );
        viewPtr->getGUI()->canvasWidget()->repaintAll( true );
    }
}

double KWDocument::ptColumnWidth() const
{
    return ( ptPaperWidth() - ptLeftBorder() - ptRightBorder() -
             ptColumnSpacing() * ( m_pageColumns.columns - 1 ) )
        / m_pageColumns.columns;
}

/* append headers and footers if needed, and create enough pages for all the existing frames */
void KWDocument::recalcFrames()
{
    kdDebug(32002) << "KWDocument::recalcFrames" << endl;
    if ( frames.isEmpty() )
        return;

    KWFrameSet *frameset = frames.at( 0 );

    double ptColumnWidth = this->ptColumnWidth();

    int firstHeadOffset = 0, evenHeadOffset = 0, oddHeadOffset = 0;
    int firstFootOffset = 0, evenFootOffset = 0, oddFootOffset = 0;
    KWTextFrameSet *firstHeader = 0L, *evenHeader = 0L, *oddHeader = 0L;
    KWTextFrameSet *firstFooter = 0L, *evenFooter = 0L, *oddFooter = 0L;

    // Lookup the various header / footer framesets into the variables above
    // [Done in all cases, in order to hide unused framesets]

    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * fs = fit.current();
        switch ( fs->frameSetInfo() ) {
            case KWFrameSet::FI_FIRST_HEADER:
                if ( isHeaderVisible() ) {
                    firstHeader = dynamic_cast<KWTextFrameSet*>( fs );
                    firstHeadOffset = static_cast<int>(m_pageHeaderFooter.ptHeaderBodySpacing +
                                                       fs->getFrame( 0 )->height());
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_EVEN_HEADER:
                if ( isHeaderVisible() ) {
                    evenHeader = dynamic_cast<KWTextFrameSet*>( fs );
                    evenHeadOffset = static_cast<int>(m_pageHeaderFooter.ptHeaderBodySpacing +
                                                      fs->getFrame( 0 )->height());
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_ODD_HEADER:
                if ( isHeaderVisible() ) {
                    oddHeader = dynamic_cast<KWTextFrameSet*>( fs );
                    oddHeadOffset = static_cast<int>(m_pageHeaderFooter.ptHeaderBodySpacing +
                                                     fs->getFrame( 0 )->height());
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_FIRST_FOOTER:
                if ( isFooterVisible() ) {
                    firstFooter = dynamic_cast<KWTextFrameSet*>( fs );
                    firstFootOffset = static_cast<int>(m_pageHeaderFooter.ptFooterBodySpacing +
                                                       fs->getFrame( 0 )->height());
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_EVEN_FOOTER:
                if ( isFooterVisible() ) {
                    evenFooter = dynamic_cast<KWTextFrameSet*>( fs );
                    evenFootOffset = static_cast<int>(m_pageHeaderFooter.ptFooterBodySpacing +
                                                      fs->getFrame( 0 )->height());
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
                break;
            case KWFrameSet::FI_ODD_FOOTER:
                if ( isFooterVisible() ) {
                    oddFooter = dynamic_cast<KWTextFrameSet*>( fs );
                    oddFootOffset = static_cast<int>(m_pageHeaderFooter.ptFooterBodySpacing +
                                                     fs->getFrame( 0 )->height());
                } else { fs->setVisible( false ); fs->deleteAllCopies(); }
            default: break;
        }
    }
    // Now hide & forget the unused header/footer framesets (e.g. 'odd pages' if we are in 'all the same' mode etc.)
    if ( isHeaderVisible() ) {
        switch ( getHeaderType() ) {
            case HF_SAME:
                evenHeader->setVisible( true );
                oddHeader->setVisible( false );
                oddHeader->deleteAllCopies();
                firstHeader->setVisible( false );
                firstHeader->deleteAllCopies();

                oddHeader = evenHeader;
                firstHeader = evenHeader;
                oddHeadOffset = evenHeadOffset;
                firstHeadOffset = evenHeadOffset;
                break;
            case HF_FIRST_DIFF:
                evenHeader->setVisible( true );
                oddHeader->setVisible( false );
                oddHeader->deleteAllCopies();
                firstHeader->setVisible( true );

                oddHeader = evenHeader;
                oddHeadOffset = evenHeadOffset;
                break;
            case HF_EO_DIFF:
                evenHeader->setVisible( true );
                oddHeader->setVisible( true );
                firstHeader->setVisible( false );
                firstHeader->deleteAllCopies();

                firstHeader = oddHeader;
                firstHeadOffset = oddHeadOffset;
                break;
        }
    }
    if ( isFooterVisible() ) {
        switch ( getFooterType() ) {
            case HF_SAME:
                evenFooter->setVisible( true );
                oddFooter->setVisible( false );
                oddFooter->deleteAllCopies();
                firstFooter->setVisible( false );
                firstFooter->deleteAllCopies();

                oddFooter = evenFooter;
                firstFooter = evenFooter;
                oddFootOffset = evenFootOffset;
                firstFootOffset = evenFootOffset;
                break;
            case HF_FIRST_DIFF:
                evenFooter->setVisible( true );
                oddFooter->setVisible( false );
                oddFooter->deleteAllCopies();
                firstFooter->setVisible( true );

                oddFooter = evenFooter;
                oddFootOffset = evenFootOffset;
                break;
            case HF_EO_DIFF:
                evenFooter->setVisible( true );
                oddFooter->setVisible( true );
                firstFooter->setVisible( false );
                firstFooter->deleteAllCopies();

                firstFooter = oddFooter;
                firstFootOffset = oddFootOffset;
                break;
        }
    }

    if ( m_processingType == WP ) { // In WP mode the pages are created automatically. In DTP not...

        int headOffset = 0, footOffset = 0;
        int oldPages = m_pages;
        unsigned int frms = frameset->getNumFrames();

        // Determine number of pages - first from the text frames
        m_pages = static_cast<int>( ceil( static_cast<double>( frms ) / static_cast<double>( m_pageColumns.columns ) ) );
#ifdef DEBUG_PAGES
        kdDebug(32002) << "KWDocument::recalcFrames frms(" << frms << ") / columns(" << m_pageColumns.columns << ") = " << m_pages << endl;
#endif
        // Then from the other frames ( frameset-num > 0 )
        double maxBottom = 0;
        for (int m = getNumFrameSets() - 1; m > 0; m-- )
        {
            KWFrameSet *fs=getFrameSet(m);
            if ( fs->isVisible() && !fs->isAHeader() && !fs->isAFooter() )
            {
                for (int n = fs->getNumFrames()-1; n >= 0 ; n--) {
                    //if ( n == fs->getNumFrames()-1 )
#ifdef DEBUG_PAGES
                    kdDebug(32002) << "KWDocument::recalcFrames frameset " << m << " " << fs->getName()
                                   << " frame " << n << " bottom=" << fs->getFrame(n)->bottom() << endl;
#endif
                    maxBottom = QMAX(maxBottom, fs->getFrame(n)->bottom());
                }
            }
        }
        int pages2 = static_cast<int>( ceil( maxBottom / ptPaperHeight() ) );
#ifdef DEBUG_PAGES
        kdDebug(32002) << "KWDocument::recalcFrames, WP, m_pages=" << m_pages << " pages2=" << pages2
                       << " (coming from maxBottom=" << maxBottom << " and ptPaperHeight=" << ptPaperHeight() << ")"
                       << endl;
#endif

        m_pages = QMAX( pages2, m_pages );
        if ( m_pages != oldPages )
        {
            emit pageNumChanged();
            recalcVariables( VT_PGNUM );
        }

        // Create new frames in the main text frameset
        // ( taking into account the header/footer sizes )
        for ( unsigned int j = 0;
              j < static_cast<unsigned int>( ceil( static_cast<double>( frms ) /
                                                   static_cast<double>( m_pageColumns.columns ) ) ); j++ ) {
            if ( j == 0 ) {
                headOffset = firstHeadOffset;
                footOffset = firstFootOffset;
            } else if ( ( ( j + 1 ) / 2 ) * 2 == j + 1 ) {
                headOffset = evenHeadOffset;
                footOffset = evenFootOffset;
            } else {
                headOffset = oddHeadOffset;
                footOffset = oddFootOffset;
            }

            for ( int i = 0; i < m_pageColumns.columns; i++ ) {
                if ( j * m_pageColumns.columns + i < frameset->getNumFrames() ) {
                    frameset->getFrame( j * m_pageColumns.columns + i )->setRect(
                        ptLeftBorder() + i * ( ptColumnWidth + ptColumnSpacing() ),
                        j * ptPaperHeight() + ptTopBorder() + headOffset,
                        ptColumnWidth,
                        ptPaperHeight() - ptTopBorder() - ptBottomBorder() -
                         headOffset - footOffset );
                } else {
                    KWFrame * frame = new KWFrame(frameset, ptLeftBorder() +
                                                  i * ( ptColumnWidth + ptColumnSpacing() ),
                                                  j * ptPaperHeight() + ptTopBorder() + headOffset,
                                                  ptColumnWidth, ptPaperHeight() -
                                                  ptTopBorder() - ptBottomBorder() -
                                                  headOffset - footOffset );
                    frameset->addFrame( frame );
                }
            }
        }

    }

    if ( isHeaderVisible() ) {
        switch ( getHeaderType() ) {
        case HF_SAME: {
            double h = evenHeader->getFrame( 0 )->height();
            for ( int l = 0; l < m_pages; l++ ) {
                if ( l < static_cast<int>( evenHeader->getNumFrames() ) )
                    evenHeader->getFrame( l )->setRect( ptLeftBorder(),
                                                        l * ptPaperHeight() + ptTopBorder(),
                                                        ptPaperWidth() -
                                                        ptLeftBorder() - ptRightBorder(), h );
                else
                {
                    KWFrame *frame = new KWFrame( evenHeader,ptLeftBorder(), l * ptPaperHeight() + ptTopBorder(),
                                                  ptPaperWidth() - ptLeftBorder() -
                                                  ptRightBorder(), h );
                    frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
                    frame->setNewFrameBehaviour( KWFrame::Copy );
                    frame->setCopy( true );
                    evenHeader->addFrame( frame );
                }
            }
            if ( m_pages < static_cast<int>( evenHeader->getNumFrames() ) ) {
                int diff = evenHeader->getNumFrames() - m_pages;
                for ( ; diff > 0; diff-- )
                    evenHeader->delFrame( evenHeader->getNumFrames() - 1 );
            }
        } break;
        case HF_EO_DIFF: {
            double h1 = evenHeader->getFrame( 0 )->height();
            double h2 = oddHeader->getFrame( 0 )->height();
            evenHeader->setCurrent( 0 );
            oddHeader->setCurrent( 0 );
            int even = 0, odd = 0;
            for ( int l = 0; l < m_pages; l++ ) {
                if ( ( ( l + 1 ) / 2 ) * 2 != l + 1 ) {
                    //kdDebug() << "KWDocument::recalcFrames considering page " << l << "(odd)" << endl;
                    odd++;
                    if ( static_cast<int>( oddHeader->getCurrent() ) <
                         static_cast<int>( oddHeader->getNumFrames() ) ) {
                        oddHeader->getFrame( oddHeader->getCurrent() )->setRect( ptLeftBorder(),
                                                                                 l * ptPaperHeight() +
                                                                                 ptTopBorder(),
                                                                                 ptPaperWidth() -
                                                                                 ptLeftBorder() -
                                                                                 ptRightBorder(), h2 );
                        oddHeader->setCurrent( oddHeader->getCurrent() + 1 );
                    } else {
#ifdef DEBUG_PAGES
                        kdDebug() << "KWDocument::recalcFrames creating new odd header" << endl;
#endif
                        KWFrame *frame = new KWFrame( oddHeader, ptLeftBorder(), l * ptPaperHeight() +
                                                      ptTopBorder(),
                                                      ptPaperWidth() - ptLeftBorder() -
                                                      ptRightBorder(), h2 );
                        frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
                        frame->setNewFrameBehaviour( KWFrame::Copy );
                        frame->setCopy( true );
                        oddHeader->addFrame( frame );
                    }
                } else {
                    //kdDebug() << "KWDocument::recalcFrames considering page " << l << "(even)" << endl;
                    even++;
                    if ( static_cast<int>( evenHeader->getCurrent() ) <
                         static_cast<int>( evenHeader->getNumFrames() ) ) {
                        evenHeader->getFrame( evenHeader->getCurrent() )->setRect( ptLeftBorder(),
                                                                                   l * ptPaperHeight() +
                                                                                   ptTopBorder(),
                                                                                   ptPaperWidth() -
                                                                                   ptLeftBorder() -
                                                                                   ptRightBorder(), h1 );
                        evenHeader->setCurrent( evenHeader->getCurrent() + 1 );
                    } else {
#ifdef DEBUG_PAGES
                        kdDebug() << "KWDocument::recalcFrames creating new even header" << endl;
#endif
                        KWFrame *frame = new KWFrame( evenHeader,ptLeftBorder(), l * ptPaperHeight() +
                                                      ptTopBorder(),
                                                      ptPaperWidth() - ptLeftBorder() -
                                                      ptRightBorder(), h1 );
                        frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
                        frame->setNewFrameBehaviour( KWFrame::Copy );
                        frame->setCopy( true );
                        evenHeader->addFrame( frame );
                    }
                }
            }
            if ( even + 1 < static_cast<int>( evenHeader->getNumFrames() ) ) {
                int diff = evenHeader->getNumFrames() - even;
#ifdef DEBUG_PAGES
                kdDebug() << "KWDocument::recalcFrames deleting " << diff << " even headers" << endl;
#endif
                for ( ; diff > 0; diff-- )
                    evenHeader->delFrame( evenHeader->getNumFrames() - 1 );
            }
            if ( odd + 1 < static_cast<int>( oddHeader->getNumFrames() ) ) {
                int diff = oddHeader->getNumFrames() - odd;
#ifdef DEBUG_PAGES
                kdDebug() << "KWDocument::recalcFrames deleting " << diff << " odd headers" << endl;
#endif
                for ( ; diff > 0; diff-- )
                    oddHeader->delFrame( oddHeader->getNumFrames() - 1 );
            }
            if ( m_pages == 1 && evenHeader->getNumFrames() > 0 ) {
#ifdef DEBUG_PAGES
                kdDebug() << "KWDocument::recalcFrames 1 page, " << evenHeader->getNumFrames() << " frames" << endl;
#endif
                // ???
                for ( unsigned int m = 0; m < evenHeader->getNumFrames(); m++ )
                    evenHeader->getFrame( m )->setRect( 0, ptPaperHeight() + h1,
                                                        ptPaperWidth() - ptLeftBorder() -
                                                        ptRightBorder(), h1 );
            }
        } break;
        case HF_FIRST_DIFF: {
            double h = firstHeader->getFrame( 0 )->height();
            firstHeader->getFrame( 0 )->setRect( ptLeftBorder(), ptTopBorder(),
                                                 ptPaperWidth() - ptLeftBorder() -
                                                 ptRightBorder(), h );
            if ( firstHeader->getNumFrames() > 1 ) {
                int diff = firstHeader->getNumFrames() - 1;
                for ( ; diff > 0; diff-- )
                    firstHeader->delFrame( firstHeader->getNumFrames() - 1 );
            }
            h = evenHeader->getFrame( 0 )->height();
            for ( int l = 1; l < m_pages; l++ ) {
                if ( l - 1 < static_cast<int>( evenHeader->getNumFrames() ) )
                    evenHeader->getFrame( l - 1 )->setRect( ptLeftBorder(), l * ptPaperHeight() +
                                                            ptTopBorder(),
                                                            ptPaperWidth() - ptLeftBorder() -
                                                            ptRightBorder(), h );
                else {
                    KWFrame *frame = new KWFrame( evenHeader, ptLeftBorder(), l * ptPaperHeight() + ptTopBorder(),
                                                  ptPaperWidth() - ptLeftBorder() -
                                                  ptRightBorder(), h );
                    frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
                    frame->setNewFrameBehaviour( KWFrame::Copy );
                    frame->setCopy( true );
                    evenHeader->addFrame( frame );
                }
            }
            if ( m_pages < static_cast<int>( evenHeader->getNumFrames() ) ) {
                int diff = evenHeader->getNumFrames() - m_pages;
                for ( ; diff > 0; diff-- )
                    evenHeader->delFrame( evenHeader->getNumFrames() - 1 );
            }
            if ( m_pages == 1 && evenHeader->getNumFrames() > 0 ) {
                for ( unsigned int m = 0; m < evenHeader->getNumFrames(); m++ )
                    evenHeader->getFrame( m )->setRect( 0, ptPaperHeight() + h,
                                                        ptPaperWidth() - ptLeftBorder() -
                                                        ptRightBorder(), h );
            }
        } break;
        }
    }

    if ( isFooterVisible() ) {
        switch ( getFooterType() ) {
        case HF_SAME: {
            double h = evenFooter->getFrame( 0 )->height();
            for ( int l = 0; l < m_pages; l++ ) {
                if ( l < static_cast<int>( evenFooter->getNumFrames() ) )
                    evenFooter->getFrame( l )->setRect( ptLeftBorder(), ( l + 1 ) * ptPaperHeight() -
                                                        ptBottomBorder() - h,
                                                        ptPaperWidth() - ptLeftBorder() -
                                                        ptRightBorder(), h );
                else {
                    KWFrame *frame = new KWFrame(evenFooter, ptLeftBorder(), ( l + 1 ) * ptPaperHeight() -
                                                  ptBottomBorder() - h,
                                                  ptPaperWidth() - ptLeftBorder() -
                                                  ptRightBorder(), h );
                    frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
                    frame->setNewFrameBehaviour( KWFrame::Copy );
                    frame->setCopy( true );
                    evenFooter->addFrame( frame );
                }
            }
            if ( m_pages < static_cast<int>( evenFooter->getNumFrames() ) ) {
                int diff = evenFooter->getNumFrames() - m_pages;
                for ( ; diff > 0; diff-- )
                    evenFooter->delFrame( evenFooter->getNumFrames() - 1 );
            }
        } break;
        case HF_EO_DIFF: {
            double h1 = evenFooter->getFrame( 0 )->height();
            double h2 = oddFooter->getFrame( 0 )->height();
            evenFooter->setCurrent( 0 );
            oddFooter->setCurrent( 0 );
            int even = 0, odd = 0;
            for ( int l = 0; l < m_pages; l++ ) {
                if ( ( ( l + 1 ) / 2 ) * 2 != l + 1 ) {
                    odd++;
                    if ( static_cast<int>( oddFooter->getCurrent() ) < static_cast<int>( oddFooter->getNumFrames() ) ) {
                        oddFooter->getFrame( oddFooter->getCurrent() )->setRect( ptLeftBorder(),
                                                                                 ( l + 1 )  *
                                                                                 ptPaperHeight() -
                                                                                 ptBottomBorder() - h2,
                                                                                 ptPaperWidth() -
                                                                                 ptLeftBorder() -
                                                                                 ptRightBorder(), h2 );
                        oddFooter->setCurrent( oddFooter->getCurrent() + 1 );
                    } else {
                        KWFrame *frame = new KWFrame(oddFooter, ptLeftBorder(),
                                                      ( l + 1 )  * ptPaperHeight() -
                                                      ptBottomBorder() - h2,
                                                      ptPaperWidth() - ptLeftBorder() -
                                                      ptRightBorder(), h2 );
                        frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
                        frame->setNewFrameBehaviour( KWFrame::Copy );
                        frame->setCopy( true );
                        oddFooter->addFrame( frame );
                    }
                } else {
                    even++;
                    if ( static_cast<int>( evenFooter->getCurrent() ) <
                         static_cast<int>( evenFooter->getNumFrames() ) ) {
                        evenFooter->getFrame( evenFooter->getCurrent() )->setRect( ptLeftBorder(),
                                                                                   ( l + 1 )  *
                                                                                   ptPaperHeight() -
                                                                                   ptBottomBorder() - h1,
                                                                                   ptPaperWidth() -
                                                                                   ptLeftBorder() -
                                                                                   ptRightBorder(), h1 );
                        evenFooter->setCurrent( evenFooter->getCurrent() + 1 );
                    } else {
                        KWFrame *frame = new KWFrame(evenFooter, ptLeftBorder(),
                                                      ( l + 1 )  * ptPaperHeight() -
                                                      ptBottomBorder() - h1,
                                                      ptPaperWidth() - ptLeftBorder() -
                                                      ptRightBorder(), h1 );
                        frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
                        frame->setNewFrameBehaviour( KWFrame::Copy );
                        frame->setCopy( true );
                        evenFooter->addFrame( frame );
                    }
                }
            }
            if ( even + 1 < static_cast<int>( evenFooter->getNumFrames() ) ) {
                int diff = evenFooter->getNumFrames() - even;
                for ( ; diff > 0; diff-- )
                    evenFooter->delFrame( evenFooter->getNumFrames() - 1 );
            }
            if ( odd + 1 < static_cast<int>( oddFooter->getNumFrames() ) ) {
                int diff = oddFooter->getNumFrames() - odd;
                for ( ; diff > 0; diff-- )
                    oddFooter->delFrame( oddFooter->getNumFrames() - 1 );
            }
            if ( m_pages == 1 && evenFooter->getNumFrames() > 0 ) {
                for ( unsigned int m = 0; m < evenFooter->getNumFrames(); m++ )
                    evenFooter->getFrame( m )->setRect( 0, m_pages * ptPaperHeight() + h1,
                                                        ptPaperWidth() - ptLeftBorder() -
                                                        ptRightBorder(), h1 );
            }
        } break;
        case HF_FIRST_DIFF: {
            double h = firstFooter->getFrame( 0 )->height();
            firstFooter->getFrame( 0 )->setRect( ptLeftBorder(), ptPaperHeight() - ptBottomBorder() - h,
                                                 ptPaperWidth() - ptLeftBorder() - ptRightBorder(), h );
            if ( firstFooter->getNumFrames() > 1 ) {
                int diff = firstFooter->getNumFrames() - 1;
                for ( ; diff > 0; diff-- )
                    firstFooter->delFrame( firstFooter->getNumFrames() - 1 );
            }
            h = evenFooter->getFrame( 0 )->height();
            for ( int l = 1; l < m_pages; l++ ) {
                if ( l - 1 < static_cast<int>( evenFooter->getNumFrames() ) )
                    evenFooter->getFrame( l - 1 )->setRect( ptLeftBorder(), ( l + 1 ) *
                                                            ptPaperHeight() - ptBottomBorder() - h,
                                                            ptPaperWidth() - ptLeftBorder() -
                                                            ptRightBorder(), h );
                else {
                    KWFrame *frame = new KWFrame(evenFooter, ptLeftBorder(), ( l + 1 ) * ptPaperHeight() -
                                                  ptBottomBorder() - h,
                                                  ptPaperWidth() - ptLeftBorder() -
                                                  ptRightBorder(), h );
                    frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
                    frame->setNewFrameBehaviour( KWFrame::Copy );
                    frame->setCopy( true );
                    evenFooter->addFrame( frame );
                }
            }
            if ( m_pages < static_cast<int>( evenFooter->getNumFrames() ) ) {
                int diff = evenFooter->getNumFrames() - m_pages;
                for ( ; diff > 0; diff-- )
                    evenFooter->delFrame( evenFooter->getNumFrames() - 1 );
            }
            if ( m_pages == 1 && evenFooter->getNumFrames() > 0 ) {
                for ( unsigned int m = 0; m < evenFooter->getNumFrames(); m++ )
                    evenFooter->getFrame( m )->setRect( 0, m_pages * ptPaperHeight() + h,
                                                        ptPaperWidth() - ptLeftBorder() -
                                                        ptRightBorder(), h );
            }
        } break;
        }
    }

    emit newContentsSize();
}

KWDocument::~KWDocument()
{
    delete m_autoFormat;
    delete m_formulaDocument;
    delete m_commandHistory;
}

bool KWDocument::loadChildren( KoStore *_store )
{
    QListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        if ( !((KoDocumentChild*)it.current())->loadDocument( _store ) )
            return FALSE;
    }

    return TRUE;
}

bool KWDocument::loadXML( QIODevice *, const QDomDocument & doc )
{
    QTime dt;
    dt.start();
    emit sigProgress( 0 );
    kdDebug(32001) << "KWDocument::loadXML" << endl;
    delete m_pixmapMap;
    m_pixmapMap = 0L;
    delete m_clipartMap;
    m_clipartMap = 0L;
    m_imageRequests.clear();
    m_imageRequests2.clear();
    m_anchorRequests.clear();
    m_clipartRequests.clear();

    m_pageLayout.unit = PG_MM;

    m_pageColumns.columns = 1;
    m_pageColumns.ptColumnSpacing = s_defaultColumnSpacing;

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.inchHeaderBodySpacing = POINT_TO_INCH( 10 );
    m_pageHeaderFooter.inchFooterBodySpacing = POINT_TO_INCH( 10 );
    m_pageHeaderFooter.mmHeaderBodySpacing = POINT_TO_MM( 10 );
    m_pageHeaderFooter.mmFooterBodySpacing = POINT_TO_MM( 10 );

    m_mapVariableFormats.clear();

    m_pages = 1;

    KoPageLayout __pgLayout;
    __pgLayout.unit = PG_MM;
    KoColumns __columns;
    KoKWHeaderFooter __hf;
    __hf.header = HF_SAME;
    __hf.footer = HF_SAME;
    __hf.ptHeaderBodySpacing = 10.0;
    __hf.ptFooterBodySpacing = 10.0;
    __hf.mmHeaderBodySpacing = POINT_TO_MM( 10 );
    __hf.mmFooterBodySpacing = POINT_TO_MM( 10 );
    __hf.inchHeaderBodySpacing = POINT_TO_INCH( 10 );
    __hf.inchFooterBodySpacing = POINT_TO_INCH( 10 );


    QString value;
    QDomElement word = doc.documentElement();
    unsigned item;

    value = KWDocument::getAttribute( word, "mime", QString::null );
    if ( value != "application/x-kword" )
    {
        kdError(32001) << "Unknown mime type " << value << endl;
        return false;
    }
    m_syntaxVersion = KWDocument::getAttribute( word, "syntaxVersion", 0 );
    if ( m_syntaxVersion > CURRENT_SYNTAX_VERSION )
    {
        int ret = KMessageBox::warningContinueCancel(
            0, i18n("This document was created with a newer version of KWord (syntax version: %1)\n"
                    "Opening it in this version of KWord will lose some information.").arg(m_syntaxVersion),
            i18n("File format mismatch"), i18n("Continue") );
        if ( ret == KMessageBox::Cancel )
            return false;
    }

    // Looks like support for the old way of naming images internally,
    // see completeLoading.
    value = KWDocument::getAttribute( word, "url", QString::null );
    if ( value != QString::null )
    {
        urlIntern = KURL( value ).path();
    }

    emit sigProgress(5);

    // <PAPER>
    QDomElement paper = word.namedItem( "PAPER" ).toElement();
    if ( !paper.isNull() )
    {
        __pgLayout.format = static_cast<KoFormat>( KWDocument::getAttribute( paper, "format", 0 ) );
        __pgLayout.orientation = static_cast<KoOrientation>( KWDocument::getAttribute( paper, "orientation", 0 ) );
        getPointBasedAttribute( __pgLayout, Width, paper, "width", 0.0 );
        getPointBasedAttribute( __pgLayout, Height, paper, "height", 0.0 );
        __hf.header = static_cast<KoHFType>( KWDocument::getAttribute( paper, "hType", 0 ) );
        __hf.footer = static_cast<KoHFType>( KWDocument::getAttribute( paper, "fType", 0 ) );
        getPointBasedAttribute( __hf, HeaderBodySpacing, paper, "spHeadBody", 0.0 );
        getPointBasedAttribute( __hf, FooterBodySpacing, paper, "spFootBody", 0.0 );
        __columns.columns = KWDocument::getAttribute( paper, "columns", 1 );
        __columns.ptColumnSpacing = KWDocument::getAttribute( paper, "columnspacing", 0.0 );
        m_zoom = KWDocument::getAttribute( paper, "zoom", 100 );
        if(m_zoom!=100)
            setZoomAndResolution( m_zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY(), false, false );
        // Support the undocumented syntax actually used by KDE 2.0 for some of the above (:-().
        if ( __pgLayout.ptWidth == 0.0 )
            getPointBasedAttribute( __pgLayout, Width, paper, "ptWidth", 0.0 );
        if ( __pgLayout.ptHeight == 0.0 )
            getPointBasedAttribute( __pgLayout, Height, paper, "ptHeight", 0.0 );
        if ( __hf.ptHeaderBodySpacing == 0.0 )
            getPointBasedAttribute( __hf, HeaderBodySpacing, paper, "ptHeadBody", 0.0 );
        if ( __hf.ptFooterBodySpacing == 0.0 )
            getPointBasedAttribute( __hf, FooterBodySpacing, paper, "ptFootBody", 0.0 );
        if ( __columns.ptColumnSpacing == 0.0 )
            __columns.ptColumnSpacing = KWDocument::getAttribute( paper, "ptColumnspc", 0.0 );

        // <PAPERBORDERS>
        QDomElement paperborders = paper.namedItem( "PAPERBORDERS" ).toElement();
        if ( !paperborders.isNull() )
        {
            getPointBasedAttribute( __pgLayout, Left, paperborders, "left", 0.0 );
            getPointBasedAttribute( __pgLayout, Top, paperborders, "top", 0.0 );
            getPointBasedAttribute( __pgLayout, Right, paperborders, "right", 0.0 );
            getPointBasedAttribute( __pgLayout, Bottom, paperborders, "bottom", 0.0 );

            // Support the undocumented syntax actually used by KDE 2.0 for some of the above (:-().
            if ( __pgLayout.ptLeft == 0.0 )
                getPointBasedAttribute( __pgLayout, Left, paperborders, "ptLeft", 0.0 );
            if ( __pgLayout.ptTop == 0.0 )
                getPointBasedAttribute( __pgLayout, Top, paperborders, "ptTop", 0.0 );
            if ( __pgLayout.ptRight == 0.0 )
                getPointBasedAttribute( __pgLayout, Right, paperborders, "ptRight", 0.0 );
            if ( __pgLayout.ptBottom == 0.0 )
                getPointBasedAttribute( __pgLayout, Bottom, paperborders, "ptBottom", 0.0 );
        }
    }

    // <ATTRIBUTES>
    QDomElement attributes = word.namedItem( "ATTRIBUTES" ).toElement();
    QString unitName;
    if ( !attributes.isNull() )
    {
        m_processingType = static_cast<ProcessingType>( KWDocument::getAttribute( attributes, "processing", 0 ) );
        //KWDocument::getAttribute( attributes, "standardpage", QString::null );
        m_headerVisible = static_cast<bool>( KWDocument::getAttribute( attributes, "hasHeader", 0 ) );
        m_footerVisible = static_cast<bool>( KWDocument::getAttribute( attributes, "hasFooter", 0 ) );
        unitName = correctQString( KWDocument::getAttribute( attributes, "unit", "pt" ) );
    } else {
        m_processingType = WP;
        m_headerVisible = false;
        m_footerVisible = false;
        unitName = "pt";
    }
    m_unit = KWUnit::unit( unitName );
    switch ( m_unit ) {
    case KWUnit::U_MM: __pgLayout.unit = PG_MM;
        break;
    case KWUnit::U_PT: __pgLayout.unit = PG_PT;
        break;
    case KWUnit::U_INCH: __pgLayout.unit = PG_INCH;
        break;
    }
    setPageLayout( __pgLayout, __columns, __hf );

    emit sigProgress(10);

#if 0
        else if ( name == "FOOTNOTEMGR" ) {
            parser.parseTag( tag, name, lst );
            footNoteManager.load( parser, lst );
        }
#endif
/*
        else if ( name == "SERIALL" ) {
            parser.parseTag( tag, name, lst );
            slDataBase->load( parser, lst );
        } */

    emit sigProgress(15);

    // Load all styles before the corresponding paragraphs try to use them!
    QDomElement stylesElem = word.namedItem( "STYLES" ).toElement();
    if ( !stylesElem.isNull() )
        loadStyleTemplates( stylesElem );

    emit sigProgress(20);

    QDomElement framesets = word.namedItem( "FRAMESETS" ).toElement();
    if ( !framesets.isNull() )
        loadFrameSets( framesets );

    emit sigProgress(85);

    QDateTime defaultDateTime = QDateTime::currentDateTime();
    // <PIXMAPS>
    QDomElement pixmapsElem = word.namedItem( "PIXMAPS" ).toElement();
    if ( !pixmapsElem.isNull() )
    {
        m_pixmapMap = new QMap<KoImageKey, QString>( m_imageCollection.readXML( pixmapsElem, defaultDateTime ) );
    }

    // <CLIPARTS>
    QDomElement clipartsElem = word.namedItem( "CLIPARTS" ).toElement();
    if ( !clipartsElem.isNull() )
    {
        m_clipartMap = new QMap<KoClipartKey, QString>( m_clipartCollection.readXML( clipartsElem, defaultDateTime ) );
    }

    emit sigProgress(90);

    // <EMBEDDED>
    QDomNodeList listEmbedded = word.elementsByTagName ( "EMBEDDED" );
    for (item = 0; item < listEmbedded.count(); item++)
    {
        QDomElement embedded = listEmbedded.item( item ).toElement();
        loadEmbedded( embedded );
    }

    emit sigProgress(95);

#if 0 // Not needed anymore
    // <CPARAGS>  (Ids of the parags that form the Table of Contents)
    QDomNodeList listCparags = word.elementsByTagName( "CPARAGS" );
    for (item = 0; item < listCparags.count(); item++)
    {
        QDomElement cparag = listCparags.item( item ).toElement();
        QDomElement parag = cparag.namedItem( "PARAG" ).toElement();
        if ( !parag.isNull() )
        {
            value = parag.attribute( "name", QString::null );
            if ( value != QString::null )
                contents->addParagId( value.toInt() );
        }
    }
#endif

    emit sigProgress(100); // the rest is only processing, not loading

    bool _first_footer = FALSE, _even_footer = FALSE, _odd_footer = FALSE;
    bool _first_header = FALSE, _even_header = FALSE, _odd_header = FALSE;
    bool _footnotes = FALSE;

    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        switch( fit.current()->frameSetInfo() ) {
        case KWFrameSet::FI_FIRST_HEADER: _first_header = TRUE; break;
        case KWFrameSet::FI_EVEN_HEADER: _odd_header = TRUE; break;
        case KWFrameSet::FI_ODD_HEADER: _even_header = TRUE; break;
        case KWFrameSet::FI_FIRST_FOOTER: _first_footer = TRUE; break;
        case KWFrameSet::FI_EVEN_FOOTER: _odd_footer = TRUE; break;
        case KWFrameSet::FI_ODD_FOOTER: _even_footer = TRUE; break;
        case KWFrameSet::FI_FOOTNOTE: _footnotes = TRUE; break;
        default: break;
        }
    }

    // create defaults if they were not in the input file.

    if ( !_first_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "First Page Header" ) );
        kdDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameSetInfo( KWFrameSet::FI_FIRST_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
            ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        kdDebug(32001) << "KWDocument::loadXML KWFrame created " << frame << endl;
        frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehaviour( KWFrame::Copy );
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_even_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Even Pages Header" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_EVEN_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
            ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehaviour( KWFrame::Copy );
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_odd_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Odd Pages Header" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_ODD_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
            ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehaviour( KWFrame::Copy );
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_first_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "First Page Footer" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_FIRST_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
            ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
            ptRightBorder(), 20 );
        frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehaviour( KWFrame::Copy );
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_even_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Even Pages Footer" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_EVEN_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
            ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
            ptRightBorder(), 20 );
        frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehaviour( KWFrame::Copy );
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_odd_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Odd Pages Footer" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_ODD_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
            ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
            ptRightBorder(), 20 );
        frame->setFrameBehaviour( KWFrame::AutoExtendFrame );
        frame->setNewFrameBehaviour( KWFrame::Copy );
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_footnotes ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Footnotes" ) );
        fs->setFrameSetInfo( KWFrameSet::FI_FOOTNOTE );

        for ( int i = 0; i < m_pages; i++ ) {
            KWFrame *frame = new KWFrame(fs, ptLeftBorder(),
                i * ptPaperHeight() + ptPaperHeight() - ptTopBorder() - 20,
                ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
            frame->setFrameBehaviour(KWFrame::AutoExtendFrame);
            fs->addFrame( frame );
        }
        frames.append( fs );
        fs->setVisible( FALSE );
    }

#if 0 // already done !
    KWChild *ch = 0L;
    for ( ch = m_lstChildren.first(); ch != 0; ch = m_lstChildren.next() ) {
        KWPartFrameSet *frameset = new KWPartFrameSet( this, ch );
        QRect r = ch->geometry();
        KWFrame *frame = new KWFrame(frameset, r.x(), r.y(), r.width(), r.height() );
        frameset->addFrame( frame );
        frames.append( frameset );
        emit sig_insertObject( ch, frameset );
    }
#endif

    // do some sanity checking on document.
    for (int i = getNumFrameSets()-1; i>-1; i--) {
        if(! getFrameSet(i)) {
            kdWarning() << "frameset " << i << " is NULL!!" << endl;
            frames.remove(i);
        } else if( getFrameSet(i)->type()==FT_TABLE) {
            static_cast<KWTableFrameSet *>( getFrameSet(i))->validate();
        } else if(! getFrameSet(i)->getFrame(0)) {
            kdWarning () << "frameset " << i << " has no frames" << endl;
            KWFrameSet * fs = getFrameSet(i);
            removeFrameSet(fs);
            delete fs;
        } else if (getFrameSet(i)->type() == FT_TEXT) {
            for (int f=getFrameSet(i)->getNumFrames()-1; f>=0; f--) {
                if(getFrameSet(i)->getFrame(f)->height() < static_cast <int>(minFrameHeight)) {
                    kdWarning() << "frame height is so small no text will fit, adjusting (was: "
                      << getFrameSet(i)->getFrame(f)->height() << " is: " << minFrameHeight << ")" << endl;
                    getFrameSet(i)->getFrame(f)->setHeight(minFrameHeight);
                }
                if(getFrameSet(i)->getFrame(f)->width() < static_cast <int>(minFrameWidth)) {
                    kdWarning() << "frame width is so small no text will fit, adjusting (was: "
                     << getFrameSet(i)->getFrame(f)->width() << " is: " << minFrameWidth  << ")" << endl;
                    getFrameSet(i)->getFrame(f)->setWidth(minFrameWidth);
                }
            }
        }
    }
    emit sigProgress(-1);

    recalcFrames(); // This computes the number of pages (from the frames)
                    // for the first time (and adds footers/headers etc.)

#if 0 // If KWCanvas calls updateViewArea right, this is not needed anymore
    kdDebug(32002) << "KWDocument::loadXML starting formatting" << endl;
    // So now we can start formatting
    fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWTextFrameSet * frameset = dynamic_cast<KWTextFrameSet *>(fit.current());
        if ( frameset )
        {
            frameset->formatMore(); // creates more pages/frames if necessary
        }
    }
#endif

    repaintAllViews( true );     // in case any view exists already

    //kdDebug(32001) << "KWDocument::loadXML done" << endl;

    setModified( false );

    kdDebug(32001) << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;

    return TRUE;
}

void KWDocument::loadEmbedded( QDomElement embedded )
{
    QDomElement object = embedded.namedItem( "OBJECT" ).toElement();
    if ( !object.isNull() )
    {
        KWChild *ch = new KWChild( this );
        ch->load( object, true );
        insertChild( ch );
        QDomElement settings = embedded.namedItem( "SETTINGS" ).toElement();
        QString name;
        if ( !settings.isNull() )
            name = settings.attribute( "name" );
        KWPartFrameSet *fs = new KWPartFrameSet( this, ch, name );
        frames.append( fs );
        if ( !settings.isNull() )
        {
            kdDebug(32001) << "KWDocument::loadXML loading embedded object" << endl;
            fs->load( settings );
        }
        else
            kdError(32001) << "No <SETTINGS> tag in EMBEDDED" << endl;
        emit sig_insertObject( ch, fs );
    } else
        kdError(32001) << "No <OBJECT> tag in EMBEDDED" << endl;
}

void KWDocument::loadStyleTemplates( QDomElement stylesElem )
{
    QValueList<QString> followingStyles;
    QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWStyle *sty = new KWStyle( styleElem, this, m_defaultFont );

        sty = addStyleTemplate( sty );

        if(m_styleList.count() > followingStyles.count() )
        {
            QString following = styleElem.namedItem("FOLLOWING").toElement().attribute("name");
            followingStyles.append( following );
        }
        else
            kdWarning () << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
    }

    ASSERT( followingStyles.count() == m_styleList.count() );

    unsigned int i=0;
    for( QValueList<QString>::Iterator it = followingStyles.begin(); it != followingStyles.end(); ++it ) {
        KWStyle * style = findStyle(*it);
        m_styleList.at(i++)->setFollowingStyle( style );
    }

}

KWStyle* KWDocument::addStyleTemplate( KWStyle * sty )
{
    // First check for duplicates.
    for ( KWStyle* p = m_styleList.first(); p != 0L; p = m_styleList.next() )
    {
        if ( p->name() == sty->name() ) {
            // Replace existing style
            if ( sty != p )
            {
                *p = *sty;
                delete sty;
            }
            return p;
        }
    }
    m_styleList.append( sty );
    return sty;
}

void KWDocument::removeStyleTemplate ( KWStyle *style ) {
    if( m_styleList.removeRef(style)) {
        m_deletedStyles.append(style);
    }
}

void KWDocument::moveDownStyleTemplate ( const QString & _styleName )
{
    unsigned int pos = 0;
    for ( KWStyle* p = m_styleList.first(); p != 0L; p = m_styleList.next(), ++pos )
    {
        if ( p->name() == _styleName )
        {
            KWStyle * next = m_styleList.at(pos+1);
            if (!next) return;
            // We have "p" "next" and we want "next" "p"
            m_styleList.insert( pos, next ); // "next", "p", "next"
            m_styleList.take( pos+2 );       // Remove last "next"
            return;
        }
    }
}

void KWDocument::moveUpStyleTemplate ( const QString & _styleName )
{
    unsigned int pos = 0;
    for ( KWStyle* p = m_styleList.first(); p != 0L; p = m_styleList.next(), ++pos )
    {
        if ( p->name() == _styleName )
        {
            // We have "prev" "p" and we want "p" "prev"
            m_styleList.insert( pos-1, p ); // "p" "prev" "p"
            m_styleList.take( pos+1 );      // Remove last "p"
            return;
        }
    }
}

void KWDocument::progressItemLoaded()
{
    m_itemsLoaded++;
    // We progress from 20 to 85 -> 65-wide range, 20 offset.
    unsigned int perc = 65 * m_itemsLoaded / m_nrItemsToLoad;
    if ( perc != 65 * (m_itemsLoaded-1) / m_nrItemsToLoad ) // only emit if different from previous call
    {
        //kdDebug(32001) << m_itemsLoaded << " items loaded. %=" << perc + 20 << endl;
        emit sigProgress( perc + 20 );
    }
}

void KWDocument::loadFrameSets( QDomElement framesetsElem )
{
    // <FRAMESET>
    // First prepare progress info
    m_nrItemsToLoad = 0; // total count of items (mostly paragraph and frames)
    QDomElement framesetElem = framesetsElem.firstChild().toElement();
    // Workaround the slowness of QDom's elementsByTagName
    QValueList<QDomElement> framesets;
    for ( ; !framesetElem.isNull() ; framesetElem = framesetElem.nextSibling().toElement() )
    {
        if ( framesetElem.tagName() == "FRAMESET" )
        {
            framesets.append( framesetElem );
            m_nrItemsToLoad += framesetElem.childNodes().count();
        }
    }

    m_itemsLoaded = 0;

    QValueList<QDomElement>::Iterator it = framesets.begin();
    QValueList<QDomElement>::Iterator end = framesets.end();
    for ( ; it != end ; ++it )
    {
        (void) loadFrameSet( *it );
    }
}

KWFrameSet * KWDocument::loadFrameSet( QDomElement framesetElem, bool loadFrames )
{
    FrameSetType frameSetType = static_cast<FrameSetType>( KWDocument::getAttribute( framesetElem, "frameType", FT_BASE ) );
    QString tableName = correctQString( KWDocument::getAttribute( framesetElem, "grpMgr", "" ) );
    QString fsname = correctQString( KWDocument::getAttribute( framesetElem, "name", "" ) );

    switch ( frameSetType ) {
    case FT_TEXT: {
        if ( !tableName.isEmpty() ) {
            // Text frameset belongs to a table -> find table by name
            KWTableFrameSet *table = 0L;
            QListIterator<KWFrameSet> fit = framesetsIterator();
            for ( ; fit.current() ; ++fit ) {
                KWFrameSet *f = fit.current();
                if( f->type() == FT_TABLE &&
                    f->isVisible() &&
                    f->getName() == tableName ) {
                    table = static_cast<KWTableFrameSet *> (f);
                    break;
                }
            }
            if ( !table ) {
                table = new KWTableFrameSet( this, tableName );
                frames.append( table );
            }
            int _row = KWDocument::getAttribute( framesetElem, "row", 0 );
            int _col = KWDocument::getAttribute( framesetElem, "col", 0 );
            KWTableFrameSet::Cell *cell = new KWTableFrameSet::Cell( table, _row, _col, fsname );
            cell->load( framesetElem, loadFrames );
            cell->m_rows = getAttribute( framesetElem, "rows", 1 );
            cell->m_cols = getAttribute( framesetElem, "cols", 1 );
            return cell;
        }
        else
        {
            KWTextFrameSet *fs = new KWTextFrameSet( this, fsname );
            fs->load( framesetElem, loadFrames );
            frames.append( fs ); // don't use addFrameSet here. We'll call finalize() once and for all in completeLoading

            // Old file format had autoCreateNewFrame as a frameset attribute
            if ( framesetElem.hasAttribute( "autoCreateNewFrame" ) )
            {
                KWFrame::FrameBehaviour behav = static_cast<KWFrame::FrameBehaviour>( framesetElem.attribute( "autoCreateNewFrame" ).toInt() );
                QListIterator<KWFrame> frameIt( fs->frameIterator() );
                for ( ; frameIt.current() ; ++frameIt ) // Apply it to all frames
                    frameIt.current()->setFrameBehaviour( behav );
            }
            return fs;
        }
    } break;
    case FT_PICTURE: {
        KWPictureFrameSet *fs = new KWPictureFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        frames.append( fs );
        return fs;
    } break;
    case FT_CLIPART: {
        KWClipartFrameSet *fs = new KWClipartFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        frames.append( fs );
        return fs;
    } break;
    case FT_FORMULA: {
        KWFormulaFrameSet *fs = new KWFormulaFrameSet( this, fsname );
        fs->load( framesetElem, loadFrames );
        frames.append( fs );
        return fs;
    } break;
    // Note that FT_PART cannot happen when loading from a file (part frames are saved into the SETTINGS tag)
    // But this can happen when pasting !
    // However cloning a KoDocumentChild isn't yet supported...
    case FT_PART: {
        /*
        KWPartFrameSet *fs = new KWPartFrameSet( this, ......, fsname );
        fs->load( framesetElem, loadFrames );
        frames.append( fs );
        return fs;
        */
        kdWarning(32001) << "Copying part objects isn't implemented yet" << endl;
    } break;
    case FT_TABLE:
        kdWarning(32001) << "Copying tables isn't implemented yet" << endl;
        break;
    case FT_BASE:
        kdWarning(32001) << "FT_BASE !?!?" << endl;
        break;
    }
    return 0L;
}

bool KWDocument::completeLoading( KoStore *_store )
{
    if ( _store ) {
        QString prefix = urlIntern.isEmpty() ? url().path() : urlIntern;
        prefix += '/';
        if ( m_pixmapMap ) {
            m_imageCollection.readFromStore( _store, *m_pixmapMap, prefix );
            delete m_pixmapMap;
            m_pixmapMap = 0L;
        } else if ( m_clipartMap ) {
            m_clipartCollection.readFromStore( _store, *m_clipartMap, prefix );
            delete m_clipartMap;
            m_clipartMap = 0L;
        }
    }

    processImageRequests();
    processAnchorRequests();

    // The fields from documentinfo.xml just got loaded -> update vars
    recalcVariables( VT_FIELD );

    // Finalize all the existing framesets
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->finalize();

    return TRUE;
}

void KWDocument::processImageRequests()
{
    QMapIterator<KoImageKey,KWTextImage *> it2 = m_imageRequests.begin();
    for ( ; it2 != m_imageRequests.end(); ++it2 )
    {
        kdDebug(32001) << "KWDocument::completeLoading loading image " << it2.key().toString() << endl;
        it2.data()->setImage( m_imageCollection.findImage( it2.key() ) );
    }
    m_imageRequests.clear();

    QListIterator<KWPictureFrameSet> it3( m_imageRequests2 );
    for ( ; it3.current(); ++it3 )
        it3.current()->setImage( m_imageCollection.findImage( it3.current()->key() ) );
    m_imageRequests2.clear();

    QListIterator<KWClipartFrameSet> it4( m_clipartRequests );
    for ( ; it4.current(); ++it4 )
        it4.current()->setClipart( m_clipartCollection.findClipart( it4.current()->key() ) );
    m_clipartRequests.clear();
}

void KWDocument::processAnchorRequests()
{
    QMapIterator<QString, KWAnchorPosition> itanch = m_anchorRequests.begin();
    for ( ; itanch != m_anchorRequests.end(); ++itanch )
    {
        QString fsname = itanch.key();
        if ( m_pasteFramesetsMap && m_pasteFramesetsMap->contains( fsname ) )
            fsname = (*m_pasteFramesetsMap)[ fsname ];
        kdDebug(32001) << "KWDocument::completeLoading anchoring frameset " << fsname << endl;
        KWFrameSet * fs = getFrameSetByName( fsname );
        ASSERT( fs );
        if ( fs )
            fs->setAnchored( itanch.data().textfs, itanch.data().paragId, itanch.data().index, true );
    }
    m_anchorRequests.clear();
}

void KWDocument::pasteFrames( QDomElement topElem, KMacroCommand * macroCmd )
{
    m_pasteFramesetsMap = new QMap<QString, QString>();
    int ref=0;

    QDomElement elem = topElem.firstChild().toElement();
    for ( ; !elem.isNull() ; elem = elem.nextSibling().toElement() )
    {
        QDomElement frameElem;
        KWFrameSet * fs = 0L;
        if ( elem.tagName() == "FRAME" )
        {
            QString frameSetName = frameElem.attribute( "parentFrameset" );
            fs = getFrameSetByName( frameSetName );
            if ( !fs )
            {
                kdWarning(32001) << "pastFrames: Frameset '" << frameSetName << "' not found" << endl;
                continue;
            }
            frameElem = elem;
        }
        else if ( elem.tagName() == "FRAMESET" )
        {
            fs = loadFrameSet( elem, false );
            frameElem = elem.namedItem( "FRAME" ).toElement();
        }
        // Test commented out since the toplevel element can contain "PARAGRAPH" now
        //else
        //kdWarning(32001) << "Unsupported toplevel-element in KWCanvas::pasteFrames : '" << elem.tagName() << "'" << endl;

        if ( fs && !frameElem.isNull() )
        {
            double offs = 20.0;
            KoRect rect;
            rect.setLeft( KWDocument::getAttribute( frameElem, "left", 0.0 ) + offs );
            rect.setTop( KWDocument::getAttribute( frameElem, "top", 0.0 ) + offs );
            rect.setRight( KWDocument::getAttribute( frameElem, "right", 0.0 ) + offs );
            rect.setBottom( KWDocument::getAttribute( frameElem, "bottom", 0.0 ) + offs );
            KWFrame * frame = new KWFrame( fs, rect.x(), rect.y(), rect.width(), rect.height() );
            frame->load( frameElem, fs->isHeaderOrFooter(), KWDocument::CURRENT_SYNTAX_VERSION );
            QString newName=i18n("Copy-%1").arg(fs->getName());
            newName = generateFramesetName( newName+"-%1" );
            m_pasteFramesetsMap->insert( fs->getName(), newName ); // remember the name transformation
            fs->setName( newName );
            fs->addFrame( frame );
            if ( macroCmd )
            {
                KWCreateFrameCommand *cmd = new KWCreateFrameCommand( QString::null, frame );
                macroCmd->addCommand(cmd);
            }
            fs->finalize();
            int type=0;
            // Please move this to some common method somewhere (e.g. in KWDocument) (David)
            switch(fs->type())
            {
            case FT_TEXT:
                type=(int)TextFrames;
                break;
            case FT_PICTURE:
                type=(int)Pictures;
                break;
            case FT_PART:
                type=(int)Embedded;
                break;
            case FT_FORMULA:
                type=(int)FormulaFrames;
                break;
            case FT_TABLE:
                type=(int)Tables;
                break;
            default:
                type=(int)TextFrames;
            }
            ref|=type;
        }
    }
    processImageRequests();
    processAnchorRequests();
    repaintAllViews();
    refreshDocStructure(ref);
    delete m_pasteFramesetsMap;
    m_pasteFramesetsMap = 0L;
}

QDomDocument KWDocument::saveXML()
{
    QDomDocument doc( "DOC" );
    doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement kwdoc = doc.createElement( "DOC" );
    kwdoc.setAttribute( "editor", "KWord" );
    kwdoc.setAttribute( "mime", "application/x-kword" );
    m_syntaxVersion = CURRENT_SYNTAX_VERSION;
    kwdoc.setAttribute( "syntaxVersion", m_syntaxVersion );
    doc.appendChild( kwdoc );

    QDomElement paper = doc.createElement( "PAPER" );
    kwdoc.appendChild( paper );
    paper.setAttribute( "format", static_cast<int>( m_pageLayout.format ) );
    paper.setAttribute( "width", m_pageLayout.ptWidth );
    paper.setAttribute( "height", m_pageLayout.ptHeight );
    paper.setAttribute( "orientation", static_cast<int>( m_pageLayout.orientation ) );
    paper.setAttribute( "columns", m_pageColumns.columns );
    paper.setAttribute( "columnspacing", m_pageColumns.ptColumnSpacing );
    paper.setAttribute( "hType", static_cast<int>( m_pageHeaderFooter.header ) );
    paper.setAttribute( "fType", static_cast<int>( m_pageHeaderFooter.footer ) );
    paper.setAttribute( "spHeadBody", m_pageHeaderFooter.ptHeaderBodySpacing );
    paper.setAttribute( "spFootBody", m_pageHeaderFooter.ptFooterBodySpacing );

    paper.setAttribute( "zoom",m_zoom );

    QDomElement borders = doc.createElement( "PAPERBORDERS" );
    paper.appendChild( borders );
    borders.setAttribute( "left", m_pageLayout.ptLeft );
    borders.setAttribute( "top", m_pageLayout.ptTop );
    borders.setAttribute( "right", m_pageLayout.ptRight );
    borders.setAttribute( "bottom", m_pageLayout.ptBottom );

    QDomElement docattrs = doc.createElement( "ATTRIBUTES" );
    kwdoc.appendChild( docattrs );
    docattrs.setAttribute( "processing", static_cast<int>( m_processingType ) );
    docattrs.setAttribute( "standardpage", 1 );
    docattrs.setAttribute( "hasHeader", static_cast<int>(isHeaderVisible()) );
    docattrs.setAttribute( "hasFooter", static_cast<int>(isFooterVisible()) );
    docattrs.setAttribute( "unit", KWUnit::unitName(getUnit()) );

//    out << otag << "<FOOTNOTEMGR>" << endl;
//    footNoteManager.save( out );
//    out << etag << "</FOOTNOTEMGR>" << endl;

    QDomElement framesets = doc.createElement( "FRAMESETS" );
    kwdoc.appendChild( framesets );

    QValueList<KoImageKey> saveImages;
    QValueList<KoClipartKey> saveCliparts;
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        // Save non-part framesets ( part are saved further down )
        if ( frameSet->type() != FT_PART )
            frameSet->save( framesets );

        // If picture frameset, make a note of the image it needs.
        if ( !frameSet->isDeleted() && frameSet->type() == FT_PICTURE )
        {
            KoImageKey key = static_cast<KWPictureFrameSet *>( frameSet )->key();
            if ( !saveImages.contains( key ) )
                saveImages.append( key );
        }
        if ( !frameSet->isDeleted() && frameSet->type() == FT_CLIPART )
        {
            KoClipartKey key = static_cast<KWPictureFrameSet *>( frameSet )->key();
            if ( !saveCliparts.contains( key ) )
                saveCliparts.append( key );
        }
    }

    QDomElement styles = doc.createElement( "STYLES" );
    kwdoc.appendChild( styles );
    for ( KWStyle * p = m_styleList.first(); p != 0L; p = m_styleList.next() )
        p->save( styles );

    // Save the PIXMAPS list
    QString prefix = isStoredExtern() ? QString::null : url().url() + "/";
    QDomElement pixmaps = m_imageCollection.saveXML( doc, saveImages, prefix );
    kwdoc.appendChild( pixmaps );
    QDomElement cliparts = m_clipartCollection.saveXML( doc, saveCliparts, prefix );
    kwdoc.appendChild( cliparts );

    // Not needed anymore
#if 0
    // Write out the list of parags (id) that form the table of contents, see KWContents::createContents
    if ( contents->hasContents() ) {
        QDomElement cParags = doc.createElement( "CPARAGS" );
        kwdoc.appendChild( cParags );
        QValueList<int>::Iterator it = contents->begin();
        for ( ; it != contents->end(); ++it )
        {
            QDomElement paragElem = doc.createElement( "PARAG" );
            cParags.appendChild( paragElem );
            paragElem.setAttribute( "name", QString::number( *it ) ); // write parag id
        }
    }
#endif

/*
    out << otag << "<SERIALL>" << endl;
    slDataBase->save( out );
    out << etag << "</SERIALL>" << endl; */

    // Write "OBJECT" tag for every child
    QListIterator<KoDocumentChild> chl( children() );
    for( ; chl.current(); ++chl ) {
        QDomElement embeddedElem = doc.createElement( "EMBEDDED" );
        kwdoc.appendChild( embeddedElem );

        KWChild* curr = static_cast<KWChild*>(chl.current());

        QDomElement objectElem = curr->save( doc, true );
        embeddedElem.appendChild( objectElem );

        QDomElement settingsElem = doc.createElement( "SETTINGS" );
        embeddedElem.appendChild( settingsElem );

        QListIterator<KWFrameSet> fit = framesetsIterator();
        for ( ; fit.current() ; ++fit )
        {
            KWFrameSet * fs = fit.current();
            if ( fs->type() == FT_PART &&
                 dynamic_cast<KWPartFrameSet*>( fs )->getChild() == curr )
                fs->save( settingsElem );
        }
    }

    return doc;
}

bool KWDocument::completeSaving( KoStore *_store )
{
    if ( !_store )
        return TRUE;

    QString u = KURL( url() ).path();

    QValueList<KoImageKey> saveImages;
    QValueList<KoClipartKey> saveCliparts;
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        // If picture frameset, make a note of the image it needs.
        if ( !frameSet->isDeleted() && frameSet->type() == FT_PICTURE )
        {
            KoImageKey key = static_cast<KWPictureFrameSet *>( frameSet )->key();
            if ( !saveImages.contains( key ) )
                saveImages.append( key );
        }
        if ( !frameSet->isDeleted() && frameSet->type() == FT_CLIPART )
        {
            KoClipartKey key = static_cast<KWPictureFrameSet *>( frameSet )->key();
            if ( !saveCliparts.contains( key ) )
                saveCliparts.append( key );
        }
    }
    QString prefix = isStoredExtern() ? QString::null : url().url() + "/";
    m_imageCollection.saveToStore( _store, saveImages, prefix );
    m_clipartCollection.saveToStore( _store, saveCliparts, prefix );
    return TRUE;
}

bool KWDocument::saveChildren( KoStore *_store, const QString &_path )
{
    int i = 0;

    QListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        QString internURL = QString( "%1/%2" ).arg( _path ).arg( i++ );
        if ( !((KoDocumentChild*)(it.current()))->document()->saveToStore( _store, internURL ) )
          return FALSE;
    }
    return true;
}

void KWDocument::addView( KoView *_view )
{
    m_lstViews.append( (KWView*)_view );
    KoDocument::addView( _view );
}

void KWDocument::removeView( KoView *_view )
{
    m_lstViews.setAutoDelete( FALSE );
    m_lstViews.removeRef( static_cast<KWView*>(_view) );
    m_lstViews.setAutoDelete( TRUE );
    KoDocument::removeView( _view );
}

void KWDocument::addShell( KoMainWindow *shell )
{
#if KDE_VERSION >= 220 // kdelibs >= 2.2 only
    connect( shell, SIGNAL( documentSaved() ), m_commandHistory, SLOT( documentSaved() ) );
#endif
    KoDocument::addShell( shell );
}

KoView* KWDocument::createViewInstance( QWidget* parent, const char* name )
{
    return new KWView( parent, name, this );
}

void KWDocument::paintContent( QPainter& painter, const QRect& _rect, bool transparent, double zoomX, double zoomY )
{
    //kdDebug() << "KWDocument::paintContent zoomX=" << zoomX << " zoomY=" << zoomY << endl;
    m_zoom = 100;
    if ( m_zoomedResolutionX != zoomX || m_zoomedResolutionY != zoomY )
    {
        m_zoomedResolutionX = zoomX;
        m_zoomedResolutionY = zoomY;
        newZoomAndResolution( false, painter.device() && painter.device()->devType() == QInternal::Printer );
    }

    QRect rect( _rect );
    // Translate the painter to avoid the margin
    /*painter.translate( -leftBorder(), -topBorder() );
    rect.moveBy( leftBorder(), topBorder() );*/

    KWViewModeNormal * viewMode = new KWViewModeNormal( 0L ); // m_canvas==0L could be dangerous,
    // but we only use viewToNormal and normalToView.

    QColorGroup cg = QApplication::palette().active();

    if (!transparent)
    {
        QRegion emptyRegion( rect );
        createEmptyRegion( rect, emptyRegion, viewMode );
        eraseEmptySpace( &painter, emptyRegion, cg.brush( QColorGroup::Base ) );
    }

    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameset = fit.current();
        if ( frameset->isVisible() && !frameset->isFloating() )
            frameset->drawContents( &painter, rect, cg, false /*onlyChanged*/, true /*resetChanged*/,
                                    0L, viewMode, 0L );
    }
    delete viewMode;
}

void KWDocument::createEmptyRegion( const QRect & crect, QRegion & emptyRegion, KWViewMode * viewMode )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameset = fit.current();
        if ( frameset->isVisible() )
            frameset->createEmptyRegion( crect, emptyRegion, viewMode );
    }
}

void KWDocument::eraseEmptySpace( QPainter * painter, const QRegion & emptySpaceRegion, const QBrush & brush )
{
    painter->save();
    // Translate emptySpaceRegion in device coordinates
    // ( ARGL why on earth isn't QPainter::setClipRegion in transformed coordinate system ?? )
    QRegion devReg;
    QArray<QRect>rs = emptySpaceRegion.rects();
    rs.detach();
    for ( uint i = 0 ; i < rs.size() ; ++i )
    {
        //kdDebug() << "KWDocument::drawBorders emptySpaceRegion includes: " << DEBUGRECT( rs[i] ) << endl;
        rs[i] = painter->xForm( rs[i] );
    }
    devReg.setRects( rs.data(), rs.size() );
    painter->setClipRegion( devReg );
    painter->setPen( Qt::NoPen );

    //kdDebug() << "KWDocument::eraseEmptySpace emptySpaceRegion: " << DEBUGRECT( emptySpaceRegion.boundingRect() ) << endl;
    painter->fillRect( emptySpaceRegion.boundingRect(), brush );
    painter->restore();
}

void KWDocument::insertObject( const KoRect& rect, KoDocumentEntry& _e )
{

    KoDocument* doc = _e.createDoc( this );
    if ( !doc || !doc->initDoc() )
        return;

    KWChild* ch = new KWChild( this, rect.toQRect(), doc );

    insertChild( ch );
    setModified( TRUE );

    KWPartFrameSet *frameset = new KWPartFrameSet( this, ch, QString::null );
    KWFrame *frame = new KWFrame(frameset, rect.x(), rect.y(), rect.width(), rect.height() );
    frameset->addFrame( frame );
    addFrameSet( frameset );

    KWCreateFrameCommand *cmd = new KWCreateFrameCommand( i18n("Create a part frame"), frame);
    addCommand(cmd);

    emit sig_insertObject( ch, frameset );

    frameChanged( frame ); // repaint etc.
}

KWStyle* KWDocument::findStyle( const QString & _name )
{
    // Caching, to speed things up
    if ( m_lastStyle && m_lastStyle->name() == _name )
        return m_lastStyle;

    QListIterator<KWStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        if ( styleIt.current()->name() == _name ) {
            m_lastStyle = styleIt.current();
            return m_lastStyle;
        }
    }

    return 0L;
}

/* Update all views of this document, area can be cleared
   before redrawing with the _erase flag. (false implied)
   All views EXCEPT the argument _view are updated ( 0L = all )
 */
void KWDocument::repaintAllViewsExcept( KWView *_view, bool erase )
{
    //kdDebug() << "KWDocument::repaintAllViewsExcept" << endl;
    for ( KWView * viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        if ( viewPtr->getGUI() && viewPtr->getGUI()->canvasWidget() ) {
            if ( viewPtr != _view )
                viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
        }
    }
}

void KWDocument::setUnit( KWUnit::Unit _unit )
{
    m_unit = _unit;
    switch ( m_unit ) {
    case KWUnit::U_MM: m_pageLayout.unit = PG_MM;
        break;
    case KWUnit::U_PT: m_pageLayout.unit = PG_PT;
        break;
    case KWUnit::U_INCH: m_pageLayout.unit = PG_INCH;
        break;
    }

    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        if ( viewPtr->getGUI() ) {
            viewPtr->getGUI()->getHorzRuler()->setUnit( KWUnit::unitName( m_unit ) );
            viewPtr->getGUI()->getVertRuler()->setUnit( KWUnit::unitName( m_unit ) );
        }
    }
}

void KWDocument::updateAllStyleLists()
{
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->updateStyleList();
}

void KWDocument::applyStyleChange( KWStyle * changedStyle, int paragLayoutChanged, int formatChanged )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        // ###### TODO virtual method in KWFrameSet, so that KWTableFrameSet works too
        // Or better, an iterator that gets all text framesets even if nested
        KWTextFrameSet * frameSet = dynamic_cast<KWTextFrameSet*>( fit.current() );
        if ( frameSet )
            frameSet->applyStyleChange( changedStyle, paragLayoutChanged, formatChanged );
    }
}

void KWDocument::repaintAllViews( bool erase )
{
    //kdDebug() << "KWDocument::repaintAllViews" << endl;
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
}

void KWDocument::appendPage( /*unsigned int _page*/ )
{
    int thisPageNum = m_pages-1;
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::appendPage m_pages=" << m_pages << " so thisPageNum=" << thisPageNum << endl;
#endif
    m_pages++;

    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        // don't add tables! A table cell ( frameset ) _must_ not have cells auto-added to them!
        if ( frameSet->type() == FT_TABLE ) continue;

#ifdef DEBUG_PAGES
        kdDebug(32002) << "KWDocument::appendPage looking at frameset " << frameSet->getName() << endl;
#endif
        // KWFrameSet::addFrame triggers a reshuffle in the frames list (KWTextFrameSet::updateFrames)
        // which destroys the iterators -> append the new frames at the end.
        QList<KWFrame> newFrames;

        QListIterator<KWFrame> frameIt( frameSet->frameIterator() );
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frame = frameIt.current();
            /* copy the frame if: - it is on this page or
                                  - it is on the former page and the frame is set to double sided.
                                  - AND the frame is set to be reconnected or copied
                                  -  */
#ifdef DEBUG_PAGES
            kdDebug(32002) << "   frame=" << frame << " frame->pageNum()=" << frame->pageNum() << endl;
            static const char * newFrameBh[] = { "Reconnect", "NoFollowup", "Copy" };
            kdDebug(32002) << "   frame->getNewFrameBehaviour()==" << newFrameBh[frame->getNewFrameBehaviour()] << endl;
#endif
            if ( (frame->pageNum() == thisPageNum ||
                  (frame->pageNum() == thisPageNum -1 && frame->getSheetSide() != KWFrame::AnySide) )
                 &&
                 ( ( frame->getNewFrameBehaviour()==KWFrame::Reconnect && frameSet->type() == FT_TEXT ) ||  // (*)
                   ( frame->getNewFrameBehaviour()==KWFrame::Copy && !frameSet->isAHeader() && !frameSet->isAFooter() ) ) // (**)
                )
            {
                // (*) : Reconnect only makes sense for text frames
                // (**) : NewFrameBehaviour == Copy is handled here except for headers/footers, which
                // are created in recalcFrames() anyway.

                KWFrame *frm = frame->getCopy();
                frm->moveBy( 0, ptPaperHeight() );
                frm->setPageNum( frame->pageNum()+1 );
                newFrames.append( frm );

                if ( frame->getNewFrameBehaviour()==KWFrame::Copy )
                    frm->setCopy( true );
                //kdDebug(32002) << "   => created frame " << frm << endl;
            }
        }
        QListIterator<KWFrame> newFrameIt( newFrames );
        for ( ; newFrameIt.current() ; ++newFrameIt )
            frameSet->addFrame( newFrameIt.current() );
    }
    emit newContentsSize();

    if ( isHeaderVisible() || isFooterVisible() )
        recalcFrames();  // Get headers and footers on the new page
    // setModified(TRUE); This is called by formatMore, possibly on loading -> don't set modified

    recalcVariables( VT_PGNUM );
    emit pageNumChanged();
}

bool KWDocument::canRemovePage( int num )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        if ( frameSet->frameSetInfo() != KWFrameSet::FI_BODY ) // if header/footer/footnote
            continue;
        if ( frameSet->isVisible() && !frameSet->canRemovePage( num ) )
            return false;
    }
#ifdef DEBUG_PAGES
    kdDebug(32002) << "KWDocument::canRemovePage " << num << "-> TRUE" << endl;
#endif
    return true;
}

void KWDocument::removePage( int num )
{
#ifdef DEBUG_PAGES
    kdDebug() << "KWDocument::removePage " << num << endl;
#endif
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        if ( frameSet->frameSetInfo() != KWFrameSet::FI_BODY )
            continue;
        QListIterator<KWFrame> frameIt( frameSet->frameIterator() );
        QList<KWFrame> toDelete;
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frm = frameIt.current();
            if ( frm->pageNum() == num )
            {
#ifdef DEBUG_PAGES
                kdDebug() << "KWDocument::removePage deleting frame " << frm << " (from frameset " << frameSet->getName() << ")" << endl;
#endif
                toDelete.append( frm ); // Can't remove the frame here, it screws up the iterator -> toDelete
            }
        }
        QListIterator<KWFrame> delIt( toDelete );
        for ( ; delIt.current(); ++delIt )
            frameSet->delFrame( delIt.current(), true );
    }
    m_pages--;
#ifdef DEBUG_PAGES
    kdDebug() << "KWDocument::removePage -- -> " << m_pages << endl;
#endif
    emit pageNumChanged();
    recalcVariables( VT_PGNUM );
    recalcFrames();
}

KWFrameSet * KWDocument::getFrameSetByName( const QString & name )
{
    // Note: this isn't recursive, so it won't find table cells.
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        if ( fit.current()->getName() == name )
            return fit.current();
    return 0L;
}

KWFrame * KWDocument::frameAtPos( double mx, double my )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( fit.toLast(); fit.current() ; --fit ) // z-order
    {
        KWFrameSet *frameSet = fit.current();
        if ( !frameSet->isVisible() || frameSet->isRemoveableHeader() )
            continue;
        KWFrame * frame = frameSet->frameAtPos( mx, my );
        //kdDebug() << "KWDocument::frameAtPos found frameset " << frameSet << " at position " << mx << "," << my << endl;
        if ( frame )
            return frame;
    }
    return 0L;
}

KWFrame * KWDocument::frameByBorder( const QPoint & nPoint )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( fit.toLast(); fit.current() ; --fit ) // z-order
    {
        KWFrameSet *frameSet = fit.current();
        if ( !frameSet->isVisible() || frameSet->isRemoveableHeader() )
            continue;
        KWFrame * frame = frameSet->frameByBorder( nPoint );
        if ( frame )
            return frame;
    }
    return 0L;
}

QString KWDocument::generateFramesetName( const QString & templateName )
{
    QString name;
    int num = 1;
    bool exists;
    do {
        name = templateName.arg( num );
        exists = getFrameSetByName( name );
        ++num;
    } while ( exists );
    return name;
}

QCursor KWDocument::getMouseCursor( const QPoint &nPoint, bool controlPressed )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( fit.toLast(); fit.current() ; --fit )
    {
        KWFrameSet *frameSet = fit.current();
        if ( !frameSet->isVisible() || frameSet->isRemoveableHeader() )
            continue;

        QCursor cursor;
        // Can't move/resize main text frame (currently), hence the last arg
        bool mainFrameSet = ( m_processingType == WP ) && fit.atFirst();
        if ( frameSet->getMouseCursor( nPoint, controlPressed, cursor, !mainFrameSet ) )
            return cursor;
    }

    return ibeamCursor;
}

QList<KWFrame> KWDocument::getSelectedFrames() {
    QList<KWFrame> frames;
    frames.setAutoDelete( FALSE );
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        if ( !frameSet->isVisible() )
            continue;
        if ( frameSet->isRemoveableHeader() )
            continue;
        QListIterator<KWFrame> frameIt = frameSet->frameIterator();
        for ( ; frameIt.current(); ++frameIt )
            if ( frameIt.current()->isSelected() )
                frames.append( frameIt.current() );
    }

    return frames;
}

KWFrame *KWDocument::getFirstSelectedFrame()
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameSet = fit.current();
        for ( unsigned int j = 0; j < frameSet->getNumFrames(); j++ ) {
            if ( !frameSet->isVisible() )
                continue;
            if ( frameSet->isRemoveableHeader() )
                continue;
            if ( frameSet->getFrame( j )->isSelected() )
                return frameSet->getFrame( j );
        }
    }
    return 0L;
}

void KWDocument::updateAllFrames()
{
    kdDebug(32002) << "KWDocument::updateAllFrames " << frames.count() << " framesets." << endl;
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->updateFrames();
}

// Tell this method when a frame is moved / resized / created / deleted
// and everything will be update / repainted accordingly
void KWDocument::frameChanged( KWFrame * frame, KWView * view )
{
    //kdDebug() << "KWDocument::frameChanged" << endl;
    updateAllFrames();
    // If frame with text flowing around it -> re-layout all frames
    if ( !frame || frame->runAround() != KWFrame::RA_NO )
    {
        layout();
    }
    else
    {
        frame->getFrameSet()->layout();
    }
    repaintAllViewsExcept( view );
}

void KWDocument::framesChanged( const QList<KWFrame> & frames, KWView * view )
{
    //kdDebug() << "KWDocument::framesChanged" << endl;
    updateAllFrames();
    QListIterator<KWFrame> it( frames );
    for ( ; it.current() ; ++it )
        if ( it.current()->runAround() != KWFrame::RA_NO )
        {
            //kdDebug() << "KWDocument::framesChanged ->layout" << endl;
            layout();
            //kdDebug() << "KWDocument::framesChanged ->repaintAllViewsExcept" << endl;
            repaintAllViewsExcept( view );
            return;
        }
}

void KWDocument::setHeaderVisible( bool h )
{
    m_headerVisible = h;
    recalcFrames();
    updateAllFrames();
    layout();
    repaintAllViews( true );
}

void KWDocument::setFooterVisible( bool f )
{
    m_footerVisible = f;
    recalcFrames();
    updateAllFrames();
    layout();
    repaintAllViews( true );
}

void KWDocument::getFrameMargins( double &l, double &r, double &t, double &b )
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( getFrameSet( i )->hasSelectedFrame() ) {
            for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ ) {
                if ( getFrameSet( i )->getFrame( j )->isSelected() ) {
                    l = getFrameSet( i )->getFrame( j )->getBLeft();
                    r = getFrameSet( i )->getFrame( j )->getBRight();
                    t = getFrameSet( i )->getFrame( j )->getBTop();
                    b = getFrameSet( i )->getFrame( j )->getBBottom();
                    return;
                }
            }
        }
    }
}

bool KWDocument::isOnlyOneFrameSelected() {
    return getSelectedFrames().count()==1;
}

void KWDocument::setFrameMargins( double l, double r, double t, double b )
{
    // todo, make this more OO, and update the tableheaders as well..
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( getFrameSet( i )->hasSelectedFrame() ) {
            KWFrameSet *frameset = getFrameSet( i );
            for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
                if ( frameset->getFrame( j )->isSelected() ) {
                    frameset->getFrame( j )->setBLeft( l );
                    frameset->getFrame( j )->setBRight( r );
                    frameset->getFrame( j )->setBTop( t );
                    frameset->getFrame( j )->setBBottom( b );
                }
            }
        }
    }

    setModified(TRUE);
}

void KWDocument::addImageRequest( const KoImageKey &key, KWTextImage *img )
{
    m_imageRequests.insert( key, img );
}

void KWDocument::addImageRequest( KWPictureFrameSet *fs )
{
    m_imageRequests2.append( fs );
}

void KWDocument::addClipartRequest( KWClipartFrameSet *fs )
{
    m_clipartRequests.append( fs );
}

void KWDocument::addAnchorRequest( const QString &framesetName, const KWAnchorPosition &anchorPos )
{
    m_anchorRequests.insert( framesetName, anchorPos );
}

KWVariableFormat * KWDocument::variableFormat( int type )
{
    // Look into the map
    QMap<int,KWVariableFormat*>::Iterator it = m_mapVariableFormats.find( type );
    if ( it != m_mapVariableFormats.end() )
    {
        return it.data();
    }
    else
    {
        KWVariableFormat * format = 0L;
        // The formats are created on demand.
        // TODO save those that have settings
        switch( type )
        {
            case VF_DATE:
                format = new KWVariableDateFormat();
                break;
            case VF_TIME:
                format = new KWVariableTimeFormat();
                break;
            case VF_NUM:
                format = new KWVariableNumberFormat();
                break;
            case VF_STRING:
                format = new KWVariableStringFormat();
                break;
            default:
                break;
        }
        if ( format )
            m_mapVariableFormats.insert( type, format );
        return format;
    }
}

void KWDocument::registerVariable( KWVariable *var )
{
    if ( !var )
        return;
    variables.append( var );
}


void KWDocument::refreshMenuCustomVariable()
{
   emit sig_refreshMenuCustomVariable();
}

void KWDocument::unregisterVariable( KWVariable *var )
{
    variables.take( variables.findRef( var ) );
}

void KWDocument::recalcVariables( int type )
{
    bool update = false;
    QListIterator<KWVariable> it( variables );
    QList<KWTextFrameSet> toRepaint;
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->type() == type )
        {
            update = true;
            it.current()->recalc();
            QTextParag * parag = it.current()->paragraph();
            if ( parag )
            {
                kdDebug() << "KWDoc::recalcVariables -> invalidating parag " << parag->paragId() << endl;
                parag->invalidate( 0 );
                parag->setChanged( true );
                KWTextFrameSet * textfs = it.current()->textDocument()->textFrameSet();
                if ( toRepaint.findRef( textfs ) == -1 )
                    toRepaint.append( textfs );
            }
        }
    }
    for ( KWTextFrameSet * fs = toRepaint.first() ; fs ; fs = toRepaint.next() )
        slotRepaintChanged( fs );
}

void KWDocument::setVariableValue( const QString &name, const QString &value )
{
    varValues[ name ] = value;
}

QString KWDocument::getVariableValue( const QString &name ) const
{
    if ( !varValues.contains( name ) )
        return i18n( "No value" );
    return varValues[ name ];
}

KWSerialLetterDataBase *KWDocument::getSerialLetterDataBase() const
{
    return slDataBase;
}

int KWDocument::getSerialLetterRecord() const
{
    return slRecordNum;
}

void KWDocument::setSerialLetterRecord( int r )
{
    slRecordNum = r;
}

void KWDocument::getPageLayout( KoPageLayout& _layout, KoColumns& _cl, KoKWHeaderFooter& _hf )
{
    _layout = m_pageLayout;
    _cl = m_pageColumns;
    _hf = m_pageHeaderFooter;
}

void KWDocument::addFrameSet( KWFrameSet *f, bool finalize /*= true*/ )
{
    frames.append(f);
    if ( finalize )
        f->finalize();
    setModified( true );
}

void KWDocument::removeFrameSet( KWFrameSet *f )
{
    emit sig_terminateEditing( f );
    frames.take( frames.find(f) );
    setModified( true );
}

// Returns 0-based page number where rect is (in fact its topleft corner).
// Use isOutOfPage to check that the rectangle is fully contained in that page.
int KWDocument::getPageOfRect( KoRect & _rect ) const
{
    int page = static_cast<int>(_rect.y() / ptPaperHeight());
    return QMIN( page, m_pages-1 );
}

// Return true if @p r is out of the page @p page
bool KWDocument::isOutOfPage( KoRect & r, int page ) const
{
    return r.x() < 0 ||
        r.right() > ptPaperWidth() ||
        r.y() < page * ptPaperHeight() ||
        r.bottom() > ( page + 1 ) * ptPaperHeight();
}

void KWDocument::addCommand( KCommand * cmd )
{
    kdDebug() << "KWDocument::addCommand " << cmd->name() << endl;
    m_commandHistory->addCommand( cmd, false );
    setModified( true );
}

void KWDocument::slotDocumentRestored()
{
    setModified( false );
}

void KWDocument::slotCommandExecuted()
{
    setModified( true );
}

void KWDocument::setKSpellConfig(KSpellConfig _kspell)
{
  if(m_pKSpellConfig==0)
    m_pKSpellConfig=new KSpellConfig();

  m_pKSpellConfig->setNoRootAffix(_kspell.noRootAffix ());
  m_pKSpellConfig->setRunTogether(_kspell.runTogether ());
  m_pKSpellConfig->setDictionary(_kspell.dictionary ());
  m_pKSpellConfig->setDictFromList(_kspell.dictFromList());
  m_pKSpellConfig->setEncoding(_kspell.encoding());
  m_pKSpellConfig->setClient(_kspell.client());
}

#ifndef NDEBUG
void KWDocument::printDebug()
{
    kdDebug() << "----------------------------------------"<<endl;
    kdDebug() << "                 Debug info"<<endl;
    kdDebug() << "Document:" << this <<endl;
    kdDebug() << "Type of document: (0=WP, 1=DTP) " << processingType() <<endl;
    kdDebug() << "size: x:" << ptLeftBorder()<< ", y:"<<ptTopBorder() << ", w:"<< ptPaperWidth() << ", h:"<<ptPaperHeight()<<endl;
    kdDebug() << "Header visible: " << isHeaderVisible() << endl;
    kdDebug() << "Footer visible: " << isFooterVisible() << endl;
    kdDebug() << "Units: " << getUnit() <<endl;
    kdDebug() << "# Framesets: " << getNumFrameSets() <<endl;
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( unsigned int iFrameset = 0; fit.current() ; ++fit, iFrameset++ )
    {
        KWFrameSet * frameset = fit.current();
        kdDebug() << "Frameset " << iFrameset << ": '" <<
            frameset->getName() << "' (" << frameset << ")" <<endl;
        frameset->printDebug();
    }
    /*
    kdDebug() << "# Images: " << getImageCollection()->iterator().count() <<endl;
    QDictIterator<KWImage> it( getImageCollection()->iterator() );
    while ( it.current() ) {
        kdDebug() << " + " << it.current()->getFilename() << ": "<<it.current()->refCount() <<endl;
        ++it;
    }
    */
}
#endif

void KWDocument::layout()
{
    QListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        if ( it.current()->isVisible() )
            it.current()->layout();
}

void KWDocument::invalidate()
{
    QListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        it.current()->invalidate();
}

KFormula::KFormulaDocument* KWDocument::getFormulaDocument()
{
    return m_formulaDocument;
}


void KWDocument::slotRepaintChanged( KWFrameSet * frameset )
{
    // This has to be a loop instead of a signal, so that we can
    // send "true" for the last view (see KWFrameSet::drawContents)
    QListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
        it.current()->getGUI()->canvasWidget()->repaintChanged( frameset, it.atLast() );
}

void KWDocument::refreshFrameBorderButton()
{

    KWFrame *frame= getFirstSelectedFrame();
    if (frame)
    {
        QListIterator<KWView> it( m_lstViews );
        frame = KWFrameSet::settingsFrame(frame);
        for ( ; it.current() ; ++it )
        {
            it.current()->showFrameBorders( frame->leftBorder(), frame->rightBorder(), frame->topBorder(), frame->bottomBorder() );
        }
    }
}

void KWDocument::updateResizeHandles( )
{
   QList<KWFrame> selectedFrames = getSelectedFrames();
   KWFrame *frame=0L;
   for(frame=selectedFrames.first(); frame != 0; frame=selectedFrames.next() )
   {
       frame->updateResizeHandles();
   }
}

void KWDocument::deleteTable( KWTableFrameSet *table )
{
    if ( !table )
        return;
    table->deselectAll();
    if ( table->isFloating() )
    {
        emit sig_terminateEditing( table ); // to unselect its cells, especially
        KWAnchor * anchor = table->findAnchor( 0 );
        table->anchorFrameset()->deleteAnchoredFrame( anchor );
    }
    else
    {
        KWDeleteTableCommand *cmd = new KWDeleteTableCommand( i18n("Delete table"), table );
        addCommand( cmd );
        cmd->execute();
    }
}

void KWDocument::deleteFrame( KWFrame * frame )
{
    KWFrameSet * fs = frame->getFrameSet();
    frame->setSelected(false);
    QString cmdName;
    TypeStructDocItem docItem = (TypeStructDocItem) 0;
    switch (fs->type() ) {
    case FT_TEXT:
        cmdName=i18n("Delete text frame");
        docItem=TextFrames;
        break;
    case FT_FORMULA:
        cmdName=i18n("Delete formula frame");
        docItem=FormulaFrames;
        break;
    case FT_PICTURE:
        cmdName=i18n("Delete picture frame");
        docItem=Pictures;
        break;
    case FT_CLIPART:
        cmdName=i18n("Delete picture frame"); // TODO "Delete clipart frame" after msg freeze
        docItem=Cliparts;
        break;
    case FT_PART:
        cmdName=i18n("Delete object frame");
        docItem=Embedded;
        break;
    case FT_TABLE:
    case FT_BASE:
        ASSERT( 0 );
        break;
    }
    if ( fs->isFloating() )
    {
        frame->setSelected( false );
        KWAnchor * anchor = fs->findAnchor( 0 );
        fs->anchorFrameset()->deleteAnchoredFrame( anchor );
    }
    else
    {
        KWDeleteFrameCommand *cmd = new KWDeleteFrameCommand( cmdName, frame );
        addCommand( cmd );
        cmd->execute();
    }
    emit docStructureChanged(docItem);
}

void KWDocument::reorganizeGUI()
{
   QListIterator<KWView> it( m_lstViews );
   for ( ; it.current() ; ++it )
       it.current()->getGUI()->reorganize();
}

void KWDocument::slotDocumentInfoModifed()
{
    recalcVariables( VT_FIELD );
}

void KWDocument::refreshDocStructure(int type)
{
     emit docStructureChanged(type);
}

void KWDocument::refreshDocStructure(FrameSetType _type)
{
    int typeItem;
    switch(_type)
    {
        case FT_TEXT:
            typeItem=(int)TextFrames;
            break;
        case FT_PICTURE:
            typeItem=(int)Pictures;
            break;
        case FT_PART:
            typeItem=(int)Embedded;
            break;
        case FT_FORMULA:
            typeItem=(int)FormulaFrames;
            break;
        case FT_TABLE:
            typeItem=(int)Tables;
            break;
        default:
            typeItem=(int)TextFrames;
    }
    emit docStructureChanged(typeItem);
}

QColor KWDocument::resolveTextColor( const QColor & col, QPainter * painter )
{
    if (col.isValid())
        return col;

    return defaultTextColor( painter );
}

QColor KWDocument::defaultTextColor( QPainter * painter )
{
    if ( painter->device()->devType() == QInternal::Printer )
        return Qt::black;
    return QApplication::palette().color( QPalette::Active, QColorGroup::Text );
}

QColor KWDocument::resolveBgColor( const QColor & col, QPainter * painter )
{
    if (col.isValid())
        return col;

    return defaultBgColor( painter );
}

QColor KWDocument::defaultBgColor( QPainter * painter )
{
    if ( painter->device()->devType() == QInternal::Printer )
        return Qt::white;
    return QApplication::palette().color( QPalette::Active, QColorGroup::Base );
}


void KWDocument::renameButtonTOC(const QString & _name)
{
    QListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
        it.current()->renameButtonTOC(_name);
}

void KWDocument::refreshMenuExpression()
{
    QListIterator<KWView> it( m_lstViews );
    for ( ; it.current() ; ++it )
        it.current()->refreshMenuExpression();
}

void KWDocument::frameSelectedChanged()
{
    emit sig_frameSelectedChanged();
}

#include "kwdoc.moc"
