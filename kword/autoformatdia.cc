/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "autoformatdia.h"
#include "autoformatdia.moc"
#include "autoformat.h"
#include "kcharselectdia.h"

#include <klocale.h>
#include <kcharselect.h>
#include <kmessagebox.h>
#include <kbuttonbox.h>
#include <klistview.h>

#include <qlayout.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qmap.h>
#include <qlineedit.h>
#include <qlabel.h>


/******************************************************************/
/* Class: KWAutoFormatDia                                         */
/******************************************************************/

KWAutoFormatDia::KWAutoFormatDia( QWidget *parent, const char *name, KWAutoFormat * autoFormat )
    : KDialogBase( Tabbed, i18n("Autocorrection"), Ok | Cancel, Ok, parent, name, true),
      oBegin( autoFormat->getConfigTypographicQuotes().begin ),
      oEnd( autoFormat->getConfigTypographicQuotes().end ),
      quotesChanged( false ),
      m_autoFormat( *autoFormat ),
      m_docAutoFormat( autoFormat )
{
    setupTab1();
    setupTab2();

    setInitialSize( QSize(500, 300) );
}

void KWAutoFormatDia::setupTab1()
{
    tab1 = addPage( i18n( "Simple Autocorrection" ) );
    QVBoxLayout *grid = new QVBoxLayout(tab1, 10, 5);
    grid->setAutoAdd( true );

    cbTypographicQuotes = new QCheckBox( tab1 );
    cbTypographicQuotes->setText( i18n( "Replace &Quotes by Typographical Quotes:" ) );
    cbTypographicQuotes->resize( cbTypographicQuotes->sizeHint() );

    cbTypographicQuotes->setChecked( m_autoFormat.getConfigTypographicQuotes().replace );

    QHBox *quotes = new QHBox( tab1 );
    quotes->setSpacing( 5 );
    pbQuote1 = new QPushButton( quotes );

    pbQuote1->setText( oBegin );

    pbQuote1->resize( pbQuote1->sizeHint() );
    pbQuote2 = new QPushButton( quotes );

    pbQuote2->setText(oEnd );

    pbQuote2->resize( pbQuote2->sizeHint() );
    ( void )new QWidget( quotes );
    quotes->setMaximumHeight( pbQuote1->sizeHint().height() );

    connect( pbQuote1, SIGNAL( clicked() ), this, SLOT( chooseQuote1() ) );
    connect( pbQuote2, SIGNAL( clicked() ), this, SLOT( chooseQuote2() ) );

    ( void )new QWidget( tab1 );

    cbUpperCase = new QCheckBox( tab1 );
    cbUpperCase->setText( i18n( "Convert first letter from the first word of a sentence automatically\n"
                                "to &Upper Case ( e.g. \"bla. this is a Test\" to \"bla. This is a Test\" )" ) );
    cbUpperCase->resize( cbUpperCase->sizeHint() );

    cbUpperCase->setChecked( m_autoFormat.getConfigUpperCase() );

    ( void )new QWidget( tab1 );

    cbUpperUpper = new QCheckBox( tab1 );
    cbUpperUpper->setText( i18n( "Convert two Upper &Case letters to one Upper Case and one Lower Case letter.\n"
                                 "( e.g. HEllo to Hello )" ) );
    cbUpperUpper->resize( cbUpperUpper->sizeHint() );
    cbUpperUpper->setChecked( m_autoFormat.getConfigUpperUpper() );
    ( void )new QWidget( tab1 );
}

void KWAutoFormatDia::setupTab2()
{
    tab2 = addPage( i18n( "Advanced Autocorrection" ) );
    QHBoxLayout *grid = new QHBoxLayout(tab2, 10, 5);
    grid->setAutoAdd( true );

    m_pListView = new KListView( tab2 );
    m_pListView->addColumn( i18n( "Find" ) );
    m_pListView->addColumn( i18n( "Replace" ) );
    m_pListView->setAllColumnsShowFocus( true );
    /*m_pListView->setRenameable( 0, true );
    m_pListView->setRenameable( 1, true );
    m_pListView->setItemsRenameable( true );
    connect( m_pListView, SIGNAL(itemRenamed(QListViewItem *, const QString &, int)),
             SLOT(slotItemRenamed(QListViewItem *, const QString &, int)) );
    */

    QMap< QString, KWAutoFormatEntry >::Iterator it = m_autoFormat.firstAutoFormatEntry();
    for ( ; it != m_autoFormat.lastAutoFormatEntry(); ++it )
        ( void )new QListViewItem( m_pListView, it.key(), it.data().replace() );

    QVBox *buttons = new QVBox( tab2 );
    buttons->setSpacing( 5 );
    buttons->setMargin( 5 );

    pbAdd = new QPushButton( i18n( "Add..."), buttons  );
    connect(pbAdd,SIGNAL(clicked()),this, SLOT(slotAddEntry()));

    pbRemove = new QPushButton( i18n( "Remove" ), buttons );
    connect(pbRemove,SIGNAL(clicked()),this,SLOT(slotRemoveEntry()));
    ( void )new QWidget( buttons );
    pbEdit = new QPushButton( i18n( "Edit..." ), buttons );
    connect(pbEdit,SIGNAL(clicked()),this,SLOT(slotEditEntry()));
    ( void )new QWidget( buttons );
}

void KWAutoFormatDia::slotRemoveEntry()
{
    if(m_pListView->currentItem()!=0)
    {
        m_autoFormat.removeAutoFormatEntry(m_pListView->currentItem()->text(0));
        refreshEntryList();
    }
}

void KWAutoFormatDia::refreshEntryList()
{
    QMap< QString, KWAutoFormatEntry >::Iterator it = m_autoFormat.firstAutoFormatEntry();
    m_pListView->clear();
    for ( ; it != m_autoFormat.lastAutoFormatEntry(); ++it )
        ( void )new QListViewItem( m_pListView, it.key(), it.data().replace() );
}

void KWAutoFormatDia::addEntryList(const QString &key, KWAutoFormatEntry &_autoEntry)
{
    m_autoFormat.addAutoFormatEntry( key, _autoEntry );
}

void KWAutoFormatDia::editEntryList(const QString &key,const QString &newFindString, KWAutoFormatEntry &_autoEntry)
{
    m_autoFormat.removeAutoFormatEntry( key );
    m_autoFormat.addAutoFormatEntry( newFindString, _autoEntry );
}

void KWAutoFormatDia::slotAddEntry()
{
    QString strFind;
    QString strReplace;
    KWAutoFormatEditDia *dia=new KWAutoFormatEditDia(this,"addDia",i18n("Add Entry"),strFind,strReplace);
    if(dia->exec())
    {
        refreshEntryList();
    }
}

void KWAutoFormatDia::slotItemRenamed(QListViewItem *, const QString & , int )
{
    // Wow. This need a redesign (we don't have the old key anymore at this point !)
    // -> inherit QListViewItem and store the KWAutoFormatEntry pointer in it.
}

void KWAutoFormatDia::slotEditEntry()
{
    if(m_pListView->currentItem()==0)
        return;
    QString strFind=m_pListView->currentItem()->text(0);
    QString strReplace=m_pListView->currentItem()->text(1);
    KWAutoFormatEditDia *dia=new KWAutoFormatEditDia(this,"addDia",i18n("Edit Entry"),strFind,strReplace,true,strFind);
    if(dia->exec())
    {
        refreshEntryList();
    }
}
bool KWAutoFormatDia::applyConfig()
{
    // First tab
    KWAutoFormat::TypographicQuotes tq = m_autoFormat.getConfigTypographicQuotes();
    tq.replace = cbTypographicQuotes->isChecked();
    tq.begin = pbQuote1->text()[ 0 ];
    tq.end = pbQuote2->text()[ 0 ];
    m_docAutoFormat->configTypographicQuotes( tq );

    m_docAutoFormat->configUpperCase( cbUpperCase->isChecked() );
    m_docAutoFormat->configUpperUpper( cbUpperUpper->isChecked() );

    // Second tab
    m_docAutoFormat->copyAutoFormatEntries( m_autoFormat );

    return true;
}

void KWAutoFormatDia::slotOk()
{
    if (applyConfig())
    {
       KDialogBase::slotOk();
    }
}

void KWAutoFormatDia::chooseQuote1()
{
    QString f = font().family();
    QChar c = oBegin;
    if ( KCharSelectDia::selectChar( f, c, false ) )
    {
        pbQuote1->setText( c );
        quotesChanged = true;
    }
}

void KWAutoFormatDia::chooseQuote2()
{
    QString f = font().family();
    QChar c = oEnd;
    if ( KCharSelectDia::selectChar( f, c, false ) )
    {
        pbQuote2->setText( c );
        quotesChanged = true;
    }
}

KWAutoFormatEditDia::KWAutoFormatEditDia( KWAutoFormatDia *parent, const char *name, const QString &title,const QString &findStr,const QString &replaceStr,bool _replaceEntry, const QString & _str )
    : QDialog( parent, name,TRUE )
{
    replaceEntry=_replaceEntry;
    replaceEntryString=_str;
    parentWidget=parent;
    setCaption(title);
    QGridLayout *grid = new QGridLayout( this, 2, 3, 15, 15 );

    QLabel *text=new QLabel(i18n("Find"),this);
    grid->addWidget(text,0,0);

    lineEditFind=new QLineEdit(this);
    grid->addWidget(lineEditFind,0,1);
    lineEditFind->setText(findStr);

    pbSpecialChar1=new QPushButton(this);
    grid->addWidget(pbSpecialChar1,0,2);

    text=new QLabel(i18n("Replace"),this);
    grid->addWidget(text,1,0);

    lineEditReplace=new QLineEdit(this);
    lineEditReplace->setText(replaceStr);

    grid->addWidget(lineEditReplace,1,1);

    pbSpecialChar2=new QPushButton(this);
    grid->addWidget(pbSpecialChar2,1,2);

    connect(pbSpecialChar1,SIGNAL(clicked()),this,SLOT(chooseSpecialChar1()));
    connect(pbSpecialChar2,SIGNAL(clicked()),this,SLOT(chooseSpecialChar2()));

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    m_pOk = bb->addButton( i18n("OK") );
    m_pOk->setDefault( TRUE );
    m_pClose = bb->addButton( i18n( "Close" ) );
    bb->layout();
    connect(m_pOk,SIGNAL(clicked()),this,SLOT(slotOk()));
    connect(m_pClose,SIGNAL(clicked()),this,SLOT(slotCancel()));
    grid->addMultiCellWidget( bb,2,2,1,2 );
    lineEditFind->setFocus();
}

void KWAutoFormatEditDia::chooseSpecialChar1()
{
    QString f = font().family();
    QChar c = ' ';
    if ( KCharSelectDia::selectChar( f, c, false ) )
    {
        pbSpecialChar1->setText( c );
        lineEditFind->setText( c );
    }
}

void KWAutoFormatEditDia::chooseSpecialChar2()
{
    QString f = font().family();
    QChar c = ' ';
    if ( KCharSelectDia::selectChar( f, c, false ) )
    {
        pbSpecialChar2->setText( c );
        lineEditReplace->setText( c );
    }
}

void KWAutoFormatEditDia::slotOk()
{
    if(lineEditReplace->text().isEmpty() || lineEditFind->text().isEmpty())
    {
       KMessageBox::sorry( 0L, i18n( "An area is empty" ) );
       return;
    }
    KWAutoFormatEntry tmp = KWAutoFormatEntry( lineEditReplace->text() );
    if(!replaceEntry)
        parentWidget->addEntryList(lineEditFind->text(), tmp);
    else
        parentWidget->editEntryList(replaceEntryString,lineEditFind->text() ,tmp);
    accept();
}

void KWAutoFormatEditDia::slotCancel()
{
    reject();
}
