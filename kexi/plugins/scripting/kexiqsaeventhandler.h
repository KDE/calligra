/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXIQSAEVENTHANDLER_H
#define KEXIQSAEVENTHANDLER_H

#include "kexieventhandler.h"
#include "eventeditor.h"

class KexiScriptHandler;
class KexiQSAHost;
class KexiQSAEventHandler;

class KexiQSAFakeHandler : public FakeHandler
{
	public:
		KexiQSAFakeHandler(KexiQSAEventHandler *ev);
		~KexiQSAFakeHandler();

		virtual void callConnect(QObject *sender, const char *signal, QString function);
		virtual QStringList handlers();

	private:
		KexiQSAEventHandler *m_handler;
};

class KexiQSAEventHandler : public KexiEventHandler
{
	Q_OBJECT

	public:
		KexiQSAEventHandler(KexiScriptHandler *parent, KexiQSAHost *host);
		~KexiQSAEventHandler();

		virtual QString name();
		virtual QStringList functions();

		virtual FakeHandler *formHandler() { return m_fake; }

		virtual void provideObject(QObject *);

		void callConnect(QObject *sender, const char *signal, QString function);

	private:
		KexiQSAHost	*m_host;
		KexiQSAFakeHandler *m_fake;
};

#endif
