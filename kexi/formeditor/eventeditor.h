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

#ifndef EVENTEDITOR_H
#define EVENTEDITOR_H

#include <qmap.h>

#include "propertybuffer.h"
#include "propertyeditor.h"

/*
struct FakeHandler
{
	QObject		*proxy;
	char		*member;
	QStringList	handlers;
};
*/

class FakeHandler
{
	public:
		FakeHandler() {;}
		virtual ~FakeHandler() {; }

		virtual void	callConnect(QObject *sender, const char *signal, QString function) = 0;

		virtual QStringList	handlers() = 0;
};

typedef QMap<QString, FakeHandler *> Fakes;

class EventEditor : public PropertyEditor
{
	Q_OBJECT

	public:
		EventEditor(QWidget *parent, const char *name=0);
		~EventEditor();

		void	appendFake(const QString &name, FakeHandler *);

		Fakes	fakes() { return m_fakes; }

	public slots:
		void	setObject(QObject *o);
//		void	setReceivers(WidgetList *receivers);

	private:
		Fakes	m_fakes;
};

#endif
