#include <qfile.h>
#include <qapplication.h>
#include <qdom.h>

#include <kmimetype.h>
#include <kdebug.h>
#include <klocale.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

#include "kexipartmanager.h"
#include "kexipartitem.h"
#include "kexipartinfo.h"
#include "kexiprojectconnectiondata.h"
#include "kexiproject.h"

KexiProject::KexiProject()
 : QObject()
{
	m_drvManager = new KexiDB::DriverManager();
	m_connData = new KexiProjectConnectionData();
	m_partManager = new KexiPart::Manager(this);
	m_partManager->lookup();
	m_connection = 0;
}

#if 0
void
KexiProject::parseCmdLineOptions()
{
	//FIXME: allow multiple files in different proceses
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if(args->count() > 0 && open(QFile::decodeName(args->arg(0))))
	{
		kdDebug() << "KexiProject::opening was nice..." << endl;
	}
	else
	{
		KexiStartupDlg *dlg = new KexiStartupDlg(m_view);
		int res = dlg->exec();
		switch(res)
		{
			case KexiStartupDlg::Cancel:
				qApp->quit();
			case KexiStartupDlg::OpenExisting:
				open(dlg->fileName());
				break;
			default:
				return;
		}
	}
}
#endif

bool
KexiProject::open(const QString &file)
{
	QFile f(file);
	if(!f.exists())
		return false;

	KMimeType::Ptr ptr = KMimeType::findByFileContent(file);
	kdDebug() << "KexiProject::open(): found mime is: " << ptr.data()->name() << endl;
	QString drivername = m_drvManager->lookupByMime(ptr.data()->name());
	kdDebug() << "KexiProject::open(): driver name: " << drivername << endl;
	if(!drivername.isNull())
	{
		kdDebug() << "KexiProject::open(): file is a living databse of engine " << drivername << endl;
		m_connData->setDriverName(drivername);
		m_connData->setDatabaseName(file);
		m_connData->setFileName(file);
	}
	else
	{
		kdDebug() << "KexiProject::open(): could be a xml :)" << endl;
		delete m_connData;
		if(!f.open(IO_ReadOnly))
		{
			m_error = i18n("Couldn't open connection file");
			return false;
		}
		QString errMsg;
		int errRow;
		int errCol;

		QDomDocument doc;
		

		if(!doc.setContent(&f, &errMsg, &errRow, &errCol))
		{
			m_error = i18n("Couldn't parse connection file: %1\nRow: %1, Col: %1").arg(errMsg).arg(errRow).arg(errCol);
			return false;
		}

		QDomElement root = doc.elementsByTagName("KexiDBConnection").item(0).toElement();

		m_connData = KexiProjectConnectionData::loadInfo(root);
		kdDebug() << "KexiProject::open(): hope stuff is up and running (" << m_connData << ")" << endl;
	}

//	f.close();
	if(!openConnection(m_connData) || !m_connection)
	{
		m_error = i18n("Couldn't connect");
		return false;
	}

	if(!m_connection->useDatabase(m_connData->dbFileName()))
	{
		m_error = i18n("Couldn't open database '%1'").arg(m_connData->databaseName());
//		/*m_error = */m_connection->debugError();
		return false;
	}

	emit dbAvailable();
	kdDebug() << "KexiProject::open(): checking project parts..." << endl;
	m_partManager->checkProject(m_connection);
	return true;
}

bool
KexiProject::openConnection(KexiProjectConnectionData *connection)
{
	KexiDB::Driver *driver = m_drvManager->driver(connection->driverName().latin1());
	if(!driver)
		return false;

	KexiDB::Connection *conn = driver->createConnection(*connection);
	if(!conn)
	{
		kdDebug() << "KexiProject::openConnection(): uuups faild " << driver->errorMsg()  << endl;
		return false;
	}

	if(driver->error())
	{
		m_error = i18n("Error while connecting to db %1").arg(driver->errorMsg());
		return false;
	}

	if(!conn->connect())
	{
		m_error = i18n("Error while connecting to db %1").arg(driver->errorMsg());
		kdDebug() << "KexiProject::openConnection() errror connecting: " << driver->errorMsg() << endl;
		return false;
	}

	m_connection = conn;
	return true;
}

bool
KexiProject::isConnected()
{
	if(m_connection && m_connection->isDatabaseUsed())
		return true;

	return false;
}

ItemList
KexiProject::items(KexiPart::Info *i)
{
	kdDebug() << "KexiProject::items()" << endl;
	ItemList list;
	if(!isConnected())
		return list;

	KexiDB::Cursor *cursor = m_connection->executeQuery("SELECT o_id, o_name, o_caption  FROM kexi__objects WHERE o_type = " + QString::number(i->projectPartID()), KexiDB::Cursor::Buffered);
	if(!cursor)
		return list;

	for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
	{
		KexiPart::Item it;
		it.setIdentifier(cursor->value(0).toInt());
		it.setMime(i->mime());
		it.setName(cursor->value(1).toString());
		it.setCaption(cursor->value(2).toString());

		list.append(it);
	}

	m_connection->deleteCursor(cursor);
	kdDebug() << "KexiProject::items(): end with cout " << list.count() << endl;
	return list;
}

KexiProject::~KexiProject()
{
}

#include "kexiproject.moc"

