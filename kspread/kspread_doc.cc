#include "kspread_doc.h"

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

int documentCount = 0;
int moduleCount = 0;

PyObject * xcl_Cell( PyObject* self, PyObject *args);

struct PyMethodDef xcl_methods[] = {
    {"Cell",	xcl_Cell, 1},
    {NULL,		NULL}		/* sentinel */
};

PyObject * xcl_Cell( PyObject*, PyObject *args)
{
    KSpreadTable *table;
    int row, column;
    if ( !PyArg_ParseTuple( args, "lii", &table, &column, &row ) )
    {
	printf("ERROR: Could not parse\n");
	return NULL;
    }

    KSpreadCell *obj = table->cellAt( column, row );

    if ( obj->isValue() )
	return Py_BuildValue( "d", (double)obj->valueDouble() );
    else if ( obj->valueString() != NULL )
	return Py_BuildValue( "s", obj->valueString() );
    else
	return Py_BuildValue( "s", "" ); 
}

/*****************************************************************************
 *
 * KSpreadDoc
 *
 *****************************************************************************/

KSpreadDoc::KSpreadDoc()
{
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
    
    m_iTableId = 1;

    initPython();
    
    m_pMap = new KSpreadMap( this );
    
    m_pUndoBuffer = new KSpreadUndo( this );
    
    m_bModified = FALSE;

    m_lstViews.setAutoDelete( false );
}

CORBA::Boolean KSpreadDoc::init()
{
  KSpreadTable *t = createTable();
  m_pMap->addTable( t );

  return true;
}

void KSpreadDoc::cleanUp()
{
  if ( m_bIsClean )
    return;

  assert( m_lstViews.count() == 0 );
  
  m_lstAllChildren.clear();
  // m_lstChildren.clear();

  Document_impl::cleanUp();
}

void KSpreadDoc::removeView( KSpreadView* _view )
{
  m_lstViews.removeRef( _view );
}

OPParts::View_ptr KSpreadDoc::createView()
{
  KSpreadView *p = new KSpreadView( 0L, 0L, this );
  p->setGeometry( 5000, 5000, 100, 100 );
  p->QWidget::show();
  m_lstViews.append( p );
  
  return OPParts::View::_duplicate( p );
}

void KSpreadDoc::viewList( OPParts::Document::ViewList*& _list )
{
  (*_list).length( m_lstViews.count() );

  int i = 0;
  QListIterator<KSpreadView> it( m_lstViews );
  for( ; it.current(); ++it )
  {
    (*_list)[i++] = OPParts::View::_duplicate( it.current() );
  }
}

void KSpreadDoc::makeChildListIntern( OPParts::Document_ptr _root, const char *_path )
{
  m_pMap->makeChildList( _root, _path );
}

bool KSpreadDoc::hasToWriteMultipart()
{
  return m_pMap->hasToWriteMultipart();
}

bool KSpreadDoc::save( ostream& out )
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

bool KSpreadDoc::load( KOMLParser& parser )
{
  string tag;
  vector<KOMLAttrib> lst;
  string name;
 
  // DOC
  if ( !parser.open( "DOC", tag ) )
  {
    cerr << "Missing DOC" << endl;
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
	  return false;
	}
      }
    }
    else if ( name == "MAP" )
    {
      if ( !m_pMap->load( parser, lst ) )
	return false;
    }
    else
      cerr << "Unknown tag '" << tag << "' in TABLE" << endl;    

    if ( !parser.close( tag ) )
    {
      cerr << "ERR: Closing Child" << endl;
      return false;
    }
  }

  parser.close( tag ); 

  return true;
}


KSpreadTable* KSpreadDoc::createTable()
{
  char buffer[ 128 ];
  
  sprintf( buffer, i18n( "Table %i" ), m_iTableId++ );
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

  // TODO
  // emit signal
  /* if ( pGui )
	pGui->canvasWidget()->repaint(); */
}

void KSpreadDoc::initPython()
{
    QString d = kapp->kde_datadir().copy();
    d += "/kspread/scripts/xcllib.py";

    // if ( access( d, R_OK ) < 0 )
    //warning( i18n( "Could not read '%s'\n" ), d.data() );
    assert( access( d, R_OK ) >= 0 );
    
    QString t2;
    t2.sprintf( "xclModule%i", ++moduleCount );
    m_pPython = new KSpreadPythonModule( t2 );
    m_pPython->registerMethods( xcl_methods );
    m_pPython->runFile( d );
}

bool KSpreadDoc::editPythonCode()
{
  if ( m_pEditor == 0L )
    m_pEditor = createEditor();
  if ( !m_pEditor->isOk() )
    return FALSE;
  
  m_pMap->movePythonCodeToFile();
  
  m_editorBuffer = m_pEditor->openFile( m_pMap->getPythonCodeFile() ).copy();
  m_pEditor->show();
  
  return TRUE;
}

void KSpreadDoc::endEditPythonCode()
{
  if ( m_pEditor == 0L )
    return;
  if ( !m_pEditor->isOk() )
    return;
  if ( !m_editorBuffer.isNull() && m_editorBuffer.length() > 0 )
    return;
  
  m_pEditor->saveBuffer( m_editorBuffer );
  m_pEditor->killBuffer( m_editorBuffer );
  m_pEditor->hide();
  
  m_pMap->getPythonCodeFromFile();
  
  m_editorBuffer = 0L;
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
  if ( m_pPython )
    delete m_pPython;
    
  endEditPythonCode();

  if ( m_pUndoBuffer )
    delete m_pUndoBuffer;
}

#include "kspread_doc.moc"
