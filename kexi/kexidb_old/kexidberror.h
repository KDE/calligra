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

#ifndef KEXIDBERROR_H
#define KEXIDBERROR_H

#include <qmap.h>

class QWidget;
class QString;

typedef QMap<int, QString> ErrorList;

class KexiDBError
{
	public:
		KexiDBError(int kexiErrno, QString text=QString::null);
		~KexiDBError();

		void	toUser(QWidget *parent);
		int	kexiErrnoFunction();
		QString	message();

	private:
		int	m_errno;
		QString	m_text;
};

#endif
