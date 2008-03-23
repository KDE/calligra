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
		QString a = _field->_hAlignment->value().toString();
		
		if (a.toLower() == "left")
		{
			return -1;
		}
		else if (a.toLower() == "center")
		{
			return 0;
		}
		else if (a.toLower() == "right")
		{
			return 1;
		}
		return -1;
	}
	void Field::setHorizonalAlignment(int a)
	{
		switch (a)
		{
			case -1:
				_field->_hAlignment->setValue("Left");
				break;
			case 0:
				_field->_hAlignment->setValue("Center");
				break;
			case 1:	
				_field->_hAlignment->setValue("Right");
				break;
			default:
				_field->_hAlignment->setValue("Left");
				break;
		}
	}
		
	int Field::verticalAlignment()
	{
		QString a = _field->_hAlignment->value().toString();
		
		if (a.toLower() == "top")
		{
			return -1;
		}
		else if (a.toLower() == "middle")
		{
			return 0;
		}
		else if (a.toLower() == "bottom")
		{
			return 1;
		}
		return -1;
	}
	void Field::setVerticalAlignment(int a)
	{
		switch (a)
		{
			case -1:
				_field->_hAlignment->setValue("Top");
				break;
			case 0:
				_field->_hAlignment->setValue("Middle");
				break;
			case 1:	
				_field->_hAlignment->setValue("Bottom");
				break;
			default:
				_field->_hAlignment->setValue("Top");
				break;
		}
	}
		
	QColor Field::backgroundColor()
	{
		return _field->_bgColor->value().value<QColor>();
	}
	void Field::setBackgroundColor(QString c)
	{
		_field->_bgColor->setValue(QColor(c));
	}
		
	QColor Field::foregroundColor()
	{
		return _field->_fgColor->value().value<QColor>();
	}
	void Field::setForegroundColor(QString c)
	{
		_field->_fgColor->setValue(QColor(c));
	}
		
	int Field::backgroundOpacity()
	{
		return _field->_bgOpacity->value().toInt();
	}
	void Field::setBackgroundOpacity(int o)
	{
		_field->_bgOpacity->setValue(o);
	}
		
	QColor Field::lineColor()
	{
		return _field->_lnColor->value().value<QColor>();
	}
	void Field::setLineColor(QString c)
	{
		_field->_lnColor->setValue(QColor(c));
	}
			
	int Field::lineWeight()
	{
		return _field->_lnWeight->value().toInt();
	}
	void Field::setLineWeight(int w)
	{
		_field->_lnWeight->setValue(w);
	}
			
	int Field::lineStyle()
	{
		return _field->_lnStyle->value().toInt();
	}
	void Field::setLineStyle(int s)
	{
		if (s < 0 || s > 5)
		{
			s = 1;
		}
		_field->_lnStyle->setValue(s);
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
