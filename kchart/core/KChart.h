/* $Id$ */

#ifndef _KCHART_H
#define _KCHART_H

#include <qobject.h>
#include <qpen.h>

#include "KChartPainter.h"

class QPaintDevice;
class KChartData;
class KChartColorArray;

class KChart : public QObject
{
	Q_OBJECT

public:
  KChart( KChartType charttype = Bars );
  ~KChart();

  void setChartType( KChartType charttype );
  KChartType chartType() const;
  
  void setChartData( KChartData* );
  KChartData* chartData() const;
  
  void repaintChart( QPaintDevice* paintdev );

  void setTitle( const char* );
  QString title() const;

  void setXLabel( const char* );
  QString xLabel() const;

  void setYLabel( const char* );
  QString yLabel() const;

  void setY1Label( const char* );

  QString y1Label() const;

  void setY2Label( const char* );

  QString y2Label() const;
  
	void setTextColor( QColor color );
	QColor textColor() const;

	void setTitleFont( QFont font );
	QFont titleFont() const;

	void setLabelFont( QFont font );
	QFont labelFont() const;

	void setXLabelFont( QFont font );
	QFont xLabelFont() const;

	void setYLabelFont( QFont font );
	QFont yLabelFont() const;

	void setValueFont( QFont font );
	QFont valueFont() const;

	void setXAxisFont( QFont font );
	QFont xAxisFont() const;

	void setYAxisFont( QFont font );
	QFont yAxisFont() const;

	void setMargin( int margin );
	int margin() const;

	void setDataColors( KChartColorArray* colors );
	KChartColorArray* dataColors() const;

	void setTickLength( int );
	int tickLength() const;

	void setXTicksEnabled( bool enabled );
	bool xTicksEnabled() const;

	void setYTicksEnabled( bool enabled );
	bool yTicksEnabled() const;

  void setYTicksNum( int ticks );
  int yTicksNum() const;

	void setXLabelSkip( int skip );
	int xLabelSkip() const;

	void setYLabelSkip( int skip );
	int yLabelSkip() const;

	void setXPlotValues( bool plot );
	bool xPlotValues() const;

	void setYPlotValues( bool plot );
	bool yPlotValues() const;

	void setYMaxValue( double value );
	double yMaxValue() const;

	void setYMinValue( double value );
	double yMinValue() const;

	void setY1MaxValue( double value );
	double y1MaxValue() const;

	void setY1MinValue( double value );
	double y1MinValue() const;

	void setY2MaxValue( double value );
	double y2MaxValue() const;

	void setY2MinValue( double value );
	double y2MinValue() const;

	void setAxisSpace( int space );
	int axisSpace() const;

	void setLineType( PenStyle linetype );
	PenStyle lineType() const;

	void setLineWidth( int width );
	int lineWidth() const;

  void setOverwrite( OverwriteMode over );
  OverwriteMode overwrite() const;

  void setTwoAxes( bool twoaxes );
  bool twoAxes() const;

  void setLongTicks( bool longticks );
  bool longTicks() const;

  void setLegends( QStrList legends );
  QStrList legends() const;

  void setLegendPlacement( LegendPlacement );
  LegendPlacement legendPlacement() const;

  void setAxisLabelColor( QColor color );
  QColor axisLabelColor() const;

  void setZeroAxisOnly( bool only );
  bool zeroAxisOnly() const;

  void setBoxAxis( bool boxaxis );
  bool boxAxis() const;

  void setXTicks( bool xticks );
  bool xTicks() const;

  void setLineTypes( PenStyle types[], int number );
  void lineTypes( PenStyle types[], int& number );

  void setLegendMarkerWidth( int width );
  int legendMarkerWidth() const;

  void setLegendMarkerHeight( int height );
  int legendMarkerHeight() const;

private:
  KChartType _charttype;
  KChartPainter* _cp;
  KChartData* _chartdata;
};


#endif
