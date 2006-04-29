/* This file is part of the KDE project
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "utils.h"
#include "cursor.h"
#include "drivermanager.h"

#include <qmap.h>
#include <qthread.h>

#include <kdebug.h>
#include <klocale.h>
#include <kstaticdeleter.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kiconloader.h>

#include "utils_p.h"

using namespace KexiDB;

//! Cache
struct TypeCache
{
	QMap< uint, TypeGroupList > tlist;
	QMap< uint, QStringList > nlist;
	QMap< uint, QStringList > slist;
	QMap< uint, Field::Type > def_tlist;
};

static KStaticDeleter<TypeCache> KexiDB_typeCacheDeleter;
TypeCache *KexiDB_typeCache = 0;

static void initList()
{
	KexiDB_typeCacheDeleter.setObject( KexiDB_typeCache, new TypeCache() );

	for (uint t=0; t<=KexiDB::Field::LastType; t++) {
		const uint tg = KexiDB::Field::typeGroup( t );
		TypeGroupList list;
		QStringList name_list, str_list;
		if (KexiDB_typeCache->tlist.find( tg )!=KexiDB_typeCache->tlist.end()) {
			list = KexiDB_typeCache->tlist[ tg ];
			name_list = KexiDB_typeCache->nlist[ tg ];
			str_list = KexiDB_typeCache->slist[ tg ];
		}
		list+= t;
		name_list += KexiDB::Field::typeName( t );
		str_list += KexiDB::Field::typeString( t );
		KexiDB_typeCache->tlist[ tg ] = list;
		KexiDB_typeCache->nlist[ tg ] = name_list;
		KexiDB_typeCache->slist[ tg ] = str_list;
	}

	KexiDB_typeCache->def_tlist[ Field::InvalidGroup ] = Field::InvalidType;
	KexiDB_typeCache->def_tlist[ Field::TextGroup ] = Field::Text;
	KexiDB_typeCache->def_tlist[ Field::IntegerGroup ] = Field::Integer;
	KexiDB_typeCache->def_tlist[ Field::FloatGroup ] = Field::Float;
	KexiDB_typeCache->def_tlist[ Field::BooleanGroup ] = Field::Boolean;
	KexiDB_typeCache->def_tlist[ Field::DateTimeGroup ] = Field::Date;
	KexiDB_typeCache->def_tlist[ Field::BLOBGroup ] = Field::BLOB;
}

const TypeGroupList KexiDB::typesForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	if (!KexiDB_typeCache)
		initList();
	return KexiDB_typeCache->tlist[ typeGroup ];
}

QStringList KexiDB::typeNamesForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	if (!KexiDB_typeCache)
		initList();
	return KexiDB_typeCache->nlist[ typeGroup ];
}

QStringList KexiDB::typeStringsForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	if (!KexiDB_typeCache)
		initList();
	return KexiDB_typeCache->slist[ typeGroup ];
}

KexiDB::Field::Type KexiDB::defaultTypeForGroup(KexiDB::Field::TypeGroup typeGroup)
{
	if (!KexiDB_typeCache)
		initList();
	return (typeGroup <= Field::LastTypeGroup) ? KexiDB_typeCache->def_tlist[ typeGroup ] : Field::InvalidType;
}

void KexiDB::getHTMLErrorMesage(Object* obj, QString& msg, QString &details)
{
	Connection *conn = 0;
	if (!obj || !obj->error()) {
		if (dynamic_cast<Cursor*>(obj)) {
			conn = dynamic_cast<Cursor*>(obj)->connection();
			obj = conn;
		}
		else {
			return;
		}
	}
//	if (dynamic_cast<Connection*>(obj)) {
	//	conn = dynamic_cast<Connection*>(obj);
	//}
	if (!obj || !obj->error())
		return;
	//lower level message is added to the details, if there is alread message specified
	if (!obj->msgTitle().isEmpty())
		msg += "<p>" + obj->msgTitle();
	
	if (msg.isEmpty())
		msg = "<p>" + obj->errorMsg();
	else
		details += "<p>" + obj->errorMsg();

	if (!obj->serverErrorMsg().isEmpty())
		details += "<p><b><nobr>" +i18n("Message from server:") + "</nobr></b><br>" + obj->serverErrorMsg();
	if (!obj->recentSQLString().isEmpty())
		details += "<p><b><nobr>" +i18n("SQL statement:") + QString("</nobr></b><br><tt>%1</tt>").arg(obj->recentSQLString());
	int serverResult;
	QString serverResultName;
	if (obj->serverResult()!=0) {
		serverResult = obj->serverResult();
		serverResultName = obj->serverResultName();
	}
	else {
		serverResult = obj->previousServerResult();
		serverResultName = obj->previousServerResultName();
	}
	if (!serverResultName.isEmpty())
		details += (QString("<p><b><nobr>")+i18n("Server result name:")+"</nobr></b><br>"+serverResultName);
	if (!details.isEmpty() 
		&& (!obj->serverErrorMsg().isEmpty() || !obj->recentSQLString().isEmpty() || !serverResultName.isEmpty() || serverResult!=0) )
	{
		details += (QString("<p><b><nobr>")+i18n("Server result number:")+"</nobr></b><br>"+QString::number(serverResult));
	}

	if (!details.isEmpty() && !details.startsWith("<qt>")) {
		if (details.startsWith("<p>"))
			details = QString::fromLatin1("<qt>")+details;
		else
			details = QString::fromLatin1("<qt><p>")+details;
	}
}

void KexiDB::getHTMLErrorMesage(Object* obj, QString& msg)
{
	getHTMLErrorMesage(obj, msg, msg);
}

void KexiDB::getHTMLErrorMesage(Object* obj, ResultInfo *result)
{
	getHTMLErrorMesage(obj, result->msg, result->desc);
}

int KexiDB::idForObjectName( Connection &conn, const QString& objName, int objType )
{
	RowData data;
	if (true!=conn.querySingleRecord(QString("select o_id from kexi__objects where lower(o_name)='%1' and o_type=%2")
		.arg(objName.lower()).arg(objType), data))
		return 0;
	bool ok;
	int id = data[0].toInt(&ok);
	return ok ? id : 0;
}

//-----------------------------------------

TableOrQuerySchema::TableOrQuerySchema(Connection *conn, const QCString& name, bool table)
 : m_name(name)
 , m_table(table ? conn->tableSchema(QString(name)) : 0)
 , m_query(table ? 0 : conn->querySchema(QString(name)))
{
	if (table && !m_table)
		kdWarning() << "TableOrQuery(Connection *conn, const QCString& name, bool table) : "
			"no table specified!" << endl;
	if (!table && !m_query)
		kdWarning() << "TableOrQuery(Connection *conn, const QCString& name, bool table) : "
			"no query specified!" << endl;
}

TableOrQuerySchema::TableOrQuerySchema(FieldList &tableOrQuery)
 : m_table(dynamic_cast<TableSchema*>(&tableOrQuery))
 , m_query(dynamic_cast<QuerySchema*>(&tableOrQuery))
{
	if (!m_table && !m_query)
		kdWarning() << "TableOrQuery(FieldList &tableOrQuery) : "
			" tableOrQuery is nether table nor query!" << endl;
}

TableOrQuerySchema::TableOrQuerySchema(Connection *conn, int id)
{
	m_table = conn->tableSchema(id);
	m_query = m_table ? 0 : conn->querySchema(id);
	if (!m_table && !m_query)
		kdWarning() << "TableOrQuery(Connection *conn, int id) : no table or query found for id==" 
			<< id << "!" << endl;
}

TableOrQuerySchema::TableOrQuerySchema(TableSchema* table)
 : m_table(table)
 , m_query(0)
{
	if (!m_table)
		kdWarning() << "TableOrQuery(TableSchema* table) : no table specified!" << endl;
}

TableOrQuerySchema::TableOrQuerySchema(QuerySchema* query)
 : m_table(0)
 , m_query(query)
{
	if (!m_query)
		kdWarning() << "TableOrQuery(QuerySchema* query) : no query specified!" << endl;
}

const QueryColumnInfo::Vector TableOrQuerySchema::columns(bool unique)
{
	if (m_table)
		return m_table->query()->fieldsExpanded();
	
	if (m_query)
		return m_query->fieldsExpanded(unique);

	kdWarning() << "TableOrQuery::fields() : no query or table specified!" << endl;
	return QueryColumnInfo::Vector();
}

QCString TableOrQuerySchema::name() const
{
	if (m_table)
		return m_table->name().latin1();
	if (m_query)
		return m_query->name().latin1();
	return m_name;
}

QString TableOrQuerySchema::captionOrName() const
{
	SchemaData *sdata = m_table ? static_cast<SchemaData *>(m_table) : static_cast<SchemaData *>(m_query);
	if (!sdata)
		return m_name;
	return sdata->caption().isEmpty() ? sdata->name() : sdata->caption();
}

Field* TableOrQuerySchema::field(const QString& name)
{
	if (m_table)
		return m_table->field(name);
	if (m_query)
		return m_query->field(name);

	return 0;
}

QueryColumnInfo* TableOrQuerySchema::columnInfo(const QString& name)
{
	if (m_table)
		return m_table->query()->columnInfo(name);
	
	if (m_query)
		return m_query->columnInfo(name);

	return 0;
}

QString TableOrQuerySchema::debugString()
{
	if (m_table)
		return m_table->debugString();
	else if (m_query)
		return m_query->debugString();
	return QString::null;
}

void TableOrQuerySchema::debug()
{
	if (m_table)
		return m_table->debug();
	else if (m_query)
		return m_query->debug();
}

Connection* TableOrQuerySchema::connection() const
{
	if (m_table)
		return m_table->connection();
	else if (m_query)
		return m_query->connection();
	return 0;
}


//------------------------------------------

class ConnectionTestThread : public QThread {
	public:
		ConnectionTestThread(ConnectionTestDialog *dlg, const KexiDB::ConnectionData& connData);
		virtual void run();
	protected:
		ConnectionTestDialog* m_dlg;
		KexiDB::ConnectionData m_connData;
};

ConnectionTestThread::ConnectionTestThread(ConnectionTestDialog* dlg, const KexiDB::ConnectionData& connData)
 : m_dlg(dlg), m_connData(connData)
{
}

void ConnectionTestThread::run()
{
	KexiDB::DriverManager manager;
	KexiDB::Driver* drv = manager.driver(m_connData.driverName);
//	KexiGUIMessageHandler msghdr;
	if (!drv || manager.error()) {
//move		msghdr.showErrorMessage(&Kexi::driverManager());
		m_dlg->error(&manager);
		return;
	}
	KexiDB::Connection * conn = drv->createConnection(m_connData);
	if (!conn || drv->error()) {
//move		msghdr.showErrorMessage(drv);
		delete conn;
		m_dlg->error(drv);
		return;
	}
	if (!conn->connect() || conn->error()) {
//move		msghdr.showErrorMessage(conn);
		m_dlg->error(conn);
		delete conn;
		return;
	}
	// SQL database backends like PostgreSQL require executing "USE database" 
	// if we really want to know connection to the server succeeded.
	QString tmpDbName;
	if (!conn->useTemporaryDatabaseIfNeeded( tmpDbName )) {
		m_dlg->error(conn);
		delete conn;
		return;
	}
	delete conn;
	m_dlg->error(0);
}

ConnectionTestDialog::ConnectionTestDialog(QWidget* parent, 
	const KexiDB::ConnectionData& data,
	KexiDB::MessageHandler& msgHandler)
 : KProgressDialog(parent, "testconn_dlg",
	i18n("Test Connection"), i18n("<qt>Testing connection to <b>%1</b> database server...</qt>")
	.arg(data.serverInfoString(true)), true /*modal*/)
 , m_thread(new ConnectionTestThread(this, data))
 , m_connData(data)
 , m_msgHandler(&msgHandler)
 , m_elapsedTime(0)
 , m_errorObj(0)
 , m_stopWaiting(false)
{
	showCancelButton(true);
	progressBar()->setPercentageVisible(false);
	progressBar()->setTotalSteps(0);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
	adjustSize();
	resize(250, height());
}

ConnectionTestDialog::~ConnectionTestDialog()
{
	m_wait.wakeAll();
	m_thread->terminate();
	delete m_thread;
}

int ConnectionTestDialog::exec()
{
	m_timer.start(20);
	m_thread->start();
	const int res = KProgressDialog::exec();
	m_thread->wait();
	m_timer.stop();
	return res;
}

void ConnectionTestDialog::slotTimeout()
{
//	KexiDBDbg << "ConnectionTestDialog::slotTimeout() " << m_errorObj << endl;
	bool notResponding = false;
	if (m_elapsedTime >= 1000*5) {//5 seconds
		m_stopWaiting = true;
		notResponding = true;
	}
	if (m_stopWaiting) {
		m_timer.disconnect(this);
		m_timer.stop();
		slotCancel();
//		reject();
//		close();
		if (m_errorObj) {
			m_msgHandler->showErrorMessage(m_errorObj);
			m_errorObj = 0;
		}
		else if (notResponding) {
			KMessageBox::sorry(0, 
				i18n("<qt>Test connection to <b>%1</b> database server failed. The server is not responding.</qt>")
					.arg(m_connData.serverInfoString(true)),
				i18n("Test Connection"));
		}
		else {
			KMessageBox::information(0, 
				i18n("<qt>Test connection to <b>%1</b> database server established successfully.</qt>")
					.arg(m_connData.serverInfoString(true)),
				i18n("Test Connection"));
		}
//		slotCancel();
//		reject();
		m_wait.wakeAll();
		return;
	}
	m_elapsedTime += 20;
	progressBar()->setProgress( m_elapsedTime );
}

void ConnectionTestDialog::error(KexiDB::Object *obj)
{
	KexiDBDbg << "ConnectionTestDialog::error()" << endl;
	m_stopWaiting = true;
	m_errorObj = obj;
/*		reject();
		m_msgHandler->showErrorMessage(obj);
	if (obj) {
	}
	else {
		accept();
	}*/
	m_wait.wait();
}

void ConnectionTestDialog::slotCancel()
{
//	m_wait.wakeAll();
	m_thread->terminate();
	m_timer.disconnect(this);
	m_timer.stop();
	KProgressDialog::slotCancel();
}

void KexiDB::connectionTestDialog(QWidget* parent, const KexiDB::ConnectionData& data, 
	KexiDB::MessageHandler& msgHandler)
{
	ConnectionTestDialog dlg(parent, data, msgHandler);
	dlg.exec();
}

int KexiDB::rowCount(const KexiDB::TableSchema& tableSchema)
{
//! @todo does not work with non-SQL data sources
	if (!tableSchema.connection()) {
		KexiDBWarn << "KexiDB::rowsCount(const KexiDB::TableSchema&): no tableSchema.connection() !" << endl;
		return -1;
	}
	int count = -1; //will be changed only on success of querySingleNumber()
	tableSchema.connection()->querySingleNumber(
		QString::fromLatin1("SELECT COUNT() FROM ") 
		+ tableSchema.connection()->driver()->escapeIdentifier(tableSchema.name()), 
		count
	);
	return count;
}

int KexiDB::rowCount(KexiDB::QuerySchema& querySchema)
{
//! @todo does not work with non-SQL data sources
	if (!querySchema.connection()) {
		KexiDBWarn << "KexiDB::rowsCount(const KexiDB::QuerySchema&): no querySchema.connection() !" << endl;
		return -1;
	}
	int count = -1; //will be changed only on success of querySingleNumber()
	querySchema.connection()->querySingleNumber(
		QString::fromLatin1("SELECT COUNT() FROM (") 
		+ querySchema.connection()->selectStatement(querySchema) + ")",
		count
	);
	return count;
}

int KexiDB::rowCount(KexiDB::TableOrQuerySchema& tableOrQuery)
{
	if (tableOrQuery.table())
		return rowCount( *tableOrQuery.table() );
	if (tableOrQuery.query())
		return rowCount( *tableOrQuery.query() );
	return -1;
}

int KexiDB::fieldCount(KexiDB::TableOrQuerySchema& tableOrQuery)
{
	if (tableOrQuery.table())
		return tableOrQuery.table()->fieldCount();
	if (tableOrQuery.query())
		return tableOrQuery.query()->fieldsExpanded().count();
	return -1;
}

QMap<QString,QString> KexiDB::toMap( const ConnectionData& data )
{
	QMap<QString,QString> m;
	m["caption"] = data.caption;
	m["description"] = data.description;
	m["driverName"] = data.driverName;
	m["hostName"] = data.hostName;
	m["port"] = QString::number(data.port);
	m["useLocalSocketFile"] = QString::number((int)data.useLocalSocketFile);
	m["localSocketFileName"] = data.localSocketFileName;
	m["password"] = data.password;
	m["savePassword"] = QString::number((int)data.savePassword);
	m["userName"] = data.userName;
	m["fileName"] = data.fileName();
	return m;
}

void KexiDB::fromMap( const QMap<QString,QString>& map, ConnectionData& data )
{
	data.caption = map["caption"];
	data.description = map["description"];
	data.driverName = map["driverName"];
	data.hostName = map["hostName"];
	data.port = map["port"].toInt();
	data.useLocalSocketFile = map["useLocalSocketFile"].toInt()==1;
	data.localSocketFileName = map["localSocketFileName"];
	data.password = map["password"];
	data.savePassword = map["savePassword"].toInt()==1;
	data.userName = map["userName"];
	data.setFileName(map["fileName"]);
}

bool KexiDB::splitToTableAndFieldParts(const QString& string, 
	QString& tableName, QString& fieldName,
	SetFieldNameIfNoTableNameOptions option)
{
	const int id = string.find('.');
	if (option & SetFieldNameIfNoTableName && id==-1) {
		tableName = QString::null;
		fieldName = string;
		return true;
	}
	if (id<=0 || id==int(string.length()-1))
		return false;
	tableName = string.left(id);
	fieldName = string.mid(id+1);
	return true;
}

bool KexiDB::supportsVisibleDecimalPlacesProperty(Field::Type type)
{
//! @todo add check for decimal type as well
	return Field::isFPNumericType(type);
}

QString KexiDB::formatNumberForVisibleDecimalPlaces(double value, int decimalPlaces)
{
//! @todo round?
	if (decimalPlaces < 0) {
		QString s = QString::number(value, 'f', 10 /*reasonable precision*/);
		uint i = s.length()-1;
		while (i>0 && s[i]=='0')
			i--;
		if (s[i]=='.') //remove '.'
			i--;
		s = s.left(i+1).replace('.', KGlobal::locale()->decimalSymbol());
		return s;
	}
	if (decimalPlaces == 0)
		return QString::number((int)value);
	return KGlobal::locale()->formatNumber(value, decimalPlaces);
}

#include <ktabwidget.h>
#include <klistview.h>
#include <qheader.h>

static KTabWidget* kexidbDebugWindow = 0;
static KListView* kexidbCursorDebugPage = 0;

#ifdef KEXI_DEBUG_GUI
QWidget *KexiDB::createDebugWindow(QWidget *parent)
{
	// (this is internal code - do not use i18n() here)
	kexidbDebugWindow = new KTabWidget(parent, "kexidbDebugWindow", Qt::WType_Dialog|Qt::WStyle_MinMax);
	kexidbCursorDebugPage = new KListView(kexidbDebugWindow, "kexidbCursorDebugPage");
	kexidbCursorDebugPage->addColumn("");
	kexidbCursorDebugPage->header()->hide();
	kexidbCursorDebugPage->setSorting(-1);
	kexidbCursorDebugPage->setAllColumnsShowFocus ( true );
	kexidbCursorDebugPage->setColumnWidthMode( 0, QListView::Maximum );
	kexidbDebugWindow->addTab( kexidbCursorDebugPage, "DB Cursors" );
	kexidbDebugWindow->resize(900, 600);

	kexidbDebugWindow->setIcon( DesktopIcon("info") );
	kexidbDebugWindow->setCaption("Kexi Internal Debugger");
	kexidbDebugWindow->show();
	return kexidbDebugWindow;
}

void KexiDB::addCursorDebug(const QString& text)
{
	if (!kexidbCursorDebugPage)
		return;
	KListViewItem * li = new KListViewItem( kexidbCursorDebugPage, kexidbCursorDebugPage->lastItem(), text );
	li->setMultiLinesEnabled( true );
}
#endif //KEXI_DEBUG_GUI

#include "utils_p.moc"
