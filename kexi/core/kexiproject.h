/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIPROJECT_H
#define KEXIPROJECT_H

#include <qobject.h>
#include <qintdict.h>
#include <qguardedptr.h>

#include "kexiprojectdata.h"
#include "kexipartitem.h"
#include "kexi.h"

#include <kexidb/object.h>

namespace KexiDB
{
	class DriverManager;
	class Driver;
	class Connection;
	class Parser;
}

namespace KexiPart
{
//	class Manager;
	class Info;
}

class KexiMainWindow;
class KexiDialogBase;

/**
 * this class represents a project it contains data about connections, 
 * current file state etc..
 */
class KEXICORE_EXPORT KexiProject : public QObject, public KexiDB::Object
{
	Q_OBJECT

	public:
		/*! Constructor 1. Creates a new object using \a pdata, which will be owned. */
		KexiProject(KexiProjectData* pdata);
//		/*! Constructor 1. Creates a new object using \a pdata, which will be owned. */
//		KexiProject(KexiDB::ConnectionData *cdata);
		~KexiProject();

		//! Opens existing project using project data.
		bool open();

		//! Creates new, empty project using project data.
		bool create();
		
		/*! Opens project using created connection
		 \return true on success, otherwise false and appropriate error is set. */
//		bool		open(KexiDB::Connection* conn);

		/**
		 * opens a project/xml-connection
		 * @return true on success
		 */
	//	bool		open(const QString &doc);

		/**
		 * @return a error wich may have occured at actions like open/openConnection or QString::null if none
		 */
//		const QString		error() { return m_error; }

		/**
		 * @return the part manager
		 */
//		KexiPart::Manager	*partManager() { return m_partManager; }

		/**
		 * @return true if a we are connected to a database
		 */
		bool isConnected();

		/**
		 * @return all items of a type \a i in this project
		 */
		KexiPart::ItemDict* items(KexiPart::Info *i);

		/**
		 * @return all items of a type \a mime in this project
		 * It is a convenience function.
		 */
		KexiPart::ItemDict* items(const QCString &mime);

		KexiPart::Item* item(const QCString &mime, const QString &name);
		//! convenience function
		KexiPart::Item* item(KexiPart::Info *i, const QString &name);

		/**
		 * @return the database connection assosiated with this project
		 */
		KexiDB::Connection	*dbConnection() const { return m_connection; }

		KexiProjectData *data() const { return m_data; }

		KexiDialogBase* openObject(KexiMainWindow *wnd, KexiPart::Item& item, int viewMode = Kexi::DataViewMode);

		//! For convenience
		KexiDialogBase* openObject(KexiMainWindow *wnd, const QCString &mime, const QString& name, int viewMode = Kexi::DataViewMode);

		/*! Remove part instance pointed by \a item.
		 \return true on success.
		*/
		bool removeObject(KexiMainWindow *wnd, KexiPart::Item& item);

		/*! Creates part item for given part \a info. 
		 Newly item will not be saved to the backend but stored in memory only
		 (owned by project), and marked as "neverSaved" (see KexiPart::Item::neverSaved()).
		 The item will have assigned new unique name like e.g. "Table15",
		 and unique name like "table15", but no specific identifier 
		 (because id will be assigned on creation at the backend side).

		 This method is used before creating new object.
		 \return newly created part item or NULL on any error. */
		KexiPart::Item* createPartItem(KexiPart::Info *info);

#if 0 //remove?
		/*! Creates object using data provided by \a dlg dialog. 
		 Dialog's \a item (KexiDialog::partItem()) must not be stored 
		 (KexiPart::Item::neverStored()==false) and created 
		 by KexiProject::createPartItem().
		 Identifier of the item will be updated to a final value 
		 (stored in the backend), because previously there was temporary one set.
		 \return true for successfully created object or false on any error. */
		bool createObject(KexiDialogBase *dlg);
#endif

		KexiDB::Parser* sqlParser();

		/*! \return true if project is started in final mode */
		bool final() { return m_final; }

	protected:
//		bool			openConnection(KexiProjectConnectionData *connection);

		/*! Creates connection using project data.
		 \return true on success, otherwise false and appropriate error is set. */
		bool createConnection();
		
		void closeConnection();

		bool initProject();

		//! reimplementation
		virtual void setError(int code = ERR_OTHER, const QString &msg = QString::null );
		virtual void setError( const QString &msg );
		virtual void setError( KexiDB::Object *obj );

		//! setting not KexiDB-related erorr
		void setError(const QString &msg, const QString &desc);
				
	signals:
		/**
		 * this signal gets emmited after succesfully connected to a db
		 */
//		void			dbAvailable();

		/** signal emitted on error */
		void error(const QString &title, KexiDB::Object *obj);

		/** signal emitted on error (not KexiDB-related) */
		void error(const QString &msg, const QString &desc);

		/** instance pointed by \a item is removed */
		void itemRemoved(const KexiPart::Item &item);


	private:
		friend class KexiMainWindowImpl;
	//		KexiDB::DriverManager		*m_drvManager;
		KexiDB::Connection		*m_connection;
		QGuardedPtr<KexiProjectData> m_data;
//		KexiDB::ConnectionData *m_conn_data_to_use; //!< 
		
		QString m_error_title;

		//! a cache for item() method, indexed by project part's ids
		QIntDict<KexiPart::ItemDict> m_itemDictsCache;

		QAsciiDict<KexiPart::Item> m_unstoredItems;
		int m_tempPartItemID_Counter; //!< helper for getting unique 
		                              //!< temporary identifiers for unstored items

		KexiDB::Parser* m_sqlParser;
		bool m_final;
//		KexiProjectConnectionData	*m_connData;
//js		KexiPart::Manager		*m_partManager;
//		QString				m_error;
};


#endif

