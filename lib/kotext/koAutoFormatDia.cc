/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2001, 2002 Sven Leiber         <s.leiber@web.de>

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

#include "koAutoFormatDia.h"
#include "koAutoFormatDia.moc"
#include <koAutoFormat.h>
#include <koCharSelectDia.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <klistview.h>

#include <qlayout.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlabel.h>


KoAutoFormatLineEdit::KoAutoFormatLineEdit ( QWidget * parent, const char * name )
    : QLineEdit(parent,name)
{
}

void KoAutoFormatLineEdit::keyPressEvent ( QKeyEvent *ke )
{
    if( ke->key()  == QKeyEvent::Key_Return ||
        ke->key()  == QKeyEvent::Key_Enter )
    {
        emit keyReturnPressed();
        return;
    }
    QLineEdit::keyPressEvent (ke);
}


/******************************************************************/
/* Class: KoAutoFormatExceptionWidget                             */
/******************************************************************/

KoAutoFormatExceptionWidget::KoAutoFormatExceptionWidget(QWidget *parent, const QString &name,const QStringList &_list,bool _abreviation)
    :QWidget( parent )
{
    m_bAbbreviation=_abreviation;
    m_listException=_list;
    QGridLayout *grid = new QGridLayout(this, 7, 2,2,2);
    exceptionLine = new KoAutoFormatLineEdit( this );
    grid->addWidget(exceptionLine,1,0);
    connect(exceptionLine,SIGNAL(keyReturnPressed()),this,SLOT(slotAddException()));

    QLabel *lab=new QLabel(name,this);
    grid->addMultiCellWidget(lab,0,0,0,1);

    pbAddException=new QPushButton(i18n("Add"),this);
    connect(pbAddException, SIGNAL(clicked()),this,SLOT(slotAddException()));
    grid->addWidget(pbAddException,2,1);

    pbRemoveException=new QPushButton(i18n("Remove"),this);
    connect(pbRemoveException, SIGNAL(clicked()),this,SLOT(slotRemoveException()));
    grid->addWidget(pbRemoveException,3,1);

    exceptionList=new QListBox(this);
    exceptionList->insertStringList(m_listException);
    grid->addMultiCellWidget(exceptionList,2,6,0,0);
    pbRemoveException->setEnabled(m_listException.count()>0);
    connect(exceptionLine ,SIGNAL(textChanged ( const QString & )),this,SLOT(textChanged ( const QString & )));
    pbAddException->setEnabled(false);
}


void KoAutoFormatExceptionWidget::textChanged ( const QString &_text )
{
    pbAddException->setEnabled(!_text.isEmpty());
}

void KoAutoFormatExceptionWidget::slotAddException()
{
    QString text=exceptionLine->text().stripWhiteSpace();
    if(!text.isEmpty())
    {
        if(text.at(text.length()-1)!='.' && m_bAbbreviation)
            text=text+".";
        m_listException<<text;

        exceptionList->clear();
        exceptionList->insertStringList(m_listException);
        pbRemoveException->setEnabled(m_listException.count()>0);
        exceptionLine->clear();
        pbAddException->setEnabled(false);
    }
}

void KoAutoFormatExceptionWidget::slotRemoveException()
{
    if(!exceptionList->currentText().isEmpty())
    {
        m_listException.remove(exceptionList->currentText());
        exceptionList->clear();
        pbAddException->setEnabled(false);
        pbRemoveException->setEnabled(m_listException.count()>0);
        exceptionList->insertStringList(m_listException);
    }
}

/******************************************************************/
/* Class: KoAutoFormatDia                                         */
/******************************************************************/

KoAutoFormatDia::KoAutoFormatDia( QWidget *parent, const char *name, KoAutoFormat * autoFormat )
    : KDialogBase( Tabbed, i18n("Autocorrection"), Ok | Cancel, Ok, parent, name, true),
      oBegin( autoFormat->getConfigTypographicQuotes().begin ),
      oEnd( autoFormat->getConfigTypographicQuotes().end ),
      m_autoFormat( *autoFormat ),
      m_docAutoFormat( autoFormat )
{
    setupTab1();
    setupTab2();
    setupTab3();
    setInitialSize( QSize(500, 300) );
}

void KoAutoFormatDia::setupTab1()
{
    tab1 = addPage( i18n( "Simple Autocorrection" ) );
    QVBoxLayout *grid = new QVBoxLayout(tab1, 10, 5);
    grid->setAutoAdd( true );

    cbTypographicQuotes = new QCheckBox( tab1 );
    cbTypographicQuotes->setText( i18n( "Replace &Quotes by Typographical Quotes:" ) );
    cbTypographicQuotes->resize( cbTypographicQuotes->sizeHint() );

    bool state=m_autoFormat.getConfigTypographicQuotes().replace;
    cbTypographicQuotes->setChecked( state );

    connect( cbTypographicQuotes,SIGNAL(toggled ( bool)),this,SLOT(slotChangeState(bool)));

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


    pbDefault = new QPushButton( quotes );

    pbDefault->setText(i18n("Default"));

    pbDefault->resize( pbDefault->sizeHint() );
    ( void )new QWidget( quotes );


    connect( pbQuote1, SIGNAL( clicked() ), this, SLOT( chooseQuote1() ) );
    connect( pbQuote2, SIGNAL( clicked() ), this, SLOT( chooseQuote2() ) );
    connect( pbDefault, SIGNAL( clicked()), this, SLOT( defaultQuote() ) );

    ( void )new QWidget( tab1 );

    cbUpperCase = new QCheckBox( tab1 );
    cbUpperCase->setText( i18n( "Convert first letter from the first word of a sentence automatically\n"
                                "to &Upper Case (e.g. \"bla. this is a Test\" to \"bla. This is a Test\")" ) );
    cbUpperCase->resize( cbUpperCase->sizeHint() );

    cbUpperCase->setChecked( m_autoFormat.getConfigUpperCase() );

    ( void )new QWidget( tab1 );

    cbUpperUpper = new QCheckBox( tab1 );
    cbUpperUpper->setText( i18n( "Convert two Upper &Case letters to one Upper Case and one Lower Case letter.\n"
                                 "(e.g. HEllo to Hello)" ) );
    cbUpperUpper->resize( cbUpperUpper->sizeHint() );
    cbUpperUpper->setChecked( m_autoFormat.getConfigUpperUpper() );
    ( void )new QWidget( tab1 );

    cbDetectUrl=new QCheckBox( tab1 );
    cbDetectUrl->setText( i18n( "Auto detect URL" ) );
    cbDetectUrl->resize( cbDetectUrl->sizeHint() );
    cbDetectUrl->setChecked( m_autoFormat.getConfigAutoDetectUrl());
    ( void )new QWidget( tab1 );

    cbIgnoreDoubleSpace=new QCheckBox( tab1 );
    cbIgnoreDoubleSpace->setText( i18n( "Ignore Double Space" ) );
    cbIgnoreDoubleSpace->resize( cbIgnoreDoubleSpace->sizeHint() );
    cbIgnoreDoubleSpace->setChecked( m_autoFormat.getConfigIgnoreDoubleSpace());
    ( void )new QWidget( tab1 );

    cbRemoveSpaceBeginEndLine=new QCheckBox( tab1 );
    cbRemoveSpaceBeginEndLine->setText( i18n( "Remove space at the begin and end line" ) );
    cbRemoveSpaceBeginEndLine->resize( cbRemoveSpaceBeginEndLine->sizeHint() );
    cbRemoveSpaceBeginEndLine->setChecked( m_autoFormat.getConfigRemoveSpaceBeginEndLine());
    ( void )new QWidget( tab1 );

    cbUseBulletStyle=new QCheckBox( tab1 );
    cbUseBulletStyle->setText( i18n( "Use bullet style - * " ) );
    cbUseBulletStyle->resize( cbUseBulletStyle->sizeHint() );
    cbUseBulletStyle->setChecked( m_autoFormat.getConfigUseBulletSyle());
    ( void )new QWidget( tab1 );

    slotChangeState(state);
}

void KoAutoFormatDia::setupTab2()
{
    tab2 = addPage( i18n( "Advanced Autocorrection" ) );
    QHBoxLayout *grid = new QHBoxLayout(tab2, 10, 5);
    grid->setAutoAdd( true );

    QVBox *left = new QVBox( tab2 );
    cbAdvancedAutoCorrection=new QCheckBox(i18n("Active autocorrection"),left);
    cbAdvancedAutoCorrection->setChecked(m_autoFormat.getConfigAdvancedAutoCorrect());
    QHBox *text = new QHBox( left );
    text->setSpacing( 3 );
    text->setMargin( 3 );
    m_find = new KoAutoFormatLineEdit( text );
    connect( m_find, SIGNAL( textChanged( const QString & ) ),
	     SLOT( slotfind( const QString & ) ) );
    connect( m_find, SIGNAL( keyReturnPressed() ),
             SLOT( slotAddEntry()));

    pbSpecialChar1 = new QPushButton( "...", text );
    connect(pbSpecialChar1,SIGNAL(clicked()),this,SLOT(chooseSpecialChar1()));
    m_replace = new KoAutoFormatLineEdit( text );
    connect( m_replace, SIGNAL( textChanged( const QString & ) ),
	     SLOT( slotfind2( const QString & ) ) );
    connect( m_replace, SIGNAL( keyReturnPressed() ),
             SLOT( slotAddEntry()));
    pbSpecialChar2 = new QPushButton( "...", text );
    connect(pbSpecialChar2,SIGNAL(clicked()),this,SLOT(chooseSpecialChar2()));
    m_pListView = new KListView( left );
    m_pListView->addColumn( i18n( "Find" ) );
    m_pListView->addColumn( i18n( "Replace" ) );
    m_pListView->setAllColumnsShowFocus( true );
    /*m_pListView->setRenameable( 0, true );
    m_pListView->setRenameable( 1, true );
    m_pListView->setItemsRenameable( true );
    connect( m_pListView, SIGNAL(itemRenamed(QListViewItem *, const QString &, int)),
             SLOT(slotItemRenamed(QListViewItem *, const QString &, int)) );
    */

    connect(m_pListView, SIGNAL(doubleClicked ( QListViewItem * )),
             SLOT(slotEditEntry()) );
    connect(m_pListView, SIGNAL(clicked ( QListViewItem * ) ),
             SLOT(slotEditEntry()) );

    QMap< QString, KoAutoFormatEntry >::Iterator it = m_autoFormat.firstAutoFormatEntry();
    for ( ; it != m_autoFormat.lastAutoFormatEntry(); ++it )
        ( void )new QListViewItem( m_pListView, it.key(), it.data().replace() );

    QVBox *buttons = new QVBox( tab2 );
    buttons->setSpacing( 5 );
    buttons->setMargin( 5 );

    pbAdd = new QPushButton( i18n( "Add"), buttons  );
    connect(pbAdd,SIGNAL(clicked()),this, SLOT(slotAddEntry()));

    pbRemove = new QPushButton( i18n( "Remove" ), buttons );
    connect(pbRemove,SIGNAL(clicked()),this,SLOT(slotRemoveEntry()));
    ( void )new QWidget( buttons );
    pbRemove->setEnabled(false);
    pbAdd->setEnabled(false);
}


void KoAutoFormatDia::setupTab3()
{
    tab3 = addPage( i18n( "Exceptions" ) );
    QVBoxLayout *grid = new QVBoxLayout(tab3, 5, 5);
    grid->setAutoAdd( true );

    abbreviation=new KoAutoFormatExceptionWidget(tab3,i18n("Do not treat as the end of a sentence:"),m_autoFormat.listException(),true);
    ( void )new QWidget( tab3 );
    twoUpperLetter=new KoAutoFormatExceptionWidget(tab3,i18n("Accept two uppercase letters in:"),m_autoFormat.listTwoUpperLetterException());
    ( void )new QWidget( tab3 );
}


void KoAutoFormatDia::slotRemoveEntry()
{
    //find entry in listbox
   if(m_pListView->currentItem())
    {
        m_autoFormat.removeAutoFormatEntry(m_pListView->currentItem()->text(0));
        pbAdd->setText(i18n("Add"));
        refreshEntryList();
    }
}


void KoAutoFormatDia::slotfind( const QString & )
{
    QMap< QString, KoAutoFormatEntry >::Iterator it = m_autoFormat.findFormatEntry(m_find->text());
    if ( !(it == m_autoFormat.lastAutoFormatEntry()) )
    {
        m_replace->setText(it.data().replace().latin1());
	pbAdd->setText(i18n("Modify"));
    } else {
        m_replace->setText("");
	pbAdd->setText(i18n("Add"));
    }
    slotfind2("");
}


void KoAutoFormatDia::slotfind2( const QString & )
{
    bool state = !m_replace->text().isEmpty() && !m_find->text().isEmpty();
    pbRemove->setEnabled(state && m_pListView->currentItem ());
    pbAdd->setEnabled(state);
}


void KoAutoFormatDia::refreshEntryList()
{
    QMap< QString, KoAutoFormatEntry >::Iterator it = m_autoFormat.firstAutoFormatEntry();
    m_pListView->clear();
    for ( ; it != m_autoFormat.lastAutoFormatEntry(); ++it )
        ( void )new QListViewItem( m_pListView, it.key(), it.data().replace() );
    m_pListView->setCurrentItem(m_pListView->firstChild ());
    bool state = !(m_replace->text().isEmpty()) && !(m_find->text().isEmpty());
    //we can delete item, as we search now in listbox and not in m_find lineedit
    pbRemove->setEnabled(m_pListView->currentItem() && m_pListView->selectedItem()!=0 );
    pbAdd->setEnabled(state);
}


void KoAutoFormatDia::addEntryList(const QString &key, KoAutoFormatEntry &_autoEntry)
{
    m_autoFormat.addAutoFormatEntry( key, _autoEntry );
}


void KoAutoFormatDia::editEntryList(const QString &key,const QString &newFindString, KoAutoFormatEntry &_autoEntry)
{
    m_autoFormat.removeAutoFormatEntry( key );
    m_autoFormat.addAutoFormatEntry( newFindString, _autoEntry );
}


void KoAutoFormatDia::slotAddEntry()
{
    if(!pbAdd->isEnabled())
        return;
    QString repl = m_replace->text();
    QString find = m_find->text();
    if(repl.isEmpty() || find.isEmpty())
    {
        KMessageBox::sorry( 0L, i18n( "An area is empty" ) );
        return;
    }
    if(repl==find)
    {
        KMessageBox::sorry( 0L, i18n( "Find string is the same as replace string!" ) );
	return;
    }
    KoAutoFormatEntry tmp = KoAutoFormatEntry( repl );

    if(pbAdd->text()=="Add")
        addEntryList(find, tmp);
    else
        editEntryList(find, find, tmp);

    m_find->setText( "" );
    m_replace->setText( "" );
    refreshEntryList();
}


void KoAutoFormatDia::chooseSpecialChar1()
{
    QString f = font().family();
    QChar c = ' ';
    if ( KoCharSelectDia::selectChar( f, c, false ) )
        m_find->setText( c );
}


void KoAutoFormatDia::chooseSpecialChar2()
{
    QString f = font().family();
    QChar c = ' ';
    if ( KoCharSelectDia::selectChar( f, c, false ) )
        m_replace->setText( c );
}


void KoAutoFormatDia::slotItemRenamed(QListViewItem *, const QString & , int )
{
    // Wow. This need a redesign (we don't have the old key anymore at this point !)
    // -> inherit QListViewItem and store the KoAutoFormatEntry pointer in it.
}


void KoAutoFormatDia::slotEditEntry()
{
    if(m_pListView->currentItem()==0)
        return;
    m_find->setText(m_pListView->currentItem()->text(0));
    m_replace->setText(m_pListView->currentItem()->text(1));
    bool state = !m_replace->text().isEmpty() && !m_find->text().isEmpty();
    pbRemove->setEnabled(state);
    pbAdd->setEnabled(state);
}


bool KoAutoFormatDia::applyConfig()
{
    // First tab
    KoAutoFormat::TypographicQuotes tq = m_autoFormat.getConfigTypographicQuotes();
    tq.replace = cbTypographicQuotes->isChecked();
    tq.begin = pbQuote1->text()[ 0 ];
    tq.end = pbQuote2->text()[ 0 ];
    m_docAutoFormat->configTypographicQuotes( tq );

    m_docAutoFormat->configUpperCase( cbUpperCase->isChecked() );
    m_docAutoFormat->configUpperUpper( cbUpperUpper->isChecked() );
    m_docAutoFormat->configAutoDetectUrl( cbDetectUrl->isChecked() );

    m_docAutoFormat->configIgnoreDoubleSpace( cbIgnoreDoubleSpace->isChecked());
    m_docAutoFormat->configRemoveSpaceBeginEndLine( cbRemoveSpaceBeginEndLine->isChecked());
    m_docAutoFormat->configUseBulletStyle(cbUseBulletStyle->isChecked());


    // Second tab
    m_docAutoFormat->copyAutoFormatEntries( m_autoFormat );
    m_docAutoFormat->copyListException(abbreviation->getListException());
    m_docAutoFormat->copyListTwoUpperCaseException(twoUpperLetter->getListException());
    m_docAutoFormat->configAdvancedAutocorrect( cbAdvancedAutoCorrection->isChecked() );
    // Save to config file
    m_docAutoFormat->saveConfig();

    return true;
}

void KoAutoFormatDia::slotOk()
{
    if (applyConfig())
    {
       KDialogBase::slotOk();
    }
}

void KoAutoFormatDia::chooseQuote1()
{
    QString f = font().family();
    QChar c = oBegin;
    if ( KoCharSelectDia::selectChar( f, c, false ) )
    {
        pbQuote1->setText( c );
    }
}

void KoAutoFormatDia::chooseQuote2()
{
    QString f = font().family();
    QChar c = oEnd;
    if ( KoCharSelectDia::selectChar( f, c, false ) )
    {
        pbQuote2->setText( c );
    }
}


void KoAutoFormatDia::defaultQuote()
{
    pbQuote1->setText("«");
    pbQuote2->setText("»");
}

void KoAutoFormatDia::slotChangeState(bool b)
{
    pbQuote1->setEnabled(b);
    pbQuote2->setEnabled(b);
    pbDefault->setEnabled(b);
}
