/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXIKWMAILMERGEBASE_H
#define KEXIKWMAILMERGEBASE_H

#include "kword/mailmerge_interface.h"
#include "kexiDB/kexidbrecordset.h"

class KexiKWMMConnection;

class KexiKWMailMergeBase : public KWMailMergeDataSource
{
	Q_OBJECT
	K_DCOP

	public:
		KexiKWMailMergeBase(KInstance *instance, QObject *parent);
		~KexiKWMailMergeBase();

		virtual bool	showConfigDialog(QWidget *parent, int action);

		virtual void	save(QDomDocument &, QDomElement &) { ; }
		virtual void	load(QDomElement &) { ;}

		virtual void	refresh(bool);

		virtual QString	getValue(const QString &, int) const;
		virtual int	getNumRecords() const;

		bool	initDB(bool count);

	k_dcop:
		bool	openDatabase();

	private:
		KexiKWMMConnection	*m_connection;
		QString			m_mime;
		QString			m_id;
		KexiDBRecordSet		*m_records;

		int			m_no;
};

#endif
