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
#include "kspread_doc.h"
#include "kspread_shell.h"

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
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

int documentCount = 0;
int moduleCount = 0;

PyObject * xcl_Cell( PyObject* self, PyObject *args);
PyObject * xcl_SetCell( PyObject*, PyObject *args);
PyObject * xcl_ParseRange( PyObject*, PyObject *args);

struct PyMethodDef xcl_methods[] = {
    {"cell",	xcl_Cell, 1},
    {"setCell",	xcl_SetCell, 1},
    {"parseRange", xcl_ParseRange, 1},
    {NULL,		NULL}		/* sentinel */
};

PyObject * xcl_Cell( PyObject*, PyObject *args)
{
  int tableid;
  int row, column;
  int docid;

  if ( !PyArg_ParseTuple( args, "iiii", &docid, &tableid, &column, &row ) )
  {
    printf("ERROR: Could not parse\n");
    return NULL;
  }

  /*
  KSpreadDoc* doc = KSpreadDoc::find( docid );
  if ( !doc )
  {
    cerr << "ERROR: Unknown document " << docid << endl;
    return NULL;
  }
  */
  KSpreadTable *t = KSpreadTable::find( tableid );
  if ( t == 0L )
  {
    cerr << "ERROR: Unknown table " << tableid << endl;
    return NULL;
  }

  KSpreadCell *obj = t->cellAt( column, row );

  if ( obj->isValue() )
    return Py_BuildValue( "d", (double)obj->valueDouble() );
  else if ( obj->valueString() != NULL )
    return Py_BuildValue( "s", obj->valueString() );
  else
    return Py_BuildValue( "s", "" );
}

PyObject * xcl_SetCell( PyObject*, PyObject *args)
{
  int tableid;
  int row, column;
  const char* value;
  int docid;

  if ( PyArg_ParseTuple( args, "iiiis", &docid, &tableid, &column, &row, &value ) )
  {
    /* KSpreadDoc* doc = KSpreadDoc::find( docid );
    if ( !doc )
    {
      cerr << "ERROR: Unknown document " << docid << endl;
      return NULL;
    }
    */
    KSpreadTable *t = KSpreadTable::find( tableid );
    if ( t == 0L )
    {
      cerr << "ERROR: Unknown table " << tableid << endl;
      return NULL;
    }
    KSpreadCell *obj = t->nonDefaultCell( column, row );

    obj->setText( value );
    return Py_None;
  }

  double dvalue;
  if ( !PyArg_ParseTuple( args, "iiiid", &docid, &tableid, &column, &row, &dvalue ) )
  {
    printf("ERROR: Could not parse\n");
    return NULL;
  }

  /* KSpreadDoc* doc = KSpreadDoc::find( docid );
  if ( !doc )
  {
    cerr << "ERROR: Unknown document " << docid << endl;
    return NULL;
  } */

  KSpreadTable *t = KSpreadTable::find( tableid );
  if ( t == 0L )
  {
    cerr << "ERROR: Unknown table " << tableid << endl;
    return NULL;
  }

  KSpreadCell *obj = t->nonDefaultCell( column, row );
  obj->setValue( dvalue );
  return Py_None;
}

PyObject * xcl_ParseRange( PyObject*, PyObject *args)
{
  /* const char* range;
  int docid;
  int tabelid; */
  /*
  if ( !PyArg_ParseTuple( args, "iis", &docid, &tableid, &range ) )
  {
    printf("ERROR: Could not parse\n");
    return NULL;
  }

  KSpreadDoc* doc = KSpreadDoc::find( docid );
  if ( !doc )
  {
    cerr << "ERROR: Unknown document " << docid << endl;
    return NULL;
  }

  KSpreadTable *t = doc->map()->findTable( table );
  if ( t == 0L )
  {
    cerr << "ERROR: Unknown table " << table << endl;
    return NULL;
  }

  KSpreadCell *obj = t->cellAt( column, row );
  */

  // Returns ( table, col1, row1, col2, row2 )
  return Py_BuildValue( "(iiiii)",2,5,8,7,6 );
}

/*****************************************************************************
 *
 * KSpreadDoc
 *
 *****************************************************************************/

int KSpreadDoc::s_docId = 0L;
QIntDict<KSpreadDoc>* KSpreadDoc::s_mapDocuments;

KSpreadDoc* KSpreadDoc::find( int _docId )
{
  if ( !s_mapDocuments )
    return 0L;

  return (*s_mapDocuments)[ _docId ];
}

KSpreadDoc::KSpreadDoc()
{
  ADD_INTERFACE( "IDL:KSpread/Document:1.0" );
  ADD_INTERFACE( "IDL:KOffice/Print:1.0" );

  if ( s_mapDocuments == 0L )
    s_mapDocuments = new QIntDict<KSpreadDoc>;
  m_docId = s_docId++;
  s_mapDocuments->insert( m_docId, this );

  m_pEditor = 0L;
  m_pPython = 0L;

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

  m_iTableId = 1;

  m_defaultGridPen.setColor( lightGray );
  m_defaultGridPen.setWidth( 1 );
  m_defaultGridPen.setStyle( SolidLine );

  initPython();

  m_pMap = new KSpreadMap( this );

  m_pUndoBuffer = new KSpreadUndo( this );

  m_bModified = FALSE;

  m_lstViews.setAutoDelete( false );

  CORBA::String_var tmp = opapp_orb->object_to_string( this );
  cout << "DOC=" << tmp.in() << endl;
}

CORBA::Boolean KSpreadDoc::init()
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

  m_lstAllChildren.clear();

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

KSpreadView* KSpreadDoc::createSpreadView()
{
  KSpreadView *p = new KSpreadView( 0L, 0L, this );
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

void KSpreadDoc::makeChildListIntern( KOffice::Document_ptr _root, const char *_path )
{
  m_pMap->makeChildList( _root, _path );
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

  if ( m_pEditor && !m_editorBuffer.isNull() && m_editorBuffer.length() > 0 )
    m_pEditor->saveBuffer( m_editorBuffer );
  m_pMap->getPythonCodeFromFile();

  out << otag << "<PAPER format=\"" << paperFormatString() << "\" orientation=\"" << orientationString() << "\">" << endl;
  out << indent << "<PAPERBORDERS left=\"" << leftBorder() << "\" top=\"" << topBorder() << "\" right=\"" << rightBorder()
      << " bottom=\"" << bottomBorder() << "\"/>" << endl;
  out << indent << "<HEAD left=\"" << headLeft() << "\" center=\"" << headMid() << "\" right=\"" << headRight() << "\"/>" << endl;
  out << indent << "<FOOT left=\"" << footLeft() << "\" center=\"" << footMid() << "\" right=\"" << footRight() << "\"/>" << endl;
  out << etag << "</PAPER>" << endl;

  // TODO
  // if ( !pythonCode.isNull() && pythonCode.length() > 0 )
  // {
  // }

  m_pMap->save( out );

  out << etag << "</DOC>" << endl;

  setModified( FALSE );

  return true;
}

bool KSpreadDoc::loadXML( KOMLParser& parser, KOStore::Store_ptr _store )
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

  // TODO
  // emit signal

    /* if ( pGui )
    {
	pGui->addKSpreadTable( _table );
	pGui->setActiveKSpreadTable( _table );
    } */

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

void KSpreadDoc::reloadScripts()
{
  if ( m_pPython )
    delete m_pPython;

  initPython();

  // Update the cell content
  KSpreadTable *t;
  for ( t = m_pMap->firstTable(); t != 0L; t = m_pMap->nextTable() )
  {
    t->setLayoutDirtyFlag();
    t->setCalcDirtyFlag();
  }

  for ( t = m_pMap->firstTable(); t != 0L; t = m_pMap->nextTable() )
    t->recalc();

  emit sig_updateView();
}

void KSpreadDoc::initPython()
{
    QString t2;
    t2.sprintf( "xclModule%i", ++moduleCount );
    m_pPython = new KSpreadPythonModule( t2, docId() );
    m_pPython->registerMethods( xcl_methods );
    m_pPython->setContext( this );

    DIR *dp = 0L;
    struct dirent *ep;

    string path = kapp->kde_datadir().data();
    path += "/kspread/scripts";

    dp = opendir( path.c_str() );
    if ( dp == 0L )
      return;

    while ( ( ep = readdir( dp ) ) != 0L )
    {
      string f = path;
      f += "/";
      f += ep->d_name;
      struct stat buff;
      if ( f != "." && f != ".." && ( stat( f.c_str(), &buff ) == 0 ) && S_ISREG( buff.st_mode ) &&
	   f[ f.size() - 1 ] != '%' && f[ f.size() - 1 ] != '~' )
      {
	cerr << "Executing " << f << endl;
        //if (strcmp(f.c_str(),"/opt/kde/share/apps/kspread/scripts/xcllib.py") != 0)
	//{
          int res = m_pPython->runFile( f.c_str() );
          cerr << "Done result=" << res << endl;
        //}
 	//else cerr << "DISABLED !!" << endl;
      }
    }

    closedir( dp );

    /* QString d = kapp->kde_datadir().copy();
    d += "/kspread/scripts/xcllib.py";
    assert( access( d, R_OK ) >= 0 );
    m_pPython->runFile( d );

    d = kapp->kde_datadir().copy();
    d += "/kspread/scripts/classes.py";
    assert( access( d, R_OK ) >= 0 );
    m_pPython->runFile( d ); */

    path = kapp->localkdedir().data();
    path += "/share/apps/kspread/scripts";

    dp = opendir( path.c_str() );
    if ( dp == 0L )
      return;

    while ( ( ep = readdir( dp ) ) != 0L )
    {
      string f = path;
      f += "/";
      f += ep->d_name;
      struct stat buff;
      if ( f != "." && f != ".." && ( stat( f.c_str(), &buff ) == 0 ) && S_ISREG( buff.st_mode ) &&
	   f[ f.size() - 1 ] != '%' && f[ f.size() - 1 ] != '~' )
      {
	cerr << "Executing " << f << endl;
	int res = m_pPython->runFile( f.c_str() );
	cerr << "Done result=" << res << endl;
      }
    }

    closedir( dp );
}

void KSpreadDoc::runPythonCode()
{
  // here we could at least pop up a lineedit dialog asking for the function
  // to execute, or show the list of functions. For now run the whole script.
  char * code = (char *) m_pMap->getPythonCode();
  cerr << "runPythonCode : code=" << code << endl;
  if (code) {
    int res = m_pPython->runCodeStr( KPythonModule::PY_STATEMENT, code );
    cerr << "runPythonCode : result=" << res << endl;
  }
}

bool KSpreadDoc::editPythonCode()
{
  if ( m_pEditor == 0L )
    m_pEditor = createEditor();
  if ( !m_pEditor->isOk() )
    return FALSE;

  debug("KSpreadDoc::editPythonCode()");
  m_pMap->movePythonCodeToFile();

  m_editorBuffer = m_pEditor->openFile( m_pMap->getPythonCodeFile() ).copy();
  debug(m_editorBuffer);
  m_pEditor->show();

  return TRUE;
}

void KSpreadDoc::endEditPythonCode()
{
  if ( m_pEditor == 0L )
    return;
  if ( !m_pEditor->isOk() )
    return;
  if ( m_editorBuffer.isNull() )
    return;

  debug("KSpreadDoc::endEditPythonCode()");
  m_pEditor->saveBuffer( m_editorBuffer );
  m_pEditor->killBuffer( m_editorBuffer );
  m_pEditor->hide();

  m_pMap->getPythonCodeFromFile();
  m_bModified = TRUE;

  m_editorBuffer = QString::null;
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

void KSpreadDoc::printMap( QPainter &_painter )
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

  s_mapDocuments->remove( m_docId );

  if ( m_pPython )
    delete m_pPython;

  endEditPythonCode();

  if ( m_pUndoBuffer )
    delete m_pUndoBuffer;
}

#include "kspread_doc.moc"
