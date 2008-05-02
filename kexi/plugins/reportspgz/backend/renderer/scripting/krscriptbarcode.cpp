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
#include "krscriptbarcode.h"

namespace Scripting
{

	Barcode::Barcode ( KRBarcodeData *b )
	{
		_barcode = b;
	}


	Barcode::~Barcode()
	{
	}

	QPointF Barcode::position()
	{
		return _barcode->_pos.toPoint();
	}
	void Barcode::setPosition ( const QPointF& p )
	{
		_barcode->_pos.setPointPos ( p );
	}

	QSizeF Barcode::size()
	{
		return _barcode->_size.toPoint();
	}
	void Barcode::setSize ( const QSizeF& s )
	{
		_barcode->_size.setPointSize ( s );
	}
	
	int Barcode::horizontalAlignment()
	{
		QString a = _barcode->_hAlignment->value().toString();
		
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
	void Barcode::setHorizonalAlignment(int a)
	{
		switch (a)
		{
			case -1:
				_barcode->_hAlignment->setValue("Left");
				break;
			case 0:
				_barcode->_hAlignment->setValue("Center");
				break;
			case 1:	
				_barcode->_hAlignment->setValue("Right");
				break;
			default:
				_barcode->_hAlignment->setValue("Left");
				break;
		}
	}
	
	QString Barcode::source()
	{
		return _barcode->_controlSource->value().toString();
	}

	void Barcode::setSource(const QString& s)
	{
		_barcode->_controlSource->setValue(s);
	}
	
	QString Barcode::format()
	{
		return _barcode->_format->value().toString();
	}

	void Barcode::setFormat(const QString& s)
	{
		_barcode->_format->setValue(s);
	}
}
