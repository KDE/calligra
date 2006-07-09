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

#include "vimagetool.h"
#include <karbon_part.h>
#include <karbon_view.h>
#include <core/vimage.h>
#include <core/vselection.h>
#include <core/vcursor.h>

VImageTool::VImageTool( KarbonView *view ) : VTool( view, "tool_image_plugin" )
{
	registerTool( this );
	m_cursor = new QCursor( VCursor::createCursor( VCursor::CrossHair ) );
}

VImageTool::~VImageTool()
{
	delete m_cursor;
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
	view()->setCursor( *m_cursor );
	VTool::activate();
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

void
VImageTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KRadioAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
		m_action = new KRadioAction( i18n( "Image Tool" ), "14_image", Qt::SHIFT+Qt::Key_H, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Image" ) );
		m_action->setExclusiveGroup( "misc" );
		//m_ownAction = true;
	}
}

