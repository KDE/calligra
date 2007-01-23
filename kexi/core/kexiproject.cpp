/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include <qfile.h>
#include <qapplication.h>
#include <qdom.h>

#include <kmimetype.h>
#include <kdebug.h>
#include <klocale.h>

#include <kexiutils/identifier.h>

#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/driver.h>
#include <kexidb/drivermanager.h>
#include <kexidb/utils.h>
#include <kexidb/parser/parser.h>
#include <kexidb/msghandler.h>
#include <kexidb/dbproperties.h>
#include <kexiutils/utils.h>

#include "kexiproject.h"
#include "kexipartmanager.h"
#include "kexipartitem.h"
#include "kexipartinfo.h"
#include "kexipart.h"
#include "kexidialogbase.h"
#include "kexi.h"
#include "keximainwindow.h"
#include "kexiblobbuffer.h"
#include "kexiguimsghandler.h"

#include <assert.h>

class KexiProject::Private
{
	public:
		Private()
		 : data(0)
		 , itemDictsCache(199)
		 , unstoredItems(199)
		 , tempPartItemID_Counter(-1)
		 , sqlParser(0)
		 , versionMajor(0)
		 , versionMinor(0)
		 , final(false)
		{
			itemDictsCache.setAutoDelete(true);
			unstoredItems.setAutoDelete(true);
		}
		~Private() {
			delete data;
			data=0;
			delete sqlParser;
		}

		QGuardedPtr<KexiDB::Connection> connection;
		QGuardedPtr<KexiProjectData> data;
		
		QString error_title;

		//! a cache for item() method, indexed by project part's ids
		QIntDict<KexiPart::ItemDict> itemDictsCache;

		QPtrDict<KexiPart::Item> unstoredItems;
		int tempPartItemID_Counter; //!< helper for getting unique 
		                              //!< temporary identifiers for unstored items
		KexiDB::Parser* sqlParser;

		int versionMajor;
		int versionMinor;
		bool final : 1;
};

//---------------------------

/*
 Helper for setting temporary error title. 
class KexiProject::ErrorTitle
{
	public:
	ErrorTitle(KexiProject* p, const QString& msg = QString::null)
		: prj(p)
		, prev_err_title(p->m_error_title)
	{ 
		p->m_error_title = msg;
	}
	~ErrorTitle()
	{
		prj->m_error_title = prev_err_title;
	}
	KexiProject* prj;
	QString prev_err_title;
};*/

KexiProject::KexiProject(KexiProjectData *pdata, KexiDB::MessageHandler* handler)
 : QObject(), Object(handler)
 , d(new Private())
{
	d->data = pdata;
//! @todo partmanager is outside project, so can be initialised just once:
	Kexi::partManager().lookup();
}

KexiProject::KexiProject(KexiProjectData *pdata, KexiDB::MessageHandler* handler, 
	KexiDB::Connection* conn)
 : QObject(), Object(handler)
 , d(new Private())
{
	d->data = pdata;
	if (d->data->connectionData() == d->connection->data())
		d->connection = conn;
	else
		kdWarning() << "KexiProject::KexiProject(): passed connection's data ("
			<< conn->data()->serverInfoString() << ") is not compatible with project's conn. data ("
			<< d->data->connectionData()->serverInfoString() << ")" << endl;
//! @todo partmanager is outside project, so can be initialised just once:
	Kexi::partManager().lookup();
}

KexiProject::~KexiProject()
{
	closeConnection();
	delete d;
}

KexiDB::Connection *KexiProject::dbConnection() const
{
	return d->connection;
}

KexiProjectData* KexiProject::data() const
{
	return d->data;
}

bool KexiProject::final() const
{
	return d->final;
}

void KexiProject::setFinal(bool set)
{
	d->final = set;
}

int KexiProject::versionMajor() const
{
	return d->versionMajor;
}

int KexiProject::versionMinor() const
{
	return d->versionMinor;
}

tristate
KexiProject::open(bool &incompatibleWithKexi)
{
	return openInternal(&incompatibleWithKexi);
}

tristate
KexiProject::open()
{
	return openInternal(0);
}

tristate
KexiProject::openInternal(bool *incompatibleWithKexi)
{
	if (incompatibleWithKexi)
		*incompatibleWithKexi = false;
	kdDebug() << "KexiProject::open(): " << d->data->databaseName() <<" "<< d->data->connectionData()->driverName  << endl;
	KexiDB::MessageTitle et(this, 
		i18n("Could not open project \"%1\".").arg(d->data->databaseName()));
	
	if (!createConnection()) {
		kdDebug() << "KexiProject::open(): !createConnection()" << endl;
		return false;
	}
	bool cancel = false;
	KexiGUIMessageHandler msgHandler;
	if (!d->connection->useDatabase(d->data->databaseName(), true, &cancel, &msgHandler))
	{
		if (cancel) {
			return cancelled;
		}
		kdDebug() << "KexiProject::open(): !d->connection->useDatabase() " 
			<< d->data->databaseName() <<" "<< d->data->connectionData()->driverName  << endl;

		if (d->connection->errorNum() == ERR_NO_DB_PROPERTY) {
//<temp> 
//! @todo this is temporary workaround as we have no import driver for SQLite
			if (/*supported?*/ !d->data->connectionData()->driverName.lower().startsWith("sqlite")) {
//</temp> 
				if (incompatibleWithKexi)
					*incompatibleWithKexi = true;
			}
			else
				setError(d->connection);
			closeConnection();
			return false;
		}

		setError(d->connection);
		closeConnection();
		return false;
	}

	if (!initProject())
		return false;

	return createInternalStructures(/*insideTransaction*/true);
}

tristate
KexiProject::create(bool forceOverwrite)
{
	KexiDB::MessageTitle et(this, 
		i18n("Could not create project \"%1\".").arg(d->data->databaseName()));
		
	if (!createConnection())
		return false;
	if (!checkWritable())
		return false;
	if (d->connection->databaseExists( d->data->databaseName() )) {
		if (!forceOverwrite)
			return cancelled;
		if (!d->connection->dropDatabase( d->data->databaseName() )) {
			setError(d->connection);
			closeConnection();
			return false;
		}
		kdDebug() << "--- DB '" << d->data->databaseName() << "' dropped ---"<< endl;
	}
	if (!d->connection->createDatabase( d->data->databaseName() )) {
		setError(d->connection);
		closeConnection();
		return false;
	}
	kdDebug() << "--- DB '" << d->data->databaseName() << "' created ---"<< endl;
	// and now: open
	if (!d->connection->useDatabase(d->data->databaseName()))
	{
		kdDebug() << "--- DB '" << d->data->databaseName() << "' USE ERROR ---"<< endl;
		setError(d->connection);
		closeConnection();
		return false;
	}
	kdDebug() << "--- DB '" << d->data->databaseName() << "' used ---"<< endl;

	//<add some data>
	KexiDB::Transaction trans = d->connection->beginTransaction();
	if (trans.isNull())
		return false;

	if (!createInternalStructures(/*!insideTransaction*/false))
		return false;

	//add some metadata
//! @todo put more props. todo - creator, created date, etc. (also to KexiProjectData)
	KexiDB::DatabaseProperties &props = d->connection->databaseProperties();
	if (!props.setValue("kexiproject_major_ver", d->versionMajor)
		|| !props.setCaption("kexiproject_major_ver", i18n("Project major version"))
		|| !props.setValue("kexiproject_minor_ver", d->versionMinor)
		|| !props.setCaption("kexiproject_minor_ver", i18n("Project minor version"))
		|| !props.setValue("project_caption", d->data->caption())
		|| !props.setCaption("project_caption", i18n("Project caption"))
		|| !props.setValue("project_desc", d->data->description())
		|| !props.setCaption("project_desc", i18n("Project description")) )
		return false;

/*	KexiDB::TableSchema *t_db = d->connection->tableSchema("kexi__db");
	//caption:
	if (!t_db)
		return false;

	if (!KexiDB::replaceRow(*d->connection, t_db, "db_property", "project_caption", 
		"db_value", QVariant( d->data->caption() ), KexiDB::Field::Text)
	 || !KexiDB::replaceRow(*d->connection, t_db, "db_property", "project_desc", 
		"db_value", QVariant( d->data->description() ), KexiDB::Field::Text) )
		return false;
*/
	if (trans.active() && !d->connection->commitTransaction(trans))
		return false;
	//</add some data>

	return initProject();
}

bool KexiProject::createInternalStructures(bool insideTransaction)
{
	KexiDB::TransactionGuard tg;
	if (insideTransaction) {
		tg.setTransaction( d->connection->beginTransaction() );
		if (tg.transaction().isNull())
			return false;
	}

	//Get information about kexiproject version.
	//kexiproject version is a version of data layer above kexidb layer.
	KexiDB::DatabaseProperties &props = d->connection->databaseProperties();
	bool ok;
	int storedMajorVersion = props.value("kexiproject_major_ver").toInt(&ok);
	if (!ok)
		storedMajorVersion = 0;
	int storedMinorVersion = props.value("kexiproject_minor_ver").toInt(&ok);
	if (!ok)
		storedMinorVersion = 1;

	bool containsKexi__blobsTable = d->connection->drv_containsTable("kexi__blobs");
	int dummy;
	bool contains_o_folder_id = containsKexi__blobsTable && true == d->connection->querySingleNumber(
		"SELECT COUNT(o_folder_id) FROM kexi__blobs", dummy, 0, false/*addLimitTo1*/);
	bool add_folder_id_column = false;

//! @todo what about read-only db access?
	if (storedMajorVersion<=0) {
		d->versionMajor = KEXIPROJECT_VERSION_MAJOR;
		d->versionMinor = KEXIPROJECT_VERSION_MINOR;
		//For compatibility for projects created before Kexi 1.0 beta 1:
		//1. no kexiproject_major_ver and kexiproject_minor_ver -> add them
		if (!d->connection->isReadOnly()) {
			if (!props.setValue("kexiproject_major_ver", d->versionMajor)
				|| !props.setCaption("kexiproject_major_ver", i18n("Project major version"))
				|| !props.setValue("kexiproject_minor_ver", d->versionMinor)
				|| !props.setCaption("kexiproject_minor_ver", i18n("Project minor version")) ) {
				return false;
			}
		}

		if (containsKexi__blobsTable) {
//! @todo what to do for readonly connections? Should we alter kexi__blobs in memory?
			if (!d->connection->isReadOnly()) {
				if (!contains_o_folder_id) {
					add_folder_id_column = true;
				}
			}
		}
	}
	if (storedMajorVersion!=d->versionMajor || storedMajorVersion!=d->versionMinor) {
		//! @todo version differs: should we change something?
		d->versionMajor = storedMajorVersion;
		d->versionMinor = storedMinorVersion;
	}

	KexiDB::InternalTableSchema *t_blobs = new KexiDB::InternalTableSchema("kexi__blobs");
	t_blobs->addField( new KexiDB::Field("o_id", KexiDB::Field::Integer, 
		KexiDB::Field::PrimaryKey | KexiDB::Field::AutoInc, KexiDB::Field::Unsigned) )
	.addField( new KexiDB::Field("o_data", KexiDB::Field::BLOB) )
	.addField( new KexiDB::Field("o_name", KexiDB::Field::Text ) )
	.addField( new KexiDB::Field("o_caption", KexiDB::Field::Text ) )
	.addField( new KexiDB::Field("o_mime", KexiDB::Field::Text, KexiDB::Field::NotNull) )
	.addField( new KexiDB::Field("o_folder_id", 
	    KexiDB::Field::Integer, 0, KexiDB::Field::Unsigned) //references kexi__gallery_folders.f_id
	                                        //If null, the BLOB only points to virtual "All" folder
	                                        //WILL BE USED in Kexi >=2.0
	);

	//*** create global BLOB container, if not present
	if (containsKexi__blobsTable) {
		//! just insert this schema
		d->connection->insertInternalTableSchema(t_blobs);
		if (add_folder_id_column && !d->connection->isReadOnly()) {
			// 2. "kexi__blobs" table contains no "o_folder_id" column -> add it 
			//    (by copying table to avoid data loss)
			KexiDB::TableSchema *kexi__blobsCopy = new KexiDB::TableSchema( *t_blobs );
			kexi__blobsCopy->setName("kexi__blobs__copy");
			if (!d->connection->drv_createTable( *kexi__blobsCopy )) {
				delete kexi__blobsCopy;
				delete t_blobs;
				return false;
			}
			// 2.1 copy data (insert 0's into o_folder_id column)
			if (!d->connection->executeSQL(
					QString::fromLatin1("INSERT INTO kexi__blobs (o_data, o_name, o_caption, o_mime, o_folder_id) "
					"SELECT o_data, o_name, o_caption, o_mime, 0 FROM kexi__blobs") )
				// 2.2 remove the original kexi__blobs
				|| !d->connection->executeSQL(QString::fromLatin1("DROP TABLE kexi__blobs")) //lowlevel
				// 2.3 rename the copy back into kexi__blobs
				|| !d->connection->drv_alterTableName(*kexi__blobsCopy, "kexi__blobs")
				)
			{
				//(no need to drop the copy, ROLLBACK will drop it)
				delete kexi__blobsCopy;
				delete t_blobs;
				return false;
			}
			delete kexi__blobsCopy; //not needed - physically renamed to kexi_blobs
		}
	}
	else {
//		if (!d->connection->createTable( t_blobs, false/*!replaceExisting*/ )) {
		if (!d->connection->isReadOnly()) {
			if (!d->connection->createTable( t_blobs, true/*replaceExisting*/ )) {
				delete t_blobs;
				return false;
			}
		}
	}

	//Store default part infos.
	//Infos for other parts (forms, reports...) are created on demand in KexiDialogBase::storeNewData()
	KexiDB::InternalTableSchema *t_parts = new KexiDB::InternalTableSchema("kexi__parts"); //newKexiDBSystemTableSchema("kexi__parts");
	t_parts->addField( 
		new KexiDB::Field("p_id", KexiDB::Field::Integer, KexiDB::Field::PrimaryKey | KexiDB::Field::AutoInc, KexiDB::Field::Unsigned) 
	)
	.addField( new KexiDB::Field("p_name", KexiDB::Field::Text) )
	.addField( new KexiDB::Field("p_mime", KexiDB::Field::Text ) )
	.addField( new KexiDB::Field("p_url", KexiDB::Field::Text ) );

	bool containsKexi__partsTable = d->connection->drv_containsTable("kexi__parts");
	bool partsTableOk = true;
	if (containsKexi__partsTable) {
		//! just insert this schema
		d->connection->insertInternalTableSchema(t_parts);
	}
	else {
		if (!d->connection->isReadOnly()) {
			partsTableOk = d->connection->createTable( t_parts, true/*replaceExisting*/ );

			KexiDB::FieldList *fl = t_parts->subList("p_id", "p_name", "p_mime", "p_url");
			if (partsTableOk)
				partsTableOk = d->connection->insertRecord(*fl, QVariant(1), QVariant("Tables"), 
					QVariant("kexi/table"), QVariant("http://koffice.org/kexi/"));

			if (partsTableOk)
				partsTableOk = d->connection->insertRecord(*fl, QVariant(2), QVariant("Queries"), 
					QVariant("kexi/query"), QVariant("http://koffice.org/kexi/"));
		}
	}

	if (!partsTableOk) {
		delete t_parts;
		return false;
	}

	if (insideTransaction) {
		if (tg.transaction().active() && !tg.commit())
			return false;
	}
	return true;
}

bool
KexiProject::createConnection()
{
	if (d->connection)
		return true;

	clearError();
//	closeConnection();//for sanity
	KexiDB::MessageTitle et(this);
	
	KexiDB::Driver *driver = Kexi::driverManager().driver(d->data->connectionData()->driverName);
	if(!driver) {
		setError(&Kexi::driverManager());
		return false;
	}

	int connectionOptions = 0;
	if (d->data->isReadOnly())
		connectionOptions |= KexiDB::Driver::ReadOnlyConnection;
	d->connection = driver->createConnection(*d->data->connectionData(), connectionOptions);
	if (!d->connection)
	{
		kdDebug() << "KexiProject::open(): uuups failed " << driver->errorMsg()  << endl;
		setError(driver);
		return false;
	}

	if (!d->connection->connect())
	{
		setError(d->connection);
		kdDebug() << "KexiProject::createConnection(): error connecting: " << (d->connection ? d->connection->errorMsg() : QString::null) << endl;
		closeConnection();
		return false;
	}

	//re-init BLOB buffer
//! @todo won't work for subsequent connection
	KexiBLOBBuffer::setConnection(d->connection);
	return true;
}


bool
KexiProject::closeConnection()
{
	if (!d->connection)
		return true;

	if (!d->connection->disconnect()) {
		setError(d->connection);
		return false;
	}

	delete d->connection; //this will also clear connection for BLOB buffer
	d->connection = 0;
	return true;
}

bool
KexiProject::initProject()
{
//	emit dbAvailable();
	kdDebug() << "KexiProject::open(): checking project parts..." << endl;
	
	if (!Kexi::partManager().checkProject(d->connection)) {
		setError(Kexi::partManager().error() ? (KexiDB::Object*)&Kexi::partManager() : (KexiDB::Connection*)d->connection);
		return false;
	}

// !@todo put more props. todo - creator, created date, etc. (also to KexiProjectData)
	KexiDB::DatabaseProperties &props = d->connection->databaseProperties();
	QString str( props.value("project_caption").toString() );
	if (!str.isEmpty())
		d->data->setCaption( str );
	str = props.value("project_desc").toString();
	if (!str.isEmpty())
		d->data->setDescription( str );
/*	KexiDB::RowData data;
	QString sql = "select db_value from kexi__db where db_property='%1'";
	if (d->connection->querySingleRecord( sql.arg("project_caption"), data ) && !data[0].toString().isEmpty())
		d->data->setCaption(data[0].toString());
	if (d->connection->querySingleRecord( sql.arg("project_desc"), data) && !data[0].toString().isEmpty())
		d->data->setDescription(data[0].toString());*/

	return true;
}

bool
KexiProject::isConnected()
{
	if(d->connection && d->connection->isDatabaseUsed())
		return true;

	return false;
}

KexiPart::ItemDict*
KexiProject::items(KexiPart::Info *i)
{
	kdDebug() << "KexiProject::items()" << endl;
	if(!i || !isConnected())
		return 0;

	//trying in cache...
	KexiPart::ItemDict *dict = d->itemDictsCache[ i->projectPartID() ];
	if (dict)
		return dict;
	//retrieve:
	KexiDB::Cursor *cursor = d->connection->executeQuery(
		"SELECT o_id, o_name, o_caption  FROM kexi__objects WHERE o_type = " 
		+ QString::number(i->projectPartID()));//, KexiDB::Cursor::Buffered);
//	kdDebug() << "KexiProject::items(): cursor handle is:" << cursor << endl;
	if(!cursor)
		return 0;

	dict = new KexiPart::ItemDict(1009);
	dict->setAutoDelete(true);

	for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
	{
		KexiPart::Item *it = new KexiPart::Item();
		bool ok;
		int ident = cursor->value(0).toInt(&ok);
		QString objName( cursor->value(1).toString() );
		
		if ( ok && (ident>0) && !d->connection->isInternalTableSchema(objName) 
			&& KexiUtils::isIdentifier(objName) )
		{
			it->setIdentifier(ident);
			it->setMimeType(i->mimeType()); //js: may be not null???
			it->setName(objName);
			it->setCaption(cursor->value(2).toString());
		}
		dict->insert(it->identifier(), it);
//		kdDebug() << "KexiProject::items(): ITEM ADDED == "<<objName <<" id="<<ident<<endl;
	}

	d->connection->deleteCursor(cursor);
//	kdDebug() << "KexiProject::items(): end with count " << dict->count() << endl;
	d->itemDictsCache.insert( i->projectPartID(), dict );
	return dict;
}

KexiPart::ItemDict*
KexiProject::itemsForMimeType(const QCString &mimeType)
{
	KexiPart::Info *info = Kexi::partManager().infoForMimeType(mimeType);
	return items(info);
}

void
KexiProject::getSortedItems(KexiPart::ItemList& list, KexiPart::Info *i)
{
	list.clear();
	KexiPart::ItemDict* dict = items(i);
	if (!dict)
		return;
	for (KexiPart::ItemDictIterator it(*dict); it.current(); ++it)
		list.append(it.current());
}

void
KexiProject::getSortedItemsForMimeType(KexiPart::ItemList& list, const QCString &mimeType)
{
	KexiPart::Info *info = Kexi::partManager().infoForMimeType(mimeType);
	getSortedItems(list, info);
}

void
KexiProject::addStoredItem(KexiPart::Info *info, KexiPart::Item *item)
{
	if (!info || !item)
		return;
	KexiPart::ItemDict *dict = items(info);
	item->setNeverSaved( false );
	d->unstoredItems.take(item); //no longer unstored
	dict->insert( item->identifier(), item );
	//let's update e.g. navigator
	emit newItemStored(*item);
}

KexiPart::Item*
KexiProject::itemForMimeType(const QCString &mimeType, const QString &name)
{
	KexiPart::ItemDict *dict = itemsForMimeType(mimeType);
	if (!dict)
		return 0;
	const QString l_name = name.lower();
	for (KexiPart::ItemDictIterator it( *dict ); it.current(); ++it) {
		if (it.current()->name().lower()==l_name)
			return it.current();
	}
	return 0;
}

KexiPart::Item*
KexiProject::item(KexiPart::Info *i, const QString &name)
{
	KexiPart::ItemDict *dict = items(i);
	if (!dict)
		return 0;
	const QString l_name = name.lower();
	for (KexiPart::ItemDictIterator it( *dict ); it.current(); ++it) {
		if (it.current()->name().lower()==l_name)
			return it.current();
	}
	return 0;
}

KexiPart::Item*
KexiProject::item(int identifier)
{
	KexiPart::ItemDict *dict;
	for (QIntDictIterator<KexiPart::ItemDict> it(d->itemDictsCache); (dict = it.current()); ++it) {
		KexiPart::Item *item = dict->find(identifier);
		if (item)
			return item;
	}
	return 0;
}

/*void KexiProject::clearMsg()
{
	clearError();
//	d->error_title=QString::null;
}

void KexiProject::setError(int code, const QString &msg )
{
	Object::setError(code, msg);
	if (Object::error())
		ERRMSG(d->error_title, this);
//		emit error(d->error_title, this);
}


void KexiProject::setError( const QString &msg )
{
	Object::setError(msg);
	if (Object::error())
		ERRMSG(d->error_title, this);
//		emit error(d->error_title, this);
}

void KexiProject::setError( KexiDB::Object *obj )
{
	if (!obj)
		return;
	Object::setError(obj);
	if (Object::error())
		ERRMSG(d->error_title, obj);
//		emit error(d->error_title, obj);
}

void KexiProject::setError(const QString &msg, const QString &desc)
{
	Object::setError(msg); //ok?
	ERRMSG(msg, desc); //not KexiDB-related
//	emit error(msg, desc); //not KexiDB-related
}
*/

KexiPart::Part *KexiProject::findPartFor(KexiPart::Item& item)
{
	clearError();
	KexiDB::MessageTitle et(this);
	KexiPart::Part *part = Kexi::partManager().partForMimeType(item.mimeType());
	if (!part)
		setError(&Kexi::partManager());
	return part;
}

KexiDialogBase* KexiProject::openObject(KexiMainWindow *wnd, KexiPart::Item& item, 
	int viewMode, QMap<QString,QString>* staticObjectArgs)
{
	clearError();
	if (viewMode!=Kexi::DataViewMode && data()->userMode())
		return 0;
	
	KexiDB::MessageTitle et(this);
	KexiPart::Part *part = findPartFor(item);
	if (!part)
		return 0;
	KexiDialogBase *dlg  = part->openInstance(wnd, item, viewMode, staticObjectArgs);
	if (!dlg) {
		if (part->lastOperationStatus().error())
			setError(i18n("Opening object \"%1\" failed.").arg(item.name())+"<br>"
				+part->lastOperationStatus().message, 
				part->lastOperationStatus().description);
		return 0;
	}
	return dlg;
}

KexiDialogBase* KexiProject::openObject(KexiMainWindow *wnd, const QCString &mimeType, 
	const QString& name, int viewMode)
{
	KexiPart::Item *it = itemForMimeType(mimeType, name);
	return it ? openObject(wnd, *it, viewMode) : 0;
}

bool KexiProject::checkWritable()
{
	if (!d->connection->isReadOnly())
		return true;
	setError(i18n("This project is opened as read only."));
	return false;
}

bool KexiProject::removeObject(KexiMainWindow *wnd, KexiPart::Item& item)
{
	clearError();
	if (data()->userMode())
		return false;
	
	KexiDB::MessageTitle et(this);
	if (!checkWritable())
		return false;
	KexiPart::Part *part = findPartFor(item);
	if (!part)
		return false;
	if (!item.neverSaved() && !part->remove(wnd, item)) {
		//js TODO check for errors
		return false;
	}
	if (!item.neverSaved()) {
		KexiDB::TransactionGuard tg( *d->connection );
		if (!tg.transaction().active()) {
			setError(d->connection);
			return false;
		}
		if (!d->connection->removeObject( item.identifier() )) {
			setError(d->connection);
			return false;
		}
		if (!tg.commit()) {
			setError(d->connection);
			return false;
		}
	}
	emit itemRemoved(item);

	//now: remove this item from cache
	if (part->info()) {
		KexiPart::ItemDict *dict = d->itemDictsCache[ part->info()->projectPartID() ];
		if (!(dict && dict->remove( item.identifier() )))
			d->unstoredItems.remove(&item);//remove temp.
	}
	return true;
}

bool KexiProject::renameObject( KexiMainWindow *wnd, KexiPart::Item& item, const QString& _newName )
{
	clearError();
	if (data()->userMode())
		return 0;
	
	KexiUtils::WaitCursor wait;
	QString newName = _newName.stripWhiteSpace();
	{
		KexiDB::MessageTitle et(this);
		if (newName.isEmpty()) {
			setError( i18n("Could not set empty name for this object.") );
			return false;
		}
		if (this->itemForMimeType(item.mimeType(), newName)!=0) {
			setError( i18n("Could not use this name. Object with name \"%1\" already exists.")
				.arg(newName) );
			return false;
		}
	}

	KexiDB::MessageTitle et(this, 
		i18n("Could not rename object \"%1\".").arg(item.name()) );
	if (!checkWritable())
		return false;
	KexiPart::Part *part = findPartFor(item);
	if (!part)
		return false;
	KexiDB::TransactionGuard tg( *d->connection );
	if (!tg.transaction().active()) {
		setError(d->connection);
		return false;
	}
	if (!part->rename(wnd, item, newName)) {
		setError(part->lastOperationStatus().message, part->lastOperationStatus().description);
		return false;
	}
	if (!d->connection->executeSQL( "update kexi__objects set o_name="
		+ d->connection->driver()->valueToSQL( KexiDB::Field::Text, newName )
		+ " where o_id=" + QString::number(item.identifier()) )) {
		setError(d->connection);
		return false;
	}
	if (!tg.commit()) {
		setError(d->connection);
		return false;
	}
	QCString oldName( item.name().latin1() );
	item.setName( newName );
	emit itemRenamed(item, oldName);
	return true;
}

KexiPart::Item* KexiProject::createPartItem(KexiPart::Info *info, const QString& suggestedCaption)
{
	clearError();
	if (data()->userMode())
		return 0;

	KexiDB::MessageTitle et(this);
	KexiPart::Part *part = Kexi::partManager().part(info);
	if (!part) {
		setError(&Kexi::partManager());
		return 0;
	}

	KexiPart::ItemDict *dict = items(info);

	//find new, unique default name for this item
	int n;
	QString new_name;
	QString base_name;
	if (suggestedCaption.isEmpty()) {
		n = 1;
		base_name = part->instanceName();
	}
	else {
		n = 0; //means: try not to add 'n'
		base_name = KexiUtils::string2Identifier(suggestedCaption).lower();
	}
	base_name = KexiUtils::string2Identifier(base_name).lower();
	KexiPart::ItemDictIterator it(*dict);
	QPtrDictIterator<KexiPart::Item> itUnstored(d->unstoredItems);
	do {
		new_name = base_name;
		if (n>=1)
			new_name += QString::number(n);
		for (it.toFirst(); it.current(); ++it) {
			if (it.current()->name().lower()==new_name)
				break;
		}
		if ( it.current() ) {
			n++;
			continue; //stored exists!
		}
		for (itUnstored.toFirst(); itUnstored.current(); ++itUnstored) {
			if (itUnstored.current()->name().lower()==new_name)
				break;
		}
		if ( !itUnstored.current() )
			break; //unstored doesn't exist
		n++;
	} while (n<1000/*sanity*/);

	if (n>=1000)
		return 0;

	QString new_caption( suggestedCaption.isEmpty() ? part->instanceCaption() : suggestedCaption);
	if (n>=1)
		new_caption += QString::number(n);

	KexiPart::Item *item = new KexiPart::Item();
	item->setIdentifier( --d->tempPartItemID_Counter );//temporary
	item->setMimeType(info->mimeType());
	item->setName(new_name);
	item->setCaption(new_caption);
	item->setNeverSaved(true);
	d->unstoredItems.insert(item, item);
	return item;
}

KexiPart::Item* KexiProject::createPartItem(KexiPart::Part *part, const QString& suggestedCaption)
{
	return createPartItem(part->info(), suggestedCaption);
}

void KexiProject::deleteUnstoredItem(KexiPart::Item *item)
{
	if (!item)
		return;
	d->unstoredItems.remove(item);
}

KexiDB::Parser* KexiProject::sqlParser()
{
	if (!d->sqlParser) {
		if (!d->connection)
			return 0;
		d->sqlParser = new KexiDB::Parser(d->connection);
	}
	return d->sqlParser;
}

static const QString warningNoUndo = i18n("Warning: entire project's data will be removed.");

/*static*/
KexiProject*
KexiProject::createBlankProject(bool &cancelled, KexiProjectData* data,
	KexiDB::MessageHandler* handler)
{
	cancelled = false;
	KexiProject *prj = new KexiProject( new KexiProjectData(*data), handler );

	tristate res = prj->create(false);
	if (~res) {
//! @todo move to KexiMessageHandler
		if (KMessageBox::Yes != KMessageBox::warningYesNo(0, "<qt>"+i18n(
			"The project %1 already exists.\n"
			"Do you want to replace it with a new, blank one?")
				.arg(prj->data()->infoString())+"\n"+warningNoUndo+"</qt>",
			QString::null, KGuiItem(i18n("Replace")), KStdGuiItem::cancel() ))
//todo add serverInfoString() for server-based prj
		{
			delete prj;
			cancelled = true;
			return 0;
		}
		res = prj->create(true/*overwrite*/);
	}
	if (res != true) {
		delete prj;
		return 0;
	}
	kdDebug() << "KexiProject::createBlankProject(): new project created --- " << endl;
//todo?	Kexi::recentProjects().addProjectData( data );

	return prj;
}

/*static*/
tristate KexiProject::dropProject(KexiProjectData* data, 
	KexiDB::MessageHandler* handler, bool dontAsk)
{
	if (!dontAsk && KMessageBox::Yes != KMessageBox::warningYesNo(0, 
		i18n("Do you want to drop the project \"%1\"?").arg(data->objectName())+"\n"+warningNoUndo ))
		return cancelled;

	KexiProject prj( new KexiProjectData(*data), handler );
	if (!prj.open())
		return false;

	if (prj.dbConnection()->isReadOnly()) {
		handler->showErrorMessage(
			i18n("Could not drop this project. Database connection for this project has been opened as read only."));
		return false;
	}

	return prj.dbConnection()->dropDatabase();
}

/*void KexiProject::reloadPartItem( KexiDialogBase* dialog )
{
	if (!dialog)
		return;

	KexiPart::Item* item = dialog->partItem();

	if (dialog || !d->connection->setQuerySchemaObsolete( queryName ))
		return;
	KexiPart::Info *partInfo = Kexi::partManager().infoForMimeType("kexi/query");
	if (!partInfo)
		return; //err?
		item(partInfo, queryName);
	if (!item)
		return; //err?
	emit itemSetO

}*/

#include "kexiproject.moc"
