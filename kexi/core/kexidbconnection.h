/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDBCONNECTION_H
#define KEXIDBCONNECTION_H

#include <qobject.h>
#include <kexiDB/kexidb.h>

class QDomElement;
class QDomDocument;
class KoStore;
class KexiDB;
class KexiDBInterfaceManager;
/**
 * this class aims to provide
 * methods to store/load database settings
 * especially for file based engines
 */

class KEXICORE_EXPORT KexiDBConnection
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
		 const QString &port=QString::null, bool createdb = false);

		/**
		 * file/dir engine connection
		 */
		KexiDBConnection(const QString &engine, const QString &file, bool persistant=false);
		~KexiDBConnection();


		/**
		 * connect to a previously set db
		 *
		 * @returns the connection handle or 0 if connection faild
		 */
		KexiDB*	connectDB(KexiDBInterfaceManager *parent, KoStore *store=0);

		/**
		 * returns if we are connected to a db
		 */
		bool	connected() { return m_connected; }

		/**
		 * load database-connection infos
		 */
		static KexiDBConnection* loadInfo(QDomElement &e);

		void	setEncoding(KexiDB::Encoding e) { m_encoding = e; }
		KexiDB::Encoding encoding() { return m_encoding; }

		/**
		 * save database-connection infos
		 */
		void	writeInfo(KexiDB* destDB,int priority);
		void	writeInfo(QDomDocument &doc);

		void	flush(KoStore *store);
		void	provide(KoStore *store);
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
		QString		m_file;
		bool		m_persistant;
		bool		m_createDb;
		KexiDB::Encoding m_encoding;

		QString		m_tmpname;
		QStringList	m_tmpindex;
};

#endif
