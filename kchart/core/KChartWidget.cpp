/* $Id$ */

#include "KChartWidget.h"
#include "KChartWidget.moc"
#include "KChartData.h"

KChartWidget::KChartWidget( KChartType charttype, QWidget* parent, 
							const char* name, WFlags f ) :
	QWidget( parent, name,f )
{
  _chart = new KChart( charttype );
}


KChartWidget::~KChartWidget()
{
	delete _chart;
}


void KChartWidget::setChartType( KChartType /* charttype */  )
{
  fatal( "Sorry, not implemented: KChartWidget::setChartType" );
}


KChartType KChartWidget::chartType() const
{
  fatal( "Sorry, not implemented: KChartWidget::chartType" );
  return Bars;
}


void KChartWidget::setChartData( KChartData* data )
{
  _chart->setChartData( data );

  connect( data, SIGNAL( dataChanged() ), this, SLOT( newData() ) );
}


KChartData* KChartWidget::chartData() const
{
  return _chart->chartData();
}


void KChartWidget::paintEvent( QPaintEvent* /* event */ )
{
  _chart->repaintChart( this );
}


void KChartWidget::resizeEvent( QResizeEvent* /* event */ )
{
  _chart->repaintChart( this );
}


void KChartWidget::newData()
{
  _chart->repaintChart( this );
}



