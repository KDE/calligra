//
// C++ Implementation: keximiniconnlist
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "keximiniconnlist.h"
#include <kdebug.h>

namespace Kexi {

KexiMiniConnListItem::KexiMiniConnListItem(KexiMiniConnList* mcl, KexiDB::ConnectionData* d)
 : KListViewItem(mcl, d->driverName, d->connName, d->hostName)
{
conndata = d;
}

KexiMiniConnListItem::~KexiMiniConnListItem()
{

}

KexiDB::ConnectionData* KexiMiniConnListItem::connectionData()
{
return conndata;
}

KexiMiniConnList::KexiMiniConnList(QWidget *parent)
 : KListView(parent)
{
this->addColumn("Driver Name");
this->addColumn("Connection Name");
this->addColumn("Host");
this->setAllColumnsShowFocus(true);

//FIXME
//Add a connection, taken from keximainwindowimpl.cpp
//This is tempoary as kexi will eventually load
//connection data from config files
    KexiDB::ConnectionData *conndata;
    conndata = new KexiDB::ConnectionData();
    conndata->connName = "Local pgsql connection";
    conndata->driverName = "postgresql";
    conndata->hostName = "localhost"; // -- default //"host.net";
    conndata->userName = "";
    Kexi::connset().addConnectionData(conndata);
//End Temporary Hack

//This code looks like it will get a list of connections
    KexiDB::DriverManager manager;
    KexiDB::ConnectionData::List connlist = Kexi::connset().list();
    KexiDB::ConnectionData *data = connlist.first();
    while (data)
    {
        KexiDB::Driver::Info info = manager.driverInfo(data->driverName);
        if (!info.name.isEmpty())
        {
            new KexiMiniConnListItem(this, data);
            kdDebug() << "Added Connection" <<endl;
        }
        else
        {
            kdWarning() << "KexiConnSelector::KexiConnSelector(): no driver found for '" << data->driverName << "'!" << endl;
        }

        data=connlist.next();
    }
    
}


KexiMiniConnList::~KexiMiniConnList()
{
}

KexiDB::ConnectionData* KexiMiniConnList::selectedConnection()
{
return ((KexiMiniConnListItem*)(currentItem()))->connectionData();
}
};
