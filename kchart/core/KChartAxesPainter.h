/* $Id$ */

#ifndef _KCHARTAXESPAINTER_H
#define _KCHARTAXESPAINTER_H

#include "KChartPainter.h"
#include "KChartData.h"

#include <qfont.h>
#include <qstrlist.h>

class KChart;
class QPaintDevice;
class QPainter;

// The base class for all chart painters
class KChartAxesPainter : public KChartPainter
{
public:
  KChartAxesPainter( KChart* chart );
  virtual ~KChartAxesPainter();

  void setXLabelFont( QFont font );
  void setYLabelFont( QFont font );
  void setXAxisFont( QFont font );
  void setYAxisFont( QFont font );
  void setLegendFont( QFont font );
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


  virtual void paintChart( QPaintDevice* paintdev );
  virtual void drawData( QPainter* painter ) = 0;
  virtual void drawText( QPainter* painter );
  virtual void drawAxes( QPainter* painter );
  virtual void drawTicks( QPainter* painter );
  virtual void drawLegend( QPainter* painter );
  virtual void drawLegendMarker( QPainter* painter, int number, int x, int y );

protected:
  QPoint valToPixel( uint pos, double value, uint dataset );
  virtual bool setupCoords( QPaintDevice* );
  virtual void setupLegend( QPaintDevice* );
  virtual bool setMaxMin();
  virtual void createYLabels();
  double upperBound( double );
  double lowerBound( double );

  double _ymins[2]; // calculated
  double _ymaxs[2]; // calculated
  QStrList _legends;
  QFont _xlabelfont;
  QFont _ylabelfont;
  QFont _xaxisfont;
  QFont _yaxisfont;
  QFont _legendfont;
  QString _xlabel;
  QString _ylabel;
  QString _y1label;
  QString _y2label;
  double _ylabels[2][100]; // PENDING(kalle): Make this dynamic, use STL here
  int _ticklength;
  double _ymin; // user-supplied
  double _y1min; // user-supplied
  double _y2min; // user-supplied
  double _ymax; // user-supplied
  double _y1max; // user-supplied
  double _y2max; // user-supplied
  int _yticksnum;
  int _xlabelskip;
  int _ylabelskip;
  int _axisspace;
  int _legendmarkerheight;
  int _legendelementheight;
  int _legendmarkerwidth;
  int _legendelementwidth;
  int _legendspacing;
  int _legendnum; // number of legend entries, computed
  int _legendcols;
  int _legendrows;
  int _legendxsize;
  int _legendysize;
  int _legendxstart;
  int _legendystart;
  int _textfontwidth;
  int _textfontheight;
  int _xlabelfontwidth;
  int _xlabelfontheight;
  int _ylabelfontwidth;
  int _ylabelfontheight;
  int _ylabelfontheight1;
  int _ylabelfontheight2;
  int _xaxisfontwidth;
  int _xaxisfontheight;
  int _yaxisfontwidth;
  int _yaxisfontheight;
  int _legendfontwidth;
  int _legendfontheight;
  LegendPlacement _legendplacement;
  int _xstep;
  int _zeropoint;
  int _ylabellength[2];
  bool _longticks;
  bool _xticks;
  bool _boxaxis;
  bool _twoaxes;
  bool _xplotvalues;
  bool _yplotvalues;
  OverwriteMode _overwrite;
  bool _zeroaxis;
  bool _zeroaxisonly;
};

#endif
