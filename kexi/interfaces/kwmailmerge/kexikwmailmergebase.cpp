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

#include <qvariant.h>

#include <kdebug.h>
#include <kfiledialog.h>

#include <kexiproject.h>
#include <kexiprojecthandler.h>
#include <kexidataprovider.h>

#include "kexikwmmconnection.h"
#include "kexikwmmconfigdlg.h"
#include "kexikwmailmergebase.h"

KexiKWMailMergeBase::KexiKWMailMergeBase(KInstance *instance, QObject *parent)
 : KWMailMergeDataSource(instance, parent)
{
	m_connection = 0;
	m_no = 0;
}

bool
KexiKWMailMergeBase::showConfigDialog(QWidget *parent, int action)
{
	kdDebug() << "KexiKWMailMergeBase::showConfigDialog() " << action << endl;

	switch(action)
	{
		case KWSLOpen:
		case KWSLCreate:
		{
			QString url = KFileDialog::getOpenFileName(QString::null, "*.kexi", parent);
			m_connection = new KexiKWMMConnection(url);
			return m_connection->load();
//			KexiKWMMConfigDlg *dlg = new KexiKWMMConfigDlg(parent);
//			return dlg->exec();
		}
		case KWSLEdit:
		{
			KexiKWMMConfigDlg *dlg = new KexiKWMMConfigDlg(parent, m_connection);
			bool accepted = dlg->exec();
			if(accepted)
			{
				m_mime = dlg->mime();
				m_id = dlg->id();
				sampleRecord = dlg->fields();
				initDB(true);
				initDB(false);
			}

			return accepted;
		}
		default:
			return false;

/*	KWSLMergePreview
	KWSLMergeDocument */

	}
}

bool
KexiKWMailMergeBase::openDatabase()
{
	kdDebug() << "KexiKWMailMergeBase::openDatabase()" << endl;

	return initDB(false);
}

bool
KexiKWMailMergeBase::initDB(bool count)
{
	KexiProjectHandler *h = m_connection->project()->handlerForMime(m_mime);
	KexiDataProvider *prov = KEXIDATAPROVIDER(h);
	if(!prov)
		return false;

	KexiDataProvider::Parameters p;
	m_records = prov->records(0, m_id, p);
	if(!m_records)
		return false;

	int i=0;
	while(m_records->next())
	{
		i++;
	}

	m_no = i;

	kdDebug() << "*** " << i << " ***" << endl;

//	delete m_records;

//	m_records = prov->records(0, m_id, p);

	return true;
}

void
KexiKWMailMergeBase::refresh(bool)
{
	kdDebug() << "KexiKWMailMergeBase::refresh()" << endl;
}

QString
KexiKWMailMergeBase::getValue(const QString &field, int rec) const
{
	kdDebug() << "KexiKWMailMergeBase::getValue('" << field << "', " << rec << ")" << endl;
	if(rec < 0)
		return field;

	m_records->gotoRecord(uint(rec));

	return m_records->value(field).toString();
}

int
KexiKWMailMergeBase::getNumRecords() const
{
//	openDatabase();
	return m_no;
}


KexiKWMailMergeBase::~KexiKWMailMergeBase()
{
}

extern "C"
{
	KWMailMergeDataSource *create_kwmailmerge_kexi(KInstance *i, QObject *p)
	{
		return new KexiKWMailMergeBase(i, p);
	}
}

#include "kexikwmailmergebase.moc"
