#include "kchart_view.h"
#include "kchart_doc.h"
#include "kchart_main.h"

#include <op_app.h>
#include <utils.h>
#include <part_frame_impl.h>

#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>
#include <qdialog.h>

#include "sheetdlg.h"

#include "KChartData.h"

/**********************************************************
 *
 * KChartView
 *
 **********************************************************/

KChartView::KChartView( QWidget *_parent = 0L, const char *_name = 0L ) :
  QWidget( _parent, _name ), View_impl(), KCharter::View_skel()
{
  setWidget( this );
  
  setBackgroundColor( white );
  
  Control_impl::setFocusPolicy( OPControls::Control::ClickFocus );
 
  m_pDoc = 0L;

  setGeometry( 5000, 5000, 100, 100 );
}

KChartView::~KChartView()
{
  cleanUp();
}

void KChartView::cleanUp()
{
  if ( m_bIsClean )
    return;
  
  m_pDoc->removeView( this );
  
  m_rMenuBar = 0L;
  m_vMenuBarFactory = 0L;

  m_rToolBarLayout = 0L;
  m_vToolBarFactory = 0L;

  View_impl::cleanUp();
}
  
void KChartView::setDocument( KChartDocument *_doc )
{
  if ( m_pDoc )
    m_pDoc->removeView( this );

  View_impl::setDocument( _doc );
  
  m_pDoc = _doc;

  m_pDoc->addView( this );

  QObject::connect( m_pDoc, SIGNAL( sig_modified() ), this, SLOT( slotChartModified() ) );
}

void KChartView::paintEvent( QPaintEvent *_ev )
{
  m_pDoc->chart().repaintChart( this );
}

void KChartView::resizeEvent( QResizeEvent *_ev )
{
  m_pDoc->chart().repaintChart( this );
}

void KChartView::slotChartModified()
{
  update();
}

void KChartView::createGUI()
{ 
  m_vMenuBarFactory = m_vPartShell->menuBarFactory();
  if ( !CORBA::is_nil( m_vMenuBarFactory ) )
  {
    // Menubar
    m_rMenuBar = m_vMenuBarFactory->createMenuBar( this );

    // View
    m_idMenuView = m_rMenuBar->insertMenu( i18n( "&View" ) );
    
    m_idMenuView_NewView = m_rMenuBar->insertItem( i18n( "&New View" ), m_idMenuView,
						   this, "newView" );

    // Edit
    m_idMenuEdit = m_rMenuBar->insertMenu( i18n( "&Edit" ) );
    
    m_idMenuEdit_EditData = m_rMenuBar->insertItem( i18n( "&Edit data..." ), m_idMenuEdit,
						    this, "editData" );
  }

  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if ( !CORBA::is_nil( m_vToolBarFactory ) )
  {
    m_rToolBarLayout = m_vToolBarFactory->createToolBar( this, i18n( "Layout" ) );

    QString tmp = kapp->kde_datadir().copy();
    tmp += "/kchart/pics/bars.xpm";
    QString pix = loadPixmap( tmp );
    m_idButtonLayout_Bars = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), i18n( "Bars" ), this, "setTypeBars" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kchart/pics/area.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Area = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), i18n("Area"), this, "setTypeArea" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kchart/pics/lines.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Lines = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), i18n("Lines"), this, "setTypeLines" );
  }
}

void KChartView::newView()
{
  assert( (m_pDoc != 0L) );

  KChartShell* shell = new KChartShell;
  shell->enableMenuBar();
  shell->PartShell_impl::enableStatusBar();
  shell->enableToolBars();
  shell->show();
  shell->setDocument( m_pDoc );
  
  CORBA::release( shell );
}

void KChartView::setTypeBars()
{
  m_pDoc->chart().setChartType( Bars );
  m_pDoc->emitModified();
}

void KChartView::setTypeLines()
{
  m_pDoc->chart().setChartType( Lines );
  m_pDoc->emitModified();
}

void KChartView::setTypeArea()
{
  m_pDoc->chart().setChartType( Area );
  m_pDoc->emitModified();
}

void KChartView::editData()
{
  // create dialog
  QDialog *_dlg = new QDialog(0,"SheetDlg",true);
  SheetDlg *_widget = new SheetDlg(_dlg,"SheetWidget");
  _widget->setGeometry(0,0,520,400);
  _widget->show();
  _dlg->resize(520,400);
  _dlg->setMaximumSize(_dlg->size());
  _dlg->setMinimumSize(_dlg->size());

  // fill cells
  int col,row;
  double maxY = 0;
  for (row = 0;row < m_pDoc->chartData()->numDatasets();row++)
    {
      for (col = 0;col < _widget->TABLE_SIZE;col++)
	{
	  if (m_pDoc->chartData()->hasYValue(row,col))
	    _widget->fillCell(row,col,m_pDoc->chartData()->yValue(row,col));
	}
    }

  for (col = 0;(unsigned int)col <= m_pDoc->chartData()->maxPos();col++)
    _widget->fillX(col,m_pDoc->chartData()->xValue(col));

  // OK pressed
  if (_dlg->exec() == QDialog::Accepted)
    {
      KChartData *m_pData = new KChartData(_widget->rows());

      for (col = 0;col < _widget->cols();col++)
	m_pData->setXValue( col, (const char*)_widget->getX(col));

      for (row = 0;row < _widget->rows();row++)
	{
	  for (col = 0;col < _widget->cols();col++)
	    {
	      m_pData->setYValue( row, col, _widget->getCell(row,col) );
	      maxY = _widget->getCell(row,col) > maxY ? _widget->getCell(row,col) : maxY;
	    }
	}

      maxY++;
       
      m_pDoc->setChartData(m_pData);
      m_pDoc->chart().setYMaxValue(maxY);
      m_pDoc->chart().setYTicksNum(maxY);
      m_pDoc->chart().repaintChart( this );
    } 
  
  // delete dialog
  delete _widget; _widget = 0;
  delete _dlg; _dlg = 0;
}

#include "kchart_view.moc"






