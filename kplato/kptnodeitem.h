/* This file is part of the KDE project
   Copyright (C) 2002 Bo Thorsen  bo@suse.de

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

#ifndef KPTNODEITEM_H
#define KPTNODEITEM_H

#include <klistview.h>

class KPTNode;


/**
 * This class is a listview item containing a project node.
 */

class KPTNodeItem : public KListViewItem {
public:
    KPTNodeItem(KListView *listView, KPTNode &node);
    KPTNodeItem(KPTNodeItem *parent, KPTNode &node);

    KPTNode &getNode() { return node; }
    const KPTNode &getNode() const { return node; }

private:
    KPTNode &node;
};


#endif // KPTNODEITEM_H
