/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

//#include "formeditor/widgetwatcher.h"

#include <qobjectlist.h>
#include <qvariant.h>

#include <kdebug.h>

#include "kexidbwidgetcontainer.h"

KexiDBWidgetContainer::KexiDBWidgetContainer(QWidget *parent, const char *name, QString identifier)
 : KFormEditor::WidgetContainer(parent, name, identifier)
{
	m_ww = 0;
	m_rec = 0;
}

QString
KexiDBWidgetContainer::dataSource() const
{
	return m_dataSource;
}

void
KexiDBWidgetContainer::setDataSource(QString source)
{
	m_dataSource = source;
}

void
KexiDBWidgetContainer::setRecord(KexiDBRecordSet *rec)
{
	if(!rec)
		return;

	m_rec = rec;
	next();
}

void
KexiDBWidgetContainer::next()
{
	if(!m_rec)
		return;

	m_rec->next();
	setupWidgets();
}

void
KexiDBWidgetContainer::prev()
{
	m_rec->prev();
	setupWidgets();
}

void
KexiDBWidgetContainer::setupWidgets()
{
	QObjectListIt it(*children());
	for(QObject *o; (o=it.current()) != 0; ++it)
	{
		QString ds = o->property("dataSource").toString();
		if(ds != QString::null || ds != "")
		{
			o->setProperty("dbdata", m_rec->value(ds).toString());
		}
	}
}

KexiDBWidgetContainer::~KexiDBWidgetContainer()
{
}

#include "kexidbwidgetcontainer.moc"

