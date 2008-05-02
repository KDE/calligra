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
#include "krscriptlabel.h"

namespace Scripting
{
	Label::Label(KRLabelData *l)
	{
		_label = l;
	}


	Label::~Label()
	{
	}

	QString Label::caption()
	{
		return _label->text();
	}

	void Label::setCaption(const QString& c)
	{
		_label->setText(c);
	}

	int Label::horizontalAlignment()
	{
		QString a = _label->_hAlignment->value().toString();
		
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
	void Label::setHorizonalAlignment(int a)
	{
		switch (a)
		{
			case -1:
				_label->_hAlignment->setValue("Left");
				break;
			case 0:
				_label->_hAlignment->setValue("Center");
				break;
			case 1:	
				_label->_hAlignment->setValue("Right");
				break;
			default:
				_label->_hAlignment->setValue("Left");
				break;
		}
	}
		
	int Label::verticalAlignment()
	{
		QString a = _label->_hAlignment->value().toString();
		
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
	void Label::setVerticalAlignment(int a)
	{
		switch (a)
		{
			case -1:
				_label->_hAlignment->setValue("Top");
				break;
			case 0:
				_label->_hAlignment->setValue("Middle");
				break;
			case 1:	
				_label->_hAlignment->setValue("Bottom");
				break;
			default:
				_label->_hAlignment->setValue("Top");
				break;
		}
	}
		
	QColor Label::backgroundColor()
	{
		return _label->_bgColor->value().value<QColor>();
	}
	void Label::setBackgroundColor(const QColor& c)
	{
		_label->_bgColor->setValue(c);
	}
		
	QColor Label::foregroundColor()
	{
		return _label->_fgColor->value().value<QColor>();
	}
	void Label::setForegroundColor(const QColor& c)
	{
		_label->_fgColor->setValue(c);
	}
		
	int Label::backgroundOpacity()
	{
		return _label->_bgOpacity->value().toInt();
	}
	void Label::setBackgroundOpacity(int o)
	{
		_label->_bgOpacity->setValue(o);
	}
		
	QColor Label::lineColor()
	{
		return _label->_lnColor->value().value<QColor>();
	}
	void Label::setLineColor(const QColor& c)
	{
		_label->_lnColor->setValue(c);
	}
			
	int Label::lineWeight()
	{
		return _label->_lnWeight->value().toInt();
	}
	void Label::setLineWeight(int w)
	{
		_label->_lnWeight->setValue(w);
	}
			
	int Label::lineStyle()
	{
		return _label->_lnStyle->value().toInt();
	}
	void Label::setLineStyle(int s)
	{
		if (s < 0 || s > 5)
		{
			s = 1;
		}
		_label->_lnStyle->setValue(s);
	}
	
	QPointF Label::position()
	{
		return _label->_pos.toPoint();
	}
	void Label::setPosition(const QPointF &p)
	{
		_label->_pos.setPointPos(p);
	}
			
	QSizeF Label::size()
	{
		return _label->_size.toPoint();
	}
	void Label::setSize(const QSizeF &s)
	{
		_label->_size.setPointSize(s);
	}
}

