//
// C++ Interface: keximiniconnlist
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KEXIKEXIMINICONNLIST_H
#define KEXIKEXIMINICONNLIST_H

#include <klistview.h>
#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <core/kexidbconnectionset.h>
#include <core/kexi.h>

namespace Kexi {
class KexiMiniConnList;

/**
@author Adam Pigg
*/

class KexiMiniConnListItem : public KListViewItem
{
public:
    KexiMiniConnListItem(KexiMiniConnList* mcl, KexiDB::ConnectionData* d);
    ~KexiMiniConnListItem();
    KexiDB::ConnectionData* connectionData();
    
private:
    KexiDB::ConnectionData *conndata;
};

//=====================================================

class KexiMiniConnList : public KListView
{
public:
    KexiMiniConnList(QWidget *parent);

    ~KexiMiniConnList();
    KexiDB::ConnectionData* selectedConnection();
};

};

#endif
