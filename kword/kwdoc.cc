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

#include <koFilterManager.h>
#include <koTemplateChooseDia.h>
#include <koStore.h>
#include <koStoreDevice.h>
#include <koMainWindow.h>
#include <koQueryTrader.h>

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
#include "contents.h"
#include "kwview.h"
#include "kwfactory.h"
#include "kwcommand.h"
#include "kwtextimage.h"
#include <kdebug.h>
#include <kfontdialog.h>

#include <kspell.h>

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

/*================================================================*/
KWChild::KWChild( KWDocument *_wdoc, const QRect& _rect, KoDocument *_doc )
    : KoDocumentChild( _wdoc, _doc, _rect )
{
}

/*================================================================*/
KWChild::KWChild( KWDocument *_wdoc )
    : KoDocumentChild( _wdoc )
{
}

/*================================================================*/
KWChild::~KWChild()
{
}

KoDocument *KWChild::hitTest( const QPoint &, const QWMatrix & )
{
  return 0L;
}

/******************************************************************/
/* Class: KWDocument                                      */
/******************************************************************/

/*================================================================*/
KWDocument::KWDocument(QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
      m_unit( KWUnit::U_MM ), // footNoteManager( this ),
      urlIntern()
{
    m_lstViews.setAutoDelete( false );
    m_lstChildren.setAutoDelete( true );
    m_styleList.setAutoDelete( true );
//    varFormats.setAutoDelete(true);
    frames.setAutoDelete( true );

    setInstance( KWFactory::global(), false );

    m_gridX = m_gridY = 10;
    m_indent = MM_TO_POINT( 10.0 );

    m_bShowRuler=true;

    _viewFormattingChars = FALSE;
    _viewFrameBorders = TRUE;
    _viewTableGrid = TRUE;

    m_autoFormat = new KWAutoFormat(this);

    m_commandHistory = new KCommandHistory( actionCollection(), false );
#if KDE_VERSION >= 220 // kdelibs >= 2.2 only
    connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
#endif
    connect( m_commandHistory, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );

    connect( documentInfo(), SIGNAL( sigDocumentInfoModifed()),this,SLOT(slotDocumentInfoModifed() ) );

    m_formulaDocument = new KFormulaDocument(actionCollection(), m_commandHistory);

    setEmpty();
    setModified(false);

    styleMask = U_FONT_FAMILY_ALL_SIZE | U_COLOR | U_BORDER | U_INDENT |
                         U_NUMBERING | U_ALIGN | U_TABS | U_SMART;
    m_headerVisible = false;
    m_footerVisible = false;

    m_lastStyle = 0L;

    slDataBase = new KWSerialLetterDataBase( this );
    slRecordNum = -1;

    spellCheck = FALSE;
    contents = new KWContents( this );

    // Get default font from KDE
    m_defaultFont = KGlobalSettings::generalFont();
    KGlobal::charsets()->setQFont(m_defaultFont, KGlobal::locale()->charset());
    m_fontList = 0L;

    // Set no-op initial values (for setZoomAndResolution)
    m_zoomedResolutionX = 1;
    m_zoomedResolutionY = 1;
    m_zoom = 100;

    setZoomAndResolution( 100, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY(), false );
    syntaxVersion = CURRENT_SYNTAX_VERSION;

    m_pKSpellConfig=0;

    // Some simple import filters don't define any style,
    // so let's have a Standard style at least
    KWStyle * standardStyle = new KWStyle( "Standard" ); // This gets translated later on
    addStyleTemplate( standardStyle );
}

void KWDocument::setZoomAndResolution( int zoom, int dpiX, int dpiY, bool updateViews )
{
    //double oldZoomedResolutionX = m_zoomedResolutionX;
    //double oldZoomedResolutionY = m_zoomedResolutionY;

    m_zoom = zoom;
    // m_resolution[XY] is in pixel per pt
    m_resolutionX = POINT_TO_INCH( static_cast<double>(dpiX) );
    m_resolutionY = POINT_TO_INCH( static_cast<double>(dpiY) );
    m_zoomedResolutionX = static_cast<double>(m_zoom) * m_resolutionX / 100.0;
    m_zoomedResolutionY = static_cast<double>(m_zoom) * m_resolutionY / 100.0;
    kdDebug(32002) << "KWDocument::setZoomAndResolution " << zoom << " " << dpiX << "," << dpiY
                   << " m_resolutionX=" << m_resolutionX
                   << " m_zoomedResolutionX=" << m_zoomedResolutionX
                   << " m_resolutionY=" << m_resolutionY
                   << " m_zoomedResolutionY=" << m_zoomedResolutionY << endl;

    newZoomAndResolution( updateViews );
}

void KWDocument::newZoomAndResolution( bool updateViews )
{
    getFormulaDocument()->setResolution( m_zoomedResolutionX, m_zoomedResolutionY );

    // Update all fonts
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->zoom();

    layout();
    updateAllFrames();
    if ( updateViews )
    {
        updateAllViewportSizes();
        repaintAllViews( true );
    }
}

bool KWDocument::initDoc()
{
    m_pageLayout.unit = PG_MM;
    m_pages = 1;

    m_pageColumns.columns = 1; //STANDARD_COLUMNS;
    m_pageColumns.ptColumnSpacing = tableCellSpacing;

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

    m_pageColumns.columns = 1; //STANDARD_COLUMNS;
    m_pageColumns.ptColumnSpacing = tableCellSpacing;

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


/*================================================================*/
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

/*================================================================*/
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
    if ( isHeaderVisible() || isFooterVisible() ) {
        QListIterator<KWFrameSet> fit = framesetsIterator();
        for ( ; fit.current() ; ++fit )
        {
            KWFrameSet * fs = fit.current();
            FrameInfo fi = fs->getFrameInfo();
            if ( fi == FI_FIRST_HEADER && isHeaderVisible() ) {
                firstHeader = dynamic_cast<KWTextFrameSet*>( fs );
                firstHeadOffset = static_cast<int>(m_pageHeaderFooter.ptHeaderBodySpacing +
                                                   fs->getFrame( 0 )->height());
            }
            if ( fi == FI_EVEN_HEADER && isHeaderVisible() ) {
                evenHeader = dynamic_cast<KWTextFrameSet*>( fs );
                evenHeadOffset = static_cast<int>(m_pageHeaderFooter.ptHeaderBodySpacing +
                                                  fs->getFrame( 0 )->height());
            }
            if ( fi == FI_ODD_HEADER && isHeaderVisible() ) {
                oddHeader = dynamic_cast<KWTextFrameSet*>( fs );
                oddHeadOffset = static_cast<int>(m_pageHeaderFooter.ptHeaderBodySpacing +
                                                 fs->getFrame( 0 )->height());
            }

            if ( fi == FI_FIRST_FOOTER && isFooterVisible() ) {
                firstFooter = dynamic_cast<KWTextFrameSet*>( fs );
                firstFootOffset = static_cast<int>(m_pageHeaderFooter.ptFooterBodySpacing +
                                                   fs->getFrame( 0 )->height());
            }
            if ( fi == FI_EVEN_FOOTER && isFooterVisible() ) {
                evenFooter = dynamic_cast<KWTextFrameSet*>( fs );
                evenFootOffset = static_cast<int>(m_pageHeaderFooter.ptFooterBodySpacing +
                                                  fs->getFrame( 0 )->height());
            }
            if ( fi == FI_ODD_FOOTER && isFooterVisible() ) {
                oddFooter = dynamic_cast<KWTextFrameSet*>( fs );
                oddFootOffset = static_cast<int>(m_pageHeaderFooter.ptFooterBodySpacing +
                                                 fs->getFrame( 0 )->height());
            }
        }
        if ( isHeaderVisible() ) {
            switch ( getHeaderType() ) {
            case HF_SAME:
                oddHeader = evenHeader;
                firstHeader = evenHeader;
                oddHeadOffset = evenHeadOffset;
                firstHeadOffset = evenHeadOffset;
                break;
            case HF_FIRST_DIFF:
                oddHeader = evenHeader;
                oddHeadOffset = evenHeadOffset;
                break;
            case HF_EO_DIFF:
                firstHeader = oddHeader;
                firstHeadOffset = oddHeadOffset;
                break;
            }
        }
        if ( isFooterVisible() ) {
            switch ( getFooterType() ) {
            case HF_SAME:
                oddFooter = evenFooter;
                firstFooter = evenFooter;
                oddFootOffset = evenFootOffset;
                firstFootOffset = evenFootOffset;
                break;
            case HF_FIRST_DIFF:
                oddFooter = evenFooter;
                oddFootOffset = evenFootOffset;
                break;
            case HF_EO_DIFF:
                firstFooter = oddFooter;
                firstFootOffset = oddFootOffset;
                break;
            }
        }
    }

    if ( m_processingType == WP ) { // In WP mode the pages are created automatically. In DTP not...

        int headOffset = 0, footOffset = 0;
        int oldPages = m_pages;
        unsigned int frms = frameset->getNumFrames();

        // Determine number of pages - first from the text frames
        m_pages = static_cast<int>( ceil( static_cast<double>( frms ) / static_cast<double>( m_pageColumns.columns ) ) );
        // Then from the other frames ( frameset-num > 0 )
        double maxBottom = 0;
        for (int m = getNumFrameSets() - 1; m > 0; m-- )
        {
            KWFrameSet *fs=getFrameSet(m);
            if ( fs->isVisible() && !fs->isAHeader() && !fs->isAFooter() )
            {
                for (int n = fs->getNumFrames()-1; n >= 0 ; n--) {
                    //if ( n == fs->getNumFrames()-1 )
                    //    kdDebug(32002) << "KWDocument::recalcFrames frameset " << m << " frame " << n << " bottom=" << fs->getFrame(n)->bottom() << endl;
                    maxBottom = QMAX(maxBottom, fs->getFrame(n)->bottom());
                }
            }
        }
        int pages2 = static_cast<int>( ceil( maxBottom / ptPaperHeight() ) );
        kdDebug(32002) << "KWDocument::recalcFrames, WP, m_pages=" << m_pages << " pages2=" << pages2 << " ptPaperHeight=" << ptPaperHeight() << endl;

        m_pages = QMAX( pages2, m_pages );
        if ( m_pages != oldPages )
        {
            emit pageNumChanged();
            recalcVariables( VT_PGNUM );
        }

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
                    frameset->addFrame( new KWFrame(frameset, ptLeftBorder() +
                         i * ( ptColumnWidth + ptColumnSpacing() ),
                         j * ptPaperHeight() + ptTopBorder() + headOffset,
                         ptColumnWidth, ptPaperHeight() -
                         ptTopBorder() - ptBottomBorder() -
                         headOffset - footOffset ) );
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
                        KWFrame *frame = new KWFrame( oddHeader, ptLeftBorder(), l * ptPaperHeight() +
                                                      ptTopBorder(),
                                                      ptPaperWidth() - ptLeftBorder() -
                                                      ptRightBorder(), h2 );
                        oddHeader->addFrame( frame );
                    }
                } else {
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
                        KWFrame *frame = new KWFrame( evenHeader,ptLeftBorder(), l * ptPaperHeight() +
                                                      ptTopBorder(),
                                                      ptPaperWidth() - ptLeftBorder() -
                                                      ptRightBorder(), h1 );
                        evenHeader->addFrame( frame );
                    }
                }
            }
            if ( even + 1 < static_cast<int>( evenHeader->getNumFrames() ) ) {
                int diff = evenHeader->getNumFrames() - even;
                for ( ; diff > 0; diff-- )
                    evenHeader->delFrame( evenHeader->getNumFrames() - 1 );
            }
            if ( odd + 1 < static_cast<int>( oddHeader->getNumFrames() ) ) {
                int diff = oddHeader->getNumFrames() - odd;
                for ( ; diff > 0; diff-- )
                    oddHeader->delFrame( oddHeader->getNumFrames() - 1 );
            }
            if ( m_pages == 1 && evenHeader->getNumFrames() > 0 ) {
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

    updateAllViewportSizes();
}

/*================================================================*/
KWDocument::~KWDocument()
{
    delete contents;
    delete m_autoFormat;
    delete m_formulaDocument;
    delete m_commandHistory;
    delete m_fontList;
}

/*================================================================*/
bool KWDocument::loadChildren( KoStore *_store )
{
    QListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        if ( !((KoDocumentChild*)it.current())->loadDocument( _store ) )
            return FALSE;
    }

    return TRUE;
}

/*================================================================*/
bool KWDocument::loadXML( QIODevice *, const QDomDocument & doc )
{
    QTime dt;
    dt.start();
    emit sigProgress( 0 );
    kdDebug(32001) << "KWDocument::loadXML" << endl;
    pixmapKeys.clear();
    pixmapNames.clear();
    m_imageRequests.clear();
    m_imageRequests2.clear();
    m_anchorRequests.clear();

    m_pageLayout.unit = PG_MM;

    m_pageColumns.columns = 1; //STANDARD_COLUMNS;
    m_pageColumns.ptColumnSpacing = tableCellSpacing;

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
    syntaxVersion = KWDocument::getAttribute( word, "syntaxVersion", 0 );
    if ( syntaxVersion < CURRENT_SYNTAX_VERSION )
    {
        // We can ignore the version mismatch for now. We will actually have to use it
        // when the syntax is extended in an incompatible manner.
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
            setZoomAndResolution( m_zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY(), false );
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

    // <PIXMAPS>
    QDomElement pixmapsElem = word.namedItem( "PIXMAPS" ).toElement();
    if ( !pixmapsElem.isNull() )
    {
        QDomNodeList listPixmaps = pixmapsElem.elementsByTagName( "KEY" );
        for (item = 0; item < listPixmaps.count(); item++)
        {
            QDomElement pixmap = listPixmaps.item( item ).toElement();
            QString k = KWDocument::getAttribute( pixmap, "key", QString::null );
            QString n = KWDocument::getAttribute( pixmap, "name", QString::null );
            pixmapNames.append( n );
            pixmapKeys.append( k );
        }
    }

    emit sigProgress(90);

    // <EMBEDDED>
    QDomNodeList listEmbedded = word.elementsByTagName ( "EMBEDDED" );
    for (item = 0; item < listEmbedded.count(); item++)
    {
        QDomElement embedded = listEmbedded.item( item ).toElement();
        KWChild *ch = new KWChild( this );
        KWPartFrameSet *fs = 0;
        QRect r;

        QDomElement object = embedded.namedItem( "OBJECT" ).toElement();
        if ( !object.isNull() )
        {
            ch->load( object, true );
            r = ch->geometry();
            insertChild( ch );
            fs = new KWPartFrameSet( this, ch );
            frames.append( fs );
            QDomElement settings = embedded.namedItem( "SETTINGS" ).toElement();
            if ( !settings.isNull() )
            {
                kdDebug(32001) << "KWDocument::loadXML loading embedded object" << endl;
                fs->load( settings );
            }
            else
                kdError(32001) << "No <SETTINGS> tag in EMBEDDED" << endl;
        } else
            kdError(32001) << "No <OBJECT> tag in EMBEDDED" << endl;
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
        switch( fit.current()->getFrameInfo() ) {
        case FI_FIRST_HEADER: _first_header = TRUE; break;
        case FI_EVEN_HEADER: _odd_header = TRUE; break;
        case FI_ODD_HEADER: _even_header = TRUE; break;
        case FI_FIRST_FOOTER: _first_footer = TRUE; break;
        case FI_EVEN_FOOTER: _odd_footer = TRUE; break;
        case FI_ODD_FOOTER: _even_footer = TRUE; break;
        case FI_FOOTNOTE: _footnotes = TRUE; break;
        default: break;
        }
    }

    // create defaults if they were not in the input file.

    if ( !_first_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        kdDebug(32001) << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameInfo( FI_FIRST_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
            ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        kdDebug(32001) << "KWDocument::loadXML KWFrame created " << frame << endl;
        frame->setFrameBehaviour(AutoExtendFrame);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_even_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_EVEN_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
            ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        frame->setFrameBehaviour(AutoExtendFrame);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_odd_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_ODD_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
            ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        frame->setFrameBehaviour(AutoExtendFrame);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_first_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_FIRST_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
            ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
            ptRightBorder(), 20 );
        frame->setFrameBehaviour(AutoExtendFrame);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_even_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_EVEN_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
            ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
            ptRightBorder(), 20 );
        frame->setFrameBehaviour(AutoExtendFrame);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_odd_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_ODD_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
            ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
            ptRightBorder(), 20 );
        frame->setFrameBehaviour(AutoExtendFrame);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_footnotes ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_FOOTNOTE );
        fs->setName( "Footnotes" );

        for ( int i = 0; i < m_pages; i++ ) {
            KWFrame *frame = new KWFrame(fs, ptLeftBorder(),
                i * ptPaperHeight() + ptPaperHeight() - ptTopBorder() - 20,
                ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
            frame->setFrameBehaviour(AutoExtendFrame);
            fs->addFrame( frame );
        }
        frames.append( fs );
        fs->setVisible( FALSE );
    }

    KWChild *ch = 0L;
    for ( ch = m_lstChildren.first(); ch != 0; ch = m_lstChildren.next() ) {
        KWPartFrameSet *frameset = new KWPartFrameSet( this, ch );
        frameset->setName( i18n( "PartFrameset %1" ).arg( frames.count() + 1 ) );
        QRect r = ch->geometry();
        KWFrame *frame = new KWFrame(frameset, r.x(), r.y(), r.width(), r.height() );
        frameset->addFrame( frame );
        frames.append( frameset );
        emit sig_insertObject( ch, frameset );
    }

    m_autoFormat->addAutoFormatEntry( KWAutoFormatEntry("(C)", "©" ) );
    m_autoFormat->addAutoFormatEntry( KWAutoFormatEntry("(c)", "©" ) );
    m_autoFormat->addAutoFormatEntry( KWAutoFormatEntry("(R)", "®" ) );
    m_autoFormat->addAutoFormatEntry( KWAutoFormatEntry("(r)", "®" ) );

    // do some sanity checking on document.
    for (int i = getNumFrameSets()-1; i>-1; i--) {
        if(! getFrameSet(i)) {
            kdWarning () << "frameset " << i << " is NULL!!" << endl;
            frames.remove(i);
        } else if( getFrameSet(i)->getFrameType()==FT_TABLE) {
            static_cast<KWTableFrameSet *>( getFrameSet(i))->validate();
        } else if(! getFrameSet(i)->getFrame(0)) {
            kdWarning () << "frameset " << i << " has no frames" << endl;
            delFrameSet(getFrameSet(i));
        } else if (getFrameSet(i)->getFrameType() == FT_TEXT) {
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

/*================================================================*/
void KWDocument::loadStyleTemplates( QDomElement stylesElem )
{
    // <STYLE>
    QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
    for (unsigned int item = 0; item < listStyles.count(); item++)
    {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KWStyle *sty = new KWStyle( styleElem, m_defaultFont );
        //kdDebug(32001) << "KWDocument::addStyleTemplate style's name is " << sty->name() << endl;
        addStyleTemplate( sty );
    }
}

void KWDocument::addStyleTemplate( KWStyle * sty )
{
    // First check for duplicates.
    // We could use a QMap to make that kind of lookup faster but
    // then the name would be duplicated, or not stored in the KWStyle,
    // and we wouldn't control the order.
    for ( KWStyle* p = m_styleList.first(); p != 0L; p = m_styleList.next() )
    {
        if ( p->name() == sty->name() )
        {
            // Replace existing style
            *p = *sty;
            //if ( p->name() == "Standard" ) defaultParagLayout = p;
            delete sty;
            return;
        }
    }
    m_styleList.append( sty );
}

void KWDocument::removeStyleTemplate ( const QString & _styleName )
{
  for ( KWStyle* p = m_styleList.first(); p != 0L; p = m_styleList.next() )
    {
        if ( p->name() == _styleName )
        {
            // Remove (and delete) style
	    m_styleList.remove( p );
            return;
        }
    }
}

/*================================================================*/
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

/*================================================================*/
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

/*================================================================*/
void KWDocument::loadFrameSets( QDomElement framesets )
{
    // <FRAMESET>
    // First prepare progress info
    m_nrItemsToLoad = 0; // total count of items (mostly paragraph and frames)
    QDomNodeList listFramesets = framesets.elementsByTagName( "FRAMESET" );
    for (unsigned int item = 0; item < listFramesets.count(); item++)
        m_nrItemsToLoad += listFramesets.item( item ).childNodes().count();
    m_itemsLoaded = 0;

    for (unsigned int item = 0; item < listFramesets.count(); item++)
    {
        QDomElement framesetElem = listFramesets.item( item ).toElement();
        FrameType frameType;
        FrameInfo frameInfo;
        QString tableName;
        int _row, _col, _rows, _cols;
        bool removeable;
        bool _visible;
        QString fsname;

        frameType = static_cast<FrameType>( KWDocument::getAttribute( framesetElem, "frameType", FT_BASE ) );
        frameInfo = static_cast<FrameInfo>( KWDocument::getAttribute( framesetElem, "frameInfo", FI_BODY ) );
        tableName = correctQString( KWDocument::getAttribute( framesetElem, "grpMgr", "" ) );
        _row = KWDocument::getAttribute( framesetElem, "row", 0 );
        _col = KWDocument::getAttribute( framesetElem, "col", 0 );
        if ( framesetElem.attribute( "removeable" ) != QString::null )
            removeable = static_cast<bool>( KWDocument::getAttribute( framesetElem, "removeable", false ) );
        else
            removeable = static_cast<bool>( KWDocument::getAttribute( framesetElem, "removable", false ) );
        _rows = getAttribute( framesetElem, "rows", 1 );
        _cols = getAttribute( framesetElem, "cols", 1 );
        _visible = static_cast<bool>( KWDocument::getAttribute( framesetElem, "visible", true ) );
        fsname = correctQString( KWDocument::getAttribute( framesetElem, "name", "" ) );

        if ( fsname.isEmpty() ) {
            if(frameInfo!=FI_BODY)
                fsname = i18n( "TextFrameset %1" ).arg( frames.count() + 1 );
            else
                fsname = i18n( "Frameset %1" ).arg( frames.count() + 1 );
        }

        switch ( frameType ) {
            case FT_TEXT: {
                if ( !tableName.isEmpty() ) {
                    KWTableFrameSet *table = 0L;
                    for ( unsigned int i = 0; i < frames.count(); i++ ) {
                        KWFrameSet *f = frames.at(i);
                        if(! f->isVisible()) continue;
                        if(! f->getFrameType() == FT_TABLE) continue;
                        if(f->getName() == tableName) {
                            table = static_cast<KWTableFrameSet *> (f);
                            break;
                        }
                    }
                    if ( !table ) {
                        table = new KWTableFrameSet( this );

                        table->setName( tableName );
                        frames.append( table );
                    }
                    KWTableFrameSet::Cell *cell = new KWTableFrameSet::Cell( table, _row, _col );
                    cell->setVisible( _visible );
                    cell->setName( fsname );
                    cell->load( framesetElem );
                    cell->setFrameInfo( frameInfo );
                    cell->setIsRemoveableHeader( removeable );
                    cell->m_rows = _rows;
                    cell->m_cols = _cols;
                }
                else
                {
                    KWTextFrameSet *fs = new KWTextFrameSet( this );
                    fs->setVisible( _visible );
                    fs->setName( fsname );
                    fs->load( framesetElem );
                    fs->setFrameInfo( frameInfo );
                    fs->setIsRemoveableHeader( removeable );
                    frames.append( fs ); // don't use addFrameSet here. We'll call finalize() once and for all in completeLoading

                    // Old file format had autoCreateNewFrame as a frameset attribute,
                    // and our templates still use that.
                    if ( framesetElem.hasAttribute( "autoCreateNewFrame" ) )
                    {
                        FrameBehaviour behav = static_cast<FrameBehaviour>( framesetElem.attribute( "autoCreateNewFrame" ).toInt() );
                        QListIterator<KWFrame> frameIt( fs->frameIterator() );
                        for ( ; frameIt.current() ; ++frameIt ) // Apply it to all frames
                            frameIt.current()->setFrameBehaviour( behav );
                    }
                }
            } break;
            case FT_PICTURE: {
                KWPictureFrameSet *fs = new KWPictureFrameSet( this );
                fs->setName( fsname );
                fs->load( framesetElem );
                fs->setFrameInfo( frameInfo );
                frames.append( fs );
            } break;
            case FT_FORMULA: {
                KWFormulaFrameSet *fs = new KWFormulaFrameSet( this );
                fs->setName( fsname );
                fs->load( framesetElem );
                fs->setFrameInfo( frameInfo );
                frames.append( fs );
            } break;
            default: break;
        }
    }
}

/*===================================================================*/
bool KWDocument::completeLoading( KoStore *_store )
{
    if ( _store ) {
        QString str = urlIntern.isEmpty() ? KURL( url() ).path() : urlIntern;

        QStringList::Iterator it = pixmapKeys.begin();
        QStringList::Iterator nit = pixmapNames.begin();

        for ( ; it != pixmapKeys.end(); ++it, ++nit ) {
            QString u;
            if ( !( *nit ).isEmpty() )
                u = *nit;
            else {
                u = str;
                u += "/";
                u += *it;
            }

            QImage img;

            if ( _store->open( u ) ) {
                KoStoreDevice dev(_store );
                QImageIO io( &dev, 0 );
                io.read( );
                img = io.image();

                _store->close();
            } else {
                u.prepend( "file:" );
                if ( _store->open( u ) ) {
                  KoStoreDevice dev(_store );
                  QImageIO io( &dev, 0 );
                  io.read( );
                  img = io.image();

                  _store->close();
                }
            }

            if ( !img.isNull() )
                m_imageCollection.insertImage( *it, img );
        }
    }

    processImageRequests();

    QMapIterator<int, KWAnchorPosition> itanch = m_anchorRequests.begin();
    for ( ; itanch != m_anchorRequests.end(); ++itanch )
    {
        //kdDebug(32001) << "KWDocument::completeLoading anchoring frameset " << itanch.key() << endl;
        KWFrameSet * fs = getFrameSet( itanch.key() );
        ASSERT( fs );
        if ( fs )
            fs->setAnchored( itanch.data(), true );
    }
    m_anchorRequests.clear();

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
    QMapIterator<QString,KWTextImage *> it2 = m_imageRequests.begin();
    for ( ; it2 != m_imageRequests.end(); ++it2 )
    {
        kdDebug(32001) << "KWDocument::completeLoading loading image " << it2.key() << endl;
        it2.data()->setImage( m_imageCollection.findImage( it2.key() ) );
    }
    m_imageRequests.clear();

    QMapIterator<QString,KWPictureFrameSet *> it3 = m_imageRequests2.begin();
    for ( ; it3 != m_imageRequests2.end(); ++it3 )
        it3.data()->setImage( m_imageCollection.findImage( it3.key() ) );
    m_imageRequests2.clear();
}

/*================================================================*/
QDomDocument KWDocument::saveXML()
{
    QDomDocument doc( "DOC" );
    doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement kwdoc = doc.createElement( "DOC" );
    kwdoc.setAttribute( "editor", "KWord" );
    kwdoc.setAttribute( "mime", "application/x-kword" );
    syntaxVersion = CURRENT_SYNTAX_VERSION;
    kwdoc.setAttribute( "syntaxVersion", syntaxVersion );
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

    KWFrameSet *frameSet = 0L;
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        frameSet = getFrameSet( i );
        if ( frameSet->getFrameType() != FT_PART )
            frameSet->save( framesets );
    }

    QDomElement styles = doc.createElement( "STYLES" );
    kwdoc.appendChild( styles );
    for ( KWStyle * p = m_styleList.first(); p != 0L; p = m_styleList.next() )
        p->save( styles );

    QDomElement pixmaps = doc.createElement( "PIXMAPS" );
    kwdoc.appendChild( pixmaps );

    KWImageCollection::ConstIterator it = m_imageCollection.begin();
    KWImageCollection::ConstIterator end = m_imageCollection.end();
    QStringList images, keys;
    int i = 0;
    for ( ; it != end; ++it ) {
        QString fileName = it.key();
        if ( keys.contains( it.key() ) || images.contains( fileName ) )
            continue;
        QString format = QFileInfo( fileName ).extension().upper();
        if ( format == "JPG" )
            format = "JPEG";
        if ( QImage::outputFormats().find( QFile::encodeName(format) ) == -1 )
            format = "BMP";
        QString pictureName = QString( "pictures/picture%1.%2" ).arg( ++i ).arg( format.lower() );
        if ( !isStoredExtern() )
          pictureName.prepend( url().url() + "/" );

        QDomElement key = doc.createElement( "KEY" );
        pixmaps.appendChild( key );
        key.setAttribute( "key", fileName );
        key.setAttribute( "name", pictureName );

        keys.append( it.key() );
        images.append( fileName );
    }

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
            if ( fs->getFrameType() == FT_PART &&
                 dynamic_cast<KWPartFrameSet*>( fs )->getChild() == curr )
                fs->save( settingsElem );
        }
    }

    return doc;
}

/*==============================================================*/
bool KWDocument::completeSaving( KoStore *_store )
{
    if ( !_store )
        return TRUE;

    QString u = KURL( url() ).path();

    KWImageCollection::ConstIterator it = m_imageCollection.begin();
    KWImageCollection::ConstIterator end = m_imageCollection.end();

    QStringList images, keys;
    int i = 0;

    for( ; it != end; ++it ) {
        QString fileName = it.key();
        if ( keys.contains( it.key() ) || images.contains( fileName ) )
            continue;

        QString format = QFileInfo( fileName ).extension().upper();
        if ( format == "JPG" )
            format = "JPEG";
        if ( QImage::outputFormats().find( QFile::encodeName(format) ) == -1 )
            format = "PNG";

        QString u2 = QString( "pictures/picture%1.%2" ).arg( ++i ).arg( format.lower() );
        if ( !isStoredExtern() )
          u2.prepend( url().url() + "/" );

        if ( _store->open( u2 ) ) {
            KoStoreDevice dev( _store );
            QImageIO io;
            io.setIODevice( &dev );
            io.setImage( (*it).image() );
            io.setFormat( QFile::encodeName(format) );
            io.write();
            _store->close();
        }
        keys.append( it.key() );
        images.append( fileName );
    }

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
        newZoomAndResolution( false );
    }

    QRect rect( _rect );
    // Translate the painter to avoid the margin
    /*painter.translate( -leftBorder(), -topBorder() );
    rect.moveBy( leftBorder(), topBorder() );*/

    drawBorders( &painter, rect, !transparent );

    QColorGroup gb = QApplication::palette().active();

    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameset = fit.current();
        if ( frameset->isVisible() && !frameset->isFloating() )
            frameset->drawContents( &painter, rect, gb, false /*onlyChanged*/, false /*resetChanged*/ );
    }
}

void KWDocument::drawBorders( QPainter *painter, const QRect & crect, bool clearEmptySpace )
{
    bool embedded = isEmbedded();
    QRegion region( crect );

    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet *frameset = fit.current();
        if ( frameset->isVisible() )
        {
            frameset->drawBorders( painter, crect, region );
        }
    }

    // Draw page borders (red), except when printing.
    if ( painter->device()->devType() != QInternal::Printer )
    {
        painter->save();
        painter->setPen( red );
        painter->setBrush( Qt::NoBrush );

        for ( int k = 0; k < getPages(); k++ )
        {
            int pagetop = pageTop( k );
            // using paperHeight() leads to rounding problems ( one pixel between two pages, belonging to none of them )
            QRect pageRect( 0, pagetop, paperWidth(), pageTop( k+1 ) - pagetop );
            if ( crect.intersects( pageRect ) )
            {
                //kdDebug() << "KWDocument::drawBorders drawing page rect " << DEBUGRECT( pageRect ) << endl;
                if ( !embedded )
                    painter->drawRect( pageRect );

                if ( clearEmptySpace )
                {
                    // Clear empty space. This is also disabled when printing because
                    // it is not needed (the blank space, well, remains blank )
                    painter->save();

                    if ( !embedded )
                    {
                        // Exclude red border line, to get the page contents rect
                        pageRect.rLeft() += 1;
                        pageRect.rTop() += 1;
                        pageRect.rRight() -= 1;
                        pageRect.rBottom() -= 1;
                        //kdDebug() << "KWDocument::drawBorders page rect w/o borders : " << DEBUGRECT( pageRect ) << endl;
                    }

                    // The empty space to clear up inside this page
                    QRegion emptySpaceRegion = region.intersect( pageRect );

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

                    //kdDebug() << "KWDocument::drawBorders clearEmptySpace in " << DEBUGRECT( emptySpaceRegion.boundingRect() ) << endl;
                    painter->fillRect( emptySpaceRegion.boundingRect(), QApplication::palette().active().brush( QColorGroup::Base ) );
                    painter->restore();
                }
            }
        }
        painter->restore();
    }
}

void KWDocument::insertObject( const KoRect& rect, KoDocumentEntry& _e )
{

    KoDocument* doc = _e.createDoc( this );
    if ( !doc || !doc->initDoc() )
        return;

    KWChild* ch = new KWChild( this, rect.toQRect(), doc );

    insertChild( ch );
    setModified( TRUE );

    KWPartFrameSet *frameset = new KWPartFrameSet( this, ch );
    KWFrame *frame = new KWFrame(frameset, rect.x(), rect.y(), rect.width(), rect.height() );
    frameset->addFrame( frame );
    addFrameSet( frameset );

    KWCreateFrameCommand *cmd=new KWCreateFrameCommand( i18n("Create a part frame"), this,  frame) ;
    addCommand(cmd);

    emit sig_insertObject( ch, frameset );

    frameChanged( frame ); // repaint etc.
}

QStringList KWDocument::fontList()
{
    if ( !m_fontList )
    {
        m_fontList = new QStringList;
        KFontChooser::getFontList(*m_fontList, false);
    }
    return *m_fontList;
}

KWStyle* KWDocument::findStyle( const QString & _name, bool noFallback )
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

    if ( noFallback )
        return 0L;
    qWarning( "Style '%s` is unknown, using default style", _name.latin1() );
    return m_styleList.first();
}

/*================================================================*/
/* Update all views of this document, area can be cleared
   before redrawing with the _erase flag. (false implied)
   All views EXCEPT the argument _view are updated ( 0L = all )
 */
void KWDocument::repaintAllViewsExcept( KWView *_view, bool erase )
{
    for ( KWView * viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        if ( viewPtr->getGUI() && viewPtr->getGUI()->canvasWidget() ) {
            if ( viewPtr != _view )
                viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
        }
    }
}

/*================================================================*/
void KWDocument::updateAllViewportSizes()
{
    //kdDebug(32002) << "KWDocument::updateAllViewportSizespages=" << m_pages << " " << paperWidth() << "x" << pageTop( m_pages ) << endl;
    emit sig_newContentsSize( paperWidth(), pageTop( m_pages ) /*i.e. bottom of last one*/ );
}

/*================================================================*/
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

/*================================================================*/
void KWDocument::updateAllStyleLists()
{
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->updateStyleList();
}

/*================================================================*/
void KWDocument::applyStyleChange( const QString & changedStyle )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit ) {
        KWTextFrameSet * frameSet = dynamic_cast<KWTextFrameSet*>( fit.current() );
        if ( frameSet )
            frameSet->applyStyleChange( changedStyle );
    }
}

/*================================================================*/
void KWDocument::repaintAllViews( bool erase )
{
    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
}

/*================================================================*/
void KWDocument::appendPage( /*unsigned int _page, bool redrawBackgroundWhenAppendPage*/ )
{
    int thisPageNum = m_pages-1;
    kdDebug(32002) << "KWDocument::appendPage m_pages=" << m_pages << " so thisPageNum=" << thisPageNum << endl;
    m_pages++;
    emit pageNumChanged();
    recalcVariables( VT_PGNUM );

    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        // don't add tables! A table cell ( frameset ) _must_ not have more than one frame!
        if ( frameSet->getFrameType() == FT_TABLE ) continue;

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
            //kdDebug(32002) << "KWDocument::appendPage frame=" << frame << " frame->pageNum()=" << frame->pageNum() << endl;
            //kdDebug(32002) << "KWDocument::appendPage frame->getNewFrameBehaviour()==" << frame->getNewFrameBehaviour() << " Reconnect=" << Reconnect << endl;
            if ( (frame->pageNum() == thisPageNum ||
                  (frame->pageNum() == thisPageNum -1 && frame->getSheetSide() != AnySide)) &&
                 (frame->getNewFrameBehaviour()==Reconnect ||
                  frame->getNewFrameBehaviour()==Copy) ) {

                switch(frameSet->getFrameType()) {
                    case FT_TEXT:  {
                        //kdDebug(32002) << "KWDocument::appendPage, copying text frame" << endl;
                        // make a new frame.
                        KWFrame *frm = frame->getCopy();
                        frm->moveBy( 0, ptPaperHeight() );
                        frm->setPageNum( frame->pageNum()+1 );
                        newFrames.append( frm );
                        } break;
                    case FT_PICTURE:  { // can not be copied at the moment.
                        } break;
                    case FT_PART: {  // can never be copied ?
                        } break;
                    case FT_FORMULA: {  // can never be copied ?
                        } break;
                    default: {}
                }
            }
        }
        QListIterator<KWFrame> newFrameIt( newFrames );
        for ( ; newFrameIt.current() ; ++newFrameIt )
            frameSet->addFrame( newFrameIt.current() );
    }
/*
    if ( redrawBackgroundWhenAppendPage )
        drawAllBorders();
*/
    updateAllViewportSizes();

    if ( isHeaderVisible() || isFooterVisible() )
        recalcFrames();  // Get headers and footers on the new page
    // setModified(TRUE); This is called by formatMore, possibly on loading -> don't set modified
}

bool KWDocument::canRemovePage( int num, KWFrame *f )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        if ( frameSet->getFrameInfo() != FI_BODY )
            continue;
        QListIterator<KWFrame> frameIt( frameSet->frameIterator() );
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frm = frameIt.current();
            if ( frm != f && frm->pageNum() == num )
                return FALSE;
        }
    }
    kdDebug(32002) << "KWDocument::canRemovePage " << num << " frame=" << f << "-> TRUE" << endl;
    return TRUE;
}

void KWDocument::removePage( int num )
{
    //kdDebug() << "KWDocument::removePage " << num << endl;
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        if ( frameSet->getFrameInfo() != FI_BODY )
            continue;
        QListIterator<KWFrame> frameIt( frameSet->frameIterator() );
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frm = frameIt.current();
            if ( frm->pageNum() == num )
                frameSet->delFrame( frm, true );
        }
    }
    m_pages--;
    //kdDebug() << "KWDocument::removePage -- -> " << m_pages << endl;
    emit pageNumChanged();
    recalcVariables( VT_PGNUM );
    recalcFrames();
}

/*================================================================*/
KWFrameSet * KWDocument::getFrameSet( double mx, double my )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( fit.toLast(); fit.current() ; --fit )
    {
        KWFrameSet *frameSet = fit.current();
        if ( frameSet->contains( mx, my ) ) {
            if ( !frameSet->isVisible() )
                continue;
            if ( frameSet->isRemoveableHeader() )
                continue;
            return frameSet;
        }
    }

    return 0L;
}

/*================================================================*/
/* Select the first frame where the x and y coords fall into
   returns 0 if none was selected, return 1 if selected, return 2
   if the frame was allready selected.
*/
int KWDocument::selectFrame( double mx, double my, bool simulate )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( fit.toLast(); fit.current() ; --fit )
    {
        KWFrameSet * frameSet = fit.current();
        if ( frameSet->contains( mx, my ) ) {
            if ( !frameSet->isVisible() )
                continue;
            if ( frameSet->isRemoveableHeader() )
                continue;
            return frameSet->selectFrame( mx, my, simulate );
        }
    }

    if ( !simulate )
        deSelectAllFrames();
    return 0;
}

/*================================================================*/
void KWDocument::deSelectFrame( double mx, double my )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( fit.toLast(); fit.current() ; --fit )
    {
        KWFrameSet * frameSet = fit.current();
        if ( frameSet->contains( mx, my ) )
            frameSet->deSelectFrame( mx, my );
    }
}

/*================================================================*/
void KWDocument::deSelectAllFrames()
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( fit.toLast(); fit.current() ; --fit )
    {
        QListIterator<KWFrame> frameIt = fit.current()->frameIterator();
        for ( ; frameIt.current(); ++frameIt )
            frameIt.current()->setSelected( FALSE );
    }
}

/*================================================================*/
QCursor KWDocument::getMouseCursor( double mx, double my )
{
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( fit.toLast(); fit.current() ; --fit )
    {
        KWFrameSet *frameSet = fit.current();
        if ( !frameSet->isVisible() )
            continue;
        if ( frameSet->isRemoveableHeader() )
            continue;
        if ( frameSet->contains( mx, my ) )
            return frameSet->getMouseCursor( mx, my );
    }

    return arrowCursor;
}


/*================================================================*/
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

/*================================================================*/
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

/*================================================================*/
KWFrameSet *KWDocument::getFirstSelectedFrameSet() {
    return getFirstSelectedFrame()->getFrameSet();
}

/*================================================================*/
void KWDocument::updateAllFrames()
{
    //kdDebug(32002) << "KWDocument::updateAllFrames " << frames.count() << " framesets." << endl;
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        //kdDebug() << "KWDocument::updateAllFrames updating " << fit.current() << endl;
        fit.current()->updateFrames();
    }
}

// Tell this method when a frame is moved / resized / created / deleted
// and everything will be update / repainted accordingly
void KWDocument::frameChanged( KWFrame * frame, KWView * view )
{
    updateAllFrames();
    if ( !frame || frame->getRunAround() != RA_NO )
        layout();
    repaintAllViewsExcept( view );
}

void KWDocument::framesChanged( const QList<KWFrame> & frames, KWView * view )
{
    updateAllFrames();
    QListIterator<KWFrame> it( frames );
    for ( ; it.current() ; ++it )
        if ( it.current()->getRunAround() != RA_NO )
        {
            layout();
            repaintAllViewsExcept( view );
            return;
        }
}

/*================================================================*/
void KWDocument::setHeaderVisible( bool h )
{
    m_headerVisible = h;
    recalcFrames();
    repaintAllViews( true );
}

/*================================================================*/
void KWDocument::setFooterVisible( bool f )
{
    m_footerVisible = f;
    recalcFrames();
    repaintAllViews( true );
}

/*================================================================*/
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

/*================================================================*/
bool KWDocument::isOnlyOneFrameSelected() {
    return getSelectedFrames().count()==1;
}

/*================================================================*/
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

void KWDocument::addImageRequest( const QString &filename, KWTextImage *img )
{
    m_imageRequests.insert( filename, img );
}

void KWDocument::addImageRequest( const QString &filename, KWPictureFrameSet *fs )
{
    m_imageRequests2.insert( filename, fs );
}

void KWDocument::addAnchorRequest( int fsnum, const KWAnchorPosition &anchorPos )
{
    m_anchorRequests.insert( fsnum, anchorPos );
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

void KWDocument::unregisterVariable( KWVariable *var )
{
    variables.take( variables.findRef( var ) );
}

void KWDocument::recalcVariables( int type )
{
    kdDebug() << "KWDocument::recalcVariables " << type << endl;
    bool update = false;
    QListIterator<KWVariable> it( variables );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->type() == type )
        {
            update = true;
            it.current()->recalc();
        }
    }
    if ( update )
        repaintAllViews();
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

void KWDocument::createContents()
{
    contents->createContents();
    repaintAllViews( /*true*/ );
}

void KWDocument::getPageLayout( KoPageLayout& _layout, KoColumns& _cl, KoKWHeaderFooter& _hf )
{
    _layout = m_pageLayout;
    _cl = m_pageColumns;
    _hf = m_pageHeaderFooter;
}

void KWDocument::addFrameSet( KWFrameSet *f )
{
    frames.append(f);
    f->finalize();
    setModified( true );
}

void KWDocument::delFrameSet( KWFrameSet *f, bool deleteit)
{
  if (deleteit)
    frames.remove( f );
  else
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
void KWDocument::printDebug() {

    static const char * typeFrameset[] = { "base", "txt", "pic", "part", "formula", "table","ERROR" };
    static const char * infoFrameset[] = { "body", "first header", "odd headers", "even headers",
                                           "first footer", "odd footers", "even footers", "footnote", "ERROR" };
    static const char * frameBh[] = { "AutoExtendFrame", "AutoCreateNewFrame", "Ignore", "ERROR" };
    static const char * newFrameBh[] = { "Reconnect", "NoFollowup", "Copy" };
    static const char * runaround[] = { "No Runaround", "Bounding Rect", "Horizontal Space", "ERROR" };

    kdDebug() << "----------------------------------------"<<endl;
    kdDebug() << "                 Debug info"<<endl;
    kdDebug() << "Document:" << this <<endl;
    kdDebug() << "Type of document: (0=WP, 1=DTP) " << processingType() <<endl;
    kdDebug() << "size: x:" << ptLeftBorder()<< ", y:"<<ptTopBorder() << ", w:"<< ptPaperWidth() << ", h:"<<ptPaperHeight()<<endl;
    kdDebug() << "Header visible: " << isHeaderVisible() << endl;
    kdDebug() << "Footer visible: " << isFooterVisible() << endl;
    kdDebug() << "Units: " << getUnit() <<endl;
    kdDebug() << "# Framesets: " << getNumFrameSets() <<endl;
    for (unsigned int iFrameset = 0; iFrameset < getNumFrameSets(); iFrameset++ ) {
        KWFrameSet * frameset = getFrameSet(iFrameset);
        kdDebug() << "Frameset " << iFrameset << ": '" <<
            frameset->getName() << "' (" << frameset << ")" <<endl;
        kdDebug() << " |  Visible: " << frameset->isVisible() << endl;
        kdDebug() << " |  Type: " << typeFrameset[ frameset->getFrameType() ] << endl;
        kdDebug() << " |  Info: " << infoFrameset[ frameset->getFrameInfo() ] << endl;

        QListIterator<KWFrame> frameIt = frameset->frameIterator();
        for ( unsigned int j = 0; frameIt.current(); ++frameIt, ++j ) {
            KWFrame * frame = frameIt.current();
            KWFrameSet * fs = frame->getFrameSet();
            if(fs != frameset) {
                KWTableFrameSet::Cell *cell = (KWTableFrameSet::Cell *)fs;
                kdDebug() << " |  |- row :" << cell->m_row << endl;
                kdDebug() << " |  |- col :" << cell->m_col << endl;
                kdDebug() << " |  |- rows:" << cell->m_rows << endl;
                kdDebug() << " |  +- cols:" << cell->m_cols << endl;
            }
            kdDebug() << " +-- Frame " << j << " of "<< frameset->getNumFrames() << "    (" << frame << ")" << endl;
            kdDebug() << "     Rectangle : " << frame->x() << "," << frame->y() << " " << frame->width() << "x" << frame->height() << endl;
            kdDebug() << "     RunAround: "<< runaround[ frame->getRunAround() ] << endl;
            kdDebug() << "     FrameBehaviour: "<< frameBh[ frame->getFrameBehaviour() ] << endl;
            kdDebug() << "     NewFrameBehaviour: "<< newFrameBh[ frame->getNewFrameBehaviour() ] << endl;
            kdDebug() << "     SheetSide "<< frame->getSheetSide() << endl;
            if(frame->isSelected())
                kdDebug() << " *   Page "<< frame->pageNum() << endl;
            else
                kdDebug() << "     Page "<< frame->pageNum() << endl;
        }
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
        it.current()->layout();
}

KFormulaDocument* KWDocument::getFormulaDocument()
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
        for ( ; it.current() ; ++it )
            it.current()->getGUI()->getView()->showFrameBorders( frame->getLeftBorder(), frame->getRightBorder(), frame->getTopBorder(), frame->getBottomBorder() );
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

#include "kwdoc.moc"
