/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXIDBERROR_H
#define KEXIDBERROR_H

#include <qmap.h>

class QWidget;
class QString;

typedef QMap<int, QString> ErrorList;

class KexiDBError
{
	public:
		KexiDBError();
		KexiDBError(const KexiDBError &old);
		KexiDBError(int kexiErrno, QString text=QString::null);
		void setup(int KexiErrno, QString text=QString::null);
		void clear();
		~KexiDBError();

		void	toUser(QWidget *parent);
		int	kexiErrnoFunction();
		QString	message();

	private:
		int	m_errno;
		QString	m_text;
};

#endif
