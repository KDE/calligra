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

void KSpreadLinkDlg::slotOk()
{
  QString text, link, str;
  bool bold, italic;
  
  switch( activePageIndex() )
  {
    case 0:
      text = _internetAnchor->text();
      link = _internetAnchor->link();
      bold = _internetAnchor->bold();
      italic = _internetAnchor->italic();
      str = i18n( "Internet address is empty" );
      break;
    case 1:
      text = _mailAnchor->text();
      link = _mailAnchor->link();
      bold = _mailAnchor->bold();
      italic = _mailAnchor->italic();
      str = i18n( "Mail address is empty" );
      break;
    case 2:
      text = _fileAnchor->text();
      link = _fileAnchor->link();
      bold = _fileAnchor->bold();
      italic = _fileAnchor->italic();
      str = i18n( "File name is empty" );
      break;
    case 3:
      text = _cellAnchor->text();
      link = _cellAnchor->link();
      bold = _cellAnchor->bold();
      italic = _cellAnchor->italic();
      str = i18n( "Destination cell is empty" );
      break;
    default:
      kdDebug(36001)<<"Error in KSpreadLinkDlg\n";
    }
    
  if( link.isEmpty() )
  {
    KMessageBox::error( this, str );
    return;
  } 
  
  if( text.isEmpty() )
    text = link;
    
  str = "<a href=\"" + link + "\">" + text + "</a>";
  if( italic ) 
    str.prepend( "<i>" ).append( "</i>" );  
  if( bold ) 
    str.prepend( "<b>" ).append( "</b>" );  
  str.prepend( '!' );
  
  setCellText( str );
}

void KSpreadLinkDlg::setCellText( const QString &_text )
{
    m_pView->doc()->emitBeginOperation(  false );

    KSpreadCell *cell = m_pView->activeTable()->cellAt( m_pView->canvasWidget()->markerColumn(),m_pView->canvasWidget()->markerRow() );

    if ( !cell->isEmpty() )
      {
	int ret = KMessageBox::warningYesNo( this, i18n( "Cell is not empty.\nDo you want to continue?" ) );
	if ( ret == 4 )
	  {
	    reject();
	    return;
	  }
      }

    //refresh editWidget
    if (!_text.isEmpty() )
    {
	m_pView->canvasWidget()->setFocus();
	m_pView->setText( _text );
	m_pView->editWidget()->setText( _text );
	accept();
    }

    m_pView->slotUpdateView( m_pView->activeTable() );
    //m_pView->doc()->emitEndOperation();
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
