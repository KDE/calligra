/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
					  Daniel Molkentin <molkentin@kde.org>

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

#include "kexiproject.h"
#include "kexidoc.h"

#include <kdebug.h>

KexiProject::KexiProject(QObject* parent)
{
}

KexiProject::~KexiProject()
{
}

void
KexiProject::saveProject(const QString& path)
{
}

void
KexiProject::loadProject(const QString& path)
{
}

bool 
KexiProject::initDbConnection(const Credentials &cred)
{
	kdDebug() << "using driver " << cred.driver << endl;
	m_db = QSqlDatabase::addDatabase(cred.driver);
	
	m_db->setDatabaseName(cred.database);
	m_db->setUserName(cred.user);
	m_db->setPassword(cred.password);
	m_db->setHostName(cred.host);
	
	if(!m_db->open())
	{
		kdDebug() << "connection failed: " << m_db->lastError().databaseText() << endl;
		return false;
	}
	else
	{
		return true;
	}
}
/*
void 
KexiProject::()
{
	QString driver = m_doc->getDriver();
	QString host = m_doc->getHost();
	QString name = m_doc->getName();
	QString user = m_doc->getUser();
	QString password = m_doc->getPassword();
	m_actionSave->setEnabled(true);
	m_docLoaded = true;

}
*/

#include "kexiproject.moc"
