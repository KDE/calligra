/*
   This file is part of the KDE project
   Copyright (C) 2004 Dirk Schmidt <fs@dirk-schmidt.net>
 
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

#ifndef _KWMAILMERGE_KABC_CONFIG_H_
#define _KWMAILMERGE_KABC_CONFIG_H_

#include <kdialogbase.h>
#include <klistview.h>
#include <kabc/stdaddressbook.h>

#include "KWMailMergeKABC.h"


class AddressPickerUI;
class KWMailMergeKABC;

class KWMailMergeKABCConfig: public KDialogBase
{
    Q_OBJECT
public:
    KWMailMergeKABCConfig( QWidget *parent, KWMailMergeKABC *db_ );
    virtual ~KWMailMergeKABCConfig();


private slots:

    /**
       Moves selected items from the left Listview to the right one.
     */
    void addSelectedContacts();

    /**
       Moves selected items from the right Listview to the left one.
     */
    void removeSelectedContacts();

    /**
       Executes KAddressbook as external application.
     */
    void launchAddressbook() const;

    /**
       Updates the parent's mail merge list from items in the right Listview.
     */
    void acceptSelection();

    /**
       Hides items in the left listview, which are not matching  @p txt.
     */
    void filterChanged( const QString& txt );

    /**
       Saves the selected single entries to a new KABC::DistributionList in KAddressbook.
     */
    void saveDistributionList();
private:
    /**
       The addresspicker widget.
     */
    AddressPickerUI *_ui;

    /**
       Store all categories used in the addressbook, to avoid some iterator cycles.
     */
    QStringList _usedCategories;

    /**
       The mail merge list.
    */
    KWMailMergeKABC *_db;

    /**
       Removes duplicates in the left QListView, when moving an item to the right.
     */
    void destroyAvailableClones( const QString& uid );

    /**
       Appends the previously selected entries to the right QListView.
     */
    void initSelectedAddressees();

    /**
      Appends the previously selected distribution lists to the right QListView.
    */
    void initSelectedLists();

    /**
       Just connects signals and slots.
     */
    void initSlotSignalConnections();

    /**
       Moves @p item from the right Listview to the left one.
       
       Called by KWMailMergeKABCConfig::removeSelectedContacts().
     */
    void removeContact( QListViewItem* item );

    /**
       Appends all KAddressbook entries in KABC::StdAddressBook::self() and all
       KABC::DistributionLists to the left QListView.
    */
    void updateAvailable();
};


class KWMailMergeKABCConfigListItem : public QListViewItem
{

public:
    KWMailMergeKABCConfigListItem( QListView *parent, const KABC::Addressee& addressEntry );
    KWMailMergeKABCConfigListItem( QListViewItem *parent, const KABC::Addressee& addressEntry );
    virtual ~KWMailMergeKABCConfigListItem();

    /**
       Returns the KABC::Addressee of a KWMailMergeKABCConfigListItem.
     */
    KABC::Addressee addressee() const;

    /**
       This is an overloaded member function of QListViewItem::text( int column ).
       It Returns the KABC::Addressee::uid(), if column is set to -1.
       Otherwise QListViewItem::text( int column ) is returned.
     */
    QString text( int column ) const;


private:
    KABC::Addressee _addressEntry;

};
#endif
