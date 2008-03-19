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
		return 0;
	}
	void Label::setHorizonalAlignment(int)
	{
	}
		
	int Label::verticalAlignment()
	{
		return 0;
	}
	void Label::setVerticalAlignment(int)
	{
	}
		
	QColor Label::backgroundColor()
	{
		return QColor();
	}
	void Label::setBackgroundColor(QColor)
	{
	}
		
	QColor Label::foregroundColor()
	{
		return QColor();
	}
	void Label::setForegroundColor(QColor)
	{
	}
		
	int Label::backgroundOpacity()
	{
		return 0;
	}
	void Label::setBackgroundOpacity(int)
	{
	}
		
	QColor Label::lineColor()
	{
		return QColor();
	}
	void Label::setLineColor(QColor)
	{
	}
			
	int Label::lineWeight()
	{
		return 1;
	}
	void Label::setLineWeight(int)
	{
	}
			
	int Label::lineStyle()
	{
		return 0;
	}
	void Label::setLineStyle(int)
	{
	}
	
	QPointF Label::position()
	{
		return QPointF();
	}
	void Label::setPosition(QPointF)
	{
		
	}
			
	QSizeF Label::size()
	{
		return QSizeF();
	}
	void Label::setSize(QSizeF)
	{
		
	}
}

