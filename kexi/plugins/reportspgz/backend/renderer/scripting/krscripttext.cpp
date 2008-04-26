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
#include "krscripttext.h"

namespace Scripting
{

	Text::Text ( KRTextData* t )
	{
		_text = t;
	}


	Text::~Text()
	{
	}

	QString Text::source()
	{
		return _text->column();
	}

	void Text::setSource(const QString& s)
	{
		_text->_controlSource->setValue(s);
	}
	
	int Text::horizontalAlignment()
	{
		QString a = _text->_hAlignment->value().toString();
		
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
	void Text::setHorizonalAlignment(int a)
	{
		switch (a)
		{
			case -1:
				_text->_hAlignment->setValue("Left");
				break;
			case 0:
				_text->_hAlignment->setValue("Center");
				break;
			case 1:	
				_text->_hAlignment->setValue("Right");
				break;
			default:
				_text->_hAlignment->setValue("Left");
				break;
		}
	}
		
	int Text::verticalAlignment()
	{
		QString a = _text->_hAlignment->value().toString();
		
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
	void Text::setVerticalAlignment(int a)
	{
		switch (a)
		{
			case -1:
				_text->_hAlignment->setValue("Top");
				break;
			case 0:
				_text->_hAlignment->setValue("Middle");
				break;
			case 1:	
				_text->_hAlignment->setValue("Bottom");
				break;
			default:
				_text->_hAlignment->setValue("Top");
				break;
		}
	}
		
	QColor Text::backgroundColor()
	{
		return _text->_bgColor->value().value<QColor>();
	}
	void Text::setBackgroundColor(const QColor& c)
	{
		_text->_bgColor->setValue(QColor(c));
	}
		
	QColor Text::foregroundColor()
	{
		return _text->_fgColor->value().value<QColor>();
	}
	void Text::setForegroundColor(const QColor& c)
	{
		_text->_fgColor->setValue(QColor(c));
	}
		
	int Text::backgroundOpacity()
	{
		return _text->_bgOpacity->value().toInt();
	}
	void Text::setBackgroundOpacity(int o)
	{
		_text->_bgOpacity->setValue(o);
	}
		
	QColor Text::lineColor()
	{
		return _text->_lnColor->value().value<QColor>();
	}
	void Text::setLineColor(const QColor& c)
	{
		_text->_lnColor->setValue(QColor(c));
	}
			
	int Text::lineWeight()
	{
		return _text->_lnWeight->value().toInt();
	}
	void Text::setLineWeight(int w)
	{
		_text->_lnWeight->setValue(w);
	}
			
	int Text::lineStyle()
	{
		return _text->_lnStyle->value().toInt();
	}
	void Text::setLineStyle(int s)
	{
		if (s < 0 || s > 5)
		{
			s = 1;
		}
		_text->_lnStyle->setValue(s);
	}
	
	QPointF Text::position()
	{
		return _text->_pos.toPoint();
	}
	void Text::setPosition(const QPointF& p)
	{
		_text->_pos.setPointPos(p);
	}
			
	QSizeF Text::size()
	{
		return _text->_size.toPoint();
	}
	void Text::setSize(const QSizeF& s)
	{
		_text->_size.setPointSize(s);
	}
}
