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

#include <sys/stat.h>
#include <unistd.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qvbox.h>
#include <qbuffer.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qdir.h>

#include <klocale.h>
#include <kconfig.h>
#include <kdialogbase.h>
#include <ktar.h>
#include <kdebug.h>
#include <ktempfile.h>

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

  bool m_bDeleteDialog;
  KDialogBase *m_dialog;
};

KoDocumentInfoDlg::KoDocumentInfoDlg( KoDocumentInfo *docInfo, QWidget *parent, const char *name,
                                      KDialogBase *dialog )
: QObject( parent, "docinfodlg" )
{
  d = new KoDocumentInfoDlgPrivate;
  d->m_info = docInfo;
  d->m_emailCfg = new KConfig( "emaildefaults", true );

  d->m_emailCfg->setGroup( "Defaults" );

  QString group = d->m_emailCfg->readEntry("Profile","Default");

  d->m_emailCfg->setGroup(QString("PROFILE_%1").arg(group));

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
  delete d->m_emailCfg;

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

void KoDocumentInfoDlg::addAuthorPage( KoDocumentInfoAuthor *authorInfo )
{
  QGrid *grid = d->m_dialog->addGridPage( 2, QGrid::Horizontal, i18n( "Author" ) );
  grid->setMargin(KDialog::marginHint());
  grid->setSpacing(KDialog::spacingHint());

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

  (void) new QLabel( i18n( "Street :" ), grid );
  d->m_leStreet = new QLineEdit( authorInfo->street(), grid );

  (void) new QLabel( i18n( "Postal Code :" ), grid );
  d->m_lePostalCode = new QLineEdit( authorInfo->postalCode(), grid );

  (void) new QLabel( i18n( "City :" ), grid );
  d->m_leCity = new QLineEdit( authorInfo->city(), grid );

  (void) new QLabel( i18n( "Country :" ), grid );
  d->m_leCountry = new QLineEdit( authorInfo->country(), grid );

  connect( d->m_leFullName, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leAuthorTitle, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leCompany, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leEmail, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_leTelephone, SIGNAL( textChanged( const QString & ) ),
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
}

void KoDocumentInfoDlg::addAboutPage( KoDocumentInfoAbout *aboutInfo )
{
  QGrid *grid = d->m_dialog->addGridPage( 2, QGrid::Horizontal, i18n( "About" ) );
  grid->setMargin(KDialog::marginHint());
  grid->setSpacing(KDialog::spacingHint());

  (void) new QLabel( i18n( "Title :" ), grid );
  d->m_leDocTitle = new QLineEdit( aboutInfo->title(), grid );

  (void) new QLabel( i18n( "Abstract :" ), grid );
  d->m_meAbstract = new QMultiLineEdit( grid );
  d->m_meAbstract->setText( aboutInfo->abstract() );

  connect( d->m_leDocTitle, SIGNAL( textChanged( const QString & ) ),
           this, SIGNAL( changed() ) );
  connect( d->m_meAbstract, SIGNAL( textChanged() ),
           this, SIGNAL( changed() ) );
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
  authorInfo->setFax( d->m_leFax->text() );
  authorInfo->setCountry( d->m_leCountry->text() );
  authorInfo->setPostalCode( d->m_lePostalCode->text() );
  authorInfo->setCity( d->m_leCity->text() );
  authorInfo->setStreet( d->m_leStreet->text() );

  KConfig config("kofficerc");
  config.setGroup( "Author" );
  config.writeEntry("full-name",d->m_leFullName->text());
  config.writeEntry("company",d->m_leCompany->text() );
  config.writeEntry("email", d->m_leEmail->text());
  config.writeEntry("telephone", d->m_leTelephone->text());
  config.writeEntry("fax", d->m_leFax->text());
  config.writeEntry("country",d->m_leCountry->text());
  config.writeEntry("postal-code",d->m_lePostalCode->text());
  config.writeEntry("city",  d->m_leCity->text());
  config.writeEntry("street", d->m_leStreet->text());

  d->m_info->documentInfochanged();
}

void KoDocumentInfoDlg::save( KoDocumentInfoAbout *aboutInfo )
{
  aboutInfo->setTitle( d->m_leDocTitle->text() );
  aboutInfo->setAbstract( d->m_meAbstract->text() );
}

class KoDocumentInfoPropsPage::KoDocumentInfoPropsPagePrivate
{
public:
  KoDocumentInfoPropsPagePrivate()
  {
  }
  ~KoDocumentInfoPropsPagePrivate()
  {
  }

  KoDocumentInfo *m_info;
  KoDocumentInfoDlg *m_dlg;
  KURL m_url;
  KTarGz *m_src;
  KTarGz *m_dst;

  const KTarFile *m_docInfoFile;
};

KoDocumentInfoPropsPage::KoDocumentInfoPropsPage( KPropertiesDialog *props )
: KPropsDlgPlugin( props )
{
  d = new KoDocumentInfoPropsPagePrivate;
  d->m_info = new KoDocumentInfo( this, "docinfo" );
  d->m_url = props->item()->url();
  d->m_dlg = 0;

  if ( !d->m_url.isLocalFile() )
    return;

  d->m_dst = 0;

  d->m_src = new KTarGz( d->m_url.path() );

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

  if ( d->m_src )
    delete d->m_src;

  if ( d->m_dst )
    delete d->m_dst;

  if ( d->m_dlg )
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

  d->m_dst = new KTarGz( tempFile.name() );

  if ( !d->m_dst->open( IO_WriteOnly ) )
    return;

  bool docInfoSaved = false;

  QStringList entries = root->entries();
  QStringList::ConstIterator it = entries.begin();
  QStringList::ConstIterator end = entries.end();
  for (; it != end; ++it )
  {
    const KTarEntry *entry = root->entry( *it );

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

void KoDocumentInfoPropsPage::copy( const QString &path, const KTarEntry *entry )
{
  kdDebug( 30003 ) << "copy" << endl;
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
      p.append( "/" );
      d->m_dst->writeDir( path + entry->name(), entry->user(), entry->group() );
    }

    QStringList entries = dir->entries();
    QStringList::ConstIterator it = entries.begin();
    QStringList::ConstIterator end = entries.end();
    for (; it != end; ++it )
      copy( p, dir->entry( *it ) );
  }
}

#include <koDocumentInfoDlg.moc>
