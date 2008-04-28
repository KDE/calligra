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


}
