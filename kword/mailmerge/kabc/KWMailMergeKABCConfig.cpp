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


#include <qvbox.h>
#include <qlayout.h>
#include <qlineedit.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <krun.h>
#include <kabc/stdaddressbook.h>
#include <kabc/distributionlist.h>

#include "addresspicker.h"
#include "KWMailMergeKABC.h"
#include "KWMailMergeKABCConfig.h"


KWMailMergeKABCConfig::KWMailMergeKABCConfig( QWidget *parent, KWMailMergeKABC *db_)
        :KDialogBase( Plain, i18n( "Mail Merge - Editor" ),
                      Ok | Cancel, Ok, parent, "", true)
{
    _db = db_;

    (new QVBoxLayout(plainPage()))->setAutoAdd(true);
    setMainWidget( _ui=new AddressPickerUI( plainPage() ) );

    updateAvailable();
    initSelectedAddressees();
    initSelectedLists();
    initSlotSignalConnections();
}


KWMailMergeKABCConfig::~KWMailMergeKABCConfig()
{
    ;
}


void KWMailMergeKABCConfig::acceptSelection()
{
    _db->clear();

    QListViewItem* top = _ui->mSelectedView->firstChild();
    while(top)
    {
        kdDebug() << "acceptSelection(): " << top->text(0) << endl;
        if( top->text(0) == i18n("Distribution Lists") )
        {
            QListViewItem* item = top->firstChild();
            while(item)
            {
                kdDebug() << "acceptSelection(): " << item->text(0) << endl;
                _db->addList( item->text(0) );
                item = item->nextSibling();
            }
        }
        else if( top->text(0) == i18n("Single Entries") )
        {
            QListViewItem* item = top->firstChild();
            while(item)
            {
                kdDebug() << "acceptSelection(): " << item->text(0) << endl;
                _db->addEntry( item->text(-1) );
                item = item->nextSibling();
            }
        }
        top = top->nextSibling();
    }

}


void KWMailMergeKABCConfig::addSelectedContacts()
{
    QListViewItemIterator it( _ui->mAvailableView, QListViewItemIterator::Selected  );
    QListViewItem* selected = _ui->mSelectedView->findItem(
                                  i18n("Single Entries"), 0, Qt::ExactMatch );
    QListViewItem* selectedLists = _ui->mSelectedView->findItem(
                                       i18n("Distribution Lists"), 0, Qt::ExactMatch );
    while ( it.current() )
    {
        if( it.current()->depth() > 0 )
        {
            QString uid = it.current()->text( -1 );
            kdDebug() << "addSelectedContacts(): uid :" << uid << endl;
            if( !uid.isEmpty() )
            {
                KWMailMergeKABCConfigListItem *item =
                    static_cast<KWMailMergeKABCConfigListItem*> ( it.current() );
                if( selected )
                {
                    selected->insertItem( item );
                    selected->setOpen( true );
                    destroyAvailableClones( uid );
                }
            }
            else if( it.current()->parent()->text(0) == i18n("Distribution Lists") )
            {
                if( selectedLists )
                {
                    selectedLists->insertItem( it.current() );
                    selectedLists->setOpen( true );
                }
            }
        }
        ++it;
    }
    _ui->mSelectedView->selectAll( false );
}


void KWMailMergeKABCConfig::destroyAvailableClones( const QString& uid )
{
    if( uid.isEmpty() )
        return;

    QListViewItemIterator it( _ui->mAvailableView  );

    while ( it.current() )
    {
        if( it.current()->depth() > 0)
        {
            if( it.current()->text(-1)== uid )
            {
                delete it.current();
            }
        }
        ++it;
    }
}


void KWMailMergeKABCConfig::filterChanged( const QString& txt )
{
    kdDebug() << "KWMailMergeKABCConfig::filterChanged( " << txt << " )" << endl;

    bool showAll = txt.isEmpty();

    QListViewItem* category = _ui->mAvailableView->firstChild();
    while(category)
    {
        if( category->text(0)!=i18n("Distribution Lists") )
        {
            QListViewItem* item = category->firstChild();
            while(item)
            {
                if(showAll)
                {
                    item->setVisible( true );
                }
                else
                {
                    item->setVisible( item->text(0).contains( txt, false ) );
                }
                item = item->nextSibling();
            }
            category->setOpen( !showAll );
        }
        else
        {
            category->setVisible( showAll );
        }
        category = category->nextSibling();
    }
}


void KWMailMergeKABCConfig::initSelectedAddressees()
{
    QStringList records = _db->singleRecords();

    QListViewItem* category = _ui->mAvailableView->firstChild();
    QListViewItem* selected = _ui->mSelectedView->findItem(
                                  i18n("Single Entries"), 0, Qt::ExactMatch );
    while ( category && (records.count()>0) )
    {
        if( category->text(0) != i18n("Distribution Lists") )
        {
            KWMailMergeKABCConfigListItem* item =
                static_cast<KWMailMergeKABCConfigListItem*> ( category->firstChild() );
            while( item && (records.count()>0) )
            {
                // Need some temporary item, because after selected->insertItem( item )
                // the item->nextSibling() is not the one we want.
                KWMailMergeKABCConfigListItem* nextItem =
                    static_cast<KWMailMergeKABCConfigListItem*> ( item->nextSibling() );

                for( QStringList::Iterator itRecords = records.begin();
                        itRecords != records.end(); ++itRecords )
                {
                    QString uid = *itRecords;
                    if( item->text(-1) == uid )
                    {
                        selected->insertItem( item );

                        // downsize records to speed up iterations
                        itRecords = records.remove( itRecords );
                        --itRecords;

                        destroyAvailableClones( uid );
                    }
                }
                item = nextItem;
            }
        }
        category = category->nextSibling();
    }
}


void KWMailMergeKABCConfig::initSelectedLists()
{
    QStringList lists = _db->lists();

    kdDebug() << "::initSelectedLists()" << lists.join(",") << endl;

    QListViewItem* l = _ui->mAvailableView->findItem(
                           i18n("Distribution Lists"), 0, Qt::ExactMatch );
    QListViewItem* selected = _ui->mSelectedView->findItem(
                                  i18n("Distribution Lists"), 0, Qt::ExactMatch );

    QListViewItem* item = ( l->firstChild() );
    while( item && (lists.count()>0) )
    {
        QListViewItem* nextItem = item->nextSibling();

        for( QStringList::Iterator itLists = lists.begin();
                itLists != lists.end(); ++itLists )
        {
            QString id = *itLists;
            if( item->text(0) == id )
            {
                selected->insertItem( item );
                itLists = lists.remove( itLists );
                --itLists;
            }
        }
        item = nextItem;
    }
}


void KWMailMergeKABCConfig::initSlotSignalConnections()
{
    connect( this, SIGNAL( okClicked() ), SLOT( acceptSelection() ) );
    connect( _ui->mAddButton, SIGNAL( clicked() ), SLOT( addSelectedContacts() ) );
    connect( _ui->mAddressBook, SIGNAL( clicked() ), SLOT( launchAddressbook() ) );

    connect( _ui->mAvailableView, SIGNAL( doubleClicked( QListViewItem *, const QPoint &, int ) ),
             SLOT( addSelectedContacts() ) );

    connect( _ui->mFilterEdit, SIGNAL( textChanged(const QString &) ),
             SLOT( filterChanged(const QString &) ) );
    connect( _ui->mRemoveButton, SIGNAL( clicked() ), SLOT( removeSelectedContacts() ) );
    connect( _ui->mSaveList, SIGNAL( clicked() ), SLOT( saveDistributionList() ) );
    connect( _ui->mSelectedView, SIGNAL( doubleClicked( QListViewItem *, const QPoint &, int ) ),
             SLOT( removeSelectedContacts() ) );
}


void KWMailMergeKABCConfig::launchAddressbook() const
{
    kapp->startServiceByDesktopName( "kaddressbook", QString() );
}



void KWMailMergeKABCConfig::removeContact( QListViewItem* item )
{
    QStringList& categories = _usedCategories;
    QListViewItem* availableLists = _ui->mAvailableView->findItem(
                                        i18n("Distribution Lists"), 0, Qt::ExactMatch );
    if( item->depth() > 0 )
    {
        if( !item->text( -1 ).isEmpty() ) // remove selected single entry here
        {
            KWMailMergeKABCConfigListItem* rightItem =
                static_cast<KWMailMergeKABCConfigListItem*> ( item );

            QStringList entryCategories = rightItem->addressee().categories();
            for ( QStringList::Iterator itEntryCat = entryCategories.begin();
                    itEntryCat != entryCategories.end(); ++itEntryCat )
            {
                int i = categories.findIndex(*itEntryCat);
                if(  i == -1 )
                {
                    QListViewItem* category = new QListViewItem( _ui->mAvailableView,
                                              *itEntryCat );
                    categories.append( *itEntryCat );

                    KWMailMergeKABCConfigListItem* leftItem = new KWMailMergeKABCConfigListItem(
                                category, rightItem->addressee() );
                }
                else
                {
                    KWMailMergeKABCConfigListItem* leftItem = new
                            KWMailMergeKABCConfigListItem(
                                _ui->mAvailableView->findItem(
                                    *itEntryCat, 0,
                                    Qt::ExactMatch),
                                rightItem->addressee() );
                }
            }
            if( entryCategories.isEmpty() )
            {
                QString noCat = i18n("no category");
                KWMailMergeKABCConfigListItem* leftItem = new KWMailMergeKABCConfigListItem(
                            _ui->mAvailableView->findItem(
                                noCat, 0, Qt::ExactMatch),
                            rightItem->addressee() );
            }
            delete item;
        }
        else if( item->parent()->text(0) == i18n("Distribution Lists") ) // remove a list
        {
            if( availableLists )
                availableLists->insertItem( item );
        }
    }
}

void KWMailMergeKABCConfig::removeSelectedContacts()
{
    QListViewItemIterator it( _ui->mSelectedView, QListViewItemIterator::Selected  );

    while( it.current() )
    {
        kdDebug() << "removeSelectedContacts(): text: " << it.current()->text(-1) << endl;
        removeContact( it.current() );
        ++it;
    }
    _ui->mAvailableView->selectAll( false );
}


void KWMailMergeKABCConfig::saveDistributionList()
{
    KABC::DistributionListManager dlm( KABC::StdAddressBook::self() );
    dlm.load();

    bool ok = false;
    QString listName = KInputDialog::getText( i18n("New Distribution List"),
                       i18n("Please enter name:"),
                       QString::null, &ok,
                       this );
    if ( !ok || listName.isEmpty() )
        return;

    if ( dlm.list( listName ) )
    {
        KMessageBox::information( 0,
                                  i18n( "<qt>Distribution list with the given name <b>%1</b> "
                                        "already exists. Please select a different name.</qt>" )
                                  .arg( listName ) );
        return;
    }
    KABC::DistributionList *distList = new KABC::DistributionList( &dlm, listName );

    QListViewItem* newListItem = new QListViewItem( _ui->mSelectedView->findItem(
                                     i18n("Distribution Lists"),0 , Qt::ExactMatch), listName );

    QListViewItem* category = _ui->mSelectedView->firstChild();
    while(category)
    {
        if( category->text(0)==i18n("Single Entries") )
        {
            KWMailMergeKABCConfigListItem* item =
                static_cast<KWMailMergeKABCConfigListItem*> ( category->firstChild() );

            while(item)
            {
                distList->insertEntry( item->addressee() );

                KABC::Addressee addr = item->addressee();
                QString formattedName = addr.formattedName();
                QListViewItem* newItem = new QListViewItem(
                                             newListItem, item->addressee().formattedName() );
                newItem->setEnabled( false );

                item = static_cast<KWMailMergeKABCConfigListItem*>( item->nextSibling() );
            }

            QListViewItemIterator it ( category->firstChild() );
            while( it.current() )
            {
                removeContact( it.current() );
                ++it;
            }
        }
        category = category->nextSibling();
    }

    dlm.save();
    newListItem->setOpen( true );
}


void KWMailMergeKABCConfig::updateAvailable()
{
    _ui->mAvailableView->clear();
    _ui->mAvailableView->setRootIsDecorated( true );

    //
    // First append the addressees.
    //
    QListViewItem* noCategory = new QListViewItem( _ui->mAvailableView,
                                i18n("no category") );

    QStringList& categories = _usedCategories ;
    categories.clear();

    KABC::AddressBook *addressBook = KABC::StdAddressBook::self();
    for( KABC::AddressBook::Iterator itAddr = addressBook->begin();
            itAddr != addressBook->end(); ++itAddr )
    {

        QStringList entryCategories = itAddr->categories();
        for ( QStringList::Iterator itCat = entryCategories.begin();
                itCat != entryCategories.end(); ++itCat )
        {
            int i = categories.findIndex(*itCat);

            // Create category, if not yet in listview and append item to it.
            if(  i == -1 )
            {
                QListViewItem* category = new QListViewItem( _ui->mAvailableView, *itCat );
                categories.append( *itCat );

                KWMailMergeKABCConfigListItem* item = new KWMailMergeKABCConfigListItem(
                                                          category, *itAddr );
            }
            // Append item to existing category in listview.
            else
            {
                KWMailMergeKABCConfigListItem* item = new KWMailMergeKABCConfigListItem(
                                                          _ui->mAvailableView->findItem(
                                                              *itCat, 0, Qt::ExactMatch),
                                                          *itAddr );
            }

        }
        // If Addressee does not belong to any category, append it to "no category".
        if( entryCategories.isEmpty() )
        {
            KWMailMergeKABCConfigListItem* item = new KWMailMergeKABCConfigListItem(
                                                      noCategory, *itAddr );
        }
    }

    //
    // Now append the distribution lists
    //
    KABC::DistributionListManager dlm ( addressBook );
    dlm.load();

    QStringList distributionLists = dlm.listNames();
    QListViewItem* distributionListsItem = new QListViewItem( _ui->mAvailableView,
                                           i18n("Distribution Lists") );

    QStringList::Iterator itDistributionLists;

    for( itDistributionLists = distributionLists.begin();
            itDistributionLists != distributionLists.end(); ++itDistributionLists )
    {
        KABC::DistributionList* list = dlm.list( *itDistributionLists );

        KABC::DistributionList::Entry::List entries = list->entries();

        QListViewItem* listItem = new QListViewItem( distributionListsItem,
                                  *itDistributionLists );

        KABC::DistributionList::Entry::List::Iterator itList;
        for ( itList = entries.begin(); itList != entries.end(); ++itList )
        {
            // Create a normal QListViewItem and disable it, because this is not a
            // distribution-list-editor. KAddressbook should be used instead.
            QListViewItem* item = new QListViewItem(
                                      listItem, (*itList).addressee.formattedName() );
            item->setEnabled( false );
        }

    }
}



KWMailMergeKABCConfigListItem::KWMailMergeKABCConfigListItem( QListView *parent,
        const KABC::Addressee& addressEntry ) : QListViewItem( parent )
{
    setText( 0, addressEntry.formattedName() );
    _addressEntry = addressEntry;
}

KWMailMergeKABCConfigListItem::KWMailMergeKABCConfigListItem( QListViewItem *parent,
        const KABC::Addressee& addressEntry ) : QListViewItem( parent )
{
    setText( 0, addressEntry.formattedName() );
    _addressEntry = addressEntry;
}

KWMailMergeKABCConfigListItem::~KWMailMergeKABCConfigListItem()
{}

KABC::Addressee KWMailMergeKABCConfigListItem::addressee() const
{
    return _addressEntry;
}

QString KWMailMergeKABCConfigListItem::text( int column ) const
{
    if( column == -1 )
    {
        return _addressEntry.uid();
    }
    else
    {
        return QListViewItem::text( column );
    }
}

#include "KWMailMergeKABCConfig.moc"
