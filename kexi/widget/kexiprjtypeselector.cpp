/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kexiprjtypeselector.h"
#include <qlabel.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <kexidb/driver.h>

KexiPrjTypeSelector::KexiPrjTypeSelector( QWidget* parent )
	: Ui::KexiPrjTypeSelector( parent )
{
	setupUi(this);
	setObjectName("KexiPrjTypeSelector");
	QString none;
	icon_file->setPixmap( 
		KGlobal::iconLoader()->loadIcon( KMimeType::mimeType( 
			KexiDB::Driver::defaultFileBasedDriverMimeType() )->icon(none,0), KIcon::Desktop, 48
		)
	);
	icon_file->setFixedSize(icon_file->pixmap()->size()/2);
	icon_server->setPixmap( DesktopIcon("network-wired", 48) );
	icon_server->setFixedSize(icon_server->pixmap()->size()/2);
  
	connect( buttonGroup, SIGNAL( clicked(int) ),
		this, SLOT( slotSelectionChanged(int) ) );
	
	slotSelectionChanged( 1 );
}

KexiPrjTypeSelector::~KexiPrjTypeSelector()
{
}

void KexiPrjTypeSelector::slotSelectionChanged( int id )
{
	frame_server->setEnabled(id==2);
}

#include "kexiprjtypeselector.moc"
