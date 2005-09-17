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
#include <kexiutils/utils.h>

#include "kexiproject.h"
#include "kexipartmanager.h"
#include "kexipartitem.h"
#include "kexipartinfo.h"
#include "kexipart.h"
#include "kexidialogbase.h"
#include "kexi.h"
#include "keximainwindow.h"

#include <assert.h>

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
 , m_data(pdata)
 , m_itemDictsCache(199)
 , m_unstoredItems(199)
 , m_tempPartItemID_Counter(-1)
 , m_sqlParser(0)
{
//TODO: partmanager is outside project, so can be initialised just once:

	m_itemDictsCache.setAutoDelete(true);
	m_unstoredItems.setAutoDelete(true);
	Kexi::partManager().lookup();
	
	m_connection = 0;
	m_final = false;
}

KexiProject::~KexiProject()
{
	closeConnection();
	delete m_data;
	m_data=0;
	delete m_sqlParser;
}

bool
KexiProject::open()
{
	kdDebug() << "KexiProject::open(): " << m_data->databaseName() <<" "<< m_data->connectionData()->driverName  << endl;
	KexiDB::MessageTitle et(this, 
		i18n("Could not open project \"%1\"").arg(m_data->databaseName()));
	
	if (!createConnection()) {
		kdDebug() << "KexiProject::open(): !createConnection()" << endl;
		return false;
	}
	if (!m_connection->useDatabase(m_data->databaseName()))
	{
		kdDebug() << "KexiProject::open(): !m_connection->useDatabase() " << m_data->databaseName() <<" "<< m_data->connectionData()->driverName  << endl;
		setError(m_connection);
		closeConnection();
		return false;
	}

	return initProject();
}

tristate
KexiProject::create(bool forceOverwrite)
{
	KexiDB::MessageTitle et(this, 
		i18n("Could not create project \"%1\"").arg(m_data->databaseName()));
		
	if (!createConnection())
		return false;
	if (m_connection->databaseExists( m_data->databaseName() )) {
		if (!forceOverwrite)
			return cancelled;
		if (!m_connection->dropDatabase( m_data->databaseName() )) {
			setError(m_connection);
			closeConnection();
			return false;
		}
		kdDebug() << "--- DB '" << m_data->databaseName() << "' dropped ---"<< endl;
	}
	if (!m_connection->createDatabase( m_data->databaseName() )) {
		setError(m_connection);
		closeConnection();
		return false;
	}
	kdDebug() << "--- DB '" << m_data->databaseName() << "' created ---"<< endl;
	// and now: open
	if (!m_connection->useDatabase(m_data->databaseName()))
	{
		kdDebug() << "--- DB '" << m_data->databaseName() << "' USE ERROR ---"<< endl;
		setError(m_connection);
		closeConnection();
		return false;
	}
	kdDebug() << "--- DB '" << m_data->databaseName() << "' used ---"<< endl;

	//<add some data>
	KexiDB::Transaction trans = m_connection->beginTransaction();

	//create global BLOB container
	KexiDB::InternalTableSchema *t_blobs = new KexiDB::InternalTableSchema("kexi__blobs");
	t_blobs->addField( new KexiDB::Field("o_id", KexiDB::Field::Integer, 
		KexiDB::Field::PrimaryKey | KexiDB::Field::AutoInc, KexiDB::Field::Unsigned) )
	.addField( new KexiDB::Field("o_data", KexiDB::Field::BLOB) )
	.addField( new KexiDB::Field("o_name", KexiDB::Field::Text ) )
	.addField( new KexiDB::Field("o_caption", KexiDB::Field::Text ) )
	.addField( new KexiDB::Field("o_mime", KexiDB::Field::Text, KexiDB::Field::NotNull, 
		KexiDB::Field::NoOptions, 0, 0 ) );

	if (!m_connection->createTable( t_blobs, false/*!replaceExisting*/ ))
		return false;

	//add some metadata
	KexiDB::TableSchema *t_db = m_connection->tableSchema("kexi__db");
//TODO: put more props. todo - creator, created date, etc. (also to KexiProjectData)
	//caption:
	if (!t_db)
		return false;

	if (!KexiDB::replaceRow(*m_connection, t_db, "db_property", "project_caption", 
		"db_value", QVariant( m_data->caption() ), KexiDB::Field::Text)
	 || !KexiDB::replaceRow(*m_connection, t_db, "db_property", "project_desc", 
		"db_value", QVariant( m_data->description() ), KexiDB::Field::Text) )
		return false;

	if (trans.active() && !m_connection->commitTransaction(trans))
		return false;
	//</add some data>

	return initProject();
}

bool
KexiProject::createConnection()
{
	if (m_connection)
		return true;

	clearError();
//	closeConnection();//for sanity
	KexiDB::MessageTitle et(this);
	
	KexiDB::Driver *driver = Kexi::driverManager().driver(m_data->connectionData()->driverName);
	if(!driver) {
		setError(&Kexi::driverManager());
		return false;
	}

	m_connection = driver->createConnection(*m_data->connectionData());
	if (!m_connection)
	{
		kdDebug() << "KexiProject::open(): uuups failed " << driver->errorMsg()  << endl;
		setError(driver);
		return false;
	}

	if (!m_connection->connect())
	{
		setError(m_connection);
		kdDebug() << "KexiProject::createConnection(): error connecting: " << (m_connection ? m_connection->errorMsg() : QString::null) << endl;
		closeConnection();
		return false;
	}

	return true;
}


void
KexiProject::closeConnection()
{
	if (!m_connection)
		return;
		
	delete m_connection;
	m_connection = 0;
}

bool
KexiProject::initProject()
{
//	emit dbAvailable();
	kdDebug() << "KexiProject::open(): checking project parts..." << endl;
	
	if (!Kexi::partManager().checkProject(m_connection)) {
		setError(&Kexi::partManager());
		return false;
	}

	//TODO: put more props. todo - creator, created date, etc. (also to KexiProjectData)
	KexiDB::RowData data;
	QString sql = "select db_value from kexi__db where db_property='%1'";
	if (m_connection->querySingleRecord( sql.arg("project_caption"), data ) && !data[0].toString().isEmpty())
		m_data->setCaption(data[0].toString());
	if (m_connection->querySingleRecord( sql.arg("project_desc"), data) && !data[0].toString().isEmpty())
		m_data->setDescription(data[0].toString());

	return true;
}

bool
KexiProject::isConnected()
{
	if(m_connection && m_connection->isDatabaseUsed())
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
	KexiPart::ItemDict *dict = m_itemDictsCache[ i->projectPartID() ];
	if (dict)
		return dict;
	//retrieve:
	KexiDB::Cursor *cursor = m_connection->executeQuery(
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
		QString objName = cursor->value(1).toString();
		if ( ok && (ident>0) && KexiUtils::isIdentifier(objName) ) {
			it->setIdentifier(ident);
			it->setMime(i->mime()); //js: may be not null???
			it->setName(objName);
			it->setCaption(cursor->value(2).toString());
		}
		dict->insert(it->identifier(), it);
//		kdDebug() << "KexiProject::items(): ITEM ADDED == "<<objName <<" id="<<ident<<endl;
	}

	m_connection->deleteCursor(cursor);
//	kdDebug() << "KexiProject::items(): end with count " << dict->count() << endl;
	m_itemDictsCache.insert( i->projectPartID(), dict );
	return dict;
}

KexiPart::ItemDict*
KexiProject::items(const QCString &mime)
{
	KexiPart::Info *info = Kexi::partManager().info(mime);
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
KexiProject::getSortedItems(KexiPart::ItemList& list, const QCString &mime)
{
	KexiPart::Info *info = Kexi::partManager().info(mime);
	getSortedItems(list, info);
}

void
KexiProject::addStoredItem(KexiPart::Info *info, KexiPart::Item *item)
{
	if (!info || !item)
		return;
	KexiPart::ItemDict *dict = items(info);
	item->setNeverSaved( false );
	m_unstoredItems.take(item); //no longer unstored
	dict->insert( item->identifier(), item );
	//let's update e.g. navigator
	emit newItemStored(*item);
}

KexiPart::Item*
KexiProject::item(const QCString &mime, const QString &name)
{
	KexiPart::ItemDict *dict = items(mime);
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

/*void KexiProject::clearMsg()
{
	clearError();
//	m_error_title=QString::null;
}

void KexiProject::setError(int code, const QString &msg )
{
	Object::setError(code, msg);
	if (Object::error())
		ERRMSG(m_error_title, this);
//		emit error(m_error_title, this);
}


void KexiProject::setError( const QString &msg )
{
	Object::setError(msg);
	if (Object::error())
		ERRMSG(m_error_title, this);
//		emit error(m_error_title, this);
}

void KexiProject::setError( KexiDB::Object *obj )
{
	if (!obj)
		return;
	Object::setError(obj);
	if (Object::error())
		ERRMSG(m_error_title, obj);
//		emit error(m_error_title, obj);
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
	KexiPart::Part *part = Kexi::partManager().part(item.mime());
	if (!part)
		setError(&Kexi::partManager());
	return part;
}

KexiDialogBase* KexiProject::openObject(KexiMainWindow *wnd, KexiPart::Item& item, int viewMode)
{
	clearError();
	KexiDB::MessageTitle et(this);
	KexiPart::Part *part = findPartFor(item);
	if (!part)
		return 0;
	KexiDialogBase *dlg  = part->openInstance(wnd, item, viewMode);
	if (!dlg) {
		if (part->lastOperationStatus().error())
			setError(i18n("Opening object \"%1\" failed.").arg(item.name())+"<br>"+part->lastOperationStatus().message, 
				part->lastOperationStatus().description);
		return 0;
	}
	return dlg;
}

KexiDialogBase* KexiProject::openObject(KexiMainWindow *wnd, const QCString &mime, const QString& name, 
	int viewMode)
{
	KexiPart::Item *it = item(mime, name);
	return it ? openObject(wnd, *it, viewMode) : 0;
}

bool KexiProject::removeObject(KexiMainWindow *wnd, KexiPart::Item& item)
{
	clearError();
	KexiDB::MessageTitle et(this);
	KexiPart::Part *part = findPartFor(item);
	if (!part)
		return false;
	if (!item.neverSaved() && !part->remove(wnd, item)) {
		//js TODO check for errors
		return false;
	}
	KexiDB::TransactionGuard tg( *m_connection );
	if (!tg.transaction().active())
		return false;
	if (!m_connection->removeObject( item.identifier() )) {
		return false;
	}
	if (!tg.commit())
		return false;
	emit itemRemoved(item);
	//now: remove this item from cache
	if (part->info()) {
		KexiPart::ItemDict *dict = m_itemDictsCache[ part->info()->projectPartID() ];
		if (!(dict && dict->remove( item.identifier() )))
			m_unstoredItems.remove(&item);//remove temp.
	}
	return true;
}

bool KexiProject::renameObject( KexiMainWindow *wnd, KexiPart::Item& item, const QString& _newName )
{
	KexiUtils::WaitCursor wait;
	clearError();
	QString newName = _newName.stripWhiteSpace();
	{
		KexiDB::MessageTitle et(this);
		if (newName.isEmpty()) {
			setError( i18n("Could not set empty name for this object.") );
			return false;
		}
		if (this->item(item.mime(), newName)!=0) {
			setError( i18n("Could not use this name. Object with name \"%1\" already exists.").arg(newName) );
			return false;
		}
	}

	KexiDB::MessageTitle et(this, 
		i18n("Could not rename object \"%1\".").arg(item.name()) );
	KexiPart::Part *part = findPartFor(item);
	if (!part)
		return false;
	KexiDB::TransactionGuard tg( *m_connection );
	if (!tg.transaction().active()) {
		setError(m_connection);
		return false;
	}
	if (!part->rename(wnd, item, newName)) {
		setError(part->lastOperationStatus().message, part->lastOperationStatus().description);
		return false;
	}
	if (!m_connection->executeSQL( "update kexi__objects set o_name="
		+ m_connection->driver()->valueToSQL( KexiDB::Field::Text, newName )
		+ " where o_id=" + QString::number(item.identifier()) )) {
		setError(m_connection);
		return false;
	}
	if (!tg.commit()) {
		setError(m_connection);
		return false;
	}
	QCString oldName( item.name().latin1() );
	item.setName( newName );
	emit itemRenamed(item, oldName);
	return true;
}

KexiPart::Item* KexiProject::createPartItem(KexiPart::Info *info, const QString& suggestedName)
{
	clearError();
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
	if (suggestedName.isEmpty()) {
		n = 1;
		base_name = part->instanceName();
	}
	else {
		n = 0; //means: try not to add 'n'
		base_name = suggestedName;
	}
	base_name = KexiUtils::string2Identifier(base_name).lower();
	KexiPart::ItemDictIterator it(*dict);
	QPtrDictIterator<KexiPart::Item> itUnstored(m_unstoredItems);
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

	QString new_caption = (suggestedName.isEmpty() ? part->instanceName() : suggestedName);
	if (n>=1)
		new_caption += QString::number(n);

	KexiPart::Item *item = new KexiPart::Item();
	item->setIdentifier( --m_tempPartItemID_Counter );//temporary
	item->setMime(info->mime());
	item->setName(new_name);
	item->setCaption(new_caption);
	item->setNeverSaved(true);
	m_unstoredItems.insert(item, item);
	return item;
}

void KexiProject::deleteUnstoredItem(KexiPart::Item *item)
{
	if (!item)
		return;
	m_unstoredItems.remove(item);
}

KexiDB::Parser* KexiProject::sqlParser()
{
	if (!m_sqlParser) {
		if (!m_connection)
			return 0;
		m_sqlParser = new KexiDB::Parser(m_connection);
	}
	return m_sqlParser;
}

static const QString warningNoUndo = i18n("Warning: entire project's data will be removed.");

/*static*/
KexiProject*
KexiProject::createBlankProject(bool &cancelled, KexiProjectData* data,
	KexiDB::MessageHandler* handler)
{
	cancelled = false;
	KexiProject *prj = new KexiProject( new KexiProjectData(*data), handler );

	bool ok = true;
	tristate res = prj->create(false);
	if (~res) {
//todo move to KexiMessageHandler
		if (KMessageBox::Yes != KMessageBox::warningYesNo(0, i18n(
			"The project \"%1\" already exists.\n"
			"Do you want to replace it with a new, blank one?")
			.arg(data->objectName())+"\n"+warningNoUndo ))
//todo add serverInfoString() for server-based prj
		{
			delete prj;
			cancelled = true;
			return 0;
		}
		res = prj->create(true/*overwrite*/);
	}
	ok = res;
	if (!ok) {
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

	return prj.dbConnection()->dropDatabase();
}

#include "kexiproject.moc"

