/* This file is part of the KDE project
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

   $Id$

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koDocumentInfoDlg.h"
#include "koDocumentInfo.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qvbox.h>

#include <klocale.h>
#include <kconfig.h>

class KoDocumentInfoDlg::KoDocumentInfoDlgPrivate
{
public:
  KoDocumentInfoDlgPrivate()
  {
  }
  ~KoDocumentInfoDlgPrivate()
  {
  }

  KoDocumentInfo *m_info;

  QLineEdit *m_leFullName;
  QLineEdit *m_leAuthorTitle;
  QLineEdit *m_leCompany;
  QLineEdit *m_leEmail;
  QLineEdit *m_leTelephone;
  QLineEdit *m_leFax;
  QLineEdit *m_leCountry;
  QLineEdit *m_lePostalCode;
  QLineEdit *m_leCity;
  QLineEdit *m_leStreet;

  QLineEdit *m_leDocTitle;
  QMultiLineEdit *m_meAbstract;

  KConfig *m_emailCfg;
};

KoDocumentInfoDlg::KoDocumentInfoDlg( KoDocumentInfo *docInfo, QWidget *parent, const char *name )
: KDialogBase( Tabbed, i18n( "Document Information" ), Ok | Cancel, Ok, parent, name, true, true )
{
  d = new KoDocumentInfoDlgPrivate;
  d->m_info = docInfo;
  d->m_emailCfg = new KConfig( "emaildefaults", true );
  d->m_emailCfg->setGroup( "UserInfo" );

  QStringList pages = docInfo->pages();
  QStringList::ConstIterator it = pages.begin();
  QStringList::ConstIterator end = pages.end();
  for (; it != end; ++it )
  {
    KoDocumentInfoPage *pg = docInfo->page( *it );
    if ( pg->inherits( "KoDocumentInfoAuthor" ) )
      addAuthorPage( static_cast<KoDocumentInfoAuthor *>( pg ) );
    else if ( pg->inherits( "KoDocumentInfoAbout" ) )
      addAboutPage( static_cast<KoDocumentInfoAbout *>( pg ) );
  }

  adjustSize();
}

KoDocumentInfoDlg::~KoDocumentInfoDlg()
{
  delete d->m_emailCfg; 
  delete d;
}

void KoDocumentInfoDlg::addAuthorPage( KoDocumentInfoAuthor *authorInfo )
{
  QGrid *grid = addGridPage( 2, QGrid::Horizontal, i18n( "Author" ) );

  (void) new QLabel( i18n( "Full Name :" ), grid );
  d->m_leFullName = new QLineEdit( authorInfo->fullName(), grid );

  if ( authorInfo->fullName().isEmpty() )
  {
    QString name = d->m_emailCfg->readEntry( "FullName" );
    if ( !name.isEmpty() )
      d->m_leFullName->setText( name );
  }
  
  (void) new QLabel( i18n( "Title :" ), grid );
  d->m_leAuthorTitle = new QLineEdit( authorInfo->title(), grid );

  (void) new QLabel( i18n( "Company :" ), grid );
  d->m_leCompany = new QLineEdit( authorInfo->company(), grid );

  (void) new QLabel( i18n( "EMail :" ), grid );
  d->m_leEmail = new QLineEdit( authorInfo->email(), grid );

  if ( authorInfo->email().isEmpty() )
  {
    QString email = d->m_emailCfg->readEntry( "EmailAddress" );
    if ( !email.isEmpty() )
      d->m_leEmail->setText( email );
  }
  
  (void) new QLabel( i18n( "Telephone :" ), grid );
  d->m_leTelephone = new QLineEdit( authorInfo->telephone(), grid );

  (void) new QLabel( i18n( "Fax :" ), grid );
  d->m_leFax = new QLineEdit( authorInfo->fax(), grid );

  (void) new QLabel( i18n( "Country :" ), grid );
  d->m_leCountry = new QLineEdit( authorInfo->country(), grid );

  (void) new QLabel( i18n( "Postal Code :" ), grid );
  d->m_lePostalCode = new QLineEdit( authorInfo->postalCode(), grid );

  (void) new QLabel( i18n( "City :" ), grid );
  d->m_leCity = new QLineEdit( authorInfo->city(), grid );

  (void) new QLabel( i18n( "Street :" ), grid );
  d->m_leStreet = new QLineEdit( authorInfo->street(), grid );
}

void KoDocumentInfoDlg::addAboutPage( KoDocumentInfoAbout *aboutInfo )
{
  QGrid *grid = addGridPage( 2, QGrid::Horizontal, i18n( "About" ) );

  (void) new QLabel( i18n( "Title :" ), grid );
  d->m_leDocTitle = new QLineEdit( aboutInfo->title(), grid );

  (void) new QLabel( i18n( "Abstract :" ), grid );
  d->m_meAbstract = new QMultiLineEdit( grid );
  d->m_meAbstract->setText( aboutInfo->abstract() );
}

void KoDocumentInfoDlg::save()
{
  QStringList pages = d->m_info->pages();
  QStringList::ConstIterator it = pages.begin();
  QStringList::ConstIterator end = pages.end();
  for (; it != end; ++it )
  {
    KoDocumentInfoPage *pg = d->m_info->page( *it );
    if ( pg->inherits( "KoDocumentInfoAuthor" ) )
      save( static_cast<KoDocumentInfoAuthor *>( pg ) );
    else if ( pg->inherits( "KoDocumentInfoAbout" ) )
      save( static_cast<KoDocumentInfoAbout *>( pg ) );
  }
}

void KoDocumentInfoDlg::save( KoDocumentInfoAuthor *authorInfo )
{
  authorInfo->setFullName( d->m_leFullName->text() );
  authorInfo->setTitle( d->m_leAuthorTitle->text() );
  authorInfo->setCompany( d->m_leCompany->text() );
  authorInfo->setEmail( d->m_leEmail->text() );
  authorInfo->setTelephone( d->m_leTelephone->text() );
  authorInfo->setFax( d->m_leTelephone->text() );
  authorInfo->setCountry( d->m_leCountry->text() );
  authorInfo->setPostalCode( d->m_lePostalCode->text() );
  authorInfo->setCity( d->m_leCity->text() );
  authorInfo->setStreet( d->m_leStreet->text() );
}

void KoDocumentInfoDlg::save( KoDocumentInfoAbout *aboutInfo )
{
  aboutInfo->setTitle( d->m_leDocTitle->text() );
  aboutInfo->setAbstract( d->m_meAbstract->text() );
}
