/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
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
	m_colortab = new VColorTab( part->document().selection()->objects().getFirst()->fill()->color(), this, name );

	connect( this, SIGNAL( okClicked() ), this, SLOT( slotApplyButtonPressed() ) );

	setMainWidget( m_colortab );
	setFixedSize( baseSize() );
}

void
VFillDlg::slotApplyButtonPressed()
{
	if( m_part )
		m_part->addCommand( new VFillCmd( &m_part->document(), VFill( m_colortab->getColor() ) ), true );

	emit fillChanged( VFill( m_colortab->getColor() ) );
}


#include "vfilldlg.moc"

