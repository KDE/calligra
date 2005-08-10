/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/utils.h>
#include <kexidb/cursor.h>
#include <kexidb/drivermanager.h>

#include <qmap.h>
#include <qthread.h>

#include <kdebug.h>
#include <klocale.h>
#include <kstaticdeleter.h>
#include <kmessagebox.h>

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
	if (dynamic_cast<Connection*>(obj)) {
		conn = dynamic_cast<Connection*>(obj);
	}
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
	if (conn && !conn->recentSQLString().isEmpty())
		details += "<p><b><nobr>" +i18n("SQL statement:") + "</nobr></b><br>" + conn->recentSQLString();
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
	if (!details.isEmpty()) {
		details += (QString("<p><b><nobr>")+i18n("Server result number:")+"</nobr></b><br>"+QString::number(serverResult));
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
	if (!conn.querySingleRecord(QString("select o_id from kexi__objects where lower(o_name)='%1' and o_type=%2")
		.arg(objName.lower()).arg(objType), data))
		return 0;
	bool ok;
	int id = data[0].toInt(&ok);
	return ok ? id : 0;
}

//-----------------------------------------

TableOrQuerySchema::TableOrQuerySchema(Connection *conn, const QCString& name, bool table)
 : m_table(table ? conn->tableSchema(QString(name)) : 0)
 , m_query(table ? 0 : conn->querySchema(QString(name)))
{
	if (table && !m_table)
		kdWarning() << "TableOrQuery(Connection *conn, const QCString& name, bool table) : no table specified!" << endl;
	if (!table && !m_query)
		kdWarning() << "TableOrQuery(Connection *conn, const QCString& name, bool table) : no query specified!" << endl;
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
	return QCString();
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

#include "utils_p.moc"
