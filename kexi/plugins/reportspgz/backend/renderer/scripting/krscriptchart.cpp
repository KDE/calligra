//
// C++ Implementation: krscriptchart
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krscriptchart.h"

#include <krchartdata.h>

namespace Scripting
{

	Chart::Chart ( KRChartData *c )
	{
		_chart = c;
	}


	Chart::~Chart()
	{
	}


}
