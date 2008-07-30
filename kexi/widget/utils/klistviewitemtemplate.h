/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KLISTVIEWITEMTEMPLATE_H
#define KLISTVIEWITEMTEMPLATE_H

#include <k3listview.h>

//! List view item class with ability for storing additional data member
template<class type>
class KListViewItemTemplate : public K3ListViewItem
{
  public:
    KListViewItemTemplate(type _data, Q3ListView *parent)
     : K3ListViewItem(parent), data(_data) {}
    KListViewItemTemplate(type _data, Q3ListViewItem *parent)
     : K3ListViewItem(parent), data(_data) {}
    KListViewItemTemplate(type _data, Q3ListView *parent, Q3ListViewItem *after)
     : K3ListViewItem(parent, after), data(_data) {}
    KListViewItemTemplate(type _data, Q3ListViewItem *parent, Q3ListViewItem *after)
     : K3ListViewItem(parent, after), data(_data) {}
    KListViewItemTemplate(type _data, Q3ListView *parent, QString label1, QString label2=QString(), QString label3=QString(), QString label4=QString(), QString label5=QString(), QString label6=QString(), QString label7=QString(), QString label8=QString())
     : K3ListViewItem(parent, label1, label2, label3, label4, label5, label6, label7, label8), data(_data) {}
    KListViewItemTemplate(type _data, Q3ListViewItem *parent, QString label1, QString label2=QString(), QString label3=QString(), QString label4=QString(), QString label5=QString(), QString label6=QString(), QString label7=QString(), QString label8=QString())
     : K3ListViewItem(parent, label1, label2, label3, label4, label5, label6, label7, label8), data(_data) {}
    KListViewItemTemplate(type _data, Q3ListView *parent, Q3ListViewItem *after, QString label1, QString label2=QString(), QString label3=QString(), QString label4=QString(), QString label5=QString(), QString label6=QString(), QString label7=QString(), QString label8=QString())
     : K3ListViewItem(parent, after, label1, label2, label3, label4, label5, label6, label7, label8), data(_data) {}
    KListViewItemTemplate(type _data, Q3ListViewItem *parent, Q3ListViewItem *after, QString label1, QString label2=QString(), QString label3=QString(), QString label4=QString(), QString label5=QString(), QString label6=QString(), QString label7=QString(), QString label8=QString())
     : K3ListViewItem(parent, after, label1, label2, label3, label4, label5, label6, label7, label8), data(_data) {}
    
    type data;
};

#endif
