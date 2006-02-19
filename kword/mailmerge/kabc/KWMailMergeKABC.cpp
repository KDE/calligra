/*
   This file is part of the KDE project
   Copyright (C) 2003 Tobias Koenig <tokoe@kde.org>
                      Joseph Wenninger <jowenn@kde.org>
                      Ingo Kloecker <kloecker@kde.org>
   Copyright (C) 2004 Tobias Koenig <tokoe@kde.org>
                      Joseph Wenninger <jowenn@kde.org>
                      Ingo Kloecker <kloecker@kde.org>
                      Dirk Schmidt <fs@dirk-schmidt.net>
 
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

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kabc/distributionlist.h>
#include <kabc/stdaddressbook.h>

#include "KWMailMergeKABC.h"
#include "KWMailMergeKABCConfig.h"

KWMailMergeKABC::KWMailMergeKABC( KInstance *inst, QObject *parent )
        : KWMailMergeDataSource( inst, parent )
{
    _addressBook = KABC::StdAddressBook::self();
    _iterator = _addressBook->begin();


    // init record list
    // Using names from kaddressbook.
    sampleRecord[ ("KAddressbook identifier") ] = KABC::Addressee::uidLabel();
    sampleRecord[ ("Name" ) ] = KABC::Addressee::nameLabel();
    sampleRecord[ ("Formatted name" ) ] = KABC::Addressee::formattedNameLabel();
    sampleRecord[ ("Family names" ) ] = KABC::Addressee::familyNameLabel();
    sampleRecord[ ("Given name" ) ] = KABC::Addressee::givenNameLabel();
    sampleRecord[ ("Additional names" ) ] = KABC::Addressee::additionalNameLabel();
    sampleRecord[ ("Honorific prefixes" ) ] = KABC::Addressee::prefixLabel();
    sampleRecord[ ("Honorific suffixes" ) ] = KABC::Addressee::suffixLabel();
    sampleRecord[ ("Nick name" ) ] = KABC::Addressee::nickNameLabel();
    sampleRecord[ ("Birthday" ) ] = KABC::Addressee::birthdayLabel();
    sampleRecord[ ("Home address: Street" ) ] = KABC::Addressee::homeAddressStreetLabel();
    sampleRecord[ ("Home address: Locality" ) ] = KABC::Addressee::homeAddressLocalityLabel();
    sampleRecord[ ("Home address: Region" ) ] = KABC::Addressee::homeAddressRegionLabel();
    sampleRecord[ ("Home address: Postal code" ) ] = KABC::Addressee::homeAddressPostalCodeLabel();
    sampleRecord[ ("Home address: Country" ) ] = KABC::Addressee::homeAddressCountryLabel();
    sampleRecord[ ("Home address: Label" ) ] = KABC::Addressee::homeAddressLabelLabel();
    sampleRecord[ ("Business address: Street" ) ] = KABC::Addressee::businessAddressStreetLabel();
    sampleRecord[ ("Business address: Locality" ) ] = KABC::Addressee::businessAddressLocalityLabel();
    sampleRecord[ ("Business address: Region" ) ] = KABC::Addressee::businessAddressRegionLabel();
    sampleRecord[ ("Business address: Postal code" ) ] = KABC::Addressee::businessAddressPostalCodeLabel();
    sampleRecord[ ("Business address: Country" ) ] = KABC::Addressee::businessAddressCountryLabel();
    sampleRecord[ ("Business address: Label" ) ] = KABC::Addressee::businessAddressLabelLabel();
    sampleRecord[ ("Home phone" ) ] = KABC::Addressee::homePhoneLabel();
    sampleRecord[ ("Business phone" ) ] = KABC::Addressee::businessPhoneLabel();
    sampleRecord[ ("Mobile phone" ) ] = KABC::Addressee::mobilePhoneLabel();
    sampleRecord[ ("Home fax" ) ] = KABC::Addressee::homeFaxLabel();
    sampleRecord[ ("Business fax" ) ] = KABC::Addressee::businessFaxLabel();
    sampleRecord[ ("Car phone" ) ] = KABC::Addressee::carPhoneLabel();
    sampleRecord[ ("ISDN" ) ] = KABC::Addressee::isdnLabel();
    sampleRecord[ ("Pager" ) ] = KABC::Addressee::pagerLabel();
    sampleRecord[ ("Email" ) ] = KABC::Addressee::emailLabel();
    sampleRecord[ ("Mailer" ) ] = KABC::Addressee::mailerLabel();
    sampleRecord[ ("Time zone" ) ] = KABC::Addressee::timeZoneLabel();
    sampleRecord[ ("Geographic position" ) ] = KABC::Addressee::geoLabel();
    sampleRecord[ ("Title" ) ] = KABC::Addressee::titleLabel();
    sampleRecord[ ("Role" ) ] = KABC::Addressee::roleLabel();
    sampleRecord[ ("Organization" ) ] = KABC::Addressee::organizationLabel();
    sampleRecord[ ("Note" ) ] = KABC::Addressee::noteLabel();
    sampleRecord[ ("productId" ) ] = KABC::Addressee::productIdLabel();
    sampleRecord[ ("Revision" ) ] = KABC::Addressee::revisionLabel();
    sampleRecord[ ("sortString" ) ] = KABC::Addressee::sortStringLabel();
    sampleRecord[ ("URL" ) ] = KABC::Addressee::urlLabel();
    sampleRecord[ ("Secrecy" ) ] = KABC::Addressee::secrecyLabel();
    sampleRecord[ ("Preferred address: Street" ) ] = QString( "preferedAddressStreet" );
    sampleRecord[ ("Preferred address: Locality" ) ] = QString( "preferedAddressLocality" );
    sampleRecord[ ("Preferred address: Region" ) ] = QString( "preferedAddressRegion" );
    sampleRecord[ ("Preferred address: Postal code" ) ] = QString( "preferedAddressPostalCode" );
    sampleRecord[ ("Preferred address: Country" ) ] = QString( "preferedAddressCountry" );
    sampleRecord[ ("Preferred address: Label" ) ] = QString( "preferedAddressLabel" );
}


KWMailMergeKABC::~KWMailMergeKABC()
{
    ;
}


void KWMailMergeKABC::addEntry( const QString &uid )
{
    _individualUIDs.append( uid );
    makeUIDsExclusive();
}


void KWMailMergeKABC::addList( const QString &id )
{
    _lists.append( id );
    parseList( id );
    makeUIDsExclusive();
}


void KWMailMergeKABC::clear()
{
    _exclusiveUIDs.clear();
    _individualUIDs.clear();
    _listUIDs.clear();
    _lists.clear();
}


int KWMailMergeKABC::getNumRecords() const
{
    kdDebug() << "KWMailMergeKABC::getNumRecords(): " << _exclusiveUIDs.count() << endl;
    return _exclusiveUIDs.count();
}


QString KWMailMergeKABC::getValue( const QString &name, int record ) const
{
    kdDebug() << "KWMailMergeKABC::getValue(" << name << ", " << record << ")" << endl;
    if ( record < 0 )
        return name;

    // This doesn't ever happen, right? So why is it there? Dirk Schmidt
    if ( record == -1 && _iterator == _addressBook->end() )
        return "";

    //
    // Set the iterator to the asked Addressee.
    //
    bool uidAvailable = false;
    if ( record != -1 )
    {
        int counter = 0;

        for ( _UIDIterator = _exclusiveUIDs.begin(); _UIDIterator != _exclusiveUIDs.end()
                && counter < record; _UIDIterator++ )
        {
            counter++;
        }

        for ( _iterator = _addressBook->begin(); _iterator != _addressBook->end(); ++_iterator )
        {

            if( _iterator->uid() == *_UIDIterator )
            {
                uidAvailable = true;
                break;
            }
        }
    }

    if( !uidAvailable )
    {
        return ( i18n ( "KAddressbook entry '%1' not available." ).arg( *_UIDIterator ) );
    }


    KABC::Addressee addr = *_iterator;
    _iterator++; // Don't know why. Could be removed? Dirk Schmidt


    //
    // Return the asked variable.
    //
    if ( name == "KAddressbook identifier" )
        return addr.uid();
    if ( name == "Name" )
        return addr.name();
    if ( name == "Formatted name" )
        return addr.formattedName();
    if ( name == "Family names" )
        return addr.familyName();
    if ( name == "Given name" )
        return addr.givenName();
    if ( name == "Additional names" )
        return addr.additionalName();
    if ( name == "Honorific prefixes" )
        return addr.prefix();
    if ( name == "Honorific suffixes" )
        return addr.suffix();
    if ( name == "Nick name" )
        return addr.nickName();
    if ( name == "Birthday" )
        return KGlobal::locale()->formatDate( addr.birthday().date() );

    if ( name == "Home address: Street" )
    {
        KABC::Address a = addr.address( KABC::Address::Home );
        return a.street();
    }
    if ( name == "Home address: Locality" )
    {
        KABC::Address a = addr.address( KABC::Address::Home );
        return a.locality();
    }
    if ( name == "Home address: Region" )
    {
        KABC::Address a = addr.address( KABC::Address::Home );
        return a.region();
    }
    if ( name == "Home address: Postal code" )
    {
        KABC::Address a = addr.address( KABC::Address::Home );
        return a.postalCode();
    }
    if ( name == "Home address: Country" )
    {
        KABC::Address a = addr.address( KABC::Address::Home );
        return a.country();
    }
    if ( name == "Home address: Label" )
    {
        KABC::Address a = addr.address( KABC::Address::Home );
        return a.label();
    }

    if ( name == "Business address: Street" )
    {
        KABC::Address a = addr.address( KABC::Address::Work );
        return a.street();
    }
    if ( name == "Business address: Locality" )
    {
        KABC::Address a = addr.address( KABC::Address::Work );
        return a.locality();
    }
    if ( name == "Business address: Region" )
    {
        KABC::Address a = addr.address( KABC::Address::Work );
        return a.region();
    }
    if ( name == "Business address: Postal code" )
    {
        KABC::Address a = addr.address( KABC::Address::Work );
        return a.postalCode();
    }
    if ( name == "Business address: Country" )
    {
        KABC::Address a = addr.address( KABC::Address::Work );
        return a.country();
    }
    if ( name == "Business address: Label" )
    {
        KABC::Address a = addr.address( KABC::Address::Work );
        return a.label();
    }

    if ( name == "Prefered address: Street" )
    {
        KABC::Address a = addr.address( KABC::Address::Pref );
        return a.street();
    }

    if ( name == "Prefered address: Locality" )
    {
        KABC::Address a = addr.address( KABC::Address::Pref );
        return a.locality();
    }
    if ( name == "Prefered address: Region" )
    {
        KABC::Address a = addr.address( KABC::Address::Pref );
        return a.region();
    }
    if ( name == "Prefered address: Postal code" )
    {
        KABC::Address a = addr.address( KABC::Address::Pref );
        return a.postalCode();
    }
    if ( name == "Prefered address: Country" )
    {
        KABC::Address a = addr.address( KABC::Address::Pref );
        return a.country();
    }
    if ( name == "Prefered address: Label" )
    {
        KABC::Address a = addr.address( KABC::Address::Pref );
        return a.label();
    }

    if ( name == "Home phone" )
    {
        KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Home );
        return phone.number();
    }
    if ( name == "Business phone" )
    {
        KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Work );
        return phone.number();
    }
    if ( name == "Mobile phone" )
    {
        KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Cell );
        return phone.number();
    }
    if ( name == "Home fax" )
    {
        KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Home | KABC::PhoneNumber::Fax );
        return phone.number();
    }
    if ( name == "Business fax" )
    {
        KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Work | KABC::PhoneNumber::Fax );
        return phone.number();
    }
    if ( name == "Car phone" )
    {
        KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Car );
        return phone.number();
    }
    if ( name == "ISDN" )
    {
        KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Isdn );
        return phone.number();
    }
    if ( name == "Pager" )
    {
        KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Pager );
        return phone.number();
    }

    if ( name == "Email" )
        return addr.preferredEmail();
    if ( name == "Mailer" )
        return addr.mailer();
    if ( name == "Time zone" )
    {
        KABC::TimeZone zone = addr.timeZone();
        return QString::number( zone.offset() );
    }
    if ( name == "Geographic position" )
    {
        KABC::Geo geo = addr.geo();
        QString lat;
        QString longi;
        if(  geo.latitude()<0 )
            lat = QString( i18n("%1 South") ).arg( -geo.latitude() );
        else
            lat = QString( i18n("%1 North") ).arg( geo.latitude() );

        if(  geo.longitude()<0 )
            // There is something going wrong, because "W" is replaced by "q ".
            // Needs fix.
            longi = QString( i18n("%1 West") ).arg( -geo.longitude() );
        else
            longi = QString( i18n("%1 East") ).arg( geo.longitude() );

        return i18n( "Geographic coordinates", "%1, %2" ).arg ( lat, longi );
    }

    if ( name == "Title" )
        return addr.title();
    if ( name == "Role" )
        return addr.role();
    if ( name == "Organization" )
        return addr.organization();
    if ( name == "Note" )
        return addr.note();
    if ( name == "productId" )
        return addr.productId();
    if ( name == "Revision" )
        return KGlobal::locale()->formatDate( addr.revision().date() );
    if ( name == "sortString" )
        return addr.sortString();
    if ( name == "URL" )
        return addr.url().url();
    if ( name == "Secrecy" )
    {
        KABC::Secrecy secrecy = addr.secrecy();
        return KABC::Secrecy::typeLabel( secrecy.type() );
    }

    return ( i18n("Unkown mail merge variable: %1").arg ( name ) ) ;
}


QStringList KWMailMergeKABC::lists() const
{
    return _lists;
}


void KWMailMergeKABC::load( QDomElement& parentElem )
{
    clear();
    QDomNode contentNode=parentElem.namedItem("CONTENT");
    if( contentNode.isNull() )
        return;
    for( QDomNode rec=contentNode.firstChild(); !rec.isNull(); rec=rec.nextSibling() )
    {
        if( rec.nodeName()== "RECORD" )
        {
            for( QDomElement recEnt=rec.firstChild().toElement(); !recEnt.isNull();
                    recEnt=recEnt.nextSibling().toElement() )
            {
                addEntry( recEnt.attribute( QString::fromLatin1("uid") ) );
            }
        }
        else if( rec.nodeName() == "LIST" )
        {
            for( QDomElement recEnt=rec.firstChild().toElement(); !recEnt.isNull();
                    recEnt=recEnt.nextSibling().toElement() )
            {
                addList( recEnt.attribute( QString::fromLatin1("listid") ) );
            }
        }
        else
            kdDebug() << "rec.nodeName(): " << rec.nodeName() << endl;
    }
}


void KWMailMergeKABC::makeUIDsExclusive()
{
    _exclusiveUIDs = _individualUIDs + _listUIDs;
    _exclusiveUIDs.sort();
    kdDebug() << "KWMailMergeKABC::makeUIDsExclusive(): before: " << _exclusiveUIDs.join(",")
    << endl;
    QString uid;
    for( QStringList::Iterator it=_exclusiveUIDs.begin();
            it!=_exclusiveUIDs.end(); ++it )
    {
        if( *it == uid )
        {
            it = _exclusiveUIDs.remove( it );
        }
        uid = *it;
    }
    kdDebug() << "KWMailMergeKABC::makeUIDsExclusive(): after: " << _exclusiveUIDs.join(",")
    << endl;
}


void KWMailMergeKABC::parseList( const QString& listName )
{
    if( listName.isEmpty() )
        return;

    kdDebug() << "KWMailMergeKABC::parseList: " << listName << endl;
    KABC::DistributionListManager dlm ( _addressBook );
    dlm.load();

    QStringList::Iterator listIt;

    KABC::DistributionList* list = dlm.list( listName );
    KABC::DistributionList::Entry::List entries = list->entries();

    KABC::DistributionList::Entry::List::Iterator itemIt;
    for ( itemIt = entries.begin(); itemIt != entries.end(); ++itemIt )
    {
        kdDebug() << "WMailMergeKABC::parseList: Listentry UID: " <<
        (*itemIt).addressee.uid() << endl;
        _listUIDs.append( (*itemIt).addressee.uid() );
    }
}


void KWMailMergeKABC::refresh( bool )
{
    kdDebug() << "KWMailMergeKABC::refresh()" << endl;
    _iterator = _addressBook->begin();
    _UIDIterator = _individualUIDs.begin();

}


void KWMailMergeKABC::save( QDomDocument& doc, QDomElement& parent)
{
    QDomElement cont=doc.createElement(QString::fromLatin1("CONTENT"));
    parent.appendChild(cont);

    QValueList<QString>::ConstIterator it = _individualUIDs.begin();
    for( ; it != _individualUIDs.end(); ++it )
    {
        QDomElement rec=doc.createElement(QString::fromLatin1("RECORD"));
        cont.appendChild(rec);
        QDomElement recEnt=doc.createElement(QString::fromLatin1("ITEM"));
        recEnt.setAttribute(QString::fromLatin1("uid"),*it);
        rec.appendChild(recEnt);
    }

    it = _lists.begin();
    for( ; !(it == _lists.end()); ++it )
    {
        QDomElement rec=doc.createElement(QString::fromLatin1("LIST"));
        cont.appendChild(rec);
        QDomElement recEnt=doc.createElement(QString::fromLatin1("ITEM"));
        recEnt.setAttribute(QString::fromLatin1("listid"),*it);
        rec.appendChild(recEnt);
    }
}

bool KWMailMergeKABC::showConfigDialog( QWidget* par, int action )
{
    bool ret=false;
    if (action == KWSLCreate )
    {
        clear();
    }

    //if (action==KWSLOpen)
    {
        KWMailMergeKABCConfig *dia=new KWMailMergeKABCConfig( par, this );

        ret=( dia->exec() == QDialog::Accepted );
        kdDebug() << "KWMailMergeKABCConfig::Accepted " << ret << endl;
        delete dia;
    }
    refresh(false);

    return ret;
}


QStringList KWMailMergeKABC::singleRecords() const
{
    return _individualUIDs;
}



extern "C"
{
    KWORD_MAILMERGE_EXPORT KWMailMergeDataSource *create_kwmailmerge_kabc( KInstance *inst, QObject *parent )
    {
        return new KWMailMergeKABC( inst, parent );
    }
}




#include "KWMailMergeKABC.moc"


