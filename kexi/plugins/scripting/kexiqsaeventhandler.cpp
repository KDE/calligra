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

#include <klocale.h>
#include <kdebug.h>

#include <qsinterpreter.h>

#include "kexiscripthandler.h"
#include "kexiqsahost.h"
#include "kexiqsaeventhandler.h"

KexiQSAEventHandler::KexiQSAEventHandler(KexiScriptHandler *parent, KexiQSAHost *host)
 : KexiEventHandler(parent)
{
	m_host = host;
	m_fake = new KexiQSAFakeHandler(this);

//	kdDebug() << "clicked()" << endl;
//	kdDebug() << SLOT(clicked()) << endl;
}

QString
KexiQSAEventHandler::name()
{
	return i18n("Script");
}

QStringList
KexiQSAEventHandler::functions()
{
	kdDebug() << "KexiQSAEventHandler::functions()" << endl;

	QStringList l(m_host->globalFunctions());
	for(QStringList::Iterator it = l.begin(); it != l.end(); ++it)
	{
		kdDebug() << "KexiQSAEventHandler::functions(): fn: " << *it << endl;
	}

	return QStringList(m_host->globalFunctions());
}

void
KexiQSAEventHandler::callConnect(QObject *sender, const char *signal, QString function)
{
	kdDebug() << "KexiQSAEventHandler::callConnect(): sender=" << sender << endl;
	kdDebug() << "KexiQSAEventHandler::callConnect(): signal=" << signal << endl;
	kdDebug() << "KexiQSAEventHandler::callConnect(): function=" << function << endl;

	QCString s;
	s.setNum(2);
	s += signal;

	kdDebug() << "KexiQSAEventHandler::callConnect(): manual=" << SIGNAL(clicked()) << endl;
	kdDebug() << "KexiQSAEventHandler::callConnect(): generated=" << s << endl;


	m_host->interpreter()->addSignalHandler(sender, s, function.latin1());
}

void
KexiQSAEventHandler::provideObject(QObject *o)
{
	m_host->interpreter()->addObject(o);
}

void
KexiQSAEventHandler::resetObject(QObject *o)
{
	m_host->interpreter()->removeObject(o);
	m_host->interpreter()->addObject(o);
}

KexiQSAEventHandler::~KexiQSAEventHandler()
{
}

// formedit's fakehandler :)

KexiQSAFakeHandler::KexiQSAFakeHandler(KexiQSAEventHandler *ev)
{
	m_handler = ev;
}

KexiQSAFakeHandler::~KexiQSAFakeHandler()
{
}

QStringList
KexiQSAFakeHandler::handlers()
{
	return m_handler->functions();
}

void
KexiQSAFakeHandler::callConnect(QObject *sender, const char *signal, QString function)
{
	kdDebug() << "KexiQSAFakeHandler::callConnect()" << endl;
	m_handler->callConnect(sender, signal, function);
}

#include "kexiqsaeventhandler.moc"
