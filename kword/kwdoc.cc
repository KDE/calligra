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
#include <qprinter.h>
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

#include <kurl.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <kglobal.h>

#include <unistd.h>
#include <math.h>

#include "kwframe.h"
#include "kwtextframeset.h"
#include "kwgroupmanager.h"
#include "kwdoc.h"
#include "kwcanvas.h"
#include "kwtextparag.h"
#include "defs.h"
#include "kwutils.h"
//#include "serialletter.h"
#include "contents.h"
#include "kwview.h"
#include "kwfactory.h"
#include <kdebug.h>
#include <assert.h>

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
KWChild::KWChild( KWDocument *_wdoc, const QRect& _rect, KoDocument *_doc, int diffx, int diffy )
    : KoDocumentChild( _wdoc, _doc, QRect( _rect.left() + diffx, _rect.top() + diffy, _rect.width(), _rect.height() ) )
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

// This is duplicated from KoDocumentChild, because it uses lowercase tags
// and we use uppercase tags :(
bool KWChild::load( const QDomElement& element )
{
    m_tmpURL = element.attribute("url");
    m_tmpMimeType = element.attribute("mime");

    if ( m_tmpURL.isEmpty() )
    {
        kdDebug(30003) << "Empty 'url' attribute in OBJECT" << endl;
        return false;
    }
    if ( m_tmpMimeType.isEmpty() )
    {
        kdDebug(30003) << "Empty 'mime' attribute in OBJECT" << endl;
        return false;
    }

    // <RECT>
    QDomElement rectElem = element.namedItem( "RECT" ).toElement();
    if ( !rectElem.isNull() )
    {
        m_tmpGeometry = QRect(
            KWDocument::getAttribute( rectElem, "x", 0 ),
            KWDocument::getAttribute( rectElem, "y", 0 ),
            KWDocument::getAttribute( rectElem, "w", 1 ),
            KWDocument::getAttribute( rectElem, "h", 1 ) );
        setGeometry( m_tmpGeometry );
    }
    else
    {
        kdDebug(30003) << "Missing RECT in OBJECT" << endl;
        return false;
    }

    return true;
}

QDomElement KWChild::save( QDomDocument& doc )
{
    assert( document() );
    QDomElement e = doc.createElement( "OBJECT" );
    e.setAttribute( "url", document()->url().url() );
    e.setAttribute( "mime", document()->nativeFormatMimeType() );
    QDomElement rect = doc.createElement( "RECT" );
    rect.setAttribute( "x", geometry().left() );
    rect.setAttribute( "y", geometry().top() );
    rect.setAttribute( "w", geometry().width() );
    rect.setAttribute( "h", geometry().height() );
    e.appendChild(rect);
    return e;
}


/******************************************************************/
/* Class: KWDocument                                      */
/******************************************************************/

/*================================================================*/
KWDocument::KWDocument(QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
      unit( "mm" ), // footNoteManager( this ),
      history( actionCollection(), false ),
      autoFormat(this),
      urlIntern()
{
    m_lstViews.setAutoDelete( false );
    m_lstChildren.setAutoDelete( true );
    m_styleList.setAutoDelete( true );
//    varFormats.setAutoDelete(true);

    setInstance( KWFactory::global() );

    // ## TODO make this configurable
    m_gridX = m_gridY = 10;

    _viewFormattingChars = FALSE;
    _viewFrameBorders = TRUE;
    _viewTableGrid = TRUE;

    setEmpty();
    setModified(false);

    styleMask = U_FONT_FAMILY_ALL_SIZE | U_COLOR | U_BORDER | U_INDENT |
                         U_NUMBERING | U_ALIGN | U_TABS | U_SMART;
    m_headerVisible = false;
    m_footerVisible = false;

    m_lastStyle = 0L;
    frames.setAutoDelete( TRUE );
    grpMgrs.setAutoDelete( TRUE );
 //   variables.setAutoDelete( FALSE );

//    slDataBase = new KWSerialLetterDataBase( this );
//    slRecordNum = -1;

    connect( this, SIGNAL( childChanged( KoDocumentChild * ) ),
             this, SLOT( slotChildChanged( KoDocumentChild * ) ) );

    spellCheck = FALSE;
    contents = new KWContents( this );

    // Set no-op initial values (for setZoomAndResolution)
    m_zoomedResolutionX = 1;
    m_zoomedResolutionY = 1;
    m_zoom = 100;

    setZoomAndResolution( 100, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
    syntaxVersion = CURRENT_SYNTAX_VERSION;

    m_pKSpellConfig=0;

    connect( &history, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
    connect( &history, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );
}

void KWDocument::setZoomAndResolution( int zoom, int dpiX, int dpiY )
{
    //double oldZoomedResolutionX = m_zoomedResolutionX;
    //double oldZoomedResolutionY = m_zoomedResolutionY;

    m_zoom = zoom;
    // m_resolution[XY] is in pixel per pt
    m_resolutionX = POINT_TO_INCH( static_cast<double>(dpiX) );
    m_resolutionY = POINT_TO_INCH( static_cast<double>(dpiY) );
    m_zoomedResolutionX = static_cast<double>(m_zoom) * m_resolutionX / 100.0;
    m_zoomedResolutionY = static_cast<double>(m_zoom) * m_resolutionY / 100.0;
    kdDebug() << "KWDocument::setZoomAndResolution " << zoom << " " << dpiX << "," << dpiY
              << " m_resolutionX=" << m_resolutionX << " m_zoomedResolutionX=" << m_zoomedResolutionX
              << " m_resolutionY=" << m_resolutionY << " m_zoomedResolutionY=" << m_zoomedResolutionY << endl;


    // Update all fonts
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->zoom();

    updateAllViewportSizes();
    repaintAllViews( true );
}

bool KWDocument::initDoc()
{
    m_pageLayout.unit = PG_MM;
    m_pages = 1;

    m_pageColumns.columns = 1; //STANDARD_COLUMNS;
    m_pageColumns.ptColumnSpacing = tableCellSpacing;
    m_pageColumns.mmColumnSpacing = POINT_TO_MM( tableCellSpacing );
    m_pageColumns.inchColumnSpacing = POINT_TO_INCH( tableCellSpacing );

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
    m_pageColumns.mmColumnSpacing = POINT_TO_MM( tableCellSpacing );
    m_pageColumns.inchColumnSpacing = POINT_TO_INCH( tableCellSpacing );

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
        //kdDebug() << "KWDocument::setPageLayout WP" << endl;
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

double KWDocument::ptColumnWidth() const
{
    return ( ptPaperWidth() - ptLeftBorder() - ptRightBorder() -
             ptColumnSpacing() * ( m_pageColumns.columns - 1 ) )
        / m_pageColumns.columns;
}

/*================================================================*/
/* append headers and footers if needed */
void KWDocument::recalcFrames( bool /*_cursor*/)
{
    if ( frames.isEmpty() )
        return;

    KWFrameSet *frameset = frames.at( 0 );

    unsigned int frms = frameset->getNumFrames();

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

    if ( m_processingType == WP ) {

        int headOffset = 0, footOffset = 0;
        int oldPages = m_pages;

        // Determine number of pages
        m_pages = static_cast<int>( ceil( static_cast<double>( frms ) / static_cast<double>( m_pageColumns.columns ) ) );
        int pages2=0;
        for (int m = getNumFrameSets()-1; m>=0; m--) {
            KWFrameSet *fs=getFrameSet(m);
            for (int n = fs->getNumFrames()-1;  n >=0; n--) {
                pages2=QMAX(pages2, fs->getFrame(n)->bottom());
            }
        }
        pages2=static_cast<int>(pages2 / ptPaperHeight());
        //kdDebug() << "KWDocument::recalcFrames, WP, m_pages=" << m_pages << " pages2=" << pages2 << endl;

        m_pages=QMAX(pages2, m_pages);
        if ( m_pages != oldPages )
            emit pageNumChanged();

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
            int h = evenHeader->getFrame( 0 )->height();
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
            int h1 = evenHeader->getFrame( 0 )->height();
            int h2 = oddHeader->getFrame( 0 )->height();
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
            int h = firstHeader->getFrame( 0 )->height();
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
            int h = evenFooter->getFrame( 0 )->height();
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
            int h1 = evenFooter->getFrame( 0 )->height();
            int h2 = oddFooter->getFrame( 0 )->height();
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
            int h = firstFooter->getFrame( 0 )->height();
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
    emit sigProgress( 0 );
    kdDebug() << "KWDocument::loadXML" << endl;
    pixmapKeys.clear();
    pixmapNames.clear();
//    imageRequests.clear();
    imageRequests2.clear();

    m_pageLayout.unit = PG_MM;

    m_pageColumns.columns = 1; //STANDARD_COLUMNS;
    m_pageColumns.ptColumnSpacing = tableCellSpacing;
    m_pageColumns.mmColumnSpacing = POINT_TO_MM( tableCellSpacing );
    m_pageColumns.inchColumnSpacing = POINT_TO_INCH( tableCellSpacing );

    m_pageHeaderFooter.header = HF_SAME;
    m_pageHeaderFooter.footer = HF_SAME;
    m_pageHeaderFooter.ptHeaderBodySpacing = 10;
    m_pageHeaderFooter.ptFooterBodySpacing = 10;
    m_pageHeaderFooter.inchHeaderBodySpacing = POINT_TO_INCH( 10 );
    m_pageHeaderFooter.inchFooterBodySpacing = POINT_TO_INCH( 10 );
    m_pageHeaderFooter.mmHeaderBodySpacing = POINT_TO_MM( 10 );
    m_pageHeaderFooter.mmFooterBodySpacing = POINT_TO_MM( 10 );

#if 0
    defaultParagLayout = new KWParagLayout( this );
    defaultParagLayout->setName( "Standard" );
    defaultParagLayout->setCounterType( KWParagLayout::CT_NONE );
    defaultParagLayout->setCounterDepth( 0 );

    KWFormat f( this );
    f.setUserFont( findUserFont( "helvetica" ) );
    f.setWeight( 75 );
    f.setPTFontSize( 24 );
    KWParagLayout *lay = new KWParagLayout( this );
    lay->setName( "Head 1" );
    lay->setFollowingParagLayout( "Standard" );
    lay->setCounterType( KWParagLayout::CT_NUM );
    lay->setCounterDepth( 0 );
    lay->setStartCounter( 1 );
    lay->setCounterRightText( "." );
    lay->setNumberingType( KWParagLayout::NT_CHAPTER );
    lay->setFormat( f );

    f.setPTFontSize( 16 );
    lay = new KWParagLayout( this );
    lay->setName( "Head 2" );
    lay->setFollowingParagLayout( "Standard" );
    lay->setCounterType( KWParagLayout::CT_NUM );
    lay->setCounterDepth( 1 );
    lay->setStartCounter( 1 );
    lay->setCounterRightText( "." );
    lay->setNumberingType( KWParagLayout::NT_CHAPTER );
    lay->setFormat( f );

    f.setPTFontSize( 12 );
    lay = new KWParagLayout( this );
    lay->setName( "Head 3" );
    lay->setFollowingParagLayout( "Standard" );
    lay->setCounterType( KWParagLayout::CT_NUM );
    lay->setCounterDepth( 2 );
    lay->setStartCounter( 1 );
    lay->setCounterRightText( "." );
    lay->setNumberingType( KWParagLayout::NT_CHAPTER );
    lay->setFormat( f );

    if ( TRUE /*no variable formats were loaded*/) {
        varFormats.clear();
        varFormats.insert( VT_DATE_FIX, new KWVariableDateFormat() );
        varFormats.insert( VT_DATE_VAR, new KWVariableDateFormat() );
        varFormats.insert( VT_TIME_FIX, new KWVariableTimeFormat() );
        varFormats.insert( VT_TIME_VAR, new KWVariableTimeFormat() );
        varFormats.insert( VT_PGNUM, new KWVariablePgNumFormat() );
        varFormats.insert( VT_CUSTOM, new KWVariableCustomFormat() );
        varFormats.insert( VT_SERIALLETTER, new KWVariableSerialLetterFormat() );
        // ... and so on ...
    }
#endif

    m_pages = 1;

    //QString tag;
    //QValueList<KOMLAttrib> lst;
    //QString name;

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
        __columns.columns = KWDocument::getAttribute( paper, "columns", 0 );
        getPointBasedAttribute( __columns, ColumnSpacing, paper, "columnspacing", 0.0 );

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
            getPointBasedAttribute( __columns, ColumnSpacing, paper, "ptColumnspc", 0.0 );

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
    if ( !attributes.isNull() )
    {
        m_processingType = static_cast<ProcessingType>( KWDocument::getAttribute( attributes, "processing", 0 ) );
        KWDocument::getAttribute( attributes, "standardpage", QString::null );
        m_headerVisible = static_cast<bool>( KWDocument::getAttribute( attributes, "hasHeader", 0 ) );
        m_footerVisible = static_cast<bool>( KWDocument::getAttribute( attributes, "hasFooter", 0 ) );
        unit = correctQString( KWDocument::getAttribute( attributes, "unit", "pt" ) );
    }

    switch ( KWUnit::unitType( unit ) ) {
    case U_MM: __pgLayout.unit = PG_MM;
        break;
    case U_PT: __pgLayout.unit = PG_PT;
        break;
    case U_INCH: __pgLayout.unit = PG_INCH;
        break;
    }
    setPageLayout( __pgLayout, __columns, __hf );

    emit sigProgress(10);

#if 0
        else if ( name == "FOOTNOTEMGR" ) {
            parser.parseTag( tag, name, lst );
            //QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            //for( ; it != lst.end(); ++it ) {
            //}
            footNoteManager.load( parser, lst );
        }
#endif
/*
        else if ( name == "SERIALL" ) {
            parser.parseTag( tag, name, lst );
            //QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            //for( ; it != lst.end(); ++it ) {
            //}
            slDataBase->load( parser, lst );
        } */

    emit sigProgress(15);

    QDomElement framesets = word.namedItem( "FRAMESETS" ).toElement();
    if ( !framesets.isNull() )
        loadFrameSets( framesets );

    emit sigProgress(75);

    QDomElement stylesElem = word.namedItem( "STYLES" ).toElement();
    if ( !stylesElem.isNull() )
        loadStyleTemplates( stylesElem );

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
            ch->load( object );
            r = ch->geometry();
            insertChild( ch );
            fs = new KWPartFrameSet( this, ch );
            frames.append( fs );
            QDomElement settings = embedded.namedItem( "SETTINGS" ).toElement();
            if ( !settings.isNull() )
            {
                kdDebug() << "KWDocument::loadXML loading embedded object" << endl;
                fs->load( settings );
            }
            else
                kdError() << "No <SETTINGS> tag in EMBEDDED" << endl;
        } else
            kdError() << "No <OBJECT> tag in EMBEDDED" << endl;
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
        KWFrameSet * frameset = fit.current();
        if ( frameset->getFrameInfo() == FI_FIRST_HEADER ) _first_header = TRUE;
        if ( frameset->getFrameInfo() == FI_EVEN_HEADER ) _odd_header = TRUE;
        if ( frameset->getFrameInfo() == FI_ODD_HEADER ) _even_header = TRUE;
        if ( frameset->getFrameInfo() == FI_FIRST_FOOTER ) _first_footer = TRUE;
        if ( frameset->getFrameInfo() == FI_EVEN_FOOTER ) _odd_footer = TRUE;
        if ( frameset->getFrameInfo() == FI_ODD_FOOTER ) _even_footer = TRUE;
        if ( frameset->getFrameInfo() == FI_FOOTNOTE ) _footnotes = TRUE;
    }

    // create defaults if they were not in the input file.

    if ( !_first_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        kdDebug() << "KWDocument::loadXML KWTextFrameSet created " << fs << endl;
        fs->setFrameInfo( FI_FIRST_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
            ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        kdDebug() << "KWDocument::loadXML KWFrame created " << frame << endl;
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_even_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_EVEN_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
            ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_odd_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_ODD_HEADER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptTopBorder(),
            ptPaperWidth() - ptLeftBorder() - ptRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_first_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_FIRST_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
            ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
            ptRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_even_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_EVEN_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
            ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
            ptRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_odd_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_ODD_FOOTER );
        KWFrame *frame = new KWFrame(fs, ptLeftBorder(), ptPaperHeight() -
            ptTopBorder() - 20, ptPaperWidth() - ptLeftBorder() -
            ptRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
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

    for ( unsigned int i = 0; i < getNumGroupManagers(); i++ ) {
        if ( getGroupManager( i )->isActive() )
            getGroupManager( i )->init();
    }

    KWChild *ch = 0L;
    for ( ch = m_lstChildren.first(); ch != 0; ch = m_lstChildren.next() ) {
        KWPartFrameSet *frameset = new KWPartFrameSet( this, ch );
        frameset->setName( i18n( "PartFrameset %1" ).arg( frames.count() + 1 ) );
        QRect r = ch->geometry();
        KWFrame *frame = new KWFrame(frameset, r.x(), r.y(), r.width(), r.height() );
        frameset->addFrame( frame );
        addFrameSet( frameset );
        emit sig_insertObject( ch, frameset );
    }

#if 0           // Not needed anymore
    if ( contents->hasContents() ) {
        QString name = *( --contents->ending() );
        KWParag *p = ( (KWTextFrameSet*)getFrameSet( 0 ) )->getFirstParag();
        KWParag *end = p;
        while ( p ) {
            if ( p->getParagName() == name ) {
                end = p;
                break;
            }
            p = p->getNext();
        }
        contents->setEnd( end );
    }
#endif

    autoFormat.addAutoFormatEntry( KWAutoFormatEntry("(C)", "©" ) );
    autoFormat.addAutoFormatEntry( KWAutoFormatEntry("(c)", "©" ) );
    autoFormat.addAutoFormatEntry( KWAutoFormatEntry("(R)", "®" ) );
    autoFormat.addAutoFormatEntry( KWAutoFormatEntry("(r)", "®" ) );

    // do some sanity checking on document.
    for (int i = getNumFrameSets()-1; i>-1; i--) {
        if(! getFrameSet(i)) {
            kdWarning () << "frameset " << i << " is NULL!!" << endl;
            frames.remove(i);
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
    for (int i = getNumGroupManagers()-1; i>-1; i--) {
        if(! getGroupManager(i)) {
            kdWarning () << "GroupManager " << i << " is NULL!!" << endl;
            grpMgrs.remove(i);
        } else {
            getGroupManager(i)->validate();
        }
    }
    emit sigProgress(-1);

    recalcFrames(); // This computes the number of pages (from the frames)
                    // for the first time (and adds footers/headers etc.)
    updateAllFrames();

#if 0 // If KWCanvas calls updateViewArea right, this is not needed anymore
    kdDebug() << "KWDocument::loadXML starting formatting" << endl;
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

    updateAllViews( 0L, true );     // in case any view exists already

    setModified( false );

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

        KWStyle *sty = new KWStyle( styleElem );
        //kdDebug() << "KWDocument::addStyleTemplate style's name is " << sty->name() << endl;
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
    // We progress from 15 to 85 -> 70-wide range, 15 offset.
    unsigned int perc = 70 * m_itemsLoaded / m_nrItemsToLoad;
    if ( perc != 70 * (m_itemsLoaded-1) / m_nrItemsToLoad ) // only emit if different from previous call
    {
        //kdDebug() << m_itemsLoaded << " items loaded. %=" << perc + 15 << endl;
        emit sigProgress( perc + 15 );
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
        QString _name;
        int _row, _col, _rows, _cols;
        bool removeable;
        bool _visible;
        QString fsname;

        frameType = static_cast<FrameType>( KWDocument::getAttribute( framesetElem, "frameType", FT_BASE ) );
        frameInfo = static_cast<FrameInfo>( KWDocument::getAttribute( framesetElem, "frameInfo", FI_BODY ) );
        _name = correctQString( KWDocument::getAttribute( framesetElem, "grpMgr", "" ) );
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
                KWTextFrameSet *fs = new KWTextFrameSet( this );
                fs->setVisible( _visible );
                fs->setName( fsname );
                fs->load( framesetElem );
                fs->setFrameInfo( frameInfo );
                fs->setIsRemoveableHeader( removeable );

                if ( !_name.isEmpty() ) {
                    KWGroupManager *grpMgr = 0L;
                    if ( getNumGroupManagers() > 0 ) {
                        for ( unsigned int i = 0; i < getNumGroupManagers(); i++ ) {
                            if ( getGroupManager( getNumGroupManagers() - 1 - i )->isActive() &&
                                getGroupManager( getNumGroupManagers() - 1 - i )->getName() == _name ) {
                                grpMgr = getGroupManager( getNumGroupManagers() - 1 - i );
                                break;
                            }
                        }
                    }
                    if ( !grpMgr ) {
                        grpMgr = new KWGroupManager( this );

                        grpMgr->setName( _name );
                        addGroupManager( grpMgr );
                    }
                    fs->setGroupManager( grpMgr );
                    grpMgr->addFrameSet( fs, _row, _col );
                    KWGroupManager::Cell *cell = grpMgr->getCell( _row, _col );
                    if ( cell ) {
                        cell->rows = _rows;
                        cell->cols = _cols;
                    }
                } else
                    frames.append( fs );

                // Old file format had autoCreateNewFrame as a frameset attribute,
                // and our templates still use that.
                if ( framesetElem.hasAttribute( "autoCreateNewFrame" ) )
                {
                    FrameBehaviour behav = static_cast<FrameBehaviour>( framesetElem.attribute( "autoCreateNewFrame" ).toInt() );
                    QListIterator<KWFrame> frameIt( fs->frameIterator() );
                    for ( ; frameIt.current() ; ++frameIt ) // Apply it to all frames
                        frameIt.current()->setFrameBehaviour( behav );
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

            QString filename = *it;
            int dashdash = filename.findRev( "--" );
            if ( dashdash != -1 )
                filename == filename.left( dashdash );

            QImage image( filename );
            if ( !img.isNull() )
                m_imageCollection.insertImage( *it, image );
        }
    }
/*
    QDictIterator<KWCharImage> it2( imageRequests );
    for ( ; it2.current(); ++it2 )
        it2.current()->setImage( imageCollection.getImage( it2.currentKey() ) ); */

    QDictIterator<KWPictureFrameSet> it3( imageRequests2 );
    for ( ; it3.current(); ++it3 )
        it3.current()->setImage( m_imageCollection.findImage( it3.currentKey() ) );

    return TRUE;
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
    paper.setAttribute( "width", m_pageLayout.ptWidth ); // ## the DTD says mm but the loading code says pt (!)
    paper.setAttribute( "height", m_pageLayout.ptHeight );
    paper.setAttribute( "orientation", static_cast<int>( m_pageLayout.orientation ) );
    paper.setAttribute( "columns", m_pageColumns.columns );
    paper.setAttribute( "columnspacing", m_pageColumns.ptColumnSpacing );
    paper.setAttribute( "hType", static_cast<int>( m_pageHeaderFooter.header ) );
    paper.setAttribute( "fType", static_cast<int>( m_pageHeaderFooter.footer ) );
    paper.setAttribute( "spHeadBody", m_pageHeaderFooter.ptHeaderBodySpacing );
    paper.setAttribute( "spFootBody", m_pageHeaderFooter.ptFooterBodySpacing );

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
    docattrs.setAttribute( "unit", getUnit() );

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

        QDomElement objectElem = curr->save( doc );
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
            format = "BMP";

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

#if 0 // old stuff, doesn't seem to be used anymore
void KWDocument::enableEmbeddedParts( bool f )
{
    KWFrameSet *frameSet = 0L;
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
        frameSet = getFrameSet( i );
        if ( frameSet->getFrameType() == FT_PART )
            dynamic_cast<KWPartFrameSet*>( frameSet )->enableDrawing( f );
    }
}
#endif

/*================================================================*/
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

/*================================================================*/
QStrList KWDocument::outputFormats()
{
    return QStrList();
}

/*================================================================*/
QStrList KWDocument::inputFormats()
{
    return QStrList();
}

/*================================================================*/
void KWDocument::addView( KoView *_view )
{
    m_lstViews.append( (KWView*)_view );
    KoDocument::addView( _view );
}

/*================================================================*/
void KWDocument::removeView( KoView *_view )
{
    m_lstViews.setAutoDelete( FALSE );
    m_lstViews.removeRef( static_cast<KWView*>(_view) );
    m_lstViews.setAutoDelete( TRUE );
    KoDocument::removeView( _view );
}

void KWDocument::addShell( KoMainWindow *shell )
{
    connect( shell, SIGNAL( documentSaved() ), &history, SLOT( documentSaved() ) );
    KoDocument::addShell( shell );
}

/*================================================================*/
KoView* KWDocument::createViewInstance( QWidget* parent, const char* name )
{
    return new KWView( parent, name, this );
}

/*================================================================*/
void KWDocument::paintContent( QPainter& /*painter*/, const QRect& /*rect*/, bool /*transparent*/ )
{
//     // Autsch.... that's an ugly hack!
//     if ( !tmpShell ) {
//      tmpShell = new KWordShell;
//      tmpShell->hide();
//      tmpShell->setRootPart( this );
//     }

//     if ( tmpShellSize.isNull() || tmpShellSize != rect ) {
//      tmpShellSize = rect;
//      tmpShell->resize( rect.width(), rect.height() );
//     }
//     tmpShell->resize( 1000, 1000 );

//     KWView *view = (KWView*)tmpShell->rootView();
//     KWCanvas *canvas = view->getGUI()->canvasWidget();

//     page->calcVisiblePages();

//     painter.save();
//     painter.eraseRect( rect );
//     QRect rr( rect );
//     rr.moveBy( painter.worldMatrix().dx(), painter.worldMatrix().dy() );
//     painter.setClipRect( rr );

//     QRegion r;
//     page->drawBorders( painter, rect, TRUE, &r );

//     int cf = page->currFrameSet == -1 ? page->fc->getFrameSet() - 1 : page->currFrameSet;

//     QPaintEvent e( rect, TRUE );
//     KWFormatContext *paintfc = new KWFormatContext( this, 1 );
//     for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
//      if ( page->redrawOnlyCurrFrameset && (int)i != cf )
//          continue;

//      switch ( getFrameSet( i )->getFrameType() ) {
//      case FT_PICTURE:
//          page->paintPicture( painter, i );
//          break;
//      case FT_PART:
//          page->paintPart( painter, i );
//          break;
//      case FT_FORMULA:
//          page->paintFormula( painter, i );
//          break;
//      case FT_TEXT:
//          page->paintText( painter, paintfc, i, &e );
//          break;
//      default: break;
//      }
//     }
//     delete paintfc;

//     page->finishPainting( &e, painter );

//     setPageLayoutChanged( FALSE );
//     painter.restore();
}

/*================================================================*/
void KWDocument::insertObject( const QRect& _rect, KoDocumentEntry& _e, int _diffx, int _diffy )
{

    KoDocument* doc = _e.createDoc( this );
    if ( !doc || !doc->initDoc() ) {
        KMessageBox::error( 0, i18n( "Due to an internal error, KWord could not\n"
                                     "perform the requested action."));
        return;
    }

    KWChild* ch = new KWChild( this, _rect, doc, _diffx, _diffy );

    insertChild( ch );
    setModified( TRUE );

    KWPartFrameSet *frameset = new KWPartFrameSet( this, ch );
    QRect r( _rect );
    r.moveBy( _diffx, _diffy );
    KWFrame *frame = new KWFrame(frameset, r.x(), r.y(), r.width(), r.height() );
    frameset->addFrame( frame );
    addFrameSet( frameset );

    emit sig_insertObject( ch, frameset );

    updateAllViews( 0 );
}

/*================================================================*/
void KWDocument::slotChildChanged( KoDocumentChild *child )
{
    setModified(TRUE);

    // Problem: we have to find the frame that contains this child.
    // We could do a lot better if we could connect this signal's changed() signal
    // directly in the appropriate KWPartFrameSet.
    // But framesets are not a QObject. Should it ?
    // (David)
    // They are now, so lets do it. (TZ)
    for ( unsigned int j = 0; j < frames.count(); j++ ) {
        if ( frames.at( j )->getFrameType() == FT_PART )
        {
            KWPartFrameSet *partFS = dynamic_cast<KWPartFrameSet*>( getFrameSet( j ) );
            if ( partFS->getChild() == child )
            {
                KWFrame *frame = partFS->getFrame( 0 );
                QRect r = child->geometry();
                frame->setCoords( r.left(), r.top(), r.right(), r.bottom() );
                break;
            }
        }
    }

    // ...to update the views - do we need that ?
    //emit sig_updateChildGeometry( _child );
}

/*================================================================*/
void KWDocument::draw( QPaintDevice *, long int, long int, float )
{
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
bool KWDocument::isPTYInFrame( unsigned int _frameSet, unsigned int _frame, unsigned int _ypos )
{
    return frames.at( _frameSet )->isPTYInFrame( _frame, _ypos );
}

/*================================================================*/
/* Update all views of this document, area can be cleared
   before redrawing with the _erase flag. (false implied)
   All views EXCEPT the argument _view are updated
 */
void KWDocument::updateAllViews( KWView *_view, bool erase )
{
    for ( KWView * viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        if ( viewPtr->getGUI() && viewPtr->getGUI()->canvasWidget() ) {
            if ( viewPtr != _view ) {
                viewPtr->getGUI()->canvasWidget()->repaintAll( erase );
            }
        }
    }
}

/*================================================================*/
void KWDocument::updateAllViewportSizes()
{
    //kdDebug() << "KWDocument::updateAllViewportSizespages=" << m_pages << " " << paperWidth() << "x" << paperHeight() * m_pages << endl;
    emit sig_newContentsSize( paperWidth(), paperHeight() * m_pages );
}

/*================================================================*/
void KWDocument::setUnitToAll()
{
    if ( unit == "mm" )
        m_pageLayout.unit = PG_MM;
    else if ( unit == "pt" )
        m_pageLayout.unit = PG_PT;
    else if ( unit == "inch" )
        m_pageLayout.unit = PG_INCH;

    for ( KWView *viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
        if ( viewPtr->getGUI() && viewPtr->getGUI()->canvasWidget() ) {
            viewPtr->getGUI()->getHorzRuler()->setUnit( getUnit() );
            viewPtr->getGUI()->getVertRuler()->setUnit( getUnit() );
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
    //kdDebug() << "KWDocument::appendPage m_pages=" << m_pages << " so thisPageNum=" << thisPageNum << endl;
    m_pages++;
    emit pageNumChanged();

    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWFrameSet * frameSet = fit.current();
        // don't add tables! A table cell ( frameset ) _must_ not have more than one frame!
        if ( frameSet->getGroupManager()) continue;

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
            //kdDebug() << "KWDocument::appendPage frame=" << frame << " frame->getPageNum()=" << frame->getPageNum() << endl;
            //kdDebug() << "KWDocument::appendPage frame->getNewFrameBehaviour()==" << frame->getNewFrameBehaviour() << " Reconnect=" << Reconnect << endl;
            if ( (frame->getPageNum() == thisPageNum ||
                  (frame->getPageNum() == thisPageNum -1 && frame->getSheetSide() != AnySide)) &&
                 (frame->getNewFrameBehaviour()==Reconnect ||
                  frame->getNewFrameBehaviour()==Copy) ) {

                switch(frameSet->getFrameType()) {
                    case FT_TEXT:  {
                        //kdDebug() << "KWDocument::appendPage, copying text frame" << endl;
                        // make a new frame.
                        KWFrame *frm = new KWFrame(frame->getFrameSet(), frame->x(), frame->y() + ptPaperHeight(), frame->width(), frame->height(), frame->getRunAround(), frame->getRunAroundGap() );
                        frm->setBackgroundColor( QBrush( frame->getBackgroundColor() ) );
                        frm->setNewFrameBehaviour(frame->getNewFrameBehaviour());
                        frm->setPageNum( frame->getPageNum()+1 );
                        frm->setFrameBehaviour(frame->getFrameBehaviour());
                        frm->setSheetSide(frame->getSheetSide());
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
        recalcFrames( false );  // Get headers and footers on the new page
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
            if ( frm != f && frm->getPageNum() == num )
                return FALSE;
        }
    }
    return TRUE;
}

void KWDocument::removePage( int num )
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
            if ( frm->getPageNum() == num )
                frameSet->delFrame( frm, true );
        }
    }
    m_pages--;
    emit pageNumChanged();
    recalcFrames();
}

/*================================================================*/
KWFrameSet * KWDocument::getFrameSet( unsigned int mx, unsigned int my )
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
int KWDocument::selectFrame( unsigned int mx, unsigned int my, bool simulate )
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
void KWDocument::deSelectFrame( unsigned int mx, unsigned int my )
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
QCursor KWDocument::getMouseCursor( unsigned int mx, unsigned int my )
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
void KWDocument::print( QPainter */*painter*/, QPrinter */*printer*/,
                           float /*left_margin*/, float /*top_margin*/ )
{
#if 0
    int tmpZoom = zoom;
    zoom = 100;
    KoPageLayout pgLayout;
    KoColumns cl;
    KoKWHeaderFooter hf;
    getPageLayout( pgLayout, cl, hf );
    if ( tmpZoom != zoom )
        setPageLayout( pgLayout, cl, hf );

    printer->setFullPage( TRUE );
    QList<KWFormatContext> fcList;
    fcList.setAutoDelete( TRUE );

    KWFormatContext *fc = 0L;
    unsigned int i = 0;

    for ( i = 0; i < frames.count(); i++ ) {
        if ( frames.at( i )->getFrameType() == FT_TEXT ) {
            frames.at( i )->setCurrent( 0 );
            fc = new KWFormatContext( this, i + 1 );
            fc->init( dynamic_cast<KWTextFrameSet*>( frames.at( i ) )->getFirstParag(), TRUE );
            fcList.append( fc );
        }
    }

    for ( i = 0; i < static_cast<unsigned int>( m_pages ); i++ ) {
        kapp->processEvents();
        // don't print if outside the bounduaries printer->fromPage() / printer->toPage()
        if ( 0 != printer->fromPage()  && i + 1 < static_cast<unsigned int>( printer->fromPage() ) ) {
            //kdDebug(32001) << "skipping page " << i+1 << ": it is less than " << printer->fromPage() << endl;
            continue;
        }
        if ( 0 != printer->toPage() && i + 1 > static_cast<unsigned int>( printer->toPage() ) ) {
            //kdDebug(32001) << "end print on page " << i+1 << ": it is greater than " << printer->toPage() << endl;
            break;
        }

        QRect pageRect( 0, i * ptPaperHeight(), ptPaperWidth(), ptPaperHeight() );
        unsigned int minus = 0;
        if ( i + 1 > static_cast<unsigned int>( printer->fromPage() ) )
            printer->newPage();
        printBorders( *painter, 0, i * ptPaperHeight(), ptPaperWidth(), ptPaperHeight() );

        QListIterator<KWFrameSet> fit = framesetsIterator();
        for ( ; fit.current() ; ++fit )
        {
            KWFrameSet * frameset = fit.current();
            if ( !frameset->isVisible() )
                continue;
            switch ( frameset->getFrameType() ) {
            case FT_PICTURE: {
                minus++;

                KWPictureFrameSet *picFS = dynamic_cast<KWPictureFrameSet*>( frameset );
                KWFrame *frame = picFS->getFrame( 0 );
                if ( !frame->intersects( pageRect ) ) break;

                QSize _size = QSize( frame->width(), frame->height() );
                if ( _size != picFS->getImage()->size() )
                    picFS->setSize( _size );

                painter->drawImage( frame->x(), frame->y() - i * ptPaperHeight(), *picFS->getImage() );
            } break;
            case FT_PART: {
                minus++;

                KWPartFrameSet *partFS = dynamic_cast<KWPartFrameSet*>( frameset );
                KWFrame *frame = partFS->getFrame( 0 );

                QPicture *pic = partFS->getPicture();

                painter->save();
                painter->setClipRect( frame->x(), frame->y() - i * ptPaperHeight(),
                                      frame->width() - 1, frame->height() - 1 );
                QRect r = painter->viewport();
                painter->setViewport( frame->x(), frame->y() - i * ptPaperHeight(), r.width(), r.height() );
                if ( pic ) painter->drawPicture( *pic );
                painter->setViewport( r );
                painter->restore();
            } break;
            case FT_FORMULA: {
                minus++;

                KWFormulaFrameSet *formulaFS = dynamic_cast<KWFormulaFrameSet*>( frameset );
                KWFrame *frame = formulaFS->getFrame( 0 );

                QPicture *pic = formulaFS->getPicture();

                painter->save();
                QRect r = painter->viewport();
                painter->setViewport( frame->x(), frame->y() - i * ptPaperHeight(), r.width(), r.height() );
                if ( pic )
                    painter->drawPicture( *pic );
                painter->setViewport( r );
                painter->restore();
            } break;
            case FT_TEXT: {
                bool bend = FALSE;
                bool reinit = TRUE;
                ///////fc = fcList.at(j - minus);
                if ( frameset->getFrameInfo() != FI_BODY ) {
                    if ( frameset->getFrameInfo() == FI_EVEN_HEADER ||
                         frameset->getFrameInfo() == FI_FIRST_HEADER ||
                         frameset->getFrameInfo() == FI_ODD_HEADER ) {
                        if ( !isHeaderVisible() ) continue;
                        switch ( getHeaderType() ) {
                        case HF_SAME: {
                            if ( frameset->getFrameInfo() != FI_EVEN_HEADER )
                                continue;
                        } break;
                        case HF_EO_DIFF: {
                            if ( frameset->getFrameInfo() == FI_FIRST_HEADER )
                                continue;
                            if ( ( ( i + 1 ) / 2 ) * 2 == i + 1 && frameset->getFrameInfo() == FI_ODD_HEADER )
                                continue;
                            if ( ( ( i + 1 ) / 2 ) * 2 != i + 1 && frameset->getFrameInfo() == FI_EVEN_HEADER )
                                continue;
                        } break;
                        case HF_FIRST_DIFF: {
                            if ( i == 0 && frameset->getFrameInfo() != FI_FIRST_HEADER )
                                continue;
                            if ( i > 0 && frameset->getFrameInfo() != FI_EVEN_HEADER )
                                continue;
                        } break;
                        default: break;
                        }
                    }
                    if ( frameset->getFrameInfo() == FI_EVEN_FOOTER ||
                         frameset->getFrameInfo() == FI_FIRST_FOOTER ||
                         frameset->getFrameInfo() == FI_ODD_FOOTER ) {
                        if ( !isFooterVisible() ) continue;
                        switch ( getFooterType() ) {
                        case HF_SAME: {
                            if ( frameset->getFrameInfo() != FI_EVEN_FOOTER )
                                continue;
                        } break;
                        case HF_EO_DIFF: {
                            if ( frameset->getFrameInfo() == FI_FIRST_FOOTER )
                                continue;
                            if ( ( ( i + 1 ) / 2 ) * 2 == i + 1 && frameset->getFrameInfo() == FI_ODD_FOOTER )
                                continue;
                            if ( ( ( i + 1 ) / 2 ) * 2 != i + 1 && frameset->getFrameInfo() == FI_EVEN_FOOTER )
                                continue;
                        } break;
                        case HF_FIRST_DIFF: {
                            if ( i == 0 && frameset->getFrameInfo() != FI_FIRST_FOOTER )
                                continue;
                            if ( i > 0 && frameset->getFrameInfo() != FI_EVEN_FOOTER )
                                continue;
                        } break;
                        default:
                            break;
                        }
                    }
                    fc->init( dynamic_cast<KWTextFrameSet*>( frameset )->getFirstParag(), TRUE,
                              frameset->getCurrent() + 1, i + 1 );
                    if ( static_cast<int>( frameset->getNumFrames() - 1 ) >
                         static_cast<int>( frameset->getCurrent() ) )
                        frameset->setCurrent( frameset->getCurrent() + 1 );
                    reinit = FALSE;
                }
                if ( reinit )
                    fc->init( dynamic_cast<KWTextFrameSet*>( frames.at( fc->getFrameSet() - 1 ) )->getFirstParag(), TRUE );
                while ( !bend ) {
                    printLine( *fc, *painter, 0, i * ptPaperHeight(), ptPaperWidth(),
                               ptPaperHeight(), FALSE, FALSE );
                    bend = !fc->makeNextLineLayout();
                }
            } break;
            default: minus++;
                break;
            }
        }
    }

    if ( tmpZoom != zoom ) {
        zoom = tmpZoom;
        setPageLayout( pgLayout, cl, hf );
    }
#endif
}

/*================================================================*/
void KWDocument::updateAllFrames()
{
    kdDebug() << "KWDocument::updateAllFrames" << endl;
    QListIterator<KWFrameSet> fit = framesetsIterator();
    for ( ; fit.current() ; ++fit )
        fit.current()->updateFrames();

    // Not needed anymore, since the intersects stuff has been replaced
    // with the clipregion/adjustLRMargin stuff... But doing stuff here
    // (to cache some info) isn't a bad idea, maybe.
#if 0
    QList<KWFrame> _frames;
    QList<KWGroupManager> mgrs;
    QList<KWFrame> del;
    _frames.setAutoDelete( FALSE );
    mgrs.setAutoDelete( FALSE );
    del.setAutoDelete( TRUE );
    unsigned int i = 0, j = 0;
    KWFrameSet *frameset = 0L;
    KWFrame *frame1, *frame2;
    KWFrame *framePtr = 0L;

    for ( i = 0; i < frames.count(); i++ ) {// loop over all known frameSets
        frameset = frames.at( i );
        if ( isAHeader( frameset->getFrameInfo() ) || isAFooter( frameset->getFrameInfo() ) || !frameset->isVisible() )
            continue;
        if ( frameset->getGroupManager() ) {
            if ( mgrs.findRef( frameset->getGroupManager() ) == -1 ) {
                QRect r = frameset->getGroupManager()->getBoundingRect();
                KWFrame *frm = new KWFrame(0L, r.x(), r.y(), r.width(), r.height() );
                _frames.append( frm );
                del.append( frm );
                mgrs.append( frameset->getGroupManager() );
            }
            for ( j = 0; j < frameset->getNumFrames(); ++j )
                frameset->getFrame( j )->clearIntersects();
        } else {
            for ( j = 0; j < frameset->getNumFrames(); j++ ) {
                _frames.append( frameset->getFrame( j ) );
                frameset->getFrame( j )->clearIntersects();
            }
        }
    }

    for ( i = 0; i < _frames.count(); i++ ) {
        framePtr = _frames.at( i );
        frame1 = _frames.at( i );
        _frames.at( i )->clearIntersects();

        for ( j = 0; j < _frames.count(); j++ ) {
            if ( i == j ) continue;

            frame2 = _frames.at( j );
            if ( frame1->intersects( QRect( frame2->x(), frame2->y(), frame2->width(), frame2->height() ) ) ) {
                QRect r = QRect( frame2->x(), frame2->y(), frame2->width(), frame2->height() );
                if ( r.left() > frame1->left() || r.top() > frame1->top() || r.right() < frame1->right() ||
                     r.bottom() < frame1->bottom() ) {
                    if ( r.left() < frame1->left() ) r.setLeft( frame1->left() );
                    if ( r.top() < frame1->top() ) r.setTop( frame1->top() );
                    if ( r.right() > frame1->right() ) r.setRight( frame1->right() );
                    if ( r.bottom() > frame1->bottom() ) r.setBottom( frame1->bottom() );
                    if ( r.left() - frame1->left() > frame1->right() - r.right() )
                        r.setRight( frame1->right() );
                    else
                        r.setLeft( frame1->left() );

                    framePtr->addIntersect( r );
                }
            }
        }
    }

    del.clear();
#endif
}

/*================================================================*/
void KWDocument::setHeaderVisible( bool h )
{
    m_headerVisible = h;
    recalcFrames( true );
    updateAllViews( 0L, true );
}

/*================================================================*/
void KWDocument::setFooterVisible( bool f )
{
    m_footerVisible = f;
    recalcFrames( true );
    updateAllViews( 0L, true );
}

/*================================================================*/
bool KWDocument::canResize( KWFrameSet *frameset, KWFrame *frame, int page, int diff )
{
    if ( diff < 0 ) return FALSE;

    if ( !frameset->getGroupManager() ) {
        // a normal frame _must_ not leave the page
        if ( frameset->getFrameInfo() == FI_BODY ) {
            if ( static_cast<int>( frame->bottom() + diff ) < static_cast<int>( ( page + 1 ) * ptPaperHeight() ) )
                return TRUE;
            return FALSE;
        } else { // headers and footers may always resize - ok this may lead to problems in strange situations, but let's ignore them :- )

            // a footer has to moved a bit to the top before he gets resized
            if ( isAFooter( frameset->getFrameInfo() ) )
                frame->moveTopLeft( QPoint( 0, frame->y() - diff ) );
            return TRUE;
        }
    } else // tables may _always_ resize, because the group managers can add pages if needed
        return TRUE;

    return FALSE;
}

/*================================================================*/
RunAround KWDocument::getRunAround()
{
    KWFrame *frame = getFirstSelectedFrame();

    if ( frame ) return frame->getRunAround();

    return RA_NO;
}

/*================================================================*/
KWUnit KWDocument::getRunAroundGap()
{
    KWFrame *frame = getFirstSelectedFrame();

    if ( frame ) return frame->getRunAroundGap();

    return FALSE;
}

/*================================================================*/
void KWDocument::setRunAround( RunAround _ra )
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( getFrameSet( i )->hasSelectedFrame() ) {
            for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ ) {
                if ( getFrameSet( i )->getFrame( j )->isSelected() )
                    getFrameSet( i )->getFrame( j )->setRunAround( _ra );
            }
        }
    }
}

/*================================================================*/
void KWDocument::setRunAroundGap( KWUnit _gap )
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( getFrameSet( i )->hasSelectedFrame() ) {
            for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ ) {
                if ( getFrameSet( i )->getFrame( j )->isSelected() )
                    getFrameSet( i )->getFrame( j )->setRunAroundGap( _gap );
            }
        }
    }
}
/*================================================================*/
void KWDocument::getFrameMargins( KWUnit &l, KWUnit &r, KWUnit &t, KWUnit &b )
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
KWFrameSet *KWDocument::getFrameCoords( unsigned int &x, unsigned int &y,
                                           unsigned int &w, unsigned int &h, unsigned int &num )
{
    x = y = w = h = 0;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( getFrameSet( i )->hasSelectedFrame() ) {
            for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ ) {
                if ( getFrameSet( i )->getFrame( j )->isSelected() ) {
                    x = getFrameSet( i )->getFrame( j )->x();
                    y = getFrameSet( i )->getFrame( j )->y();
                    w = getFrameSet( i )->getFrame( j )->width();
                    h = getFrameSet( i )->getFrame( j )->height();
                    num = j;
                    return getFrameSet( i );
                }
            }
        }
    }
    return 0L;
}

/*================================================================*/
void KWDocument::setFrameMargins( KWUnit l, KWUnit r, KWUnit t, KWUnit b )
{
    QList<KWGroupManager> grpMgrs;
    grpMgrs.setAutoDelete( FALSE );

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( getFrameSet( i )->hasSelectedFrame() ) {
            KWFrameSet *frameset = getFrameSet( i );
            for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ ) {
                if ( getFrameSet( i )->getFrame( j )->isSelected() ) {
                    getFrameSet( i )->getFrame( j )->setBLeft( l );
                    getFrameSet( i )->getFrame( j )->setBRight( r );
                    getFrameSet( i )->getFrame( j )->setBTop( t );
                    getFrameSet( i )->getFrame( j )->setBBottom( b );
                }
                if ( frameset->getGroupManager() && grpMgrs.findRef( frameset->getGroupManager() ) == -1 )
                    grpMgrs.append( frameset->getGroupManager() );
            }
        }
    }

    updateTableHeaders( grpMgrs );
    setModified(TRUE);
}

/*================================================================*/
void KWDocument::setFrameCoords( unsigned int x, unsigned int y, unsigned int w, unsigned int h )
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( getFrameSet( i )->hasSelectedFrame() ) {
            for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ ) {
                if ( getFrameSet( i )->getFrame( j )->isSelected() && x + w < ptPaperWidth() &&
                     y + h < m_pages * ptPaperHeight() ) {
                    if ( !getFrameSet( i )->getGroupManager() )
                        getFrameSet( i )->getFrame( j )->setRect( x, y, w, h );
                }
            }
        }
    }
//    updateAllSelections();
    setModified(TRUE);
}

/*================================================================*/
void KWDocument::updateTableHeaders( QList<KWGroupManager> &grpMgrs )
{
    KWGroupManager *grpMgr;

    for ( grpMgr = grpMgrs.first(); grpMgr != 0; grpMgr = grpMgrs.next() )
        grpMgr->updateTempHeaders();
}

/*================================================================*/
long int KWDocument::getPageNum( int bottom )
{ // this can be done better (TZ)
    int num = 0;
    while ( TRUE ) {
        if ( bottom < ( num + 1 ) * static_cast<int>( ptPaperHeight() ) )
            return num;

        num++;
    }

    return num;
}

/*================================================================*/
#if 0
void KWDocument::addImageRequest( const QString &filename, KWCharImage *img )
{
    imageRequests.insert( filename, img );
}
#endif
/*================================================================*/
void KWDocument::addImageRequest( const QString &filename, KWPictureFrameSet *fs )
{
    imageRequests2.insert( filename, fs );
}

#if 0
/*================================================================*/
void KWDocument::registerVariable( KWVariable *var )
{
    if ( !var )
        return;

    variables.append( var );
    if ( var->getType() == VT_CUSTOM ) {
        if ( !varValues.contains( ( (KWCustomVariable*)var )->getName() ) )
            varValues[ ( (KWCustomVariable*)var )->getName() ] = i18n( "No value" );
    }
}

/*================================================================*/
void KWDocument::unregisterVariable( KWVariable *var )
{
    variables.take( variables.findRef( var ) );
}

/*================================================================*/
void KWDocument::setVariableValue( const QString &name, const QString &value )
{
    varValues[ name ] = value;
}

/*================================================================*/
QString KWDocument::getVariableValue( const QString &name ) const
{
    return varValues[ name ];
}

/*================================================================*/
KWSerialLetterDataBase *KWDocument::getSerialLetterDataBase() const
{
    return slDataBase;
}

/*================================================================*/
int KWDocument::getSerialLetterRecord() const
{
    return slRecordNum;
}

/*================================================================*/
void KWDocument::setSerialLetterRecord( int r )
{
    slRecordNum = r;
}
#endif

/*================================================================*/
void KWDocument::createContents()
{
    contents->createContents();
    updateAllViews( 0, TRUE );
}

void KWDocument::getPageLayout( KoPageLayout& _layout, KoColumns& _cl, KoKWHeaderFooter& _hf )
{
    _layout = m_pageLayout;
    _cl = m_pageColumns;
    _hf = m_pageHeaderFooter;
}

#if 0
// somewhat of an ugly hack. KWFrame should have unzoomed and zoomed values instead.
void KWDocument::updateFrameSizes( double factorX, double factorY )
{
    kdDebug() << "KWDocument::updateFrameSizes " << factorX << "," << factorY << endl;
    KWFrameSet *fs = frames.first();
    fs = frames.next();
    KWFrame *frm = 0;
    for ( ; fs; fs = frames.next() ) {
        if ( fs->getFrameInfo() != FI_BODY )
            continue;
        for ( unsigned int i = 0; i < fs->getNumFrames(); ++i ) {
            frm = fs->getFrame( i );
            double x = factorX * (double)frm->x();
            double y = factorY * (double)frm->y();
            double w = factorX * (double)frm->width();
            double h = factorY * (double)frm->height();
            frm->setRect( x, y, w, h );
        }
    }
}
#endif

void KWDocument::delGroupManager( KWGroupManager *g, bool deleteit )
{
  if (deleteit)
    grpMgrs.remove( g );
  else
    grpMgrs.take(grpMgrs.find(g));
}

void KWDocument::addFrameSet( KWFrameSet *f )
{
  frames.append(f);
  //updateAllFrames();
  setModified( true );
  //updateAllViews(0L);
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
int KWDocument::getPageOfRect( QRect & _rect ) const
{
    int page = static_cast<int>(_rect.y() / ptPaperHeight());
    return QMIN( page, m_pages-1 );
}

// Return true if @p r is out of the page @p page
bool KWDocument::isOutOfPage( QRect & r, int page ) const
{
    return r.x() < 0 ||
        r.right() > static_cast<int>( ptPaperWidth() ) ||
        r.y() < page * static_cast<int>( ptPaperHeight() ) ||
        r.bottom() > ( page + 1 ) * static_cast<int>( ptPaperHeight() );
}

bool KWDocument::selection() {
    for (unsigned int i=0; i < getNumFrameSets(); i++) {
        KWFrameSet *frameSet = getFrameSet( i );

        if ( frameSet->getFrameType() == FT_TEXT &&
             static_cast<KWTextFrameSet *>( frameSet )->textDocument()->hasSelection(QTextDocument::Standard)) return true;
    }
    return false;
}

void KWDocument::addCommand( KCommand * cmd )
{
    history.addCommand( cmd, false );
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

void KWDocument::printDebug() {

    static const char * typeFrameset[] = { "base", "txt", "pic", "part", "formula", "ERROR" };
    static const char * infoFrameset[] = { "body", "first header", "odd headers", "even headers",
                                           "first footer", "odd footers", "even footers", "footnote", "ERROR" };
    static const char * frameBh[] = { "AutoExtendFrame", "AutoCreateNewFrame", "Ignore", "ERROR" };
    static const char * newFrameBh[] = { "Reconnect", "NoFollowup", "Copy" };

    KWDocument * doc = this; // this code was in another class :)
    kdDebug() << "----------------------------------------"<<endl;
    kdDebug() << "                 Debug info"<<endl;
    kdDebug() << "Document:" << doc <<endl;
    kdDebug() << "Type of document: (0=WP, 1=DTP) " << doc->processingType() <<endl;
    kdDebug() << "size: x:" << doc->ptLeftBorder()<< ", y:"<<doc->ptTopBorder() << ", w:"<< doc->ptPaperWidth() << ", h:"<<doc->ptPaperHeight()<<endl;
    kdDebug() << "Header visible: " << doc->isHeaderVisible() << endl;
    kdDebug() << "Footer visible: " << doc->isFooterVisible() << endl;
    kdDebug() << "Units: " << doc->getUnit() <<endl;
    kdDebug() << "# Framesets: " << doc->getNumFrameSets() <<endl;
    for (unsigned int iFrameset = 0; iFrameset < doc->getNumFrameSets(); iFrameset++ ) {
        KWFrameSet * frameset = doc->getFrameSet(iFrameset);
        kdDebug() << "Frameset " << iFrameset << ": '" <<
            frameset->getName() << "' (" << frameset << ")" <<endl;
        kdDebug() << " |  Visible: " << frameset->isVisible() << endl;
        kdDebug() << " |  Type: " << typeFrameset[ frameset->getFrameType() ] << endl;
        kdDebug() << " |  Info: " << infoFrameset[ frameset->getFrameInfo() ] << endl;
        if(frameset->getGroupManager()) {
            kdDebug() << " |  Groupmanager: " << frameset->getGroupManager() << endl;
            KWGroupManager::Cell *cell = frameset->getGroupManager()->getCell(frameset);
            kdDebug() << " |  |- row :" << cell->row << endl;
            kdDebug() << " |  |- col :" << cell->col << endl;
            kdDebug() << " |  |- rows:" << cell->rows << endl;
            kdDebug() << " |  +- cols:" << cell->cols << endl;
        }
        QListIterator<KWFrame> frameIt = frameset->frameIterator();
        for ( unsigned int j = 0; frameIt.current(); ++frameIt, ++j ) {
            KWFrame * frame = frameIt.current();
            kdDebug() << " +-- Frame " << j << " of "<< frameset->getNumFrames() << "    (" << frame << ")" << endl;
            kdDebug() << "     Rectangle : " << frame->x() << "," << frame->y() << " " << frame->width() << "x" << frame->height() << endl;
            kdDebug() << "     FrameBehaviour: "<< frameBh[ frame->getFrameBehaviour() ] << endl;
            kdDebug() << "     NewFrameBehaviour: "<< newFrameBh[ frame->getNewFrameBehaviour() ] << endl;
            kdDebug() << "     SheetSide "<< frame->getSheetSide() << endl;
            if(frame->isSelected())
                kdDebug() << " *   Page "<< frame->getPageNum() << endl;
            else
                kdDebug() << "     Page "<< frame->getPageNum() << endl;
        }
    }
    /*
    kdDebug() << "# Images: " << doc->getImageCollection()->iterator().count() <<endl;
    QDictIterator<KWImage> it( doc->getImageCollection()->iterator() );
    while ( it.current() ) {
        kdDebug() << " + " << it.current()->getFilename() << ": "<<it.current()->refCount() <<endl;
        ++it;
    }
    */
}

void KWDocument::layout()
{
    QListIterator<KWFrameSet> it = framesetsIterator();
    for (; it.current(); ++it )
        it.current()->layout();
}

#include "kwdoc.moc"
