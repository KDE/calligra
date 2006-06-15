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


#include <q3vbox.h>
#include <QLayout>
#include <QLineEdit>
//Added by qt3to4:
#include <Q3VBoxLayout>

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
#include <ktoolinvocation.h>

#include "addresspicker.h"
#include "KWMailMergeKABC.h"
#include "KWMailMergeKABCConfig.h"


KWMailMergeKABCConfig::KWMailMergeKABCConfig( QWidget *parent, KWMailMergeKABC *db_)
        :KDialog( parent)
{
    setCaption( i18n( "Mail Merge - Editor" ) );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    _db = db_;
    QWidget*page = new QWidget( this );
    setMainWidget( page );
    (new Q3VBoxLayout(page))->setAutoAdd(true);
    setMainWidget( _ui=new AddressPickerUI( page ) );

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

    Q3ListViewItem* top = _ui->mSelectedView->firstChild();
    while(top)
    {
        kDebug() << "acceptSelection(): " << top->text(0) << endl;
        if( top->text(0) == i18n("Distribution Lists") )
        {
            Q3ListViewItem* item = top->firstChild();
            while(item)
            {
                kDebug() << "acceptSelection(): " << item->text(0) << endl;
                _db->addList( item->text(0) );
                item = item->nextSibling();
            }
        }
        else if( top->text(0) == i18n("Single Entries") )
        {
            Q3ListViewItem* item = top->firstChild();
            while(item)
            {
                kDebug() << "acceptSelection(): " << item->text(0) << endl;
                _db->addEntry( item->text(-1) );
                item = item->nextSibling();
            }
        }
        top = top->nextSibling();
    }

}


void KWMailMergeKABCConfig::addSelectedContacts()
{
    Q3ListViewItemIterator it( _ui->mAvailableView, Q3ListViewItemIterator::Selected  );
    Q3ListViewItem* selected = _ui->mSelectedView->findItem(
                                  i18n("Single Entries"), 0, Q3ListView::ExactMatch );
    Q3ListViewItem* selectedLists = _ui->mSelectedView->findItem(
                                       i18n("Distribution Lists"), 0, Q3ListView::ExactMatch );
    while ( it.current() )
    {
        if( it.current()->depth() > 0 )
        {
            QString uid = it.current()->text( -1 );
            kDebug() << "addSelectedContacts(): uid :" << uid << endl;
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

    Q3ListViewItemIterator it( _ui->mAvailableView  );

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
    kDebug() << "KWMailMergeKABCConfig::filterChanged( " << txt << " )" << endl;

    bool showAll = txt.isEmpty();

    Q3ListViewItem* category = _ui->mAvailableView->firstChild();
    while(category)
    {
        if( category->text(0)!=i18n("Distribution Lists") )
        {
            Q3ListViewItem* item = category->firstChild();
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

    Q3ListViewItem* category = _ui->mAvailableView->firstChild();
    Q3ListViewItem* selected = _ui->mSelectedView->findItem(
                                  i18n("Single Entries"), 0, Q3ListView::ExactMatch );
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

    kDebug() << "::initSelectedLists()" << lists.join(",") << endl;

    Q3ListViewItem* l = _ui->mAvailableView->findItem(
                           i18n("Distribution Lists"), 0, Q3ListView::ExactMatch );
    Q3ListViewItem* selected = _ui->mSelectedView->findItem(
                                  i18n("Distribution Lists"), 0, Q3ListView::ExactMatch );

    Q3ListViewItem* item = ( l->firstChild() );
    while( item && (lists.count()>0) )
    {
        Q3ListViewItem* nextItem = item->nextSibling();

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

    connect( _ui->mAvailableView, SIGNAL( doubleClicked( Q3ListViewItem *, const QPoint &, int ) ),
             SLOT( addSelectedContacts() ) );

    connect( _ui->mFilterEdit, SIGNAL( textChanged(const QString &) ),
             SLOT( filterChanged(const QString &) ) );
    connect( _ui->mRemoveButton, SIGNAL( clicked() ), SLOT( removeSelectedContacts() ) );
    connect( _ui->mSaveList, SIGNAL( clicked() ), SLOT( saveDistributionList() ) );
    connect( _ui->mSelectedView, SIGNAL( doubleClicked( Q3ListViewItem *, const QPoint &, int ) ),
             SLOT( removeSelectedContacts() ) );
}


void KWMailMergeKABCConfig::launchAddressbook() const
{
    KToolInvocation::startServiceByDesktopName( "kaddressbook", QString() );
}



void KWMailMergeKABCConfig::removeContact( Q3ListViewItem* item )
{
    QStringList& categories = _usedCategories;
    Q3ListViewItem* availableLists = _ui->mAvailableView->findItem(
                                        i18n("Distribution Lists"), 0, Q3ListView::ExactMatch );
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
                    Q3ListViewItem* category = new Q3ListViewItem( _ui->mAvailableView,
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
                                    Q3ListView::ExactMatch),
                                rightItem->addressee() );
                }
            }
            if( entryCategories.isEmpty() )
            {
                QString noCat = i18n("no category");
                KWMailMergeKABCConfigListItem* leftItem = new KWMailMergeKABCConfigListItem(
                            _ui->mAvailableView->findItem(
                                noCat, 0, Q3ListView::ExactMatch),
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
    Q3ListViewItemIterator it( _ui->mSelectedView, Q3ListViewItemIterator::Selected  );

    while( it.current() )
    {
        kDebug() << "removeSelectedContacts(): text: " << it.current()->text(-1) << endl;
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

    Q3ListViewItem* newListItem = new Q3ListViewItem( _ui->mSelectedView->findItem(
                                     i18n("Distribution Lists"),0 , Q3ListView::ExactMatch), listName );

    Q3ListViewItem* category = _ui->mSelectedView->firstChild();
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
                Q3ListViewItem* newItem = new Q3ListViewItem(
                                             newListItem, item->addressee().formattedName() );
                newItem->setEnabled( false );

                item = static_cast<KWMailMergeKABCConfigListItem*>( item->nextSibling() );
            }

            Q3ListViewItemIterator it ( category->firstChild() );
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
    Q3ListViewItem* noCategory = new Q3ListViewItem( _ui->mAvailableView,
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
                Q3ListViewItem* category = new Q3ListViewItem( _ui->mAvailableView, *itCat );
                categories.append( *itCat );

                KWMailMergeKABCConfigListItem* item = new KWMailMergeKABCConfigListItem(
                                                          category, *itAddr );
            }
            // Append item to existing category in listview.
            else
            {
                KWMailMergeKABCConfigListItem* item = new KWMailMergeKABCConfigListItem(
                                                          _ui->mAvailableView->findItem(
                                                              *itCat, 0, Q3ListView::ExactMatch),
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
    Q3ListViewItem* distributionListsItem = new Q3ListViewItem( _ui->mAvailableView,
                                           i18n("Distribution Lists") );

    QStringList::Iterator itDistributionLists;

    for( itDistributionLists = distributionLists.begin();
            itDistributionLists != distributionLists.end(); ++itDistributionLists )
    {
        KABC::DistributionList* list = dlm.list( *itDistributionLists );

        KABC::DistributionList::Entry::List entries = list->entries();

        Q3ListViewItem* listItem = new Q3ListViewItem( distributionListsItem,
                                  *itDistributionLists );

        KABC::DistributionList::Entry::List::Iterator itList;
        for ( itList = entries.begin(); itList != entries.end(); ++itList )
        {
            // Create a normal QListViewItem and disable it, because this is not a
            // distribution-list-editor. KAddressbook should be used instead.
            Q3ListViewItem* item = new Q3ListViewItem(
                                      listItem, (*itList).addressee.formattedName() );
            item->setEnabled( false );
        }

    }
}



KWMailMergeKABCConfigListItem::KWMailMergeKABCConfigListItem( Q3ListView *parent,
        const KABC::Addressee& addressEntry ) : Q3ListViewItem( parent )
{
    setText( 0, addressEntry.formattedName() );
    _addressEntry = addressEntry;
}

KWMailMergeKABCConfigListItem::KWMailMergeKABCConfigListItem( Q3ListViewItem *parent,
        const KABC::Addressee& addressEntry ) : Q3ListViewItem( parent )
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
        return Q3ListViewItem::text( column );
    }
}

#include "KWMailMergeKABCConfig.moc"
