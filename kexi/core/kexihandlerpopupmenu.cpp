/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
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

#include <kdebug.h>

#include "kexihandlerpopupmenu.h"

KexiPartPopupMenu::KexiPartPopupMenu(QObject *receiver)
{
	kdDebug() << "KexiPartPopupMenu::KexiPartPopupMenu()" << endl;

	m_receiver = receiver;
	//m_slots.resize(0);

	connect(this, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));
}

void KexiPartPopupMenu::insertAction( const KexiPartItemAction &a, int id )
{
	m_part_item_actions.insert(id, &a);
}

void
KexiPartPopupMenu::insertAction(QString label, const char *slot)
{
	kdDebug() << "KexiPartPopupMenu::insertAction()" << endl;

	int id = insertItem(label, m_slots.count());
	kdDebug() << "KexiPartPopupMenu::insertAction(): id = " << id << endl;
	m_slots.resize(id+1);
	m_slots[id]=slot;
}

void
KexiPartPopupMenu::setPartItemId(QString part_item_id)
{
	m_part_item_id = part_item_id;
}

void
KexiPartPopupMenu::slotActivated(int id)
{
	KexiPartItemAction *a = m_part_item_actions.find(id);
	if (!a)
		return;
	a->executeAction( m_part_item_id );

//	connect(this, SIGNAL(execute(const QString&)), m_receiver, m_slots.at(id));
//	emit execute(id, m_part_item_id);
//	disconnect(m_receiver);
}

KexiPartPopupMenu::~KexiPartPopupMenu()
{
}

#include "kexihandlerpopupmenu.moc"
