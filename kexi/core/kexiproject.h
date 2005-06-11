/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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
#include <qptrdict.h>
#include <qguardedptr.h>

#include <kexidb/object.h>
#include <utils/tristate.h>
#include "kexiprojectdata.h"
#include "kexipartitem.h"
#include "kexi.h"

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
 * This class represents a project's controller. It also contains connection data,
 * current file state, etc.
 */
class KEXICORE_EXPORT KexiProject : public QObject, protected KexiDB::Object
{
	Q_OBJECT

	public:
		/*! Constructor 1. Creates a new object using \a pdata, which will be owned. 
		 \a handler can be provided to receive error messages. */
		KexiProject(KexiProjectData* pdata, KexiDB::MessageHandler* handler = 0);
//		/*! Constructor 1. Creates a new object using \a pdata, which will be owned. */
//		KexiProject(KexiDB::ConnectionData *cdata);

		~KexiProject();

		//! Opens existing project using project data.
		bool open();

		/*! Creates new, empty project using project data.
		 Warning: If database exist, it is silently overwritten!
		*/
		tristate create(bool forceOverwrite = false);
		
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
		KexiDB::Connection *dbConnection() const { return m_connection; }

		KexiProjectData *data() const { return m_data; }

		KexiDialogBase* openObject(KexiMainWindow *wnd, KexiPart::Item& item, int viewMode = Kexi::DataViewMode);

		//! For convenience
		KexiDialogBase* openObject(KexiMainWindow *wnd, const QCString &mime, const QString& name, int viewMode = Kexi::DataViewMode);

		/*! Remove a part instance pointed by \a item.
		 \return true on success. */
		bool removeObject(KexiMainWindow *wnd, KexiPart::Item& item);

		/*! Renames a part instance pointed by \a item to a new name \a newName.
		 \return true on success. */
		bool renameObject(KexiMainWindow *wnd, KexiPart::Item& item, const QString& newName);

		/*! Creates part item for given part \a info. 
		 Newly item will not be saved to the backend but stored in memory only
		 (owned by project), and marked as "neverSaved" (see KexiPart::Item::neverSaved()).
		 The item will have assigned a new unique caption like e.g. "Table15",
		 and unique name like "table15", but no specific identifier 
		 (because id will be assigned on creation at the backend side).
		 If \a suggestedName is not empty, it will be set as a name 
		 (with number suffix, to avoid duplicated, e.g. "employees7" 
		 for "employees" sugested name)

		 This method is used before creating new object.
		 \return newly created part item or NULL on any error. */
		KexiPart::Item* createPartItem(KexiPart::Info *info, 
			const QString& suggestedName = QString::null );

		/*! Adds item \a item after it is succesfully stored as an instance of part
		 pointed by \a info. Also clears 'neverSaved' flag if \a item.
		 Used by KexiDialogBase::storeNewData(). 
		 @internal */
		void addStoredItem(KexiPart::Info *info, KexiPart::Item *item);

		/*! removes \a item from internal dictionaries. The item is destroyed 
		 after successful removal. 
		 Used to delete an unstored part item previusly created with createPartItem(). */
		void deleteUnstoredItem(KexiPart::Item *item);

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

		/*! \return true if the project is started in final mode */
		bool final() { return m_final; }

		/*! Shows dialog for creating new blank project,
		 ans creates one. Dialog is not shown if option for automatic creation 
		 is checked or Kexi::startupHandler().projectData() was provided from command line.
		 \a cancelled is set to true if creation has been cancelled (e.g. user answered 
		 no when asked for database overwriting, etc.
		 \return true if database was created, false on error or when cancel was pressed */
		static KexiProject* createBlankProject(bool &cancelled, KexiProjectData* data,
			KexiDB::MessageHandler* handler = 0);

		/*! Drops project described by \a data. \return true on success. 
		 Use with care: Any KexiProject objects allocated for this project will become invalid! */
		static tristate dropProject(KexiProjectData* data, 
			KexiDB::MessageHandler* handler, bool dontAsk = false);

		/** new table \a schema created */
		void emitTableCreated(KexiDB::TableSchema& schema) { emit tableCreated(schema); }

	signals:
		void newItemStored(KexiPart::Item *item);

	protected:
//		bool			openConnection(KexiProjectConnectionData *connection);

		/*! Creates connection using project data.
		 \return true on success, otherwise false and appropriate error is set. */
		bool createConnection();
		
		void closeConnection();

		bool initProject();

/* moved to Object
		virtual void setError(int code = ERR_OTHER, const QString &msg = QString::null );
		virtual void setError( const QString &msg );
		virtual void setError( KexiDB::Object *obj );

		//! setting not KexiDB-related erorr
		void setError(const QString &msg, const QString &desc);
*/
		/*! Finds part for \a item. Sets error retrieved from Part Manager 
		 if the part cannot be found. */
		KexiPart::Part *findPartFor(KexiPart::Item& item);

//		void clearMsg();

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

		/** instance pointed by \a item is renamed */
		void itemRenamed(const KexiPart::Item &item);

		/** new table \a schema created */
		void tableCreated(KexiDB::TableSchema& schema);

	protected:
		QGuardedPtr<KexiDB::Connection> m_connection;
		QGuardedPtr<KexiProjectData> m_data;
		
		QString m_error_title;

		//! a cache for item() method, indexed by project part's ids
		QIntDict<KexiPart::ItemDict> m_itemDictsCache;

		QPtrDict<KexiPart::Item> m_unstoredItems;
		int m_tempPartItemID_Counter; //!< helper for getting unique 
		                              //!< temporary identifiers for unstored items

		KexiDB::Parser* m_sqlParser;
		bool m_final;

		friend class KexiMainWindowImpl;
};


#endif

