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

#include <koMainWindow.h>

#include "vcontexthelpdocker.h"
#include "karbon_view.h"
#include "vtool.h"

VContextHelpDocker::VContextHelpDocker( KarbonView* view )
		: VDocker( view->shell() )
{
	helpLabel = new QLabel( i18n( "Context help will appear here." ), this );
	helpLabel->setMargin( 4 );
	helpLabel->setFrameStyle( Sunken | Box );
	helpLabel->setBackgroundMode( PaletteLight );
	
	setCaption( "Context Help" );
	setWidget( helpLabel );
} // VContextHelpDocker::VContextHelpDocker

VContextHelpDocker::~VContextHelpDocker()
{
} // VContextHelpDocker::~VContextHelpDocker

void VContextHelpDocker::manageTool( VTool* tool )
{
	helpLabel->setText( tool->contextHelp() );
	helpLabel->updateGeometry();
} // VContextHelpDocker::manageTool

#include "vcontexthelpdocker.moc"
