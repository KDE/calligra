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

#include <qprinter.h>
#include <qmessagebox.h>

#include "kspread_doc.h"
#include "kspread_shell.h"
#include "kspread_interpreter.h"

#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>
#include <komlMime.h>

#include <fstream>
#include <string>

#include <unistd.h>
#include <qmsgbox.h>
#include <kurl.h>
#include <kapp.h>
#include <qdatetm.h>
#include <klocale.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include <kscript_context.h>
#include <kstddirs.h>

/*****************************************************************************
 *
 * KSpreadDoc
 *
 *****************************************************************************/

KSpreadDoc::KSpreadDoc()
{
  // The two supported interfaces
  ADD_INTERFACE( "IDL:KSpread/Document:1.0" );
  ADD_INTERFACE( "IDL:KOffice/Print:1.0" );

  m_iTableId = 1;

  m_leftBorder = 20.0;
  m_rightBorder = 20.0;
  m_topBorder = 20.0;
  m_bottomBorder = 20.0;
  m_paperFormat = PG_DIN_A4;
  m_paperWidth = PG_A4_WIDTH;
  m_paperHeight = PG_A4_HEIGHT;
  calcPaperSize();
  m_orientation = PG_PORTRAIT;
  m_pMap = 0L;
  m_bLoading = false;
  m_bEmpty = true;

  m_defaultGridPen.setColor( lightGray );
  m_defaultGridPen.setWidth( 1 );
  m_defaultGridPen.setStyle( SolidLine );

  initInterpreter();

  m_pMap = new KSpreadMap( this );

  m_pUndoBuffer = new KSpreadUndo( this );

  m_bModified = FALSE;

  m_lstViews.setAutoDelete( false );

  // DEBUG: print the IOR
  CORBA::String_var tmp = opapp_orb->object_to_string( this );
  cout << "DOC=" << tmp.in() << endl;
}

CORBA::Boolean KSpreadDoc::initDoc()
{
  KSpreadTable *t = createTable();
  m_pMap->addTable( t );

  return true;
}

void KSpreadDoc::cleanUp()
{
  cerr << "CLeanUp KSpreadDoc" << endl;

  if ( m_bIsClean )
    return;

  assert( m_lstViews.count() == 0 );

  if ( m_pMap )
  {
    delete m_pMap;
    m_pMap = 0L;
  }

  KoDocument::cleanUp();
}

KSpread::Book_ptr KSpreadDoc::book()
{
  return KSpread::Book::_duplicate( m_pMap );
}

void KSpreadDoc::removeView( KSpreadView* _view )
{
  m_lstViews.removeRef( _view );

  EMIT_EVENT( this, KSpread::Document::eventRemovedView, _view );
}

KSpreadView* KSpreadDoc::createSpreadView( QWidget* _parent )
{
  KSpreadView *p = new KSpreadView( _parent, 0L, this );
  //p->QWidget::show();
  m_lstViews.append( p );

  EMIT_EVENT( this, KSpread::Document::eventNewView, p );

  return p;
}

OpenParts::View_ptr KSpreadDoc::createView()
{
  return OpenParts::View::_duplicate( createSpreadView() );
}

void KSpreadDoc::viewList( OpenParts::Document::ViewList*& _list )
{
  (*_list).length( m_lstViews.count() );

  int i = 0;
  QListIterator<KSpreadView> it( m_lstViews );
  for( ; it.current(); ++it )
  {
    (*_list)[i++] = OpenParts::View::_duplicate( it.current() );
  }
}

int KSpreadDoc::viewCount()
{
  return m_lstViews.count();
}

bool KSpreadDoc::saveChildren( KOStore::Store_ptr _store, const char *_path )
{
  return m_pMap->saveChildren( _store, _path );
}

bool KSpreadDoc::hasToWriteMultipart()
{
  return m_pMap->hasToWriteMultipart();
}

bool KSpreadDoc::save( ostream& out, const char* /* format */ )
{
  out << "<?xml version=\"1.0\"?>" << endl;
  out << otag << "<DOC author=\"" << "Torben Weis" << "\" email=\"" << "weis@kde.org" << "\" editor=\"" << "KSpread"
      << "\" mime=\"" << "application/x-kspread" << "\" >" << endl;

  // TODO: Save KScript code in some CDATA here

  out << otag << "<PAPER format=\"" << paperFormatString() << "\" orientation=\"" << orientationString() << "\">" << endl;
  out << indent << "<PAPERBORDERS left=\"" << leftBorder() << "\" top=\"" << topBorder() << "\" right=\"" << rightBorder()
      << " bottom=\"" << bottomBorder() << "\"/>" << endl;
  out << indent << "<HEAD left=\"" << headLeft() << "\" center=\"" << headMid() << "\" right=\"" << headRight() << "\"/>" << endl;
  out << indent << "<FOOT left=\"" << footLeft() << "\" center=\"" << footMid() << "\" right=\"" << footRight() << "\"/>" << endl;
  out << etag << "</PAPER>" << endl;

  m_pMap->save( out );

  out << etag << "</DOC>" << endl;

  setModified( FALSE );

  return true;
}

bool KSpreadDoc::loadXML( KOMLParser& parser, KOStore::Store_ptr )
{
  cerr << "------------------------ LOADING --------------------" << endl;

  m_bLoading = true;

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  // DOC
  if ( !parser.open( "DOC", tag ) )
  {
    cerr << "Missing DOC" << endl;
    m_bLoading = false;
    return false;
  }

  KOMLParser::parseTag( tag.c_str(), name, lst );
  vector<KOMLAttrib>::const_iterator it = lst.begin();
  for( ; it != lst.end(); it++ )
  {
    if ( (*it).m_strName == "mime" )
    {
      if ( (*it).m_strValue != "application/x-kspread" )
      {
	cerr << "Unknown mime type " << (*it).m_strValue << endl;
	m_bLoading = false;
	return false;
      }
    }
  }

  // PAPER, MAP
  while( parser.open( 0L, tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );

    if ( name == "PAPER" )
    {
      KOMLParser::parseTag( tag.c_str(), name, lst );
      vector<KOMLAttrib>::const_iterator it = lst.begin();
      for( ; it != lst.end(); it++ )
      {
	if ( (*it).m_strName == "format" )
	{
	}
	else if ( (*it).m_strName == "orientation" )
	{
	}
	else
	  cerr << "Unknown attrib PAPER:'" << (*it).m_strName << "'" << endl;
      }

      // PAPERBORDERS, HEAD, FOOT
      while( parser.open( 0L, tag ) )
      {
	KOMLParser::parseTag( tag.c_str(), name, lst );

	if ( name == "PAPERBORDERS" )
	{
	  KOMLParser::parseTag( tag.c_str(), name, lst );
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for( ; it != lst.end(); it++ )
	  {
	    if ( (*it).m_strName == "left" )
	    {
	    }
	    else if ( (*it).m_strName == "top" )
	    {
	    }
	    else if ( (*it).m_strName == "right" )
	    {
	    }
	    else if ( (*it).m_strName == "bottom" )
	    {
	    }
	    else
	      cerr << "Unknown attrib 'PAPERBORDERS:" << (*it).m_strName << "'" << endl;
	  }
	}
      	else if ( name == "HEAD" )
	{
	  KOMLParser::parseTag( tag.c_str(), name, lst );
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for( ; it != lst.end(); it++ )
	  {
	    if ( (*it).m_strName == "left" )
	    {
	    }
	    else if ( (*it).m_strName == "center" )
	    {
	    }
	    else if ( (*it).m_strName == "right" )
	    {
	    }
	    else
	      cerr << "Unknown attrib 'HEAD:" << (*it).m_strName << "'" << endl;
	  }
	}
      	else if ( name == "FOOT" )
	{
	  KOMLParser::parseTag( tag.c_str(), name, lst );
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for( ; it != lst.end(); it++ )
	  {
	    if ( (*it).m_strName == "left" )
	    {
	    }
	    else if ( (*it).m_strName == "center" )
	    {
	    }
	    else if ( (*it).m_strName == "right" )
	    {
	    }
	    else
	      cerr << "Unknown attrib 'FOOT:" << (*it).m_strName << "'" << endl;
	  }
	}
	else
	  cerr << "Unknown tag '" << tag << "' in PAPER" << endl;
	
	if ( !parser.close( tag ) )
        {
	  cerr << "ERR: Closing Child" << endl;
	  m_bLoading = false;
	  return false;
	}
      }
    }
    else if ( name == "MAP" )
    {
      if ( !m_pMap->load( parser, lst ) )
      {
	m_bLoading = false;
	return false;
      }
    }
    else
      cerr << "Unknown tag '" << tag << "' in TABLE" << endl;

    if ( !parser.close( tag ) )
    {
      cerr << "ERR: Closing Child" << endl;
      m_bLoading = false;
      return false;
    }
  }

  parser.close( tag );

  cerr << "------------------------ LOADING DONE --------------------" << endl;

  return true;
}

bool KSpreadDoc::completeLoading( KOStore::Store_ptr /* _store */ )
{
  cerr << "------------------------ COMPLETING --------------------" << endl;

  m_bLoading = false;

  m_pMap->update();

  cerr << "------------------------ COMPLETION DONE --------------------" << endl;

  m_bModified = false;

  return true;
}

KSpreadTable* KSpreadDoc::createTable()
{
  char buffer[ 128 ];

  sprintf( buffer, i18n( "Table%i" ), m_iTableId++ );
  KSpreadTable *t = new KSpreadTable( this, buffer );
  t->setMap( m_pMap );
  return t;
}

void KSpreadDoc::addTable( KSpreadTable *_table )
{
  m_pMap->addTable( _table );

  m_bModified = TRUE;

  emit sig_addTable( _table );
}

void KSpreadDoc::setHeadFootLine( const char *_headl, const char *_headm, const char *_headr,
			       const char *_footl, const char *_footm, const char *_footr )
{
  m_headLeft = _headl;
  m_headRight = _headr;
  m_headMid = _headm;
  m_footLeft = _footl;
  m_footRight = _footr;
  m_footMid = _footm;

  m_bModified = TRUE;
}

void KSpreadDoc::setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder,
			      KoFormat _paper, KoOrientation _orientation )
{
  m_leftBorder = _leftBorder;
  m_rightBorder = _rightBorder;
  m_topBorder = _topBorder;
  m_bottomBorder = _bottomBorder;
  m_orientation = _orientation;
  m_paperFormat = _paper;

  calcPaperSize();

  emit sig_updateView();

  m_bModified = TRUE;
}

void KSpreadDoc::setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder,
			      const char * _paper, const char* _orientation )
{
    KoFormat f = paperFormat();
    KoOrientation o = orientation();

    if ( strcmp( "A3", _paper ) == 0L )
	f = PG_DIN_A3;
    else if ( strcmp( "A4", _paper ) == 0L )
	f = PG_DIN_A4;
    else if ( strcmp( "A5", _paper ) == 0L )
	f = PG_DIN_A5;
    else if ( strcmp( "B5", _paper ) == 0L )
	f = PG_DIN_B5;
    else if ( strcmp( "Executive", _paper ) == 0L )
	f = PG_US_EXECUTIVE;
    else if ( strcmp( "Letter", _paper ) == 0L )
	f = PG_US_LETTER;
    else if ( strcmp( "Legal", _paper ) == 0L )
	f = PG_US_LEGAL;
    else if ( strcmp( "Screen", _paper ) == 0L )
	f = PG_SCREEN;
    else if ( strcmp( "Custom", _paper ) == 0L )
    {
      m_paperWidth = 0.0;
      m_paperHeight = 0.0;
      f = PG_CUSTOM;
      QString tmp( _paper );
      m_paperWidth = atof( _paper );
      int i = tmp.find( 'x' );
      if ( i != -1 )
	m_paperHeight = atof( tmp.data() + i + 1 );
      if ( m_paperWidth < 10.0 )
	m_paperWidth = PG_A4_WIDTH;
      if ( m_paperHeight < 10.0 )
	m_paperWidth = PG_A4_HEIGHT;
    }

    if ( strcmp( "Portrait", _orientation ) == 0L )
	o = PG_PORTRAIT;
    else if ( strcmp( "Landscape", _orientation ) == 0L )
	o = PG_LANDSCAPE;

    setPaperLayout( _leftBorder, _topBorder, _rightBorder, _bottomBorder, f, o );
}

void KSpreadDoc::calcPaperSize()
{
    switch( m_paperFormat )
    {
    case PG_DIN_A5:
        m_paperWidth = PG_A5_WIDTH;
	m_paperHeight = PG_A5_HEIGHT;
	break;
    case PG_DIN_A4:
	m_paperWidth = PG_A4_WIDTH;
	m_paperHeight = PG_A4_HEIGHT;
	break;
    case PG_DIN_A3:
	m_paperWidth = PG_A3_WIDTH;
	m_paperHeight = PG_A3_HEIGHT;
	break;
    case PG_DIN_B5:
	m_paperWidth = PG_B5_WIDTH;
	m_paperHeight = PG_B5_HEIGHT;
	break;
    case PG_US_EXECUTIVE:
	m_paperWidth = PG_US_EXECUTIVE_WIDTH;
	m_paperHeight = PG_US_EXECUTIVE_HEIGHT;
	break;
    case PG_US_LETTER:
	m_paperWidth = PG_US_LETTER_WIDTH;
	m_paperHeight = PG_US_LETTER_HEIGHT;
	break;
    case PG_US_LEGAL:
	m_paperWidth = PG_US_LEGAL_WIDTH;
	m_paperHeight = PG_US_LEGAL_HEIGHT;
	break;
    case PG_SCREEN:
        m_paperWidth = PG_SCREEN_WIDTH;
        m_paperHeight = PG_SCREEN_HEIGHT;
    case PG_CUSTOM:
        return;
    }
}

QString KSpreadDoc::paperFormatString()
{
    switch( m_paperFormat )
    {
    case PG_DIN_A5:
	return QString( "A5" );
    case PG_DIN_A4:
	return QString( "A4" );
    case PG_DIN_A3:
	return QString( "A3" );
    case PG_DIN_B5:
	return QString( "B5" );
    case PG_US_EXECUTIVE:
	return QString( "Executive" );
    case PG_US_LETTER:
	return QString( "Letter" );
    case PG_US_LEGAL:
	return QString( "Legal" );
    case PG_SCREEN:
        return QString( "Screen" );
    case PG_CUSTOM:
      QString tmp;
      tmp.sprintf( "%fx%f", m_paperWidth, m_paperHeight );
      return QString( tmp );
    }

    assert( 0 );
    return QString::null;
}

const char* KSpreadDoc::orientationString()
{
    switch( m_orientation )
    {
    case QPrinter::Portrait:
	return "Portrait";
    case QPrinter::Landscape:
	return "Landscape";
    }

    assert( 0 );
    return 0;
}

QString KSpreadDoc::completeHeading( const char *_data, int _page, const char *_table )
{
    QString page;
    page.sprintf( "%i", _page );
    QString f = m_strFileURL.data();
    if ( f.isNull() )
	f = "";
    QString n = "";
    if ( f != "" )
    {
	KURL u( f.data() );
	n = u.filename();
    }
    QString t = QTime::currentTime().toString().copy();
    QString d = QDate::currentDate().toString().copy();
    QString ta = "";
    if ( _table )
	ta = _table;

    QString tmp = _data;
    int pos = 0;
    while ( ( pos = tmp.find( "<page>", pos ) ) != -1 )
	tmp.replace( pos, 6, page.data() );
    pos = 0;
    while ( ( pos = tmp.find( "<file>", pos ) ) != -1 )
	tmp.replace( pos, 6, f.data() );
    pos = 0;
    while ( ( pos = tmp.find( "<name>", pos ) ) != -1 )
	tmp.replace( pos, 6, n.data() );
    pos = 0;
    while ( ( pos = tmp.find( "<time>", pos ) ) != -1 )
	tmp.replace( pos, 6, t.data() );
    pos = 0;
    while ( ( pos = tmp.find( "<date>", pos ) ) != -1 )
	tmp.replace( pos, 6, d.data() );
    pos = 0;
    while ( ( pos = tmp.find( "<author>", pos ) ) != -1 )
	tmp.replace( pos, 8, "??" );
    pos = 0;
    while ( ( pos = tmp.find( "<email>", pos ) ) != -1 )
	tmp.replace( pos, 7, "??" );
    pos = 0;
    while ( ( pos = tmp.find( "<table>", pos ) ) != -1 )
	tmp.replace( pos, 7, ta.data() );

    return QString( tmp.data() );
}

void KSpreadDoc::resetInterpreter()
{
  destroyInterpreter();
  initInterpreter();

  // Update the cell content
  // TODO
  /* KSpreadTable *t;
  for ( t = m_pMap->firstTable(); t != 0L; t = m_pMap->nextTable() )
  t->initInterpreter(); */

  // Perhaps something changed. Lets repaint
  emit sig_updateView();
}

void KSpreadDoc::initInterpreter()
{
  m_pInterpreter = new KSpreadInterpreter( this );

  QString koffice_global_path = locate( "data", "/koffice/scripts" );
  m_pInterpreter->addSearchPath( koffice_global_path );

  QString global_path = locate( "data", "/koffice/scripts" );
  m_pInterpreter->addSearchPath( global_path );

  QString koffice_local_path = locate( "data", "/share/apps/kspread/scripts" );
  m_pInterpreter->addSearchPath( koffice_local_path );

  QString local_path = locate( "data", "/share/apps/kspread/scripts" );
  m_pInterpreter->addSearchPath( local_path );

  // Get all modules which contain kspread extensions
  m_kscriptModules += findScripts( global_path );
  m_kscriptModules += findScripts( local_path );

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

  // Load the extension scripts
  QMap<QString,QString>::Iterator mip = m.begin();
  for( ; mip != m.end(); ++mip )
  {
    KSContext context;
    if ( !m_pInterpreter->runModule( context, mip.key(), mip.data() ) )
      QMessageBox::critical( 0L, i18n("KScript error"), context.exception()->toString(), i18n("OK") );
  }
}

QStringList KSpreadDoc::findScripts( const QString& path )
{
  QStringList lst;

  DIR *dp = 0L;
  struct dirent *ep;

  dp = opendir( path );
  if ( dp == 0L )
    return lst;

  while ( ( ep = readdir( dp ) ) != 0L )
  {
    QString f = path;
    f += "/";
    f += ep->d_name;
    struct stat buff;
    if ( f != "." && f != ".." && ( stat( f, &buff ) == 0 ) && S_ISREG( buff.st_mode ) &&
	 f[ f.length() - 1 ] != '%' && f[ f.length() - 1 ] != '~' )
      lst.append( f );
  }

  closedir( dp );

  return lst;
}

void KSpreadDoc::destroyInterpreter()
{
  m_kscriptMap.clear();

  // TODO
}

KSValue* KSpreadDoc::lookupKeyword( const QString& keyword )
{
  QMap<QString,KSValue::Ptr>::Iterator it = m_kscriptMap.find( keyword );
  if ( it != m_kscriptMap.end() )
    return it.data();

  QStringList::Iterator sit = m_kscriptModules.begin();
  for( ; sit != m_kscriptModules.end(); ++sit )
  {
    KSModule::Ptr mod = m_pInterpreter->module( *sit );
    if ( mod )
    {
      KSValue* v = mod->object( keyword );
      if ( v )
      {
	v->ref();
	m_kscriptMap.insert( keyword, v );
	return v;
      }
    }
  }

  return 0;
}

KSValue* KSpreadDoc::lookupClass( const QString& name )
{
  // Is the module loaded ?
  KSModule::Ptr mod = m_pInterpreter->module( "KSpread" );
  if ( !mod )
  {
    // Try to load the module
    KSContext context;
    if ( !m_pInterpreter->runModule( context, "KSpread" ) )
      // TODO: give error
      return 0;

    context.value()->moduleValue()->ref();
    mod = context.value()->moduleValue();
  }

  // Lookup
  return mod->object( name );
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
  KSpreadView *v;
  for( v = m_lstViews.first(); v != 0L; v = m_lstViews.next() )
    v->enableUndo( _b );
}

void KSpreadDoc::enableRedo( bool _b )
{
  KSpreadView *v;
  for( v = m_lstViews.first(); v != 0L; v = m_lstViews.next() )
    v->enableRedo( _b );
}

KOffice::MainWindow_ptr KSpreadDoc::createMainWindow()
{
  KSpreadShell* shell = new KSpreadShell;
  shell->show();
  shell->setDocument( this );

  return KOffice::MainWindow::_duplicate( shell->koInterface() );
}

void KSpreadDoc::draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height,
		       CORBA::Float _scale)
{
  if ( m_pMap )
    m_pMap->draw( _dev, _width, _height, _scale );
}

void KSpreadDoc::printMap( QPainter & )
{
  // TODO
  /*
  KSpreadTable *t;
  for ( t = m_pMap->firstTable(); t != 0L; t = m_pMap->nextTable() )
  {
    t->print( _painter, false );
  } */
}

void KSpreadDoc::paperLayoutDlg()
{
  KoPageLayout pl;
  pl.format = paperFormat();
  pl.orientation = orientation();
  pl.unit = PG_MM;
  pl.width = m_paperWidth;
  pl.height = m_paperHeight;
  pl.left = leftBorder();
  pl.right = rightBorder();
  pl.top = topBorder();
  pl.bottom = bottomBorder();

  KoHeadFoot hf;
  hf.headLeft = headLeft();
  hf.headRight = headRight();
  hf.headMid = headMid();
  hf.footLeft = footLeft();
  hf.footRight = footRight();
  hf.footMid = footMid();

  if ( !KoPageLayoutDia::pageLayout( pl, hf, FORMAT_AND_BORDERS | HEADER_AND_FOOTER ) )
    return;

  if ( pl.format == PG_CUSTOM )
  {
    m_paperWidth = pl.width;
    m_paperHeight = pl.height;
  }

  setPaperLayout( pl.left, pl.top, pl.right, pl.bottom, pl.format, pl.orientation );

  setHeadFootLine( hf.headLeft, hf.headMid, hf.headRight, hf.footLeft, hf.footMid, hf.footRight );
}

KSpreadDoc::~KSpreadDoc()
{
  cerr << "KSpreadDoc::~KSpreadDoc()" << endl;

  destroyInterpreter();

  if ( m_pUndoBuffer )
    delete m_pUndoBuffer;
}

#include "kspread_doc.moc"
