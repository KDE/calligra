/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexipartitemaction.h"
#include "kexihandlerpopupmenu.h"
#include "kdebug.h"

KexiPartItemAction::KexiPartItemAction( 
	const QString& text, const QString& pix, const KShortcut& cut,
	const QObject* receiver, const char* slot,
	KActionCollection* parent, const char* name )
: KAction( text, pix, cut, 0, 0, parent, name )
	,m_receiver(receiver)
	,m_slot(slot)
{
	if (m_receiver && m_slot)
		connect(this, SIGNAL(execute(const QString&)), m_receiver, m_slot);
}

int KexiPartItemAction::plug( QWidget *w, int index )
{
	int plug_i = KAction::plug(w, index);
	int id = itemId( containerCount()-1 );

	if (w->isA("KexiPartPopupMenu") && m_receiver && m_slot) {
		KexiPartPopupMenu *popup = (KexiPartPopupMenu *)w;
		//store id of this action item inside popupmenu struct.
		popup->insertAction( *this, id );
//		connect(this, SIGNAL(execute(const QString&)), m_receiver, m_slot);
			//, m_receiver, m_slot);
//		connect(popup, SIGNAL(execute(int, const QString&)), this, SLOT(slotExecute(int, const QString &)));
			//, m_receiver, m_slot);
//		connect(this,SIGNAL(activated()),this,SLOT(slotActivated()));
	}
	return plug_i;
}

void KexiPartItemAction::executeAction( const QString &part_item_id )
{
	emit execute(part_item_id);
}

#include "kexipartitemaction.moc"
