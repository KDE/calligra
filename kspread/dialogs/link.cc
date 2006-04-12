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

#include <qcombobox.h>
#include <q3frame.h>
#include <qlabel.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

#include <kdesktopfile.h>
#include <kdialogbase.h>
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
    QFrame* internetPage;
    KLineEdit* internetText;
    KLineEdit* internetLink;
    QFrame* mailPage;
    KLineEdit* mailText;
    KLineEdit* mailLink;
    QFrame* filePage;
    KLineEdit* fileText;
    KUrlRequester* fileLink;
    QFrame* cellPage;
    KLineEdit* cellText;
    KLineEdit* cellLink;
};

LinkDialog::LinkDialog( QWidget*, const char* )
  :  KDialogBase( KDialogBase::IconList,i18n( "Insert Link") ,
                  KDialogBase::Ok | KDialogBase::Cancel,
                  KDialogBase::Ok )
{
    d = new Private;
    
    // link for web or ftp
    d->internetPage = addPage( i18n( "Internet" ), QString::null, 
        BarIcon( "html",K3Icon::SizeMedium ) );
    Q3VBoxLayout* iLayout = new Q3VBoxLayout( d->internetPage, marginHint(), spacingHint() );
    iLayout->add( new QLabel( i18n("Text to display:" ), d->internetPage ) );
    d->internetText = new KLineEdit( d->internetPage );
    iLayout->add( d->internetText );
    iLayout->add( new QLabel( i18n("Internet address:" ), d->internetPage ) );
    d->internetLink = new KLineEdit( d->internetPage );
    iLayout->add( d->internetLink );
    iLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    connect( d->internetText, SIGNAL( textChanged( const QString& ) ), this,
        SLOT( setText( const QString& ) ) );
    
    // link for e-mail
    d->mailPage = addPage( i18n( "Mail" ), QString::null, 
        BarIcon( "mail_generic",K3Icon::SizeMedium ) );
    Q3VBoxLayout* mLayout = new Q3VBoxLayout( d->mailPage, marginHint(), spacingHint() );
    mLayout->add( new QLabel( i18n("Text to display:" ), d->mailPage ) );
    d->mailText = new KLineEdit( d->mailPage );
    mLayout->add( d->mailText );
    mLayout->add( new QLabel( i18n("Email:" ), d->mailPage ) );
    d->mailLink = new KLineEdit( d->mailPage );
    mLayout->add( d->mailLink );
    mLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    connect( d->mailText, SIGNAL( textChanged( const QString& ) ), this,
        SLOT( setText( const QString& ) ) );
    
    // link for external file
    d->filePage = addPage( i18n( "File" ), QString::null, 
        BarIcon( "filenew",K3Icon::SizeMedium ) );
    Q3VBoxLayout* fLayout = new Q3VBoxLayout( d->filePage, marginHint(), spacingHint() );
    fLayout->add( new QLabel( i18n("Text to display:" ), d->filePage ) );
    d->fileText = new KLineEdit( d->filePage );
    fLayout->add( d->fileText );
    fLayout->add( new QLabel( i18n("File location:" ), d->filePage ) );
    d->fileLink = new KUrlRequester( d->filePage );
    fLayout->add( d->fileLink );
    fLayout->add( new QLabel( i18n("Recent file:" ), d->filePage ) );
    QComboBox* recentFile = new QComboBox( d->filePage );
    recentFile->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    fLayout->add( recentFile );
    fLayout->addItem( new QSpacerItem( 0, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );
    connect( d->fileText, SIGNAL( textChanged( const QString& ) ), this,
        SLOT( setText( const QString& ) ) );
    QObject::connect( recentFile, SIGNAL( highlighted ( const QString &) ),
        d->fileLink->lineEdit(), SLOT( setText( const QString & ) ) );
    
    
    // populate recent files
    QStringList fileList = KRecentDocument::recentDocuments();
    for( QStringList::ConstIterator it = fileList.begin();it != fileList.end(); ++it )
    {
        KDesktopFile f(*it, true /* read only */);
        if ( !f.readURL().isEmpty() )
            recentFile->insertItem( f.readURL() );
    }
    if( recentFile->count()==0 )
    {
        recentFile->insertItem( i18n("No Entries") );
        recentFile->setEnabled( false );
    }    
    
    // link to another cell
    d->cellPage =  addPage( i18n( "Cell" ), QString::null, 
        BarIcon( "misc",K3Icon::SizeMedium ) );
    Q3VBoxLayout* cLayout = new Q3VBoxLayout( d->cellPage, marginHint(), spacingHint() );
    cLayout->add( new QLabel( i18n("Text to display:" ), d->cellPage ) );
    d->cellText = new KLineEdit( d->cellPage );
    cLayout->add( d->cellText );
    cLayout->add( new QLabel( i18n("Cell:" ), d->cellPage ) );
    d->cellLink = new KLineEdit( d->cellPage );
    cLayout->add( d->cellLink );
    cLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    connect( d->cellText, SIGNAL( textChanged( const QString& ) ), this,
        SLOT( setText( const QString& ) ) );
    
    enableButtonSeparator( true );
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
    switch( activePageIndex() )
    {
        case 0:  
          str = d->internetLink->text();  
          if( !str.isEmpty() )
          if( str.indexOf( "http://" )==-1 )
          if( str.indexOf( "https://" )==-1 )
          if( str.indexOf( "ftp://" )==-1 )
              str.prepend( "http://" );
          break;
          
        case 1:  
           str = d->mailLink->text(); 
           if( !str.isEmpty() )
           if( str.indexOf( "mailto:" )==-1 )
               str.prepend( "mailto:" );
           break;
           
        case 2:  
           str = d->fileLink->lineEdit()->text(); 
           if( !str.isEmpty() )
           if( str.indexOf( "file:/" )==-1 )
               str.prepend( "file://" );
           break;
           
        case 3:  
            str = d->cellLink->text(); 
            break;
            
        break;
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
      showPage( 0 );
      return;
    }
    
    if( link.startsWith( "http://" ) )
    {
      d->internetLink->setText( link.mid( QString("http://").length() ) );
      showPage( 0 );
      return;
    }
    
    if( link.startsWith( "ftp://" ) )
    {
      d->internetLink->setText( link.mid( QString("ftp://").length() ) );
      showPage( 0 );
      return;
    }
    
    if( link.startsWith( "mailto:" ) )
    {
      d->mailLink->setText( link.mid( QString("mailto:").length() ) );
      showPage( 1 );
      return;
    }
    
    if( link.startsWith( "file:/" ) )
    {
      QString s = link.mid( QString("file:/").length() );
      while(s.startsWith("//")) s.remove(0,1);
      d->fileLink->lineEdit()->setText(s);
      showPage( 2 );
      return;
    }
    
    // assume cell reference
    d->cellLink->setText( link );
    showPage( 3 );
}

void LinkDialog::slotOk()
{
    QString str;
    switch( activePageIndex() )
    {
        case 0:  str = i18n( "Internet address is empty" );  break;
        case 1:  str = i18n( "Mail address is empty" ); break;
        case 2:  str = i18n( "File name is empty" ); break;
        case 3:  str = i18n( "Destination cell is empty" ); break;
        break;
    }
    
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
