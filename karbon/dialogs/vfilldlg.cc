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

#include <klocale.h>

#include "karbon_part.h"
#include "vfilldlg.h"
#include "vcolortab.h"
#include "vselection.h"

#include "vfillcmd.h"

#include <kdebug.h>


VFillDlg::VFillDlg( KarbonPart* part, QWidget* parent, const char* name )
	: KDialogBase ( parent, name, true, i18n( "Uniform Color" ),
		KDialogBase::Ok | KDialogBase::Cancel ), m_part( part )
{
	m_colortab = new VColorTab( part->document().selection()->objects().count() == 0 ? part->document().selection()->fill()->color() :
								part->document().selection()->objects().getFirst()->fill()->color(), this, name );

	connect( this, SIGNAL( okClicked() ), this, SLOT( slotApplyButtonPressed() ) );

	setMainWidget( m_colortab );
	setFixedSize( baseSize() );
}

void
VFillDlg::slotApplyButtonPressed()
{
	if( m_part && m_part->document().selection() )
		m_part->addCommand( new VFillCmd( &m_part->document(), VFill( m_colortab->getColor() ) ), true );

	emit fillChanged( VFill( m_colortab->getColor() ) );
}


#include "vfilldlg.moc"

