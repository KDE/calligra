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

#include <qmetaobject.h>

#include <kdebug.h>
#include <klocale.h>
#include <qheader.h>

#include "eventeditoritem.h"
#include "eventeditor.h"
#include "eventbuffer.h"

EventEditor::EventEditor(QWidget *parent, KFormEditor::EventBuffer *buff, const char *name)
 : PropertyEditor(parent, name)
{
	header()->setLabel(0, i18n("Event"));
	header()->setLabel(1, i18n("Handler"));
}

void
EventEditor::setObject(QObject *o)
{
	reset();

	kdDebug() << "EventEditor::setObject()" << endl;

	char *name;
	kdDebug() << "EventEditor::setObject(): items: " << o->metaObject()->signalNames(true).count() << endl;

	QStrList events(o->metaObject()->signalNames(true));
	for(name = events.first(); name; name = events.next())
	{
		kdDebug() << "EventEditor::setObject(): events: " << name << endl;
//		PropertyEditorItem *i = new PropertyEditorItem(this, name, QVariant::BitArray, QVariant(QString()));
		new EventEditorItem(this, o, name, "", "", 0);
	}
}

void
EventEditor::appendFake(const QString &name, FakeHandler *h)
{
	m_fakes.insert(name, h);
}


EventEditor::~EventEditor()
{
}

#include "eventeditor.moc"
