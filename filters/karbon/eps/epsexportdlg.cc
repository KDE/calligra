/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qstring.h>

#include <klocale.h>
#include <knuminput.h>

#include "epsexportdlg.h"

EpsExportDlg::EpsExportDlg( QWidget* parent, const char* name )
	: KDialogBase( parent, name, true, i18n( "EPS Export" ), Ok | Cancel )
{
	QWidget* page = new QWidget( this );
	setMainWidget( page );

	m_psLevelButtons = new QButtonGroup( i18n( "Options" ), page );

	QRadioButton* radio;
	radio = new QRadioButton( i18n( "PostScript Level 1" ), m_psLevelButtons );
	radio = new QRadioButton( i18n( "PostScript Level 2" ), m_psLevelButtons );
	radio->setEnabled( false );
	radio = new QRadioButton( i18n( "PostScript Level 3" ), m_psLevelButtons );
	radio->setEnabled( false );
}

uint
EpsExportDlg::psLevel() const
{
	return
		static_cast<uint>(
			m_psLevelButtons->id( m_psLevelButtons->selected() ) );
}

#include "epsexportdlg.moc"

