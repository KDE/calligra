/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qlabel.h>
#include <qlayout.h>
#include "vtooloptionsdocker.h"
#include <koMainWindow.h>
#include "karbon_view.h"
#include "vtool.h"

VToolOptionsDocker::VToolOptionsDocker( KarbonView* view )
		: VDocker( view->shell() )
{
	noOptionLabel = new QLabel( i18n( "There are no options for the selected tool" ), this );
	
	setCaption( "Karbon tool" );
	setWidget( noOptionLabel );
} // VToolOptionsDocker::VToolOptionsDocker

VToolOptionsDocker::~VToolOptionsDocker()
{
	widget()->reparent( 0L, QPoint( 0, 0 ) );
	
	delete noOptionLabel;
} // VToolOptionsDocker::~VToolOptionsDocker

void VToolOptionsDocker::manageTool( VTool* tool )
{
	setCaption( i18n( "Properties: %1" ).arg( tool->name() ) );
	widget()->reparent( 0L, QPoint( 0, 0 ) );
	resize( 0, 0 );
	QWidget* w;
	if ( !( w = tool->optionsWidget() ) )
		w = noOptionLabel;
	w->reparent( this, QPoint( 0, 0 ) );
	resize( 0, 0 );
	setWidget( w );
	w->show();
	 // TODO: Find why the docker shrinks only for the next tool...
} // VToolOptionsDocker::manageTool

#include "vtooloptionsdocker.moc"