//
// C++ Implementation: krscriptreport
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krscriptreport.h"
#include <krreportdata.h>

namespace Scripting
{

	Report::Report ( KRReportData *r )
	{
		_reportdata = r;
	}


	Report::~Report()
	{
	}
	QString Report::title()
	{
		return _reportdata->title;
	}
	QString Report::recordSource()
	{
		return _reportdata->query;
	}

	KRObjectData* Report::objectByName ( const QString &n )
	{
		return _reportdata->objectByName(n);
	}

}
