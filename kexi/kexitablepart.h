/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#ifndef KEXITABLEPART_H
#define KEXITABLEPART_H

#include "kexiprojectpart.h"

class QPixmap;

class KexiTablePart : public KexiProjectPart
{
	Q_OBJECT

	public:
		KexiTablePart(KexiProject *project);
		~KexiTablePart();

		QString				name();
		QString				mime();
		bool				visible();

		KexiPartPopupMenu		*groupContext();
		KexiPartPopupMenu		*itemContext();

//		KexiBrowserItem	*group();
//		KexiBrowserItem	*itemTemplate();

		QPixmap				groupPixmap();
		QPixmap				itemPixmap();

		ItemList			*items();

		void				open(QString identifier);
		KexiDialogBase			*view(QString identifier);


		void				getTables();

	private:
		KexiProject			*m_project;
		ItemList			*m_items;

	public slots:
		void				slotCreate();
};

#endif
