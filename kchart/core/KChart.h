/* $Id$ */

#ifndef _KCHART_H
#define _KCHART_H

#include <qobject.h>
#include <qpen.h>
#include <qstrlist.h>
#include <qfont.h>

class QPaintDevice;
class KChartData;
class KChartPainter;

#include <KChartTypes.h>
#include <KChartColorArray.h>

class KChart : public QObject
{
friend class KChartPainter;
friend class KChartLinesPainter;
friend class KChartLinesPointsPainter;
friend class KChartBarsPainter;
friend class KChartAreaPainter;
friend class KChartAxesPainter;
friend class KChartPointsPainter;
friend class KChartPiePainter;
friend class KChartPie3DPainter;

	Q_OBJECT

public:
  KChart( KChartType charttype = Bars );
  ~KChart();

  void setChartData( KChartData* );
  void addAutoUpdate( QPaintDevice* device );
  void removeAutoUpdate( QPaintDevice* device );

  KChartType chartType() const;
  KChartData* chartData() const;
  
  void repaintChart( QPaintDevice* paintdev );

  QString title() const;
  QString xLabel() const;
  QString yLabel() const;
  QString y1Label() const;
  QString y2Label() const;
  QColor textColor() const;
  QFont titleFont() const;
  QFont labelFont() const;
  QFont xLabelFont() const;
  QFont yLabelFont() const;
  QFont valueFont() const;
  QFont xAxisFont() const;
  QFont yAxisFont() const;
  int margin() const;
  const KChartColorArray* dataColors() const;
  int tickLength() const;
  bool xTicksEnabled() const;
  bool yTicksEnabled() const;
  int yTicksNum() const;
  int xLabelSkip() const;
  int yLabelSkip() const;
  bool xPlotValues() const;
  bool yPlotValues() const;
  double yMaxValue() const;
  double yMinValue() const;
  double y1MaxValue() const;
  double y1MinValue() const;
  double y2MaxValue() const;
  double y2MinValue() const;
  int axisSpace() const;
  PenStyle lineType() const;
  int lineWidth() const;
  OverwriteMode overwrite() const;
  bool twoAxes() const;
  bool longTicks() const;
  QStrList legends() const;
  LegendPlacement legendPlacement() const;
  QFont legendFont() const;
  QColor axisLabelColor() const;
  bool zeroAxisOnly() const;
  bool boxAxis() const;
  bool xTicks() const;
  void lineTypes( PenStyle types[], int& number );
  int legendMarkerWidth() const;
  int legendMarkerHeight() const;
  int pieHeight() const;

public slots:
  void setChartType( KChartType charttype );
  void setTitle( const char* );
  void setXLabel( const char* );
  void setYLabel( const char* );
  void setY1Label( const char* );
  void setY2Label( const char* );
  void setTextColor( QColor color );
  void setTitleFont( QFont font );
  void setLabelFont( QFont font );
  void setXLabelFont( QFont font );
  void setYLabelFont( QFont font );
  void setValueFont( QFont font );
  void setXAxisFont( QFont font );
  void setYAxisFont( QFont font );
  void setMargin( int margin );
  void setDataColors( KChartColorArray* colors );
  void setTickLength( int );
  void setYTicksNum( int ticks );
  void setXLabelSkip( int skip );
  void setYLabelSkip( int skip );
  void setXPlotValues( bool plot );
  void setYPlotValues( bool plot );
  void setYMaxValue( double value );
  void setYMinValue( double value );
  void setY1MaxValue( double value );
  void setY1MinValue( double value );
  void setY2MaxValue( double value );
  void setY2MinValue( double value );
  void setAxisSpace( int space );
  void setLineWidth( int width );
  void setOverwrite( OverwriteMode over );
  void setTwoAxes( bool twoaxes );
  void setLongTicks( bool longticks );
  void setLegends( QStrList legends );
  void setLegendPlacement( int );
  void setLegendFont( QFont font );
  void setAxisLabelColor( QColor color );
  void setZeroAxisOnly( bool only );
  void setBoxAxis( bool boxaxis );
  void setXTicks( bool xticks );
  void setLineTypes( PenStyle types[], int number );
  void setLegendMarkerWidth( int width );
  void setLegendMarkerHeight( int height );
  void setPieHeight( int height );

private:
  void doAutoUpdate();

  KChartType _charttype;
  KChartPainter* _cp;
  KChartData* _chartdata;

  KChartColorArray _datacolors;
  LegendPlacement _legendplacement;
  OverwriteMode _overwrite;
  QColor _accentcolor;
  QColor _axislabelcolor;
  QColor _bgcolor;
  QColor _fgcolor;
  QColor _labelcolor;
  QColor _textcolor;
  QFont _legendfont;
  QFont _titlefont;
  QFont _valuefont;
  QFont _xaxisfont;
  QFont _xlabelfont;
  QFont _yaxisfont;
  QFont _ylabelfont;
  QList<PenStyle> _linetypes;
  QList<QPaintDevice> _autoupdatedevices;
  QStrList _legends;
  QString _title;
  QString _xlabel;
  QString _y1label;
  QString _y2label;
  QString _ylabel;
  bool _blockautoupdate;
  bool _boxaxis;
  bool _longticks;
  bool _transparency;
  bool _twoaxes;
  bool _xplotvalues;
  bool _xticks;
  bool _yplotvalues;
  bool _zeroaxis;
  bool _zeroaxisonly;
  double _y1max; // user-supplied
  double _y1min; // user-supplied
  double _y2max; // user-supplied
  double _y2min; // user-supplied
  double _ylabels[2][100]; // PENDING(kalle): Make this dynamic, use STL here
  double _ymax; // user-supplied
  double _ymaxs[2]; // calculated
  double _ymin; // user-supplied
  double _ymins[2]; // calculated
  int _axisspace;
  int _legendcols;
  int _legendelementheight;
  int _legendelementwidth;
  int _legendfontheight;
  int _legendfontwidth;
  int _legendmarkerheight;
  int _legendmarkerwidth;
  int _legendnum; // number of legend entries, computed
  int _legendrows;
  int _legendspacing;
  int _legendxsize;
  int _legendxstart;
  int _legendysize;
  int _legendystart;
  int _linewidth;
  int _pieheight;
  int _startangle;
  int _textfontheight;
  int _textfontwidth;
  int _ticklength;
  int _titlefontheight;
  int _titlefontwidth;
  int _valuefontheight;
  int _valuefontwidth;
  int _xaxisfontheight;
  int _xaxisfontwidth;
  int _xlabelfontheight;
  int _xlabelfontwidth;
  int _xlabelskip;
  int _xstep;
  int _yaxisfontheight;
  int _yaxisfontwidth;
  int _ylabelfontheight1;
  int _ylabelfontheight2;
  int _ylabelfontheight;
  int _ylabelfontwidth;
  int _ylabellength[2];
  int _ylabelskip;
  int _yticksnum;
  int _zeropoint;
  uint _bottom;
  uint _bottommargin;
  uint _height;
  uint _left;
  uint _leftmargin;
  uint _right;
  uint _rightmargin;
  uint _textspacing;
  uint _top;
  uint _topmargin;
  uint _width;
};


#endif
