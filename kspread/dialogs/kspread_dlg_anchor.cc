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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "kspread_dlg_anchor.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_editors.h"
#include "kspread_sheet.h"
#include "kspread_view.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qvbox.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kurlrequester.h>
#include <klineedit.h>
#include <kseparator.h>
#include <qcheckbox.h>
#include <kdesktopfile.h>
#include <krecentdocument.h>
#include <qcombobox.h>


KSpreadLinkDlg::KSpreadLinkDlg( KSpreadView* parent, const char* /*name*/ )
  :  KDialogBase( KDialogBase::IconList,i18n( "Insert Link") ,
                  KDialogBase::Ok | KDialogBase::Cancel,
                  KDialogBase::Ok )
{
  m_pView = parent;
  m_bold = false;
  m_italic = false;
  
  QVBox *page=addVBoxPage( i18n( "Internet" ), QString::null,BarIcon( "html",KIcon::SizeMedium ) );
  _internetAnchor = new  internetAnchor( parent,page );

  page=addVBoxPage( i18n( "Mail" ), QString::null,BarIcon( "mail_generic",KIcon::SizeMedium ) );
  _mailAnchor = new  mailAnchor( parent,page );

   page=addVBoxPage( i18n( "File" ), QString::null,BarIcon( "filenew",KIcon::SizeMedium ) );
  _fileAnchor = new  fileAnchor( parent,page );

  page=addVBoxPage( i18n( "Cell" ), QString::null,BarIcon( "misc",KIcon::SizeMedium ) );
  _cellAnchor = new  cellAnchor( parent,page );

  connect( this, SIGNAL( okClicked( ) ),this,SLOT( slotOk() ) );
  resize( 400,300 );
}

QString KSpreadLinkDlg::text() const
{
  return m_text;
}

QString KSpreadLinkDlg::link() const
{
  return m_link;
}

bool KSpreadLinkDlg::bold() const
{
  return m_bold;
}

bool KSpreadLinkDlg::italic() const
{
  return m_italic;
}

void KSpreadLinkDlg::slotOk()
{
  QString text, link, str;
  bool bold, italic;
  
  switch( activePageIndex() )
  {
    case 0:
      m_text = _internetAnchor->text();
      m_link = _internetAnchor->link();
      m_bold = _internetAnchor->bold();
      m_italic = _internetAnchor->italic();
      str = i18n( "Internet address is empty" );
      break;
    case 1:
      m_text = _mailAnchor->text();
      m_link = _mailAnchor->link();
      m_bold = _mailAnchor->bold();
      m_italic = _mailAnchor->italic();
      str = i18n( "Mail address is empty" );
      break;
    case 2:
      m_text = _fileAnchor->text();
      m_link = _fileAnchor->link();
      m_bold = _fileAnchor->bold();
      m_italic = _fileAnchor->italic();
      str = i18n( "File name is empty" );
      break;
    case 3:
      m_text = _cellAnchor->text();
      m_link = _cellAnchor->link();
      m_bold = _cellAnchor->bold();
      m_italic = _cellAnchor->italic();
      str = i18n( "Destination cell is empty" );
      break;
    default:
      kdDebug(36001)<<"Error in KSpreadLinkDlg\n";
    }
    
  if( m_link.isEmpty() )
  {
    KMessageBox::error( this, str );
    return;
  } 
  
  if( m_text.isEmpty() )
    m_text = m_link;
    
  accept();
}

internetAnchor::internetAnchor( KSpreadView* _view, QWidget *parent , char *name )
  : QWidget ( parent,name )
{
  m_pView=_view;
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( KDialog::marginHint() );
  lay1->setSpacing( KDialog::spacingHint() );
  QVBoxLayout *lay2 = new QVBoxLayout( lay1 );
  lay2->setSpacing( KDialog::spacingHint() );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this );

  lay2->addWidget(tmpQLabel );
  tmpQLabel->setText( i18n( "Comment:" ) );

  textEdit = new QLineEdit( this );
  lay2->addWidget( textEdit );

  tmpQLabel = new QLabel( this );
  lay2->addWidget( tmpQLabel );
  tmpQLabel->setText( i18n( "Internet address:" ) );
  l_internet = new QLineEdit( this );

  lay2->addWidget( l_internet );

  boldCheck=new QCheckBox( i18n( "Bold" ),this );

  lay2->addWidget( boldCheck );

  italicCheck=new QCheckBox( i18n( "Italic" ),this );

  lay2->addWidget( italicCheck );
  
  lay2->addStretch( 1 );

  KSeparator* bar1 = new KSeparator(  KSeparator::HLine, this );
  bar1->setFixedHeight( 10 );
  lay2->addWidget( bar1 );

  textEdit->setFocus();
}

QString internetAnchor::text() const
{
  return textEdit->text();
}

QString internetAnchor::link() const
{
  QString str = l_internet->text();
  
  if( str.find( "http://" )==-1 )
  if( str.find( "https://" )==-1 )
  if( str.find( "ftp://" )==-1 )
    str.prepend( "http://" );
    
  return str;  
}

bool internetAnchor::bold() const
{
  return boldCheck->isChecked();
}

bool internetAnchor::italic() const
{
  return italicCheck->isChecked();
}

mailAnchor::mailAnchor( KSpreadView* _view,QWidget *parent , char *name )
 :QWidget ( parent,name )
{
  m_pView=_view;
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( KDialog::marginHint() );
  lay1->setSpacing( KDialog::spacingHint() );
  QVBoxLayout *lay2 = new QVBoxLayout( lay1 );
  lay2->setSpacing( KDialog::spacingHint() );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this );

  lay2->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n( "Comment:" ) );

  textEdit = new QLineEdit( this );
  lay2->addWidget( textEdit );

  tmpQLabel = new QLabel( this );
  lay2->addWidget( tmpQLabel );
  tmpQLabel->setText( i18n( "Email:" ) );
  l_mail = new QLineEdit( this );

  lay2->addWidget( l_mail );

  boldCheck=new QCheckBox( i18n( "Bold" ),this );

  lay2->addWidget( boldCheck );

  italicCheck=new QCheckBox( i18n( "Italic" ),this );

  lay2->addWidget( italicCheck );

  lay2->addStretch( 1 );
  
  KSeparator* bar1 = new KSeparator( KSeparator::HLine, this );
  bar1->setFixedHeight( 10 );
  lay2->addWidget( bar1 );

  textEdit->setFocus();
}

QString mailAnchor::text() const
{
  return textEdit->text();
}

QString mailAnchor::link() const
{
  QString str = l_mail->text();
  
  if( str.find( "mailto:" )==-1 )
    str.prepend( "mailto:" );
    
  return str;  
}

bool mailAnchor::bold() const
{
  return boldCheck->isChecked();
}

bool mailAnchor::italic() const
{
  return italicCheck->isChecked();
}


fileAnchor::fileAnchor( KSpreadView* _view,QWidget *parent , char *name )
 :QWidget ( parent,name)
{
  m_pView=_view;
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( KDialog::marginHint() );
  lay1->setSpacing( KDialog::spacingHint()  );
  QVBoxLayout *lay2 = new QVBoxLayout( lay1);
  lay2->setSpacing( KDialog::marginHint()  );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this);

  lay2->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("Comment:"));

  textEdit = new QLineEdit( this );
  lay2->addWidget(textEdit);

  tmpQLabel = new QLabel( this);
  lay2->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("Recent file:"));


  QComboBox * recentFile = new QComboBox( this );
  lay2->addWidget(recentFile);


  tmpQLabel = new QLabel( this);
  lay2->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("File location:"));
  //l_file = new QLineEdit( this );
  l_file = new KURLRequester( this );

  lay2->addWidget(l_file);

  boldCheck=new QCheckBox(i18n("Bold"),this);

  lay2->addWidget(boldCheck);

  italicCheck=new QCheckBox(i18n("Italic"),this);

  lay2->addWidget(italicCheck);

  QStringList fileList = KRecentDocument::recentDocuments();
  QStringList lst;
  lst <<"";
  for (QStringList::ConstIterator it = fileList.begin();it != fileList.end(); ++it)
  {
      KDesktopFile f(*it, true /* read only */);
      if ( !f.readURL().isEmpty())
          lst.append( f.readURL());
  }
  if ( lst.count()<= 1 )
  {
      recentFile->clear();
      recentFile->insertItem( i18n("No Entries") );
      recentFile->setEnabled( false );
  }
  else
      recentFile->insertStringList( lst);

  recentFile->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    
  connect( recentFile , SIGNAL(highlighted ( const QString &)), this,  SLOT( slotSelectRecentFile( const QString & )));

  lay2->addStretch( 1 );

  KSeparator* bar1 = new KSeparator( KSeparator::HLine, this);
  bar1->setFixedHeight( 10 );
  lay2->addWidget( bar1 );

  textEdit->setFocus();
}

QString fileAnchor::text() const
{
  return textEdit->text();
}

QString fileAnchor::link() const
{
  QString str = l_file->lineEdit()->text();
  
  if( str.find( "file:/" )==-1 )
    str.prepend( "file:/" );
    
  return str;  
}

bool fileAnchor::bold() const
{
  return boldCheck->isChecked();
}

bool fileAnchor::italic() const
{
  return italicCheck->isChecked();
}

void fileAnchor::slotSelectRecentFile( const QString &_file )
{
    l_file->lineEdit()->setText(_file );
}

cellAnchor::cellAnchor( KSpreadView* _view,QWidget *parent , char *name )
 :QWidget ( parent,name)
{
  m_pView=_view;
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( KDialog::marginHint()  );
  lay1->setSpacing( KDialog::spacingHint() );
  QVBoxLayout *lay2 = new QVBoxLayout( lay1);
  lay2->setSpacing( KDialog::spacingHint() );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this);

  lay2->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("Comment:"));

  textEdit = new QLineEdit( this );
  lay2->addWidget(textEdit);

  tmpQLabel = new QLabel( this);
  lay2->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("Cell:"));
  l_cell = new QLineEdit( this );

  lay2->addWidget(l_cell);
  l_cell->setText( "A1" );

  boldCheck=new QCheckBox(i18n("Bold"),this);

  lay2->addWidget(boldCheck);

  italicCheck=new QCheckBox(i18n("Italic"),this);

  lay2->addWidget(italicCheck);

  lay2->addStretch( 1 );
  
  KSeparator* bar1 = new KSeparator( KSeparator::HLine, this);
  bar1->setFixedHeight( 10 );
  lay2->addWidget( bar1 );

  textEdit->setFocus();
}

QString cellAnchor::text() const
{
  return textEdit->text();
}

QString cellAnchor::link() const
{
  QString str = m_pView->activeTable()->tableName() + "!" + 
    l_cell->text().upper();
   
  return str;  
}

bool cellAnchor::bold() const
{
  return boldCheck->isChecked();
}

bool cellAnchor::italic() const
{
  return italicCheck->isChecked();
}

#include "kspread_dlg_anchor.moc"
