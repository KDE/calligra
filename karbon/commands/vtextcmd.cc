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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <klocale.h>

#include "karbon_view.h"

#include "vpath.h"
#include "vtext.h"
#include "vtextcmd.h"

VTextCmd::VTextCmd( KarbonPart* part, const QFont& font, const QString& text )
	: VShapeCmd( part, i18n( "Insert Text" ) ), m_font( font ), m_text( text )
{
}

VObject*
VTextCmd::createPath()
{
	return
		new VText( static_cast<KarbonView*>( m_part->views().getFirst() ), m_font, m_text );
}

