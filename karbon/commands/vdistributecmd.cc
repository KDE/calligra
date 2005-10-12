/* This file is doc of the KDE project
   Copyright (C) 2005 The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include <klocale.h>

#include "vdistributecmd.h"
#include "vtransformcmd.h"
#include "vdocument.h"
#include "vselection.h"

VDistributeCmd::VDistributeCmd( VDocument *doc, Distribute distribute )
	: VCommand( doc, i18n( "Distribute Objects" ) ), m_distribute( distribute )
{
	m_trafoCmds.setAutoDelete( true );
}

VDistributeCmd::~VDistributeCmd()
{
}

void
VDistributeCmd::execute()
{
	if( document()->selection()->objects().count() <= 2 )
		return;
	
	KoRect bbox;
	double extent = 0.0;
	double dx, dy;

	VObjectList objs = document()->selection()->objects();
	VObjectListIterator itr( objs );

	QMap<double,VObject*> sortedPos;

	// sort by position and calculate sum of objects widht/height
	for( ; itr.current(); ++itr )
	{
		bbox = itr.current()->boundingBox();
		switch( m_distribute )
		{
			case DISTRIBUTE_HORIZONTAL_CENTER:
				sortedPos[bbox.center().x()] = itr.current();
			break;
			case DISTRIBUTE_HORIZONTAL_GAP:
			case DISTRIBUTE_HORIZONTAL_LEFT:
				sortedPos[bbox.left()] = itr.current();
				extent += bbox.width();
			break;
			case DISTRIBUTE_HORIZONTAL_RIGHT:
				sortedPos[bbox.right()] = itr.current();
			break;
			case DISTRIBUTE_VERTICAL_CENTER:
				sortedPos[bbox.center().y()] = itr.current();
			break;
			case DISTRIBUTE_VERTICAL_GAP:
			case DISTRIBUTE_VERTICAL_BOTTOM:
				sortedPos[bbox.bottom()] = itr.current();
				extent += bbox.height();
			break;
			case DISTRIBUTE_VERTICAL_TOP:
				sortedPos[bbox.top()] = itr.current();
			break;
		}
	}
	
	VObject* first = sortedPos.begin().data();
	VObject* last = (--sortedPos.end()).data();

	// determine the available space to distribute
	double space = getAvailableSpace( first, last, extent );
	double pos = 0.0, step = space / double(objs.count() - 1);

	VTranslateCmd *trafoCmd = 0L;
	QMapIterator<double,VObject*> it = sortedPos.begin(), et = sortedPos.end();

	for( ; it != et; ++it )	
	{
		if( it.data() == first || it.data() == last )
			continue;

		pos += step;

		document()->selection()->clear();
		
		bbox = it.data()->boundingBox();

		switch( m_distribute )
		{
			case DISTRIBUTE_HORIZONTAL_CENTER:
				dx = first->boundingBox().center().x() + pos - bbox.center().x();
				dy = 0.0;
			break;
			case DISTRIBUTE_HORIZONTAL_GAP:
				dx = first->boundingBox().right() + pos + 0.5 * bbox.width() - bbox.center().x();
				dy = 0.0;
				pos += bbox.width();
			break;
			case DISTRIBUTE_HORIZONTAL_LEFT:
				dx = first->boundingBox().left() + pos - bbox.left();
				dy = 0.0;
			break;
			case DISTRIBUTE_HORIZONTAL_RIGHT:
				dx = first->boundingBox().right() + pos - bbox.right();
				dy = 0.0;
			break;
			case DISTRIBUTE_VERTICAL_CENTER:
				dx = 0.0;
				dy = first->boundingBox().center().y() + pos - bbox.center().y();
			break;
			case DISTRIBUTE_VERTICAL_GAP:
				dx = 0.0;
				dy = first->boundingBox().bottom() + pos + 0.5 * bbox.height() - bbox.center().y();
				pos += bbox.height();
			break;
			case DISTRIBUTE_VERTICAL_BOTTOM:
				dx = 0.0;
				dy = first->boundingBox().bottom() + pos - bbox.bottom();
			break;
			case DISTRIBUTE_VERTICAL_TOP:
				dx = 0.0;
				dy = first->boundingBox().top() + pos - bbox.top();
			break;
		};
		document()->selection()->append( it.data() );
		trafoCmd = new VTranslateCmd( document(), dx, dy );
		m_trafoCmds.append( trafoCmd );
		trafoCmd->execute();
	}
	
	// re-add object to selection
	itr.toFirst();
	for( ; itr.current() ; ++itr )
		document()->selection()->append( itr.current() );
	setSuccess( true );
}

void
VDistributeCmd::unexecute()
{
	QPtrListIterator<VTranslateCmd> itr( m_trafoCmds );
	for( ; itr.current() ; ++itr )
		itr.current()->unexecute();
	setSuccess( false );
}

double
VDistributeCmd::getAvailableSpace( VObject *first, VObject *last, double extent )
{
	switch( m_distribute )
	{
		case DISTRIBUTE_HORIZONTAL_CENTER:
			return last->boundingBox().center().x() - first->boundingBox().center().x();
		break;
		case DISTRIBUTE_HORIZONTAL_GAP:
			extent -= first->boundingBox().width() + last->boundingBox().width();
			return last->boundingBox().left() - first->boundingBox().right() - extent;
		break;
		case DISTRIBUTE_HORIZONTAL_LEFT:
			return last->boundingBox().left() - first->boundingBox().left();
		break;
		case DISTRIBUTE_HORIZONTAL_RIGHT:
			return last->boundingBox().right() - first->boundingBox().right();
		break;
		case DISTRIBUTE_VERTICAL_CENTER:
			return last->boundingBox().center().y() - first->boundingBox().center().y();
		break;
		case DISTRIBUTE_VERTICAL_GAP:
			extent -= first->boundingBox().height() + last->boundingBox().height();
			return last->boundingBox().top() - first->boundingBox().bottom() - extent;
		break;
		case DISTRIBUTE_VERTICAL_BOTTOM:
			return last->boundingBox().bottom() - first->boundingBox().bottom();
		break;
		case DISTRIBUTE_VERTICAL_TOP:
			return last->boundingBox().top() - first->boundingBox().top();
		break;
	}

	return 0.0;
}
