/* This file is part of the KDE project
Copyright (C) 2002   Peter Simonsson <psn@linux.se>

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

#ifndef KEXICREATEPROJECTPAGEAUTH_H
#define KEXICREATEPROJECTPAGEAUTH_H

#include "kexicreateprojectpage.h"

class KLineEdit;
class QCheckBox;

class KexiCreateProjectPageAuth : public KexiCreateProjectPage
{
	Q_OBJECT

	public:
		KexiCreateProjectPageAuth(KexiCreateProject *parent, QPixmap *wpic, const char *name=0);

	protected:
		KLineEdit* m_userEdit;
		KLineEdit* m_passwordEdit;
		QCheckBox* m_savePassChk;
	
	protected slots:
		void slotUserChanged(const QString &);
		void slotPassChanged(const QString &);
};

#endif
