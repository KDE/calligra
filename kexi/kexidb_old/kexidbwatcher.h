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

#ifndef KEXIDBWATCHER_H
#define KEXIDBWATCHER_H

#include <qvariant.h>
#include <qobject.h>

class KexiDBWatcher : public QObject
{
	Q_OBJECT

	public:
		KexiDBWatcher(QObject *parent, const char *name);
		~KexiDBWatcher();

		void	registerClient(QObject *parent, const QString &user);
		void	unregisterClient(QObject *parent);

		bool	lock(QObject *sender, const QString &table, const QString &field, uint record);
		bool	unlock(QObject *sender, const QString &table, const QString &field, uint record);
		bool	update(QObject *sender, const QString &table, const QString &field, uint record, QVariant value);
		bool	remove(QObject *sender, const QString &table, uint record);

	signals:
		void	locked(QObject *sender, const QString &table, const QString &field, uint record);
		void	unlocked(QObject *sender, const QString &table, const QString &field, uint record);
		void	updated(QObject *sender, const QString &table, const QString &field, uint record, QVariant &value);
		void	removed(QObject *sender, const QString &table, uint record);
};

#endif
