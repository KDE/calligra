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

#ifndef KEXIPARTPOPUPMENU_H
#define KEXIPARTPOPUPMENU_H

#include <qpopupmenu.h>
#include <qmemarray.h>
#include <qintdict.h>

#include "kexipartitemaction.h"

typedef QMemArray<const char*> Slots;

class KEXICORE_EXPORT KexiPartPopupMenu : public QPopupMenu
{
	Q_OBJECT

	public:
		KexiPartPopupMenu(QObject *receiver);
		~KexiPartPopupMenu();

		void	insertAction(QString label, const char *slot);
		void	setPartItemId(QString part_item_id);
		const QString partItemId() { return m_part_item_id; }

	signals:
//		void	execute(int menu_item_id, const QString &part_item_id);


	protected slots:
		void slotActivated(int);

	protected:
		void insertAction( const KexiPartItemAction &a, int id );

		QString	m_part_item_id;
		QObject	*m_receiver;
		Slots	m_slots;
		QIntDict<KexiPartItemAction> m_part_item_actions;

	friend class KexiPartItemAction;
};

#endif
