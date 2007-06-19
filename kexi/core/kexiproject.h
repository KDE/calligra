/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIPROJECT_H
#define KEXIPROJECT_H

#include <qobject.h>
#include <qpointer.h>

#include <kexiutils/tristate.h>
#include <kexidb/object.h>
#include "kexiprojectdata.h"
#include "kexipartitem.h"
#include "kexi.h"

/*! KexiProject implementation version. 
 It is altered after every change: 
 - major number is increased after KexiProject storage format change, 
 - minor is increased after adding binary-incompatible change.
 Use KexiProject::versionMajor() and KexiProject::versionMinor() to get real project's version.
*/

#define KEXIPROJECT_VERSION_MAJOR 1
#define KEXIPROJECT_VERSION_MINOR 0

namespace KexiDB
{
	class Connection;
	class Parser;
}

namespace KexiPart
{
	class Part;
	class Info;
}

class KexiMainWindow;
class KexiWindow;

/**
 * @short A project's main controller. 
 * It also contains connection data,
 * current file state, etc.
 */
class KEXICORE_EXPORT KexiProject : public QObject, public KexiDB::Object
{
	Q_OBJECT

	public:
		/*! Constructor 1. Creates a new object using \a pdata. 
		 \a pdata which will be then owned by KexiProject object.
		 \a handler can be provided to receive error messages during 
		 entire KexiProject object's lifetime. */
		KexiProject(KexiProjectData* pdata, KexiDB::MessageHandler* handler = 0);

		/*! Constructor 2. Like above but sets predefined connections \a conn. 
		 The connection should be created using the same connection data 
		 as pdata->connectionData(). The connection will become owned by created KexiProject
		 object, so do not destroy it. */
		KexiProject(KexiProjectData *pdata, KexiDB::MessageHandler* handler, 
			KexiDB::Connection* conn);

//		KexiProject(KexiDB::ConnectionData *cdata);

		~KexiProject();

		/*! \return major version of KexiProject object. 
		 This information is retrieved from database when existing project is opened. */
		int versionMajor() const;

		/*! \return minor version of KexiProject object. 
		 @see versionMajor() */
		int versionMinor() const;

		/*! Opens existing project using project data. 
		 \return true on success */
		tristate open();

		/*! Like open(). 
		 \return true on success.
		 Additional \a incompatibleWithKexi, is set to false on failure when
		 connection for the project was successfully started bu the project 
		 is probably not compatible with Kexi - no valid "kexidb_major_ver" 
		 value in "kexi__db" table. 
		 This is often the case for native server-based databases. 
		 If so, Kexi application can propose importing the database
		 or linking it to parent project (the latter isn't yet implemented).
		 For other types of errors the variable is set to true. */
		tristate open(bool &incompatibleWithKexi);

		/*! Creates new, empty project using project data.
		 If \a forceOverwrite is true, existing database project is silently overwritten.
		 Connection is created (accessible then with KexiProject::dbConnection()).

		 Since KexiProject inherits KexiDB::Object, it is possible to get error message 
		 and other information on error.

		 \return true on success, false on failure, and cancelled when database exists 
		 but \a forceOverwrite is false. */
		tristate create(bool forceOverwrite = false);

		/*! \return true if there was error during last operation on the object. */
		bool error() const { return KexiDB::Object::error(); }

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
		KexiPart::ItemDict* itemsForMimeType(const QString &mimeType);

		/**
		 * Puts a list of items of a type \a i in this project into \a list.
		 * You can then sort this list using ItemList::sort().
		 */
		void getSortedItems(KexiPart::ItemList& list, KexiPart::Info *i);

		/**
		 * Puts a sorted list of items of a type \a mimeType in this project into \a list.
		 * You can then sort this list using ItemList::sort().
		 */
		void getSortedItemsForMimeType(KexiPart::ItemList& list, const QString &mimeType);

		/**
		 * @return item of type \a mime and name \a name
		 */
		KexiPart::Item* itemForMimeType(const QString &mimeType, const QString &name);

		/**
		 * @return item of type \a i and name \a name
		 */
		KexiPart::Item* item(KexiPart::Info *i, const QString &name);

		/**
		 * @return item for \a identifier
		 */
		KexiPart::Item* item(int identifier);

		/**
		 * @return the database connection associated with this project
		 */
		KexiDB::Connection *dbConnection() const;

		/**
		 * @return the project's data
		 */
		KexiProjectData *data() const;

		/*! Opens object pointed by \a item in a view \a viewMode.
		 \a staticObjectArgs can be passed for static object 
		 (only works when part for this item is of type KexiPart::StaticPart) */
		KexiWindow* openObject(KexiPart::Item& item, 
			Kexi::ViewMode viewMode = Kexi::DataViewMode, QMap<QString,QString>* staticObjectArgs = 0);

		//! For convenience
		KexiWindow* openObject(const QString &mimeType, 
			const QString& name, Kexi::ViewMode viewMode = Kexi::DataViewMode);

		/*! Remove a part instance pointed by \a item.
		 \return true on success. */
		bool removeObject(KexiPart::Item& item);

		/*! Renames a part instance pointed by \a item to a new name \a newName.
		 \return true on success. */
		bool renameObject(KexiPart::Item& item, const QString& newName);

		/*! Creates part item for given part \a info. 
		 Newly item will not be saved to the backend but stored in memory only
		 (owned by project), and marked as "neverSaved" (see KexiPart::Item::neverSaved()).
		 The item will have assigned a new unique caption like e.g. "Table15",
		 and unique name like "table15", but no specific identifier 
		 (because id will be assigned on creation at the backend side).

		 If \a suggestedCaption is not empty, it will be set as a caption 
		 (with number suffix, to avoid duplicated, e.g. "employees7" 
		 for "employees" sugested name). Name will be then built based 
		 on this caption using KexiUtils::string2Identifier().

		 This method is used before creating new object.
		 \return newly created part item or NULL on any error. */
		KexiPart::Item* createPartItem(KexiPart::Info *info, 
			const QString& suggestedCaption = QString() );

		//! Added for convenience.
		KexiPart::Item* createPartItem(KexiPart::Part *part, 
			const QString& suggestedCaption = QString());

		/*! Adds item \a item after it is succesfully stored as an instance of part
		 pointed by \a info. Also clears 'neverSaved' flag if \a item.
		 Used by KexiWindow::storeNewData(). 
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
		bool createObject(KexiWindow *window);
#endif

		KexiDB::Parser* sqlParser();

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

		/*! @see KexiDB::Connection::setQuerySchemaObsolete( const QString& queryName ) */
//		void setQuerySchemaObsolete( const QString& queryName );

//		/** used to emit objectCreated() signal */
//		void emitObjectCreated(const QCString &mime, const QCString& name) { emit objectCreated(mime, name); }
//		void emitTableCreated(KexiDB::TableSchema& schema) { emit tableCreated(schema); }

		/*! Generates ID for private "document" like Relations window.
		 Private IDs are negative numbers (while ID regular part instance's IDs are >0)
		 Private means that the object is not stored as-is in the project but is somewhat
		 generated and in most cases there is at most one unique instance document of such type (part).
		 To generate this ID, just app-wide internal counter is used. */
		virtual int generatePrivateID();

	protected:
		/*! Creates connection using project data. 
		 The connection will be readonly if data()->isReadOnly().
		 \return true on success, otherwise false and appropriate error is set. */
		bool createConnection();
		
		bool closeConnection();

		bool initProject();

		//! Used in open() and open(bool&).
		tristate openInternal(bool *incompatibleWithKexi);

		/*! Kexi itself can define a number of internal database objects (mostly data structures), 
		 usually tables for it's own purposes.
		 Even while at KexiDB library level, such "system" tables, like "kexi__objects", "kexi__objectdata"
		 are created automatically on database project creation, this is not enough: there are objects
		 needed specifically for Kexi but not for other applications utilizing KexiDB library.
		 Example table created here for now is "kexi__blobs". 

		 This method is called on create() and open(): creates necessary objects 
		 if they are not yet existing. This especially allows to create to create these objects
		 (on open) within a project made with previous Kexi version not supporting 
		 all currently defined structurtes. We're trying to be here as much backward compatible as possible.
		 For this purpose, here's the complete list of currently created objects:
		 - "kexi__blobs" - a table containing BLOBs data that can be accessed globally at Kexi projects
		   from within any database-aware view (table views, forms, reports, etc.)
		 
		 @param insideTransaction Embed entire creation process inside a transaction

		 \return true on successful object's creation. Objects are created only once, they are not overwritten.
		*/
		bool createInternalStructures(bool insideTransaction);

		/*! \return Kexi part for \a item. */
		KexiPart::Part *findPartFor(KexiPart::Item& item);

	signals:
		/** signal emitted on error */
		void error(const QString &title, KexiDB::Object *obj);

		/** signal emitted on error (not KexiDB-related) */
		void error(const QString &msg, const QString &desc);

		/** New \a item has been stored. */
		void newItemStored(KexiPart::Item& item);

		/** instance pointed by \a item is removed */
		void itemRemoved(const KexiPart::Item &item);

		/** instance pointed by \a item is renamed */
		void itemRenamed(const KexiPart::Item &item, const QString& oldName);

//		/** new table \a schema created */
//		void tableCreated(KexiDB::TableSchema& schema);
//		/** New object of mimetype \a mime and \a name has been created. */
//		void objectCreated(const QCString &mime, const QCString& name);

	protected:
		/*! Checks whether the project's connection is read-only. 
		 If so, error message is set and false is returned. */
		bool checkWritable();

		class Private;
		Private *d;

		friend class KexiMainWindow;
};


#endif
