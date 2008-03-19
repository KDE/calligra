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
		return 0;
	}
	void Field::setHorizonalAlignment(int)
	{
	}
		
	int Field::verticalAlignment()
	{
		return 0;
	}
	void Field::setVerticalAlignment(int)
	{
	}
		
	QColor Field::backgroundColor()
	{
		return QColor();
	}
	void Field::setBackgroundColor(QColor)
	{
	}
		
	QColor Field::foregroundColor()
	{
		return QColor();
	}
	void Field::setForegroundColor(QColor)
	{
	}
		
	int Field::backgroundOpacity()
	{
		return 0;
	}
	void Field::setBackgroundOpacity(int o)
	{
		_field->_bgOpacity->setValue(o);
	}
		
	QColor Field::lineColor()
	{
		return QColor();
	}
	void Field::setLineColor(QColor)
	{
	}
			
	int Field::lineWeight()
	{
		return 1;
	}
	void Field::setLineWeight(int)
	{
	}
			
	int Field::lineStyle()
	{
		return 0;
	}
	void Field::setLineStyle(int)
	{
	}
	
	QPointF Field::position()
	{
		return QPointF();
	}
	void Field::setPosition(QPointF)
	{
		
	}
			
	QSizeF Field::size()
	{
		return QSizeF();
	}
	void Field::setSize(QSizeF)
	{
		
	}
}
