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

#include <q3buttongroup.h>
#include <QRadioButton>
#include <QString>
#include <kvbox.h>

#include <klocale.h>
#include <knuminput.h>

#include "epsexportdlg.h"


EpsExportDlg::EpsExportDlg( QWidget* parent, const char* name )
	: KDialog( parent)
{
	setButtons(Ok | Cancel);
	setDefaultButton(Ok);
	setModal(true);
	setCaption(i18n( "EPS Export" ));
	KVBox* page = new KVBox(this);
	setMainWidget(page);

	m_psLevelButtons = new Q3ButtonGroup( 1, Qt::Horizontal,
		i18n( "Options" ), page );

	QRadioButton* radio;
	radio = new QRadioButton( i18n( "PostScript level 1" ), m_psLevelButtons );
	radio = new QRadioButton( i18n( "PostScript level 2" ), m_psLevelButtons );
	radio = new QRadioButton( i18n( "PostScript level 3" ), m_psLevelButtons );

	m_psLevelButtons->setRadioButtonExclusive( true );
	m_psLevelButtons->setButton( 2 );
}

uint
EpsExportDlg::psLevel() const
{
	return static_cast<uint>(
		m_psLevelButtons->id( m_psLevelButtons->selected() ) );
}

#include "epsexportdlg.moc"

