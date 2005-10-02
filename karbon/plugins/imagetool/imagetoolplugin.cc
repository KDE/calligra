/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

*/

#include <qcursor.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kdebug.h>

#include "imagetoolplugin.h"
#include <karbon_part_base.h>
#include <karbon_part.h>
#include <karbon_view_base.h>
#include <karbon_view.h>
#include <core/vimage.h>
#include <core/vselection.h>
#include <kgenericfactory.h>

typedef KGenericFactory<VImageTool, KarbonViewBase> ImageToolPluginFactory;
K_EXPORT_COMPONENT_FACTORY( karbon_imagetoolplugin, ImageToolPluginFactory( "karbonimagetoolplugin" ) )

VImageTool::VImageTool( KarbonViewBase* view, const char *name, const QStringList & )
	: VTool( (KarbonPart *)view->part(), name ), VKarbonPlugin( view, name )
{
	registerTool( this );
}

VImageTool::~VImageTool()
{
}

QString
VImageTool::contextHelp()
{
	QString s = i18n( "<qt><b>Image tool:</b><br>" );
	return s;
}

void
VImageTool::activate()
{
	view()->setCursor( QCursor( Qt::crossCursor ) );
}

QString
VImageTool::statusText()
{
	return i18n( "Image Tool" );
}

void
VImageTool::deactivate()
{
}

void
VImageTool::mouseButtonRelease()
{
	QString fname = KFileDialog::getOpenFileName( QString::null, "*.jpg *.gif *.png", view(), i18n( "Choose Image to Add" ) );
	if( !fname.isEmpty() )
	{
		VImage *image = new VImage( 0L, fname );
		VInsertImageCmd *cmd = new VInsertImageCmd( &view()->part()->document(), i18n( "Insert Image" ), image, first() );

		view()->part()->addCommand( cmd, true );
	}
}

VImageTool::VInsertImageCmd::VInsertImageCmd( VDocument* doc, const QString& name, VImage *image, KoPoint pos )
	: VCommand( doc, name, "frame_image" ), m_image( image ), m_pos( pos )
{
}

void
VImageTool::VInsertImageCmd::execute()
{
	if( !m_image )
		return;

	if( m_image->state() == VObject::deleted )
		m_image->setState( VObject::normal );
	else
	{
		m_image->setState( VObject::normal );
		m_image->transform( QWMatrix().translate( m_pos.x(), m_pos.y() ) );
		document()->append( m_image );
		document()->selection()->clear();
		document()->selection()->append( m_image );
	}
															
	setSuccess( true );
}

void
VImageTool::VInsertImageCmd::unexecute()
{
	if( !m_image )
		return;

	document()->selection()->take( *m_image );
	m_image->setState( VObject::deleted );

	setSuccess( false );
}

