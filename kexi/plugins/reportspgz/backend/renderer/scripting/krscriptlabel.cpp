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
	}
	void Label::setHorizonalAlignment(int)
	{
	}
		
	int Label::verticalAlignment()
	{
	}
	void Label::setVerticalAlignment(int)
	{
	}
		
	QColor Label::backgroundColor()
	{
	}
	void Label::setBackgroundColor(QColor)
	{
	}
		
	QColor Label::foregroundColor()
	{
	}
	void Label::setForegroundColor(QColor)
	{
	}
		
	int Label::backgroundOpacity()
	{
	}
	void Label::setBackgroundOpacity(int)
	{
	}
		
	QColor Label::lineColor()
	{
	}
	void Label::setLineColor(QColor)
	{
	}
			
	int Label::lineWeight()
	{
	}
	void Label::setLineWeight(int)
	{
	}
			
	int Label::lineStyle()
	{
	}
	void Label::setLineStyle(int)
	{
	}
	
	QPointF Label::position()
	{
		
	}
	void Label::setPosition(QPointF)
	{
		
	}
			
	QSizeF Label::size()
	{
		
	}
	void Label::setSize(QSizeF)
	{
		
	}
}

