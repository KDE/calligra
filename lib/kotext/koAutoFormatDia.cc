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
#include <qwhatsthis.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <kdebug.h>
#include <knuminput.h>
#include <kcompletion.h>
#include <kconfig.h>

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

KoAutoFormatExceptionWidget::KoAutoFormatExceptionWidget(QWidget *parent, const QString &name,const QStringList &_list, bool _autoInclude, bool _abreviation)
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
    grid->addMultiCellWidget(exceptionList,2,5,0,0);
    pbRemoveException->setEnabled( exceptionList->currentItem()!=-1 && m_listException.count()>0);
    connect(exceptionLine ,SIGNAL(textChanged ( const QString & )),this,SLOT(textChanged ( const QString & )));

    connect( exceptionList , SIGNAL(selectionChanged () ),
            this,SLOT(slotExceptionListSelected()) );

    pbAddException->setEnabled(false);

    cbAutoInclude = new QCheckBox( i18n("Auto includes"), this );
    grid->addMultiCellWidget(cbAutoInclude,6,6,0,1);
    cbAutoInclude->setChecked( _autoInclude );
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
        if( m_listException.findIndex( text )==-1)
        {
            m_listException<<text;

            exceptionList->clear();
            exceptionList->insertStringList(m_listException);
            pbRemoveException->setEnabled( exceptionList->currentItem()!=-1 && m_listException.count()>0);
            pbAddException->setEnabled(false);
        }
        exceptionLine->clear();
    }
}

void KoAutoFormatExceptionWidget::slotRemoveException()
{
    if(!exceptionList->currentText().isEmpty())
    {
        m_listException.remove(exceptionList->currentText());
        exceptionList->clear();
        pbAddException->setEnabled(false);
        pbRemoveException->setEnabled( exceptionList->currentItem()!=-1 && m_listException.count()>0);
        exceptionList->insertStringList(m_listException);
        exceptionLine->clear();
    }
}

bool KoAutoFormatExceptionWidget::autoInclude()
{
    return cbAutoInclude->isChecked();
}

void KoAutoFormatExceptionWidget::setListException( const QStringList &list)
{
    exceptionList->clear();
    exceptionList->insertStringList(list);
}

void KoAutoFormatExceptionWidget::setAutoInclude(bool b)
{
    cbAutoInclude->setChecked( b );
}

void KoAutoFormatExceptionWidget::slotExceptionListSelected()
{
    pbRemoveException->setEnabled( exceptionList->currentItem()!=-1 );
}

/******************************************************************/
/* Class: KoAutoFormatDia                                         */
/******************************************************************/

KoAutoFormatDia::KoAutoFormatDia( QWidget *parent, const char *name, KoAutoFormat * autoFormat )
    : KDialogBase( Tabbed, i18n("Autocorrection"), Ok | Cancel | User1, Ok, parent, name, true),
      oSimpleBegin( autoFormat->getConfigTypographicSimpleQuotes().begin ),
      oSimpleEnd( autoFormat->getConfigTypographicSimpleQuotes().end ),
      oDoubleBegin( autoFormat->getConfigTypographicDoubleQuotes().begin ),
      oDoubleEnd( autoFormat->getConfigTypographicDoubleQuotes().end ),
      bulletStyle( autoFormat->getConfigBulletStyle()),
      m_autoFormat( *autoFormat ),
      m_docAutoFormat( autoFormat )
{
    setButtonText( KDialogBase::User1, i18n("Reset") );

    setupTab1();
    setupTab2();
    setupTab3();
    setupTab4();
    setInitialSize( QSize(500, 300) );
    connect( this, SIGNAL( user1Clicked() ), this, SLOT(slotResetConf()));

}

void KoAutoFormatDia::slotResetConf()
{
    switch( activePageIndex() ) {
    case 0:
        initTab1();
        break;
    case 1:
        initTab2();
        break;
    case 2:
        initTab3();
        break;
    case 3:
        initTab4();
        break;
    default:
        break;
    }
}

void KoAutoFormatDia::setupTab1()
{
    tab1 = addPage( i18n( "Simple Autocorrection" ) );
    QVBoxLayout *grid = new QVBoxLayout(tab1, 10, 5);
    grid->setAutoAdd( true );

    cbUpperCase = new QCheckBox( tab1 );
    cbUpperCase->setText( i18n( "Convert &first letter of a sentence automatically to &uppercase\n"
                                "(e.g. \"my house. in this town\" to \"my house. In this town\")" ) );
    QWhatsThis::add( cbUpperCase, i18n("Detect when a new sentence is started and always ensure that the first character is an uppercase character."));
    cbUpperCase->resize( cbUpperCase->sizeHint() );


    ( void )new QWidget( tab1 );

    cbUpperUpper = new QCheckBox( tab1 );
    cbUpperUpper->setText( i18n( "Convert two uppercase characters to one uppercase and one lowercase character.\n"
                                 "(e.g. PErfect to Perfect)" ) );
    QWhatsThis::add( cbUpperUpper, i18n("All words are checked for the common mistake of holding the shift key down a bit too long. If some words must have two uppercase characters, then those exceptions should be added in the %1 tab.").arg(i18n("Exceptions")));
    cbUpperUpper->resize( cbUpperUpper->sizeHint() );
    ( void )new QWidget( tab1 );

    cbDetectUrl=new QCheckBox( tab1 );
    cbDetectUrl->setText( i18n( "Auto format &URLs" ) );
    QWhatsThis::add( cbDetectUrl, i18n("Detect when a URL (Universal Remote Location) is typed and provide formatting that matches the way an Internet browser would show a URL."));
    cbDetectUrl->resize( cbDetectUrl->sizeHint() );
    ( void )new QWidget( tab1 );

    cbIgnoreDoubleSpace=new QCheckBox( tab1 );
    cbIgnoreDoubleSpace->setText( i18n( "&Suppress double &spaces" ) );
    QWhatsThis::add( cbIgnoreDoubleSpace, i18n("Make sure that more than one space cannot be typed, as this is a common mistake which is quite hard to find in formatted text."));
    cbIgnoreDoubleSpace->resize( cbIgnoreDoubleSpace->sizeHint() );
    ( void )new QWidget( tab1 );

    cbRemoveSpaceBeginEndLine=new QCheckBox( tab1 );
    cbRemoveSpaceBeginEndLine->setText( i18n( "&Remove spaces at the beginning and end of paragraphs" ) );
    QWhatsThis::add( cbRemoveSpaceBeginEndLine, i18n("Keep correct formatting and indenting of sentences by automatically removing spaces typed at the beginning and end of a paragraph."));
    cbRemoveSpaceBeginEndLine->resize( cbRemoveSpaceBeginEndLine->sizeHint() );
    ( void )new QWidget( tab1 );

    cbAutoChangeFormat=new QCheckBox( tab1 );
    cbAutoChangeFormat->setText( i18n( "Automatically do &bold and underline formatting") );
    QWhatsThis::add( cbAutoChangeFormat, i18n("When you use _underline_ or *bold*, the text between the underscores or asterisks will be converted to underlined or bold text.") );
    cbAutoChangeFormat->resize( cbAutoChangeFormat->sizeHint() );
    ( void )new QWidget( tab1 );

    cbAutoReplaceNumber=new QCheckBox( tab1 );
    cbAutoReplaceNumber->setText( i18n( "We add the 1/2 char at the %1", "Replace 1/2... with %1..." ).arg(QString("½")) );
    QWhatsThis::add( cbAutoReplaceNumber, i18n("Most standard fraction notations will be converted when available") );
    cbAutoReplaceNumber->resize( cbAutoReplaceNumber->sizeHint() );
    ( void )new QWidget( tab1 );

    cbUseNumberStyle=new QCheckBox( tab1 );
    cbUseNumberStyle->setText( i18n( "Use auto-numbering for numbered paragraphs" ) );
    QWhatsThis::add( cbUseNumberStyle, i18n("When typing '1)' or similar in front of a paragraph, automatically convert the paragraph to use that numbering style. This has the advantage that further paragraphs will also be numbered and the spacing is done correctly.") );
    cbUseNumberStyle->resize( cbUseNumberStyle->sizeHint() );
    ( void )new QWidget( tab1 );

    cbAutoSuperScript = new QCheckBox( tab1 );
    cbAutoSuperScript->setText( i18n("Replace 1st... with 1^st"));
    cbAutoSuperScript->resize( cbAutoSuperScript->sizeHint() );
    cbAutoSuperScript->setEnabled( m_autoFormat.nbSuperScriptEntry()>0 );
    ( void )new QWidget( tab1 );

    cbUseBulletStyle=new QCheckBox( tab1 );
    cbUseBulletStyle->setText( i18n( "Use lists-formatting for bulletted paragraphs" ) );
    QWhatsThis::add( cbUseBulletStyle, i18n("When typing '*' or '-' in front of a paragraph, automatically convert the paragraph to use that list-style. Using a list-style formatting means that a correct bullet is used to draw the list.") );
    cbUseBulletStyle->resize( cbUseBulletStyle->sizeHint() );
    ( void )new QWidget( tab1 );


    QHBox *quotes = new QHBox( tab1 );
    quotes->setSpacing( 5 );
    pbBulletStyle = new QPushButton( quotes );

    pbBulletStyle->resize( pbBulletStyle->sizeHint() );

    pbDefaultBulletStyle = new QPushButton( quotes );

    pbDefaultBulletStyle->setText(i18n("Default"));

    pbDefaultBulletStyle->resize( pbDefaultBulletStyle->sizeHint() );
    ( void )new QWidget( quotes );

    initTab1();

    connect( pbBulletStyle, SIGNAL( clicked() ), this, SLOT( chooseBulletStyle() ) );
    connect( pbDefaultBulletStyle, SIGNAL( clicked()), this, SLOT( defaultBulletStyle() ) );
}

void KoAutoFormatDia::initTab1()
{
    cbUpperCase->setChecked( m_autoFormat.getConfigUpperCase() );
    cbUpperUpper->setChecked( m_autoFormat.getConfigUpperUpper() );
    cbDetectUrl->setChecked( m_autoFormat.getConfigAutoDetectUrl());
    cbIgnoreDoubleSpace->setChecked( m_autoFormat.getConfigIgnoreDoubleSpace());
    cbRemoveSpaceBeginEndLine->setChecked( m_autoFormat.getConfigRemoveSpaceBeginEndLine());
    cbAutoChangeFormat->setChecked( m_autoFormat.getConfigAutoChangeFormat());
    cbAutoReplaceNumber->setChecked( m_autoFormat.getConfigAutoReplaceNumber());
    cbUseNumberStyle->setChecked( m_autoFormat.getConfigAutoNumberStyle());
    cbUseBulletStyle->setChecked( m_autoFormat.getConfigUseBulletSyle());
    pbBulletStyle->setText( bulletStyle );

    cbAutoSuperScript->setChecked( m_autoFormat.getConfigAutoSuperScript());
}

void KoAutoFormatDia::setupTab2()
{
    tab2 = addPage( i18n( "Custom Quotes" ) );
    QVBoxLayout *grid = new QVBoxLayout(tab2, 10, 5);
    grid->setAutoAdd( true );

    cbTypographicDoubleQuotes = new QCheckBox( tab2 );
    cbTypographicDoubleQuotes->setText( i18n( "Replace double &quotes with typographical quotes:" ) );
    cbTypographicDoubleQuotes->resize( cbTypographicDoubleQuotes->sizeHint() );


    connect( cbTypographicDoubleQuotes,SIGNAL(toggled ( bool)),this,SLOT(slotChangeStateDouble(bool)));

    QHBox *doubleQuotes = new QHBox( tab2 );
    doubleQuotes->setSpacing( 5 );
    pbDoubleQuote1 = new QPushButton( doubleQuotes );


    pbDoubleQuote1->resize( pbDoubleQuote1->sizeHint() );
    pbDoubleQuote2 = new QPushButton( doubleQuotes );


    pbDoubleQuote2->resize( pbDoubleQuote2->sizeHint() );
    ( void )new QWidget( doubleQuotes );
    doubleQuotes->setMaximumHeight( pbDoubleQuote1->sizeHint().height() );


    pbDoubleDefault = new QPushButton( doubleQuotes );

    pbDoubleDefault->setText(i18n("Default"));

    pbDoubleDefault->resize( pbDoubleDefault->sizeHint() );
    ( void )new QWidget( doubleQuotes );


    connect( pbDoubleQuote1, SIGNAL( clicked() ), this, SLOT( chooseDoubleQuote1() ) );
    connect( pbDoubleQuote2, SIGNAL( clicked() ), this, SLOT( chooseDoubleQuote2() ) );
    connect( pbDoubleDefault, SIGNAL( clicked()), this, SLOT( defaultDoubleQuote() ) );

    ( void )new QWidget( tab2 );



    cbTypographicSimpleQuotes = new QCheckBox( tab2 );
    cbTypographicSimpleQuotes->setText( i18n( "Replace simple &quotes with typographical quotes:" ) );
    cbTypographicSimpleQuotes->resize( cbTypographicSimpleQuotes->sizeHint() );


    connect( cbTypographicSimpleQuotes,SIGNAL(toggled ( bool)),this,SLOT(slotChangeStateSimple(bool)));

    QHBox *simpleQuotes = new QHBox( tab2 );
    simpleQuotes->setSpacing( 5 );
    pbSimpleQuote1 = new QPushButton( simpleQuotes );


    pbSimpleQuote1->resize( pbSimpleQuote1->sizeHint() );
    pbSimpleQuote2 = new QPushButton( simpleQuotes );


    pbSimpleQuote2->resize( pbSimpleQuote2->sizeHint() );
    ( void )new QWidget( simpleQuotes );
    simpleQuotes->setMaximumHeight( pbSimpleQuote1->sizeHint().height() );


    pbSimpleDefault = new QPushButton( simpleQuotes );

    pbSimpleDefault->setText(i18n("Default"));

    pbSimpleDefault->resize( pbSimpleDefault->sizeHint() );
    ( void )new QWidget( simpleQuotes );


    connect( pbSimpleQuote1, SIGNAL( clicked() ), this, SLOT( chooseSimpleQuote1() ) );
    connect( pbSimpleQuote2, SIGNAL( clicked() ), this, SLOT( chooseSimpleQuote2() ) );
    connect( pbSimpleDefault, SIGNAL( clicked()), this, SLOT( defaultSimpleQuote() ) );

    ( void )new QWidget( tab2 );

    initTab2();
}

void KoAutoFormatDia::initTab2()
{
    bool state=m_autoFormat.getConfigTypographicDoubleQuotes().replace;
    cbTypographicDoubleQuotes->setChecked( state );
    pbDoubleQuote1->setText( oDoubleBegin );
    pbDoubleQuote2->setText(oDoubleEnd );
    slotChangeStateDouble(state);

    state=m_autoFormat.getConfigTypographicSimpleQuotes().replace;
    cbTypographicSimpleQuotes->setChecked( state );
    pbSimpleQuote1->setText( oSimpleBegin );
    pbSimpleQuote2->setText(oSimpleEnd );
    slotChangeStateSimple(state);

}

void KoAutoFormatDia::setupTab3()
{
    tab3 = addPage( i18n( "Advanced Autocorrection" ) );
    QHBoxLayout *grid = new QHBoxLayout(tab3, 10, 5);
    grid->setAutoAdd( true );

    QVBox *left = new QVBox( tab3 );
    cbAdvancedAutoCorrection=new QCheckBox(i18n("Active autocorrection"),left);
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


    QVBox *buttons = new QVBox( tab3 );
    buttons->setSpacing( 5 );
    buttons->setMargin( 5 );

    pbAdd = new QPushButton( i18n( "Add"), buttons  );
    connect(pbAdd,SIGNAL(clicked()),this, SLOT(slotAddEntry()));

    pbRemove = new QPushButton( i18n( "Remove" ), buttons );
    connect(pbRemove,SIGNAL(clicked()),this,SLOT(slotRemoveEntry()));
    ( void )new QWidget( buttons );
    pbRemove->setEnabled(false);
    pbAdd->setEnabled(false);
    initTab3();
}

void KoAutoFormatDia::initTab3()
{
    cbAdvancedAutoCorrection->setChecked(m_autoFormat.getConfigAdvancedAutoCorrect());
    m_pListView->clear();
    QMap< QString, KoAutoFormatEntry >::Iterator it = m_autoFormat.firstAutoFormatEntry();
    for ( ; it != m_autoFormat.lastAutoFormatEntry(); ++it )
        ( void )new QListViewItem( m_pListView, it.key(), it.data().replace() );

}

void KoAutoFormatDia::setupTab4()
{
    tab4 = addPage( i18n( "Exceptions" ) );
    QVBoxLayout *grid = new QVBoxLayout(tab4, 5, 5);
    grid->setAutoAdd( true );

    abbreviation=new KoAutoFormatExceptionWidget(tab4,i18n("Do not treat as the end of a sentence:"), m_autoFormat.listException(),m_autoFormat.getConfigIncludeAbbreviation() , true);
    ( void )new QWidget( tab4 );
    twoUpperLetter=new KoAutoFormatExceptionWidget(tab4,i18n("Accept two uppercase letters in:"),m_autoFormat.listTwoUpperLetterException(),m_autoFormat.getConfigIncludeTwoUpperUpperLetterException());
    ( void )new QWidget( tab4 );
    initTab4();
}

void KoAutoFormatDia::initTab4()
{
    abbreviation->setListException( m_docAutoFormat->listException() );
    abbreviation->setAutoInclude( m_docAutoFormat->getConfigIncludeAbbreviation() );
    twoUpperLetter->setListException( m_docAutoFormat->listTwoUpperLetterException() );
    twoUpperLetter->setAutoInclude( m_docAutoFormat->getConfigIncludeTwoUpperUpperLetterException() );
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
        m_pListView->setCurrentItem(m_pListView->findItem(m_find->text(),0));

    } else {
        m_replace->setText("");
	pbAdd->setText(i18n("Add"));
        m_pListView->setCurrentItem(0L);
    }
    slotfind2("");
}


void KoAutoFormatDia::slotfind2( const QString & )
{
    bool state = !m_replace->text().isEmpty() && !m_find->text().isEmpty();
    QMap< QString, KoAutoFormatEntry >::Iterator it = m_autoFormat.findFormatEntry(m_find->text());
    pbRemove->setEnabled(state && !(it == m_autoFormat.lastAutoFormatEntry() ));
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
    KoAutoFormat::TypographicQuotes tq = m_autoFormat.getConfigTypographicSimpleQuotes();
    tq.replace = cbTypographicSimpleQuotes->isChecked();
    tq.begin = pbSimpleQuote1->text()[ 0 ];
    tq.end = pbSimpleQuote2->text()[ 0 ];
    m_docAutoFormat->configTypographicSimpleQuotes( tq );

    tq = m_autoFormat.getConfigTypographicDoubleQuotes();
    tq.replace = cbTypographicDoubleQuotes->isChecked();
    tq.begin = pbDoubleQuote1->text()[ 0 ];
    tq.end = pbDoubleQuote2->text()[ 0 ];
    m_docAutoFormat->configTypographicDoubleQuotes( tq );


    m_docAutoFormat->configUpperCase( cbUpperCase->isChecked() );
    m_docAutoFormat->configUpperUpper( cbUpperUpper->isChecked() );
    m_docAutoFormat->configAutoDetectUrl( cbDetectUrl->isChecked() );

    m_docAutoFormat->configIgnoreDoubleSpace( cbIgnoreDoubleSpace->isChecked());
    m_docAutoFormat->configRemoveSpaceBeginEndLine( cbRemoveSpaceBeginEndLine->isChecked());
    m_docAutoFormat->configUseBulletStyle(cbUseBulletStyle->isChecked());

    m_docAutoFormat->configBulletStyle(pbBulletStyle->text()[ 0 ]);

    m_docAutoFormat->configAutoChangeFormat( cbAutoChangeFormat->isChecked());

    m_docAutoFormat->configAutoReplaceNumber( cbAutoReplaceNumber->isChecked());
    m_docAutoFormat->configAutoNumberStyle(cbUseNumberStyle->isChecked());

    m_docAutoFormat->configAutoSuperScript ( cbAutoSuperScript->isChecked() );

    // Second tab
    m_docAutoFormat->copyAutoFormatEntries( m_autoFormat );
    m_docAutoFormat->copyListException(abbreviation->getListException());
    m_docAutoFormat->copyListTwoUpperCaseException(twoUpperLetter->getListException());
    m_docAutoFormat->configAdvancedAutocorrect( cbAdvancedAutoCorrection->isChecked() );


    m_docAutoFormat->configIncludeTwoUpperUpperLetterException( twoUpperLetter->autoInclude());
    m_docAutoFormat->configIncludeAbbreviation( abbreviation->autoInclude());

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

void KoAutoFormatDia::chooseDoubleQuote1()
{
    QString f = font().family();
    QChar c = oDoubleBegin;
    if ( KoCharSelectDia::selectChar( f, c, false ) )
    {
        pbDoubleQuote1->setText( c );
    }
}

void KoAutoFormatDia::chooseDoubleQuote2()
{
    QString f = font().family();
    QChar c = oDoubleEnd;
    if ( KoCharSelectDia::selectChar( f, c, false ) )
    {
        pbDoubleQuote2->setText( c );
    }
}


void KoAutoFormatDia::defaultDoubleQuote()
{
    pbDoubleQuote1->setText(m_docAutoFormat->getDefaultTypographicDoubleQuotes().begin);
    pbDoubleQuote2->setText(m_docAutoFormat->getDefaultTypographicDoubleQuotes().end);
}

void KoAutoFormatDia::chooseSimpleQuote1()
{
    QString f = font().family();
    QChar c = oSimpleBegin;
    if ( KoCharSelectDia::selectChar( f, c, false ) )
    {
        pbSimpleQuote1->setText( c );
    }
}

void KoAutoFormatDia::chooseSimpleQuote2()
{
    QString f = font().family();
    QChar c = oSimpleEnd;
    if ( KoCharSelectDia::selectChar( f, c, false ) )
    {
        pbSimpleQuote2->setText( c );
    }
}

void KoAutoFormatDia::defaultSimpleQuote()
{

    pbSimpleQuote1->setText(m_docAutoFormat->getDefaultTypographicSimpleQuotes().begin);
    pbSimpleQuote2->setText(m_docAutoFormat->getDefaultTypographicSimpleQuotes().end);
}


void KoAutoFormatDia::chooseBulletStyle()
{
    QString f = font().family();
    QChar c = bulletStyle;
    if ( KoCharSelectDia::selectChar( f, c, false ) )
    {
        pbBulletStyle->setText( c );
    }
}

void KoAutoFormatDia::defaultBulletStyle()
{
    pbBulletStyle->setText( "" );
}

void KoAutoFormatDia::slotChangeStateSimple(bool b)
{
    pbSimpleQuote1->setEnabled(b);
    pbSimpleQuote2->setEnabled(b);
    pbSimpleDefault->setEnabled(b);
}

void KoAutoFormatDia::slotChangeStateDouble(bool b)
{
    pbDoubleQuote1->setEnabled(b);
    pbDoubleQuote2->setEnabled(b);
    pbDoubleDefault->setEnabled(b);
}


/******************************************************************/
/* Class: KoCompletionDia                                         */
/******************************************************************/

KoCompletionDia::KoCompletionDia( QWidget *parent, const char *name, KoAutoFormat * autoFormat )
    : KDialogBase( parent, name , true, "", Ok|Cancel|User1, Ok, true ),
      m_autoFormat( *autoFormat ),
      m_docAutoFormat( autoFormat )
{
    setButtonText( KDialogBase::User1, i18n("&Reset") );
    setCaption( i18n("Completion") );
    setup();
    slotResetConf();
    setInitialSize( QSize( 400, 400 ) );
    connect( this, SIGNAL( user1Clicked() ), this, SLOT(slotResetConf()));
}

void KoCompletionDia::setup()
{
    QVBox *page = makeVBoxMainWidget();
    cbAllowCompletion = new QCheckBox( page );
    cbAllowCompletion->setText( i18n( "E&nable completion" ) );
    // TODO whatsthis or text, to tell about the key to use for autocompletion....
    cbAddCompletionWord = new QCheckBox( page );
    cbAddCompletionWord->setText( i18n( "&Automatically add new words to completion list" ) );
    QWhatsThis::add( cbAddCompletionWord, i18n("If this is option is enabled, any word typed in this document will automatically be added to the list of words used by the completion." ) );

    m_lbListCompletion = new QListBox( page );
    connect( m_lbListCompletion, SIGNAL( selected ( const QString & ) ), this, SLOT( slotCompletionWordSelected( const QString & )));
    connect( m_lbListCompletion, SIGNAL( highlighted ( const QString & ) ), this, SLOT( slotCompletionWordSelected( const QString & )));


    pbRemoveCompletionEntry = new QPushButton(i18n( "R&emove Completion Entry"), page  );
    connect( pbRemoveCompletionEntry, SIGNAL( clicked() ), this, SLOT( slotRemoveCompletionEntry()));

    pbSaveCompletionEntry= new QPushButton(i18n( "&Save Completion List"), page );
    connect( pbSaveCompletionEntry, SIGNAL( clicked() ), this, SLOT( slotSaveCompletionEntry()));


    m_minWordLength = new KIntNumInput( page );
    m_minWordLength->setRange ( 5, 100,1,true );
    m_minWordLength->setLabel( i18n( "&Minimum word length:" ) );

    m_maxNbWordCompletion = new KIntNumInput( page );
    m_maxNbWordCompletion->setRange( 1, 500, 1, true);
    m_maxNbWordCompletion->setLabel( i18n( "Ma&ximum number of completion words:" ) );

    cbAppendSpace = new QCheckBox( page );
    cbAppendSpace->setText( i18n( "A&ppend space" ) );

    m_listCompletion = m_docAutoFormat->listCompletion();
}

void KoCompletionDia::slotResetConf()
{
   cbAllowCompletion->setChecked( m_autoFormat.getConfigCompletion());
    cbAddCompletionWord->setChecked( m_autoFormat.getConfigAddCompletionWord());
    m_lbListCompletion->clear();
    QStringList lst = m_docAutoFormat->listCompletion();
    m_lbListCompletion->insertStringList( lst );
    if( lst.isEmpty() || m_lbListCompletion->currentText().isEmpty())
        pbRemoveCompletionEntry->setEnabled( false );
    m_minWordLength->setValue ( m_docAutoFormat->getConfigMinWordLength() );
    m_maxNbWordCompletion->setValue ( m_docAutoFormat->getConfigNbMaxCompletionWord() );
    cbAppendSpace->setChecked( m_autoFormat.getConfigAppendSpace() );
}

void KoCompletionDia::slotSaveCompletionEntry()
{

    KConfig config("kofficerc");
    KConfigGroupSaver cgs( &config, "Completion Word" );
    config.writeEntry( "list", m_listCompletion );
    config.sync();
    KMessageBox::information( this, i18n("Completion list saved.\nIt will be used for all documents from now on."),
                              i18n("Completion List Saved") /* + dontShowAgainName? */ );
}

void KoCompletionDia::slotOk()
{
    if (applyConfig())
    {
       KDialogBase::slotOk();
    }
}

bool KoCompletionDia::applyConfig()
{

    m_docAutoFormat->configCompletion( cbAllowCompletion->isChecked());
    m_docAutoFormat->configAppendSpace( cbAppendSpace->isChecked() );
    m_docAutoFormat->configMinWordLength( m_minWordLength->value() );
    m_docAutoFormat->configNbMaxCompletionWord( m_maxNbWordCompletion->value () );
    m_docAutoFormat->configAddCompletionWord( cbAddCompletionWord->isChecked());

    m_docAutoFormat->getCompletion()->setItems( m_listCompletion );
    // Save to config file
    m_docAutoFormat->saveConfig();
    return true;
}

void KoCompletionDia::slotRemoveCompletionEntry()
{
    QString text = m_lbListCompletion->currentText();
    if( !text.isEmpty() )
    {
        m_listCompletion.remove( text );
        m_lbListCompletion->removeItem( m_lbListCompletion->currentItem () );
        if( m_lbListCompletion->count()==0 )
            pbRemoveCompletionEntry->setEnabled( false );
    }
}

void KoCompletionDia::slotCompletionWordSelected( const QString & word)
{
    pbRemoveCompletionEntry->setEnabled( !word.isEmpty() );
}
