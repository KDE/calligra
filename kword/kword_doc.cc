/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998	  */
/* Version: 0.0.1						  */
/* Author: Reginald Stadlbauer, Torben Weis			  */
/* E-Mail: reggie@kde.org, weis@kde.org				  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Document (header)					  */
/******************************************************************/

#include <qmsgbox.h>
#include <qcolor.h>
#include <qprinter.h>
#include <qevent.h>
#include <qregion.h>
#include <qclipboard.h>
#include <qmessagebox.h>
#include <qdict.h>

#include "kword_doc.h"
#include "kword_page.h"
#include "kword_shell.h"
#include "defs.h"
#include "font.h"
#include "kword_doc.moc"
#include "clipbrd_dnd.h"
#include "image.h"
#include "char.h"
#include "kword_utils.h"

#include <kurl.h>
#include <klocale.h>
#include <kglobal.h>

#include <unistd.h>
#include <math.h>

#include <koTemplateChooseDia.h>
#include <kstddirs.h>

/******************************************************************/
/* Class: KWordChild						  */
/******************************************************************/

/*================================================================*/
KWordChild::KWordChild( KWordDocument *_wdoc, const QRect& _rect, KOffice::Document_ptr _doc, int diffx, int diffy )
    : KoDocumentChild( _rect, _doc )
{
    m_pKWordDoc = _wdoc;
    m_rDoc = KOffice::Document::_duplicate( _doc );
    setGeometry( QRect( _rect.left() + diffx, _rect.top() + diffy, _rect.width(), _rect.height() ) );
}

/*================================================================*/
KWordChild::KWordChild( KWordDocument *_wdoc )
    : KoDocumentChild()
{

    m_pKWordDoc = _wdoc;
}

/*================================================================*/
KWordChild::~KWordChild()
{
}


/******************************************************************/
/* Class: KWordDocument					     */
/******************************************************************/

/*================================================================*/
KWordDocument::KWordDocument()
    : formatCollection( this ), imageCollection( this ), selStart( this, 1 ), selEnd( this, 1 ),
      ret_pix( ICON( "return.xpm" ) ), unit( "mm" ), numParags( 0 ), footNoteManager( this ),
      autoFormat( this ), urlIntern(), pglChanged( true )
{
    ADD_INTERFACE( "IDL:KOffice/Print:1.0" );

    // Use CORBA mechanism for deleting views
    m_lstViews.setAutoDelete( false );
    m_lstChildren.setAutoDelete( true );

    m_bModified = false;
    hasSelection = false;

    rastX = rastY = 10;

    m_bEmpty = true;
    applyStyleTemplate = 0;
    applyStyleTemplate = applyStyleTemplate | U_FONT_FAMILY_ALL_SIZE | U_COLOR | U_BORDER | U_INDENT | U_NUMBERING | U_ALIGN | U_TABS;
    _loaded = false;
    _header = false;
    _footer = false;
    _needRedraw = false;

    cUserFont = 0L;
    cParagLayout = 0L;
    cDisplayFont = 0L;

    paragLayoutList.setAutoDelete( false );
    userFontList.setAutoDelete( false );
    displayFontList.setAutoDelete( false );
    frames.setAutoDelete( true );
    grpMgrs.setAutoDelete( true );

    QObject::connect( &history, SIGNAL( undoRedoChanged( QString, QString ) ), this,
		      SLOT( slotUndoRedoChanged( QString, QString ) ) );
}

/*================================================================*/
CORBA::Boolean KWordDocument::initDoc()
{
    pageLayout.unit = PG_MM;
    pages = 1;

    pageColumns.columns = 1; //STANDARD_COLUMNS;
    pageColumns.ptColumnSpacing = STANDARD_COLUMN_SPACING;
    pageColumns.mmColumnSpacing = POINT_TO_MM( STANDARD_COLUMN_SPACING );
    pageColumns.inchColumnSpacing = POINT_TO_INCH( STANDARD_COLUMN_SPACING );

    pageHeaderFooter.header = HF_SAME;
    pageHeaderFooter.footer = HF_SAME;
    pageHeaderFooter.ptHeaderBodySpacing = 10;
    pageHeaderFooter.ptFooterBodySpacing = 10;
    pageHeaderFooter.inchHeaderBodySpacing = POINT_TO_INCH( 10 );
    pageHeaderFooter.inchFooterBodySpacing = POINT_TO_INCH( 10 );
    pageHeaderFooter.inchHeaderBodySpacing = POINT_TO_MM( 10 );
    pageHeaderFooter.inchFooterBodySpacing = POINT_TO_MM( 10 );

    QString _template;

    KoTemplateChooseDia::ReturnType ret = KoTemplateChooseDia::chooseTemplate( "kword_template", _template, true, false );
    if ( ret == KoTemplateChooseDia::Template ) {
	QFileInfo fileInfo( _template );
	QString fileName( fileInfo.dirPath( true ) + "/" + fileInfo.baseName() + ".kwt" );
	bool ok = loadTemplate( fileName.data() );
	setURL( QString::null );
	return ok;
    } else if ( ret == KoTemplateChooseDia::File ) {
	QString fileName( _template );
	bool ok = loadTemplate( fileName.data() );
	setURL( fileName );
	_loaded = true;
	return ok;
    } else if ( ret == KoTemplateChooseDia::Empty ) {
	QString fileName( locate("kword_template", "Wordprocessing/PlainText.kwt") );
	bool ok = loadTemplate( fileName.data() );
	setURL( QString::null );
	return ok;
    }
    else
	return false;

    return false;
}

/*================================================================*/
bool KWordDocument::loadTemplate( const char *_url )
{
//     KURL u( _url );

//     if ( u.isMalformed() )
//	   return false;

//     if ( !u.isLocalFile() )
//     {
//	   cerr << "Can not open remote URL" << endl;
//	   return false;
//     }

//     ifstream in( u.path().ascii() );
//     if ( !in )
//     {
//	   cerr << "Could not open" << u.path().ascii() << endl;
//	   return false;
//     }

//     KOMLStreamFeed feed( in );
//     KOMLParser parser( &feed );

//     if ( !loadXML( parser, 0L ) )
//	   return false;

//     setModified( true );

//     _loaded = false;
//     return true;
    bool ok = loadFromURL( _url, "application/x-kword" );
    _loaded = false;
    return ok;
}

/*================================================================*/
void KWordDocument::setPageLayout( KoPageLayout _layout, KoColumns _cl, KoKWHeaderFooter _hf )
{
    pglChanged = true;
    if ( processingType == WP ) {
	pageLayout = _layout;
	pageColumns = _cl;
	pageHeaderFooter = _hf;
    } else {
	pageLayout = _layout;
	pageLayout.left = 0;
	pageLayout.right = 0;
	pageLayout.top = 0;
	pageLayout.bottom = 0;
	pageLayout.ptLeft = 0;
	pageLayout.ptRight = 0;
	pageLayout.ptTop = 0;
	pageLayout.ptBottom = 0;
	pageLayout.mmLeft = 0;
	pageLayout.mmRight = 0;
	pageLayout.mmTop = 0;
	pageLayout.mmBottom = 0;
	pageLayout.inchLeft = 0;
	pageLayout.inchRight = 0;
	pageLayout.inchTop = 0;
	pageLayout.inchBottom = 0;
	pageHeaderFooter = _hf;
    }

    recalcFrames();

    updateAllFrames();
    updateAllCursors();
}

/*================================================================*/
void KWordDocument::recalcFrames( bool _cursor, bool _fast )
{
    if ( processingType != DTP )
	pages = 1;

    KWTextFrameSet *frameset = dynamic_cast<KWTextFrameSet*>( frames.at( 0 ) );

    unsigned int frms = frameset->getNumFrames();

    ptColumnWidth = ( getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder() - getPTColumnSpacing() * ( pageColumns.columns - 1 ) )
		    / pageColumns.columns;

    int firstHeadOffset = 0, evenHeadOffset = 0, oddHeadOffset = 0;
    int firstFootOffset = 0, evenFootOffset = 0, oddFootOffset = 0;
    KWTextFrameSet *firstHeader = 0L, *evenHeader = 0L, *oddHeader = 0L;
    KWTextFrameSet *firstFooter = 0L, *evenFooter = 0L, *oddFooter = 0L;
    if ( hasHeader() || hasFooter() ) {
	for ( unsigned int k = 0; k < getNumFrameSets(); k++ ) {
	    if ( getFrameSet( k )->getFrameInfo() == FI_FIRST_HEADER && hasHeader() ) {
		firstHeader = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
		firstHeadOffset = pageHeaderFooter.ptHeaderBodySpacing + getFrameSet( k )->getFrame( 0 )->height();
	    }
	    if ( getFrameSet( k )->getFrameInfo() == FI_EVEN_HEADER && hasHeader() ) {
		evenHeader = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
		evenHeadOffset = pageHeaderFooter.ptHeaderBodySpacing + getFrameSet( k )->getFrame( 0 )->height();
	    }
	    if ( getFrameSet( k )->getFrameInfo() == FI_ODD_HEADER && hasHeader() ) {
		oddHeader = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
		oddHeadOffset = pageHeaderFooter.ptHeaderBodySpacing + getFrameSet( k )->getFrame( 0 )->height();
	    }

	    if ( getFrameSet( k )->getFrameInfo() == FI_FIRST_FOOTER && hasFooter() ) {
		firstFooter = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
		firstFootOffset = pageHeaderFooter.ptFooterBodySpacing + getFrameSet( k )->getFrame( 0 )->height();
	    }
	    if ( getFrameSet( k )->getFrameInfo() == FI_EVEN_FOOTER && hasFooter() ) {
		evenFooter = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
		evenFootOffset = pageHeaderFooter.ptFooterBodySpacing + getFrameSet( k )->getFrame( 0 )->height();
	    }
	    if ( getFrameSet( k )->getFrameInfo() == FI_ODD_FOOTER && hasFooter() ) {
		oddFooter = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
		oddFootOffset = pageHeaderFooter.ptFooterBodySpacing + getFrameSet( k )->getFrame( 0 )->height();
	    }
	}
	if ( hasHeader() ) {
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
	if ( hasFooter() ) {
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

    if ( processingType == WP ) {
	int headOffset = 0, footOffset = 0;

	for ( unsigned int j = 0; j < static_cast<unsigned int>( ceil( static_cast<double>( frms ) /
								       static_cast<double>( pageColumns.columns ) ) ); j++ ) {
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

	    for ( int i = 0; i < pageColumns.columns; i++ ) {
		if ( j * pageColumns.columns + i < frameset->getNumFrames() ) {
		    frameset->getFrame( j * pageColumns.columns + i )->setRect( getPTLeftBorder() + i * ( ptColumnWidth + getPTColumnSpacing() ),
										j * getPTPaperHeight() + getPTTopBorder() + headOffset,
										ptColumnWidth,
										getPTPaperHeight() - getPTTopBorder() - getPTBottomBorder() -
										headOffset - footOffset );
		} else {
		    frameset->addFrame( new KWFrame( getPTLeftBorder() + i * ( ptColumnWidth + getPTColumnSpacing() ),
						     j * getPTPaperHeight() + getPTTopBorder() + headOffset,
						     ptColumnWidth, getPTPaperHeight() - getPTTopBorder() - getPTBottomBorder() -
						     headOffset - footOffset ) );
		}
	    }
	}

	pages = static_cast<int>( ceil( static_cast<double>( frms ) / static_cast<double>( pageColumns.columns ) ) );
    }

    if ( hasHeader() ) {
	switch ( getHeaderType() ) {
	case HF_SAME: {
	    int h = evenHeader->getFrame( 0 )->height();
	    for ( int l = 0; l < pages; l++ )
	    {
		if ( l < static_cast<int>( evenHeader->getNumFrames() ) )
		    evenHeader->getFrame( l )->setRect( getPTLeftBorder(), l * getPTPaperHeight() + getPTTopBorder(),
							getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
		else
		{
		    KWFrame *frame = new KWFrame( getPTLeftBorder(), l * getPTPaperHeight() + getPTTopBorder(),
						  getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
		    evenHeader->addFrame( frame );
		}
	    }
	    if ( pages < static_cast<int>( evenHeader->getNumFrames() ) )
	    {
		int diff = evenHeader->getNumFrames() - pages;
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
	    for ( int l = 0; l < pages; l++ ) {
		if ( ( ( l + 1 ) / 2 ) * 2 != l + 1 ) {
		    odd++;
		    if ( static_cast<int>( oddHeader->getCurrent() ) < static_cast<int>( oddHeader->getNumFrames() ) ) {
			oddHeader->getFrame( oddHeader->getCurrent() )->setRect( getPTLeftBorder(), l * getPTPaperHeight() + getPTTopBorder(),
										 getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h2 );
			oddHeader->setCurrent( oddHeader->getCurrent() + 1 );
		    } else {
			KWFrame *frame = new KWFrame( getPTLeftBorder(), l * getPTPaperHeight() + getPTTopBorder(),
						      getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h2 );
			oddHeader->addFrame( frame );
		    }
		} else {
		    even++;
		    if ( static_cast<int>( evenHeader->getCurrent() ) < static_cast<int>( evenHeader->getNumFrames() ) ) {
			evenHeader->getFrame( evenHeader->getCurrent() )->setRect( getPTLeftBorder(), l * getPTPaperHeight() + getPTTopBorder(),
										   getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h1 );
			evenHeader->setCurrent( evenHeader->getCurrent() + 1 );
		    } else {
			KWFrame *frame = new KWFrame( getPTLeftBorder(), l * getPTPaperHeight() + getPTTopBorder(),
						      getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h1 );
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
	    if ( pages == 1 && evenHeader->getNumFrames() > 0 ) {
		for ( unsigned int m = 0; m < evenHeader->getNumFrames(); m++ )
		    evenHeader->getFrame( m )->setRect( 0, pages * getPTPaperHeight() + h1,
							getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h1 );
	    }
	} break;
	case HF_FIRST_DIFF: {
	    int h = firstHeader->getFrame( 0 )->height();
	    firstHeader->getFrame( 0 )->setRect( getPTLeftBorder(), getPTTopBorder(),
						 getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
	    if ( firstHeader->getNumFrames() > 1 ) {
		int diff = firstHeader->getNumFrames() - 1;
		for ( ; diff > 0; diff-- )
		    firstHeader->delFrame( firstHeader->getNumFrames() - 1 );
	    }
	    h = evenHeader->getFrame( 0 )->height();
	    for ( int l = 1; l < pages; l++ ) {
		if ( l - 1 < static_cast<int>( evenHeader->getNumFrames() ) )
		    evenHeader->getFrame( l - 1 )->setRect( getPTLeftBorder(), l * getPTPaperHeight() + getPTTopBorder(),
							    getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
		else {
		    KWFrame *frame = new KWFrame( getPTLeftBorder(), l * getPTPaperHeight() + getPTTopBorder(),
						  getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
		    evenHeader->addFrame( frame );
		}
	    }
	    if ( pages < static_cast<int>( evenHeader->getNumFrames() ) ) {
		int diff = evenHeader->getNumFrames() - pages;
		for ( ; diff > 0; diff-- )
		    evenHeader->delFrame( evenHeader->getNumFrames() - 1 );
	    }
	    if ( pages == 1 && evenHeader->getNumFrames() > 0 ) {
		for ( unsigned int m = 0; m < evenHeader->getNumFrames(); m++ )
		    evenHeader->getFrame( m )->setRect( 0, pages * getPTPaperHeight() + h,
							getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
	    }
	} break;
	}
    }

    if ( hasFooter() ) {
	switch ( getFooterType() )
	{
	case HF_SAME: {
	    int h = evenFooter->getFrame( 0 )->height();
	    for ( int l = 0; l < pages; l++ ) {
		if ( l < static_cast<int>( evenFooter->getNumFrames() ) )
		    evenFooter->getFrame( l )->setRect( getPTLeftBorder(), ( l + 1 ) * getPTPaperHeight() - getPTBottomBorder() - h,
							getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
		else {
		    KWFrame *frame = new KWFrame( getPTLeftBorder(), ( l + 1 ) * getPTPaperHeight() - getPTBottomBorder() - h,
						  getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
		    evenFooter->addFrame( frame );
		}
	    }
	    if ( pages < static_cast<int>( evenFooter->getNumFrames() ) ) {
		int diff = evenFooter->getNumFrames() - pages;
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
	    for ( int l = 0; l < pages; l++ )
	    {
		if ( ( ( l + 1 ) / 2 ) * 2 != l + 1 )
		{
		    odd++;
		    if ( static_cast<int>( oddFooter->getCurrent() ) < static_cast<int>( oddFooter->getNumFrames() ) ) {
			oddFooter->getFrame( oddFooter->getCurrent() )->setRect( getPTLeftBorder(),
										 ( l + 1 )  * getPTPaperHeight() - getPTBottomBorder() - h2,
										 getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h2 );
			oddFooter->setCurrent( oddFooter->getCurrent() + 1 );
		    } else {
			KWFrame *frame = new KWFrame( getPTLeftBorder(),
						      ( l + 1 )	 * getPTPaperHeight() - getPTBottomBorder() - h2,
						      getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h2 );
			oddFooter->addFrame( frame );
		    }
		} else {
		    even++;
		    if ( static_cast<int>( evenFooter->getCurrent() ) < static_cast<int>( evenFooter->getNumFrames() ) ) {
			evenFooter->getFrame( evenFooter->getCurrent() )->setRect( getPTLeftBorder(),
										   ( l + 1 )  * getPTPaperHeight() - getPTBottomBorder() - h1,
										   getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h1 );
			evenFooter->setCurrent( evenFooter->getCurrent() + 1 );
		    } else {
			KWFrame *frame = new KWFrame( getPTLeftBorder(),
						      ( l + 1 )	 * getPTPaperHeight() - getPTBottomBorder() - h1,
						      getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h1 );
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
	    if ( pages == 1 && evenFooter->getNumFrames() > 0 ) {
		for ( unsigned int m = 0; m < evenFooter->getNumFrames(); m++ )
		    evenFooter->getFrame( m )->setRect( 0, pages * getPTPaperHeight() + h1,
							getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h1 );
	    }
	} break;
	case HF_FIRST_DIFF: {
	    int h = firstFooter->getFrame( 0 )->height();
	    firstFooter->getFrame( 0 )->setRect( getPTLeftBorder(), getPTPaperHeight() - getPTBottomBorder() - h,
						 getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
	    if ( firstFooter->getNumFrames() > 1 ) {
		int diff = firstFooter->getNumFrames() - 1;
		for ( ; diff > 0; diff-- )
		    firstFooter->delFrame( firstFooter->getNumFrames() - 1 );
	    }
	    h = evenFooter->getFrame( 0 )->height();
	    for ( int l = 1; l < pages; l++ ) {
		if ( l - 1 < static_cast<int>( evenFooter->getNumFrames() ) )
		    evenFooter->getFrame( l - 1 )->setRect( getPTLeftBorder(), ( l + 1 ) * getPTPaperHeight() - getPTBottomBorder() - h,
							    getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
		else {
		    KWFrame *frame = new KWFrame( getPTLeftBorder(), ( l + 1 ) * getPTPaperHeight() - getPTBottomBorder() - h,
						  getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
		    evenFooter->addFrame( frame );
		}
	    }
	    if ( pages < static_cast<int>( evenFooter->getNumFrames() ) ) {
		int diff = evenFooter->getNumFrames() - pages;
		for ( ; diff > 0; diff-- )
		    evenFooter->delFrame( evenFooter->getNumFrames() - 1 );
	    }
	    if ( pages == 1 && evenFooter->getNumFrames() > 0 ) {
		for ( unsigned int m = 0; m < evenFooter->getNumFrames(); m++ )
		    evenFooter->getFrame( m )->setRect( 0, pages * getPTPaperHeight() + h,
							getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), h );
	    }
	} break;
	}
    }

    updateAllViewportSizes();
    recalcWholeText( _cursor, _fast );
}

/*================================================================*/
KWordDocument::~KWordDocument()
{
    cerr << "KWordDocument::~KWordDocument()" << endl;
    cleanUp();
    cerr << "...KWordDocument::~KWordDocument()" << endl;
}

/*================================================================*/
void KWordDocument::cleanUp()
{
    if ( m_bIsClean ) return;

    assert( m_lstViews.count() == 0 );

    m_lstChildren.clear();

    KoDocument::cleanUp();
}

/*================================================================*/
bool KWordDocument::hasToWriteMultipart()
{
    //if ( m_lstChildren.count() == 0 ) return false;

    return true;
}

/*================================================================*/
bool KWordDocument::loadChildren( KOStore::Store_ptr _store )
{
    cerr << "bool KWordDocument::loadChildren( OPParts::MimeMultipartDict_ptr _dict )" << endl;

    QListIterator<KWordChild> it( m_lstChildren );
    for( ; it.current(); ++it ) {
	cerr << "Loading child" << endl;
	if ( !it.current()->loadDocument( _store, it.current()->mimeType() ) )
	    return false;
    }

    cerr << "Loading done" << endl;

    return true;
}

/*================================================================*/
bool KWordDocument::loadXML( const QDOM::Document& doc, KOStore::Store_ptr )
{
    _loaded = true;
    pixmapKeys.clear();
    imageRequests.clear();
    imageRequests2.clear();

    pageLayout.unit = PG_MM;

    pageColumns.columns = 1; //STANDARD_COLUMNS;
    pageColumns.ptColumnSpacing = STANDARD_COLUMN_SPACING;
    pageColumns.mmColumnSpacing = POINT_TO_MM( STANDARD_COLUMN_SPACING );
    pageColumns.inchColumnSpacing = POINT_TO_INCH( STANDARD_COLUMN_SPACING );

    pageHeaderFooter.header = HF_SAME;
    pageHeaderFooter.footer = HF_SAME;
    pageHeaderFooter.ptHeaderBodySpacing = 10;
    pageHeaderFooter.ptFooterBodySpacing = 10;
    pageHeaderFooter.inchHeaderBodySpacing = POINT_TO_INCH( 10 );
    pageHeaderFooter.inchFooterBodySpacing = POINT_TO_INCH( 10 );
    pageHeaderFooter.inchHeaderBodySpacing = POINT_TO_MM( 10 );
    pageHeaderFooter.inchFooterBodySpacing = POINT_TO_MM( 10 );

    defaultUserFont = findUserFont( "times" );
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
    lay->setStartCounter( "1" );
    lay->setCounterRightText( "." );
    lay->setNumberingType( KWParagLayout::NT_CHAPTER );
    lay->setFormat( f );

    f.setPTFontSize( 16 );
    lay = new KWParagLayout( this );
    lay->setName( "Head 2" );
    lay->setFollowingParagLayout( "Standard" );
    lay->setCounterType( KWParagLayout::CT_NUM );
    lay->setCounterDepth( 1 );
    lay->setStartCounter( "1" );
    lay->setCounterRightText( "." );
    lay->setNumberingType( KWParagLayout::NT_CHAPTER );
    lay->setFormat( f );

    f.setPTFontSize( 12 );
    lay = new KWParagLayout( this );
    lay->setName( "Head 3" );
    lay->setFollowingParagLayout( "Standard" );
    lay->setCounterType( KWParagLayout::CT_NUM );
    lay->setCounterDepth( 2 );
    lay->setStartCounter( "1" );
    lay->setCounterRightText( "." );
    lay->setNumberingType( KWParagLayout::NT_CHAPTER );
    lay->setFormat( f );

    lay = new KWParagLayout( this );
    lay->setName( "Enumerated List" );
    lay->setFollowingParagLayout( "Enumerated List" );
    lay->setCounterType( KWParagLayout::CT_NUM );
    lay->setCounterDepth( 0 );
    lay->setStartCounter( "1" );
    lay->setCounterRightText( "." );
    lay->setNumberingType( KWParagLayout::NT_LIST );

    lay = new KWParagLayout( this );
    lay->setName( "Alphabetical List" );
    lay->setFollowingParagLayout( "Alphabetical List" );
    lay->setCounterType( KWParagLayout::CT_ALPHAB_L );
    lay->setCounterDepth( 0 );
    lay->setStartCounter( "a" );
    lay->setCounterRightText( " )" );
    lay->setNumberingType( KWParagLayout::NT_LIST );

    lay = new KWParagLayout( this );
    lay->setName( "Bullet List" );
    lay->setFollowingParagLayout( "Bullet List" );
    lay->setCounterType( KWParagLayout::CT_BULLET );
    lay->setCounterDepth( 0 );
    lay->setStartCounter( "1" );
    lay->setCounterRightText( "" );
    lay->setNumberingType( KWParagLayout::NT_LIST );

    if (true /*no variable formats were loaded*/)
    {
	varFormats.insert( VT_DATE_FIX, new KWVariableDateFormat() );
	varFormats.insert( VT_DATE_VAR, new KWVariableDateFormat() );
	varFormats.insert( VT_TIME_FIX, new KWVariableTimeFormat() );
	varFormats.insert( VT_TIME_VAR, new KWVariableTimeFormat() );
	varFormats.insert( VT_PGNUM, new KWVariablePgNumFormat() );
	// ... and so on ...
    }

    pages = 1;

    KoPageLayout __pgLayout;
    __pgLayout.unit = PG_MM;
    KoColumns __columns;
    KoKWHeaderFooter __hf;
    __hf.header = HF_SAME;
    __hf.footer = HF_SAME;
    __hf.ptHeaderBodySpacing = 10;
    __hf.ptFooterBodySpacing = 10;
    __hf.mmHeaderBodySpacing = POINT_TO_MM( 10 );
    __hf.mmFooterBodySpacing = POINT_TO_MM( 10 );
    __hf.inchHeaderBodySpacing = POINT_TO_INCH( 10 );
    __hf.inchFooterBodySpacing = POINT_TO_INCH( 10 );

    // <DOC>
    if ( doc.doctype().name() != "DOC" )
	return false;

    QDOM::Element word = doc.documentElement();

    if ( word.attribute( "mime" ) != "application/x-kword" )
	return false;

    QDOM::Element paper = doc.namedItem("PAPER").toElement();
    if ( paper.isNull() )
	return false;
    __pgLayout.format = (KoFormat)paper.attribute( "format" ).toInt();
    __pgLayout.orientation = (KoOrientation)paper.attribute( "orientation" ).toInt();
    __pgLayout.mmWidth = paper.attribute( "mmWidth" ).toInt();
    __pgLayout.inchWidth = paper.attribute( "inchWidth" ).toInt();
    __pgLayout.ptWidth = paper.attribute( "ptWidth" ).toInt();
    __pgLayout.mmHeight = paper.attribute( "mmHeight" ).toInt();
    __pgLayout.inchHeight = paper.attribute( "inchHeight" ).toInt();
    __pgLayout.ptHeight = paper.attribute( "ptHeight" ).toInt();
    __pgLayout.columns = paper.attribute( "columns" ).toInt();
    __pgLayout.ptColumnSpacing = paper.attribute( "ptColumnspc" ).toInt();
    __pgLayout.mmColumnSpacing = paper.attribute( "mmColumnspc" ).toInt();
    __pgLayout.inchColumnSpacing = paper.attribute( "inchColumnspc" ).toInt();
    __hf.header = (KoHFType)paper.attribute( "hType" ).toInt();
    __hf.footer = (KoHFType)paper.attribute( "fType" ).toInt();
    __hf.ptHeadBody = paper.attribute( "ptHeadBody" ).toInt();
    __hf.mmHeadBody = paper.attribute( "mmHeadBody" ).toInt();
    __hf.inchHeadBody = paper.attribute( "inchHeadBody" ).toInt();
    __hf.ptFootBody = paper.attribute( "ptFootBody" ).toInt();
    __hf.mmFootBody = paper.attribute( "mmFootBody" ).toInt();
    __hf.inchFootBody = paper.attribute( "inchFootBody" ).toInt();

    QDOM::Element border = paper.namedItem( "PAPERBORDERS" ).toElement();
    if ( border.isNull() )
	return false;
    __pgLayout.ptLeft = border.attribute( "ptLeft" ).toInt();
    __pgLayout.ptRight = border.attribute( "ptRight" ).toInt();
    __pgLayout.ptTop = border.attribute( "ptTop" ).toInt();
    __pgLayout.ptBottom = border.attribute( "ptBottom" ).toInt();
    __pgLayout.mmLeft = border.attribute( "mmLeft" ).toInt();
    __pgLayout.mmRight = border.attribute( "mmRight" ).toInt();
    __pgLayout.mmTop = border.attribute( "mmTop" ).toInt();
    __pgLayout.mmBottom = border.attribute( "mmBottom" ).toInt();
    __pgLayout.inchLeft = border.attribute( "inchLeft" ).toInt();
    __pgLayout.inchRight = border.attribute( "inchRight" ).toInt();
    __pgLayout.inchTop = border.attribute( "inchTop" ).toInt();
    __pgLayout.inchBottom = border.attribute( "inchBottom" ).toInt();
    
    QDOM::Element attribs = doc.namedItem( "ATTRIBUTES" ).toElement();
    if ( attribs.isNull() )
	return false;
    processingType = attribs.attribute( "processing" ).toInt();
    _header = (bool)attribs.attribute( "hasHeader" ).toInt();
    _footer = (bool)attribs.attribute( "hasFooter" ).toInt();
    unit = attribs.attribute( "unit" );
    
    if ( !footNoteManager.load( word.namedItem( "FOOTNOTE-GLOBAL" ).toElement() ) )
	return FALSE;
    
    if ( !loadFrameSets( word.namedItem( "FRAMESETS" ).toElement() ) )
	return FALSE;
    
    if ( !loadStyleTemplates( word.namedItem( "STYLES" ).toElement() ) )
	return FALSE;

    QDOM::Element pix = word.namedItem( "PIXMAPS" ).toElement();
    QDOM::Element key = pix.firstChild();
    for ( ; !key.isNull(); key = key.nextSibling() )
	pixmapKeys.append( key.attribute( "key" ) );
    

    switch ( KWUnit::unitType( unit ) ) {
    case U_MM: __pgLayout.unit = PG_MM;
	break;
    case U_PT: __pgLayout.unit = PG_PT;
	break;
    case U_INCH: __pgLayout.unit = PG_INCH;
	break;
    }
    setPageLayout( __pgLayout, __columns, __hf );

    bool _first_footer = false, _even_footer = false, _odd_footer = false;
    bool _first_header = false, _even_header = false, _odd_header = false;
    bool _footnotes = false;

    for ( unsigned int k = 0; k < getNumFrameSets(); k++ ) {
	if ( getFrameSet( k )->getFrameInfo() == FI_FIRST_HEADER ) _first_header = true;
	if ( getFrameSet( k )->getFrameInfo() == FI_EVEN_HEADER ) _odd_header = true;
	if ( getFrameSet( k )->getFrameInfo() == FI_ODD_HEADER ) _even_header = true;
	if ( getFrameSet( k )->getFrameInfo() == FI_FIRST_FOOTER ) _first_footer = true;
	if ( getFrameSet( k )->getFrameInfo() == FI_EVEN_FOOTER ) _odd_footer = true;
	if ( getFrameSet( k )->getFrameInfo() == FI_ODD_FOOTER ) _even_footer = true;
	if ( getFrameSet( k )->getFrameInfo() == FI_FOOTNOTE ) _footnotes = true;
    }

    if ( !_first_header ) {
	KWTextFrameSet *fs = new KWTextFrameSet( this );
	fs->setFrameInfo( FI_FIRST_HEADER );
	KWFrame *frame = new KWFrame( getPTLeftBorder(), getPTTopBorder(), 
				      getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
	fs->addFrame( frame );
	frames.append( fs );
	fs->setAutoCreateNewFrame( false );
    }

    if ( !_even_header ) {
	KWTextFrameSet *fs = new KWTextFrameSet( this );
	fs->setFrameInfo( FI_EVEN_HEADER );
	KWFrame *frame = new KWFrame( getPTLeftBorder(), getPTTopBorder(), 
				      getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
	fs->addFrame( frame );
	frames.append( fs );
	fs->setAutoCreateNewFrame( false );
    }

    if ( !_odd_header ) {
	KWTextFrameSet *fs = new KWTextFrameSet( this );
	fs->setFrameInfo( FI_ODD_HEADER );
	KWFrame *frame = new KWFrame( getPTLeftBorder(), getPTTopBorder(), 
				      getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
	fs->addFrame( frame );
	frames.append( fs );
	fs->setAutoCreateNewFrame( false );
    }

    if ( !_first_footer ) {
	KWTextFrameSet *fs = new KWTextFrameSet( this );
	fs->setFrameInfo( FI_FIRST_FOOTER );
	KWFrame *frame = new KWFrame( getPTLeftBorder(), getPTPaperHeight() - getPTTopBorder() - 20,
				      getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
	fs->addFrame( frame );
	frames.append( fs );
	fs->setAutoCreateNewFrame( false );
    }

    if ( !_even_footer ) {
	KWTextFrameSet *fs = new KWTextFrameSet( this );
	fs->setFrameInfo( FI_EVEN_FOOTER );
	KWFrame *frame = new KWFrame( getPTLeftBorder(), getPTPaperHeight() - getPTTopBorder() - 20,
				      getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
	fs->addFrame( frame );
	frames.append( fs );
	fs->setAutoCreateNewFrame( false );
    }

    if ( !_odd_footer ) {
	KWTextFrameSet *fs = new KWTextFrameSet( this );
	fs->setFrameInfo( FI_ODD_FOOTER );
	KWFrame *frame = new KWFrame( getPTLeftBorder(), getPTPaperHeight() - getPTTopBorder() - 20,
				      getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
	fs->addFrame( frame );
	frames.append( fs );
	fs->setAutoCreateNewFrame( false );
    }

    if ( !_footnotes ) {
	KWTextFrameSet *fs = new KWTextFrameSet( this );
	fs->setFrameInfo( FI_FOOTNOTE );
	fs->setName( "Footnotes" );

	for ( int i = 0; i < pages; i++ ) {
	    KWFrame *frame = new KWFrame( getPTLeftBorder(), i * getPTPaperHeight() + getPTPaperHeight() - getPTTopBorder() - 20,
					  getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
	    fs->addFrame( frame );
	}
	frames.append( fs );
	fs->setAutoCreateNewFrame( true );
	fs->setVisible( false );
    }

    for ( unsigned int i = 0; i < getNumGroupManagers(); i++ ) {
	if ( getGroupManager( i )->isActive() )
	    getGroupManager( i )->init();
    }

    KWordChild *ch = 0L;
    for ( ch = m_lstChildren.first(); ch != 0; ch = m_lstChildren.next() ) {
	KWPartFrameSet *frameset = new KWPartFrameSet( this, ch );
	frameset->setName( i18n( "Frameset %1" ).arg( frames.count() + 1 ) );
	QRect r = ch->geometry();
	KWFrame *frame = new KWFrame( r.x(), r.y(), r.width(), r.height() );
	frameset->addFrame( frame );
	addFrameSet( frameset );
	emit sig_insertObject( ch, frameset );
    }

    KWAutoFormatEntry *entry;

    entry = new KWAutoFormatEntry;
    entry->setFind( "(C)" );
    entry->setReplace( "©" );
    autoFormat.addAutoFormatEntry( entry );

    entry = new KWAutoFormatEntry;
    entry->setFind( "(c)" );
    entry->setReplace( "©" );
    autoFormat.addAutoFormatEntry( entry );

    entry = new KWAutoFormatEntry;
    entry->setFind( "(R)" );
    entry->setReplace( "®" );
    autoFormat.addAutoFormatEntry( entry );

    entry = new KWAutoFormatEntry;
    entry->setFind( "(r)" );
    entry->setReplace( "®" );
    autoFormat.addAutoFormatEntry( entry );

    return true;
}

/*================================================================*/
bool KWordDocument::loadStyleTemplates( const QDOM::Element& element )
{
    QDOM::Element style = element.firstChild();
    for ( ; !style.isNull(); style = style.nextSibling() ) {
	KWParagLayout *pl = new KWParagLayout( this, FALSE );
	if ( pl->load( style ) )
	    return FALSE;
	addStyleTemplate( pl );
    }
    
    return TRUE;
}

/*================================================================*/
void KWordDocument::loadFrameSets( const QDOM::Element &framesets )
{
    string tag;
    string name;

    bool autoCreateNewFrame = true;
    FrameInfo frameInfo = FI_BODY;
    QString _name = "";
    int _row = 0, _col = 0, _rows = 1, _cols = 1;
    bool _visible = true;

    QDOM::Element frameset = framesets.firstChild();
    for ( ; !frameset.isNull(); frameset = frameset.nextSibling() ) {
	FrameType frameType = FT_BASE;
	_name = "";
	_row = _col = 0, _rows = 1, _cols = 1;
	_visible = true;
	bool removeable = false;
	QString fsname;
	
	frameType = (FrameType)frameset.attribute( "frameType" ).toInt();
	autoCreateNewFrame = frameset.attribute( "autoCreateNewFrame" ).toInt();
	frameInfo = (FrameInfo)frameset.attribute( "frameInfo" )toInt();
	_name = frameset.attribute( "grpMgr" );
	_row = frameset.attribute( "row" ).toInt();
	_col = frameset.attribute( "col" ).toInt();
	removeable = (bool)frameset.attribute( "removeable" ).toInt();
	_rows = frameset.attribute( "rows" ).toInt();
	_cols = frameset.attribute( "cols" ).toInt();
	_visible = (bool)frameset.attribute( "visible" ).toInt();
	fsname = frameset.attribute( "name" );
	
	if ( fsname.isEmpty() )
	    fsname = i18n( "Frameset %1" ).arg( frames.count() + 1 );

	switch ( frameType ) {
	case FT_TEXT: {
	    KWTextFrameSet *frame = new KWTextFrameSet( this );
	    frame->setVisible( _visible );
	    frame->setName( fsname );
	    frame->load( parser, lst );
	    frame->setAutoCreateNewFrame( autoCreateNewFrame );
	    frame->setFrameInfo( frameInfo );
	    frame->setIsRemoveableHeader( removeable );

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
		frame->setGroupManager( grpMgr );
		grpMgr->addFrameSet( frame, _row, _col );
		KWGroupManager::Cell *cell = grpMgr->getCell( _row, _col );
		if ( cell ) {
		    cell->rows = _rows;
		    cell->cols = _cols;
		}
	    } else
		frames.append( frame );
	} break;
	case FT_PICTURE: {
	    KWPictureFrameSet *frame = new KWPictureFrameSet( this );
	    frame->setName( fsname );
	    frame->load( parser, lst );
	    frame->setFrameInfo( frameInfo );
	    frames.append( frame );
	} break;
	default: break;
	}
    }
}

/*===================================================================*/
bool KWordDocument::completeLoading( KOStore::Store_ptr _store )
{
    if ( _store )
    {
	CORBA::String_var str = urlIntern.isEmpty() ? url() : urlIntern.latin1();

	QStringList::Iterator it = pixmapKeys.begin();

	for ( ; it != pixmapKeys.end(); ++it )
	{
	    QString u = str.in();
	    u += "/";
	    u += *it;

	    QImage img;

	    _store->open( u, 0L );
	    {
		istorestream in( _store );
		in >> img;
	    }
	    _store->close();

	    QString filename = *it;
	    int dashdash = filename.findRev( "--" );
	    if ( dashdash != -1 )
		filename == filename.left( dashdash );

	    KWImage image( this, img, filename );
	    if ( !img.isNull() )
		imageCollection.insertImage( *it, image );
	}
    }

    QDictIterator<KWCharImage> it2( imageRequests );
    for ( ; it2.current(); ++it2 )
	it2.current()->setImage( imageCollection.getImage( it2.currentKey() ) );

    QDictIterator<KWPictureFrameSet> it3( imageRequests2 );
    for ( ; it3.current(); ++it3 )
	it3.current()->setImage( imageCollection.getImage( it3.currentKey() ) );

    return true;
}

/*================================================================*/
bool KWordDocument::save( QIODevice* dev, KOStore::Store_ptr, const char* format )
{
    QDOM::Document doc( "DOC" );
    doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDOM::Element word = doc.createElement( "DOC" );

    word.setAttribute( "author", "Reginald Stadlbauer and Torben Weis" );
    word.setAttribute( "email", "reggie@kde.org and weis@kde.org" );
    word.setAttribute( "editor", "KWord" );
    word.setAttribute( "mime", "application/x-kword" );
    word.appendChild( spread );

    QDOM::Element paper = doc.createElement( "PAPER" );
    word.appendChild( paper );
    paper.setAttribute( "format", (int)pageLayout.format );
    paper.setAttribute( "ptWidth", pageLayout.ptWidth );
    paper.setAttribute( "ptHeight", pageLayout.ptHeight );
    paper.setAttribute( "mmWidth", pageLayout.mmWidth );
    paper.setAttribute( "mmHeight", pageLayout.mmHeight );
    paper.setAttribute( "inchWidth", pageLayout.inchWidth );
    paper.setAttribute( "inchHeight", pageLayout.inchHeight );
    paper.setAttribute( "inchHeight", pageLayout.inchHeight );
    paper.setAttribute( "columns", pageColumns.columns );
    paper.setAttribute( "ptColumnspc", pageColumns.ptColumnSpacing );
    paper.setAttribute( "mmColumnspc", pageColumns.mmColumnSpacing );
    paper.setAttribute( "inchColumnspc", pageColumns.inchColumnSpacing );
    paper.setAttribute( "hType", (int)pageHeaderFooter.header );
    paper.setAttribute( "fType", (int)pageHeaderFooter.footer );
    paper.setAttribute( "ptHeadBody", pageHeaderFooter.ptHeaderBodySpacing );
    paper.setAttribute( "mmHeadBody", pageHeaderFooter.mmHeaderBodySpacing );
    paper.setAttribute( "inchHeadBody", pageHeaderFooter.inchHeaderBodySpacing );
    paper.setAttribute( "ptFootBody", pageHeaderFooter.ptFooterBodySpacing );
    paper.setAttribute( "mmFootBody", pageHeaderFooter.mmFooterBodySpacing );
    paper.setAttribute( "inchFootBody", pageHeaderFooter.inchFooterBodySpacing );

    QDOM::Element e = formatCollection->save( doc );
    if ( e.isNull() )
	return FALSE;
    word.append( e );

    QDOM::Element border = doc.createElement( "PAPERBORDERS" );
    paper.appendChild( border );
    border.setAttribute( "mmLeft", pageLayout.mmLeft );
    border.setAttribute( "mmTop", pageLayout.mmTop );
    border.setAttribute( "mmRight", pageLayout.mmRight );
    border.setAttribute( "mmBottom", pageLayout.mmBottom );
    border.setAttribute( "ptLeft", pageLayout.ptLeft );
    border.setAttribute( "ptTop", pageLayout.ptTop );
    border.setAttribute( "ptRight", pageLayout.ptRight );
    border.setAttribute( "ptBottom", pageLayout.ptBottom );
    border.setAttribute( "inchLeft", pageLayout.inchLeft );
    border.setAttribute( "inchTop", pageLayout.inchTop );
    border.setAttribute( "inchRight", pageLayout.inchRight );
    border.setAttribute( "inchBottom", pageLayout.inchBottom );

    QDOM::Element attr = doc.createElement( "ATTRIBUTES" );
    word.appendChild( attr );
    attr.setAttribute( "processing", (int)processingType );
    attr.setAttribute( "hasHeader", hasHeader() );
    attr.setAttribute( "hasFooter", hasFooter() );
    attr.setAttribute( "unit", getUnit() );

    QDOM::Element fn = footNodeManager.save( doc );
    if ( fn.isNull() )
	return false;
    word.appendChild( fn );

    QDOM::Element fs = doc.createElement( "FRAMESETS" );
    word.appendChild( fs );

    KWFrameSet *frameSet = 0L;
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	frameSet = getFrameSet( i );
	if ( frameSet->getFrameType() != FT_PART )
	{
	    QDOM::Element e = frameSet->save( doc );
	    if ( e.isNull() )
		return false;
	    fs.append( e );
	}
    }

    QDOM::Element styles = doc.createElement( "STYLES" );
    word.appendChild( styles );

    for ( unsigned int j = 0; j < paragLayoutList.count(); j++ )
    {
	QDOM::Element e = paragLayoutList.at( j )->save( doc );
	if ( e.isNull() )
	    return false;
	styles.append( e );
    }

    QDOM::Element pix = doc.createElement( "PIXMAPS" );
    word.appendChild( pix );

    QDictIterator<KWImage> it = imageCollection.iterator();
    QStringList keys, images;
    for ( ; it.current(); ++it )
    {
	if ( keys.contains( it.currentKey() ) || images.contains( it.current()->getFilename() ) )
	    continue;
	QDOM::Element e = doc.createElement( "KEY" );
	e.setAttribute( "key", it.current()->getFilename() );
	pix.appendChild( e )
	    keys.append( it.currentKey() );
	images.append( it.current()->getFilename() );
    }

    // Write "OBJECT" tag for every child
    QListIterator<KWordChild> chl( m_lstChildren );
    for( ; chl.current(); ++chl )
    {
	QDOM::Element e = chl.current()->save( doc );
	if ( e.isNull() )
	    return false;
	word.appendChild( e );
    }

    return true;
}

/*==============================================================*/
bool KWordDocument::completeSaving( KOStore::Store_ptr _store )
{
    if ( !_store )
	return true;

    CORBA::String_var u = url();
    QDictIterator<KWImage> it = imageCollection.iterator();

    QStringList keys, images;

    for( ; it.current(); ++it )
    {
	if ( keys.contains( it.currentKey() ) || images.contains( it.current()->getFilename() ) )
	    continue;

	QString u2 = u.in();
	u2 += "/";
	u2 += it.current()->getFilename();

	QString format = QFileInfo( it.current()->getFilename() ).extension().upper();
	if ( format == "JPG" )
	    format = "JPEG";
	if ( QImage::outputFormats().find( format ) == -1 )
	    format = "BMP";

	QString mime = "image/" + format.lower();
	_store->open( u2, mime.lower() );
	ostorestream out( _store );
	writeImageToStream( out, *it.current(), format );
	out.flush();
	_store->close();
	keys.append( it.currentKey() );
	images.append( it.current()->getFilename() );
    }

    return true;
}

/*================================================================*/
void KWordDocument::enableEmbeddedParts( bool f )
{
    KWFrameSet *frameSet = 0L;
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	frameSet = getFrameSet( i );
	if ( frameSet->getFrameType() == FT_PART )
	    dynamic_cast<KWPartFrameSet*>( frameSet )->enableDrawing( f );
    }
}

/*================================================================*/
void KWordDocument::makeChildListIntern( KOffice::Document_ptr _doc, const char *_path )
{
    cerr << "void KWordDocument::makeChildList( OPParts::Document_ptr _doc, const char *_path )" << endl;

    int i = 0;

    QListIterator<KWordChild> it( m_lstChildren );
    for( ; it.current(); ++it )
    {
	QString tmp;
	tmp.sprintf( "/%i", i++ );
	QString path( _path );
	path += tmp.data();
	cerr << "SETTING NAME To " << path.data() << endl;

	KOffice::Document_var doc = it.current()->document();
	doc->makeChildList( _doc, path );
    }
}

/*================================================================*/
QStrList KWordDocument::outputFormats()
{
    return QStrList();
}

/*================================================================*/
QStrList KWordDocument::inputFormats()
{
    return QStrList();
}

/*================================================================*/
KOffice::MainWindow_ptr KWordDocument::createMainWindow()
{
    KWordShell* shell = new KWordShell;
    shell->show();
    shell->setDocument( this );

    return KOffice::MainWindow::_duplicate( shell->koInterface() );
}

/*================================================================*/
void KWordDocument::viewList( OpenParts::Document::ViewList*& _list )
{
    ( *_list ).length( m_lstViews.count() );

    int i = 0;
    QListIterator<KWordView> it( m_lstViews );
    for( ; it.current(); ++it )
    {
	( *_list )[ i++ ] = OpenParts::View::_duplicate( it.current() );
    }
}

/*================================================================*/
void KWordDocument::addView( KWordView *_view )
{
    m_lstViews.append( _view );
}

/*================================================================*/
void KWordDocument::removeView( KWordView *_view )
{
    m_lstViews.setAutoDelete( false );
    m_lstViews.removeRef( _view );
    m_lstViews.setAutoDelete( true );
}

/*================================================================*/
KWordView* KWordDocument::createWordView( QWidget* _parent )
{
    KWordView *p = new KWordView( _parent, 0L, this );
    //p->QWidget::show();
    m_lstViews.append( p );

    return p;
}

/*================================================================*/
OpenParts::View_ptr KWordDocument::createView()
{
    return OpenParts::View::_duplicate( createWordView() );
}

/*================================================================*/
void KWordDocument::insertObject( const QRect& _rect, KoDocumentEntry& _e, int diffx, int diffy )
{
    KOffice::Document_var doc = _e.createDoc();
    if ( CORBA::is_nil( doc ) )
	return;

    if ( !doc->initDoc() )
    {
	QMessageBox::critical( ( QWidget* )0L, i18n( "KWord Error" ), i18n( "Could not init" ), i18n( "OK" ) );
	return;
    }

    KWordChild* ch = new KWordChild( this, _rect, doc, diffx, diffy );

    insertChild( ch );
    m_bModified = true;

    KWPartFrameSet *frameset = new KWPartFrameSet( this, ch );
    KWFrame *frame = new KWFrame( _rect.x() + diffx, _rect.y() + diffy, _rect.width(), _rect.height() );
    frameset->addFrame( frame );
    addFrameSet( frameset );

    emit sig_insertObject( ch, frameset );
}

/*================================================================*/
void KWordDocument::insertChild( KWordChild *_child )
{
    m_lstChildren.append( _child );
}

/*================================================================*/
void KWordDocument::changeChildGeometry( KWordChild *_child, const QRect& _rect )
{
    _child->setGeometry( _rect );

    emit sig_updateChildGeometry( _child );
}

/*================================================================*/
QListIterator<KWordChild> KWordDocument::childIterator()
{
    return QListIterator<KWordChild> ( m_lstChildren );
}

/*================================================================*/
void KWordDocument::draw( QPaintDevice *, CORBA::Long, CORBA::Long, CORBA::Float )
{
}

/*================================================================*/
QPen KWordDocument::setBorderPen( KWParagLayout::Border _brd )
{
    QPen pen( black, 1, SolidLine );

    pen.setWidth( _brd.ptWidth );
    pen.setColor( _brd.color );

    switch ( _brd.style )
    {
    case KWParagLayout::SOLID:
	pen.setStyle( SolidLine );
	break;
    case KWParagLayout::DASH:
	pen.setStyle( DashLine );
	break;
    case KWParagLayout::DOT:
	pen.setStyle( DotLine );
	break;
    case KWParagLayout::DASH_DOT:
	pen.setStyle( DashDotLine );
	break;
    case KWParagLayout::DASH_DOT_DOT:
	pen.setStyle( DashDotDotLine );
	break;
    }

    return QPen( pen );
}

/*================================================================*/
KWUserFont* KWordDocument::findUserFont( QString _userfontname )
{
    if ( cUserFont )
    {
	if ( cUserFont->getFontName() == _userfontname ) return cUserFont;
    }

    KWUserFont* font = 0L;
    for ( font = userFontList.first(); font != 0L; font = userFontList.next() )
    {
	if ( font->getFontName() == _userfontname )
	{
	    cUserFont = font;
	    return font;
	}
    }

    font = new KWUserFont( this, _userfontname );
    cUserFont = font;

    return font;
}

/*================================================================*/
KWDisplayFont* KWordDocument::findDisplayFont( KWUserFont* _font, unsigned int _size, int _weight, bool _italic, bool _underline )
{
    if ( cDisplayFont )
    {
	if ( cDisplayFont->getUserFont()->getFontName() == _font->getFontName() && cDisplayFont->getPTSize() == _size &&
	     cDisplayFont->weight() == _weight && cDisplayFont->italic() == _italic && cDisplayFont->underline() == _underline )
	    return cDisplayFont;
    }

    KWDisplayFont* font = 0L;
    for ( font = displayFontList.first(); font != 0L; font = displayFontList.next() )
    {
	if ( font->getUserFont()->getFontName() == _font->getFontName() && font->getPTSize() == _size &&
	     font->weight() == _weight && font->italic() == _italic && font->underline() == _underline )
	{
	    cDisplayFont = font;
	    return font;
	}
    }

    font = new KWDisplayFont( this, _font, _size, _weight, _italic, _underline );
    cDisplayFont = font;

    return font;
}

/*================================================================*/
KWParagLayout* KWordDocument::findParagLayout( QString _name )
{
    if ( cParagLayout )
    {
	if ( cParagLayout->getName() == _name ) return cParagLayout;
    }

    KWParagLayout* p;
    for ( p = paragLayoutList.first(); p != 0L; p = paragLayoutList.next() )
    {
	if ( p->getName() == _name )
	{
	    cParagLayout = p;
	    return p;
	}
    }

    return 0L;
}

/*================================================================*/
bool KWordDocument::isPTYInFrame( unsigned int _frameSet, unsigned int _frame, unsigned int _ypos )
{
    return frames.at( _frameSet )->isPTYInFrame( _frame, _ypos );
}

/*================================================================*/
KWParag* KWordDocument::findFirstParagOfPage( unsigned int _page, unsigned int _frameset )
{
    if ( frames.at( _frameset )->getFrameType() != FT_TEXT ) return 0L;

    KWParag *p = dynamic_cast<KWTextFrameSet*>( frames.at( _frameset ) )->getFirstParag();
    while ( p )
    {
	if ( p->getEndPage() == _page || p->getStartPage() == _page || ( p->getEndPage() > _page && p->getStartPage() < _page ) )
	    return p;
	p = p->getNext();
    }

    return 0L;
}

/*================================================================*/
KWParag* KWordDocument::findFirstParagOfRect( unsigned int _ypos, unsigned int _page, unsigned int _frameset )
{
    if ( frames.at( _frameset )->getFrameType() != FT_TEXT ) return 0L;

    if ( frames.at( _frameset )->getFrameInfo() != FI_BODY )
	return dynamic_cast<KWTextFrameSet*>( frames.at( _frameset ) )->getFirstParag();

    KWParag *p = dynamic_cast<KWTextFrameSet*>( frames.at( _frameset ) )->getFirstParag();
    while ( p )
    {
	if ( p->getPTYEnd() >= _ypos || p->getPTYStart() >= _ypos || ( p->getPTYEnd() >= _ypos && p->getPTYStart() <= _ypos )
	     || ( p->getPTYEnd() <= _ypos && p->getPTYStart() <= _ypos && p->getPTYStart() > p->getPTYEnd() &&
		  ( p->getEndPage() == _page || p->getStartPage() == _page || ( p->getEndPage() > _page && p->getStartPage() < _page ) ) ) )
	    return p;
	p = p->getNext();
    }

    return 0L;
}

/*================================================================*/
bool KWordDocument::printLine( KWFormatContext &_fc, QPainter &_painter, int xOffset, int yOffset, int _w, int _h,
			       bool _viewFormattingChars, bool _drawVarBack )
{
    if ( !getFrameSet( _fc.getFrameSet() - 1 )->isVisible() )
	return false;

    _painter.save();

    unsigned int xShift = getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->left();

    QRegion cr = QRegion( xShift - xOffset - _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth / 2 -
			  _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth,
			  _fc.getPTY() - yOffset - _fc.getParag()->getParagLayout()->getTopBorder().ptWidth -
			  _fc.getParag()->getParagLayout()->getTopBorder().ptWidth / 2,
			  getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->width() +
			  _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth +
			  _fc.getParag()->getParagLayout()->getRightBorder().ptWidth +
			  _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth,
			  _fc.getLineHeight() + _fc.getParag()->getParagLayout()->getTopBorder().ptWidth +
			  _fc.getParag()->getParagLayout()->getBottomBorder().ptWidth +
			  _fc.getParag()->getParagLayout()->getTopBorder().ptWidth );

    if ( static_cast<int>( _fc.getPTY() + _fc.getLineHeight() ) > getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->bottom() )
	cr = QRegion( 0, 0, 0, 0 );

    QRegion visible( 0, 0, _w, _h );

    if ( _painter.hasClipping() )
	cr = _painter.clipRegion().intersect( cr );

    if ( cr.intersect( visible ).isEmpty() )
    {
	_painter.restore();
	return false;
    }

    _painter.setClipRegion( cr );

    if ( _fc.isCursorInFirstLine() && _fc.getParag()->getParagLayout()->getTopBorder().ptWidth > 0 )
    {
	unsigned int _x1 = getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->left() - xOffset;
	unsigned int _y = _fc.getPTY() - yOffset + _fc.getParag()->getParagLayout()->getTopBorder().ptWidth / 2;
	unsigned int _x2 = _x1 + getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->width();

	_painter.setPen( setBorderPen( _fc.getParag()->getParagLayout()->getTopBorder() ) );
	_painter.drawLine( _x1, _y, _x2, _y );
    }
    if ( _fc.isCursorInLastLine() && _fc.getParag()->getParagLayout()->getBottomBorder().ptWidth > 0 )
    {
	unsigned int _x1 = getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->left() - xOffset;
	unsigned int _y = _fc.getPTY() - yOffset + _fc.getLineHeight() - _fc.getParag()->getParagLayout()->getBottomBorder().ptWidth / 2 - 1;
	unsigned int _x2 = _x1 + getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->width();

	_painter.setPen( setBorderPen( _fc.getParag()->getParagLayout()->getBottomBorder() ) );
	_painter.drawLine( _x1, _y, _x2, _y );
    }
    if ( _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth > 0 )
    {
	unsigned int _x = getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->left() - xOffset +
			  _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth / 2;
	unsigned int _y1 = _fc.getPTY() - yOffset;
	unsigned int _y2 = _fc.getPTY() - yOffset + _fc.getLineHeight();

	_painter.setPen( setBorderPen( _fc.getParag()->getParagLayout()->getLeftBorder() ) );
	_painter.drawLine( _x, _y1, _x, _y2 );
    }
    if ( _fc.getParag()->getParagLayout()->getRightBorder().ptWidth > 0 )
    {
	unsigned int _x = getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->right() - xOffset -
			  _fc.getParag()->getParagLayout()->getRightBorder().ptWidth / 2;
	unsigned int _y1 = _fc.getPTY() - yOffset;
	unsigned int _y2 = _fc.getPTY() - yOffset + _fc.getLineHeight();

	_painter.setPen( setBorderPen( _fc.getParag()->getParagLayout()->getRightBorder() ) );
	_painter.drawLine( _x, _y1, _x, _y2 );
    }

    // Shortcut to the text memory segment
    unsigned int textLen = _fc.getParag()->getTextLen() - 1;
    KWChar* text = _fc.getParag()->getText();
    // Shortcut to the current paragraph layout
    KWParagLayout *lay = _fc.getParag()->getParagLayout();
    // Index in the text memory segment that points to the line start
    unsigned int pos = _fc.getLineStartPos();
    int plus = 0;

    // First line ? Draw the counter ?
    if ( pos == 0 && lay->getCounterType() != KWParagLayout::CT_NONE )
    {
	//_painter.fillRect( _fc.getPTCounterPos() - xOffset, _fc.getPTY(), _fc.getPTCounterWidth(), _fc.getLineHeight(), lightGray );
	KWFormat counterfm( this, _fc );
	counterfm.apply( lay->getFormat() );
	if ( _fc.getParag()->getParagLayout()->getCounterType() == KWParagLayout::CT_BULLET )
	    counterfm.setUserFont( findUserFont( _fc.getParag()->getParagLayout()->getBulletFont() ) );
	_painter.setFont( *( counterfm.loadFont( this ) ) );
	_painter.setPen( counterfm.getColor() );

	_painter.drawText( _fc.getPTCounterPos() - xOffset, _fc.getPTY() + _fc.getPTMaxAscender() - yOffset, _fc.getCounterText() );
    }

    // paint it character for character. Provisionally! !!HACK!!
    _fc.cursorGotoLineStart();

    // Init font and style
    _painter.setFont( *_fc.loadFont( this ) );
    _painter.setPen( _fc.getColor() );

    char buffer[ 200 ];
    int i = 0;
    unsigned int tmpPTPos = 0;
    unsigned int lastPTPos = 0;
    while ( !_fc.isCursorAtLineEnd() )
    {
	// Init position
	if ( i == 0 )
	{
	    // Change the painter
	    tmpPTPos = _fc.getPTPos();
	    _painter.setFont( *_fc.loadFont( this ) );
	    _painter.setPen( _fc.getColor() );
	}

	if ( i > 200 || _fc.getTextPos() > textLen )
	{
	    warning( "Reggie: WOW - something has gone really wrong here!!!!!" );
	    return false;
	}

	buffer[ i ] = text[ _fc.getTextPos() ].c;

	if ( buffer[ i ] == 0 )
	{
	    buffer[ i ] = '\0';
	    _painter.drawText( tmpPTPos - xOffset,
			       _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() -
			       _fc.getParag()->getParagLayout()->getLineSpacing().pt() - yOffset + plus, buffer );
	    i = 0;

	    switch ( text[ _fc.getTextPos() ].attrib->getClassId() )
	    {
	    case ID_KWCharImage:
	    {
		_painter.drawImage( QPoint( tmpPTPos - xOffset, _fc.getPTY() - yOffset +
					    ( ( _fc.getLineHeight() - _fc.getParag()->getParagLayout()->getLineSpacing().pt() )
					      - ( ( KWCharImage* )text[ _fc.getTextPos() ].attrib )->getImage()->height() ) ),
				    *( ( KWCharImage* )text[ _fc.getTextPos() ].attrib )->getImage() );
		_fc.cursorGotoNextChar();
	    } break;
	    case ID_KWCharVariable:
	    {
		KWCharFormat *f = ( KWCharFormat* )text[ _fc.getTextPos() ].attrib;
		_fc.apply( *f->getFormat() );
		// Change the painter
		if ( _fc.getVertAlign() == KWFormat::VA_NORMAL )
		{
		    _painter.setFont( *_fc.loadFont( this ) );
		    plus = 0;
		}
		else if ( _fc.getVertAlign() == KWFormat::VA_SUB )
		{
		    QFont _font = *_fc.loadFont( this );
		    _font.setPointSize( ( 2 * _font.pointSize() ) / 3 );
		    _painter.setFont( _font );
		    plus = _font.pointSize() / 2;
		}
		else if ( _fc.getVertAlign() == KWFormat::VA_SUPER )
		{
		    QFont _font = *_fc.loadFont( this );
		    _font.setPointSize( ( 2 * _font.pointSize() ) / 3 );
		    _painter.setFont( _font );
		    plus = - _fc.getPTAscender() + _font.pointSize() / 2;
		}
		_painter.setPen( _fc.getColor() );

		KWCharVariable *v = dynamic_cast<KWCharVariable*>( text[ _fc.getTextPos() ].attrib );

		if ( _drawVarBack )
		    _painter.fillRect( tmpPTPos - xOffset, _fc.getPTY() - yOffset,
				       _painter.fontMetrics().width( v->getText() ), _fc.getLineHeight(), gray );

		_painter.drawText( tmpPTPos - xOffset,
				   _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() - yOffset -
				   _fc.getParag()->getParagLayout()->getLineSpacing().pt() + plus, v->getText() );

		_fc.cursorGotoNextChar();
	    } break;
	    case ID_KWCharFootNote:
	    {
		KWCharFootNote *fn = dynamic_cast<KWCharFootNote*>( text[ _fc.getTextPos() ].attrib );

//	if ( _drawVarBack )
//	  _painter.fillRect( tmpPTPos - xOffset, _fc.getPTY() - yOffset,
//		    _painter.fontMetrics().width( v->getText() ), _fc.getLineHeight(), gray );

		KWCharFormat *f = ( KWCharFormat* )text[ _fc.getTextPos() ].attrib;
		_fc.apply( *f->getFormat() );
		// Change the painter
		if ( _fc.getVertAlign() == KWFormat::VA_NORMAL )
		{
		    _painter.setFont( *_fc.loadFont( this ) );
		    plus = 0;
		}
		else if ( _fc.getVertAlign() == KWFormat::VA_SUB )
		{
		    QFont _font = *_fc.loadFont( this );
		    _font.setPointSize( ( 2 * _font.pointSize() ) / 3 );
		    _painter.setFont( _font );
		    plus = _font.pointSize() / 2;
		}
		else if ( _fc.getVertAlign() == KWFormat::VA_SUPER )
		{
		    QFont _font = *_fc.loadFont( this );
		    _font.setPointSize( ( 2 * _font.pointSize() ) / 3 );
		    _painter.setFont( _font );
		    plus = - _fc.getPTAscender() + _font.pointSize() / 2;
		}
		_painter.setPen( _fc.getColor() );

		_painter.drawText( tmpPTPos - xOffset,
				   _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() - yOffset -
				   _fc.getParag()->getParagLayout()->getLineSpacing().pt() + plus, fn->getText() );

		_fc.cursorGotoNextChar();
	    } break;
	    case ID_KWCharTab:
	    {
		lastPTPos = _fc.getPTPos();
		_fc.cursorGotoNextChar();
		QPen _pen = QPen( _painter.pen() );
		_painter.setPen( QPen( blue, 1, DotLine ) );
		if ( _viewFormattingChars )
		    _painter.drawLine( lastPTPos, _fc.getPTY() + _fc.getPTMaxAscender(),
				       _fc.getPTPos(), _fc.getPTY() + _fc.getPTMaxAscender() );
		_painter.setPen( _pen );
	    } break;
	    }
	}
	else
	{
	    if ( text[ _fc.getTextPos() ].attrib != 0L )
	    {
		// Change text format here
		assert( text[ _fc.getTextPos() ].attrib->getClassId() == ID_KWCharFormat );
		KWCharFormat *f = ( KWCharFormat* )text[ _fc.getTextPos() ].attrib;
		_fc.apply( *f->getFormat() );
		// Change the painter
		if ( _fc.getVertAlign() == KWFormat::VA_NORMAL )
		{
		    _painter.setFont( *_fc.loadFont( this ) );
		    plus = 0;
		}
		else if ( _fc.getVertAlign() == KWFormat::VA_SUB )
		{
		    QFont _font = *_fc.loadFont( this );
		    _font.setPointSize( ( 2 * _font.pointSize() ) / 3 );
		    _painter.setFont( _font );
		    plus = _font.pointSize() / 2;
		}
		else if ( _fc.getVertAlign() == KWFormat::VA_SUPER )
		{
		    QFont _font = *_fc.loadFont( this );
		    _font.setPointSize( ( 2 * _font.pointSize() ) / 3 );
		    _painter.setFont( _font );
		    plus = - _fc.getPTAscender() + _font.pointSize() / 2;
		}
		_painter.setPen( _fc.getColor() );
	    }

	    // Test next character.
	    i++;
	    if ( _fc.cursorGotoNextChar() != 1 || ( text[ _fc.getTextPos() ].c == ' ' &&
						    _fc.getParag()->getParagLayout()->getFlow() == KWParagLayout::BLOCK ) || i >= 199 )
	    {
		// there was a blank _or_ there will be a font switch _or_ a special object next, so print
		// what we have so far
		buffer[ i ] = '\0';
		_painter.drawText(tmpPTPos - xOffset, /*_fc.getPTY() + _fc.getPTMaxAscender() - yOffset*/
				  _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() - yOffset -
				  _fc.getParag()->getParagLayout()->getLineSpacing().pt() + plus, buffer );
		//cerr << "#'" << buffer << "'" << endl;
		i = 0;
		// Blanks are not printed at all - but we have to underline it in some cases
		if ( text[ _fc.getTextPos() ].c == ' ' )
		{
		    bool goneForward = false;
		    if ( _fc.getUnderline() && _fc.getTextPos() > _fc.getLineStartPos() && _fc.getTextPos() < _fc.getLineEndPos() - 1 )
		    {
			if ( text[ _fc.getTextPos() - 1 ].c != KWSpecialChar && text[ _fc.getTextPos() + 1 ].c != KWSpecialChar )
			{
			    KWCharFormat *f1 = ( KWCharFormat* )text[ _fc.getTextPos() - 1 ].attrib;
			    KWCharFormat *f2 = ( KWCharFormat* )text[ _fc.getTextPos() + 1 ].attrib;
			    if ( f1->getFormat()->getUnderline() && f2->getFormat()->getUnderline() )
			    {
				KWFormat *_f = f1->getFormat();
				QFontMetrics fm( *findDisplayFont( _f->getUserFont(), _f->getPTFontSize(), _f->getWeight(),
								   _f->getItalic(), _f->getUnderline() ) );

				_painter.setPen( QPen( _fc.getColor(), fm.lineWidth(), SolidLine ) );
				int ly = _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() - yOffset -
					 _fc.getParag()->getParagLayout()->getLineSpacing().pt() + plus + fm.underlinePos() + fm.lineWidth() / 2;
				int lx1 = _fc.getPTPos();
				_fc.cursorGotoNextChar();
				goneForward = true;
				int lx2 = _fc.getPTPos();
				_painter.drawLine( lx1, ly, lx2, ly );
			    }
			}

		    }
		    lastPTPos = _fc.getPTPos();
		    if ( !goneForward ) _fc.cursorGotoNextChar();
		    if ( _viewFormattingChars )
			_painter.fillRect( lastPTPos + ( _fc.getPTPos() - lastPTPos ) / 2, _fc.getPTY() + _fc.getPTMaxAscender() / 2, 1, 1, blue );
		}
	    }
	}
    }

    if ( _viewFormattingChars && _fc.isCursorAtParagEnd() )
	_painter.drawPixmap( _fc.getPTPos() + 3, _fc.getPTY() + _fc.getPTMaxAscender() - ret_pix.height(), ret_pix );

    _painter.restore();
    return true;
}

/*================================================================*/
void KWordDocument::printBorders( QPainter &_painter, int xOffset, int yOffset, int, int )
{
    KWFrameSet *frameset = 0;
    KWFrame *tmp;
    QRect frame;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	frameset = getFrameSet( i );
	if ( !frameset->isVisible() ) continue;

	if ( isAHeader( getFrameSet( i )->getFrameInfo() ) && !hasHeader() ||
	     isAFooter( getFrameSet( i )->getFrameInfo() ) && !hasFooter() ||
	     isAWrongHeader( getFrameSet( i )->getFrameInfo(), getHeaderType() ) ||
	     isAWrongFooter( getFrameSet( i )->getFrameInfo(), getFooterType() ) )
	    continue;
	for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ )
	{
	    bool isRight = true, isBottom = true;
	    if ( frameset->getGroupManager() )
	    {
		unsigned int r, c;
		frameset->getGroupManager()->getFrameSet( frameset, r, c );
		if ( r < frameset->getGroupManager()->getRows() - 1 ) isBottom = false;
		if ( c < frameset->getGroupManager()->getCols() - 1 ) isRight = false;
	    }

	    tmp = frameset->getFrame( j );
	    frame = QRect( tmp->x() - xOffset - 1, tmp->y() - yOffset - 1, tmp->width() + 2, tmp->height() + 2 );

	    //if ( !frame.intersects( QRect( xOffset, yOffset, _w, _h ) ) ) continue;

	    if ( isAHeader( frameset->getFrameInfo() ) || isAFooter( frameset->getFrameInfo() ) )
		tmp = frameset->getFrame( 0 );
	    else
		tmp = frameset->getFrame( j );

	    _painter.fillRect( QRect( frame.x(), frame.y(), frame.width() - ( isRight ? 1 : 0 ),
				      frame.height() - ( isBottom ? 1 : 0 ) ), tmp->getBackgroundColor() );

	    if ( tmp->getLeftBorder().ptWidth > 0 && tmp->getLeftBorder().color != tmp->getBackgroundColor().color() )
	    {
		QPen p( setBorderPen( tmp->getLeftBorder() ) );
		_painter.setPen( p );
		_painter.drawLine( frame.x() + tmp->getLeftBorder().ptWidth / 2, frame.y(),
				   frame.x() + tmp->getLeftBorder().ptWidth / 2, frame.bottom() + ( isBottom ? 0 : 1 ) );
	    }
	    if ( tmp->getRightBorder().ptWidth > 0 && tmp->getRightBorder().color != tmp->getBackgroundColor().color() )
	    {
		QPen p( setBorderPen( tmp->getRightBorder() ) );
		_painter.setPen( p );
		int w = tmp->getRightBorder().ptWidth;
		if ( ( w / 2 ) * 2 == w ) w--;
		w /= 2;
		_painter.drawLine( frame.right() - w, frame.y(),
				   frame.right() - w, frame.bottom() + ( isBottom ? 0 : 1 ) );
	    }
	    if ( tmp->getTopBorder().ptWidth > 0 && tmp->getTopBorder().color != tmp->getBackgroundColor().color() )
	    {
		QPen p( setBorderPen( tmp->getTopBorder() ) );
		_painter.setPen( p );
		_painter.drawLine( frame.x(), frame.y() + tmp->getTopBorder().ptWidth / 2,
				   frame.right() + ( isRight ? 0 : 1 ), frame.y() + tmp->getTopBorder().ptWidth / 2 );
	    }
	    if ( tmp->getBottomBorder().ptWidth > 0 && tmp->getBottomBorder().color != tmp->getBackgroundColor().color() )
	    {
		QPen p( setBorderPen( tmp->getBottomBorder() ) );
		_painter.setPen( p );
		int w = tmp->getBottomBorder().ptWidth;
		if ( ( w / 2 ) * 2 == w ) w--;
		w /= 2;
		_painter.drawLine( frame.x(), frame.bottom() - w,
				   frame.right() + ( isRight ? 0 : 1 ), frame.bottom() - w );
	    }
	}
    }
}

/*================================================================*/
void KWordDocument::drawMarker( KWFormatContext &_fc, QPainter *_painter, int xOffset, int yOffset )
{
    RasterOp rop = _painter->rasterOp();

    _painter->setRasterOp( NotROP );
    QPen pen;
    pen.setWidth( 1 );
    _painter->setPen( pen );

    unsigned int diffx1 = 0;
    unsigned int diffx2 = 0;
    if ( _fc.getItalic() )
    {
	diffx1 = static_cast<int>( static_cast<float>( _fc.getLineHeight() ) / 3.732 );
	diffx2 = 0;
    }

    _painter->drawLine( _fc.getPTPos() - xOffset + diffx1,
			_fc.getPTY() - yOffset,
			_fc.getPTPos() - xOffset + diffx2,
			_fc.getPTY() + _fc.getLineHeight() - _fc.getParag()->getParagLayout()->getLineSpacing().pt() - yOffset );

    _painter->setRasterOp( rop );
}

/*================================================================*/
void KWordDocument::updateAllViews( KWordView *_view, bool _clear )
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() )
    {
	for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
	{
	    if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
	    {
		if ( viewPtr != _view )
		{
		    if ( _clear ) viewPtr->getGUI()->getPaperWidget()->clear();
		    viewPtr->getGUI()->getPaperWidget()->repaintScreen( false );
		}
	    }
	}
    }
}

/*================================================================*/
void KWordDocument::updateAllViewportSizes()
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() )
    {
	for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
	{
	    if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
		viewPtr->getGUI()->getPaperWidget()->resizeContents( getPTPaperWidth(), getPTPaperHeight() * pages );
	}
    }
}

/*================================================================*/
void KWordDocument::setUnitToAll()
{
    if ( unit == "mm" )
	pageLayout.unit = PG_MM;
    else if ( unit == "pt" )
	pageLayout.unit = PG_PT;
    else if ( unit == "inch" )
	pageLayout.unit = PG_INCH;


    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() )
    {
	for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
	{
	    if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
	    {
		viewPtr->getGUI()->getHorzRuler()->setUnit( getUnit() );
		viewPtr->getGUI()->getVertRuler()->setUnit( getUnit() );
	    }
	}
    }
}

/*================================================================*/
void KWordDocument::updateAllCursors()
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() )
    {
	for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
	{
	    if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
	    {
		if ( viewPtr->getGUI() )
		{
		    viewPtr->getGUI()->getPaperWidget()->recalcText();
		    viewPtr->getGUI()->getPaperWidget()->recalcCursor();
		}
	    }
	}
    }
}

/*================================================================*/
void KWordDocument::updateAllStyleLists()
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() )
    {
	for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
	{
	    if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
		viewPtr->updateStyleList();
	}
    }
}

/*================================================================*/
void KWordDocument::drawAllBorders( bool back )
{
    QPainter *_painter = 0L;
    KWordView *viewPtr;
    QPainter p;

    if ( !m_lstViews.isEmpty() )
    {
	for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
	{
	    if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
	    {
		if ( viewPtr->getGUI() )
		{
		    if ( !_painter )
		    {
			p.begin( viewPtr->getGUI()->getPaperWidget()->viewport() );
			viewPtr->getGUI()->getPaperWidget()->drawBorders( p, viewPtr->getGUI()->getPaperWidget()->rect(), back );
			p.end();
		    }
		    else
			viewPtr->getGUI()->getPaperWidget()->drawBorders( *_painter, viewPtr->getGUI()->getPaperWidget()->rect(), back );
		}
	    }
	}
    }
}

/*================================================================*/
void KWordDocument::updateAllStyles()
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	frameSet = getFrameSet( i );
	if ( frameSet->getFrameType() == FT_TEXT )
	    dynamic_cast<KWTextFrameSet*>( frameSet )->updateAllStyles();
    }

    updateAllViews( 0L );
    changedStyles.clear();
}

/*================================================================*/
void KWordDocument::insertPicture( QString _filename, KWPage *_paperWidget )
{
    _paperWidget->insertPictureAsChar( _filename );
}

/*================================================================*/
void KWordDocument::drawSelection( QPainter &_painter, int xOffset, int yOffset,
				   KWFormatContext *_selStart, KWFormatContext *_selEnd )
{
    if ( !_selStart )
	_selStart = &selStart;
    if ( !_selEnd )
	_selEnd = &selEnd;

    if ( !_selStart->getParag() || !selEnd.getParag() )
	return;

    _painter.save();
    RasterOp rop = _painter.rasterOp();

    _painter.setRasterOp( NotROP );
    _painter.setBrush( black );
    _painter.setPen( NoPen );

    KWFormatContext tmpFC2( this, _selStart->getFrameSet() - 1 );
    KWFormatContext tmpFC1( this, _selStart->getFrameSet() - 1 );

    if ( _selStart->getParag() == _selEnd->getParag() )
    {
	if ( _selStart->getTextPos() < _selEnd->getTextPos() )
	{
	    tmpFC1 = *_selStart;
	    tmpFC2 = *_selEnd;
	}
	else
	{
	    tmpFC1 = *_selEnd;
	    tmpFC2 = *_selStart;
	}
    }
    else
    {
	KWParag *parag = getFirstParag( _selStart->getFrameSet() - 1 );
	while ( parag )
	{
	    if ( parag == _selStart->getParag() )
	    {
		tmpFC1 = *_selStart;
		tmpFC2 = *_selEnd;
		break;
	    }
	    if ( parag == _selEnd->getParag() )
	    {
		tmpFC2 = *_selStart;
		tmpFC1 = *_selEnd;
		break;
	    }
	    parag = parag->getNext();
	}
    }

    if ( tmpFC1.getPTY() == tmpFC2.getPTY() )
	_painter.drawRect( tmpFC1.getPTPos() - xOffset, tmpFC2.getPTY() - yOffset,
			   tmpFC2.getPTPos() - tmpFC1.getPTPos(), tmpFC2.getLineHeight() );
    else
    {
	_painter.drawRect( tmpFC1.getPTPos() - xOffset, tmpFC1.getPTY() - yOffset,
			   tmpFC1.getPTLeft() + tmpFC1.getPTWidth() - tmpFC1.getPTPos(), tmpFC1.getLineHeight() );
	tmpFC1.makeNextLineLayout();

	while ( tmpFC1.getPTY() < tmpFC2.getPTY() || tmpFC1.getFrame() != tmpFC2.getFrame() )
	{
	    _painter.drawRect( tmpFC1.getPTLeft() - xOffset, tmpFC1.getPTY() - yOffset, tmpFC1.getPTWidth(), tmpFC1.getLineHeight() );
	    tmpFC1.makeNextLineLayout();
	}

	_painter.drawRect( tmpFC2.getPTLeft() - xOffset, tmpFC2.getPTY() - yOffset, tmpFC2.getPTPos() - tmpFC2.getPTLeft(), tmpFC2.getLineHeight() );
    }

    _painter.setRasterOp( rop );
    _painter.restore();
}

/*================================================================*/
void KWordDocument::deleteSelectedText( KWFormatContext *_fc )
{
    KWFormatContext tmpFC2( this, selStart.getFrameSet() - 1 );
    KWFormatContext tmpFC1( this, selStart.getFrameSet() - 1 );

    if ( selStart.getParag() == selEnd.getParag() )
    {
	if ( selStart.getTextPos() < selEnd.getTextPos() )
	{
	    tmpFC1 = selStart;
	    tmpFC2 = selEnd;
	}
	else
	{
	    tmpFC1 = selEnd;
	    tmpFC2 = selStart;
	}

	tmpFC1.getParag()->deleteText( tmpFC1.getTextPos(), tmpFC2.getTextPos() - tmpFC1.getTextPos() );

	_fc->setTextPos( tmpFC1.getTextPos() );

	KWParag *parag = 0;

	if ( _fc->getParag()->getTextLen() == 0 )
	{
	    if ( _fc->getParag()->getNext() )
	    {
		parag = _fc->getParag()->getNext();
		dynamic_cast<KWTextFrameSet*>( frames.at( _fc->getFrameSet() - 1 ) )->deleteParag( _fc->getParag() );
		_fc->init( parag );
	    }
	    else if ( _fc->getParag()->getPrev() )
	    {
		parag = _fc->getParag()->getPrev();
		dynamic_cast<KWTextFrameSet*>( frames.at( _fc->getFrameSet() - 1 ) )->deleteParag( _fc->getParag() );
		_fc->init( parag );
	    }
	}
	_fc->setTextPos( tmpFC1.getTextPos() );
    }
    else
    {
	KWParag *parag = getFirstParag( selStart.getFrameSet() - 1 ), *tmpParag = 0;
	while ( parag )
	{
	    if ( parag == selStart.getParag() )
	    {
		tmpFC1 = selStart;
		tmpFC2 = selEnd;
		break;
	    }
	    if ( parag == selEnd.getParag() )
	    {
		tmpFC2 = selStart;
		tmpFC1 = selEnd;
		break;
	    }
	    parag = parag->getNext();
	}
	tmpFC1.getParag()->deleteText( tmpFC1.getTextPos(), tmpFC1.getParag()->getTextLen() - tmpFC1.getTextPos() );
	parag = tmpFC1.getParag()->getNext();
	while ( parag && parag != tmpFC2.getParag() )
	{
	    tmpParag = parag->getNext();
	    dynamic_cast<KWTextFrameSet*>( frames.at( _fc->getFrameSet() - 1 ) )->deleteParag( parag );
	    parag = tmpParag;
	}
	tmpFC2.getParag()->deleteText( 0, tmpFC2.getTextPos() );

	dynamic_cast<KWTextFrameSet*>( frames.at( _fc->getFrameSet() - 1 ) )->joinParag( tmpFC1.getParag(), tmpFC2.getParag() );
	_fc->init( tmpFC1.getParag() );
	_fc->setTextPos( tmpFC1.getTextPos() );

	if ( _fc->getParag()->getTextLen() == 0 )
	{
	    if ( _fc->getParag()->getNext() )
	    {
		parag = _fc->getParag()->getNext();
		dynamic_cast<KWTextFrameSet*>( frames.at( _fc->getFrameSet() - 1 ) )->deleteParag( _fc->getParag() );
		_fc->init( parag );
	    }
	    else if ( _fc->getParag()->getPrev() )
	    {
		parag = _fc->getParag()->getPrev();
		dynamic_cast<KWTextFrameSet*>( frames.at( _fc->getFrameSet() - 1 ) )->deleteParag( _fc->getParag() );
		_fc->init( parag );
	    }
	}
	_fc->setTextPos( tmpFC1.getTextPos() );
    }
}

/*================================================================*/
void KWordDocument::copySelectedText()
{
    KWFormatContext tmpFC2( this, selStart.getFrameSet() - 1 );
    KWFormatContext tmpFC1( this, selStart.getFrameSet() - 1 );

    QString clipString = "";

    KWParag *firstParag = 0L, *parag2 = 0L, *parag3 = 0L;

    if ( selStart.getParag() == selEnd.getParag() )
    {
	if ( selStart.getTextPos() < selEnd.getTextPos() )
	{
	    tmpFC1 = selStart;
	    tmpFC2 = selEnd;
	}
	else
	{
	    tmpFC1 = selEnd;
	    tmpFC2 = selStart;
	}

	clipString = tmpFC1.getParag()->getKWString()->toString( tmpFC1.getTextPos(), tmpFC2.getTextPos() - tmpFC1.getTextPos() - 1 );
	firstParag = new KWParag( *tmpFC1.getParag() );
	firstParag->setPrev( 0L );
	firstParag->setNext( 0L );
	firstParag->deleteText( tmpFC2.getTextPos(), firstParag->getTextLen() - tmpFC2.getTextPos() );
	firstParag->deleteText( 0, tmpFC1.getTextPos() );
    }
    else
    {
	KWParag *parag = getFirstParag( selStart.getFrameSet() - 1 );
	while ( parag )
	{
	    if ( parag == selStart.getParag() )
	    {
		tmpFC1 = selStart;
		tmpFC2 = selEnd;
		break;
	    }
	    if ( parag == selEnd.getParag() )
	    {
		tmpFC2 = selStart;
		tmpFC1 = selEnd;
		break;
	    }
	    parag = parag->getNext();
	}

	clipString = tmpFC1.getParag()->getKWString()->toString( tmpFC1.getTextPos(), tmpFC1.getParag()->getTextLen() - tmpFC1.getTextPos() );
	firstParag = new KWParag( *tmpFC1.getParag() );
	firstParag->setPrev( 0L );
	firstParag->setNext( 0L );
	firstParag->deleteText( 0, tmpFC1.getTextPos() );
	parag3 = firstParag;

	parag = tmpFC1.getParag()->getNext();
	while ( parag && parag != tmpFC2.getParag() )
	{
	    parag2 = new KWParag( *parag );
	    parag3->setNext( parag2 );
	    parag2->setPrev( parag3 );
	    parag2->setNext( 0L );
	    parag3 = parag2;

	    clipString += "\n";
	    if ( parag->getTextLen() > 0 )
		clipString += parag->getKWString()->toString( 0, parag->getTextLen() );
	    else
		clipString += " ";
	    parag = parag->getNext();
	}
	clipString += "\n";
	if ( tmpFC2.getParag()->getTextLen() > 0 )
	{
	    clipString += tmpFC2.getParag()->getKWString()->toString( 0, tmpFC2.getTextPos() );
	    parag2 = new KWParag( *tmpFC2.getParag() );
	    parag2->setPrev( parag3 );
	    parag3->setNext( parag2 );
	    parag2->setNext( 0L );
	    parag2->deleteText( tmpFC2.getTextPos(), tmpFC2.getParag()->getTextLen() - tmpFC2.getTextPos() );
	}
    }

    QClipboard *cb = QApplication::clipboard();

    string clip_string;
    tostrstream out( clip_string );

    parag2 = firstParag;
    out << otag << "<PARAGRAPHS>" << endl;
    while ( parag2 )
    {
	out << otag << "<PARAGRAPH>" << endl;
	parag2->save( out );
	out << etag << "</PARAGRAPH>" << endl;
	parag2 = parag2->getNext();
    }
    out << etag << "</PARAGRAPHS>" << endl;

    KWordDrag *d = new KWordDrag;
    d->setPlain( clipString );
    d->setKWord( clip_string.c_str() );

    cb->setData( d );
}

/*================================================================*/
void KWordDocument::setFormat( KWFormat &_format )
{
    KWFormatContext tmpFC2( this, selStart.getFrameSet() - 1 );
    KWFormatContext tmpFC1( this, selStart.getFrameSet() - 1 );

    if ( selStart.getParag() == selEnd.getParag() )
    {
	if ( selStart.getTextPos() < selEnd.getTextPos() )
	{
	    tmpFC1 = selStart;
	    tmpFC2 = selEnd;
	}
	else
	{
	    tmpFC1 = selEnd;
	    tmpFC2 = selStart;
	}

	tmpFC1.getParag()->setFormat( tmpFC1.getTextPos(), tmpFC2.getTextPos() - tmpFC1.getTextPos(), _format );
    }
    else
    {
	KWParag *parag = getFirstParag( selStart.getFrameSet() - 1 );
	while ( parag )
	{
	    if ( parag == selStart.getParag() )
	    {
		tmpFC1 = selStart;
		tmpFC2 = selEnd;
		break;
	    }
	    if ( parag == selEnd.getParag() )
	    {
		tmpFC2 = selStart;
		tmpFC1 = selEnd;
		break;
	    }
	    parag = parag->getNext();
	}

	tmpFC1.getParag()->setFormat( tmpFC1.getTextPos(), tmpFC1.getParag()->getTextLen() - tmpFC1.getTextPos(), _format );
	parag = tmpFC1.getParag()->getNext();
	while ( parag && parag != tmpFC2.getParag() )
	{
	    if ( parag->getTextLen() > 0 )
		parag->setFormat( 0, parag->getTextLen(), _format );
	    parag = parag->getNext();
	}
	tmpFC2.getParag()->setFormat( 0, tmpFC2.getTextPos(), _format );
    }
}

/*================================================================*/
void KWordDocument::paste( KWFormatContext *_fc, QString _string, KWPage *_page, KWFormat *_format, const QString &_mime )
{
    QStrList strList;
    KWParag *firstParag = 0L, *parag = 0L, *parag2 = 0L, *calcParag = 0L;
    int index;

    if ( _string.isEmpty() ) return;

    if ( _mime == "text/plain" ) {     // ----------------- MIME type text/plain
	while ( true ) {
	    index = _string.find( '\n', 0 );
	    if ( index == -1 ) break;

	    if ( index > 0 && !_string.left( index ).simplifyWhiteSpace().isEmpty() )
		strList.append( QString( _string.left( index ) ) );
	    _string.remove( 0, index + 1 );
	}

	if ( !_string.isEmpty() && !_string.simplifyWhiteSpace().isEmpty() )
	    strList.append( QString( _string ) );
    } else if ( _mime == MIME_TYPE ) {     // -------------- MIME type application/x-kword
	istrstream in( _string.ascii() );
	if ( !in )
	    return;

	KOMLStreamFeed feed( in );
	KOMLParser parser( &feed );

	string tag;
	vector<KOMLAttrib> lst;
	string name;

	if ( !parser.open( "PARAGRAPHS", tag ) ) {
	    cerr << "Missing PARAGRAPHS" << endl;
	    return;
	}

	while ( parser.open( 0L, tag ) ) {
	    KOMLParser::parseTag( tag.c_str(), name, lst );

	    if ( name == "PARAGRAPH" ) {
		KOMLParser::parseTag( tag.c_str(), name, lst );
		vector<KOMLAttrib>::const_iterator it = lst.begin();
		for( ; it != lst.end(); it++ ) {
		}

		parag2 = new KWParag( dynamic_cast<KWTextFrameSet*>( getFrameSet( _fc->getFrameSet() - 1 ) ),
				      this, 0L, 0L, defaultParagLayout, false );
		parag2->load( parser, lst );
		
		KWParag::correctFormat( _fc->getParag(), parag2 );
		
		if ( !firstParag )
		    firstParag = parag2;
		parag2->setPrev( parag );
		if ( parag ) parag->setNext( parag2 );
		parag = parag2;
	    } else ;

	    if ( !parser.close( tag ) )
		return;

	}
    }

    if ( ( _mime == "text/plain" && !strList.isEmpty() ) || ( _mime == MIME_TYPE && firstParag ) ) {
	if ( ( _mime == "text/plain" && strList.count() == 1 ) || ( _mime == MIME_TYPE && !firstParag->getNext() ) ) {
	    // --------------- MIME: text/plain
	    if ( _mime == "text/plain" ) {
		QString str;
		unsigned int len;
		KWFormat *format = _format;
		if ( !format ) {
		    format = new KWFormat( this );
		    *format = *( (KWFormat*)_fc );
		}
		str = QString( strList.at( 0 ) );
		len = str.length();
		_fc->getParag()->insertText( _fc->getTextPos(), str );
		_fc->getParag()->setFormat( _fc->getTextPos(), len, *format );

		for ( unsigned int j = 0; j < len; j++ )
		    _fc->cursorGotoRight();
		delete format;
	    } else  { // ---------------- MIME: application/x-kword
		KWString *str = new KWString( this );
		*str = *firstParag->getKWString();
		_fc->getParag()->insertText( _fc->getTextPos(), str );

		for ( unsigned int j = 0; j < firstParag->getTextLen(); j++ )
		    _fc->cursorGotoRight();

		delete firstParag;
	    }
	} else if ( ( _mime == "text/plain" && strList.count() == 2 ) ||
		    ( _mime == "application/x-kword" && !firstParag->getNext()->getNext() ) ) {
	    if ( _mime == "text/plain" ) {
		QString str;
		unsigned int len;
		KWFormat *format = _format;
		if ( !format ) {
		    format = new KWFormat( this );
		    *format = *( (KWFormat*)_fc );
		}
		str = QString( strList.at( 0 ) );
		len = str.length();
		_fc->getParag()->insertText( _fc->getTextPos(), str );
		_fc->getParag()->setFormat( _fc->getTextPos(), len, *format );

		for ( unsigned int j = 0; j <= len; j++ )
		    _fc->cursorGotoRight();

		QKeyEvent ev(static_cast<QEvent::Type>(6) /*QEvent::KeyPress*/ ,Qt::Key_Return,13,0);
		_page->keyPressEvent( &ev );

		str = QString( strList.at( 1 ) );
		len = str.length();
		_fc->getParag()->insertText( _fc->getTextPos(), str );
		_fc->getParag()->setFormat( _fc->getTextPos(), len, *format );

		for ( unsigned int j = 0; j < len; j++ )
		    _fc->cursorGotoRight();
		delete format;
	    } else {
		KWString *str = new KWString( this );
		*str = *firstParag->getKWString();
		_fc->getParag()->insertText( _fc->getTextPos(), str );

		for ( unsigned int j = 0; j < firstParag->getTextLen(); j++ )
		    _fc->cursorGotoRight();

		QKeyEvent ev(static_cast<QEvent::Type>(6) /*QEvent::KeyPress*/ ,Qt::Key_Return,13,0);
		_page->keyPressEvent( &ev );

		KWString *str2 = new KWString( this );
		*str2 = *firstParag->getNext()->getKWString();
		_fc->getParag()->insertText( _fc->getTextPos(), str2 );

		for ( unsigned int j = 0; j < firstParag->getTextLen(); j++ )
		    _fc->cursorGotoRight();

		delete firstParag->getNext();
		delete firstParag;
	    }
	} else {
	    if ( _mime == "text/plain" ) {
		QString str;
		unsigned int len;
		KWFormat *format = _format;
		if ( !format ) {
		    format = new KWFormat( this );
		    *format = *( (KWFormat*)_fc );
		}
		str = QString( strList.at( 0 ) );
		len = str.length();
		_fc->getParag()->insertText( _fc->getTextPos(), str );
		_fc->getParag()->setFormat( _fc->getTextPos(), len, *format );

		for ( unsigned int j = 0; j < len; j++ )
		    _fc->cursorGotoRight();

		QKeyEvent ev(static_cast<QEvent::Type>(6) /*QEvent::KeyPress*/ ,Key_Return,13,0);
		_page->keyPressEvent( &ev );

		_fc->cursorGotoLeft();
		_fc->cursorGotoLeft();
		KWParag *p = _fc->getParag(), *next = _fc->getParag()->getNext();

		for ( unsigned int i = 1; i < strList.count(); i++ ) {
		    str = QString( strList.at( i ) );
		    len = str.length();
		    p = new KWParag( dynamic_cast<KWTextFrameSet*>( getFrameSet( _fc->getFrameSet() - 1 ) ), this,
				     p, 0L, defaultParagLayout );
		    if ( !calcParag )
			calcParag = p;
		    p->insertText( 0, str );
		    p->setFormat( 0, len, *format );
		}
		p->setNext( next );
		if ( next ) next->setPrev( p );
		delete format;
	    } else {
		KWString *str = new KWString( this );
		*str = *firstParag->getKWString();
		_fc->getParag()->insertText( _fc->getTextPos(), str );

		for ( unsigned int j = 0; j < firstParag->getTextLen(); j++ )
		    _fc->cursorGotoRight();
		QKeyEvent ev(static_cast<QEvent::Type>(6) /*QEvent::KeyPress*/ ,Qt::Key_Return,13,0);
		_page->keyPressEvent( &ev );

		_fc->cursorGotoLeft();
		_fc->cursorGotoLeft();

		KWParag *p = 0L, *prev = _fc->getParag(), *parag = firstParag->getNext(), *next = _fc->getParag()->getNext();

		while ( parag ) {
		    p = new KWParag( *parag );
		    if ( !calcParag )
			calcParag = p;
		    p->setPrev( prev );
		    prev->setNext( p );
		    p->setNext( 0L );
		    prev = p;
		    parag = parag->getNext();
		}
		p->setNext( next );
		if ( next ) next->setPrev( p );
	    }
	}
    }

    if ( !calcParag )
	calcParag = _fc->getParag();
    if ( calcParag->getPrev() )
	calcParag = calcParag->getPrev();

    recalcWholeText( calcParag, _fc->getFrameSet() - 1 );
}

/*================================================================*/
void KWordDocument::appendPage( unsigned int _page )
{
    pages++;
    QRect pageRect( 0, _page * getPTPaperHeight(), getPTPaperWidth(), getPTPaperHeight() );

    QList<KWFrame> frameList;
    frameList.setAutoDelete( false );

    KWFrameSet *frameSet = 0L;
    KWFrame *frame;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->getFrameType() != FT_TEXT || getFrameSet( i )->getFrameInfo() != FI_BODY ) continue;

	// don't add tables! A table cell ( frameset ) _must_ not have more than one frame!
	if ( getFrameSet( i )->getGroupManager() || !dynamic_cast<KWTextFrameSet*>( getFrameSet( i ) )->getAutoCreateNewFrame() ) continue;

	frameSet = getFrameSet( i );

	for ( unsigned int j = 0; j < frameSet->getNumFrames(); j++ )
	{
	    frame = frameSet->getFrame( j );
	    if ( frame->intersects( pageRect ) )
	    {
		KWFrame *frm = new KWFrame( frame->x(), frame->y() + getPTPaperHeight(), frame->width(), frame->height(), frame->getRunAround(),
					    frame->getRunAroundGap() );
		frm->setLeftBorder( frame->getLeftBorder2() );
		frm->setRightBorder( frame->getRightBorder2() );
		frm->setTopBorder( frame->getTopBorder2() );
		frm->setBottomBorder( frame->getBottomBorder2() );
		frm->setBLeft( frame->getBLeft() );
		frm->setBRight( frame->getBRight() );
		frm->setBTop( frame->getBTop() );
		frm->setBBottom( frame->getBBottom() );
		frm->setBackgroundColor( QBrush( frame->getBackgroundColor() ) );
		frameList.append( frm );
	    }
	}

	if ( !frameList.isEmpty() )
	{
	    for ( unsigned int k = 0; k < frameList.count(); k++ )
		frameSet->addFrame( frameList.at( k ) );
	}

	frameList.clear();
    }
    drawAllBorders();
    updateAllFrames();
    updateAllViewportSizes();

    if ( hasHeader() || hasFooter() )
	recalcFrames( false, true );
}

/*================================================================*/
int KWordDocument::getFrameSet( unsigned int mx, unsigned int my )
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	frameSet = getFrameSet( getNumFrameSets() - 1 - i );
	if ( frameSet->contains( mx, my ) )
	{
	    if ( !frameSet->isVisible() ) continue;
	    if ( isAHeader( frameSet->getFrameInfo() ) && !hasHeader() ||
		 isAFooter( frameSet->getFrameInfo() ) && !hasFooter() ||
		 isAWrongHeader( frameSet->getFrameInfo(), getHeaderType() ) ||
		 isAWrongFooter( frameSet->getFrameInfo(), getFooterType() ) )
		continue;
	    if ( frameSet->isRemoveableHeader() )
		continue;
	    return getNumFrameSets() - 1 - i;
	}
    }

    return -1;
}

/*================================================================*/
int KWordDocument::selectFrame( unsigned int mx, unsigned int my, bool simulate )
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	frameSet = getFrameSet( getNumFrameSets() - 1 - i );
	if ( frameSet->contains( mx, my ) )
	{
	    if ( !frameSet->isVisible() ) continue;
	    if ( isAHeader( frameSet->getFrameInfo() ) && !hasHeader() ||
		 isAFooter( frameSet->getFrameInfo() ) && !hasFooter() ||
		 isAWrongHeader( frameSet->getFrameInfo(), getHeaderType() ) ||
		 isAWrongFooter( frameSet->getFrameInfo(), getFooterType() ) )
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
void KWordDocument::deSelectFrame( unsigned int mx, unsigned int my )
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	frameSet = getFrameSet( getNumFrameSets() - 1 - i );
	if ( frameSet->contains( mx, my ) )
	    frameSet->deSelectFrame( mx, my );
    }
}

/*================================================================*/
void KWordDocument::deSelectAllFrames()
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	frameSet = getFrameSet( getNumFrameSets() - 1 - i );
	for ( unsigned int j = 0; j < frameSet->getNumFrames(); j++ )
	    frameSet->getFrame( j )->setSelected( false );
    }
}

/*================================================================*/
QCursor KWordDocument::getMouseCursor( unsigned int mx, unsigned int my )
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	frameSet = getFrameSet( getNumFrameSets() - 1 - i );
	if ( frameSet->contains( mx, my ) )
	{
	    if ( !frameSet->isVisible() ) continue;
	    if ( isAHeader( frameSet->getFrameInfo() ) && !hasHeader() ||
		 isAFooter( frameSet->getFrameInfo() ) && !hasFooter() ||
		 isAWrongHeader( frameSet->getFrameInfo(), getHeaderType() ) ||
		 isAWrongFooter( frameSet->getFrameInfo(), getFooterType() ) )
		continue;
	    if ( frameSet->isRemoveableHeader() )
		continue;
	    return frameSet->getMouseCursor( mx, my );
	}
    }

    return arrowCursor;
}

/*================================================================*/
KWFrame *KWordDocument::getFirstSelectedFrame()
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	frameSet = getFrameSet( getNumFrameSets() - 1 - i );
	for ( unsigned int j = 0; j < frameSet->getNumFrames(); j++ )
	{
	    if ( !frameSet->isVisible() ) continue;
	    if ( isAHeader( frameSet->getFrameInfo() ) && !hasHeader() ||
		 isAFooter( frameSet->getFrameInfo() ) && !hasFooter() ||
		 isAWrongHeader( frameSet->getFrameInfo(), getHeaderType() ) ||
		 isAWrongFooter( frameSet->getFrameInfo(), getFooterType() ) )
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
KWFrame *KWordDocument::getFirstSelectedFrame( int &_frameset )
{
    KWFrameSet *frameSet = 0L;
    _frameset = 0;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	_frameset = getNumFrameSets() - 1 - i;
	frameSet = getFrameSet( getNumFrameSets() - 1 - i );
	for ( unsigned int j = 0; j < frameSet->getNumFrames(); j++ )
	{
	    if ( !frameSet->isVisible() ) continue;
	    if ( isAHeader( frameSet->getFrameInfo() ) && !hasHeader() ||
		 isAFooter( frameSet->getFrameInfo() ) && !hasFooter() ||
		 isAWrongHeader( frameSet->getFrameInfo(), getHeaderType() ) ||
		 isAWrongFooter( frameSet->getFrameInfo(), getFooterType() ) )
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
KWFrameSet *KWordDocument::getFirstSelectedFrameSet()
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->hasSelectedFrame() )
	    return getFrameSet( i );
    }

    return 0L;
}

/*================================================================*/
void KWordDocument::print( QPainter *painter, QPrinter *printer,
			   float /*left_margin*/, float /*top_margin*/ )
{
    QList<KWFormatContext> fcList;
    fcList.setAutoDelete( true );

    KWFormatContext *fc = 0L;
    unsigned int i = 0, j = 0;

    for ( i = 0; i < frames.count(); i++ )
    {
	if ( frames.at( i )->getFrameType() == FT_TEXT )
	{
	    frames.at( i )->setCurrent( 0 );
	    fc = new KWFormatContext( this, i + 1 );
	    fc->init( dynamic_cast<KWTextFrameSet*>( frames.at( i ) )->getFirstParag(), true );
	    fcList.append( fc );
	}
    }

    for ( i = 0; i < static_cast<unsigned int>( pages ); i++ )
    {
	kapp->processEvents();
	QRect pageRect( 0, i * getPTPaperHeight(), getPTPaperWidth(), getPTPaperHeight() );
	unsigned int minus = 0;
	if ( i + 1 > static_cast<unsigned int>( printer->fromPage() ) ) printer->newPage();
	printBorders( *painter, 0, i * getPTPaperHeight(), getPTPaperWidth(), getPTPaperHeight() );
	for ( j = 0; j < frames.count(); j++ )
	{
	    if ( !getFrameSet( j )->isVisible() ) continue;
	    if ( isAHeader( getFrameSet( j )->getFrameInfo() ) && !hasHeader() ||
		 isAFooter( getFrameSet( j )->getFrameInfo() ) && !hasFooter() ||
		 isAWrongHeader( getFrameSet( j )->getFrameInfo(), getHeaderType() ) ||
		 isAWrongFooter( getFrameSet( j )->getFrameInfo(), getFooterType() ) )
		continue;
	    switch ( frames.at( j )->getFrameType() )
	    {
	    case FT_PICTURE:
	    {
		minus++;

		KWPictureFrameSet *picFS = dynamic_cast<KWPictureFrameSet*>( frames.at( j ) );
		KWFrame *frame = picFS->getFrame( 0 );
		if ( !frame->intersects( pageRect ) ) break;

		QSize _size = QSize( frame->width(), frame->height() );
		if ( _size != picFS->getImage()->size() )
		    picFS->setSize( _size );

		painter->drawImage( frame->x(), frame->y() - i * getPTPaperHeight(), *picFS->getImage() );
	    } break;
	    case FT_PART:
	    {
		minus++;

		KWPartFrameSet *partFS = dynamic_cast<KWPartFrameSet*>( getFrameSet( j ) );
		KWFrame *frame = partFS->getFrame( 0 );

		QPicture *pic = partFS->getPicture();

		painter->save();
		QRect r = painter->viewport();
		painter->setViewport( frame->x(), frame->y() - i * getPTPaperHeight(), r.width(), r.height() );
		if ( pic ) painter->drawPicture( *pic );
		painter->setViewport( r );
		painter->restore();
	    } break;
	    case FT_TEXT:
	    {
		bool bend = false;
		bool reinit = true;
		fc = fcList.at(j - minus);
		if ( frames.at( j )->getFrameInfo() != FI_BODY )
		{
		    if ( frames.at( j )->getFrameInfo() == FI_EVEN_HEADER || frames.at( j )->getFrameInfo() == FI_FIRST_HEADER ||
			 frames.at( j )->getFrameInfo() == FI_ODD_HEADER )
		    {
			if ( !hasHeader() ) continue;
			switch ( getHeaderType() )
			{
			case HF_SAME:
			{
			    if ( frames.at( j )->getFrameInfo() != FI_EVEN_HEADER ) continue;
			} break;
			case HF_EO_DIFF:
			{
			    if ( frames.at( j )->getFrameInfo() == FI_FIRST_HEADER ) continue;
			    if ( ( ( i + 1 ) / 2 ) * 2 == i + 1 && frames.at( j )->getFrameInfo() == FI_ODD_HEADER ) continue;
			    if ( ( ( i + 1 ) / 2 ) * 2 != i + 1 && frames.at( j )->getFrameInfo() == FI_EVEN_HEADER ) continue;
			} break;
			case HF_FIRST_DIFF:
			{
			    if ( i == 0 && frames.at( j )->getFrameInfo() != FI_FIRST_HEADER ) continue;
			    if ( i > 0 && frames.at( j )->getFrameInfo() != FI_EVEN_HEADER ) continue;
			} break;
			default: break;
			}
		    }
		    if ( frames.at( j )->getFrameInfo() == FI_EVEN_FOOTER || frames.at( j )->getFrameInfo() == FI_FIRST_FOOTER ||
			 frames.at( j )->getFrameInfo() == FI_ODD_FOOTER )
		    {
			if ( !hasFooter() ) continue;
			switch ( getFooterType() )
			{
			case HF_SAME:
			{
			    if ( frames.at( j )->getFrameInfo() != FI_EVEN_FOOTER ) continue;
			} break;
			case HF_EO_DIFF:
			{
			    if ( frames.at( j )->getFrameInfo() == FI_FIRST_FOOTER ) continue;
			    if ( ( ( i + 1 ) / 2 ) * 2 == i + 1 && frames.at( j )->getFrameInfo() == FI_ODD_FOOTER ) continue;
			    if ( ( ( i + 1 ) / 2 ) * 2 != i + 1 && frames.at( j )->getFrameInfo() == FI_EVEN_FOOTER ) continue;
			} break;
			case HF_FIRST_DIFF:
			{
			    if ( i == 0 && frames.at( j )->getFrameInfo() != FI_FIRST_FOOTER ) continue;
			    if ( i > 0 && frames.at( j )->getFrameInfo() != FI_EVEN_FOOTER ) continue;
			} break;
			default: break;
			}
		    }
		    fc->init( dynamic_cast<KWTextFrameSet*>( frames.at( j ) )->getFirstParag(), true,
			      frames.at( j )->getCurrent() + 1, i + 1 );
		    if ( static_cast<int>( frames.at( j )->getNumFrames() - 1 ) > static_cast<int>( frames.at( j )->getCurrent() ) )
			frames.at( j )->setCurrent( frames.at( j )->getCurrent() + 1 );
		    reinit = false;
		}
		if ( reinit )
		    fc->init( dynamic_cast<KWTextFrameSet*>( frames.at( fc->getFrameSet() - 1 ) )->getFirstParag(), true );
		while ( !bend )
		{
		    printLine( *fc, *painter, 0, i * getPTPaperHeight(), getPTPaperWidth(), getPTPaperHeight(), false, false );
		    bend = !fc->makeNextLineLayout();
		}
	    } break;
	    default: minus++; break;
	    }
	}
    }
}

/*================================================================*/
void KWordDocument::updateAllFrames()
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
	getFrameSet( i )->update();

    QList<KWFrame> _frames;
    QList<KWGroupManager> mgrs;
    QList<KWFrame> del;
    _frames.setAutoDelete( false );
    mgrs.setAutoDelete( false );
    del.setAutoDelete( true );
    unsigned int i = 0, j = 0;
    KWFrameSet *frameset = 0L;
    KWFrame *frame1, *frame2;
    KWFrame *framePtr = 0L;

    for ( i = 0; i < frames.count(); i++ )
    {
	frameset = frames.at( i );
	if ( isAHeader( frameset->getFrameInfo() ) || isAFooter( frameset->getFrameInfo() ) ) continue;
	if ( frameset->getGroupManager() )
	{
	    if ( mgrs.findRef( frameset->getGroupManager() ) == -1 )
	    {
		QRect r = frameset->getGroupManager()->getBoundingRect();
		KWFrame *frm = new KWFrame( r.x(), r.y(), r.width(), r.height() );
		_frames.append( frm );
		del.append( frm );
		mgrs.append( frameset->getGroupManager() );
	    }
	}
	else
	{
	    for ( j = 0; j < frameset->getNumFrames(); j++ )
		_frames.append( frameset->getFrame( j ) );
	}
    }

    for ( i = 0; i < _frames.count(); i++ )
    {
	framePtr = _frames.at( i );
	frame1 = _frames.at( i );
	_frames.at( i )->clearIntersects();

	for ( j = 0; j < _frames.count(); j++ )
	{
	    if ( i == j ) continue;

	    frame2 = _frames.at( j );
	    if ( frame1->intersects( QRect( frame2->x(), frame2->y(), frame2->width(), frame2->height() ) ) )
	    {
		QRect r = QRect( frame2->x(), frame2->y(), frame2->width(), frame2->height() );
		if ( r.left() > frame1->left() || r.top() > frame1->top() || r.right() < frame1->right() || r.bottom() < frame1->bottom() )
		{
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
}

/*================================================================*/
void KWordDocument::recalcWholeText( bool _cursor, bool _fast )
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() )
    {
	viewPtr = m_lstViews.first();
	if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
	    viewPtr->getGUI()->getPaperWidget()->recalcWholeText( _cursor, _fast );
    }
}

/*================================================================*/
void KWordDocument::recalcWholeText( KWParag *start, unsigned int fs )
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() )
    {
	viewPtr = m_lstViews.first();
	if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
	    viewPtr->getGUI()->getPaperWidget()->recalcWholeText( start, fs );
    }
}

/*================================================================*/
void KWordDocument::addStyleTemplate( KWParagLayout *pl )
{
    KWParagLayout* p;
    for ( p = paragLayoutList.first(); p != 0L; p = paragLayoutList.next() )
    {
	if ( p->getName() == pl->getName() )
	{
	    *p = *pl;
	    if ( p->getName() == "Standard" ) defaultParagLayout = p;
	    delete pl;
	    return;
	}
    }
    paragLayoutList.append( pl );
}
/*================================================================*/
void KWordDocument::setStyleChanged( QString _name )
{
    changedStyles.append( _name );
}

/*================================================================*/
bool KWordDocument::isStyleChanged( QString _name )
{
    return ( changedStyles.find( _name ) != -1 );
}

/*================================================================*/
void KWordDocument::setHeader( bool h )
{
    _header = h;
    if ( !_header )
    {
	KWordView *viewPtr = 0L;

	if ( !m_lstViews.isEmpty() )
	{
	    for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
		viewPtr->getGUI()->getPaperWidget()->footerHeaderDisappeared();
	}
    }

    recalcFrames( true, true );
    updateAllViews( 0L, true );
}

/*================================================================*/
void KWordDocument::setFooter( bool f )
{
    _footer = f;
    if ( !_footer )
    {
	KWordView *viewPtr = 0L;

	if ( !m_lstViews.isEmpty() )
	{
	    for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
		viewPtr->getGUI()->getPaperWidget()->footerHeaderDisappeared();
	}
    }

    recalcFrames( true, true );
    updateAllViews( 0L, true );
}

/*================================================================*/
bool KWordDocument::canResize( KWFrameSet *frameset, KWFrame *frame, int page, int diff )
{
    if ( diff < 0 ) return false;

    if ( !frameset->getGroupManager() )
    {
	// a normal frame _must_ not leave the page
	if ( frameset->getFrameInfo() == FI_BODY )
	{
	    if ( static_cast<int>( frame->bottom() + diff ) < static_cast<int>( ( page + 1 ) * getPTPaperHeight() ) )
		return true;
	    return false;
	}
	// headers and footers may always resize - ok this may lead to problems in strange situations, but let's ignore them :- )
	else
	{
	    // a footer has to moved a bit to the top before he gets resized
	    if ( isAFooter( frameset->getFrameInfo() ) )
		frame->moveTopLeft( QPoint( 0, frame->y() - diff ) );
	    return true;
	}
    }
    // tables may _always_ resize, because the group managers can add pages if needed
    else
	return true;

    return false;
}

/*================================================================*/
bool KWordDocument::getAutoCreateNewFrame()
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->hasSelectedFrame() && getFrameSet( i )->getFrameType() == FT_TEXT )
	    return dynamic_cast<KWTextFrameSet*>( getFrameSet( i ) )->getAutoCreateNewFrame();
    }

    return false;
}

/*================================================================*/
RunAround KWordDocument::getRunAround()
{
    KWFrame *frame = getFirstSelectedFrame();

    if ( frame ) return frame->getRunAround();

    return RA_NO;
}

/*================================================================*/
KWUnit KWordDocument::getRunAroundGap()
{
    KWFrame *frame = getFirstSelectedFrame();

    if ( frame ) return frame->getRunAroundGap();

    return false;
}

/*================================================================*/
void KWordDocument::setAutoCreateNewFrame( bool _auto )
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->hasSelectedFrame() && getFrameSet( i )->getFrameType() == FT_TEXT )
	    dynamic_cast<KWTextFrameSet*>( getFrameSet( i ) )->setAutoCreateNewFrame( _auto );
    }
}

/*================================================================*/
void KWordDocument::setRunAround( RunAround _ra )
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->hasSelectedFrame() )
	{
	    for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ )
	    {
		if ( getFrameSet( i )->getFrame( j )->isSelected() )
		    getFrameSet( i )->getFrame( j )->setRunAround( _ra );
	    }
	}
    }
}

/*================================================================*/
void KWordDocument::setRunAroundGap( KWUnit _gap )
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->hasSelectedFrame() )
	{
	    for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ )
	    {
		if ( getFrameSet( i )->getFrame( j )->isSelected() )
		    getFrameSet( i )->getFrame( j )->setRunAroundGap( _gap );
	    }
	}
    }
}
/*================================================================*/
void KWordDocument::getFrameMargins( KWUnit &l, KWUnit &r, KWUnit &t, KWUnit &b )
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->hasSelectedFrame() )
	{
	    for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ )
	    {
		if ( getFrameSet( i )->getFrame( j )->isSelected() )
		{
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
bool KWordDocument::isOnlyOneFrameSelected()
{
    int _selected = 0;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->hasSelectedFrame() )
	{
	    for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ )
	    {
		if ( getFrameSet( i )->getFrame( j )->isSelected() )
		    _selected++;
	    }
	}
    }

    return _selected == 1;
}

/*================================================================*/
KWFrameSet *KWordDocument::getFrameCoords( unsigned int &x, unsigned int &y, unsigned int &w, unsigned int &h, unsigned int &num )
{
    x = y = w = h = 0;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->hasSelectedFrame() )
	{
	    for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ )
	    {
		if ( getFrameSet( i )->getFrame( j )->isSelected() )
		{
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
void KWordDocument::setFrameMargins( KWUnit l, KWUnit r, KWUnit t, KWUnit b )
{
    QList<KWGroupManager> grpMgrs;
    grpMgrs.setAutoDelete( false );

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->hasSelectedFrame() )
	{
	    KWFrameSet *frameset = getFrameSet( i );
	    for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ )
	    {
		if ( getFrameSet( i )->getFrame( j )->isSelected() )
		{
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
}

/*================================================================*/
void KWordDocument::setFrameCoords( unsigned int x, unsigned int y, unsigned int w, unsigned int h )
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
    {
	if ( getFrameSet( i )->hasSelectedFrame() )
	{
	    for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ )
	    {
		if ( getFrameSet( i )->getFrame( j )->isSelected() && x + w < getPTPaperWidth() && y + h < pages * getPTPaperHeight() )
		{
		    if ( !getFrameSet( i )->getGroupManager() )
			getFrameSet( i )->getFrame( j )->setRect( x, y, w, h );
		}
	    }
	}
    }
}

/*================================================================*/
void KWordDocument::saveParagInUndoBuffer( QList<KWParag> /*parags*/,
					   int /*frameset*/,
					   KWFormatContext */*_fc*/ )
{
}

/*================================================================*/
void KWordDocument::saveParagInUndoBuffer( KWParag *parag, int frameset, KWFormatContext *_fc )
{
    KWTextChangeCommand *cmd = new KWTextChangeCommand( i18n( "Text changed" ), this, _fc, _fc->getTextPos() );
    cmd->addParag( *parag );

    if ( parag->getPrev() )
	cmd->setBefore( parag->getPrev()->getParagName() );
    else
	cmd->setBefore( "" );

    if ( parag->getNext() )
	cmd->setAfter( parag->getNext()->getParagName() );
    else
	cmd->setAfter( "" );

    cmd->setFrameSet( frameset );

    history.addCommand( cmd );
}

/*================================================================*/
void KWordDocument::undo()
{
    history.undo();
}

/*================================================================*/
void KWordDocument::redo()
{
    history.redo();
}

/*================================================================*/
void KWordDocument::slotUndoRedoChanged( QString undo, QString redo )
{
    KWordView *viewPtr = 0L;

    if ( !m_lstViews.isEmpty() )
    {
	for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
	{
	    viewPtr->changeUndo( undo, !undo.isEmpty() );
	    viewPtr->changeRedo( redo, !redo.isEmpty() );
	}
    }
}

/*================================================================*/
void KWordDocument::updateTableHeaders( QList<KWGroupManager> &grpMgrs )
{
    KWGroupManager *grpMgr;

    for ( grpMgr = grpMgrs.first(); grpMgr != 0; grpMgr = grpMgrs.next() )
	grpMgr->updateTempHeaders();
}

/*================================================================*/
long int KWordDocument::getPageNum( int bottom )
{
    int num = 0;
    while ( true )
    {
	if ( bottom < ( num + 1 ) * static_cast<int>( getPTPaperHeight() ) )
	    return num;

	num++;
    }

    return num;
}

/*================================================================*/
void KWordDocument::addImageRequest( const QString &filename, KWCharImage *img )
{
    imageRequests.insert( filename, img );
}

/*================================================================*/
void KWordDocument::addImageRequest( const QString &filename, KWPictureFrameSet *fs )
{
    imageRequests2.insert( filename, fs );
}
