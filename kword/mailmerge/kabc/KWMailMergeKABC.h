/*
   This file is part of the KDE project
   Copyright (C) 2003 Tobias Koenig <tokoe@kde.org>
   Copyright (C) 2004 Tobias Koenig <tokoe@kde.org>
                      Dirk Schmidt <fs@dirk.schmidt.net>
 
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

#ifndef _KWMAILMERGE_KABC_H_
#define _KWMAILMERGE_KABC_H_

#include <qdom.h>
#include <QPointer>

#include <kabc/addressbook.h>

#include "KWMailMergeDataSource.h"

class KWMailMergeKABC: public KWMailMergeDataSource
{
    Q_OBJECT

public:
    KWMailMergeKABC( KInstance *inst, QObject *parent );
    ~KWMailMergeKABC();

    /**
       Saves the mail merge list to the kword document.
     */
    virtual void save( QDomDocument&, QDomElement& );

    /**
       Loads the mail merge list stored in the kword document.
     */
    virtual void load( QDomElement& );

    /**
       @param name	The name of the value e.g. "Family name".
       @param record	The position of the the entry in mail merge list.
       @return		The value of the mail merge variable.
       
       If @p record equals -1, @p name is returned.
     */
    virtual class QString getValue( const class QString &name, int record = -1 ) const;

    /**
       @return	The number of available contacts in mail merge list.
     */
    virtual int getNumRecords() const;

    /**
       Only for compatability reasons.
       
       @param force	Hasn't any effect.
     */
    virtual void refresh( bool force );

    /**
       Shows a KWMailMergeKABCConfig dialog for selecting entries from KAddressbook.
     */
    virtual bool showConfigDialog( QWidget*, int action);

protected:
    friend class KWMailMergeKABCConfig;

    /**
       Adds an entry from KABC::StdAddressBook::self()
       to the mail merge list.
      
       To be called by KWMailMergeKABC::load() and 
       KWMailMergeKABCConfig::acceptSelection() only.
       
       @param uid	The entry's KABC::Addressee::uid().
     */
    void addEntry( const QString &uid );

    /**
       Adds a distribution list to the mail merge list.
       
       To be called by KWMailMergeKABC::load() and 
       KWMailMergeKABCConfig::acceptSelection() only.
       
       @param id	The DistributionList::name().
     */
    void addList( const QString &id );

    /**
       Removes all entries and distribution lists from the mail merge list.
     */
    void clear();

    /**
       @return	All selected DistributionList::name().
       
       To be called by KWMailMergeKABCConfig::initSelectedLists()
     */
    virtual QStringList lists() const;

    /**
       @return	The KABC::Addressee::uid() of all individually selected 
       entries in mail merge list.
       
       To be called by KWMailMergeKABCConfig::initSelectedAddressees()
    */
    virtual QStringList singleRecords() const;

private:
    /**
       The KABC::StdAddressBook::self().
     */
    KABC::AddressBook* _addressBook;

    /**
       Just an Iterator.
     */
    mutable KABC::AddressBook::ConstIterator _iterator;

    /**
       Just an Iterator.
     */
    mutable QStringList::ConstIterator _UIDIterator;

    /**
       The "real" mail merge list. A list of QStrings. Each represents 
       the KABC::Addressee::uid() of a KAdressbook entry.
       There is no UID twice in this list.
      
       Needed because selected contacts may appear in a selected 
       distribution list, too. And we don't want to print it multiple.
     */
    QStringList _exclusiveUIDs;

    /**
       This list contains all the KABC::Addressee::uid() selected 
       individually with the KWMailMergeKABCConfig dialog.
     */
    QStringList _individualUIDs;

    /**
       This list contains all the KABC::Addressee::uid() from the distribution 
       lists selected with the KWMailMergeKABCConfig dialog.
     */
    QStringList _listUIDs;

    /**
       This list contains all the DistributionList::name() selected with the
       KWMailMergeKABCConfig dialog.
     */
    QStringList _lists;


    /**
       Appends all KABC::Addressee::uid() of a distribution list to _listUIDs
       and updates the mail merge list.
       
       To be used by KWMailMergeKABCConfig::addList( const QString &id )
       only.
       
       @param listName	The DistributionList::name() of the distribution list.
     */
    void parseList( const QString& listName );

    /**
       Removes duplicate entries in the mail merge list. 
     */
    void makeUIDsExclusive();

};

#endif

