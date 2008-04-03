//
// C++ Implementation: krscriptlabel
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
	void Label::setBackgroundColor(QString c)
	{
		_label->_bgColor->setValue(QColor(c));
	}
		
	QColor Label::foregroundColor()
	{
		return _label->_fgColor->value().value<QColor>();
	}
	void Label::setForegroundColor(QString c)
	{
		_label->_fgColor->setValue(QColor(c));
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
	void Label::setLineColor(QString c)
	{
		_label->_lnColor->setValue(QColor(c));
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
	void Label::setPosition(qreal x, qreal y)
	{
		_label->_pos.setPointPos(QPointF(x,y));
	}
			
	QSizeF Label::size()
	{
		return _label->_size.toPoint();
	}
	void Label::setSize(qreal w, qreal h)
	{
		_label->_size.setPointSize(QSizeF(w,h));
	}
}

