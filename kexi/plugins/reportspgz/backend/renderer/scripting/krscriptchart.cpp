/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
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

	QString Chart::dataSource()
	{
		return _chart->_dataSource->value().toString();
	}

	void Chart::setDataSource ( const QString &ds )
	{
		_chart->_dataSource->setValue ( ds );
	}

	bool Chart::threeD()
	{
		return _chart->_threeD->value().toBool();
	}

	void Chart::setThreeD ( bool td )
	{
		_chart->_threeD->setValue ( td );
	}

	bool Chart::legendVisible()
	{
		return _chart->_displayLegend->value().toBool();
	}

	void Chart::setLegendVisible ( bool v )
	{
		_chart->_displayLegend->setValue ( v );
	}

	int Chart::colorScheme()
	{
		return _chart->_colorScheme->value().toInt();
	}

	void Chart::setColorScheme ( int cs )
	{
		_chart->_colorScheme->setValue ( cs );
	}

	QColor Chart::backgroundColor()
	{
		return _chart->_bgColor->value().value<QColor>();
	}

	void Chart::setBackgroundColor ( const QColor &bc )
	{
		_chart->_bgColor->setValue(bc);
	}

	QString Chart::xAxisTitle()
	{
		return _chart->_xTitle->value().toString();
	}

	void Chart::setXAxisTitle ( const QString &t )
	{
		_chart->_xTitle->setValue(t);
	}

	QString Chart::yAxisTitle()
	{
		return _chart->_yTitle->value().toString();
	}

	void Chart::setYAxisTitle ( const QString &t )
	{
		_chart->_yTitle->setValue(t);
	}
}
