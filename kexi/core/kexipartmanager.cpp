#include <klibloader.h>
#include <ktrader.h>
#include <kdebug.h>
#include <kparts/componentfactory.h>

#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexipartmanager.h"
#include "../kexi_global.h"

#include <kexidb/connection.h>
#include <kexidb/cursor.h>

namespace KexiPart
{

Manager::Manager(QObject *parent)
 : QObject(parent)
{
}

void
Manager::lookup()
{
	KTrader::OfferList tlist = KTrader::self()->query("Kexi/Handler", "[X-Kexi-PartVersion] == " + QString::number(KEXI_PART_VERSION));
	for(KTrader::OfferList::Iterator it(tlist.begin()); it != tlist.end(); ++it)
	{
		KService::Ptr ptr = (*it);
		kdDebug() << "Manager::lookup(): " << ptr->property("X-Kexi-TypeMime").toString() << endl;
		Info *info = new Info(ptr, this);
		m_parts.insert(ptr->property("X-Kexi-TypeMime").toString(), info);
		m_partlist.append(info);
	}
}

Part *
Manager::load(Info *i)
{
	kdDebug() << "Manager::load()" << endl;

	if(!i || i->broken())
		return 0;

	kdDebug() << "Manager::load()" << endl;
	Part *p = KParts::ComponentFactory::createInstanceFromService<Part>(i->ptr(), this, 0, QStringList());

	if(!p)
	{
		kdDebug() << "Manager::load(): faild :(" << endl;
		i->setBroken(true);
		return 0;
	}

	p->setInfo(i);
	kdDebug() << "Manager::load(): fine!" << endl;
	return p;
}

Part *
Manager::load(const QString &mime)
{
	return load(m_parts[mime]);
}

Info *
Manager::info(const QString &mime)
{
	return m_parts[mime];
}


void
Manager::checkProject(KexiDB::Connection *conn)
{
	if(!conn->isDatabaseUsed())
		return;

	KexiDB::Cursor *cursor = conn->executeQuery("SELECT * FROM kexi__parts", KexiDB::Cursor::Buffered);
	if(!cursor)
		return;

	for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
	{
		Info *i = info(cursor->value(2).toString());
		if(!i)
		{
			Missing m;
			m.name = cursor->value(1).toString();
			m.mime = cursor->value(2).toString();
			m.url = cursor->value(3).toString();

			m_missing.append(m);
		}
		else
		{
			i->setProjectPartID(cursor->value(0).toInt());
		}
	}

	conn->deleteCursor(cursor);

	return;
}

Manager::~Manager()
{
}

}

#include "kexipartmanager.moc"

