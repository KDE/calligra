#include "kspread_doc.h"

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
 * KSpread
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
    m_paperFormat = A4;
    calcPaperSize();
    m_orientation = QPrinter::Portrait;
    m_pMap = 0L;
    
    m_iTableId = 1;

    initPython();
    
    m_pMap = new KSpreadMap( this );
    
    KSpreadTable *t = createTable();
    m_pMap->addTable( t );

    m_pUndoBuffer = new KSpreadUndo( this );
    
    m_bModified = FALSE;

    m_lstViews.setAutoDelete( false );
}

void KSpreadDoc::cleanUp()
{
  if ( m_bIsClean )
    return;

  assert( m_lstViews.count() == 0 );
  
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

CORBA::Boolean KSpreadDoc::open( const char *_filename )
{
  return false;
}

CORBA::Boolean KSpreadDoc::saveAs( const char *_filename, const char *_format )
{
  return false;
}

/* void KSpreadDoc::activate( bool _status )
{
    if ( _status )
    {
	QString tmp;
	if ( fileURL.isEmpty() )
	{
	    tmp.sprintf( "Kxcl: New Document %i", ++documentCount );
	    shell()->setCaption( this, tmp.data() );
	}
	else
	{
	    tmp.sprintf( "Kxcl: %s", fileURL.data() );
	    shell()->setCaption( this, tmp.data() );
	}

	if ( pGui )
	    pGui->getPaperWidget()->setFocus();
    }
    KPart::activate( _status );
} */

/* void KSpreadDoc::showGUI()
{
    KPart::showGUI();

    if ( pGui )
	pGui->showGUI( TRUE );
} */

/* void KSpreadDoc::hideGUI()
{
    KPart::hideGUI();

    if ( pGui )
	pGui->showGUI( FALSE );
} */

/*
bool KSpreadDoc::save( const char *_url )
{
  KURL u( _url );
    if ( u.isMalformed() )
	return FALSE;
    
    KorbSession* korb = new KorbSession( u.path(), IO_WriteOnly );
    korb->setAuthor( "(c) Torben Weis, weis@kde.org" );

    OBJECT o_map = save( korb );

    if ( o_map == 0 )
    {
	korb->release();
	delete korb;
	return FALSE;
    }
    
    korb->setRootObject( o_map );
    korb->release();
    delete korb;

    fileURL = _url;
    
    return TRUE;
}
*/
/*
OBJECT KSpreadDoc::save( KorbSession* _korb )
{
    // For use as values in the ObjectType property
    TYPE t_map   =  _korb->registerType( "KDE:kxcl:Map" );

    if ( editor && !editorBuffer.isNull() && editorBuffer.length() > 0 )
	editor->saveBuffer( editorBuffer );
    pMap->getPythonCodeFromFile();
    
    OBJECT o_map = KPart::save( _korb, t_map );
    if ( o_map )
	return pMap->save( _korb, o_map );

    return o_map;
}
*/
/*
bool KSpreadDoc::load( const char *_url )
{
    KURL u( _url );
    if ( u.isMalformed() )
	return FALSE;

    KorbSession* korb = new KorbSession( u.path(), IO_ReadOnly );    

    printf("Searching author .....\n");
    QString author = korb->getAuthor();
    author.detach();

    printf("The Author is: %s\n",author.data());

    OBJECT o_root = korb->getRootObject();
    if ( o_root == 0 )
    {
	printf("ERROR: No root object\n");
	return FALSE;
    }
    
    bool ret = load( korb, o_root );
    
    if ( !ret )
    {
	korb->release();
	delete korb;
	return FALSE;
    }

    korb->release();
    delete korb;

    fileURL = _url;
    
    return TRUE;
}
*/
/*
bool KSpreadDoc::load( KorbSession *_korb, OBJECT _map )
{
    KSpreadMap *map2 = new XclMap( this );

    printf("Loading map ....\n");

    // For use as values in the ObjectType property
    TYPE t_map = _korb->findType( "KDE:kxcl:Map" );
    if ( !t_map || !KPart::load( _korb, _map, t_map ) )
	return FALSE;
    
    bool ret = map2->load( _korb, _map );

    if ( ret )
    {
	tableId = 1;
	
	printf("Adding to gui\n");

	if ( pGui )
	    pGui->removeAllKSpreadTables();
	delete pMap;

	pMap = map2;

	if ( pGui )
	{
	    KSpreadTable *t;
	    for ( t = pMap->firstKSpreadTable(); t != 0L; t = pMap->nextTable() )
		pGui->addKSpreadTable( t );
	}
	
	pMap->initAfterLoading();
	tableId = pMap->count() + 1;
	
	if ( pGui )
	    pGui->setActiveKSpreadTable( pMap->firstTable() );
    }
    else
	delete map2;

    printf("... Done map\n");

    return TRUE;
}
*/

KSpreadTable* KSpreadDoc::createTable()
{
  char buffer[ 128 ];
  
  sprintf( buffer, i18n( "KSpreadTable%i" ), m_iTableId++ );
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
			      PaperFormat _paper, QPrinter::Orientation _orientation )
{
  m_leftBorder = _leftBorder;
  m_rightBorder = _rightBorder;
  m_topBorder = _topBorder;
  m_bottomBorder = _bottomBorder;
  m_orientation = _orientation;
  m_paperFormat = _paper;
  
  calcPaperSize();
    
  // TODO
  // emit signal
  /* if ( pGui )
	pGui->canvasWidget()->repaint(); */

  m_bModified = TRUE;
}

void KSpreadDoc::setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder,
			      const char * _paper, const char* _orientation )
{
    PaperFormat f = A4;
    QPrinter::Orientation o = QPrinter::Portrait;
    
    if ( strcmp( "A3", _paper ) == 0L )
	f = A3;
    else if ( strcmp( "A4", _paper ) == 0L )
	f = A4;
    else if ( strcmp( "A5", _paper ) == 0L )
	f = A5;
    else if ( strcmp( "Letter", _paper ) == 0L )
	f = LETTER;
    else if ( strcmp( "Executive", _paper ) == 0L )
	f = EXECUTIVE;
    
    if ( strcmp( "Portrait", _orientation ) == 0L )
	o = QPrinter::Portrait;
    else if ( strcmp( "Landscape", _orientation ) == 0L )
	o = QPrinter::Landscape;
    
    setPaperLayout( _leftBorder, _topBorder, _rightBorder, _bottomBorder, f, o );
}

void KSpreadDoc::calcPaperSize()
{
    switch( m_paperFormat )
    {
    case A5:
	m_paperWidth = 150.0;
	m_paperHeight = 210.0;
	break;
    case A4:
	m_paperWidth = 210.0;
	m_paperHeight = 297.0;
	break;
    case A3:
	m_paperWidth = 297.0;
	m_paperHeight = 420.0;
	break;
    case LETTER:
	m_paperWidth = 216.0;
	m_paperHeight = 355.0;
	break;
    case EXECUTIVE:
	m_paperWidth = 184.0;
	m_paperHeight = 266.0;
	break;
    }
}

const char* KSpreadDoc::paperFormatString()
{
    switch( m_paperFormat )
    {
    case A5:
	return "A5";
    case A4:
	return "A4";
    case A3:
	return "A3";
    case LETTER:
	return "Letter";
    case EXECUTIVE:
	return "Executive";
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

KSpreadDoc::~KSpreadDoc()
{
  if ( m_pPython )
    delete m_pPython;
    
  endEditPythonCode();

  if ( m_pUndoBuffer )
    delete m_pUndoBuffer;
}

#include "kspread_doc.moc"
