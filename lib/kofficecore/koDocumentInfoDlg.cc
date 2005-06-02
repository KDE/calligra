/* This file is part of the KDE project
   Copyright (c) 2000 Simon Hausmann <hausmann@kde.org>

   $Id$

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

#include <koDocumentInfoDlg.h>
#include <koDocumentInfo.h>
#include <koGlobal.h>
#include <koStore.h>

#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qbuffer.h>
#include <qdom.h>
#include <qdir.h>
#include <qpushbutton.h>

#include <kabc/addressee.h>
#include <kabc/stdaddressbook.h>
#include <kdeversion.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktar.h>
#include <kdebug.h>
#include <ktempfile.h>
#include <kmimetype.h>
#include <qlayout.h>
#include <qgrid.h>
#include <kfilterdev.h>

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
  QLineEdit *m_leInitial;
  QLineEdit *m_leAuthorTitle;
  QLineEdit *m_leCompany;
  QLineEdit *m_leEmail;
  QLineEdit *m_leTelephoneWork;
  QLineEdit *m_leTelephoneHome;
  QLineEdit *m_leFax;
  QLineEdit *m_leCountry;
  QLineEdit *m_lePostalCode;
  QLineEdit *m_leCity;
  QLineEdit *m_leStreet;
  QPushButton *m_pbLoadKABC;

  QLineEdit *m_leDocTitle;
  QMultiLineEdit *m_meAbstract;
    QLineEdit *m_leDocSubject;
    QLineEdit *m_leDocKeywords;
    QLineEdit *m_leAuthorPosition;


  bool m_bDeleteDialog;
  KDialogBase *m_dialog;
};

KoDocumentInfoDlg::KoDocumentInfoDlg( KoDocumentInfo *docInfo, QWidget *parent, const char *name,
                                      KDialogBase *dialog )
: QObject( parent, "docinfodlg" )
{
  d = new KoDocumentInfoDlgPrivate;
  d->m_info = docInfo;

  d->m_dialog = dialog;
  d->m_bDeleteDialog = false;

  if ( !dialog )
  {
    d->m_dialog = new KDialogBase( KDialogBase::Tabbed,
                                   i18n( "Document Information" ),
                                   KDialogBase::Ok | KDialogBase::Cancel,
                                   KDialogBase::Ok, parent, name, true, true );
    d->m_bDeleteDialog = true;
  }

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
}

KoDocumentInfoDlg::~KoDocumentInfoDlg()
{
  if ( d->m_bDeleteDialog )
    delete d->m_dialog;

  delete d;
}

int KoDocumentInfoDlg::exec()
{
  return d->m_dialog->exec();
}

KDialogBase *KoDocumentInfoDlg::dialog() const
{
  return d->m_dialog;
}

void KoDocumentInfoDlg::loadFromKABC()
{
  KABC::StdAddressBook *ab = static_cast<KABC::StdAddressBook*>
                             ( KABC::StdAddressBook::self() );

  if ( !ab )
    return;

  KABC::Addressee addr = ab->whoAmI();
  if ( addr.isEmpty() )
  {
    KMessageBox::sorry( 0L, i18n( "No personal contact data set, please use the option \
                                  \"Set as Personal Contact Data\" from the \"Edit\" menu in KAddressbook to set one." ) );
    return;
  }

  d->m_leFullName->setText( addr.formattedName() );
  d->m_leInitial->setText( addr.givenName()[ 0 ] + ". " +
                           addr.familyName()[ 0 ] + "." );
  d->m_leAuthorTitle->setText( addr.title() );
  d->m_leCompany->setText( addr.organization() );
  d->m_leEmail->setText( addr.preferredEmail() );

  KABC::PhoneNumber phone = addr.phoneNumber( KABC::PhoneNumber::Home );
  d->m_leTelephoneHome->setText( phone.number() );
  phone = addr.phoneNumber( KABC::PhoneNumber::Work );
  d->m_leTelephoneWork->setText( phone.number() );

  phone = addr.phoneNumber( KABC::PhoneNumber::Fax );
  d->m_leFax->setText( phone.number() );

  KABC::Address a = addr.address( KABC::Address::Home );
  d->m_leCountry->setText( a.country() );
  d->m_lePostalCode->setText( a.postalCode() );
  d->m_leCity->setText( a.locality() );
  d->m_leStreet->setText( a.street() );

  emit changed();
}

void KoDocumentInfoDlg::addAuthorPage( KoDocumentInfoAuthor *authorInfo )
{
  QFrame *page = d->m_dialog->addPage( i18n( "Author" ) );
  QGridLayout *layout = new QGridLayout( page, 11, 2, KDialog::marginHint(),
                                         KDialog::spacingHint() );

  layout->addWidget( new QLabel( i18n( "Name:" ), page ), 0, 0 );
  d->m_leFullName = new QLineEdit( authorInfo->fullName(), page );
  layout->addWidget( d->m_leFullName, 0, 1 );


  layout->addWidget( new QLabel( i18n( "Initials:" ), page ), 1, 0 );
  d->m_leInitial = new QLineEdit( authorInfo->initial(), page );
  layout->addWidget( d->m_leInitial, 1, 1 );

  layout->addWidget( new QLabel( i18n( "Title:" ), page ), 2, 0 );
  d->m_leAuthorTitle = new QLineEdit( authorInfo->title(), page );
  layout->addWidget( d->m_leAuthorTitle, 2, 1 );

  layout->addWidget( new QLabel( i18n( "Position:" ), page ), 3, 0 );
  d->m_leAuthorPosition = new QLineEdit( authorInfo->position(), page );
  layout->addWidget( d->m_leAuthorPosition, 3, 1 );


  layout->addWidget( new QLabel( i18n( "Company:" ), page ), 4, 0 );
  d->m_leCompany = new QLineEdit( authorInfo->company(), page );
  layout->addWidget( d->m_leCompany, 4, 1 );


  layout->addWidget( new QLabel( i18n( "Email:" ), page ), 5, 0 );
  d->m_leEmail = new QLineEdit( authorInfo->email(), page );
  layout->addWidget( d->m_leEmail, 5, 1 );


  layout->addWidget( new QLabel( i18n( "Telephone (Home):" ), page ), 6, 0 );
  d->m_leTelephoneHome = new QLineEdit( authorInfo->telephoneHome(), page );
  layout->addWidget( d->m_leTelephoneHome, 6, 1 );

  layout->addWidget( new QLabel( i18n( "Telephone (Work):" ), page ), 7, 0 );
  d->m_leTelephoneWork = new QLineEdit( authorInfo->telephoneWork(), page );
  layout->addWidget( d->m_leTelephoneWork, 7, 1 );

  layout->addWidget( new QLabel( i18n( "Fax:" ), page ), 8, 0 );
  d->m_leFax = new QLineEdit( authorInfo->fax(), page );
  layout->addWidget( d->m_leFax, 8, 1 );

  layout->addWidget( new QLabel( i18n( "Street:" ), page ), 9, 0 );
  d->m_leStreet = new QLineEdit( authorInfo->street(), page );
  layout->addWidget( d->m_leStreet, 9, 1 );

  layout->addWidget( new QLabel( i18n( "Postal code:" ), page ), 10, 0 );
  d->m_lePostalCode = new QLineEdit( authorInfo->postalCode(), page );
  layout->addWidget( d->m_lePostalCode, 10, 1 );

  layout->addWidget( new QLabel( i18n( "City:" ), page ), 11, 0 );
  d->m_leCity = new QLineEdit( authorInfo->city(), page );
  layout->addWidget( d->m_leCity, 11, 1 );

  layout->addWidget( new QLabel( i18n( "Country:" ), page ), 12, 0 );
  d->m_leCountry = new QLineEdit( authorInfo->country(), page );
  layout->addWidget( d->m_leCountry, 12, 1 );

  d->m_pbLoadKABC = new QPushButton( i18n( "Load From Address Book" ), page );
  layout->addMultiCellWidget( d->m_pbLoadKABC, 13, 13, 0, 1 );

  connect( d->m_leFullName, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leInitial, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );

  connect( d->m_leAuthorTitle, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leCompany, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leEmail, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leTelephoneWork, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leTelephoneHome, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leFax, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leCountry, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_lePostalCode, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leCity, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leStreet, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leAuthorPosition, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_pbLoadKABC, SIGNAL( clicked() ),
           this, SLOT( loadFromKABC() ) );
}

void KoDocumentInfoDlg::addAboutPage( KoDocumentInfoAbout *aboutInfo )
{
  QFrame *page = d->m_dialog->addPage( i18n("about the document", "Document") );
  QGridLayout *grid = new QGridLayout( page, 6, 2, KDialog::marginHint(), KDialog::spacingHint() );

  grid->addWidget( new QLabel( i18n( "Title:" ), page ), 0, 0);
  d->m_leDocTitle = new QLineEdit( aboutInfo->title(), page );
  grid->addWidget(d->m_leDocTitle, 0, 1);

  grid->addWidget( new QLabel( i18n( "Subject:" ), page ), 1, 0);
  d->m_leDocSubject = new QLineEdit( aboutInfo->subject(), page );
  grid->addWidget(d->m_leDocSubject, 1, 1);

  grid->addWidget( new QLabel( i18n( "Keywords:" ), page ), 2, 0);
  d->m_leDocKeywords = new QLineEdit( aboutInfo->keywords(), page );
  grid->addWidget(d->m_leDocKeywords, 2, 1);


  grid->addWidget(new QLabel( i18n( "Abstract:" ), page ), 3, 0, Qt::AlignTop );

  d->m_meAbstract = new QMultiLineEdit( page );
  d->m_meAbstract->setText( aboutInfo->abstract() );
  grid->addMultiCellWidget(d->m_meAbstract, 3, 5, 1, 1);

  connect( d->m_leDocTitle, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_meAbstract, SIGNAL( textChanged() ),
           this, SIGNAL( changed() ) );
  connect( d->m_leDocSubject, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leDocKeywords, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
}

void KoDocumentInfoDlg::save()
{
  QStringList pages = d->m_info->pages();
  QStringList::ConstIterator it = pages.begin();
  QStringList::ConstIterator end = pages.end();
  bool saveInfo=false;
  for (; it != end; ++it )
  {
    KoDocumentInfoPage *pg = d->m_info->page( *it );
    if ( pg->inherits( "KoDocumentInfoAuthor" ) )
    {
        saveInfo=true;
        save( static_cast<KoDocumentInfoAuthor *>( pg ) );
    }
    else if ( pg->inherits( "KoDocumentInfoAbout" ) )
    {
        saveInfo=true;
        save( static_cast<KoDocumentInfoAbout *>( pg ) );
    }
  }
  if(saveInfo)
      d->m_info->documentInfochanged();
}

void KoDocumentInfoDlg::save( KoDocumentInfoAuthor *authorInfo )
{
  authorInfo->setFullName( d->m_leFullName->text() );
  authorInfo->setInitial( d->m_leInitial->text() );
  authorInfo->setTitle( d->m_leAuthorTitle->text() );
  authorInfo->setCompany( d->m_leCompany->text() );
  authorInfo->setEmail( d->m_leEmail->text() );
  authorInfo->setTelephoneWork( d->m_leTelephoneWork->text() );
  authorInfo->setTelephoneHome( d->m_leTelephoneHome->text() );
  authorInfo->setFax( d->m_leFax->text() );
  authorInfo->setCountry( d->m_leCountry->text() );
  authorInfo->setPostalCode( d->m_lePostalCode->text() );
  authorInfo->setCity( d->m_leCity->text() );
  authorInfo->setStreet( d->m_leStreet->text() );
  authorInfo->setPosition( d->m_leAuthorPosition->text() );

  KConfig* config = KoGlobal::kofficeConfig();
  KConfigGroupSaver cgs( config, "Author" );
  config->writeEntry("telephone", d->m_leTelephoneHome->text());
  config->writeEntry("telephone-work", d->m_leTelephoneWork->text());
  config->writeEntry("fax", d->m_leFax->text());
  config->writeEntry("country",d->m_leCountry->text());
  config->writeEntry("postal-code",d->m_lePostalCode->text());
  config->writeEntry("city",  d->m_leCity->text());
  config->writeEntry("street", d->m_leStreet->text());
  config->sync();
}

void KoDocumentInfoDlg::save( KoDocumentInfoAbout *aboutInfo )
{
  aboutInfo->setTitle( d->m_leDocTitle->text() );
  aboutInfo->setSubject( d->m_leDocSubject->text() );
  aboutInfo->setKeywords( d->m_leDocKeywords->text() );
  aboutInfo->setAbstract( d->m_meAbstract->text() );
}

class KoDocumentInfoPropsPage::KoDocumentInfoPropsPagePrivate
{
public:
  KoDocumentInfo *m_info;
  KoDocumentInfoDlg *m_dlg;
  KURL m_url;
  KTarGz *m_src;
  KTarGz *m_dst;

  const KTarFile *m_docInfoFile;
};

KoDocumentInfoPropsPage::KoDocumentInfoPropsPage( KPropertiesDialog *props,
                                                  const char *,
                                                  const QStringList & )
: KPropsDlgPlugin( props )
{
  d = new KoDocumentInfoPropsPagePrivate;
  d->m_info = new KoDocumentInfo( this, "docinfo" );
  d->m_url = props->item()->url();
  d->m_dlg = 0;

  if ( !d->m_url.isLocalFile() )
    return;

  d->m_dst = 0;

#ifdef __GNUC__
#warning TODO port this to KoStore !!!
#endif
  d->m_src = new KTarGz( d->m_url.path(), "application/x-gzip" );

  if ( !d->m_src->open( IO_ReadOnly ) )
    return;

  const KTarDirectory *root = d->m_src->directory();
  if ( !root )
    return;

  const KTarEntry *entry = root->entry( "documentinfo.xml" );

  if ( entry && entry->isFile() )
  {
    d->m_docInfoFile = static_cast<const KTarFile *>( entry );

    QBuffer buffer( d->m_docInfoFile->data() );
    buffer.open( IO_ReadOnly );

    QDomDocument doc;
    doc.setContent( &buffer );

    d->m_info->load( doc );
  }

  d->m_dlg = new KoDocumentInfoDlg( d->m_info, 0, 0, props );
  connect( d->m_dlg, SIGNAL( changed() ),
           this, SIGNAL( changed() ) );
}

KoDocumentInfoPropsPage::~KoDocumentInfoPropsPage()
{
  delete d->m_info;
  delete d->m_src;
  delete d->m_dst;
  delete d->m_dlg;
  delete d;
}

void KoDocumentInfoPropsPage::applyChanges()
{
  const KTarDirectory *root = d->m_src->directory();
  if ( !root )
    return;

  struct stat statBuff;

  if ( stat( QFile::encodeName( d->m_url.path() ), &statBuff ) != 0 )
    return;

  KTempFile tempFile( d->m_url.path(), QString::null, statBuff.st_mode );

  tempFile.setAutoDelete( true );

  if ( tempFile.status() != 0 )
    return;

  if ( !tempFile.close() )
    return;

  d->m_dst = new KTarGz( tempFile.name(), "application/x-gzip" );

  if ( !d->m_dst->open( IO_WriteOnly ) )
    return;

  KMimeType::Ptr mimeType = KMimeType::findByURL( d->m_url, 0, true );
  if ( mimeType && dynamic_cast<KFilterDev *>( d->m_dst->device() ) != 0 )
  {
      QCString appIdentification( "KOffice " ); // We are limited in the number of chars.
      appIdentification += mimeType->name().latin1();
      appIdentification += '\004'; // Two magic bytes to make the identification
      appIdentification += '\006'; // more reliable (DF)
      d->m_dst->setOrigFileName( appIdentification );
  }

  bool docInfoSaved = false;

  QStringList entries = root->entries();
  QStringList::ConstIterator it = entries.begin();
  QStringList::ConstIterator end = entries.end();
  for (; it != end; ++it )
  {
    const KTarEntry *entry = root->entry( *it );

    assert( entry );

    if ( entry->name() == "documentinfo.xml" ||
         ( !docInfoSaved && !entries.contains( "documentinfo.xml" ) ) )
    {
      d->m_dlg->save();

      QBuffer buffer;
      buffer.open( IO_WriteOnly );
      QTextStream str( &buffer );
      str << d->m_info->save();
      buffer.close();

      kdDebug( 30003 ) << "writing documentinfo.xml" << endl;
      d->m_dst->writeFile( "documentinfo.xml", entry->user(), entry->group(), buffer.buffer().size(),
                           buffer.buffer().data() );

      docInfoSaved = true;
    }
    else
      copy( QString::null, entry );
  }

  d->m_dst->close();

  QDir dir;
  dir.rename( tempFile.name(), d->m_url.path() );

  delete d->m_dst;
  d->m_dst = 0;
}

void KoDocumentInfoPropsPage::copy( const QString &path, const KArchiveEntry *entry )
{
  kdDebug( 30003 ) << "copy " << entry->name() << endl;
  if ( entry->isFile() )
  {
    const KTarFile *file = static_cast<const KTarFile *>( entry );
    kdDebug( 30003 ) << "file :" << entry->name() << endl;
    kdDebug( 30003 ) << "full path is: " << path << entry->name() << endl;
    d->m_dst->writeFile( path + entry->name(), entry->user(), entry->group(),
                         file->size(),
                         file->data().data() );
  }
  else
  {
    const KTarDirectory *dir = static_cast<const KTarDirectory *>( entry );
    kdDebug( 30003 ) << "dir : " << entry->name() << endl;
    kdDebug( 30003 ) << "full path is: " << path << entry->name() << endl;

    QString p = path + entry->name();
    if ( p != "/" )
    {
      d->m_dst->writeDir( p, entry->user(), entry->group() );
      p.append( "/" );
    }

    QStringList entries = dir->entries();
    QStringList::ConstIterator it = entries.begin();
    QStringList::ConstIterator end = entries.end();
    for (; it != end; ++it )
      copy( p, dir->entry( *it ) );
  }
}

/* vim: sw=2 et
 */

#include <koDocumentInfoDlg.moc>
