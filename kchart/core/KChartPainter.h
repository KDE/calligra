/* $Id$ */

#ifndef _KCHARTPAINTER_H
#define _KCHARTPAINTER_H

#include "KChartColorArray.h"
#include "KChartTypes.h"

#include <qcolor.h>
#include <qfont.h>
#include <qpen.h>

class KChart;
class QPainter;

// The base class for all chart painters
class KChartPainter
{
public:
  KChartPainter( KChart* chart );
  virtual ~KChartPainter();

  void setTitleFont( QFont font );
  virtual bool checkData();
  virtual void paintChart( QPaintDevice* paintdev ) = 0;
  virtual void drawData( QPainter* painter ) = 0;
  virtual void drawText( QPainter* painter ) = 0;
  virtual QPainter* setupPaintDev( QPaintDevice* paintdev );
  virtual QColor chooseDataColor( int dataset );
  virtual void setTitle( const char* );
  virtual QString title() const;
  virtual void setXLabel( const char* ) = 0;
  virtual QString xLabel() const = 0;
  virtual void setYLabel( const char* ) = 0;
  virtual QString yLabel() const = 0;
  virtual void setY1Label( const char* ) = 0;
  virtual QString y1Label() const = 0;
  virtual void setY2Label( const char* ) = 0;
  virtual QString y2Label() const = 0;
  virtual void setYMaxValue( double value ) = 0;
  virtual double yMaxValue() const = 0;
  virtual void setYMinValue( double value ) = 0;
  virtual double yMinValue() const = 0;
  virtual void setY1MaxValue( double value ) = 0;
  virtual double y1MaxValue() const = 0;
  virtual void setY1MinValue( double value ) = 0;
  virtual double y1MinValue() const = 0;
  virtual void setY2MaxValue( double value ) = 0;
  virtual double y2MaxValue() const = 0;
  virtual void setY2MinValue( double value ) = 0;
  virtual double y2MinValue() const = 0;
  virtual void setYTicksNum( int ) = 0;
  virtual int yTicksNum() const = 0;
  virtual void setXLabelSkip( int ) = 0;
  virtual int xLabelSkip() const = 0;
  virtual void setYLabelSkip( int ) = 0;
  virtual int yLabelSkip() const = 0;
  virtual void setOverwrite( OverwriteMode ) = 0;
  virtual OverwriteMode overwrite() const = 0;
  virtual void setTwoAxes( bool twoaxes ) = 0;
  virtual bool twoAxes() const = 0;
  virtual void setLongTicks( bool longticks ) = 0;
  virtual bool longTicks() const = 0;
  virtual void setLegends( QStrList legends ) = 0;
  virtual QStrList legends() const = 0;
  virtual void setLegendPlacement( LegendPlacement ) = 0;
  virtual LegendPlacement legendPlacement() const = 0;
  virtual void setAxisLabelColor( QColor color );
  virtual QColor axisLabelColor() const;
  virtual void setZeroAxisOnly( bool only ) = 0;
  virtual bool zeroAxisOnly() const = 0;
  virtual void setDataColors( KChartColorArray* colors );
  virtual KChartColorArray* dataColors() const;
  virtual void setBoxAxis( bool boxaxis ) = 0;
  virtual bool boxAxis() const = 0;
  virtual void setLineWidth( int width );
  virtual int lineWidth() const;
  virtual void setXTicks( bool xticks ) = 0;
  virtual bool xTicks() const = 0;
  virtual void setLineTypes( PenStyle types[], int number );
  virtual void lineTypes( PenStyle types[], int& number );
  virtual void setLegendMarkerWidth( int width ) = 0;
  virtual int legendMarkerWidth() const = 0;
  virtual void setLegendMarkerHeight( int height ) = 0;
  virtual int legendMarkerHeight() const = 0;

protected:

  KChart* _chart;
  KChartColorArray _datacolors;

  uint _topmargin;
  uint _bottommargin;
  uint _leftmargin;
  uint _rightmargin;

  uint _top;
  uint _bottom;
  uint _left;
  uint _right;

  QColor _bgcolor;
  QColor _fgcolor;
  QColor _textcolor;
  QColor _labelcolor;
  QColor _axislabelcolor;
  QColor _accentcolor;

  QString _title;

  bool _transparency;

  uint _textspacing;

  QFont _titlefont;
  int _titlefontwidth;
  int _titlefontheight;
};

#endif
