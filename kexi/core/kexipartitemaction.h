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

#ifndef KEXIPARTITEMACTION_H
#define KEXIPARTITEMACTION_H

#include <kaction.h>

class KEXICORE_EXPORT KexiPartItemAction : public KAction
{
	Q_OBJECT

	public:
		KexiPartItemAction( const QString& text, const QString& pix, const KShortcut& cut,
			const QObject* receiver, const char* slot,
			KActionCollection* parent, const char* name );

		virtual int plug( QWidget *w, int index = -1 );

	signals:
		void execute(const QString&);

	protected:
		void executeAction( const QString &part_item_id );

		const QObject *m_receiver;
		const char *m_slot;

	friend class KexiPartPopupMenu;
};

#endif
