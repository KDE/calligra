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
#include <qclipboard.h>
#include <kmessagebox.h>
#include <qdict.h>
#include <qpixmap.h>
#include <qfileinfo.h>

#include <kdebug.h>
#include <komlMime.h>
#include <koStream.h>
#include <komlParser.h>
#include <komlWriter.h>
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
#include "serialletter.h"
#include "contents.h"
#include "kword_factory.h"
#include <kdebug.h>

/******************************************************************/
/* Class: KWordChild                                              */
/******************************************************************/

/*================================================================*/
KWordChild::KWordChild( KWordDocument *_wdoc, const QRect& _rect, KoDocument *_doc, int diffx, int diffy )
    : KoDocumentChild( _wdoc, _doc, QRect( _rect.left() + diffx, _rect.top() + diffy, _rect.width(), _rect.height() ) )
{
}

/*================================================================*/
KWordChild::KWordChild( KWordDocument *_wdoc )
    : KoDocumentChild( _wdoc )
{
}

/*================================================================*/
KWordChild::~KWordChild()
{
}

KoDocument *KWordChild::hitTest( const QPoint &, const QWMatrix & )
{
  return 0L;
}

bool KWordChild::load( KOMLParser& parser, vector<KOMLAttrib>& _attribs )
{
  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end(); it++ )
  {
    if ( (*it).m_strName == "url" )
    {
      m_tmpURL = (*it).m_strValue.c_str();
    }
    else if ( (*it).m_strName == "mime" )
    {
      m_tmpMimeType = (*it).m_strValue.c_str();
    }
    else
      kdDebug(30003) << "Unknown attrib 'OBJECT:" << (*it).m_strName.c_str() << "'" << endl;
  }

  if ( m_tmpURL.isEmpty() )
  {
    kdDebug(30003) << "Empty 'url' attribute in OBJECT" << endl;
    return false;
  }
  else if ( m_tmpMimeType.isEmpty() )
  {
    kdDebug(30003) << "Empty 'mime' attribute in OBJECT" << endl;
    return false;
  }

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  bool brect = false;

  // RECT
  while( parser.open( 0L, tag ) )
  {
    parser.parseTag( tag.c_str(), name, lst );

    if ( name == "RECT" )
    {
      brect = true;
      m_tmpGeometry = tagToRect( lst );
      setGeometry( m_tmpGeometry );
    }
    else
      kdDebug(30003) << "Unknown tag '" << tag.c_str() << "' in OBJECT" << endl;

    if ( !parser.close( tag ) )
    {
      kdDebug(30003) << "ERR: Closing Child in OBJECT" << endl;
      return false;
    }
  }

  if ( !brect )
  {
    kdDebug(30003) << "Missing RECT in OBJECT" << endl;
    return false;
  }

  return true;
}

bool KWordChild::save( QTextStream& out )
{
  assert( document() );
  QString u = document()->url().url();
  QString mime = document()->mimeType();

  out << indent << "<OBJECT url=\"" << u.ascii() << "\" mime=\"" << mime.ascii() << "\">"
      << geometry() << "</OBJECT>" << endl;

  return true;
}


/******************************************************************/
/* Class: KWordDocument                                      */
/******************************************************************/

/*================================================================*/
KWordDocument::KWordDocument(QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ), cUserFont(0L), defaultUserFont(0L),
      formatCollection( this ), imageCollection( this ), selStart( this, 1 ), selEnd( this, 1 ),
      ret_pix( KWBarIcon( "return" ) ), unit( "mm" ), numParags( 0 ), footNoteManager( this ),
      autoFormat( this ), urlIntern(), pglChanged( TRUE )
{
    m_lstViews.setAutoDelete( FALSE );
    m_lstChildren.setAutoDelete( TRUE );
    varFormats.setAutoDelete(true);

    setInstance( KWordFactory::global() );

    hasSelection = FALSE;

    rastX = rastY = 10;

    setEmpty();
    setModified(FALSE);

    applyStyleTemplate = 0;
    applyStyleTemplate = applyStyleTemplate | U_FONT_FAMILY_ALL_SIZE | U_COLOR | U_BORDER | U_INDENT |
                         U_NUMBERING | U_ALIGN | U_TABS | U_SMART;
    _header = FALSE;
    _footer = FALSE;
    _needRedraw = FALSE;

    cParagLayout = 0L;
    cDisplayFont = 0L;

    userFontList.setAutoDelete( FALSE );
    displayFontList.setAutoDelete( FALSE );
    frames.setAutoDelete( TRUE );
    grpMgrs.setAutoDelete( TRUE );
    variables.setAutoDelete( FALSE );

    slDataBase = new KWSerialLetterDataBase( this );
    slRecordNum = -1;

    QObject::connect( &history, SIGNAL( undoRedoChanged( QString, QString ) ), this,
                      SLOT( slotUndoRedoChanged( QString, QString ) ) );
    connect( this, SIGNAL( childChanged( KoDocumentChild * ) ),
             this, SLOT( slotChildChanged( KoDocumentChild * ) ) );

    spellCheck = FALSE;
    contents = new KWContents( this );
    tmpShell = 0;

    connect( this, SIGNAL( completed() ),
             this, SLOT( slotDocumentLoaded() ) );
    zoom = 100;
    syntaxVersion = CURRENT_SYNTAX_VERSION;
}

/*================================================================*/
bool KWordDocument::initDoc()
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

    bool ok = FALSE;
    KoTemplateChooseDia::ReturnType ret = KoTemplateChooseDia::choose(
        KWordFactory::global(), _template, "application/x-kword", "*.kwd", i18n("KWord"),
        KoTemplateChooseDia::Everything, "kword_template");
    if ( ret == KoTemplateChooseDia::Template ) {
        QFileInfo fileInfo( _template );
        QString fileName( fileInfo.dirPath( TRUE ) + "/" + fileInfo.baseName() + ".kwt" );
        resetURL();
        ok = loadNativeFormat( fileName );
    } else if ( ret == KoTemplateChooseDia::File ) {
        KURL url;
        url.setPath( _template);
        ok = openURL( url );
    } else if ( ret == KoTemplateChooseDia::Empty ) {
        QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWordFactory::global() ) );
        resetURL();
        ok = loadNativeFormat( fileName );
    }
    setModified( FALSE );
    setEmpty();
    return ok;
}

void KWordDocument::initEmpty()
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

    QString fileName( locate( "kword_template", "Normal/.source/PlainText.kwt" , KWordFactory::global() ) );
    /*bool ok = */loadNativeFormat( fileName );
    resetURL();
    setModified( FALSE );
    setEmpty();
}


/*================================================================*/
void KWordDocument::setPageLayout( KoPageLayout _layout, KoColumns _cl, KoKWHeaderFooter _hf )
{
    pglChanged = TRUE;
    if ( processingType == WP ) {
        pageLayout = _layout;
        pageColumns = _cl;
        pageHeaderFooter = _hf;
    } else {
        pageLayout = _layout;
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

    ptColumnWidth = ( getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder() -
                      getPTColumnSpacing() * ( pageColumns.columns - 1 ) )
                    / pageColumns.columns;

    int firstHeadOffset = 0, evenHeadOffset = 0, oddHeadOffset = 0;
    int firstFootOffset = 0, evenFootOffset = 0, oddFootOffset = 0;
    KWTextFrameSet *firstHeader = 0L, *evenHeader = 0L, *oddHeader = 0L;
    KWTextFrameSet *firstFooter = 0L, *evenFooter = 0L, *oddFooter = 0L;
    if ( hasHeader() || hasFooter() ) {
        for ( unsigned int k = 0; k < getNumFrameSets(); k++ ) {
            if ( getFrameSet( k )->getFrameInfo() == FI_FIRST_HEADER && hasHeader() ) {
                firstHeader = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
                firstHeadOffset = static_cast<int>(pageHeaderFooter.ptHeaderBodySpacing +
                    getFrameSet( k )->getFrame( 0 )->height());
            }
            if ( getFrameSet( k )->getFrameInfo() == FI_EVEN_HEADER && hasHeader() ) {
                evenHeader = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
                evenHeadOffset = static_cast<int>(pageHeaderFooter.ptHeaderBodySpacing +
                    getFrameSet( k )->getFrame( 0 )->height());
            }
            if ( getFrameSet( k )->getFrameInfo() == FI_ODD_HEADER && hasHeader() ) {
                oddHeader = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
                oddHeadOffset = static_cast<int>(pageHeaderFooter.ptHeaderBodySpacing +
                    getFrameSet( k )->getFrame( 0 )->height());
            }

            if ( getFrameSet( k )->getFrameInfo() == FI_FIRST_FOOTER && hasFooter() ) {
                firstFooter = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
                firstFootOffset = static_cast<int>(pageHeaderFooter.ptFooterBodySpacing +
                    getFrameSet( k )->getFrame( 0 )->height());
            }
            if ( getFrameSet( k )->getFrameInfo() == FI_EVEN_FOOTER && hasFooter() ) {
                evenFooter = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
                evenFootOffset = static_cast<int>(pageHeaderFooter.ptFooterBodySpacing +
                    getFrameSet( k )->getFrame( 0 )->height());
            }
            if ( getFrameSet( k )->getFrameInfo() == FI_ODD_FOOTER && hasFooter() ) {
                oddFooter = dynamic_cast<KWTextFrameSet*>( getFrameSet( k ) );
                oddFootOffset = static_cast<int>(pageHeaderFooter.ptFooterBodySpacing +
                    getFrameSet( k )->getFrame( 0 )->height());
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

        for ( unsigned int j = 0;
              j < static_cast<unsigned int>( ceil( static_cast<double>( frms ) /
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
                    frameset->getFrame( j * pageColumns.columns + i )->setRect(
                        getPTLeftBorder() + i * ( ptColumnWidth + getPTColumnSpacing() ),
                        j * getPTPaperHeight() + getPTTopBorder() + headOffset,
                        ptColumnWidth,
                        getPTPaperHeight() - getPTTopBorder() - getPTBottomBorder() -
                         headOffset - footOffset );
                } else {
                    frameset->addFrame( new KWFrame(frameset, getPTLeftBorder() +
                         i * ( ptColumnWidth + getPTColumnSpacing() ),
                         j * getPTPaperHeight() + getPTTopBorder() + headOffset,
                         ptColumnWidth, getPTPaperHeight() -
                         getPTTopBorder() - getPTBottomBorder() -
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
            for ( int l = 0; l < pages; l++ ) {
                if ( l < static_cast<int>( evenHeader->getNumFrames() ) )
                    evenHeader->getFrame( l )->setRect( getPTLeftBorder(),
                                                        l * getPTPaperHeight() + getPTTopBorder(),
                                                        getPTPaperWidth() -
                                                        getPTLeftBorder() - getPTRightBorder(), h );
                else
                {
                    KWFrame *frame = new KWFrame( evenHeader,getPTLeftBorder(), l * getPTPaperHeight() + getPTTopBorder(),
                                                  getPTPaperWidth() - getPTLeftBorder() -
                                                  getPTRightBorder(), h );
                    evenHeader->addFrame( frame );
                }
            }
            if ( pages < static_cast<int>( evenHeader->getNumFrames() ) ) {
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
                    if ( static_cast<int>( oddHeader->getCurrent() ) <
                         static_cast<int>( oddHeader->getNumFrames() ) ) {
                        oddHeader->getFrame( oddHeader->getCurrent() )->setRect( getPTLeftBorder(),
                                                                                 l * getPTPaperHeight() +
                                                                                 getPTTopBorder(),
                                                                                 getPTPaperWidth() -
                                                                                 getPTLeftBorder() -
                                                                                 getPTRightBorder(), h2 );
                        oddHeader->setCurrent( oddHeader->getCurrent() + 1 );
                    } else {
                        KWFrame *frame = new KWFrame( oddHeader, getPTLeftBorder(), l * getPTPaperHeight() +
                                                      getPTTopBorder(),
                                                      getPTPaperWidth() - getPTLeftBorder() -
                                                      getPTRightBorder(), h2 );
                        oddHeader->addFrame( frame );
                    }
                } else {
                    even++;
                    if ( static_cast<int>( evenHeader->getCurrent() ) <
                         static_cast<int>( evenHeader->getNumFrames() ) ) {
                        evenHeader->getFrame( evenHeader->getCurrent() )->setRect( getPTLeftBorder(),
                                                                                   l * getPTPaperHeight() +
                                                                                   getPTTopBorder(),
                                                                                   getPTPaperWidth() -
                                                                                   getPTLeftBorder() -
                                                                                   getPTRightBorder(), h1 );
                        evenHeader->setCurrent( evenHeader->getCurrent() + 1 );
                    } else {
                        KWFrame *frame = new KWFrame( evenHeader,getPTLeftBorder(), l * getPTPaperHeight() +
                                                      getPTTopBorder(),
                                                      getPTPaperWidth() - getPTLeftBorder() -
                                                      getPTRightBorder(), h1 );
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
                                                        getPTPaperWidth() - getPTLeftBorder() -
                                                        getPTRightBorder(), h1 );
            }
        } break;
        case HF_FIRST_DIFF: {
            int h = firstHeader->getFrame( 0 )->height();
            firstHeader->getFrame( 0 )->setRect( getPTLeftBorder(), getPTTopBorder(),
                                                 getPTPaperWidth() - getPTLeftBorder() -
                                                 getPTRightBorder(), h );
            if ( firstHeader->getNumFrames() > 1 ) {
                int diff = firstHeader->getNumFrames() - 1;
                for ( ; diff > 0; diff-- )
                    firstHeader->delFrame( firstHeader->getNumFrames() - 1 );
            }
            h = evenHeader->getFrame( 0 )->height();
            for ( int l = 1; l < pages; l++ ) {
                if ( l - 1 < static_cast<int>( evenHeader->getNumFrames() ) )
                    evenHeader->getFrame( l - 1 )->setRect( getPTLeftBorder(), l * getPTPaperHeight() +
                                                            getPTTopBorder(),
                                                            getPTPaperWidth() - getPTLeftBorder() -
                                                            getPTRightBorder(), h );
                else {
                    KWFrame *frame = new KWFrame( evenHeader, getPTLeftBorder(), l * getPTPaperHeight() + getPTTopBorder(),
                                                  getPTPaperWidth() - getPTLeftBorder() -
                                                  getPTRightBorder(), h );
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
                                                        getPTPaperWidth() - getPTLeftBorder() -
                                                        getPTRightBorder(), h );
            }
        } break;
        }
    }

    if ( hasFooter() ) {
        switch ( getFooterType() ) {
        case HF_SAME: {
            int h = evenFooter->getFrame( 0 )->height();
            for ( int l = 0; l < pages; l++ ) {
                if ( l < static_cast<int>( evenFooter->getNumFrames() ) )
                    evenFooter->getFrame( l )->setRect( getPTLeftBorder(), ( l + 1 ) * getPTPaperHeight() -
                                                        getPTBottomBorder() - h,
                                                        getPTPaperWidth() - getPTLeftBorder() -
                                                        getPTRightBorder(), h );
                else {
                    KWFrame *frame = new KWFrame(evenFooter, getPTLeftBorder(), ( l + 1 ) * getPTPaperHeight() -
                                                  getPTBottomBorder() - h,
                                                  getPTPaperWidth() - getPTLeftBorder() -
                                                  getPTRightBorder(), h );
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
            for ( int l = 0; l < pages; l++ ) {
                if ( ( ( l + 1 ) / 2 ) * 2 != l + 1 ) {
                    odd++;
                    if ( static_cast<int>( oddFooter->getCurrent() ) < static_cast<int>( oddFooter->getNumFrames() ) ) {
                        oddFooter->getFrame( oddFooter->getCurrent() )->setRect( getPTLeftBorder(),
                                                                                 ( l + 1 )  *
                                                                                 getPTPaperHeight() -
                                                                                 getPTBottomBorder() - h2,
                                                                                 getPTPaperWidth() -
                                                                                 getPTLeftBorder() -
                                                                                 getPTRightBorder(), h2 );
                        oddFooter->setCurrent( oddFooter->getCurrent() + 1 );
                    } else {
                        KWFrame *frame = new KWFrame(oddFooter, getPTLeftBorder(),
                                                      ( l + 1 )  * getPTPaperHeight() -
                                                      getPTBottomBorder() - h2,
                                                      getPTPaperWidth() - getPTLeftBorder() -
                                                      getPTRightBorder(), h2 );
                        oddFooter->addFrame( frame );
                    }
                } else {
                    even++;
                    if ( static_cast<int>( evenFooter->getCurrent() ) <
                         static_cast<int>( evenFooter->getNumFrames() ) ) {
                        evenFooter->getFrame( evenFooter->getCurrent() )->setRect( getPTLeftBorder(),
                                                                                   ( l + 1 )  *
                                                                                   getPTPaperHeight() -
                                                                                   getPTBottomBorder() - h1,
                                                                                   getPTPaperWidth() -
                                                                                   getPTLeftBorder() -
                                                                                   getPTRightBorder(), h1 );
                        evenFooter->setCurrent( evenFooter->getCurrent() + 1 );
                    } else {
                        KWFrame *frame = new KWFrame(evenFooter, getPTLeftBorder(),
                                                      ( l + 1 )  * getPTPaperHeight() -
                                                      getPTBottomBorder() - h1,
                                                      getPTPaperWidth() - getPTLeftBorder() -
                                                      getPTRightBorder(), h1 );
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
                                                        getPTPaperWidth() - getPTLeftBorder() -
                                                        getPTRightBorder(), h1 );
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
                    evenFooter->getFrame( l - 1 )->setRect( getPTLeftBorder(), ( l + 1 ) *
                                                            getPTPaperHeight() - getPTBottomBorder() - h,
                                                            getPTPaperWidth() - getPTLeftBorder() -
                                                            getPTRightBorder(), h );
                else {
                    KWFrame *frame = new KWFrame(evenFooter, getPTLeftBorder(), ( l + 1 ) * getPTPaperHeight() -
                                                  getPTBottomBorder() - h,
                                                  getPTPaperWidth() - getPTLeftBorder() -
                                                  getPTRightBorder(), h );
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
                                                        getPTPaperWidth() - getPTLeftBorder() -
                                                        getPTRightBorder(), h );
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
    delete contents;
    delete cUserFont;
    delete slDataBase;
    paragLayoutList.setAutoDelete(true);
}

/*================================================================*/
bool KWordDocument::loadChildren( KoStore *_store )
{
    QListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        if ( !((KoDocumentChild*)it.current())->loadDocument( _store ) )
            return FALSE;
    }

    return TRUE;
}

/*================================================================*/
bool KWordDocument::loadXML( QIODevice *, const QDomDocument & doc )
{
    pixmapKeys.clear();
    pixmapNames.clear();
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

    // defaultUserFont = findUserFont( "times" ); // we don't like mem leaks that much (Werner)
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

    lay = new KWParagLayout( this );
    lay->setName( "Enumerated List" );
    lay->setFollowingParagLayout( "Enumerated List" );
    lay->setCounterType( KWParagLayout::CT_NUM );
    lay->setCounterDepth( 0 );
    lay->setStartCounter( 1 );
    lay->setCounterRightText( "." );
    lay->setNumberingType( KWParagLayout::NT_LIST );

    lay = new KWParagLayout( this );
    lay->setName( "Alphabetical List" );
    lay->setFollowingParagLayout( "Alphabetical List" );
    lay->setCounterType( KWParagLayout::CT_ALPHAB_L );
    lay->setCounterDepth( 0 );
    lay->setStartCounter( 1 );
    lay->setCounterRightText( " )" );
    lay->setNumberingType( KWParagLayout::NT_LIST );

    lay = new KWParagLayout( this );
    lay->setName( "Bullet List" );
    lay->setFollowingParagLayout( "Bullet List" );
    lay->setCounterType( KWParagLayout::CT_BULLET );
    lay->setCounterDepth( 0 );
    lay->setStartCounter( 1 );
    lay->setCounterRightText( "" );
    lay->setNumberingType( KWParagLayout::NT_LIST );

    f.setUserFont( findUserFont( "helvetica" ) );
    f.setWeight( 75 );
    f.setPTFontSize( 20 );
    lay = new KWParagLayout( this );
    lay->setName( "Contents Title" );
    lay->setFollowingParagLayout( "Standard" );
    lay->setCounterType( KWParagLayout::CT_NONE );
    lay->setCounterDepth( 0 );
    lay->setTopBorder( KWParagLayout::Border() );
    lay->setBottomBorder( KWParagLayout::Border() );
    lay->setFlow( KWParagLayout::CENTER );
    lay->setFormat( f );

    f.setUserFont( findUserFont( "helvetica" ) );
    f.setWeight( 75 );
    f.setPTFontSize( 16 );
    lay = new KWParagLayout( this );
    lay->setName( "Contents Head 1" );
    lay->setFollowingParagLayout( "Standard" );
    lay->setCounterType( KWParagLayout::CT_NONE );
    lay->setCounterDepth( 0 );
    lay->setFormat( f );

    f.setUserFont( findUserFont( "helvetica" ) );
    f.setWeight( 75 );
    f.setPTFontSize( 12 );
    lay = new KWParagLayout( this );
    lay->setName( "Contents Head 2" );
    lay->setFollowingParagLayout( "Standard" );
    lay->setCounterType( KWParagLayout::CT_NONE );
    lay->setCounterDepth( 0 );
    lay->setFormat( f );

    f.setUserFont( findUserFont( "helvetica" ) );
    f.setItalic( TRUE );
    f.setWeight( 50 );
    f.setPTFontSize( 12 );
    lay = new KWParagLayout( this );
    lay->setName( "Contents Head 3" );
    lay->setFollowingParagLayout( "Standard" );
    lay->setCounterType( KWParagLayout::CT_NONE );
    lay->setCounterDepth( 0 );
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

    pages = 1;

    string tag;
    vector<KOMLAttrib> lst;
    string name;

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


    KOMLParser parser( doc );

    // DOC
    if ( !parser.open( "DOC", tag ) ) {
        kdError(32001) << "Missing DOC" << endl;
        return FALSE;
    }

    parser.parseTag( tag.c_str(), name, lst );
    vector<KOMLAttrib>::const_iterator it = lst.begin();
    for( ; it != lst.end(); it++ ) {
        if ( ( *it ).m_strName == "mime" ) {
            if ( ( *it ).m_strValue != "application/x-kword" ) {
                kdError(32001) << "Unknown mime type " << ( *it ).m_strValue.c_str() << endl;
                return FALSE;
            }
        } else if ( ( *it ).m_strName == "url" ) {
            urlIntern = KURL( ( *it ).m_strValue.c_str() ).path();
        } else if ( ( *it ).m_strName == "syntaxVersion" ) {
            syntaxVersion = atoi( ( *it ).m_strValue.c_str() );
            if ( syntaxVersion < CURRENT_SYNTAX_VERSION )
            {
                // We can ignore the version mismatch for now. We will actually have to use it
                // when the syntax is extended in an incompatible manner.
            }
        }
    }

    // PAPER
    while ( parser.open( 0L, tag ) ) {
        parser.parseTag( tag.c_str(), name, lst );

        if ( name == "EMBEDDED" ) {
            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
            }
            KWordChild *ch = new KWordChild( this );
            KWPartFrameSet *fs = 0;
            QRect r;

            while ( parser.open( 0L, tag ) ) {
                parser.parseTag( tag.c_str(), name, lst );
                if ( name == "OBJECT" ) {
                    ch->load( parser, lst );
                    r = ch->geometry();
                    insertChild( ch );
                    fs = new KWPartFrameSet( this, ch );
                    frames.append( fs );
                } else if ( name == "SETTINGS" ) {
                    parser.parseTag( tag.c_str(), name, lst );
                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                    for( ; it != lst.end(); it++ ) {
                    }
                    fs->load( parser, lst );
                } else
                    kdError(32001) << "Unknown tag '" << tag.c_str() << "' in EMBEDDED" << endl;

                if ( !parser.close( tag ) ) {
                    kdError(32001) << "Closing " << tag.c_str() << endl;
                    return FALSE;
                }
            }
        } else if ( name == "PAPER" ) {
            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "format" )
                    __pgLayout.format = ( KoFormat )atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "orientation" )
                    __pgLayout.orientation = ( KoOrientation )atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "width" ) {
                    __pgLayout.mmWidth = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                    __pgLayout.ptWidth = MM_TO_POINT( __pgLayout.mmWidth );
                    __pgLayout.inchWidth = MM_TO_INCH( __pgLayout.mmWidth );
                } else if ( ( *it ).m_strName == "height" ) {
                    __pgLayout.mmHeight = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                    __pgLayout.ptHeight = MM_TO_POINT( __pgLayout.mmHeight );
                    __pgLayout.inchHeight = MM_TO_INCH( __pgLayout.mmHeight );
                } else if ( ( *it ).m_strName == "columns" )
                    __columns.columns = atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "columnspacing" ) {
                    __columns.ptColumnSpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                    __columns.mmColumnSpacing = POINT_TO_MM( __columns.ptColumnSpacing );
                    __columns.inchColumnSpacing = POINT_TO_INCH( __columns.ptColumnSpacing );
                } else if ( ( *it ).m_strName == "hType" )
                    __hf.header = static_cast<KoHFType>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "fType" )
                    __hf.footer = static_cast<KoHFType>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "spHeadBody" ) {
                    __hf.ptHeaderBodySpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                    __hf.mmHeaderBodySpacing = POINT_TO_MM( __hf.ptHeaderBodySpacing );
                    __hf.inchHeaderBodySpacing = POINT_TO_INCH( __hf.ptHeaderBodySpacing );
                } else if ( ( *it ).m_strName == "spFootBody" ) {
                    __hf.ptFooterBodySpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                    __hf.mmFooterBodySpacing = POINT_TO_MM( __hf.ptFooterBodySpacing );
                    __hf.inchFooterBodySpacing = POINT_TO_INCH( __hf.ptFooterBodySpacing );
                } else if ( ( *it ).m_strName == "ptWidth" )
                    __pgLayout.ptWidth = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "inchWidth" )
                    __pgLayout.inchWidth = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "mmWidth" )
                    __pgLayout.mmWidth = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "ptHeight" )
                    __pgLayout.ptHeight = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "inchHeight" )
                    __pgLayout.inchHeight = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "mmHeight" )
                    __pgLayout.mmHeight = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "ptHeadBody" )
                    __hf.ptHeaderBodySpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "inchHeadBody" )
                    __hf.inchHeaderBodySpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "mmHeadBody" )
                    __hf.mmHeaderBodySpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "ptFootBody" )
                    __hf.ptFooterBodySpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "inchFootBody" )
                    __hf.inchFooterBodySpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "mmFootBody" )
                    __hf.mmFooterBodySpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "mmColumnspc" )
                    __columns.mmColumnSpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "ptColumnspc" )
                    __columns.ptColumnSpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "inchColumnspc" )
                    __columns.inchColumnSpacing = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                else
                    kdError(32001) << "Unknown attrib 'PAPER:" << ( *it ).m_strName.c_str() << "'" << endl;
            }

            // PAPERBORDERS, HEAD, FOOT
            while ( parser.open( 0L, tag ) ) {
                parser.parseTag( tag.c_str(), name, lst );
                if ( name == "PAPERBORDERS" ) {
                    parser.parseTag( tag.c_str(), name, lst );
                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                    for( ; it != lst.end(); it++ ) {
                        if ( ( *it ).m_strName == "left" ) {
                            __pgLayout.mmLeft = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                            __pgLayout.ptLeft = MM_TO_POINT( __pgLayout.mmLeft );
                            __pgLayout.inchLeft = MM_TO_INCH( __pgLayout.mmLeft );
                        } else if ( ( *it ).m_strName == "top" ) {
                            __pgLayout.mmTop = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                            __pgLayout.ptTop = MM_TO_POINT( __pgLayout.mmTop );
                            __pgLayout.inchTop = MM_TO_INCH( __pgLayout.mmTop );
                        } else if ( ( *it ).m_strName == "right" ) {
                            __pgLayout.mmRight = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                            __pgLayout.ptRight = MM_TO_POINT( __pgLayout.mmRight );
                            __pgLayout.inchRight = MM_TO_INCH( __pgLayout.mmRight );
                        } else if ( ( *it ).m_strName == "bottom" ) {
                            __pgLayout.mmBottom = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                            __pgLayout.ptBottom = MM_TO_POINT( __pgLayout.mmBottom );
                            __pgLayout.inchBottom = MM_TO_INCH( __pgLayout.mmBottom );
                        } else if ( ( *it ).m_strName == "ptLeft" )
                            __pgLayout.ptLeft = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "inchLeft" )
                            __pgLayout.inchLeft = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "mmLeft" )
                            __pgLayout.mmLeft = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "ptRight" )
                            __pgLayout.ptRight = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "inchRight" )
                            __pgLayout.inchRight = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "mmRight" )
                            __pgLayout.mmRight = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "ptTop" )
                            __pgLayout.ptTop = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "inchTop" )
                            __pgLayout.inchTop = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "mmTop" )
                            __pgLayout.mmTop = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "ptBottom" )
                            __pgLayout.ptBottom = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "inchBottom" )
                            __pgLayout.inchBottom = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else if ( ( *it ).m_strName == "mmBottom" )
                            __pgLayout.mmBottom = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                        else
                            kdError(32001) << "Unknown attrib 'PAPERBORDERS:" << ( *it ).m_strName.c_str() << "'" << endl;
                    }
                }
                else
                    kdError(32001) << "Unknown tag '" << tag.c_str() << "' in PAPER" << endl;

                if ( !parser.close( tag ) ) {
                    kdError(32001) << "Closing " << tag.c_str() << endl;
                    return FALSE;
                }
            }

        }

        else if ( name == "ATTRIBUTES" ) {
            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "processing" )
                    processingType = static_cast<ProcessingType>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "standardpage" )
                                     ;
                else if ( ( *it ).m_strName == "hasHeader" )
                    _header = static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "hasFooter" )
                    _footer = static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) );
                else if ( ( *it ).m_strName == "unit" )
                    unit = correctQString( ( *it ).m_strValue.c_str() );
            }
        }

        else if ( name == "FOOTNOTEMGR" ) {
            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
            }
            footNoteManager.load( parser, lst );
        }

        else if ( name == "SERIALL" ) {
            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
            }
            slDataBase->load( parser, lst );
        }

        else if ( name == "FRAMESETS" ) {
            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
            }
            loadFrameSets( parser, lst );
        }

        else if ( name == "STYLES" ) {
            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
            }
            loadStyleTemplates( parser, lst );
        }

        else if ( name == "PIXMAPS" ) {
            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
            }

            while ( parser.open( 0L, tag ) ) {
                QString key;
                QString n = QString::null;

                parser.parseTag( tag.c_str(), name, lst );
                if ( name == "KEY" ) {
                    parser.parseTag( tag.c_str(), name, lst );
                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                    for( ; it != lst.end(); it++ ) {
                        if ( ( *it ).m_strName == "key" )
                            key = ( *it ).m_strValue.c_str();
                        else if ( ( *it ).m_strName == "name" )
                            n = ( *it ).m_strValue.c_str();
                        else
                            kdError(32001) << "Unknown attrib 'KEY:" << ( *it ).m_strName.c_str() << "'" << endl;
                    }
                    pixmapNames.append( n );
                    pixmapKeys.append( key );
                } else
                    kdError(32001) << "Unknown tag '" << tag.c_str() << "' in PIXMAPS" << endl;

                if ( !parser.close( tag ) ) {
                    kdError(32001) << "Closing " << tag.c_str() << endl;
                    return FALSE;
                }
            }
        }

        else if ( name == "CPARAGS" ) {
            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
            }

            while ( parser.open( 0L, tag ) ) {
                parser.parseTag( tag.c_str(), name, lst );
                if ( name == "PARAG" ) {
                    parser.parseTag( tag.c_str(), name, lst );
                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                    for( ; it != lst.end(); it++ ) {
                        if ( ( *it ).m_strName == "name" )
                            contents->addParagName( ( *it ).m_strValue.c_str() );
                    }
                } else
                    kdError(32001) << "Unknown tag '" << tag.c_str() << "' in CPARAGS" << endl;

                if ( !parser.close( tag ) ) {
                    kdError(32001) << "Closing " << tag.c_str() << endl;
                    return FALSE;
                }
            }
        }

        else
            kdError(32001) << "Unknown tag '" << tag.c_str() << "' in the DOCUMENT" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag.c_str() << endl;
            return FALSE;
        }
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

    bool _first_footer = FALSE, _even_footer = FALSE, _odd_footer = FALSE;
    bool _first_header = FALSE, _even_header = FALSE, _odd_header = FALSE;
    bool _footnotes = FALSE;

    for ( unsigned int k = 0; k < getNumFrameSets(); k++ ) {
        if ( getFrameSet( k )->getFrameInfo() == FI_FIRST_HEADER ) _first_header = TRUE;
        if ( getFrameSet( k )->getFrameInfo() == FI_EVEN_HEADER ) _odd_header = TRUE;
        if ( getFrameSet( k )->getFrameInfo() == FI_ODD_HEADER ) _even_header = TRUE;
        if ( getFrameSet( k )->getFrameInfo() == FI_FIRST_FOOTER ) _first_footer = TRUE;
        if ( getFrameSet( k )->getFrameInfo() == FI_EVEN_FOOTER ) _odd_footer = TRUE;
        if ( getFrameSet( k )->getFrameInfo() == FI_ODD_FOOTER ) _even_footer = TRUE;
        if ( getFrameSet( k )->getFrameInfo() == FI_FOOTNOTE ) _footnotes = TRUE;
    }

    // create defaults if they ware not in the input file.

    if ( !_first_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_FIRST_HEADER );
        KWFrame *frame = new KWFrame(fs, getPTLeftBorder(), getPTTopBorder(),
            getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_even_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_EVEN_HEADER );
        KWFrame *frame = new KWFrame(fs, getPTLeftBorder(), getPTTopBorder(),
            getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_odd_header ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_ODD_HEADER );
        KWFrame *frame = new KWFrame(fs, getPTLeftBorder(), getPTTopBorder(),
            getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_first_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_FIRST_FOOTER );
        KWFrame *frame = new KWFrame(fs, getPTLeftBorder(), getPTPaperHeight() -
            getPTTopBorder() - 20, getPTPaperWidth() - getPTLeftBorder() -
            getPTRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_even_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_EVEN_FOOTER );
        KWFrame *frame = new KWFrame(fs, getPTLeftBorder(), getPTPaperHeight() -
            getPTTopBorder() - 20, getPTPaperWidth() - getPTLeftBorder() -
            getPTRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_odd_footer ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_ODD_FOOTER );
        KWFrame *frame = new KWFrame(fs, getPTLeftBorder(), getPTPaperHeight() -
            getPTTopBorder() - 20, getPTPaperWidth() - getPTLeftBorder() -
            getPTRightBorder(), 20 );
        frame->setFrameBehaviour(Ignore);
        fs->addFrame( frame );
        frames.append( fs );
    }

    if ( !_footnotes ) {
        KWTextFrameSet *fs = new KWTextFrameSet( this );
        fs->setFrameInfo( FI_FOOTNOTE );
        fs->setName( "Footnotes" );

        for ( int i = 0; i < pages; i++ ) {
            KWFrame *frame = new KWFrame(fs, getPTLeftBorder(),
                i * getPTPaperHeight() + getPTPaperHeight() - getPTTopBorder() - 20,
                getPTPaperWidth() - getPTLeftBorder() - getPTRightBorder(), 20 );
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

    KWordChild *ch = 0L;
    for ( ch = m_lstChildren.first(); ch != 0; ch = m_lstChildren.next() ) {
        KWPartFrameSet *frameset = new KWPartFrameSet( this, ch );
        frameset->setName( i18n( "PartFrameset %1" ).arg( frames.count() + 1 ) );
        QRect r = ch->geometry();
        KWFrame *frame = new KWFrame(frameset, r.x(), r.y(), r.width(), r.height() );
        frameset->addFrame( frame );
        addFrameSet( frameset );
        emit sig_insertObject( ch, frameset );
    }

    if ( contents->numParags() > 0 ) {
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

    autoFormat.addAutoFormatEntry( KWAutoFormatEntry("(C)", "©" ) );
    autoFormat.addAutoFormatEntry( KWAutoFormatEntry("(c)", "©" ) );
    autoFormat.addAutoFormatEntry( KWAutoFormatEntry("(R)", "®" ) );
    autoFormat.addAutoFormatEntry( KWAutoFormatEntry("(r)", "®" ) );

    // do some sanity checking on document.
    for (int i = getNumFrameSets()-1; i>-1; i--) {
        if(! getFrameSet(i)) {
            kdDebug () << "frameset " << i << " is NULL!!" << endl;
            frames.remove(i);
        } else if(! getFrameSet(i)->getFrame(0)) {
            kdDebug () << "frameset " << i << " has no frames" << endl;
            delFrameSet(getFrameSet(i));
        }
    }
    for (int i = getNumGroupManagers()-1; i>-1; i--) {
        if(! getGroupManager(i)) {
            kdDebug () << "GroupManager " << i << " is NULL!!" << endl;
            grpMgrs.remove(i);
        } else if(! getGroupManager(i)->getFrameSet(0,0)) {
            kdDebug () << "GroupManager " << i << " has no frames" << endl;
            delGroupManager(getGroupManager(i));
        } else {
            KWGroupManager *gm = getGroupManager(i);
            for (int j=0; j < gm->getNumCells() ; j++) {
                KWFrame *frame = gm->getCell(j)->frameSet->getFrame(0);
                if(frame->getFrameBehaviour()==AutoCreateNewFrame) {
                    frame->setFrameBehaviour(AutoExtendFrame);
                    kdDebug() << "Table cell property frameBehaviour was incorrect; fixed" << endl;
                }
                if(frame->getNewFrameBehaviour()!=NoFollowup) {
                    kdDebug() << "Table cell property newFrameBehaviour was incorrect; fixed" << endl;
                    frame->setNewFrameBehaviour(NoFollowup);
                }
            }
        }
    }

    return TRUE;
}

/*================================================================*/
void KWordDocument::loadStyleTemplates( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
    string tag;
    string name;

    while ( parser.open( 0L, tag ) ) {
        parser.parseTag( tag.c_str(), name, lst );

        if ( name == "STYLE" ) {
            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
            }
            KWParagLayout *pl = new KWParagLayout( this, FALSE );
            pl->load( parser, lst );
            addStyleTemplate( pl );
        } else
            kdError(32001) << "Unknown tag '" << tag.c_str() << "' in STYLES" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag.c_str() << endl;
            return;
        }
    }
}

/*================================================================*/
void KWordDocument::loadFrameSets( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
    string tag;
    string name;

    FrameInfo frameInfo = FI_BODY;
    QString _name = "";
    int _row = 0, _col = 0, _rows = 1, _cols = 1;
    bool _visible = TRUE;

    while ( parser.open( 0L, tag ) ) {
        parser.parseTag( tag.c_str(), name, lst );

        // paragraph
        if ( name == "FRAMESET" ) {
            FrameType frameType = FT_BASE;
            _name = "";
            _row = _col = 0, _rows = 1, _cols = 1;
            _visible = TRUE;
            bool removeable = FALSE;
            QString fsname;

            parser.parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ ) {
                if ( ( *it ).m_strName == "frameType" )
                    frameType = static_cast<FrameType>( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "frameInfo" )
                    frameInfo = static_cast<FrameInfo>( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "grpMgr" )
                    _name = correctQString( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "row" )
                    _row = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "col" )
                    _col = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "removeable" )
                    removeable = static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "rows" )
                    _rows = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "cols" )
                    _cols = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "visible" )
                    _visible = static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "name" )
                    fsname = ( *it ).m_strValue.c_str();
            }

            if ( fsname.isEmpty() ) {
            if(frameInfo!=FI_BODY)
                fsname = i18n( "TextFrameset %1" ).arg( frames.count() + 1 );
            else
                fsname = i18n( "Frameset %1" ).arg( frames.count() + 1 );
        }

            switch ( frameType ) {
            case FT_TEXT: {
                KWTextFrameSet *frame = new KWTextFrameSet( this );
                frame->setVisible( _visible );
                frame->setName( fsname );
                frame->load( parser, lst );
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
            case FT_FORMULA: {
                KWFormulaFrameSet *frame = new KWFormulaFrameSet( this );
                frame->setName( fsname );
                frame->load( parser, lst );
                frame->setFrameInfo( frameInfo );
                frames.append( frame );
            } break;
            default: break;
            }
        } else
            kdError(32001) << "Unknown tag '" << tag.c_str() << "' in FRAMESETS" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag.c_str() << endl;
            return;
        }
    }
}

/*===================================================================*/
bool KWordDocument::completeLoading( KoStore *_store )
{
    if ( _store ) {
        QString str = urlIntern.isEmpty() ? KURL( url() ).path().latin1() : urlIntern.latin1();

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

    return TRUE;
}

/*================================================================*/
bool KWordDocument::saveToStream( QIODevice * dev )
{
    QTextStream out( dev );
    // For now, we always save documents using the current syntax version.

    syntaxVersion = CURRENT_SYNTAX_VERSION;
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    //out << "<!DOCTYPE DOC SYSTEM \"" << kapp->kde_datadir() << "/kword/dtd/kword.dtd\"/>" << endl;
    out << otag << "<DOC author=\"" << "Reginald Stadlbauer and Torben Weis" << "\" email=\""
        << "reggie@kde.org and weis@kde.org"
        << "\" editor=\"" << "KWord" << "\" mime=\"" << "application/x-kword"
        << "\" syntaxVersion=\"" << syntaxVersion << "\">"
        << endl;
    out << otag << "<PAPER format=\"" << static_cast<int>( pageLayout.format ) << "\" ptWidth=\"" << pageLayout.ptWidth
        << "\" ptHeight=\"" << pageLayout.ptHeight
        << "\" mmWidth =\"" << pageLayout.mmWidth << "\" mmHeight=\"" << pageLayout.mmHeight
        << "\" inchWidth =\"" << pageLayout.inchWidth << "\" inchHeight=\"" << pageLayout.inchHeight
        << "\" orientation=\"" << static_cast<int>( pageLayout.orientation )
        << "\" columns=\"" << pageColumns.columns << "\" ptColumnspc=\"" << pageColumns.ptColumnSpacing
        << "\" mmColumnspc=\"" << pageColumns.mmColumnSpacing << "\" inchColumnspc=\"" << pageColumns.inchColumnSpacing
        << "\" hType=\"" << static_cast<int>( pageHeaderFooter.header ) << "\" fType=\""
        << static_cast<int>( pageHeaderFooter.footer )
        << "\" ptHeadBody=\"" << pageHeaderFooter.ptHeaderBodySpacing << "\" ptFootBody=\""
        << pageHeaderFooter.ptFooterBodySpacing
        << "\" mmHeadBody=\"" << pageHeaderFooter.mmHeaderBodySpacing << "\" mmFootBody=\""
        << pageHeaderFooter.mmFooterBodySpacing
        << "\" inchHeadBody=\"" << pageHeaderFooter.inchHeaderBodySpacing << "\" inchFootBody=\""
        << pageHeaderFooter.inchFooterBodySpacing
        << "\">" << endl;
    out << indent << "<PAPERBORDERS mmLeft=\"" << pageLayout.mmLeft << "\" mmTop=\"" << pageLayout.mmTop
        << "\" mmRight=\""
        << pageLayout.mmRight << "\" mmBottom=\"" << pageLayout.mmBottom
        << "\" ptLeft=\"" << pageLayout.ptLeft << "\" ptTop=\"" << pageLayout.ptTop << "\" ptRight=\""
        << pageLayout.ptRight << "\" ptBottom=\"" << pageLayout.ptBottom
        << "\" inchLeft=\"" << pageLayout.inchLeft << "\" inchTop=\"" << pageLayout.inchTop << "\" inchRight=\""
        << pageLayout.inchRight << "\" inchBottom=\"" << pageLayout.inchBottom << "\"/>" << endl;
    out << etag << "</PAPER>" << endl;
    out << indent << "<ATTRIBUTES processing=\"" << static_cast<int>( processingType ) << "\" standardpage=\"" << 1
        << "\" hasHeader=\"" << hasHeader() << "\" hasFooter=\"" << hasFooter()
        << "\" unit=\"" << correctQString( getUnit() ).latin1() << "\"/>" << endl;

    out << otag << "<FOOTNOTEMGR>" << endl;
    footNoteManager.save( out );
    out << etag << "</FOOTNOTEMGR>" << endl;

    out << otag << "<FRAMESETS>" << endl;

    KWFrameSet *frameSet = 0L;
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        frameSet = getFrameSet( i );
        if ( frameSet->getFrameType() != FT_PART )
            frameSet->save( out );
    }

    out << etag << "</FRAMESETS>" << endl;

    out << otag << "<STYLES>" << endl;
    for ( unsigned int j = 0; j < paragLayoutList.count(); j++ ) {
        out << otag << "<STYLE>" << endl;
        paragLayoutList.at( j )->save( out );
        out << etag << "</STYLE>" << endl;
    }
    out << etag << "</STYLES>" << endl;

    out << otag << "<PIXMAPS>" << endl;

    QDictIterator<KWImage> it = imageCollection.iterator();
    QStringList keys, images;
    int i = 0;
    for ( ; it.current(); ++it ) {
        if ( keys.contains( it.currentKey() ) || images.contains( it.current()->getFilename() ) )
            continue;
        QString format = QFileInfo( it.current()->getFilename() ).extension().upper();
        if ( format == "JPG" )
            format = "JPEG";
        if ( QImage::outputFormats().find( QFile::encodeName(format) ) == -1 )
            format = "BMP";
        QString pictureName = QString( "pictures/picture%1.%2" ).arg( ++i ).arg( format.lower() );
        if ( !isStoredExtern() )
          pictureName.prepend( url().url() + "/" );

        out << indent << "<KEY key=\"" << it.current()->getFilename().latin1()
            << "\" name=\"" << pictureName.latin1()
            << "\"/>" << endl;
        keys.append( it.currentKey() );
        images.append( it.current()->getFilename() );
    }
    out << etag << "</PIXMAPS>" << endl;

    if ( contents->hasContents() ) {
        out << otag << "<CPARAGS>" << endl;
        QStringList::Iterator it = contents->begin();
        for ( ; it != contents->ending(); ++it )
            out << indent << "<PARAG name=\"" << correctQString( *it ).latin1() << "\"/>" << endl;
        out << etag << "</CPARAGS>" << endl;
    }

    out << otag << "<SERIALL>" << endl;
    slDataBase->save( out );
    out << etag << "</SERIALL>" << endl;


    // Write "OBJECT" tag for every child
    QListIterator<KoDocumentChild> chl( children() );
    for( ; chl.current(); ++chl ) {
        out << otag << "<EMBEDDED>" << endl;

        KWordChild* curr = (KWordChild*)chl.current();

        curr->save( out );

        out << otag << "<SETTINGS>" << endl;
        for ( unsigned int i = 0; i < frames.count(); i++ ) {
            KWFrameSet *fs = frames.at( i );
            if ( fs->getFrameType() == FT_PART &&
                 dynamic_cast<KWPartFrameSet*>( fs )->getChild() == curr )
                fs->save( out );
        }
        out << etag << "</SETTINGS> "<< endl;

        out << etag << "</EMBEDDED>" << endl;
    }

    out << etag << "</DOC>" << endl;

    return TRUE;
}

/*==============================================================*/
bool KWordDocument::completeSaving( KoStore *_store )
{
    if ( !_store )
        return TRUE;

    QString u = KURL( url() ).path().latin1();
    QDictIterator<KWImage> it = imageCollection.iterator();

    QStringList keys, images;
    int i = 0;

    for( ; it.current(); ++it ) {
        if ( keys.contains( it.currentKey() ) || images.contains( it.current()->getFilename() ) )
            continue;

        QString format = QFileInfo( it.current()->getFilename() ).extension().upper();
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
            io.setImage( *it.current() );
            io.setFormat( QFile::encodeName(format) );
            io.write();
            _store->close();
        }
        keys.append( it.currentKey() );
        images.append( it.current()->getFilename() );
    }

    return TRUE;
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
bool KWordDocument::saveChildren( KoStore *_store, const QString &_path )
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
void KWordDocument::addView( KoView *_view )
{
    qDebug( "addView" );
    m_lstViews.append( (KWordView*)_view );
    KoDocument::addView( _view );
}

/*================================================================*/
void KWordDocument::removeView( KWordView *_view )
{
    m_lstViews.setAutoDelete( FALSE );
    m_lstViews.removeRef( _view );
    m_lstViews.setAutoDelete( TRUE );
}

/*================================================================*/
KoMainWindow* KWordDocument::createShell()
{
    KoMainWindow* shell = new KWordShell;
    shell->show();

    return shell;
}

/*================================================================*/
KoView* KWordDocument::createViewInstance( QWidget* parent, const char* name )
{
    return new KWordView( parent, name, this );
}

/*================================================================*/
void KWordDocument::paintContent( QPainter& /*painter*/, const QRect& /*rect*/, bool /*transparent*/ )
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

//     KWordView *view = (KWordView*)tmpShell->rootView();
//     KWPage *page = view->getGUI()->getPaperWidget();

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
void KWordDocument::insertObject( const QRect& _rect, KoDocumentEntry& _e, int _diffx, int _diffy )
{

    KoDocument* doc = _e.createDoc( this );
    if ( !doc || !doc->initDoc() ) {
        KMessageBox::error( 0, i18n( "Due to an internal error, KWord could not\n"
                                     "perform the requested action."));
        return;
    }

    KWordChild* ch = new KWordChild( this, _rect, doc, _diffx, _diffy );

    insertChild( ch );
    setModified( TRUE );

    KWPartFrameSet *frameset = new KWPartFrameSet( this, ch );
    KWFrame *frame = new KWFrame(frameset, _rect.x() + _diffx, _rect.y() + _diffy, _rect.width(), _rect.height() );
    frameset->addFrame( frame );
    addFrameSet( frameset );

    emit sig_insertObject( ch, frameset );

    updateAllViews( 0 );
}

/*================================================================*/
void KWordDocument::slotChildChanged( KoDocumentChild *child )
{
    setModified(TRUE);

    // Problem: we have to find the frame that contains this child.
    // We could do a lot better if we could connect this signal's changed() signal
    // directly in the appropriate KWPartFrameSet.
    // But framesets are not a QObject. Should it ?
    // (David)
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

#if 0
/*================================================================*/
QListIterator<KWordChild> KWordDocument::childIterator()
{
    return QListIterator<KWordChild> ( m_lstChildren );
}
#endif

/*================================================================*/
void KWordDocument::draw( QPaintDevice *, long int, long int, float )
{
}

/*================================================================*/
QPen KWordDocument::setBorderPen( KWParagLayout::Border _brd )
{
    QPen pen( black, 1, SolidLine );

    pen.setWidth( _brd.ptWidth );
    pen.setColor( _brd.color );

    switch ( _brd.style ) {
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
    if ( cUserFont ) {
        if ( cUserFont->getFontName() == _userfontname ) return cUserFont;
    }

    KWUserFont* font = 0L;
    for ( font = userFontList.first(); font != 0L; font = userFontList.next() ) {
        if ( font->getFontName() == _userfontname ) {
            cUserFont = font;
            return font;
        }
    }

    font = new KWUserFont( this, _userfontname );
    cUserFont = font;

    return font;
}

/*================================================================*/
KWDisplayFont* KWordDocument::findDisplayFont( KWUserFont* _font, unsigned int _size, int _weight,
                                               bool _italic, bool _underline )
{
    if ( cDisplayFont ) {
        if ( cDisplayFont->getUserFont()->getFontName() == _font->getFontName() && cDisplayFont->pointSize() == _size &&
             cDisplayFont->weight() == _weight && cDisplayFont->italic() == _italic &&
             cDisplayFont->underline() == _underline )
            return cDisplayFont;
    }

    KWDisplayFont* font = 0L;
    for ( font = displayFontList.first(); font != 0L; font = displayFontList.next() ) {
        if ( font->getUserFont()->getFontName() == _font->getFontName() && font->pointSize() == _size &&
             font->weight() == _weight && font->italic() == _italic && font->underline() == _underline ) {
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
    if ( cParagLayout ) {
        if ( cParagLayout->getName() == _name ) return cParagLayout;
    }

    KWParagLayout* p;
    for ( p = paragLayoutList.first(); p != 0L; p = paragLayoutList.next() ) {
        if ( p->getName() == _name ) {
            cParagLayout = p;
            return p;
        }
    }

    qWarning( "Parag Layout: '%s` is unknown, using default parag layout", _name.latin1() );
    return defaultParagLayout;
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
    while ( p ) {
        if ( p->getEndPage() == _page || p->getStartPage() == _page || ( p->getEndPage() > _page &&
                                                                         p->getStartPage() < _page ) )
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
    while ( p ) {
        if ( p->getPTYEnd() >= _ypos || p->getPTYStart() >= _ypos || ( p->getPTYEnd() >= _ypos &&
                                                                       p->getPTYStart() <= _ypos )
             || ( p->getPTYEnd() <= _ypos && p->getPTYStart() <= _ypos && p->getPTYStart() > p->getPTYEnd() &&
                  ( p->getEndPage() == _page ||
                    p->getStartPage() == _page || ( p->getEndPage() >
                                                    _page && p->getStartPage() < _page ) ) ) )
            return p;
        p = p->getNext();
    }

    return 0L;
}

/*================================================================*/
bool KWordDocument::printLine( KWFormatContext &_fc, QPainter &_painter, int xOffset, int yOffset, int _w, int _h,
                               bool _viewFormattingChars, bool _drawVarBack, int dbx, int dby,
                               int dbw, int dbh, const QBrush &dbback )
{
    if ( !getFrameSet( _fc.getFrameSet() - 1 )->isVisible() )
        return FALSE;

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

    if ( static_cast<int>( _fc.getPTY() + _fc.getLineHeight() ) >
         getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->bottom() )
        cr = QRegion( 0, 0, 0, 0 );

    QRegion visible( 0, 0, _w, _h );

    if ( _painter.hasClipping() )
        cr = _painter.clipRegion().intersect( cr );

    if ( cr.intersect( visible ).isEmpty() ) {
        _painter.restore();
        return FALSE;
    }

    _painter.setClipRegion( cr );

    // paint it character for character. Provisionally! !!HACK!!
    _fc.cursorGotoLineStart();

    if ( dbx != -1 )
        _painter.fillRect( dbx, dby, dbw, dbh, dbback );

    if ( _fc.isCursorInFirstLine() && _fc.getParag()->getParagLayout()->getTopBorder().ptWidth > 0 ) {
        unsigned int _x1 = getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->left() - xOffset;
        unsigned int _y = _fc.getPTY() - yOffset + _fc.getParag()->getParagLayout()->getTopBorder().ptWidth / 2;
        unsigned int _x2 = _x1 + getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->width();

        _painter.setPen( setBorderPen( _fc.getParag()->getParagLayout()->getTopBorder() ) );
        _painter.drawLine( _x1, _y, _x2, _y );
    }

    if ( _fc.isCursorInLastLine() && _fc.getParag()->getParagLayout()->getBottomBorder().ptWidth > 0 ) {
        unsigned int _x1 = getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->left() - xOffset;
        unsigned int _y = _fc.getPTY() - yOffset + _fc.getLineHeight() -
                          _fc.getParag()->getParagLayout()->getBottomBorder().ptWidth / 2 - 1;
        unsigned int _x2 = _x1 + getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->width();

        _painter.setPen( setBorderPen( _fc.getParag()->getParagLayout()->getBottomBorder() ) );
        _painter.drawLine( _x1, _y, _x2, _y );
    }

    if ( _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth > 0 ) {
        unsigned int _x = getFrameSet( _fc.getFrameSet() - 1 )->getFrame( _fc.getFrame() - 1 )->left() - xOffset +
                          _fc.getParag()->getParagLayout()->getLeftBorder().ptWidth / 2;
        unsigned int _y1 = _fc.getPTY() - yOffset;
        unsigned int _y2 = _fc.getPTY() - yOffset + _fc.getLineHeight();

        _painter.setPen( setBorderPen( _fc.getParag()->getParagLayout()->getLeftBorder() ) );
        _painter.drawLine( _x, _y1, _x, _y2 );
    }

    if ( _fc.getParag()->getParagLayout()->getRightBorder().ptWidth > 0 ) {
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
    if ( pos == 0 && lay->getCounterType() != KWParagLayout::CT_NONE ) {
        KWFormat counterfm( this, _fc );
        counterfm.apply( lay->getFormat() );
        if ( _fc.getParag()->getParagLayout()->getCounterType() == KWParagLayout::CT_BULLET )
            counterfm.setUserFont( findUserFont( _fc.getParag()->getParagLayout()->getBulletFont() ) );

        QFont f( *counterfm.loadFont( this ) );
        if ( zoom != 100 )
            f.setPointSize( zoomIt( f.pointSize() ) );
        _painter.setFont( f );
        _painter.setPen( counterfm.getColor() );

        _painter.drawText( _fc.getPTCounterPos() - xOffset, _fc.getPTY() + _fc.getPTMaxAscender() - yOffset,
                           _fc.getCounterText() );
    }

    // Init font and style
    QFont f( *_fc.loadFont( this ) );
    if ( zoom != 100 )
        f.setPointSize( zoomIt( f.pointSize() ) );
    _painter.setFont( f );
    _painter.setPen( _fc.getColor() );

    QString buffer;
    unsigned int tmpPTPos = 0;
    unsigned int lastPTPos = 0;
    bool atBegin = TRUE;
    while ( !_fc.isCursorAtLineEnd() ) {
        // Init position
        if ( atBegin ) {
            // Change the painter
            tmpPTPos = _fc.getPTPos();
            QFont f( *_fc.loadFont( this ) );
            if ( zoom != 100 )
                f.setPointSize( zoomIt( f.pointSize() ) );
            _painter.setFont( f );
            _painter.setPen( _fc.getColor() );
            atBegin = FALSE;
        }

        if ( _fc.getTextPos() > textLen ) {
            kdWarning() << "Reggie: WOW - something has gone really wrong here!!!!!, my position in the text is longer then my text" << endl;
            return FALSE;
        }

        QChar c = text[ _fc.getTextPos() ].c;
        buffer += c;

        if ( c == KWSpecialChar ) {
            _painter.drawText( tmpPTPos - xOffset,
                               _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() -
                               _fc.getParag()->getParagLayout()->getLineSpacing().pt() - yOffset + plus, buffer );

            switch ( text[ _fc.getTextPos() ].attrib->getClassId() ) {
            case ID_KWCharImage: {
                _painter.drawImage( QPoint( tmpPTPos - xOffset, _fc.getPTY() - yOffset +
                                            ( ( _fc.getLineHeight() -
                                                _fc.getParag()->getParagLayout()->getLineSpacing().pt() )
                                              - ( ( KWCharImage* )text[ _fc.getTextPos() ].attrib )->
                                              getImage()->height() ) ),
                                    *( ( KWCharImage* )text[ _fc.getTextPos() ].attrib )->getImage() );
                _fc.cursorGotoNextChar();
            } break;
            case ID_KWCharVariable: {
                KWCharFormat *f = ( KWCharFormat* )text[ _fc.getTextPos() ].attrib;
                _fc.apply( *f->getFormat() );
                // Change the painter
                if ( _fc.getVertAlign() == KWFormat::VA_NORMAL ) {
                    _painter.setFont( *_fc.loadFont( this ) );
                    plus = 0;
                } else if ( _fc.getVertAlign() == KWFormat::VA_SUB ) {
                    QFont _font = *_fc.loadFont( this );
                    _font.setPointSize( ( 2 * _font.pointSize() ) / 3 );
                    _painter.setFont( _font );
                    plus = _font.pointSize() / 2;
                } else if ( _fc.getVertAlign() == KWFormat::VA_SUPER ) {
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
            case ID_KWCharFootNote: {
                KWCharFootNote *fn = dynamic_cast<KWCharFootNote*>( text[ _fc.getTextPos() ].attrib );

                KWCharFormat *f = ( KWCharFormat* )text[ _fc.getTextPos() ].attrib;
                _fc.apply( *f->getFormat() );
                // Change the painter
                if ( _fc.getVertAlign() == KWFormat::VA_NORMAL ) {
                    QFont f( *_fc.loadFont( this ) );
                    if ( zoom != 100 )
                        f.setPointSize( zoomIt( f.pointSize() ) );
                    _painter.setFont( f );
                    plus = 0;
                } else if ( _fc.getVertAlign() == KWFormat::VA_SUB ) {
                    QFont _font = *_fc.loadFont( this );
                    _font.setPointSize( ( 2 * _font.pointSize() ) / 3 );
                    _painter.setFont( _font );
                    plus = _font.pointSize() / 2;
                } else if ( _fc.getVertAlign() == KWFormat::VA_SUPER ) {
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
            case ID_KWCharTab: {
                lastPTPos = _fc.getPTPos();
                _fc.cursorGotoNextChar();
                QPen _pen = QPen( _painter.pen() );
                _painter.setPen( QPen( blue, 1, DotLine ) );
                if ( _viewFormattingChars )
                    _painter.drawLine( lastPTPos - xOffset, _fc.getPTY() + _fc.getPTMaxAscender() - yOffset,
                                       _fc.getPTPos() - xOffset, _fc.getPTY() + _fc.getPTMaxAscender() - yOffset );
                _painter.setPen( _pen );
            } break;
            case ID_KWCharAnchor: {
                int x = _fc.getPTPos();
                int y = _fc.getPTY();
                KWCharAnchor *a = ( KWCharAnchor* )text[ _fc.getTextPos() ].attrib;

                // We might have moved. Signal this fact to the anchor.
//kdDebug(32001) << "new x=" << x <<", new y="<<y<<endl;
                a->setOrigin( QPoint( x, y ) );

                // An anchor needs to be a zero-width object. We draw it as a
                // thin "T" shape, then allow the subclass to draw anything it
                // wants in addition.
                lastPTPos = x;
                _fc.cursorGotoNextChar();
                if ( _viewFormattingChars )
                {
                    QPen _pen = QPen( _painter.pen() );

                    _painter.setPen( QPen( blue, 1, SolidLine ) );
                    _painter.drawLine( x, y,
                                       x, y + _fc.getPTMaxAscender() + _fc.getPTMaxDescender() );
                    _painter.drawLine( x - 5, y,
                                       x + 5, y );
                    a->viewFormatting( _painter, zoom );
                    _painter.setPen( _pen );
                }
            } break;
            }
            buffer = QString::null;
            atBegin = TRUE;
        } else {
            if ( text[ _fc.getTextPos() ].attrib != 0L ) {
                // Change text format here
                KWCharFormat *f = ( KWCharFormat* )text[ _fc.getTextPos() ].attrib;
                _fc.apply( *f->getFormat() );
                // Change the painter
                if ( _fc.getVertAlign() == KWFormat::VA_NORMAL ) {
                    QFont f( *_fc.loadFont( this ) );
                    if ( zoom != 100 )
                        f.setPointSize( zoomIt( f.pointSize() ) );
                    _painter.setFont( f );
                    plus = 0;
                } else if ( _fc.getVertAlign() == KWFormat::VA_SUB ) {
                    QFont _font = *_fc.loadFont( this );
                    _font.setPointSize( ( 2 * _font.pointSize() ) / 3 );
                    _painter.setFont( _font );
                    plus = _font.pointSize() / 2;
                } else if ( _fc.getVertAlign() == KWFormat::VA_SUPER ) {
                    QFont _font = *_fc.loadFont( this );
                    _font.setPointSize( ( 2 * _font.pointSize() ) / 3 );
                    _painter.setFont( _font );
                    plus = - _fc.getPTAscender() + _font.pointSize() / 2;
                }
                _painter.setPen( _fc.getColor() );
            }

            // Test next character.
            if ( _fc.cursorGotoNextChar() != 1 || ( text[ _fc.getTextPos() ].c == ' ' &&
                                                    ( _fc.getParag()->getParagLayout()->getFlow() ==
                                                      KWParagLayout::BLOCK || _viewFormattingChars ) ) ) {
                // there was a blank _or_ there will be a font switch _or_ a special object next, so print
                // what we have so far
                _painter.drawText( tmpPTPos - xOffset, /*_fc.getPTY() + _fc.getPTMaxAscender() - yOffset*/
                                   _fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() - yOffset -
                                   _fc.getParag()->getParagLayout()->getLineSpacing().pt() + plus, buffer );
                buffer = QString::null;
                atBegin = TRUE;
                // Blanks are not printed at all - but we have to underline it in some cases
                if ( text[ _fc.getTextPos() ].c == ' ' ) {
                    bool goneForward = FALSE;
                    if ( _fc.getUnderline() && _fc.getTextPos() > _fc.getLineStartPos() &&
                         _fc.getTextPos() < _fc.getLineEndPos() - 1 ) {
                        if ( text[ _fc.getTextPos() - 1 ].c != KWSpecialChar &&
                             text[ _fc.getTextPos() + 1 ].c != KWSpecialChar ) {
                            KWCharFormat *f1 = ( KWCharFormat* )text[ _fc.getTextPos() - 1 ].attrib;
                            KWCharFormat *f2 = ( KWCharFormat* )text[ _fc.getTextPos() + 1 ].attrib;
                            if ( f1->getFormat()->getUnderline() && f2->getFormat()->getUnderline() ) {
                                KWFormat *_f = f1->getFormat();
                                QFontMetrics fm( *findDisplayFont( _f->getUserFont(), _f->getPTFontSize(),
                                                                   _f->getWeight(),
                                                                   _f->getItalic(), _f->getUnderline() ) );

                                _painter.setPen( QPen( _fc.getColor(), fm.lineWidth(), SolidLine ) );
                                int ly = static_cast<int>(_fc.getPTY() + _fc.getLineHeight() - _fc.getPTMaxDescender() - yOffset -
                                         _fc.getParag()->getParagLayout()->getLineSpacing().pt() + plus +
                                         fm.underlinePos() + fm.lineWidth() / 2);
                                int lx1 = static_cast<int>(_fc.getPTPos());
                                lastPTPos = _fc.getPTPos();
                                _fc.cursorGotoNextChar();
                                goneForward = TRUE;
                                int lx2 = static_cast<int>(_fc.getPTPos());
                                _painter.drawLine( lx1, ly, lx2, ly );
                            }
                        }

                    }
                    if ( !goneForward ) {
                        lastPTPos = _fc.getPTPos();
                        _fc.cursorGotoNextChar();
                    }
                    if ( _viewFormattingChars ) {
                        _painter.fillRect( lastPTPos + ( _fc.getPTPos() - lastPTPos ) / 2 - xOffset,
                                           _fc.getPTY() + _fc.getPTMaxAscender() / 2 - yOffset, 2, 2, blue );
                    }
                }
            }
        }
    }

    if ( _viewFormattingChars && _fc.isCursorAtParagEnd() )
        _painter.drawPixmap( _fc.getPTPos() + 3 - xOffset,
                             _fc.getPTY() + _fc.getPTMaxAscender() - ret_pix.height() - yOffset,
                             ret_pix );

    _painter.restore();

    return TRUE;
}

/*================================================================*/
void KWordDocument::printBorders( QPainter &_painter, int xOffset, int yOffset, int, int )
{
    KWFrameSet *frameset = 0;
    KWFrame *tmp;
    QRect frame;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        frameset = getFrameSet( i );
        if ( !frameset->isVisible() )
            continue;

        if ( isAHeader( frameset->getFrameInfo() ) && !hasHeader() ||
             isAFooter( frameset->getFrameInfo() ) && !hasFooter() ||
             isAWrongHeader( frameset->getFrameInfo(), getHeaderType() ) ||
             isAWrongFooter( frameset->getFrameInfo(), getFooterType() ) )
            continue;
        for ( unsigned int j = 0; j < frameset->getNumFrames(); j++ ) {
            bool isRight = TRUE, isBottom = TRUE;
            if ( frameset->getGroupManager() ) {
                unsigned int r, c;
                r=frameset->getGroupManager()->getCell( frameset )->row;
                c=frameset->getGroupManager()->getCell( frameset )->col;
                if ( r < frameset->getGroupManager()->getRows() - 1 ) isBottom = FALSE;
                if ( c < frameset->getGroupManager()->getCols() - 1 ) isRight = FALSE;
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

            if ( tmp->getLeftBorder().ptWidth > 0 && tmp->getLeftBorder().color != tmp->getBackgroundColor().color() ) {
                QPen p( setBorderPen( tmp->getLeftBorder() ) );
                _painter.setPen( p );
                _painter.drawLine( frame.x() + tmp->getLeftBorder().ptWidth / 2, frame.y(),
                                   frame.x() + tmp->getLeftBorder().ptWidth / 2, frame.bottom() + ( isBottom ? 0 : 1 ) );
            }
            if ( tmp->getRightBorder().ptWidth > 0 && tmp->getRightBorder().color != tmp->getBackgroundColor().color() ) {
                QPen p( setBorderPen( tmp->getRightBorder() ) );
                _painter.setPen( p );
                int w = tmp->getRightBorder().ptWidth;
                if ( ( w / 2 ) * 2 == w ) w--;
                w /= 2;
                _painter.drawLine( frame.right() - w, frame.y(),
                                   frame.right() - w, frame.bottom() + ( isBottom ? 0 : 1 ) );
            }
            if ( tmp->getTopBorder().ptWidth > 0 && tmp->getTopBorder().color != tmp->getBackgroundColor().color() ) {
                QPen p( setBorderPen( tmp->getTopBorder() ) );
                _painter.setPen( p );
                _painter.drawLine( frame.x(), frame.y() + tmp->getTopBorder().ptWidth / 2,
                                   frame.right() + ( isRight ? 0 : 1 ), frame.y() + tmp->getTopBorder().ptWidth / 2 );
            }
            if ( tmp->getBottomBorder().ptWidth > 0 &&
                 tmp->getBottomBorder().color != tmp->getBackgroundColor().color() ) {
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
    if ( !isReadWrite() )
      return;

    RasterOp rop = _painter->rasterOp();

    _painter->setRasterOp( NotROP );
    QPen pen;
    pen.setWidth( 1 );
    _painter->setPen( pen );

    unsigned int diffx1 = 0;
    unsigned int diffx2 = 0;
    if ( _fc.getItalic() ) {
        diffx1 = static_cast<int>( static_cast<float>( _fc.getLineHeight() ) / 3.732 );
        diffx2 = 0;
    }

    _painter->drawLine( _fc.getPTPos() - xOffset + diffx1,
                        _fc.getPTY() - yOffset,
                        _fc.getPTPos() - xOffset + diffx2,
                        _fc.getPTY() + _fc.getLineHeight() -
                        _fc.getParag()->getParagLayout()->getLineSpacing().pt()
                        - yOffset );

    _painter->setRasterOp( rop );
}

/*================================================================*/
void KWordDocument::updateAllViews( KWordView *_view, bool _clear )
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() ) {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
            if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() ) {
                if ( viewPtr != _view ) {
                    if ( _clear )
                        viewPtr->getGUI()->getPaperWidget()->clear();
                    viewPtr->getGUI()->getPaperWidget()->repaintScreen( FALSE );
                }
            }
        }
    }
}

/*================================================================*/
void KWordDocument::updateAllViewportSizes()
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() ) {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
            if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
                viewPtr->getGUI()->getPaperWidget()->resizeContents( getPTPaperWidth(),
                                                                     getPTPaperHeight() * pages );
        }
    }
}

/*================================================================*/
void KWordDocument::updateAllSelections()
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() ) {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
            if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
                viewPtr->getGUI()->getPaperWidget()->updateSelections();
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

    if ( !m_lstViews.isEmpty() ) {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
            if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() ) {
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

    if ( !m_lstViews.isEmpty() ) {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
            if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() ) {
                if ( viewPtr->getGUI() ) {
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

    if ( !m_lstViews.isEmpty() ) {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
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

    if ( !m_lstViews.isEmpty() ) {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
            if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() ) {
                if ( viewPtr->getGUI() ) {
                    if ( !_painter ) {
                        p.begin( viewPtr->getGUI()->getPaperWidget()->viewport() );
                        viewPtr->getGUI()->getPaperWidget()->drawBorders( p,
                                                                          viewPtr->getGUI()->getPaperWidget()->rect(),
                                                                          back );
                        p.end();
                    } else
                        viewPtr->getGUI()->getPaperWidget()->drawBorders( *_painter,
                                                                          viewPtr->getGUI()->getPaperWidget()->rect(),
                                                                          back );
                }
            }
        }
    }
}

/*================================================================*/
void KWordDocument::updateAllStyles()
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
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
    setModified(TRUE);
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
    {
        return;
    }

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
    setModified(TRUE);
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

    QString clip_string;
    QTextStream out( &clip_string, IO_WriteOnly );

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

    KWordDrag *kd = new KWordDrag;
    kd->setPlain( clipString );
    kd->setKWord( clip_string.utf8() );

    cb->setData( kd );
}

/*================================================================*/
void KWordDocument::setFormat( KWFormat &_format, int flags )
{
    KWFormatContext tmpFC2( this, selStart.getFrameSet() - 1 );
    KWFormatContext tmpFC1( this, selStart.getFrameSet() - 1 );

    if ( selStart.getParag() == selEnd.getParag() ) {
        if ( selStart.getTextPos() < selEnd.getTextPos() ) {
            tmpFC1 = selStart;
            tmpFC2 = selEnd;
        } else {
            tmpFC1 = selEnd;
            tmpFC2 = selStart;
        }

        tmpFC1.getParag()->setFormat( tmpFC1.getTextPos(),
                                      tmpFC2.getTextPos() - tmpFC1.getTextPos(),
                                      _format, flags );
    } else {
        KWParag *parag = getFirstParag( selStart.getFrameSet() - 1 );
        while ( parag ) {
            if ( parag == selStart.getParag() ) {
                tmpFC1 = selStart;
                tmpFC2 = selEnd;
                break;
            }
            if ( parag == selEnd.getParag() ) {
                tmpFC2 = selStart;
                tmpFC1 = selEnd;
                break;
            }
            parag = parag->getNext();
        }

        tmpFC1.getParag()->setFormat( tmpFC1.getTextPos(),
                                      tmpFC1.getParag()->getTextLen() - tmpFC1.getTextPos(),
                                      _format, flags );
        parag = tmpFC1.getParag()->getNext();
        while ( parag && parag != tmpFC2.getParag() ) {
            if ( parag->getTextLen() > 0 )
                parag->setFormat( 0, parag->getTextLen(), _format, flags );
            parag = parag->getNext();
        }
        tmpFC2.getParag()->setFormat( 0, tmpFC2.getTextPos(), _format, flags );
    }
    setModified(TRUE);
}

/*================================================================*/
void KWordDocument::paste( KWFormatContext *_fc, QString _string, KWPage *_page,
                           KWFormat *_format, const QString &_mime )
{
    QStringList strList;
    KWParag *firstParag = 0L, *parag = 0L, *parag2 = 0L, *calcParag = 0L;
    int index;

    if ( _string.isEmpty() ) return;

    if ( _mime == "text/plain" ) {     // ----------------- MIME type text/plain
        while ( TRUE ) {
            index = _string.find( '\n', 0 );
            if ( index == -1 ) break;

            if ( index > 0 && !_string.left( index ).simplifyWhiteSpace().isEmpty() )
                strList.append( _string.left( index ) );
            _string.remove( 0, index + 1 );
        }

        if ( !_string.isEmpty() && !_string.simplifyWhiteSpace().isEmpty() )
            strList.append( _string );
    } else if ( _mime == MIME_TYPE ) {     // -------------- MIME type application/x-kword

        QDomDocument doc;
        doc.setContent( _string );

        KOMLParser parser( doc );

        string tag;
        vector<KOMLAttrib> lst;
        string name;

        if ( !parser.open( "PARAGRAPHS", tag ) ) {
            kdError(32001) << "Missing PARAGRAPHS" << endl;
            return;
        }

        while ( parser.open( 0L, tag ) ) {
            parser.parseTag( tag.c_str(), name, lst );

            if ( name == "PARAGRAPH" ) {
                parser.parseTag( tag.c_str(), name, lst );
                vector<KOMLAttrib>::const_iterator it = lst.begin();
                for( ; it != lst.end(); it++ ) {
                }

                parag2 = new KWParag( dynamic_cast<KWTextFrameSet*>( getFrameSet( _fc->getFrameSet() - 1 ) ),
                                      this, 0L, 0L, defaultParagLayout, FALSE );
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
                str = strList[ 0 ];
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
                str = strList[ 0 ];
                len = str.length();
                _fc->getParag()->insertText( _fc->getTextPos(), str );
                _fc->getParag()->setFormat( _fc->getTextPos(), len, *format );

                for ( unsigned int j = 0; j <= len; j++ )
                    _fc->cursorGotoRight();

                QKeyEvent ev(static_cast<QEvent::Type>(6) /*QEvent::KeyPress*/ ,Qt::Key_Return,13,0);
                _page->keyPressEvent( &ev );

                str = strList[ 1 ];
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
                str = strList[ 0 ];
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
                    str = strList[ i ];
                    len = str.length();
                    p = new KWParag( dynamic_cast<KWTextFrameSet*>( getFrameSet( _fc->getFrameSet() - 1 ) ),
                                     this,
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

                KWParag *p = 0L, *prev = _fc->getParag(), *parag = firstParag->getNext(),
                     *next = _fc->getParag()->getNext();

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
    setModified(TRUE);
}

/*================================================================*/
void KWordDocument::appendPage( unsigned int /*_page*/, bool /*redrawBackgroundWhenAppendPage*/ )
{
    //QRect pageRect( 0, _page * getPTPaperHeight(), getPTPaperWidth(), getPTPaperHeight() );
    kdDebug() <<"KWordDocument::appendPage" << endl;
    pages++;

    int thisPageNum  = pages-2;
    KWFrameSet *frameSet = 0L;
    KWFrame *frame;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        frameSet = getFrameSet( i );
        // don't add tables! A table cell ( frameset ) _must_ not have more than one frame!
        if ( frameSet->getGroupManager()) continue;

        unsigned int numFrames=frameSet->getNumFrames();
        for (unsigned int j =0; j < numFrames; j++) {
            frame = frameSet->getFrame(j);
            // if frame is no last page && frame may be copied to next frame.
            if((frame->getPageNum() == thisPageNum || frame->getPageNum() == thisPageNum && frame->getSheetSide() != AnySide)  && (
                  frame->getNewFrameBehaviour() == Copy || frame->getNewFrameBehaviour() == Reconnect)) {
                // make a new frame.
                KWFrame *frm = new KWFrame(frameSet, frame->x(), frame->y() + getPTPaperHeight(), frame->width(), frame->height(), frame->getRunAround(),
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
                frm->setPageNum( pages+1);
                frm->setNewFrameBehaviour(frame->getNewFrameBehaviour());
                frm->setFrameBehaviour(frame->getFrameBehaviour());
                frm->setSheetSide(frame->getSheetSide());
                frameSet->addFrame( frm );
            }
        }
    }

//     if ( redrawBackgroundWhenAppendPage )
//      drawAllBorders();
    updateAllFrames();
    updateAllViewportSizes();

    if ( hasHeader() || hasFooter() )
        recalcFrames( FALSE, TRUE );
    setModified(TRUE);
}

/*================================================================*/
int KWordDocument::getFrameSet( unsigned int mx, unsigned int my )
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        frameSet = getFrameSet( getNumFrameSets() - 1 - i );
        if ( frameSet->contains( mx, my ) ) {
            if ( !frameSet->isVisible() )
                continue;
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

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        frameSet = getFrameSet( getNumFrameSets() - 1 - i );
        if ( frameSet->contains( mx, my ) ) {
            if ( !frameSet->isVisible() )
                continue;
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

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        frameSet = getFrameSet( getNumFrameSets() - 1 - i );
        if ( frameSet->contains( mx, my ) )
            frameSet->deSelectFrame( mx, my );
    }
}

/*================================================================*/
void KWordDocument::deSelectAllFrames()
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        frameSet = getFrameSet( getNumFrameSets() - 1 - i );
        for ( unsigned int j = 0; j < frameSet->getNumFrames(); j++ )
            frameSet->getFrame( j )->setSelected( FALSE );
    }
}

/*================================================================*/
QCursor KWordDocument::getMouseCursor( unsigned int mx, unsigned int my )
{
    KWFrameSet *frameSet = 0L;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        frameSet = getFrameSet( getNumFrameSets() - 1 - i );
        if ( frameSet->contains( mx, my ) ) {
            if ( !frameSet->isVisible() )
                continue;
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
QList<KWFrame> KWordDocument::getSelectedFrames() {
    QList<KWFrame> frames;
    frames.setAutoDelete( FALSE );
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        KWFrameSet *frameSet = getFrameSet(i);
        if ( !frameSet->isVisible() )
            continue;
        if ( frameSet->isRemoveableHeader() )
            continue;
        if ( isAHeader( frameSet->getFrameInfo() ) && !hasHeader() ||
             isAFooter( frameSet->getFrameInfo() ) && !hasFooter() ||
             isAWrongHeader( frameSet->getFrameInfo(), getHeaderType() ) ||
             isAWrongFooter( frameSet->getFrameInfo(), getFooterType() ) )
            continue;
        for ( unsigned int j = 0; j < getFrameSet(i)->getNumFrames(); j++ ) {
            if ( frameSet->getFrame( j )->isSelected() ) {
                frames.append( frameSet->getFrame( j ));
            }
        }
    }
    return frames;
}

/*================================================================*/
KWFrame *KWordDocument::getFirstSelectedFrame()
{
    int foo=0; // This variable is not needed, but we mustn't pass i (loops quite nicely)!!!
    for (int i = 0; i < getNumFrameSets(); i++ ) {
        KWFrame *frame = getFirstSelectedFrame(foo);
        if(frame)
            return frame;
    }
    return 0L;
}

/*================================================================*/
KWFrame *KWordDocument::getFirstSelectedFrame( int &_frameset )
{
    KWFrameSet *frameSet = 0L;
    _frameset = 0;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        _frameset = getNumFrameSets() - 1 - i;
        frameSet = getFrameSet( getNumFrameSets() - 1 - i );
        for ( unsigned int j = 0; j < frameSet->getNumFrames(); j++ ) {
            if ( !frameSet->isVisible() )
                continue;
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
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( getFrameSet( i )->hasSelectedFrame() )
            return getFrameSet( i );
    }

    return 0L;
}

/*================================================================*/
void KWordDocument::print( QPainter *painter, QPrinter *printer,
                           float /*left_margin*/, float /*top_margin*/ )
{
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
    unsigned int i = 0, j = 0;

    for ( i = 0; i < frames.count(); i++ ) {
        if ( frames.at( i )->getFrameType() == FT_TEXT ) {
            frames.at( i )->setCurrent( 0 );
            fc = new KWFormatContext( this, i + 1 );
            fc->init( dynamic_cast<KWTextFrameSet*>( frames.at( i ) )->getFirstParag(), TRUE );
            fcList.append( fc );
        }
    }

    for ( i = 0; i < static_cast<unsigned int>( pages ); i++ ) {
        kapp->processEvents();
        QRect pageRect( 0, i * getPTPaperHeight(), getPTPaperWidth(), getPTPaperHeight() );
        unsigned int minus = 0;
        if ( i + 1 > static_cast<unsigned int>( printer->fromPage() ) )
            printer->newPage();
        printBorders( *painter, 0, i * getPTPaperHeight(), getPTPaperWidth(), getPTPaperHeight() );
        for ( j = 0; j < frames.count(); j++ ) {
            if ( !getFrameSet( j )->isVisible() )
                continue;
            if ( isAHeader( getFrameSet( j )->getFrameInfo() ) && !hasHeader() ||
                 isAFooter( getFrameSet( j )->getFrameInfo() ) && !hasFooter() ||
                 isAWrongHeader( getFrameSet( j )->getFrameInfo(), getHeaderType() ) ||
                 isAWrongFooter( getFrameSet( j )->getFrameInfo(), getFooterType() ) )
                continue;
            switch ( frames.at( j )->getFrameType() ) {
            case FT_PICTURE: {
                minus++;

                KWPictureFrameSet *picFS = dynamic_cast<KWPictureFrameSet*>( frames.at( j ) );
                KWFrame *frame = picFS->getFrame( 0 );
                if ( !frame->intersects( pageRect ) ) break;

                QSize _size = QSize( frame->width(), frame->height() );
                if ( _size != picFS->getImage()->size() )
                    picFS->setSize( _size );

                painter->drawImage( frame->x(), frame->y() - i * getPTPaperHeight(), *picFS->getImage() );
            } break;
            case FT_PART: {
                minus++;

                KWPartFrameSet *partFS = dynamic_cast<KWPartFrameSet*>( getFrameSet( j ) );
                KWFrame *frame = partFS->getFrame( 0 );

                QPicture *pic = partFS->getPicture();

                painter->save();
                painter->setClipRect( frame->x(), frame->y() - i * getPTPaperHeight(),
                                      frame->width() - 1, frame->height() - 1 );
                QRect r = painter->viewport();
                painter->setViewport( frame->x(), frame->y() - i * getPTPaperHeight(), r.width(), r.height() );
                if ( pic ) painter->drawPicture( *pic );
                painter->setViewport( r );
                painter->restore();
            } break;
            case FT_FORMULA: {
                minus++;

                KWFormulaFrameSet *formulaFS = dynamic_cast<KWFormulaFrameSet*>( getFrameSet( j ) );
                KWFrame *frame = formulaFS->getFrame( 0 );

                QPicture *pic = formulaFS->getPicture();

                painter->save();
                QRect r = painter->viewport();
                painter->setViewport( frame->x(), frame->y() - i * getPTPaperHeight(), r.width(), r.height() );
                if ( pic )
                    painter->drawPicture( *pic );
                painter->setViewport( r );
                painter->restore();
            } break;
            case FT_TEXT: {
                bool bend = FALSE;
                bool reinit = TRUE;
                fc = fcList.at(j - minus);
                if ( frames.at( j )->getFrameInfo() != FI_BODY ) {
                    if ( frames.at( j )->getFrameInfo() == FI_EVEN_HEADER ||
                         frames.at( j )->getFrameInfo() == FI_FIRST_HEADER ||
                         frames.at( j )->getFrameInfo() == FI_ODD_HEADER ) {
                        if ( !hasHeader() ) continue;
                        switch ( getHeaderType() ) {
                        case HF_SAME: {
                            if ( frames.at( j )->getFrameInfo() != FI_EVEN_HEADER )
                                continue;
                        } break;
                        case HF_EO_DIFF: {
                            if ( frames.at( j )->getFrameInfo() == FI_FIRST_HEADER )
                                continue;
                            if ( ( ( i + 1 ) / 2 ) * 2 == i + 1 && frames.at( j )->getFrameInfo() == FI_ODD_HEADER )
                                continue;
                            if ( ( ( i + 1 ) / 2 ) * 2 != i + 1 && frames.at( j )->getFrameInfo() == FI_EVEN_HEADER )
                                continue;
                        } break;
                        case HF_FIRST_DIFF: {
                            if ( i == 0 && frames.at( j )->getFrameInfo() != FI_FIRST_HEADER )
                                continue;
                            if ( i > 0 && frames.at( j )->getFrameInfo() != FI_EVEN_HEADER )
                                continue;
                        } break;
                        default: break;
                        }
                    }
                    if ( frames.at( j )->getFrameInfo() == FI_EVEN_FOOTER ||
                         frames.at( j )->getFrameInfo() == FI_FIRST_FOOTER ||
                         frames.at( j )->getFrameInfo() == FI_ODD_FOOTER ) {
                        if ( !hasFooter() ) continue;
                        switch ( getFooterType() ) {
                        case HF_SAME: {
                            if ( frames.at( j )->getFrameInfo() != FI_EVEN_FOOTER )
                                continue;
                        } break;
                        case HF_EO_DIFF: {
                            if ( frames.at( j )->getFrameInfo() == FI_FIRST_FOOTER )
                                continue;
                            if ( ( ( i + 1 ) / 2 ) * 2 == i + 1 && frames.at( j )->getFrameInfo() == FI_ODD_FOOTER )
                                continue;
                            if ( ( ( i + 1 ) / 2 ) * 2 != i + 1 && frames.at( j )->getFrameInfo() == FI_EVEN_FOOTER )
                                continue;
                        } break;
                        case HF_FIRST_DIFF: {
                            if ( i == 0 && frames.at( j )->getFrameInfo() != FI_FIRST_FOOTER )
                                continue;
                            if ( i > 0 && frames.at( j )->getFrameInfo() != FI_EVEN_FOOTER )
                                continue;
                        } break;
                        default:
                            break;
                        }
                    }
                    fc->init( dynamic_cast<KWTextFrameSet*>( frames.at( j ) )->getFirstParag(), TRUE,
                              frames.at( j )->getCurrent() + 1, i + 1 );
                    if ( static_cast<int>( frames.at( j )->getNumFrames() - 1 ) >
                         static_cast<int>( frames.at( j )->getCurrent() ) )
                        frames.at( j )->setCurrent( frames.at( j )->getCurrent() + 1 );
                    reinit = FALSE;
                }
                if ( reinit )
                    fc->init( dynamic_cast<KWTextFrameSet*>( frames.at( fc->getFrameSet() - 1 ) )->getFirstParag(), TRUE );
                while ( !bend ) {
                    printLine( *fc, *painter, 0, i * getPTPaperHeight(), getPTPaperWidth(),
                               getPTPaperHeight(), FALSE, FALSE );
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
}

/*================================================================*/
void KWordDocument::updateAllFrames()
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ )
        getFrameSet( i )->update();

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

    for ( i = 0; i < frames.count(); i++ ) {
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
}

/*================================================================*/
void KWordDocument::recalcWholeText( bool _cursor, bool _fast )
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() ) {
        viewPtr = m_lstViews.first();
        if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
            viewPtr->getGUI()->getPaperWidget()->recalcWholeText( _cursor, _fast );
    }
}

/*================================================================*/
void KWordDocument::recalcWholeText( KWParag *start, unsigned int fs )
{
    KWordView *viewPtr;

    if ( !m_lstViews.isEmpty() ) {
        viewPtr = m_lstViews.first();
        if ( viewPtr->getGUI() && viewPtr->getGUI()->getPaperWidget() )
            viewPtr->getGUI()->getPaperWidget()->recalcWholeText( start, fs );
    }
}

/*================================================================*/
void KWordDocument::addStyleTemplate( KWParagLayout *pl )
{
    KWParagLayout* p;
    for ( p = paragLayoutList.first(); p != 0L; p = paragLayoutList.next() ) {
        if ( p->getName() == pl->getName() ) {
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
    setModified(TRUE);
}

/*================================================================*/
bool KWordDocument::isStyleChanged( QString _name )
{
    return ( changedStyles.findIndex( _name ) != -1 );
}

/*================================================================*/
void KWordDocument::setHeader( bool h )
{
    _header = h;
    if ( !_header ) {
        KWordView *viewPtr = 0L;

        if ( !m_lstViews.isEmpty() ) {
            for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
                viewPtr->getGUI()->getPaperWidget()->footerHeaderDisappeared();
        }
    }

    recalcFrames( TRUE, TRUE );
    updateAllViews( 0L, TRUE );
}

/*================================================================*/
void KWordDocument::setFooter( bool f )
{
    _footer = f;
    if ( !_footer ) {
        KWordView *viewPtr = 0L;

        if ( !m_lstViews.isEmpty() ) {
            for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
                viewPtr->getGUI()->getPaperWidget()->footerHeaderDisappeared();
        }
    }

    recalcFrames( TRUE, TRUE );
    updateAllViews( 0L, TRUE );
}

/*================================================================*/
bool KWordDocument::canResize( KWFrameSet *frameset, KWFrame *frame, int page, int diff )
{
    if ( diff < 0 ) return FALSE;

    if ( !frameset->getGroupManager() ) {
        // a normal frame _must_ not leave the page
        if ( frameset->getFrameInfo() == FI_BODY ) {
            if ( static_cast<int>( frame->bottom() + diff ) < static_cast<int>( ( page + 1 ) * getPTPaperHeight() ) )
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

    return FALSE;
}

/*================================================================*/
void KWordDocument::setRunAround( RunAround _ra )
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
void KWordDocument::setRunAroundGap( KWUnit _gap )
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
void KWordDocument::getFrameMargins( KWUnit &l, KWUnit &r, KWUnit &t, KWUnit &b )
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
bool KWordDocument::isOnlyOneFrameSelected()
{

    return getSelectedFrames().count()==1;

/* This code does not check if the frames are headers and visible.. (TZ)
    int _selected = 0;

    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( getFrameSet( i )->hasSelectedFrame() ) {
            for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ ) {
                if ( getFrameSet( i )->getFrame( j )->isSelected() ) {
                    _selected++;
                }
            }
        }
    }

    return _selected == 1;
*/
}

/*================================================================*/
KWFrameSet *KWordDocument::getFrameCoords( unsigned int &x, unsigned int &y,
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
void KWordDocument::setFrameMargins( KWUnit l, KWUnit r, KWUnit t, KWUnit b )
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
void KWordDocument::setFrameCoords( unsigned int x, unsigned int y, unsigned int w, unsigned int h )
{
    for ( unsigned int i = 0; i < getNumFrameSets(); i++ ) {
        if ( getFrameSet( i )->hasSelectedFrame() ) {
            for ( unsigned int j = 0; j < getFrameSet( i )->getNumFrames(); j++ ) {
                if ( getFrameSet( i )->getFrame( j )->isSelected() && x + w < getPTPaperWidth() &&
                     y + h < pages * getPTPaperHeight() ) {
                    if ( !getFrameSet( i )->getGroupManager() )
                        getFrameSet( i )->getFrame( j )->setRect( x, y, w, h );
                }
            }
        }
    }
    updateAllSelections();
    setModified(TRUE);
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
    setModified(TRUE);
}

/*================================================================*/
void KWordDocument::redo()
{
    history.redo();
    setModified(TRUE);
}

/*================================================================*/
void KWordDocument::slotUndoRedoChanged( QString undo, QString redo )
{
    KWordView *viewPtr = 0L;

    if ( !m_lstViews.isEmpty() ) {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() ) {
            viewPtr->changeUndo( undo, !undo.isEmpty() );
            viewPtr->changeRedo( redo, !redo.isEmpty() );
        }
    }
}

/*================================================================*/
void KWordDocument::slotDocumentLoaded()
{
  updateAllViews( 0L, true );
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
    while ( TRUE ) {
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

/*================================================================*/
void KWordDocument::registerVariable( KWVariable *var )
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
void KWordDocument::unregisterVariable( KWVariable *var )
{
    variables.take( variables.findRef( var ) );
}

/*================================================================*/
void KWordDocument::setVariableValue( const QString &name, const QString &value )
{
    varValues[ name ] = value;
}

/*================================================================*/
QString KWordDocument::getVariableValue( const QString &name ) const
{
    return varValues[ name ];
}

/*================================================================*/
KWSerialLetterDataBase *KWordDocument::getSerialLetterDataBase() const
{
    return slDataBase;
}

/*================================================================*/
int KWordDocument::getSerialLetterRecord() const
{
    return slRecordNum;
}

/*================================================================*/
void KWordDocument::setSerialLetterRecord( int r )
{
    slRecordNum = r;
}

/*================================================================*/
void KWordDocument::createContents()
{
    contents->createContents();
    recalcWholeText();
    updateAllCursors();
    updateAllViews( 0, TRUE );
}

/*================================================================*/
void KWordDocument::checkNumberOfPages( KWFormatContext *fc )
{
    // ### finish this stuff!
    return;

    // what about a loop through all frames and ask the frames which page they
    // are on. Thereby selecting the max page. filter empty frames from
    // frameset 0 (only if WP)
    // Will get back to this, but the picture and other frames first must remember
    // their page num ;-) (TZ)

    if ( processingType == DTP || fc->getParag()->getNext() || fc->getFrameSet() != 1 )
        return;

    int num = fc->getPage() - 1;
    if ( pages - 1 > num && pages > 1 ) {
        pages = num + 1;
        KWFrameSet *fs = 0;
        KWFrame *f = 0;
        bool changed = FALSE;
        fs = getFrameSet( fc->getFrameSet() - 1 );
        if ( !fs || fs->getFrameType() != FT_TEXT )
                return;
        int del = 0;
        for ( unsigned int i = 0; i < fs->getNumFrames() - del; ++i ) {
            if ( i >= fs->getNumFrames() )
                break;
            f = fs->getFrame( i );
            if ( f && f->getPageNum() > num ) {
                if ( canRemovePage( f->getPageNum(), f ) ) {
                    kdDebug() << "remove page > " <<f->getPageNum() <<", frame; "<<i<< endl;
                    fs->delFrame( i );
                    changed = TRUE;
                    del++;
                    i--;
                }
            }
        }
        if ( fs->getNumFrames() == 0 )
            frames.removeRef( fs );
        if ( changed )
            recalcFrames();
    }
}

/*================================================================*/
bool KWordDocument::canRemovePage( int num, KWFrame *f )
{
    KWFrameSet *fs = 0;
    KWFrame *frm = 0;

    for ( fs = frames.first(); fs; fs = frames.next() ) {
        if ( fs->getFrameInfo() != FI_BODY )
            continue;
        for ( unsigned int i = 0; i < fs->getNumFrames(); ++i ) {
            frm = fs->getFrame( i );
            if ( frm != f && frm->getPageNum() == num )
                return FALSE;
        }
    }

    return TRUE;
}

void KWordDocument::getPageLayout( KoPageLayout& _layout, KoColumns& _cl, KoKWHeaderFooter& _hf )
{
    _layout = pageLayout;
    _cl = pageColumns;
    _hf = pageHeaderFooter;

    if ( zoom != 100 ) {
        _layout.ptWidth = zoomIt( _layout.ptWidth );
        _layout.ptHeight = zoomIt( _layout.ptHeight );
        _layout.ptLeft = zoomIt( _layout.ptLeft );
        _layout.ptRight = zoomIt( _layout.ptRight );
        _layout.ptTop = zoomIt( _layout.ptTop );
        _layout.ptBottom = zoomIt( _layout.ptBottom );
        _layout.mmWidth = zoomIt( _layout.mmWidth );
        _layout.mmHeight = zoomIt( _layout.mmHeight );
        _layout.mmLeft = zoomIt( _layout.mmLeft );
        _layout.mmRight = zoomIt( _layout.mmRight );
        _layout.mmTop = zoomIt( _layout.mmTop );
        _layout.mmBottom = zoomIt( _layout.mmBottom );
        _layout.inchWidth = zoomIt( _layout.inchWidth );
        _layout.inchHeight = zoomIt( _layout.inchHeight );
        _layout.inchLeft = zoomIt( _layout.inchLeft );
        _layout.inchRight = zoomIt( _layout.inchRight );
        _layout.inchTop = zoomIt( _layout.inchTop );
        _layout.inchBottom = zoomIt( _layout.inchBottom );

        _cl.ptColumnSpacing = zoomIt( _cl.ptColumnSpacing );
        _cl.mmColumnSpacing = zoomIt( _cl.mmColumnSpacing );
        _cl.inchColumnSpacing = zoomIt( _cl.inchColumnSpacing );

        _hf.ptHeaderBodySpacing = zoomIt( _hf.ptHeaderBodySpacing );
        _hf.ptFooterBodySpacing = zoomIt( _hf.ptFooterBodySpacing );
        _hf.mmHeaderBodySpacing = zoomIt( _hf.mmHeaderBodySpacing );
        _hf.mmFooterBodySpacing = zoomIt( _hf.mmFooterBodySpacing );
        _hf.inchHeaderBodySpacing = zoomIt( _hf.inchHeaderBodySpacing );
        _hf.inchFooterBodySpacing = zoomIt( _hf.inchFooterBodySpacing );
    }
}

KWUserFont* KWordDocument::getDefaultUserFont() {

    if(defaultUserFont==0L)
        defaultUserFont=findUserFont( "times" );
    return defaultUserFont;
}

void KWordDocument::updateFrameSizes( int oldZoom )
{
    KWFrameSet *fs = frames.first();
    fs = frames.next();
    KWFrame *frm = 0;
    for ( ; fs; fs = frames.next() ) {
        if ( fs->getFrameInfo() != FI_BODY )
            continue;
        for ( unsigned int i = 0; i < fs->getNumFrames(); ++i ) {
            frm = fs->getFrame( i );
            double x = ( 100.0 * (double)frm->x() ) / (double)oldZoom;
            double y = ( 100.0 * (double)frm->y() ) / (double)oldZoom;
            double w = ( 100.0 * (double)frm->width() ) / (double)oldZoom;
            double h = ( 100.0 * (double)frm->height() ) / (double)oldZoom;
            x = zoomIt( x );
            y = zoomIt( y );
            w = zoomIt( w );
            h = zoomIt( h );
            frm->setRect( x, y, w, h );
        }
    }
}

void KWordDocument::delGroupManager( KWGroupManager *g, bool deleteit )
{
  if (deleteit)
    grpMgrs.remove( g );
  else
    grpMgrs.take(grpMgrs.find(g));
}

void KWordDocument::delFrameSet( KWFrameSet *f, bool deleteit)
{
  if (deleteit)
    frames.remove( f );
  else
    frames.take( frames.find(f) );
  setModified( true );
}
