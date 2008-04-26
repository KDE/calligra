//
// C++ Implementation: krscriptbarcode
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
