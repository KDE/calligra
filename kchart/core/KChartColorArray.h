/* $Id$ */

#ifndef _KCHARTCOLORARRAY_H
#define _KCHARTCOLORARRAY_H

#include <qcolor.h>
#include <qvector.h>

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
  

#endif
