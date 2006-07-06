/* This file is part of the KDE project
   Copyright (C) 2005 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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

#include "link.h"

#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

#include <kdesktopfile.h>
#include <kpagedialog.h>
#include <kicon.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <krecentdocument.h>
#include <kurlrequester.h>

using namespace KSpread;

class LinkDialog::Private
{
public:
    QString text;
    QWidget* internetPage;
    KLineEdit* internetText;
    KLineEdit* internetLink;
    QWidget* mailPage;
    KLineEdit* mailText;
    KLineEdit* mailLink;
    QWidget* filePage;
    KLineEdit* fileText;
    KUrlRequester* fileLink;
    QWidget* cellPage;
    KLineEdit* cellText;
    KLineEdit* cellLink;
    KPageWidgetItem* p1, *p2, *p3, *p4;
};

LinkDialog::LinkDialog( QWidget*, const char* )
  :  KPageDialog( )
{
    setCaption( i18n("Insert Link") );
    setButtons( Ok | Cancel );
    setFaceType( List );

    d = new Private;

    // link for web or ftp
    d->internetPage = new QWidget();
    d->p1 = addPage( d->internetPage, i18n( "Internet" ) );
    d->p1->setIcon( KIcon( BarIcon( "html",K3Icon::SizeMedium ) ) );
    QVBoxLayout* iLayout = new QVBoxLayout( d->internetPage );
    iLayout->setMargin( marginHint() );
    iLayout->setSpacing( spacingHint() );
    iLayout->addWidget( new QLabel( i18n("Text to display:" ), d->internetPage ) );
    d->internetText = new KLineEdit( d->internetPage );
    iLayout->addWidget( d->internetText );
    iLayout->addWidget( new QLabel( i18n("Internet address:" ), d->internetPage ) );
    d->internetLink = new KLineEdit( d->internetPage );
    iLayout->addWidget( d->internetLink );
    iLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    connect( d->internetText, SIGNAL( textChanged( const QString& ) ), this,
        SLOT( setText( const QString& ) ) );

    // link for e-mail
    d->mailPage = new QWidget();
    d->p2 = addPage( d->mailPage, i18n( "Mail" ) );
    d->p2->setIcon( KIcon( BarIcon( "mail_generic",K3Icon::SizeMedium ) ) );
    QVBoxLayout* mLayout = new QVBoxLayout( d->mailPage );
    mLayout->setMargin( marginHint() );
    mLayout->setSpacing( spacingHint() );
    mLayout->addWidget( new QLabel( i18n("Text to display:" ), d->mailPage ) );
    d->mailText = new KLineEdit( d->mailPage );
    mLayout->addWidget( d->mailText );
    mLayout->addWidget( new QLabel( i18n("Email:" ), d->mailPage ) );
    d->mailLink = new KLineEdit( d->mailPage );
    mLayout->addWidget( d->mailLink );
    mLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    connect( d->mailText, SIGNAL( textChanged( const QString& ) ), this,
        SLOT( setText( const QString& ) ) );

    // link for external file
    d->filePage = new QWidget();
    d->p3 = addPage( d->filePage, i18n( "File" ) );
    d->p3->setIcon( KIcon( BarIcon( "filenew",K3Icon::SizeMedium ) ) );
    QVBoxLayout* fLayout = new QVBoxLayout( d->filePage );
    fLayout->setMargin( marginHint() );
    fLayout->setSpacing( spacingHint() );
    fLayout->addWidget( new QLabel( i18n("Text to display:" ), d->filePage ) );
    d->fileText = new KLineEdit( d->filePage );
    fLayout->addWidget( d->fileText );
    fLayout->addWidget( new QLabel( i18n("File location:" ), d->filePage ) );
    d->fileLink = new KUrlRequester( d->filePage );
    fLayout->addWidget( d->fileLink );
    fLayout->addWidget( new QLabel( i18n("Recent file:" ), d->filePage ) );
    QComboBox* recentFile = new QComboBox( d->filePage );
    recentFile->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    fLayout->addWidget( recentFile );
    fLayout->addItem( new QSpacerItem( 0, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );
    connect( d->fileText, SIGNAL( textChanged( const QString& ) ), this,
        SLOT( setText( const QString& ) ) );
    QObject::connect( recentFile, SIGNAL( highlighted ( const QString &) ),
        d->fileLink->lineEdit(), SLOT( setText( const QString & ) ) );


    // populate recent files
    int index = 0;
    QStringList fileList = KRecentDocument::recentDocuments();
    for( QStringList::ConstIterator it = fileList.begin();it != fileList.end(); ++it )
    {
        KDesktopFile f(*it, true /* read only */);
        if ( !f.readURL().isEmpty() )
            recentFile->insertItem( index++, f.readURL() );
    }
    if( recentFile->count()==0 )
    {
        recentFile->insertItem( 0, i18n("No Entries") );
        recentFile->setEnabled( false );
    }

    // link to another cell
    d->cellPage = new QWidget();
    d->p4 = addPage( d->cellPage, i18n( "Cell" ) );
    d->p4->setIcon( KIcon( BarIcon( "misc",K3Icon::SizeMedium ) ) );
    QVBoxLayout* cLayout = new QVBoxLayout( d->cellPage );
    cLayout->setSpacing( spacingHint() );
    cLayout->addWidget( new QLabel( i18n("Text to display:" ), d->cellPage ) );
    d->cellText = new KLineEdit( d->cellPage );
    cLayout->addWidget( d->cellText );
    cLayout->addWidget( new QLabel( i18n("Cell:" ), d->cellPage ) );
    d->cellLink = new KLineEdit( d->cellPage );
    cLayout->addWidget( d->cellLink );
    cLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    connect( d->cellText, SIGNAL( textChanged( const QString& ) ), this,
        SLOT( setText( const QString& ) ) );

    showButtonSeparator( true );
    d->internetText->setFocus();
    resize( 400,300 );
}

LinkDialog::~LinkDialog()
{
    delete d;
}

QString LinkDialog::text() const
{
    return d->text;
}

QString LinkDialog::link() const
{
    QString str;
    if ( currentPage() == d->p1 )
    {
      str = d->internetLink->text();
      if( !str.isEmpty() )
        if( str.indexOf( "http://" )==-1 )
          if( str.indexOf( "https://" )==-1 )
            if( str.indexOf( "ftp://" )==-1 )
              str.prepend( "http://" );
    }
    else if ( currentPage() == d->p2 )
    {
      str = d->mailLink->text();
      if( !str.isEmpty() )
        if( str.indexOf( "mailto:" )==-1 )
          str.prepend( "mailto:" );
      str = d->fileLink->lineEdit()->text();
      if( !str.isEmpty() )
        if( str.indexOf( "file:/" )==-1 )
          str.prepend( "file://" );
    }
    else if ( currentPage() == d->p3 )
    {
      str = d->fileLink->lineEdit()->text();
      if( !str.isEmpty() )
        if( str.indexOf( "file:/" )==-1 )
          str.prepend( "file://" );
    }
    else if ( currentPage() == d->p4 )
    {
      str = d->cellLink->text();
    }

    return str;
}

void LinkDialog::setText( const QString& text )
{
    d->text = text;

    d->internetText->blockSignals( true );
    d->internetText->setText( text );
    d->internetText->blockSignals( false );

    d->mailText->blockSignals( true );
    d->mailText->setText( text );
    d->mailText->blockSignals( false );

    d->fileText->blockSignals( true );
    d->fileText->setText( text );
    d->fileText->blockSignals( false );

    d->cellText->blockSignals( true );
    d->cellText->setText( text );
    d->cellText->blockSignals( false );
}

// link must be complete, e.g. "http://www.koffice.org" instead of
// "www.koffice.org" only, since protocol is used to decide which page to show
void LinkDialog::setLink( const QString& link )
{
    if( link.startsWith( "https://" ) )
    {
      d->internetLink->setText( link.mid( QString("https://").length() ) );
      setCurrentPage( d->p1 );
      return;
    }

    if( link.startsWith( "http://" ) )
    {
      d->internetLink->setText( link.mid( QString("http://").length() ) );
      setCurrentPage( d->p1 );
      return;
    }

    if( link.startsWith( "ftp://" ) )
    {
      d->internetLink->setText( link.mid( QString("ftp://").length() ) );
      setCurrentPage( d->p1 );
      return;
    }

    if( link.startsWith( "mailto:" ) )
    {
      d->mailLink->setText( link.mid( QString("mailto:").length() ) );
      setCurrentPage( d->p1 );
      return;
    }

    if( link.startsWith( "file:/" ) )
    {
      QString s = link.mid( QString("file:/").length() );
      while(s.startsWith("//")) s.remove(0,1);
      d->fileLink->lineEdit()->setText(s);
      setCurrentPage( d->p2 );
      return;
    }

    // assume cell reference
    d->cellLink->setText( link );
    setCurrentPage( d->p4 );
}

void LinkDialog::slotOk()
{
    QString str;

    if ( currentPage() == d->p1 )
      str = i18n( "Internet address is empty" );
    else if ( currentPage() == d->p2 )
      str = i18n( "Mail address is empty" );
    else if ( currentPage() == d->p3 )
      str = i18n( "File name is empty" );
    else if ( currentPage() == d->p4)
      i18n( "Destination cell is empty" );

    if( link().isEmpty() )
    {
       KMessageBox::error( this, str );
       return;
    }

    if( d->text.isEmpty() )
        d->text = link();

    accept();
}

#include "link.moc"
