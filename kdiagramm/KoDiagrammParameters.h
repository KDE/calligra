/* $Id$ */

#ifndef _KDIAGRAMMPARAMETERS_H
#define _KDIAGRAMMPARAMETERS_H

#include <qcolor.h>
#include <qpen.h>
#include <qvector.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qlist.h>
#include <qstrlist.h>

enum KChartType { NoneYet, Bars, Lines, Points, LinesPoints, Area, Pie, Pie3D };
enum OverwriteMode { SideBySide, InFront, OnTop };
enum LegendPlacement { BottomLeft, BottomCenter, BottomRight,
					   RightTop, RightCenter, RightBottom };

typedef QVector<QColor> _KChartColorArray;

class KChartColorArray : protected _KChartColorArray
{
public:
  KChartColorArray() :
	_KChartColorArray( 0 ) { setAutoDelete( true ); }

  void setColor( uint pos, QColor );
  QColor color( uint pos ) const;
  uint count() const { return _KChartColorArray::count(); }
};

inline void KChartColorArray::setColor( uint pos, QColor color )
{
  if( pos >= size() )
	resize( pos + 1 );

  insert( pos, new QColor( color ) );
}


inline QColor KChartColorArray::color( uint pos ) const
{
  return *( at( pos ) );
}


struct KoDiagrammParameters
{
	KoDiagrammParameters();

	void setXLabelFont( QFont );
	QFont xLabelFont() const;
	void setYLabelFont( QFont );
	QFont yLabelFont() const;
	void setLabelFont( QFont );
	QFont labelFont() const;
	void setXAxisFont( QFont );
	QFont xAxisFont() const;
	void setYAxisFont( QFont );
	QFont yAxisFont() const;
	void setValueFont( QFont );
	QFont valueFont() const;
	void setLegendFont( QFont );
	QFont legendFont() const;
	void setTitleFont( QFont );
	QFont titleFont() const;

  KChartColorArray _datacolors;
  LegendPlacement _legendplacement;
  OverwriteMode _overwrite;
  QColor _accentcolor;
  QColor _axislabelcolor;
  QColor _bgcolor;
  QColor _fgcolor;
  QColor _labelcolor;
  QColor _textcolor;
  QList<Qt::PenStyle> _linetypes;
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
	int _xbardist; // distance between two bars in percent of bar width
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

private:
  QFont _valuefont;
  QFont _xaxisfont;
  QFont _xlabelfont;
  QFont _yaxisfont;
  QFont _ylabelfont;
  QFont _legendfont;
	QFont _titlefont;
};


inline KoDiagrammParameters::KoDiagrammParameters()
{
  // margins that will be left empty
  _topmargin = 0;
  _bottommargin = 0;
  _leftmargin = 0;
  _rightmargin = 0;

  _width = 20;
  _height = 20;

  // the colours
  _bgcolor = Qt::white;
  _fgcolor = Qt::darkBlue;
  _textcolor = Qt::darkBlue;
  _labelcolor = Qt::darkBlue;
  _axislabelcolor = Qt::darkBlue;
  _accentcolor = Qt::gray;

  // draw in transparent mode?
  _transparency = false;

  // pixels for text spacing
  _textspacing = 8;

  /* Choose a default title font */
  setTitleFont( QFont( "courier", 16 ) );

  /* Set some defaults that apply for all charts with axes */
  _ticklength = 4; // length of the tick marks
  _longticks = false; // do the ticks cross the whole chart
  _yticksnum = 5; // number of ticks on Y axis
  _xlabelskip = 1; // number of labels skipped on X axis
  _ylabelskip = 1; // number of labels skipped on Y axis
  _xticks = true; // show ticks on X axis?
  _boxaxis = true; // draw right and top of axes?
  _twoaxes = false; // two axes for first and second dataset?
  _xplotvalues = true; // print value on X axis?
  _yplotvalues = true; // print value on Y axis?
  _axisspace = 4; // space between axis and text
  _overwrite = SideBySide; // draw bars on top of each other?
  _zeroaxis = true; // draw zero axis if there are negative values?
  _zeroaxisonly = true; // draw zero axis, but not bottom axis?
  _legendmarkerheight = 8; // legend parameters
  _legendmarkerwidth = 12;
  _legendspacing = 4;
  _legendplacement = BottomCenter;
  _legendcols = -1; // not user-defined by default
  _legendrows = -1; // not user-defined by default
  // Y number format not yet supported


  /* User-supplied values for minima and maxima. */
  _ymin = 0.0;
  _ymax = 0.0;
  _y1min = 0.0;
  _y2min = 0.0;
  _y1max = 0.0;
  _y2max = 0.0;

  /* set some font defaults */
  setXLabelFont( QFont( "courier", 12 ) );
  setYLabelFont( QFont( "courier", 12 ) );
  setXAxisFont( QFont( "courier", 10 ) );
  setYAxisFont( QFont( "courier", 10 ) );
  setLegendFont( QFont( "courier", 10 ) );

  // Distance between two bars (in percent of bar width), initialized
  // with 0 here, may be overwritten for bars.
  _xbardist = 0;


  _startangle = 0;

  setValueFont( QFont( "courier", 10 ) );
  setLabelFont( QFont( "courier", 12 ) );

  _linewidth = 1;
  Qt::PenStyle* myint = new Qt::PenStyle;
  *myint = Qt::SolidLine;
  _linetypes.setAutoDelete( true );
  _linetypes.append( myint );
}




inline void KoDiagrammParameters::setXLabelFont( QFont font  )
{
  _xlabelfont = font;

  QFontMetrics fm( font );
  _xlabelfontwidth = fm.width( 'm' );
  _xlabelfontheight = fm.height();
}

inline QFont KoDiagrammParameters::xLabelFont() const
{
  return _xlabelfont;
}


inline void KoDiagrammParameters::setYLabelFont( QFont font )
{
  _ylabelfont = font;

  QFontMetrics fm( font );
  _ylabelfontwidth = fm.width( 'm' );
  _ylabelfontheight = fm.height();
}

inline QFont KoDiagrammParameters::yLabelFont() const
{
  return _ylabelfont;
}


inline void KoDiagrammParameters::setValueFont( QFont font  )
{
  _valuefont = font;
  QFontMetrics fm( font );
  _valuefontwidth = fm.width( 'm' );
  _valuefontheight = fm.height();
}

inline QFont KoDiagrammParameters::valueFont() const
{
  return _valuefont;
}


inline void KoDiagrammParameters::setXAxisFont( QFont font  )
{
  _xaxisfont = font;

  QFontMetrics fm( font );
  _xaxisfontwidth = fm.width( 'm' );
  _xaxisfontheight = fm.height();
}

inline QFont KoDiagrammParameters::xAxisFont() const
{
  return _xaxisfont;
}


inline void KoDiagrammParameters::setYAxisFont( QFont font )
{
  _yaxisfont = font;

  QFontMetrics fm( font );
  _yaxisfontwidth = fm.width( 'm' );
  _yaxisfontheight = fm.height();

}

inline QFont KoDiagrammParameters::yAxisFont() const
{
  return _yaxisfont;
}


inline void KoDiagrammParameters::setLegendFont( QFont font )
{
  _legendfont = font;

  QFontMetrics fm( font );
  _legendfontwidth = fm.width( 'm' );
  _legendfontheight = fm.height();

}

inline QFont KoDiagrammParameters::legendFont() const
{
  return _legendfont;
}


inline void KoDiagrammParameters::setLabelFont( QFont font )
{
  _xlabelfont = font;
  _ylabelfont = font;

  QFontMetrics fm( font );
  _xlabelfontwidth = fm.width( 'm' );
  _xlabelfontheight = fm.height();
  _ylabelfontwidth = fm.width( 'm' );
  _ylabelfontheight = fm.height();
}

inline QFont KoDiagrammParameters::labelFont() const
{
  return _xlabelfont;
}

inline void KoDiagrammParameters::setTitleFont( QFont font )
{
  _titlefont = font;

  QFontMetrics fm( font );
  _titlefontwidth = fm.width( 'm' );
  _titlefontheight = fm.height();

}

inline QFont KoDiagrammParameters::titleFont() const
{
  return _titlefont;
}



#endif
