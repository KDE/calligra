/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIRELATIONDIALOG_H
#define KEXIRELATIONDIALOG_H

#include <qwidget.h>

class QComboBox;
class KexiRelationView;
class KexiMainWindow;

namespace KexiDB
{
	class Connection;
}

class KEXI_HAND_RELAT_EXPORT KexiRelationDialog : public QWidget
{
	Q_OBJECT

	public:
		KexiRelationDialog(QWidget *parent, KexiMainWindow *w);
		~KexiRelationDialog();

		KexiRelationView	*relationView() const { return m_relationView; }
		void			chooseTable(QString t);

	public slots:
		void			slotAddTable();

	protected:
		virtual void		keyPressEvent(QKeyEvent *ev);

	private:
		QComboBox		*m_tableCombo;
		KexiRelationView	*m_relationView;
		KexiDB::Connection	*m_conn;
};

#endif
