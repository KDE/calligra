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

#ifndef KEXICREATEPROJECTPAGEDB_H
#define KEXICREATEPROJECTPAGEDB_H

#include "kexiproject.h"
#include "kexicreateprojectpage.h"

class KListView;
class QTimer;
class QRadioButton;
class KLineEdit;

class KexiCreateProject;

class KexiCreateProjectPageDB : public KexiCreateProjectPage
{
	Q_OBJECT
	
	public:
		KexiCreateProjectPageDB(KexiCreateProject *parent, QPixmap *wpic, const char *name=0);
		~KexiCreateProjectPageDB();

		void		connectHost(QString driver, QString host, QString user, QString password,
			QString socket, QString port, bool savePass);
		bool		connectDB();

	protected:
		KListView	*m_databases;
		QRadioButton *m_existingRBtn;
		QRadioButton *m_newRBtn;
		KLineEdit *m_newEdit;
		Credentials	m_cred;

	protected slots:
		void		slotDatabaseChanged();
		void		slotModeChanged(bool state);
};

#endif
