/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

#include <qdom.h>

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <koStore.h>

#include <kexiproject.h>

#include "kexikwmmconnection.h"

KexiKWMMConnection::KexiKWMMConnection(const QString &url)
{
	kdDebug() << "KexiKWMMConnection::KexiKWMMConnection(): " << url << endl;
	m_url = url;
	m_project = new KexiProject();
}

bool
KexiKWMMConnection::load()
{
	KoStore *store = KoStore::createStore(m_url, KoStore::Read);
	store->open("maindoc.xml");

	QDomDocument doc;
	if(doc.setContent(store->device()) && m_project->loadXML(store->device(), doc))
	{
		m_project->completeLoading(store);
		store->close();
		return true;
	}
	else
	{
		KMessageBox::error(0, i18n("Error while reading the document"), i18n("Kexi Mail Merge"));
	}

	return false;
}

KexiKWMMConnection::~KexiKWMMConnection()
{
}
