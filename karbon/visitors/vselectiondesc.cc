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

#include "vselectiondesc.h"
#include "vselection.h"
#include "vgroup.h"
#include "vtext.h"
#include "vimage.h"
#include "vcomposite.h"
#include <kdebug.h>
#include <klocale.h>

void
VSelectionDescription::visitVSelection( VSelection& selection )
{
	if( selection.objects().count() == 1 )
		VVisitor::visitVSelection( selection );
	else
		m_desc = i18n( "One object", "%n objects", selection.objects().count() );
}

void
VSelectionDescription::visitVPath( VPath& composite )
{
	m_desc = m_shortdesc = !composite.name().isEmpty() ? composite.name() : i18n( "path" );
}

void
VSelectionDescription::visitVGroup( VGroup &group )
{
	m_desc = i18n( "One group, containing one object", "One group, containing %n objects", group.objects().count() );
	m_shortdesc = !group.name().isEmpty() ? group.name() : i18n( "group" );
}

void
VSelectionDescription::visitVText( VText &text )
{
	m_desc = m_shortdesc = !text.name().isEmpty() ? text.name() : i18n( "text" );
}

void
VSelectionDescription::visitVImage( VImage &img )
{
	m_desc = m_shortdesc = !img.name().isEmpty() ? img.name() : i18n( "image" );
}

