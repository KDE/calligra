/*
   This file is part of the KDE project
   Copyright (C) 2003 Tobias Koenig <tokoe@kde.org>
                      Joseph Wenninger <jowenn@kde.org>
                      Ingo Kloecker <kloecker@kde.org>

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

#include <kabc/stdaddressbook.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

#include "kwmailmerge_kabc.h"

KWMailMergeKABC::KWMailMergeKABC( KInstance *inst, QObject *parent )
  : KWMailMergeDataSource( inst, parent )
{
  mAddressBook = KABC::StdAddressBook::self();
  mIterator = mAddressBook->begin();

  // init record list
  sampleRecord[ "uid" ] = KABC::Addressee::uidLabel();
  sampleRecord[ "name" ] = KABC::Addressee::nameLabel();
  sampleRecord[ "formattedName" ] = KABC::Addressee::formattedNameLabel();
  sampleRecord[ "familyName" ] = KABC::Addressee::familyNameLabel();
  sampleRecord[ "givenName" ] = KABC::Addressee::givenNameLabel();
  sampleRecord[ "additionalName" ] = KABC::Addressee::additionalNameLabel();
  sampleRecord[ "prefix" ] = KABC::Addressee::prefixLabel();
  sampleRecord[ "suffix" ] = KABC::Addressee::suffixLabel();
  sampleRecord[ "nickName" ] = KABC::Addressee::nickNameLabel();
  sampleRecord[ "birthday" ] = KABC::Addressee::birthdayLabel();
  sampleRecord[ "homeAddressStreet" ] = KABC::Addressee::homeAddressStreetLabel();
  sampleRecord[ "homeAddressLocality" ] = KABC::Addressee::homeAddressLocalityLabel();
  sampleRecord[ "homeAddressRegion" ] = KABC::Addressee::homeAddressRegionLabel();
  sampleRecord[ "homeAddressPostalCode" ] = KABC::Addressee::homeAddressPostalCodeLabel();
  sampleRecord[ "homeAddressCountry" ] = KABC::Addressee::homeAddressCountryLabel();
  sampleRecord[ "homeAddressLabel" ] = KABC::Addressee::homeAddressLabelLabel();
  sampleRecord[ "businessAddressStreet" ] = KABC::Addressee::businessAddressStreetLabel();
  sampleRecord[ "businessAddressLocality" ] = KABC::Addressee::businessAddressLocalityLabel();
  sampleRecord[ "businessAddressRegion" ] = KABC::Addressee::businessAddressRegionLabel();
  sampleRecord[ "businessAddressPostalCode" ] = KABC::Addressee::businessAddressPostalCodeLabel();
  sampleRecord[ "businessAddressCountry" ] = KABC::Addressee::businessAddressCountryLabel();
  sampleRecord[ "businessAddressLabel" ] = KABC::Addressee::businessAddressLabelLabel();
  sampleRecord[ "homePhone" ] = KABC::Addressee::homePhoneLabel();
  sampleRecord[ "businessPhone" ] = KABC::Addressee::businessPhoneLabel();
  sampleRecord[ "mobilePhone" ] = KABC::Addressee::mobilePhoneLabel();
  sampleRecord[ "homeFax" ] = KABC::Addressee::homeFaxLabel();
  sampleRecord[ "businessFax" ] = KABC::Addressee::businessFaxLabel();
  sampleRecord[ "carPhone" ] = KABC::Addressee::carPhoneLabel();
  sampleRecord[ "isdn" ] = KABC::Addressee::isdnLabel();
  sampleRecord[ "pager" ] = KABC::Addressee::pagerLabel();
  sampleRecord[ "email" ] = KABC::Addressee::emailLabel();
  sampleRecord[ "mailer" ] = KABC::Addressee::mailerLabel();
  sampleRecord[ "timeZone" ] = KABC::Addressee::timeZoneLabel();
  sampleRecord[ "geo" ] = KABC::Addressee::geoLabel();
  sampleRecord[ "title" ] = KABC::Addressee::titleLabel();
  sampleRecord[ "role" ] = KABC::Addressee::roleLabel();
  sampleRecord[ "organization" ] = KABC::Addressee::organizationLabel();
  sampleRecord[ "note" ] = KABC::Addressee::noteLabel();
  sampleRecord[ "productId" ] = KABC::Addressee::productIdLabel();
  sampleRecord[ "revision" ] = KABC::Addressee::revisionLabel();
  sampleRecord[ "sortString" ] = KABC::Addressee::sortStringLabel();
  sampleRecord[ "url" ] = KABC::Addressee::urlLabel();
  sampleRecord[ "secrecy" ] = KABC::Addressee::secrecyLabel();
}

KWMailMergeKABC::~KWMailMergeKABC()
{
}

QString KWMailMergeKABC::getValue( const QString &name, int record ) const
{
  if ( record < 0 )
    return name;

  if ( record == -1 && mIterator == mAddressBook->end() )
    return "";

  if ( record != -1 ) {
    int counter = 0;
    for ( mIterator = mAddressBook->begin(); mIterator != mAddressBook->end() && counter < record; mIterator++ )
      counter++;
  }

  KABC::Addressee addr = *mIterator;
  mIterator++;

  if ( name == "uid" )
    return addr.uid();
  if ( name == "name" )
    return addr.name();
  if ( name == "formattedName" )
    return addr.formattedName();
  if ( name == "familyName" )
    return addr.familyName();
  if ( name == "givenName" )
    return addr.givenName();
  if ( name == "additionalName" )
    return addr.additionalName();
  if ( name == "prefix" )
    return addr.prefix();
  if ( name == "suffix" )
    return addr.suffix();
  if ( name == "nickName" )
    return addr.nickName();
  if ( name == "birthday" )
    return KGlobal::locale()->formatDate( addr.birthday().date() );

  if ( name == "homeAddressStreet" ) {
    KABC::Address a = addr.address( KABC::Address::Home );
    return a.street();
  }
  if ( name == "homeAddressLocality" ) {
    KABC::Address a = addr.address( KABC::Address::Home );
    return a.locality();
  }
  if ( name == "homeAddressRegion" ) {
    KABC::Address a = addr.address( KABC::Address::Home );
    return a.region();
  }
  if ( name == "homeAddressPostalCode" ) {
    KABC::Address a = addr.address( KABC::Address::Home );
    return a.postalCode();
  }
  if ( name == "homeAddressCountry" ) {
    KABC::Address a = addr.address( KABC::Address::Home );
    return a.country();
  }
  if ( name == "homeAddressLabel" ) {
    KABC::Address a = addr.address( KABC::Address::Home );
    return a.label();
  }

  if ( name == "businessAddressStreet" ) {
    KABC::Address a = addr.address( KABC::Address::Work );
    return a.street();
  }
  if ( name == "businessAddressLocality" ) {
    KABC::Address a = addr.address( KABC::Address::Work );
    return a.locality();
  }
  if ( name == "businessAddressRegion" ) {
    KABC::Address a = addr.address( KABC::Address::Work );
    return a.region();
  }
  if ( name == "businessAddressPostalCode" ) {
    KABC::Address a = addr.address( KABC::Address::Work );
    return a.postalCode();
  }
  if ( name == "businessAddressCountry" ) {
    KABC::Address a = addr.address( KABC::Address::Work );
    return a.country();
  }
  if ( name == "businessAddressLabel" ) {
    KABC::Address a = addr.address( KABC::Address::Work );
    return a.label();
  }

  if ( name == "homePhone" ) {
    KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Home );
    return phone.number();
  }
  if ( name == "businessPhone" ) {
    KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Work );
    return phone.number();
  }
  if ( name == "mobilePhone" ) {
    KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Cell );
    return phone.number();
  }
  if ( name == "homeFax" ) {
    KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Home | KABC::PhoneNumber::Fax );
    return phone.number();
  }
  if ( name == "businessFax" ) {
    KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Work | KABC::PhoneNumber::Fax );
    return phone.number();
  }
  if ( name == "carPhone" ) {
    KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Car );
    return phone.number();
  }
  if ( name == "isdn" ) {
    KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Isdn );
    return phone.number();
  }
  if ( name == "pager" ) {
    KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Pager );
    return phone.number();
  }

  if ( name == "email" )
    return addr.preferredEmail();
  if ( name == "mailer" )
    return addr.mailer();
  if ( name == "timeZone" ) {
    KABC::TimeZone zone = addr.timeZone();
    return QString::number( zone.offset() );
  }
  if ( name == "geo" ) {
    KABC::Geo geo = addr.geo();
    return QString( "%1:%2" ).arg( geo.latitude() ).arg( geo.longitude() );
  }
  if ( name == "title" )
    return addr.title();
  if ( name == "role" )
    return addr.role();
  if ( name == "organization" )
    return addr.organization();
  if ( name == "note" )
    return addr.note();
  if ( name == "productId" )
    return addr.productId();
  if ( name == "revision" )
    return KGlobal::locale()->formatDate( addr.revision().date() );
  if ( name == "sortString" )
    return addr.sortString();
  if ( name == "url" )
    return addr.url().url();
  if ( name == "secrecy" ) {
    KABC::Secrecy secrecy = addr.secrecy();
    return KABC::Secrecy::typeLabel( secrecy.type() );
  }

  return "";
}

int KWMailMergeKABC::getNumRecords() const
{
  KABC::AddressBook::Iterator it;
  int counter = 0;
  for ( it = mAddressBook->begin(); it != mAddressBook->end(); ++it )
    counter++;

  return counter;
}

void KWMailMergeKABC::refresh( bool )
{
  mIterator = mAddressBook->begin();
}


extern "C" {
  KWMailMergeDataSource *create_kwmailmerge_kabc( KInstance *inst, QObject *parent )
  {
    return new KWMailMergeKABC( inst, parent );
  }
}





#include "kwmailmerge_kabc.moc"
