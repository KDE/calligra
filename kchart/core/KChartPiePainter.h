/* $Id$ */

#ifndef _KCHARTPIEPAINTER_H
#define _KCHARTPIEPAINTER_H

#include "KChartPainter.h"

class KChart;
class QPaintDevice;
class QPainter;

class KChartPiePainter : public KChartPainter
{
public:
  KChartPiePainter( KChart* chart );
  ~KChartPiePainter();

  virtual void paintChart( QPaintDevice* paintdev );
  virtual void drawData( QPainter* painter );
  virtual void drawText( QPainter* painter );
  virtual QColor chooseDataColor( int dataset );
  virtual void setXLabel( const char* );
  virtual QString xLabel() const;
  virtual void setYLabel( const char* );
  virtual QString yLabel() const;
  virtual void setY1Label( const char* );
  virtual QString y1Label() const;
  virtual void setY2Label( const char* );
  virtual QString y2Label() const;
  virtual void setYMaxValue( double value );
  virtual double yMaxValue() const;
  virtual void setYMinValue( double value );
  virtual double yMinValue() const;
  virtual void setY1MaxValue( double value );
  virtual double y1MaxValue() const;
  virtual void setY1MinValue( double value );
  virtual double y1MinValue() const;
  virtual void setY2MaxValue( double value );
  virtual double y2MaxValue() const;
  virtual void setY2MinValue( double value );
  virtual double y2MinValue() const;
  virtual void setYTicksNum( int );
  virtual int yTicksNum() const;
  virtual void setXLabelSkip( int );
  virtual int xLabelSkip() const;
  virtual void setYLabelSkip( int );
  virtual int yLabelSkip() const;
  virtual void setOverwrite( OverwriteMode );
  virtual OverwriteMode overwrite() const;
  virtual void setTwoAxes( bool twoaxes );
  virtual bool twoAxes() const;
  virtual void setLongTicks( bool longticks );
  virtual bool longTicks() const;
  virtual void setLegends( QStrList legends );
  virtual QStrList legends() const;
  virtual void setLegendPlacement( LegendPlacement );
  virtual LegendPlacement legendPlacement() const;
  virtual void setZeroAxisOnly( bool only );
  virtual bool zeroAxisOnly() const;
  virtual void setBoxAxis( bool boxaxis );
  virtual bool boxAxis() const;
  virtual void setXTicks( bool xticks );
  virtual bool xTicks() const;
  virtual void setLegendMarkerWidth( int width );
  virtual int legendMarkerWidth() const;
  virtual void setLegendMarkerHeight( int height );
  virtual int legendMarkerHeight() const;

protected:
  bool setupCoords( QPaintDevice* );

private:
};

#endif
