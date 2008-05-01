//
// C++ Implementation: krscriptline
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krscriptline.h"
#include <krlinedata.h>

namespace Scripting
{

	Line::Line ( KRLineData* l )
	{
		_line = l;
	}

	Line::~Line()
	{
	}

	
	QColor Line::lineColor()
	{
		return _line->_lnColor->value().value<QColor>();
	}
	
	void Line::setLineColor(const QColor& c)
	{
		_line->_lnColor->setValue(c);
	}
			
	int Line::lineWeight()
	{
		return _line->_lnWeight->value().toInt();
	}
	
	void Line::setLineWeight(int w)
	{
		_line->_lnWeight->setValue(w);
	}
			
	int Line::lineStyle()
	{
		return _line->_lnStyle->value().toInt();
	}
	void Line::setLineStyle(int s)
	{
		if (s < 0 || s > 5)
		{
			s = 1;
		}
		_line->_lnStyle->setValue(s);
	}
	
	QPointF Line::startPosition()
	{
		return _line->_start.toPoint();
	}
	
	void Line::setStartPosition(const QPointF& p)
	{
		_line->_start.setPointPos(p);
	}
	
	QPointF Line::endPosition()
	{
		return _line->_end.toPoint();
	}
	
	void Line::setEndPosition(const QPointF& p)
	{
		_line->_end.setPointPos(p);
	}
}
