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

#ifndef KEXIDBCONNECTION_H
#define KEXIDBCONNECTION_H

#include <qobject.h>

class QDomElement;
class KoStore;
class KexiDB;

/**
 * this class aims to provide
 * methods to store/load database settings
 * especially for file based engines
 */

class KexiDBConnection
{
	public:
		/**
		 * constructs an "empty" object for lateron processing
		 */
		KexiDBConnection();

		/**
		 * remote engine connection
		 */
		KexiDBConnection(const QString &engine, const QString &host, const QString &dbname,
		 const QString &user, const QString &pass, const QString &socket=QString::null,
		 const QString &port=QString::null);

		/**
		 * file/dir engine connection
		 */
		KexiDBConnection(const QString &engine, const QString &file);
		~KexiDBConnection();


		/**
		 * connect to a previously set db
		 *
		 * @returns the connection handle or 0 if connection faild
		 */
		KexiDB*	connectDB(KexiDB *parent, KoStore *store=0);

		/**
		 * returns if we are connected to a db
		 */
		bool	connected() { return m_connected; }

		/**
		 * load database-connection infos
		 */
		static KexiDBConnection* loadInfo(QDomElement &e);

		/**
		 * save database-connection infos
		 */
		void	writeInfo(QDomDocument &doc);

		void	flush(KoStore *store);
		void	clean();


		// connection info:
		KexiDB::DBType	dbtype() const { return m_type; }
		QString		engine() const { return m_engine; }
		QString		dbname() const { return m_dbname; }
		QString		host() const { return m_host; }
		QString		user() const { return m_user; }
		QString		pass() const { return m_pass; }
		QString		port() const { return m_port; }
	private:
		bool	m_connected;

		//db-settings
		KexiDB::DBType	m_type;
		QString		m_engine;
		QString		m_dbname;
		QString		m_host;
		QString		m_user;
		QString		m_pass;
		QString		m_socket;
		QString		m_port;

		QString		m_tmpname;
};

#endif
