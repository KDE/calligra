//
// C++ Implementation: krscriptchart
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krscriptchart.h"

#include <krchartdata.h>

namespace Scripting
{

	Chart::Chart ( KRChartData *c )
	{
		_chart = c;
	}


	Chart::~Chart()
	{
	}

	QPointF Chart::position()
	{
		return _chart->_pos.toPoint();
	}
	void Chart::setPosition ( const QPointF& p )
	{
		_chart->_pos.setPointPos ( p );
	}

	QSizeF Chart::size()
	{
		return _chart->_size.toPoint();
	}
	void Chart::setSize ( const QSizeF& s )
	{
		_chart->_size.setPointSize ( s );
	}
	
}
