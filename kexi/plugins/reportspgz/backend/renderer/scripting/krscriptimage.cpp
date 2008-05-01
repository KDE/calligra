//
// C++ Implementation: krscriptimage
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krscriptimage.h"
#include <krimagedata.h>

namespace Scripting
{

	Image::Image ( KRImageData *i )
	{
		_image = i;
	}


	Image::~Image()
	{
	}

	QPointF Image::position()
	{
		return _image->_pos.toPoint();
	}
	void Image::setPosition ( const QPointF& p )
	{
		_image->_pos.setPointPos ( p );
	}

	QSizeF Image::size()
	{
		return _image->_size.toPoint();
	}
	void Image::setSize ( const QSizeF& s )
	{
		_image->_size.setPointSize ( s );
	}
}
