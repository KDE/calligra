/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIDATASOURCEDLG_H
#define KEXIDATASOURCEDLG_H

#include <qdialog.h>

class KexiProject;
class KexiDataSourceComboBox;
class QLineEdit;

class KEXICORE_EXPORT KexiDataSourceDlg : public QDialog
{
	Q_OBJECT

	public:
		KexiDataSourceDlg(KexiProject *p, QWidget *parent=0, const char *name=0);
		~KexiDataSourceDlg();

		QString		source() const;
		QString		name() const;

	private:
		KexiDataSourceComboBox	*m_ds;
		QLineEdit	*m_name;
};

#endif

