#include "kchart_doc.h"

#include <komlMime.h>
#include <koStream.h>

#include <kurl.h>
#include <qmsgbox.h>

#include <strstream>
#include <fstream>
#include <unistd.h>

#include "KChartData.h"

#include <component_impl.h>

KChartDocument::KChartDocument()
{
  ADD_INTERFACE( "IDL:Chart/SimpleChart:1.0" );

  m_pWizard = 0L;
  
  // Use CORBA mechanism for deleting views
  m_lstViews.setAutoDelete( false );

  m_bModified = false;

  // HACK ...

  // setup some chart data
  m_pData = new KChartData(1);
  m_pData->setXValue( 0, "1st" );
  m_pData->setXValue( 1, "2nd" );
  m_pData->setXValue( 2, "3rd" );
  m_pData->setXValue( 3, "4th" );
  m_pData->setXValue( 4, "5th" );
  m_pData->setXValue( 5, "6th" );
  m_pData->setXValue( 6, "7th" );
  m_pData->setXValue( 7, "8th" );
  m_pData->setXValue( 8, "9th" );
  m_pData->setYValue( 0, 0, 1.0 );
  m_pData->setYValue( 0, 1, 2.0 );
  m_pData->setYValue( 0, 2, 5.0 );
  m_pData->setYValue( 0, 3, 6.0 );
  m_pData->setYValue( 0, 4, 3.0 );
  m_pData->setYValue( 0, 5, 1.5 );
  m_pData->setYValue( 0, 6, 1.0 );
  m_pData->setYValue( 0, 7, 3.0 );
  m_pData->setYValue( 0, 8, 4.0 );

  m_chart.setChartData( m_pData );

  // some more settings
  m_chart.setTitle( i18n("A Simple Bar Chart") );
  m_chart.setXLabel( i18n("X Label") );
  m_chart.setYLabel( i18n("Y Label") );
  m_chart.setYMaxValue( 8 );
  m_chart.setYTicksNum( 8 );
  m_chart.setYLabelSkip( 2 );
}

CORBA::Boolean KChartDocument::init()
{
  return true;
}

KChartDocument::~KChartDocument()
{
  cleanUp();
}

void KChartDocument::cleanUp()
{
  if ( m_bIsClean )
    return;

  assert( m_lstViews.count() == 0 );
  
  Document_impl::cleanUp();
}

bool KChartDocument::load( KOMLParser& parser )
{
  // HACK
  return false;
}

bool KChartDocument::save( ostream &out )
{
  out << "<?xml version=\"1.0\"?>" << endl;
  out << otag << "<DOC author=\"" << "Kalle Dallheimer & Torben Weis" << "\" email=\"" << "kalle@kde.org, weis@kde.org"
      << "\" editor=\"" << "KChart" << "\" mime=\"" << MIME_TYPE << "\" >" << endl;
  out << etag << "</DOC>" << endl;
  
  return true;
}

void KChartDocument::viewList( OPParts::Document::ViewList*& _list )
{
  (*_list).length( m_lstViews.count() );

  int i = 0;
  QListIterator<KChartView> it( m_lstViews );
  for( ; it.current(); ++it )
  {
    (*_list)[i++] = OPParts::View::_duplicate( it.current() );
  }
}

void KChartDocument::addView( KChartView *_view )
{
  m_lstViews.append( _view );
}

void KChartDocument::removeView( KChartView *_view )
{
  m_lstViews.setAutoDelete( false );
  m_lstViews.removeRef( _view );
  m_lstViews.setAutoDelete( true );
}

OPParts::View_ptr KChartDocument::createView()
{
  KChartView *p = new KChartView( 0L );
  p->setDocument( this );
  // p->QWidget::show();
  
  return OPParts::View::_duplicate( p );
}

const char *KChartDocument::createColumnLabel( int column )
{
  if ( column <= 26 )
    sprintf( m_arrColumnLabel, "%c", 'A' + column - 1 );
  else if ( column <= 26 * 26 )
    sprintf( m_arrColumnLabel, "%c%c",'A'+((column-1)/26)-1,'A'+((column-1)%26));
  else
    strcpy( m_arrColumnLabel,"@@@");

  return m_arrColumnLabel;
}                           

string KChartDocument::createDataArea( const QRect& _rect )
{
  string l1 = createColumnLabel( _rect.left() );
  string l2 = createColumnLabel( _rect.right() );
  
  char buffer[ 1024 ];
  sprintf( buffer, "%s%i:%s%i", l1.c_str(), _rect.top(), l2.c_str(), _rect.bottom() );
  return string( buffer );
}

void KChartDocument::slotWizardOk()
{
  emit sig_modified();
  
  // HACK , KWizard deletion does not work
  m_pWizard->hide();
  // delete m_pWizard;
  // m_pWizard = 0L;
}

void KChartDocument::showWizard()
{
  if ( m_pWizard != 0L )
  {    
    m_pWizard->show();
    return;
  }
  
  m_pWizard = new KChartWizard( &m_chart, 0, "kchartwizard", false );
  
  string area = createDataArea( m_rctDataArea );
  m_pWizard->setDataArea( area.c_str() );

  QObject::connect( m_pWizard, SIGNAL( okclicked() ), this, SLOT( slotWizardOk() ) );

  m_pWizard->adjustSize();
  m_pWizard->show();
}

void KChartDocument::setChartData(KChartData *newData)
{
  if ( m_pData )
    delete m_pData;

  m_pData = newData;

  m_chart.setChartData( m_pData );

  emit sig_modified();
}

void KChartDocument::fill( const Chart::Range& range, const Chart::Matrix& matrix )
{
  cout << "Got w=" << matrix.columns << " h=" << matrix.rows << endl;
  
  KChartData* data = new KChartData( matrix.columns );

  cerr << "1" << endl;
  
  int l = matrix.rowDescription.length();
  int i;
  for ( i = 0; i < l; i++ )
  {
    if ( static_cast<const char*>( matrix.rowDescription[ i ] ) == 0 )
    {
      cerr << "Unexpected 0L in row " << i << endl;
      delete data;
      return;
    }
    cerr << "Desc at " << i << " is '" << static_cast<const char*>( matrix.rowDescription[ i ] ) << "'" << endl;
    data->setXValue( i, matrix.rowDescription[ i ] );
  }
  
  cerr << "2" << endl;

  for ( int x = 0; x < matrix.columns; x++ )
    for ( int y = 0; y < matrix.rows; y++ )
      data->setYValue( x, y, matrix.matrix[ y * matrix.columns + x ] );
  
  cerr << "3" << endl;

  cerr << "4" << endl;

  m_rctDataArea.setCoords( range.left, range.top, range.right, range.bottom );

  if ( m_pData )
    delete m_pData;
  
  m_pData = data;
  m_chart.setChartData( data );

  cerr << "5" << endl;  
  emit sig_modified();

  cerr << "6" << endl;
}

void KChartDocument::emitModified()
{
  emit sig_modified();
}

#include "kchart_doc.moc"
