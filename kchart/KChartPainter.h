/* $Id$ */

#ifndef _KCHARTPAINTER_H
#define _KCHARTPAINTER_H

#include <qcolor.h>
#include <qfont.h>
#include <qpen.h>
#include "koDiagramm.h"

class QPainter;
class table_t;
class KoDiagrammParameters;

// The base class for all chart painters
class KChartPainter
{
public:
  KChartPainter( KoDiagrammParameters* chart );
  virtual ~KChartPainter();

  void setTitleFont( QFont font );
  virtual bool checkData( table_t* table );
  virtual void paintChart( QPainter* painter, table_t* table ) = 0;
  virtual void drawData( QPainter* painter, table_t* table ) = 0;
  virtual void drawText( QPainter* painter, table_t* table ) = 0;
  virtual QPainter* setupPaintDev( QPaintDevice* paintdev );
  virtual QColor chooseDataColor( int dataset );

  inline void setWidthHeight(unsigned int w,unsigned int h) {
    _chart->_width = w; _chart->_height = h; 
    _chart->_left = _chart->_top = 0; _chart->_right = w; _chart->_bottom = h;
  }

protected:
	uint numDatasets( table_t* table );
	uint maxPos( table_t* table );
	double maxValue( line_t& );
	double minValue( line_t& );
	void minMaxOverallYValue( table_t* table, double& min, double& max );
	KoDiagrammParameters* _chart;
};

#endif
