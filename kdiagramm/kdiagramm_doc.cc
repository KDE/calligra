#include <qprinter.h>
#include "kdiagramm_doc.h"

#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>
#include <komlMime.h>

#include <fstream>
#include <string>

#include <unistd.h>
#include <k2url.h>
#include <kapp.h>

#include "sheetdlg.h"

/*****************************************************************************
 *
 * KDiagrammDoc
 *
 *****************************************************************************/

KDiagrammDoc::KDiagrammDoc()
{
  ADD_INTERFACE( "IDL:Chart/SimpleChart:1.0" );

  m_bEmpty = true;
  m_bModified = FALSE;

  m_lstViews.setAutoDelete( false );
}

CORBA::Boolean KDiagrammDoc::init()
{
  // Demo values
  m_table.xDesc.append( "Torben" );
  m_table.xDesc.append( "Claudia" );
  m_table.xDesc.append( "Lars" );
  m_table.xDesc.append( "Matthias" );
  
  m_table.yDesc.append( "Januar" );
  m_table.yDesc.append( "Februar" );
  
  line_t l;
  l.push_back( 6.0 );
  l.push_back( 7.0 );
  l.push_back( 3.0 );
  l.push_back( 1.5 );
  m_table.data.push_back( l );

  l.clear();
  l.push_back( 5.0 );
  l.push_back( 3.0 );
  l.push_back( 2.0 );
  l.push_back( 2.5 );
  m_table.data.push_back( l );
  
  return true;
}

void KDiagrammDoc::cleanUp()
{
  cerr << "CLeanUp KDiagrammDoc" << endl;
  
  if ( m_bIsClean )
    return;

  assert( m_lstViews.count() == 0 );

  m_lstAllChildren.clear();

  KoDocument::cleanUp();
}

void KDiagrammDoc::removeView( KDiagrammView* _view )
{
  m_lstViews.removeRef( _view );
}

KDiagrammView* KDiagrammDoc::createDiagrammView()
{
  KDiagrammView *p = new KDiagrammView( 0L, 0L, this );
  p->QWidget::show();
  m_lstViews.append( p );
  
  return p;
}

OpenParts::View_ptr KDiagrammDoc::createView()
{
  return OpenParts::View::_duplicate( createDiagrammView() );
}

void KDiagrammDoc::viewList( OpenParts::Document::ViewList*& _list )
{
  (*_list).length( m_lstViews.count() );

  int i = 0;
  QListIterator<KDiagrammView> it( m_lstViews );
  for( ; it.current(); ++it )
  {
    (*_list)[i++] = OpenParts::View::_duplicate( it.current() );
  }
}

int KDiagrammDoc::viewCount()
{
  return m_lstViews.count();
}

bool KDiagrammDoc::save( ostream& out, const char* /* format */ )
{
  out << "<?xml version=\"1.0\"?>" << endl;
  out << otag << "<DOC author=\"" << "Torben Weis" << "\" email=\"" << "weis@kde.org" << "\" editor=\"" << "kdiagramm"
      << "\" mime=\"" << "application/x-kdiagramm" << "\" >" << endl;
    
  out << etag << "</DOC>" << endl;
    
  setModified( FALSE );
    
  return true;
}

bool KDiagrammDoc::loadXML( KOMLParser& parser, KOStore::Store_ptr _store )
{
  cerr << "------------------------ LOADING --------------------" << endl;
  
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
      if ( (*it).m_strValue != "application/x-kdiagramm" )
      {
	cerr << "Unknown mime type " << (*it).m_strValue << endl;
	return false;
      }
    }
  }
    
  parser.close( tag ); 

  cerr << "------------------------ LOADING DONE --------------------" << endl;
  
  return true;
}

bool KDiagrammDoc::completeLoading( KOStore::Store_ptr /* _store */ )
{
  cerr << "------------------------ COMPLETION DONE --------------------" << endl;

  m_bModified = false;

  return true;
}

void KDiagrammDoc::showWizard()
{
  // TODO
}

void KDiagrammDoc::fill( const Chart::Range& range, const Chart::Matrix& matrix )
{
  cout << "Got w=" << matrix.columns << " h=" << matrix.rows << endl;
  
  cerr << "1" << endl;

  m_table.yDesc.clear();
  int l = matrix.rowDescription.length();
  for ( int i = 0; i < l; i++ )
  {
    m_table.yDesc.append( matrix.rowDescription[i].in() );
  }

  m_table.xDesc.clear();
  l = matrix.columnDescription.length();
  for ( int i = 0; i < l; i++ )
  {
    m_table.xDesc.append( matrix.columnDescription[i].in() );
  }

  cerr << "2" << endl;

  m_table.data.clear();
  for ( int y = 0; y < matrix.rows; y++ )
  {
    line_t l;    
    for ( int x = 0; x < matrix.columns; x++ )
      l.push_back( matrix.matrix[ y * matrix.columns + x ] );
    m_table.data.push_back( l );
  }

  cerr << "4" << endl;

  m_range = range;

  cerr << "6" << endl;

  emit sig_updateView();
}

void KDiagrammDoc::editData()
{
  // create dialog
  QDialog *dlg = new QDialog(0,"SheetDlg",true);
  SheetDlg *widget = new SheetDlg( dlg, "SheetWidget" );
  widget->setGeometry(0,0,520,400);
  widget->show();
  dlg->resize(520,400);
  dlg->setMaximumSize( dlg->size() );
  dlg->setMinimumSize( dlg->size() );

  // fill cells
  int col = 0,row = 0;
  for( data_t::iterator it1 = m_table.data.begin(); it1 != m_table.data.end(); ++it1 )
  {
    col = 0;
    for( line_t::iterator it2 = it1->begin(); it2 != it1->end(); ++it2 )
    {
      widget->fillCell( row, col++, *it2 );
    }
    row++;
  }

  const char *s;
  col = 0;
  for( s = m_table.xDesc.first(); s != 0L; s = m_table.xDesc.next() )
    widget->fillX( col++, s );

  row = 0;
  for( s = m_table.yDesc.first(); s != 0L; s = m_table.yDesc.next() )
    widget->fillY( row++, s );

  // OK pressed
  if ( dlg->exec() == QDialog::Accepted )
  {
    m_table.xDesc.clear();
    for ( col = 0; col < widget->cols(); col++ )
      m_table.xDesc.append( widget->getX( col ) );

    m_table.yDesc.clear();
    for ( row = 0; row < widget->rows(); row++ )
      m_table.yDesc.append( widget->getY( row ) );
   
    m_table.data.clear();
    for ( row = 0; row < widget->rows(); row++ )
    {
      line_t line;
      for ( col = 0; col < widget->cols(); col++ )
	line.push_back( widget->getCell( row, col ) );
      m_table.data.push_back( line );
    }

    emit sig_updateView();
  }

  // delete dialog
  delete widget; widget = 0;
  delete dlg; dlg = 0;
}

KDiagrammDoc::~KDiagrammDoc()
{
}

#include "kdiagramm_doc.moc"
