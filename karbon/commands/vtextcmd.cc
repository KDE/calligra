/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
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

