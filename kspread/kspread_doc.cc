/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

#include <qapplication.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qpair.h>

#include <kstandarddirs.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <ksconfig.h>

#include "koDocumentInfo.h"
#include <koTemplateChooseDia.h>

#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_sheetprint.h"
#include "kspread_style_manager.h"
#include "kspread_undo.h"
#include "kspread_util.h"
#include "kspread_view.h"
#include "KSpreadDocIface.h"

#include "koApplication.h"

using namespace std;

static const int CURRENT_SYNTAX_VERSION = 1;
// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.2";

/*****************************************************************************
 *
 * KSpreadDoc
 *
 *****************************************************************************/

QPtrList<KSpreadDoc>* KSpreadDoc::s_docs = 0;
int KSpreadDoc::s_docId = 0;

QPtrList<KSpreadDoc>& KSpreadDoc::documents()
{
    if ( s_docs == 0 )
        s_docs = new QPtrList<KSpreadDoc>;
    return *s_docs;
}

KSpreadDoc::KSpreadDoc( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
  : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
    m_pStyleManager( new KSpreadStyleManager() ),
    m_pageBorderColor( Qt::red )
{
  QFont f( KoGlobal::defaultFont() );

  KSpreadFormat::setGlobalRowHeight( f.pointSizeFloat() + 3 );
  KSpreadFormat::setGlobalColWidth( ( f.pointSizeFloat() + 3 ) * 5 );

  m_plugins.setAutoDelete( false );

  m_bDelayCalculation = false;
  m_syntaxVersion = CURRENT_SYNTAX_VERSION;

  if ( s_docs == 0 )
      s_docs = new QPtrList<KSpreadDoc>;
  s_docs->append( this );

  setInstance( KSpreadFactory::global(), false );

  // Set a name if there is no name specified
  if ( !name )
  {
      QString tmp( "Document%1" );
      tmp = tmp.arg( s_docId++ );
      setName( tmp.local8Bit());//tmp.latin1() );
  }

  m_iTableId = 1;
  m_dcop = 0;
  m_pMap = 0L;
  m_bLoading = false;
  m_numOperations = 1; // don't start repainting before the GUI is done...

  m_defaultGridPen.setColor( lightGray );
  m_defaultGridPen.setWidth( 1 );
  m_defaultGridPen.setStyle( SolidLine );

  initInterpreter();

  m_pMap = new KSpreadMap( this, "Map" );

  m_pUndoBuffer = new KSpreadUndo( this );

  // Make us scriptable if the document has a name
  if ( name )
      dcopObject();

  m_iCompletionMode=KGlobalSettings::CompletionAuto;

  m_bVerticalScrollBarShow=true;
  m_bHorizontalScrollBarShow=true;
  m_bShowColHeader=true;
  m_bShowRowHeader=true;
  m_dIndentValue = 10.0;
  m_EMoveTo=KSpread::Bottom;
  m_bShowError=false;
  m_EMethodOfCalc=SumOfNumber;
  m_bShowCommentIndicator=true;
  m_bShowTabBar=true;
  m_bShowFormulaBar=true;
  m_bShowStatusBar=true;
  m_pKSpellConfig=0;

  m_bDontCheckUpperWord=false;
  m_bDontCheckTitleCase=false;
  m_unit = KoUnit::U_MM;
  m_activeTable= 0L;
}

bool KSpreadDoc::initDoc()
{
  //  ElapsedTime et( "      initDoc        " );

    QString f;
    KoTemplateChooseDia::ReturnType ret;

    KoTemplateChooseDia::DialogType dlgtype;
    if (initDocFlags() != KoDocument::InitDocFileNew )
            dlgtype = KoTemplateChooseDia::Everything;
    else
            dlgtype = KoTemplateChooseDia::OnlyTemplates;

    ret = KoTemplateChooseDia::choose( KSpreadFactory::global(), f, "application/x-kspread",
                                       "*.ksp", i18n("KSpread"),
                                        dlgtype, "kspread_template");

    if ( ret == KoTemplateChooseDia::File )
    {
	KURL url( f );
	return openURL( url );
    }

    if ( ret == KoTemplateChooseDia::Empty )
    {
	KConfig *config = KSpreadFactory::global()->config();
	int _page=1;
	if( config->hasGroup("Parameters" ))
	{
		config->setGroup( "Parameters" );
		_page=config->readNumEntry( "NbPage",1 ) ;
	}

	for( int i=0; i<_page; i++ )
	{
		KSpreadSheet *t = createTable();
		m_pMap->addTable( t );
	}

	resetURL();
	setEmpty();
	initConfig();
        m_pStyleManager->createBuiltinStyles();
	return true;
    }

    if ( ret == KoTemplateChooseDia::Template )
    {
        QFileInfo fileInfo( f );
        QString fileName( fileInfo.dirPath( true ) + "/" +
            fileInfo.baseName() + ".kst" );
        resetURL();
        loadNativeFormat( fileName );
        setEmpty();
        initConfig();
        return true;
    }

    return false;
}


void KSpreadDoc::saveConfig()
{
    if ( isEmbedded() ||!isReadWrite())
        return;
    KConfig *config = KSpreadFactory::global()->config();
    config->setGroup( "Parameters" );
    config->writeEntry( "Zoom", m_zoom );

}

void KSpreadDoc::initConfig()
{
    KSpellConfig ksconfig;
    KConfig *config = KSpreadFactory::global()->config();
    if( config->hasGroup("KSpell kspread" ) )
    {
        config->setGroup( "KSpell kspread" );
        ksconfig.setNoRootAffix(config->readNumEntry ("KSpell_NoRootAffix", 0));
        ksconfig.setRunTogether(config->readNumEntry ("KSpell_RunTogether", 0));
        ksconfig.setDictionary(config->readEntry ("KSpell_Dictionary", ""));
        ksconfig.setDictFromList(config->readNumEntry ("KSpell_DictFromList", FALSE));
        ksconfig.setEncoding(config->readNumEntry ("KSpell_Encoding", KS_E_ASCII));
        ksconfig.setClient(config->readNumEntry ("KSpell_Client", KS_CLIENT_ISPELL));
        setKSpellConfig(ksconfig);

        setDontCheckUpperWord(config->readBoolEntry("KSpell_IgnoreUppercaseWords", false));
        setDontCheckTitleCase(config->readBoolEntry("KSpell_IgnoreTitleCaseWords", false));
    }
    if( config->hasGroup("KSpread Page Layout" ))
    {
      config->setGroup( "KSpread Page Layout" );
      setUnit( (KoUnit::Unit)config->readNumEntry( "Default unit page" ,0));
    }
    if( config->hasGroup("Parameters" ))
    {
        config->setGroup( "Parameters" );
        m_zoom = config->readNumEntry( "Zoom", 100 );
    }
    else
      m_zoom = 100;

    setZoomAndResolution( m_zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
}

KoView* KSpreadDoc::createViewInstance( QWidget* parent, const char* name )
{
    if ( name == 0 )
        name = "View";
    return new KSpreadView( parent, name, this );
}

bool KSpreadDoc::saveChildren( KoStore* _store )
{
  return m_pMap->saveChildren( _store );
}

QDomDocument KSpreadDoc::saveXML()
{
    //Terminate current cell edition, if any
    QPtrListIterator<KoView> it( views() );

    /* don't pull focus away from the editor if this is just a background
       autosave */
    if (!isAutosaving())
    {
        for (; it.current(); ++it )
            static_cast<KSpreadView *>( it.current() )->deleteEditor( true );
    }

    QDomDocument doc = createDomDocument( "spreadsheet", CURRENT_DTD_VERSION );
    QDomElement spread = doc.documentElement();
    spread.setAttribute( "editor", "KSpread" );
    spread.setAttribute( "mime", "application/x-kspread" );
    spread.setAttribute( "syntaxVersion", CURRENT_SYNTAX_VERSION );

    /* Backwards compatibility with KSpread < 1.2
       Looks like a hack, but it saves us to define an export filter for this issue.

       In KSpread < 1.2, the paper format was per map, since 1.2 it's per sheet.
       To enable KSpread < 1.2 to open these files, we store the page layout of the first sheet
       for the whole map as the map paper layout. */
    if ( specialOutputFlag() == KoDocument::SaveAsKOffice1dot1 /* so it's KSpread < 1.2 */)
    {
        KSpreadSheetPrint* printObject = m_pMap->firstTable()->print();

        QDomElement paper = doc.createElement( "paper" );
        paper.setAttribute( "format", printObject->paperFormatString() );
        paper.setAttribute( "orientation", printObject->orientationString() );
        spread.appendChild( paper );
        QDomElement borders = doc.createElement( "borders" );
        borders.setAttribute( "left", printObject->leftBorder() );
        borders.setAttribute( "top", printObject->topBorder() );
        borders.setAttribute( "right", printObject->rightBorder() );
        borders.setAttribute( "bottom", printObject->bottomBorder() );
        paper.appendChild( borders );
        QDomElement head = doc.createElement( "head" );
        paper.appendChild( head );
        if ( !printObject->headLeft().isEmpty() )
        {
            QDomElement left = doc.createElement( "left" );
            head.appendChild( left );
            left.appendChild( doc.createTextNode( printObject->headLeft() ) );
        }
        if ( !printObject->headMid().isEmpty() )
        {
            QDomElement center = doc.createElement( "center" );
            head.appendChild( center );
            center.appendChild( doc.createTextNode( printObject->headMid() ) );
        }
        if ( !printObject->headRight().isEmpty() )
        {
            QDomElement right = doc.createElement( "right" );
            head.appendChild( right );
            right.appendChild( doc.createTextNode( printObject->headRight() ) );
        }
        QDomElement foot = doc.createElement( "foot" );
        paper.appendChild( foot );
        if ( !printObject->footLeft().isEmpty() )
        {
            QDomElement left = doc.createElement( "left" );
            foot.appendChild( left );
            left.appendChild( doc.createTextNode( printObject->footLeft() ) );
        }
        if ( !printObject->footMid().isEmpty() )
        {
            QDomElement center = doc.createElement( "center" );
            foot.appendChild( center );
            center.appendChild( doc.createTextNode( printObject->footMid() ) );
        }
        if ( !printObject->footRight().isEmpty() )
        {
            QDomElement right = doc.createElement( "right" );
            foot.appendChild( right );
            right.appendChild( doc.createTextNode( printObject->footRight() ) );
        }
    }

    QDomElement locale = m_locale.save( doc );
    spread.appendChild( locale );

    if (m_refs.count() != 0 )
    {
        QDomElement areaname = saveAreaName( doc );
        spread.appendChild( areaname );
    }

    if( !m_spellListIgnoreAll.isEmpty() )
    {
        QDomElement spellCheckIgnore = doc.createElement( "SPELLCHECKIGNORELIST" );
        spread.appendChild( spellCheckIgnore );
        for ( QStringList::Iterator it = m_spellListIgnoreAll.begin(); it != m_spellListIgnoreAll.end(); ++it )
        {
            QDomElement spellElem = doc.createElement( "SPELLCHECKIGNOREWORD" );
            spellCheckIgnore.appendChild( spellElem );
            spellElem.setAttribute( "word", *it );
        }
    }

    SavedDocParts::const_iterator iter = m_savedDocParts.begin();
    SavedDocParts::const_iterator end  = m_savedDocParts.end();
    while ( iter != end )
    {
      // save data we loaded in the beginning and which has no owner back to file
      spread.appendChild( iter.data() );
      ++iter;
    }

    QDomElement defaults = doc.createElement( "defaults" );
    defaults.setAttribute( "row-height", KSpreadFormat::globalRowHeight() );
    defaults.setAttribute( "col-width", KSpreadFormat::globalColWidth() );
    spread.appendChild( defaults );

    KSpreadPlugin * plugin = m_plugins.first();
    for ( ; plugin != 0; plugin = m_plugins.next() )
    {
      QDomElement data( plugin->saveXML( doc ) );
      if ( !data.isNull() )
        spread.appendChild( data );
    }

    QDomElement s = m_pStyleManager->save( doc );
    spread.appendChild( s );

    QDomElement e = m_pMap->save( doc );
    spread.appendChild( e );

    setModified( false );

    return doc;
}

bool KSpreadDoc::loadChildren( KoStore* _store )
{
    return m_pMap->loadChildren( _store );
}

bool KSpreadDoc::loadXML( QIODevice *, const QDomDocument& doc )
{
  QTime dt;
  dt.start();

  emit sigProgress( 0 );
  m_bLoading = TRUE;
  m_spellListIgnoreAll.clear();
  // <spreadsheet>
  QDomElement spread = doc.documentElement();

  if ( spread.attribute( "mime" ) != "application/x-kspread" && spread.attribute( "mime" ) != "application/vnd.kde.kspread" )
  {
    m_bLoading = false;
    setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-kspread or application/vnd.kde.kspread, got %1" ).arg( spread.attribute("mime") ) );
    return false;
  }

  m_syntaxVersion = KSpreadDoc::getAttribute( spread, "syntaxVersion", 0 );
  if ( m_syntaxVersion > CURRENT_SYNTAX_VERSION )
  {
      int ret = KMessageBox::warningContinueCancel(
          0, i18n("This document was created with a newer version of KSpread (syntax version: %1)\n"
                  "When you open it with this version of KSpread, some information may be lost.").arg(m_syntaxVersion),
          i18n("File Format Mismatch"), i18n("Continue") );
      if ( ret == KMessageBox::Cancel )
      {
          setErrorMessage( "USER_CANCELED" );
          return false;
      }
  }

  // <locale>
  QDomElement locale = spread.namedItem( "locale" ).toElement();
  if ( !locale.isNull() )
      m_locale.load( locale );

  emit sigProgress( 5 );

  QDomElement defaults = spread.namedItem( "defaults" ).toElement();
  if ( !defaults.isNull() )
  {
    bool ok = false;
    double d = defaults.attribute( "row-height" ).toDouble( &ok );
    if ( !ok )
      return false;
    KSpreadFormat::setGlobalRowHeight( d );

    d = defaults.attribute( "col-width" ).toDouble( &ok );

    if ( !ok )
      return false;

    KSpreadFormat::setGlobalColWidth( d );
  }

  m_refs.clear();
  //<areaname >
  QDomElement areaname = spread.namedItem( "areaname" ).toElement();
  if ( !areaname.isNull())
    loadAreaName(areaname);

  QDomElement ignoreAll = spread.namedItem( "SPELLCHECKIGNORELIST").toElement();
  if ( !ignoreAll.isNull())
  {
      QDomElement spellWord=spread.namedItem("SPELLCHECKIGNORELIST").toElement();

      spellWord=spellWord.firstChild().toElement();
      while ( !spellWord.isNull() )
      {
          if ( spellWord.tagName()=="SPELLCHECKIGNOREWORD" )
          {
              m_spellListIgnoreAll.append(spellWord.attribute("word"));
          }
          spellWord=spellWord.nextSibling().toElement();
      }
  }

  emit sigProgress( 40 );
  // In case of reload (e.g. from konqueror)
  m_pMap->tableList().clear(); // it's set to autoDelete

  QDomElement styles = spread.namedItem( "styles" ).toElement();
  if ( !styles.isNull() )
  {
    if ( !m_pStyleManager->loadXML( styles ) )
    {
      setErrorMessage( i18n( "Styles cannot be loaded." ) );
      m_bLoading = false;
      return false;
    }
  }

  // <map>
  QDomElement mymap = spread.namedItem( "map" ).toElement();
  if ( mymap.isNull() )
  {
      setErrorMessage( i18n("Invalid document. No map tag.") );
      m_bLoading = false;
      return false;
  }
  if ( !m_pMap->loadXML( mymap ) )
  {
      m_bLoading = false;
      return false;
  }

  //Backwards compatibility with older versions for paper layout
  if ( m_syntaxVersion < 1 )
  {
    QDomElement paper = spread.namedItem( "paper" ).toElement();
    if ( !paper.isNull() )
    {
      loadPaper( paper );
    }
  }

  emit sigProgress( 85 );

  QDomElement element( spread.firstChild().toElement() );
  while ( !element.isNull() )
  {
    QString tagName( element.tagName() );

    if ( tagName != "locale" && tagName != "map" && tagName != "styles"
         && tagName != "SPELLCHECKIGNORELIST" && tagName != "areaname"
         && tagName != "paper" )
    {
      // belongs to a plugin, load it and save it for later use
      m_savedDocParts[ tagName ] = element;
    }

    element = element.nextSibling().toElement();
  }

  emit sigProgress( 90 );
  initConfig();
  emit sigProgress(-1);

   kdDebug(36001) << "Loading took " << (float)(dt.elapsed()) / 1000.0 << " seconds" << endl;

  return true;
}

void KSpreadDoc::loadPaper( QDomElement const & paper )
{
  // <paper>
  QString format = paper.attribute( "format" );
  QString orientation = paper.attribute( "orientation" );

  // <borders>
  QDomElement borders = paper.namedItem( "borders" ).toElement();
  if ( !borders.isNull() )
  {
    float left = borders.attribute( "left" ).toFloat();
    float right = borders.attribute( "right" ).toFloat();
    float top = borders.attribute( "top" ).toFloat();
    float bottom = borders.attribute( "bottom" ).toFloat();

    //apply to all tables
    QPtrListIterator<KSpreadSheet> it ( m_pMap->tableList() );
    for( ; it.current(); ++it )
    {
      it.current()->print()->setPaperLayout( left, top, right, bottom,
                                             format, orientation );
    }
  }

  QString hleft, hright, hcenter;
  QString fleft, fright, fcenter;
  // <head>
  QDomElement head = paper.namedItem( "head" ).toElement();
  if ( !head.isNull() )
  {
    QDomElement left = head.namedItem( "left" ).toElement();
    if ( !left.isNull() )
      hleft = left.text();
    QDomElement center = head.namedItem( "center" ).toElement();
    if ( !center.isNull() )
      hcenter = center.text();
    QDomElement right = head.namedItem( "right" ).toElement();
    if ( !right.isNull() )
      hright = right.text();
  }
  // <foot>
  QDomElement foot = paper.namedItem( "foot" ).toElement();
  if ( !foot.isNull() )
  {
    QDomElement left = foot.namedItem( "left" ).toElement();
    if ( !left.isNull() )
      fleft = left.text();
    QDomElement center = foot.namedItem( "center" ).toElement();
    if ( !center.isNull() )
      fcenter = center.text();
    QDomElement right = foot.namedItem( "right" ).toElement();
    if ( !right.isNull() )
      fright = right.text();
  }
  //The macro "<sheet>" formerly was typed as "<table>"
  hleft   = hleft.replace(   "<table>", "<sheet>" );
  hcenter = hcenter.replace( "<table>", "<sheet>" );
  hright  = hright.replace(  "<table>", "<sheet>" );
  fleft   = fleft.replace(   "<table>", "<sheet>" );
  fcenter = fcenter.replace( "<table>", "<sheet>" );
  fright  = fright.replace(  "<table>", "<sheet>" );

  QPtrListIterator<KSpreadSheet> it ( m_pMap->tableList() );
  for( ; it.current(); ++it )
  {
    it.current()->print()->setHeadFootLine( hleft, hcenter, hright,
                                            fleft, fcenter, fright);
  }
}

bool KSpreadDoc::completeLoading( KoStore* /* _store */ )
{
  kdDebug(36001) << "------------------------ COMPLETING --------------------" << endl;

  m_bLoading = false;

  //  m_pMap->update();

  kdDebug(36001) << "------------------------ COMPLETION DONE --------------------" << endl;

  setModified( FALSE );
  return true;
}

void KSpreadDoc::registerPlugin( KSpreadPlugin * plugin )
{
  m_plugins.append( plugin );
}

void KSpreadDoc::deregisterPlugin( KSpreadPlugin * plugin )
{
  m_plugins.remove( plugin );
}

bool KSpreadDoc::docData( QString const & xmlTag, QDomElement & data )
{
  SavedDocParts::iterator iter = m_savedDocParts.find( xmlTag );
  if ( iter == m_savedDocParts.end() )
    return false;

  data = iter.data();
  m_savedDocParts.erase( iter );

  return true;
}

void KSpreadDoc::setDefaultGridPen( const QPen& p )
{
  m_defaultGridPen = p;
}

KSpreadSheet* KSpreadDoc::createTable()
{
  QString s( i18n("Sheet%1") );
  s = s.arg( m_iTableId++ );
  //KSpreadSheet *t = new KSpreadSheet( m_pMap, s.latin1() );
  KSpreadSheet *t = new KSpreadSheet( m_pMap, s,s.utf8() );
  t->setTableName( s, TRUE ); // huh? (Werner)
  return t;
}

void KSpreadDoc::resetInterpreter()
{
  destroyInterpreter();
  initInterpreter();

  // Update the cell content
  // TODO
  /* KSpreadSheet *t;
  for ( t = m_pMap->firstTable(); t != 0L; t = m_pMap->nextTable() )
  t->initInterpreter(); */

  // Perhaps something changed. Lets repaint
  emit sig_updateView();
}


void KSpreadDoc::addTable( KSpreadSheet *_table )
{
  m_pMap->addTable( _table );

  setModified( TRUE );

  emit sig_addTable( _table );
}

void KSpreadDoc::setZoomAndResolution( int zoom, int dpiX, int dpiY )
{
    KoZoomHandler::setZoomAndResolution( zoom, dpiX, dpiY );
}

void KSpreadDoc::newZoomAndResolution( bool updateViews, bool /*forPrint*/ )
{
/*    layout();
    updateAllFrames();*/
    if ( updateViews )
    {
        emit sig_refreshView();
    }
}

void KSpreadDoc::initInterpreter()
{
  m_pInterpreter = new KSpreadInterpreter( this );

  // Create the module which is used to evaluate all formulas
  m_module = m_pInterpreter->module( "kspread" );
  m_context.setScope( new KSScope( m_pInterpreter->globalNamespace(), m_module ) );

  // Find all scripts
  m_kscriptModules = KSpreadFactory::global()->dirs()->findAllResources( "extensions", "*.ks", TRUE );

  // Remove dupes
  QMap<QString,QString> m;
  for( QStringList::Iterator it = m_kscriptModules.begin(); it != m_kscriptModules.end(); ++it )
  {
    int pos = (*it).findRev( '/' );
    if ( pos != -1 )
    {
      QString name = (*it).mid( pos + 1 );
      pos = name.find( '.' );
      if ( pos != -1 )
        name = name.left( pos );
      m[ name ] = *it;
    }
  }

  // Load and execute the scripts
  QMap<QString,QString>::Iterator mip = m.begin();
  for( ; mip != m.end(); ++mip )
  {
    kdDebug(36001) << "SCRIPT="<<  mip.key() << ", " << mip.data() << endl;
    KSContext context;
    QStringList args;
    if ( !m_pInterpreter->runModule( context, mip.key(), mip.data(), args ) )
    {
        if ( context.exception() )
            KMessageBox::error( 0L, context.exception()->toString( context ) );
        // else ... well, nothing to show...
    }
  }
}

void KSpreadDoc::destroyInterpreter()
{
    m_context.setValue( 0 );
    m_context.setScope( 0 );
    m_context.setException( 0 );

    m_module = 0;

    m_pInterpreter = 0;
}

void KSpreadDoc::undo()
{
  m_pUndoBuffer->undo();
}

void KSpreadDoc::redo()
{
  m_pUndoBuffer->redo();
}

void KSpreadDoc::enableUndo( bool _b )
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
      static_cast<KSpreadView *>( it.current() )->enableUndo( _b );
}

void KSpreadDoc::enableRedo( bool _b )
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
      static_cast<KSpreadView *>( it.current() )->enableRedo( _b );
}

void KSpreadDoc::paintContent( QPainter& painter, const QRect& rect,
                               bool transparent, double zoomX, double zoomY )
{
  //  ElapsedTime et( "KSpreadDoc::paintContent1" );
    //kdDebug(36001) << "KSpreadDoc::paintContent m_zoom=" << m_zoom << " zoomX=" << zoomX << " zoomY=" << zoomY << " transparent=" << transparent << endl;

    // save current zoom
    int oldZoom = m_zoom;

    // choose sheet: the first or the active
    KSpreadSheet* table = 0L;
    if ( !m_activeTable )
        table = m_pMap->firstTable();
    else
        table = m_activeTable;
    if ( !table )
        return;

    // only one zoom is supported
    double d_zoom = 1.0;
    setZoomAndResolution( 100, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
    if ( m_zoomedResolutionX != zoomX )
        d_zoom *= ( zoomX / m_zoomedResolutionX );

    // KSpread support zoom, therefore no need to scale with worldMatrix
    QWMatrix matrix = painter.worldMatrix();
    matrix.setMatrix( 1, 0, 0, 1, matrix.dx(), matrix.dy() );
    QRect prect = rect;
    prect.setWidth( prect.width() * painter.worldMatrix().m11() );
    prect.setHeight( prect.height() * painter.worldMatrix().m22() );
    setZoomAndResolution( d_zoom * 100, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );

    // paint the content, now zoom is correctly set
    kdDebug(36001)<<"paintContent-------------------------------------\n";
    painter.save();
    painter.setWorldMatrix( matrix );
    paintContent( painter, prect, transparent, table, false );
    painter.restore();

    // restore zoom
    m_zoom = oldZoom;
    setZoomAndResolution( oldZoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
}

void KSpreadDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/, KSpreadSheet* table, bool drawCursor )
{
    if ( isLoading() )
        return;
    //    ElapsedTime et( "KSpreadDoc::paintContent2" );

    // if ( !transparent )
    // painter.eraseRect( rect );

    double xpos;
    double ypos;
    int left_col  = table->leftColumn( unzoomItX( rect.x() ), xpos );
    int right_col = table->rightColumn( unzoomItX( rect.right() ) );
    int top_row = table->topRow( unzoomItY( rect.y() ), ypos );
    int bottom_row = table->bottomRow( unzoomItY( rect.bottom() ) );

    QPen pen;
    pen.setWidth( 1 );
    painter.setPen( pen );
    /* update the entire visible area */

    QValueList<QRect> cellAreaList;
    cellAreaList.append( QRect( left_col,
                                top_row,
                                right_col - left_col + 1,
                                bottom_row - top_row + 1) );

    paintCellRegions(painter, rect, NULL, cellAreaList, table, drawCursor);
}

void KSpreadDoc::paintUpdates()
{
  //  ElapsedTime et( "KSpreadDoc::paintUpdates" );

  QPtrListIterator<KoView> it( views() );
  KSpreadView  * view  = NULL;
  KSpreadSheet * table = NULL;

  for (; it.current(); ++it )
  {
    view = static_cast<KSpreadView *>( it.current() );
    view->paintUpdates();
  }

  for (table = m_pMap->firstTable(); table != NULL;
       table = m_pMap->nextTable())
  {
    table->clearPaintDirtyData();
  }
}

void KSpreadDoc::paintCellRegions(QPainter& painter, const QRect &viewRect,
                                  KSpreadView* view,
                                  QValueList<QRect> cellRegions,
                                  const KSpreadSheet* table, bool drawCursor)
{
  //
  // Clip away children
  //

  QRegion rgn = painter.clipRegion();
  if ( rgn.isEmpty() )
    rgn = QRegion( QRect( 0, 0, viewRect.width(), viewRect.height() ) );

  QWMatrix matrix;
  if ( view )
  {
    matrix.scale( zoomedResolutionX(),
                  zoomedResolutionY() );
    matrix.translate( - view->canvasWidget()->xOffset(),
                      - view->canvasWidget()->yOffset() );
  }
  else
  {
    matrix = painter.worldMatrix();
  }

  QPtrListIterator<KoDocumentChild> it( children() );
  for( ; it.current(); ++it )
  {
    // if ( ((KSpreadChild*)it.current())->table() == table &&
    //    !m_pView->hasDocumentInWindow( it.current()->document() ) )
    if ( ((KSpreadChild*)it.current())->table() == table)
      rgn -= it.current()->region( matrix );
  }
  painter.setClipRegion( rgn );

  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );

  QRect cellRegion;
  KoRect unzoomedViewRect = unzoomRect( viewRect );

  for (unsigned int i=0; i < cellRegions.size(); i++)
  {
    cellRegion = cellRegions[i];

    PaintRegion(painter, unzoomedViewRect, view, cellRegion, table);
  }

  if ((view != NULL) && drawCursor && !(painter.device()->isExtDev()))
  {
    if (view->activeTable() == table)
    {
      //    PaintNormalMarker(painter, unzoomedViewRect, view, table, view->selection());
      ;
    }

    if (view->selectionInfo()->getChooseTable() == table)
    {
//      PaintChooseRect(painter, unzoomedViewRect, view, table, view->selectionInfo()->getChooseRect());
      ;
    }
  }
}


void KSpreadDoc::PaintRegion(QPainter &painter, const KoRect &viewRegion,
                             KSpreadView* view, const QRect &paintRegion,
                             const KSpreadSheet* table)
{
  /* paint region has cell coordinates (col,row) while viewRegion has world
     coordinates.  paintRegion is the cells to update and viewRegion is the
     area actually onscreen.
  */

  if ( paintRegion.left() <= 0 || paintRegion.top() <= 0 )
    return;

  /*
    get the world coordinates of the upper left corner of the paintRegion
    The view is NULL, when paintRegion is called from paintContent, which itself
    is only called, when we should paint the output for INACTIVE embedded view.
    If inactive embedded, then there is no view and we alwas start at top/left,
    so the offset is 0.
  */

  KoPoint dblCorner;
  if ( view == 0L ) //Most propably we are embedded and inactive, so no offset
        dblCorner = KoPoint( table->dblColumnPos( paintRegion.left() ),
                             table->dblRowPos( paintRegion.top() ) );
  else
        dblCorner = KoPoint( table->dblColumnPos( paintRegion.left() ) -
                               view->canvasWidget()->xOffset(),
                             table->dblRowPos( paintRegion.top() ) -
                               view->canvasWidget()->yOffset() );
  KoPoint dblCurrentCellPos( dblCorner );

  int regionBottom = paintRegion.bottom();
  int regionRight  = paintRegion.right();
  int regionLeft   = paintRegion.left();
  int regionTop    = paintRegion.top();

  for ( int y = regionTop;
        y <= regionBottom && dblCurrentCellPos.y() <= viewRegion.bottom();
        ++y )
  {
    const RowFormat * row_lay = table->rowFormat( y );
    dblCurrentCellPos.setX( dblCorner.x() );

    for ( int x = regionLeft;
          x <= regionRight && dblCurrentCellPos.x() <= viewRegion.right();
          ++x )
    {
      const ColumnFormat *col_lay = table->columnFormat( x );
      KSpreadCell* cell = table->cellAt( x, y );

      QPoint cellRef( x, y );

      bool paintBordersBottom = false;
      bool paintBordersRight = false;
      bool paintBordersLeft = false;
      bool paintBordersTop = false;

      QPen rightPen( cell->effRightBorderPen( x, y ) );
      QPen leftPen( cell->effLeftBorderPen( x, y ) );
      QPen topPen( cell->effTopBorderPen( x, y ) );
      QPen bottomPen( cell->effBottomBorderPen( x, y ) );

      // paint right border if rightmost cell or if the pen is more "worth" than the left border pen
      // of the cell on the left or if the cell on the right is not painted. In the latter case get
      // the pen that is of more "worth"
      if ( x >= KS_colMax )
        paintBordersRight = true;
      else
        if ( x == regionRight )
        {
          paintBordersRight = true;
          if ( cell->effRightBorderValue( x, y ) < table->cellAt( x + 1, y )->effLeftBorderValue( x + 1, y ) )
            rightPen = table->cellAt( x + 1, y )->effLeftBorderPen( x + 1, y );
        }
      else
      {
        paintBordersRight = true;
        if ( cell->effRightBorderValue( x, y ) < table->cellAt( x + 1, y )->effLeftBorderValue( x + 1, y ) )
          rightPen = table->cellAt( x + 1, y )->effLeftBorderPen( x + 1, y );
      }

      // similiar for other borders...
      // bottom border:
      if ( y >= KS_rowMax )
        paintBordersBottom = true;
      else
        if ( y == regionBottom )
        {
          paintBordersBottom = true;
          if ( cell->effBottomBorderValue( x, y ) < table->cellAt( x, y + 1 )->effTopBorderValue( x, y + 1) )
            bottomPen = table->cellAt( x, y + 1 )->effTopBorderPen( x, y + 1 );
        }
      else
      {
        paintBordersBottom = true;
        if ( cell->effBottomBorderValue( x, y ) < table->cellAt( x, y + 1 )->effTopBorderValue( x, y + 1) )
          bottomPen = table->cellAt( x, y + 1 )->effTopBorderPen( x, y + 1 );
      }

      // left border:
      if ( x == 1 )
        paintBordersLeft = true;
      else
        if ( x == regionLeft )
        {
          paintBordersLeft = true;
          if ( cell->effLeftBorderValue( x, y ) < table->cellAt( x - 1, y )->effRightBorderValue( x - 1, y ) )
            leftPen = table->cellAt( x - 1, y )->effRightBorderPen( x - 1, y );
        }
      else
      {
        paintBordersLeft = true;
        if ( cell->effLeftBorderValue( x, y ) < table->cellAt( x - 1, y )->effRightBorderValue( x - 1, y ) )
          leftPen = table->cellAt( x - 1, y )->effRightBorderPen( x - 1, y );
      }

      // top border:
      if ( y == 1 )
        paintBordersTop = true;
      else
        if ( y == regionTop )
        {
          paintBordersTop = true;
          if ( cell->effTopBorderValue( x, y ) < table->cellAt( x, y - 1 )->effBottomBorderValue( x, y - 1 ) )
            topPen = table->cellAt( x, y - 1 )->effBottomBorderPen( x, y - 1 );
        }
      else
      {
        paintBordersTop = true;
        if ( cell->effTopBorderValue( x, y ) < table->cellAt( x, y - 1 )->effBottomBorderValue( x, y - 1 ) )
          topPen = table->cellAt( x, y - 1 )->effBottomBorderPen( x, y - 1 );
      }

      cell->paintCell( viewRegion, painter, view, dblCurrentCellPos,
                       cellRef, paintBordersRight, paintBordersBottom, paintBordersLeft,
                       paintBordersTop, rightPen, bottomPen, leftPen, topPen, false );

      dblCurrentCellPos.setX( dblCurrentCellPos.x() + col_lay->dblWidth() );
    }
    dblCurrentCellPos.setY( dblCurrentCellPos.y() + row_lay->dblHeight() );
  }
}

void KSpreadDoc::PaintChooseRect(QPainter& painter, const KoRect &viewRect,
                                 KSpreadView* view, const KSpreadSheet* table,
				 const QRect &chooseRect)
{
  double positions[4];
  bool paintSides[4];

  if ( chooseRect.left() != 0 )
  {
    QPen pen;
    pen.setWidth( 2 );
    pen.setStyle( DashLine );

    retrieveMarkerInfo( chooseRect, table, view, viewRect, positions, paintSides );

    double left =   positions[0];
    double top =    positions[1];
    double right =  positions[2];
    double bottom = positions[3];

    bool paintLeft =   paintSides[0];
    bool paintTop =    paintSides[1];
    bool paintRight =  paintSides[2];
    bool paintBottom = paintSides[3];

    RasterOp rop = painter.rasterOp();
    painter.setRasterOp( NotROP );
    painter.setPen( pen );

    if ( paintTop )
    {
      painter.drawLine( zoomItX( left ),  zoomItY( top ),
                        zoomItX( right ), zoomItY( top ) );
    }
    if ( paintLeft )
    {
      painter.drawLine( zoomItX( left ), zoomItY( top ),
                        zoomItX( left ), zoomItY( bottom ) );
    }
    if ( paintRight )
    {
      painter.drawLine( zoomItX( right ), zoomItY( top ),
                        zoomItX( right ), zoomItY( bottom ) );
    }
    if ( paintBottom )
    {
      painter.drawLine( zoomItX( left ),  zoomItY( bottom ),
                        zoomItX( right ), zoomItY( bottom ) );
    }

    /* restore the old raster mode */
    painter.setRasterOp( rop );
  }
  return;
}

void KSpreadDoc::PaintNormalMarker(QPainter& painter, const KoRect &viewRect,
                                   KSpreadView* view, const KSpreadSheet* table,
                                   const QRect &marker)
{
  double positions[4];
  bool paintSides[4];

  QPen pen( Qt::black, 3 );
  painter.setPen( pen );

  retrieveMarkerInfo( marker, table, view, viewRect, positions, paintSides );

  painter.setPen( pen );

  double left =   positions[0];
  double top =    positions[1];
  double right =  positions[2];
  double bottom = positions[3];

  bool paintLeft =   paintSides[0];
  bool paintTop =    paintSides[1];
  bool paintRight =  paintSides[2];
  bool paintBottom = paintSides[3];

  /* the extra '-1's thrown in here account for the thickness of the pen.
     want to look like this:                     not this:
                            * * * * * *                     * * * *
                            *         *                   *         *
     .                      *         *                   *         *
  */
  int l = 1;

  if( paintTop )
  {
    painter.drawLine( zoomItX( left ) - l,      zoomItY( top ),
                      zoomItX( right ) + 2 * l, zoomItY( top ) );
  }
  if( paintLeft )
  {
    painter.drawLine( zoomItX( left ), zoomItY( top ),
                      zoomItX( left ), zoomItY( bottom ) );
  }
  if( paintRight && paintBottom )
  {
    /* then the 'handle' in the bottom right corner is visible. */
    painter.drawLine( zoomItX( right ), zoomItY( top ),
                      zoomItX( right ), zoomItY( bottom ) - 3 );
    painter.drawLine( zoomItX( left ) - l,  zoomItY( bottom ),
                      zoomItX( right ) - 3, zoomItY( bottom ) );
    painter.fillRect( zoomItX( right ) - 2, zoomItY( bottom ) - 2, 5, 5,
                      painter.pen().color() );
  }
  else
  {
    if( paintRight )
    {
      painter.drawLine( zoomItX( right ), zoomItY( top ),
                        zoomItX( right ), zoomItY( bottom ) );
    }
    if( paintBottom )
    {
      painter.drawLine( zoomItX( left ) - l,  zoomItY( bottom ),
                        zoomItX( right ) + l, zoomItY( bottom ) );
    }
  }
}


void KSpreadDoc::retrieveMarkerInfo( const QRect &marker,
                                     const KSpreadSheet* table,
                                     KSpreadView* view,
                                     const KoRect &viewRect,
                                     double positions[],
                                     bool paintSides[] )
{
  double xpos = table->dblColumnPos( marker.left() ) -
                view->canvasWidget()->xOffset();
  double ypos = table->dblRowPos( marker.top() ) -
                view->canvasWidget()->yOffset();

  double x = table->dblColumnPos( marker.right() ) -
             view->canvasWidget()->xOffset();
  const ColumnFormat *columnFormat = table->columnFormat( marker.right() );
  double tw = columnFormat->dblWidth( );
  double w = ( x - xpos ) + tw;

  double y = table->dblRowPos( marker.bottom() ) -
             view->canvasWidget()->yOffset();
  const RowFormat* rowFormat = table->rowFormat( marker.bottom() );
  double th = rowFormat->dblHeight( );
  double h = ( y - ypos ) + th;

  /* left, top, right, bottom */
  positions[0] = xpos;
  positions[1] = ypos;
  positions[2] = xpos + w;
  positions[3] = ypos + h;

  /* these vars are used for clarity, the array for simpler function arguments  */
  double left = positions[0];
  double top = positions[1];
  double right = positions[2];
  double bottom = positions[3];

  /* left, top, right, bottom */
  paintSides[0] = (viewRect.left() <= left) && (left <= viewRect.right()) &&
                  (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  paintSides[1] = (viewRect.top() <= top) && (top <= viewRect.bottom()) &&
                  (right >= viewRect.left()) && (left <= viewRect.right());
  paintSides[2] = (viewRect.left() <= right ) && (right <= viewRect.right()) &&
                  (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  paintSides[3] = (viewRect.top() <= bottom) && (bottom <= viewRect.bottom()) &&
                  (right >= viewRect.left()) && (left <= viewRect.right());

  positions[0] = QMAX( left,   viewRect.left() );
  positions[1] = QMAX( top,    viewRect.top() );
  positions[2] = QMIN( right,  viewRect.right() );
  positions[3] = QMIN( bottom, viewRect.bottom() );
}


KSpreadDoc::~KSpreadDoc()
{
  //don't save config when kword is embedded into konqueror
  if(isReadWrite())
    saveConfig();
  destroyInterpreter();

  delete m_pUndoBuffer;

  delete m_dcop;
  s_docs->removeRef(this);
  kdDebug(36001) << "alive 1" << endl;
  delete m_pMap;
  delete m_pStyleManager;
  delete m_pKSpellConfig;
}

DCOPObject* KSpreadDoc::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KSpreadDocIface( this );

    return m_dcop;
}

void KSpreadDoc::addAreaName(const QRect &_rect,const QString & name,const QString & tableName)
{
  setModified( true );
  Reference tmp;
  tmp.rect = _rect;
  tmp.table_name = tableName;
  tmp.ref_name = name;
  m_refs.append( tmp);
}

void KSpreadDoc::removeArea( const QString & name)
{
  QValueList<Reference>::Iterator it2;
  for ( it2 = m_refs.begin(); it2 != m_refs.end(); ++it2 )
        {
        if((*it2).ref_name==name)
                {
                m_refs.remove(it2);
                return;
                }
        }
}

void KSpreadDoc::changeAreaTableName(const QString & oldName,const QString & tableName)
{
  QValueList<Reference>::Iterator it2;
  for ( it2 = m_refs.begin(); it2 != m_refs.end(); ++it2 )
        {
        if((*it2).table_name==oldName)
                   (*it2).table_name=tableName;
        }
}

QRect KSpreadDoc::getRectArea(const QString  &_tableName)
{
  QValueList<Reference>::Iterator it2;
  for ( it2 = m_refs.begin(); it2 != m_refs.end(); ++it2 )
        {
        if((*it2).ref_name==_tableName)
                {
                return (*it2).rect;
                }
        }
  return QRect(-1,-1,-1,-1);
}

QDomElement KSpreadDoc::saveAreaName( QDomDocument& doc )
{
   QDomElement element = doc.createElement( "areaname" );
   QValueList<Reference>::Iterator it2;
   for ( it2 = m_refs.begin(); it2 != m_refs.end(); ++it2 )
   {
        QDomElement e = doc.createElement("reference");
        QDomElement tabname = doc.createElement( "tabname" );
        tabname.appendChild( doc.createTextNode( (*it2).table_name ) );
        e.appendChild( tabname );

        QDomElement refname = doc.createElement( "refname" );
        refname.appendChild( doc.createTextNode( (*it2).ref_name ) );
        e.appendChild( refname );

        QDomElement rect = doc.createElement( "rect" );
        rect.setAttribute( "left-rect", ((*it2).rect).left() );
        rect.setAttribute( "right-rect",((*it2).rect).right() );
        rect.setAttribute( "top-rect", ((*it2).rect).top() );
        rect.setAttribute( "bottom-rect", ((*it2).rect).bottom() );
        e.appendChild( rect );
        element.appendChild(e);
   }
   return element;
}

void KSpreadDoc::loadAreaName( const QDomElement& element )
{
  QDomElement tmp=element.firstChild().toElement();
  for( ; !tmp.isNull(); tmp=tmp.nextSibling().toElement()  )
  {
    if ( tmp.tagName() == "reference" )
    {
        QString tabname;
        QString refname;
        int left=0;
        int right=0;
        int top=0;
        int bottom=0;
        QDomElement tableName = tmp.namedItem( "tabname" ).toElement();
        if ( !tableName.isNull() )
        {
          tabname=tableName.text();
        }
        QDomElement referenceName = tmp.namedItem( "refname" ).toElement();
        if ( !referenceName.isNull() )
        {
          refname=referenceName.text();
        }
        QDomElement rect =tmp.namedItem( "rect" ).toElement();
        if (!rect.isNull())
        {
          bool ok;
          if ( rect.hasAttribute( "left-rect" ) )
            left=rect.attribute("left-rect").toInt( &ok );
          if ( rect.hasAttribute( "right-rect" ) )
            right=rect.attribute("right-rect").toInt( &ok );
          if ( rect.hasAttribute( "top-rect" ) )
            top=rect.attribute("top-rect").toInt( &ok );
          if ( rect.hasAttribute( "bottom-rect" ) )
            bottom=rect.attribute("bottom-rect").toInt( &ok );
        }
        QRect _rect;
        _rect.setCoords(left,top,right,bottom);
        addAreaName(_rect,refname,tabname);
    }
  }
}

void KSpreadDoc::addStringCompletion(const QString &stringCompletion)
{
  if ( listCompletion.items().contains(stringCompletion) == 0 )
    listCompletion.addItem( stringCompletion );
}

void KSpreadDoc::refreshInterface()
{
  emit sig_refreshView();
}

void KSpreadDoc::setKSpellConfig(KSpellConfig _kspell)
{
  if (m_pKSpellConfig == 0 )
    m_pKSpellConfig = new KSpellConfig();

  m_pKSpellConfig->setNoRootAffix(_kspell.noRootAffix ());
  m_pKSpellConfig->setRunTogether(_kspell.runTogether ());
  m_pKSpellConfig->setDictionary(_kspell.dictionary ());
  m_pKSpellConfig->setDictFromList(_kspell.dictFromList());
  m_pKSpellConfig->setEncoding(_kspell.encoding());
  m_pKSpellConfig->setClient(_kspell.client());
}

void KSpreadDoc::refreshLocale()
{
    emit sig_refreshLocale();
}


void KSpreadDoc::emitBeginOperation(bool waitCursor)
{
    if (waitCursor)
    {
        QApplication::setOverrideCursor(Qt::waitCursor);
    }
    /* just duplicate the current cursor on the stack, then */
    else if (QApplication::overrideCursor() != NULL)
    {
        QApplication::setOverrideCursor(QApplication::overrideCursor()->shape());
    }

    KoDocument::emitBeginOperation();
    m_bDelayCalculation = true;
    m_numOperations++;
}

void KSpreadDoc::emitBeginOperation(void)
{
  emitBeginOperation(true);
}


void KSpreadDoc::emitEndOperation()
{
  //  ElapsedTime et( "*KSpreadDoc::emitEndOperation*" );
   KSpreadSheet *t = NULL;
   CellBinding* b = NULL;
   m_numOperations--;

   if (m_numOperations <= 0)
   {
     m_numOperations = 0;
     m_bDelayCalculation = false;
     for ( t = m_pMap->firstTable(); t != NULL; t = m_pMap->nextTable() )
     {
       //       ElapsedTime etm( "Updating table..." );
       t->update();

       // ElapsedTime etm2( "Sub: Updating cellbindings..." );
       for (b = t->firstCellBinding(); b != NULL; b = t->nextCellBinding())
       {
         b->cellChanged(NULL);
       }
     }
   }

   KoDocument::emitEndOperation();
   QApplication::restoreOverrideCursor();

   if (m_numOperations == 0)
   {
     /* do this after the parent class emitEndOperation because that allows updates
        on the view again
     */
     paintUpdates();
   }
}

void KSpreadDoc::emitEndOperation( QRect const & rect )
{
  // ElapsedTime et( "*KSpreadDoc::emitEndOperation - 2 -*" );
  CellBinding  * b = 0;
  m_numOperations--;

  if ( m_numOperations > 0 || !m_activeTable )
  {
    KoDocument::emitEndOperation();
    QApplication::restoreOverrideCursor();
    return;
  }

  m_numOperations = 0;
  m_bDelayCalculation = false;

  {
    //ElapsedTime etm( "Updating active table..." );
    m_activeTable->updateCellArea( rect );
  }

  //  ElapsedTime etm2( "Sub: Updating cellbindings..." );
  for ( b = m_activeTable->firstCellBinding(); b != 0; b = m_activeTable->nextCellBinding() )
  {
    b->cellChanged( 0 );
  }

  KoDocument::emitEndOperation();

  QApplication::restoreOverrideCursor();

  if ( m_numOperations == 0 )
  {
    /* do this after the parent class emitEndOperation because that allows updates
       on the view again
    */
    paintUpdates();
  }
}

bool KSpreadDoc::delayCalculation()
{
   return m_bDelayCalculation;
}

void KSpreadDoc::updateBorderButton()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
      static_cast<KSpreadView *>( it.current() )->updateBorderButton();
}

void KSpreadDoc::insertTable( KSpreadSheet * table )
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
	((KSpreadView*)it.current())->insertTable( table );
}

void KSpreadDoc::takeTable( KSpreadSheet * table )
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
	((KSpreadView*)it.current())->removeTable( table );
}

void KSpreadDoc::setUnit( KoUnit::Unit _unit )
{
    m_unit = _unit;
}

void KSpreadDoc::addIgnoreWordAll( const QString & word)
{
    if( m_spellListIgnoreAll.findIndex( word )==-1)
        m_spellListIgnoreAll.append( word );
}

void KSpreadDoc::clearIgnoreWordAll( )
{
    m_spellListIgnoreAll.clear();
}

void KSpreadDoc::setDisplayTable(KSpreadSheet *_table )
{
    m_activeTable = _table;
}

KSpreadSheet * KSpreadDoc::displayTable()const
{
    return m_activeTable;
}

void KSpreadDoc::addView( KoView *_view )
{
    KoDocument::addView( _view );
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
	((KSpreadView*)it.current())->closeEditor();
}


#include "kspread_doc.moc"

