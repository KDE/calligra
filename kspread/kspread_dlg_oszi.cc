#include <kbuttonbox.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "kspread_dlg_oszi.h"
#include "kspread_view.h"
#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_util.h"
#include "qtai_plot.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qpushbutton.h>
#include <qslider.h>

#include <math.h>

#define NPOINTS 400

KSpreadOsziDlg::KSpreadOsziDlg( KSpreadView* view, const char* name )
    : QDialog( view, name, FALSE )
{
    m_view = view;
    setCaption(i18n("Osciloscope"));
    QVBoxLayout* lay = new QVBoxLayout( this, 6 ,6 );
    QHBoxLayout* hbox = new QHBoxLayout( lay, 6 );
    // lay->addLayout( hbox );

    m_data1 = new QGroupBox( 2, Vertical, i18n("Data X"), this );
    m_data2 = new QGroupBox( 2, Vertical, i18n("Data Y"), this );
    m_result1 = new QGroupBox( 1, Vertical, i18n("Result X"), this );
    m_result2 = new QGroupBox( 1, Vertical, i18n("Result Y"), this );
    m_time = new QGroupBox( 1, Vertical, i18n("Time"), this );

    QLabel* l = new QLabel( i18n("Cell &X" ), m_data1 );
    m_cell1 = new QLineEdit( m_data1 );
    l->setBuddy( m_cell1 );
    l = new QLabel( i18n("&From" ), m_data1 );
    m_from1 = new QLineEdit( m_data1 );
    l->setBuddy( m_from1 );
    l = new QLabel( i18n("&To" ), m_data1 );
    m_to1 = new QLineEdit( m_data1 );
    l->setBuddy( m_to1 );

    l = new QLabel( i18n("Cell &Y" ), m_data2 );
    m_cell2 = new QLineEdit( m_data2 );
    l->setBuddy( m_cell2 );
    l = new QLabel( i18n("From" ), m_data2 );
    m_from2 = new QLineEdit( m_data2 );
    l->setBuddy( m_from2 );
    l = new QLabel( i18n("To" ), m_data2 );
    m_to2 = new QLineEdit( m_data2 );
    l->setBuddy( m_to2 );
	
    l = new QLabel( i18n("C&ell" ), m_result1 );
    m_resultCell1 = new QLineEdit( m_result1 );
    l->setBuddy( m_resultCell1 );

    l = new QLabel( i18n("Cell" ), m_result2 );
    m_resultCell2 = new QLineEdit( m_result2 );
    l->setBuddy( m_resultCell2 );

    m_start = new QPushButton( i18n("Start"), m_time );
    m_stop = new QPushButton( i18n("Stop"), m_time );
    m_slider = new QSlider( Horizontal, m_time );
    m_slider->setTickmarks( QSlider::Below );

    m_slider->setEnabled( FALSE );
    m_stop->setEnabled( FALSE );
    m_start->setEnabled( FALSE );

    m_plot = new AIPlot( this, "graphic", NPOINTS );
    m_plot->setRanges(-200,200,-350.0,350.0);
    m_plot->setPlotColor(1,255,127,0);
    m_plot->setDoubleBuffer(TRUE);
    m_plot->resize( QSize( 400, 400 ) );
    m_plot->setMinimumSize( QSize( 400, 400 ) );

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    QPushButton* ok = bb->addButton( i18n("&Calc") );
    ok->setDefault( TRUE );
    QPushButton* close = bb->addButton( i18n( "Cl&ose" ) );
    bb->layout();

    QVBoxLayout* vbox = new QVBoxLayout( hbox, 6 );
    // hbox->addLayout( vbox );
    vbox->addWidget( m_data1 );
    vbox->addWidget( m_data2 );
    vbox->addWidget( m_result1 );
    vbox->addWidget( m_result2 );
    vbox->addWidget( m_time );
    vbox->addStretch( 2 );
    hbox->addWidget( m_plot );
	
    lay->addWidget( bb );
	
    connect( ok, SIGNAL( clicked() ), this, SLOT( slotCalc() ) );
    connect( close, SIGNAL( clicked() ), this, SLOT( reject() ) );

    static int xmax = 200;
    static int xmin = -200;
    static double x1[NPOINTS],y1[NPOINTS];
    for (int x=0;x<NPOINTS;x++)
    {
	x1[x]=((double)x)*(xmax-xmin)/NPOINTS+xmin;
	y1[x]=300.0 * sin( ((double)x)/100.0 );
    }
    m_plot->setData(0,NPOINTS,x1,y1);
}

void KSpreadOsziDlg::slotCalc()
{
    KSpreadCell* cell_data1 = 0;
    KSpreadCell* cell_data2 = 0;
    KSpreadCell* cell_result1 = 0;
    KSpreadCell* cell_result2 = 0;

    KSpreadPoint data1( m_cell1->text(), m_view->doc()->map() );
    if ( !data1.isValid() )
    {
	KMessageBox::error( this, i18n("Invalid cell in DataX") );
	return;
    }
    if ( data1.table == 0 )
    {
	KMessageBox::error( this, i18n("No table specified in DataX") );
	return;
    }

    bool double_data = !m_cell2->text().isEmpty();
    KSpreadPoint data2( m_cell2->text(), m_view->doc()->map() );
    if ( double_data && !data2.isValid() )
    {
	KMessageBox::error( this, i18n("Invalid cell in DataY") );
	return;
    }
    if ( double_data && data2.table == 0 )
    {
	KMessageBox::error( this, i18n("No table specified in DataY") );
	return;
    }
	
    KSpreadPoint result1( m_resultCell1->text(), m_view->doc()->map() );
    if ( !result1.isValid() )
    {
	KMessageBox::error( this, i18n("Invalid cell in ResultX") );
	return;
    }
    if ( result1.table == 0 )
    {
	KMessageBox::error( this, i18n("No table specifiedin ResultX") );
	return;
    }

    bool double_result = !m_resultCell2->text().isEmpty();
    KSpreadPoint result2( m_resultCell2->text(), m_view->doc()->map() );
    if ( double_result && !result2.isValid() )
    {
	KMessageBox::error( this, i18n("Invalid cell in ResultY") );
	return;
    }
    if ( double_result && result2.table == 0 )
    {
	KMessageBox::error( this, i18n("No table specified in Result Y") );
	return;
    }

    cell_data1 = data1.table->nonDefaultCell( data1.pos.x(), data1.pos.y() );
    if ( double_data )
	cell_data2 = data2.table->nonDefaultCell( data2.pos.x(), data2.pos.y() );
    cell_result1 = result1.table->cellAt( result1.pos.x(), result1.pos.y() );
    if ( double_result )
	cell_result2 = result2.table->cellAt( result2.pos.x(), result2.pos.y() );
	
    if ( cell_data1->content() == KSpreadCell::Formula )
    {
	KMessageBox::error( this, i18n("The Data X cell must not be a formula") );
	return;
    }
    if ( double_data && cell_data2->content() == KSpreadCell::Formula )
    {
	KMessageBox::error( this, i18n("The Data Y cell must not be a formula") );
	return;
    }
    if ( cell_result1->content() != KSpreadCell::Formula )
    {
	KMessageBox::error( this, i18n("The ResultX cell must be a formula") );
	return;
    }
    if ( double_result && cell_result1->content() != KSpreadCell::Formula )
    {
	KMessageBox::error( this, i18n("The ResultY cell must be a formula") );
	return;
    }

    QProgressDialog dlg( i18n("Calculating"), i18n("Cancel"), 400, this, 0, FALSE );
    dlg.show();

    double ymin;
    double ymax;
    double xmin = m_from1->text().toDouble();
    double xmax = m_to1->text().toDouble();
	
    if ( xmin > xmax )
    {
	double tmp = xmin;
	xmin = xmax;
	xmax = tmp;
    }

    double tmin;
    double tmax;
    if ( double_data )
    {
	tmin = m_from2->text().toDouble();
	tmax = m_to2->text().toDouble();
	
	if ( tmin > tmax )
        {
	    double tmp = tmin;
	    tmin = tmax;
	    tmax = tmp;
	}
	
	m_slider->setRange( tmin, tmax );
    }

    static double x1[NPOINTS],y1[NPOINTS];
	
    if ( double_result )
    {
	int x;
	for( x = 0; x < NPOINTS; x++ )
        {
	    double v = ((double)x)*(xmax-xmin)/NPOINTS+xmin;
	
	    cell_data1->setValue( v );
	    cell_result1->calc();
	    cell_result2->calc();
	
	    x1[x] = cell_result1->valueDouble();
	    y1[x] = cell_result2->valueDouble();
	
	    dlg.setProgress( x );
	}

	ymin = y1[0];
	ymax = y1[0];
	for( x = 0; x < NPOINTS; x++ )
        {
	    if ( ymin > y1[x] )
		ymin = y1[x];
	    if ( ymax < y1[x] )
		ymax = y1[x];
	}
	
	// ymin -= (ymin >= 0 ? ymin : -ymin ) * 0.05;
	// ymax += ymax * 0.05;

	xmin = x1[0];
	xmax = x1[0];
	for( x = 0; x < NPOINTS; x++ )
        {
	    if ( xmin > x1[x] )
		xmin = x1[x];
	    if ( xmax < x1[x] )
		xmax = x1[x];
	}
	
	// xmin -= (xmin >= 0 ? xmin : -xmin ) * 0.05;
	// xmax += xmax * 0.05;

	m_plot->setPlotType( AI_PLOT_POLAR );
	m_plot->setRanges( xmin, xmax, ymin, ymax );
	m_plot->setData( 0, NPOINTS, x1, y1 );
    }
    else
    {
	int x;
	for( x = 0; x < NPOINTS; x++ )
        {
	    x1[x]=((double)x)*(xmax-xmin)/NPOINTS+xmin;
	
	    cell_data1->setValue( x1[x] );
	    cell_result1->calc();
	
	    y1[x] = cell_result1->valueDouble();
	
	    dlg.setProgress( x );
	}

	ymin = y1[0];
	ymax = y1[0];
	for( x = 0; x < NPOINTS; x++ )
        {
	    if ( ymin > y1[x] )
		ymin = y1[x];
	    if ( ymax < y1[x] )
		ymax = y1[x];
	}
	
	ymin -= (ymin >= 0 ? ymin : -ymin ) * 0.05;
	ymax += ymax * 0.05;
	
	m_plot->setPlotType( AI_PLOT_TREND );
	m_plot->setRanges( xmin, xmax, ymin, ymax );
	m_plot->setData( 0, NPOINTS, x1, y1 );
    }

    m_slider->setEnabled( double_data );
    m_stop->setEnabled( double_data );
    m_start->setEnabled( double_data );
	
    dlg.hide();
    m_plot->repaint( FALSE );
}
#include "kspread_dlg_oszi.moc"
