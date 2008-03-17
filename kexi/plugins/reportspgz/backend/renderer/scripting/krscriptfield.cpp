//
// C++ Implementation: krscriptfield
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krscriptfield.h"

namespace Scripting
{
	Field::Field(KRFieldData *f)
	{
		_field = f;
	}


	Field::~Field()
	{
	}

	QString Field::source()
	{
		return _field->column();
	}

	void Field::setSource(const QString& s)
	{
		_field->setColumn(s);
	}
	
	int Field::horizontalAlignment()
	{
	}
	void Field::setHorizonalAlignment(int)
	{
	}
		
	int Field::verticalAlignment()
	{
	}
	void Field::setVerticalAlignment(int)
	{
	}
		
	QColor Field::backgroundColor()
	{
	}
	void Field::setBackgroundColor(QColor)
	{
	}
		
	QColor Field::foregroundColor()
	{
	}
	void Field::setForegroundColor(QColor)
	{
	}
		
	int Field::backgroundOpacity()
	{
		
	}
	void Field::setBackgroundOpacity(int o)
	{
		_field->_bgOpacity->setValue(o);
	}
		
	QColor Field::lineColor()
	{
	}
	void Field::setLineColor(QColor)
	{
	}
			
	int Field::lineWeight()
	{
	}
	void Field::setLineWeight(int)
	{
	}
			
	int Field::lineStyle()
	{
	}
	void Field::setLineStyle(int)
	{
	}
	
	QPointF Field::position()
	{
		
	}
	void Field::setPosition(QPointF)
	{
		
	}
			
	QSizeF Field::size()
	{
		
	}
	void Field::setSize(QSizeF)
	{
		
	}
}
