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
	const KChartColorArray* dataColors() const;

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

  void setLegendFont( QFont font );
  QFont legendFont() const;

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

  // This is stuff that used to be in the painters
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
  int _linewidth;
  QList<PenStyle> _linetypes;
};


#endif
