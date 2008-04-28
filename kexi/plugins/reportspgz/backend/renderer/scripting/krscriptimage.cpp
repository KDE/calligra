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


}
