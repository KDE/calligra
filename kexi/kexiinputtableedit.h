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

#ifndef KEXIINPUTTABLEEDIT_H
#define KEXIINPUTTABLEEDIT_H

#include <qlineedit.h>
#include <qvariant.h>

class KexiInputTableEdit : public QLineEdit
{
	Q_OBJECT
	
	public:
		KexiInputTableEdit(QVariant value, QVariant::Type type, QWidget *parent, const char *name=0);
		~KexiInputTableEdit();

		virtual QVariant value();

	protected:
		void showHintButton();

		void keyPressEvent(QKeyEvent *ev);

		
		QVariant::Type	m_type;
		QVariant	m_value;
	signals:
		void hintClicked();
};

#endif
