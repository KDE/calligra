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

#include <qlayout.h>
#include <qlabel.h>

#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>

#include "kexiDB/kexidb.h"

#include "kexiapplication.h"
#include "kexiproject.h"
#include "kexitabbrowser.h"
#include "kexicreateprojectpagedb.h"

KexiCreateProjectPageDB::KexiCreateProjectPageDB(KexiCreateProject *parent, QPixmap *wpic, const char *name)
 : KexiCreateProjectPage(parent, wpic, name)
{
	//cool picture ;)
	QLabel *lPic = new QLabel("", this);
	lPic->setPixmap(*wpic);

	m_databases = new KListView(this);
	m_databases->addColumn(i18n("Select Database"));
	connect(m_databases, SIGNAL(selectionChanged()), this, SLOT(slotDatabaseChanged()));

	//layout once again...
	QGridLayout *g = new QGridLayout(this);
	g->addWidget(lPic,		0,	0);
	g->addWidget(m_databases,	0,	1);
}

void
KexiCreateProjectPageDB::connectHost(QString driver, QString host, QString user, QString password)
{
	m_cred.driver = driver;
	m_cred.host = host;
	m_cred.user = user;
	m_cred.password = password;

	if(kexi->project()->initHostConnection(m_cred))
	{
		m_databases->clear();

		KexiDB *db = kexi->project()->db();
		QStringList databases = db->databases();
		for(QStringList::Iterator it = databases.begin(); it != databases.end(); it++)
		{
			new KListViewItem(m_databases, (*it));
		}
	}
}

void
KexiCreateProjectPageDB::connectDB()
{
	m_cred.database = data("database").toString();
	if(kexi->project()->initDbConnection(m_cred))
	{
		kexi->mainWindow()->browser()->generateView();
	}
}

void
KexiCreateProjectPageDB::slotDatabaseChanged()
{
	if(!m_databases->currentItem())
		return;

	setProperty("database", QVariant(m_databases->currentItem()->text(0)));
	setProperty("finish", QVariant(true));
}

KexiCreateProjectPageDB::~KexiCreateProjectPageDB()
{
}

#include "kexicreateprojectpagedb.moc"
