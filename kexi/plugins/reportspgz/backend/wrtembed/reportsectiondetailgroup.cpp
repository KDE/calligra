/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
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

#include "reportsectiondetailgroup.h"
#include <qobject.h>
#include "reportdesigner.h"
#include "reportsection.h"
#include "reportsectiondetail.h"
#include <kdebug.h>

//
// ReportSectionDetailGroup
//
ReportSectionDetailGroup::ReportSectionDetailGroup ( const QString & column, ReportSectionDetail * rsd, QWidget * parent, const char * name )
	: QObject ( parent )
{
	_pagebreak = BreakNone;
	ReportDesigner * rd = 0;
	_rsd = rsd;
	if ( _rsd )
	{
		rd = rsd->reportDesigner();
	}
	else
	{
		kDebug () << "Error RSD is null" <<endl;
	}
	_head = new ReportSection ( rd /*, _rsd*/ );
	_foot = new ReportSection ( rd /*, _rsd*/ );
	showGroupHead ( false );
	showGroupFoot ( false );

	setColumn ( column );
}

ReportSectionDetailGroup::~ReportSectionDetailGroup()
{
	// I delete these here so that there are no widgets 
	//left floating around
	delete _head;
	delete _foot;
}

void ReportSectionDetailGroup::showGroupHead ( bool yes )
{
	if ( isGroupHeadShowing() != yes )
	{
		if ( _rsd && _rsd->reportDesigner() ) _rsd->reportDesigner()->setModified ( true );
	}
	if ( yes ) _head->show();
	else _head->hide();
	_rsd->adjustSize();
}

void ReportSectionDetailGroup::showGroupFoot ( bool yes )
{
	if ( isGroupFootShowing() != yes )
	{
		if ( _rsd && _rsd->reportDesigner() ) _rsd->reportDesigner()->setModified ( true );
	}
	if ( yes ) _foot->show();
	else _foot->hide();
	_rsd->adjustSize();
}

void ReportSectionDetailGroup::setPageBreak ( int pb )
{
	_pagebreak = pb;
}

bool ReportSectionDetailGroup::isGroupHeadShowing() { return _head->isVisible(); }
bool ReportSectionDetailGroup::isGroupFootShowing() { return _foot->isVisible(); }
int ReportSectionDetailGroup::pageBreak() const { return _pagebreak; }

QString ReportSectionDetailGroup::column() { return _column; }
void ReportSectionDetailGroup::setColumn ( const QString & s )
{
	if ( _column != s )
	{
		_column = s;
		if ( _rsd && _rsd->reportDesigner() ) _rsd->reportDesigner()->setModified ( true );
	}
	
	_head->setTitle ( _column + " Group Header" );
	_foot->setTitle ( _column + " Group Footer" );
}

ReportSection * ReportSectionDetailGroup::getGroupHead() { return _head; }
ReportSection * ReportSectionDetailGroup::getGroupFoot() { return _foot; }




