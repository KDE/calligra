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

#include <kdebug.h>

#include "kexidbwatcher.h"

KexiDBWatcher::KexiDBWatcher(QObject *parent, const char *name)
 : QObject(parent, name)
{
}

void
KexiDBWatcher::registerClient(QObject *parent, const QString &user)
{
	//register a handler and care for user
}

void
KexiDBWatcher::unregisterClient(QObject *parent)
{
}


bool
KexiDBWatcher::lock(QObject *sender, const QString &table, const QString &field, uint record)
{
	kdDebug() << "KexiDBWatcher::lock" << endl;
	emit locked(sender, table, field, record);
	return true;
}

bool
KexiDBWatcher::remove(QObject *sender, const QString &table, uint record)
{
	kdDebug() << "KexiDBWatcher::unlock" << endl;

	emit removed(sender, table, record);
	return true;
}

bool
KexiDBWatcher::unlock(QObject *sender, const QString &table, const QString &field, uint record)
{
	kdDebug() << "KexiDBWatcher::unlock" << endl;
	emit unlocked(sender, table, field, record);
	return true;
}

bool
KexiDBWatcher::update(QObject *sender, const QString &table, const QString &field, uint record, QVariant value)
{
	kdDebug() << "KexiDBWatcher::update(" << table << "," << field << "," << record << "," <<
	value.toString() << endl;
	emit updated(sender, table, field, record, value);
	return true;
}

KexiDBWatcher::~KexiDBWatcher()
{
}

#include "kexidbwatcher.moc"
