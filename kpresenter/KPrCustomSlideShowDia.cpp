/* This file is part of the KDE project
   Copyright (C) 2004 Laurent Montel <montel@kde.org>

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

#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <q3listbox.h>
#include <QPushButton>
#include <qtoolbutton.h>
#include <qapplication.h>
#include <QLayout>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3ValueList>
#include <Q3PtrList>
#include <QHideEvent>
#include <Q3VBoxLayout>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kvbox.h>

#include "KPrPage.h"
#include "KPrDocument.h"
#include "KPrView.h"

#include "KPrCustomSlideShowDia.h"


KPrCustomSlideShowDia::KPrCustomSlideShowDia( KPrView* _view, KPrDocument *_doc, const char* name )
  : KDialog( _view ),
    m_doc( _doc ),
    m_view( _view ),
    m_customSlideShowMap( m_doc->customSlideShows() )
{
  setCaption( i18n("Custom Slide Show") );
  setButtons(Ok|Cancel);
  QWidget* page = new QWidget( this );
  setMainWidget( page );

  Q3GridLayout *grid1 = new Q3GridLayout( page,10,3,KDialog::marginHint(), KDialog::spacingHint());

  list=new Q3ListBox(page);
  grid1->addMultiCellWidget(list,0,8,0,0);

  m_pAdd=new QPushButton(i18n("&Add..."),page);
  grid1->addWidget(m_pAdd,1,2);

  m_pModify=new QPushButton(i18n("&Modify..."),page);
  grid1->addWidget(m_pModify,2,2);

  m_pRemove=new QPushButton(i18n("&Remove"),page);
  grid1->addWidget(m_pRemove,3,2);

  m_pCopy=new QPushButton(i18n("Co&py"),page);
  grid1->addWidget(m_pCopy,4,2);

  m_pTest=new QPushButton(i18n("Test"),page);
  grid1->addWidget(m_pTest,5,2);


  connect( m_pRemove, SIGNAL( clicked() ), this, SLOT( slotRemove() ) );
  connect( m_pAdd, SIGNAL( clicked() ), this, SLOT( slotAdd() ) );
  connect( m_pModify, SIGNAL( clicked() ), this, SLOT( slotModify() ) );
  connect( m_pCopy, SIGNAL( clicked() ), this, SLOT( slotCopy() ) );
  connect( m_pTest, SIGNAL( clicked() ), this, SLOT( slotTest() ) );

  connect( list, SIGNAL(doubleClicked(Q3ListBoxItem *)),this,SLOT(slotDoubleClicked(Q3ListBoxItem *)));
  connect( list, SIGNAL(clicked ( Q3ListBoxItem * )),this,SLOT(slotTextClicked(Q3ListBoxItem * )));

  connect( m_view, SIGNAL( presentationFinished() ), this, SLOT( slotPresentationFinished() ) );

  init();
  updateButton();

  resize( 600, 250 );

  m_bChanged=false;
}


KPrCustomSlideShowDia::~KPrCustomSlideShowDia()
{
    kDebug()<<"KPrCustomSlideShowDia::~KPrCustomSlideShowDia()********************\n";
}

void KPrCustomSlideShowDia::init()
{
    CustomSlideShowMap::ConstIterator it( m_customSlideShowMap.begin() );
    for ( ; it != m_customSlideShowMap.end(); ++it )
    {
        list->insertItem( it.key() );
    }
}

void KPrCustomSlideShowDia::updateButton()
{
    bool state = ( list->currentItem() >= 0 );
    m_pRemove->setEnabled( state );
    m_pModify->setEnabled( state );
    m_pCopy->setEnabled( state );
    m_pTest->setEnabled( state );
}

void KPrCustomSlideShowDia::slotTextClicked(Q3ListBoxItem*)
{
    updateButton();
}

void KPrCustomSlideShowDia::slotDoubleClicked(Q3ListBoxItem *)
{
    updateButton();
    slotModify();
}

void KPrCustomSlideShowDia::slotPresentationFinished()
{
    kDebug()<<"void KPrCustomSlideShowDia::slotPresentationFinished()*************************\n";
    show();
}


void KPrCustomSlideShowDia::hideEvent( QHideEvent* )
{
}

void KPrCustomSlideShowDia::slotTest()
{
    Q3ListBoxItem *item = list->selectedItem();
    if ( item )
    {
        m_doc->testCustomSlideShow( m_customSlideShowMap[item->text()], m_view );
        hide();
    }
}

void KPrCustomSlideShowDia::slotAdd()
{
    QStringList listCustomName;
    CustomSlideShowMap::Iterator it( m_customSlideShowMap.begin() ) ;
    for ( ; it != m_customSlideShowMap.end(); ++it )
    {
        listCustomName.append( it.key() );
    }

    KPrDefineCustomSlideShow * dlg = new KPrDefineCustomSlideShow( this, listCustomName, m_doc->getPageList() );
    if ( dlg->exec() )
    {
        //insert new element
        m_customSlideShowMap.insert( dlg->customSlideShowName(), dlg->customSlides() );
        list->insertItem( dlg->customSlideShowName() );
        updateButton();
    }
    delete dlg;

}

void KPrCustomSlideShowDia::slotRemove()
{
    if (list->selectedItem() )
    {
        m_customSlideShowMap.remove( list->selectedItem()->text() );
        list->removeItem( list->currentItem() );
        updateButton();
    }
}

void KPrCustomSlideShowDia::slotOk()
{
    m_doc->setCustomSlideShows( m_customSlideShowMap );
    accept();
}

void KPrCustomSlideShowDia::slotModify()
{
    Q3ListBoxItem *item = list->selectedItem();
    if ( item )
    {
        QStringList listCustomName;
        CustomSlideShowMap::ConstIterator it( m_customSlideShowMap.begin() );
        for ( ; it != m_customSlideShowMap.end(); ++it )
        {
            if ( it.key() !=item->text() )
                listCustomName.append( it.key() );
        }

        KPrDefineCustomSlideShow * dlg = new KPrDefineCustomSlideShow( this, item->text(), listCustomName,
                                                                       m_doc->getPageList(), m_customSlideShowMap[item->text()]);
        if ( dlg->exec() )
        {
            //insert new element
            m_customSlideShowMap.remove( list->selectedItem()->text() );
            m_customSlideShowMap.insert( dlg->customSlideShowName(), dlg->customSlides() );
            list->changeItem( dlg->customSlideShowName(), list->currentItem() );
        }
        delete dlg;
    }

}

void KPrCustomSlideShowDia::slotCopy()
{
    Q3ListBoxItem *item = list->selectedItem();
    if ( item )
    {
        QString str( list->selectedItem()->text() );
        str+=i18n( "(Copy %1)" );
        for ( int i =1;; ++i )
        {
            if ( !uniqueName( i, str ) )
            {
                str = str.arg( i );
                m_customSlideShowMap.insert( str, m_customSlideShowMap[item->text()] );
                list->insertItem( str );
                break;
            }
        }
    }
}

bool KPrCustomSlideShowDia::uniqueName( int val, const QString & name ) const
{
    QString str = name.arg( val );
    for ( int i= 0; i < ( int )list->count(); ++i )
    {
        if ( list->text ( i ) == str )
            return true;
    }
    return false;
}


KPrCustomSlideShowItem::KPrCustomSlideShowItem( Q3ListBox * listbox, KPrPage * page )
: Q3ListBoxText( listbox, page->pageTitle() )
, m_page( page )
{
}

KPrCustomSlideShowItem::KPrCustomSlideShowItem( KPrPage * page )
: Q3ListBoxText( page->pageTitle() )
, m_page( page )
{
}

KPrCustomSlideShowItem::KPrCustomSlideShowItem( Q3ListBox * listbox, KPrPage * page, Q3ListBoxItem * after )
: Q3ListBoxText( listbox, page->pageTitle(), after )
, m_page( page )
{
}

KPrDefineCustomSlideShow::KPrDefineCustomSlideShow( QWidget* parent, QStringList &_listNameSlideShow,
                                                    const Q3PtrList<KPrPage> &pages, const char *name )
: KDialog( parent )
, listNameCustomSlideShow( _listNameSlideShow )
{
    setCaption(i18n("Define Custom Slide Show"));
    setButtons(Ok|Cancel);
    init();
    for ( Q3PtrList<KPrPage>::ConstIterator it = pages.begin(); it != pages.end(); ++it )
    {
        listSlide->insertItem( new KPrCustomSlideShowItem( *it ) );
    }
}

KPrDefineCustomSlideShow::KPrDefineCustomSlideShow( QWidget* parent, const QString &_customName, QStringList &_listNameSlideShow,
                                                    const Q3PtrList<KPrPage> &pages, Q3ValueList<KPrPage *> &customPages, const char* name )
: KDialog( parent )
, listNameCustomSlideShow( _listNameSlideShow )
{
    setCaption(i18n("Define Custom Slide Show"));
    setButtons(Ok|Cancel);
    init();
    m_name->setText( _customName );
    for ( Q3PtrList<KPrPage>::ConstIterator it = pages.begin(); it != pages.end(); ++it )
    {
        listSlide->insertItem( new KPrCustomSlideShowItem( *it ) );
    }
    for ( Q3ValueList<KPrPage *>::ConstIterator it = customPages.begin(); it != customPages.end(); ++it )
    {
        listSlideShow->insertItem( new KPrCustomSlideShowItem( *it ) );
    }
}

void KPrDefineCustomSlideShow::init()
{
  QWidget* page = new QWidget( this );
  setMainWidget( page );

  Q3VBoxLayout *lov = new Q3VBoxLayout( page );
  lov->setSpacing( KDialog::spacingHint() );
  Q3HBoxLayout *loh = new Q3HBoxLayout( lov );

  QLabel *lab = new QLabel( i18n( "Name:" ), page );
  loh->addWidget( lab );
  m_name = new QLineEdit( page );
  loh->addWidget( m_name );

  Q3HBoxLayout *lo = new Q3HBoxLayout( lov );
  lo->setSpacing( KDialog::spacingHint() );

  Q3VBoxLayout *loAv = new Q3VBoxLayout( lo );
  lab = new QLabel( i18n("Existing slides:"), page );
  loAv->addWidget( lab );
  listSlide = new Q3ListBox( page );
  loAv->addWidget( listSlide );
  lab->setBuddy( listSlide );

  Q3VBoxLayout *loHBtns = new Q3VBoxLayout( lo );
  loHBtns->addStretch( 1 );
  m_insertSlide = new QToolButton( page );
  loHBtns->addWidget( m_insertSlide );
  m_removeSlide = new QToolButton( page );
  loHBtns->addWidget( m_removeSlide );
  loHBtns->addStretch( 1 );

  Q3VBoxLayout *loS = new Q3VBoxLayout( lo );
  lab = new QLabel( i18n("Selected slides:"), page );
  loS->addWidget( lab );
  listSlideShow = new Q3ListBox( page );
  loS->addWidget( listSlideShow );
  lab->setBuddy( listSlideShow );

  Q3VBoxLayout *loVBtns = new Q3VBoxLayout( lo );
  loVBtns->addStretch( 1 );
  m_moveUpSlide = new QToolButton( page );
  m_moveUpSlide->setAutoRepeat( true );
  loVBtns->addWidget( m_moveUpSlide );
  m_moveDownSlide = new QToolButton( page );
  m_moveDownSlide->setAutoRepeat( true );
  loVBtns->addWidget( m_moveDownSlide );
  loVBtns->addStretch( 1 );


  m_name->setFocus();

  connect( m_insertSlide, SIGNAL(clicked()), this, SLOT(slotMoveInsertSlide() ) );
  connect( m_removeSlide, SIGNAL(clicked()), this, SLOT(slotMoveRemoveSlide()) );
  connect( m_moveUpSlide, SIGNAL(clicked()), this, SLOT( slotMoveUpSlide() ) );
  connect( m_moveDownSlide, SIGNAL(clicked()), this, SLOT(slotMoveDownSlide()) );
  connect(  m_name, SIGNAL( textChanged ( const QString & ) ), this, SLOT( slideNameChanged( const QString & ) ) );
  connect( listSlideShow, SIGNAL( clicked ( Q3ListBoxItem * ) ), this, SLOT( updateButton() ) );
  connect( listSlide, SIGNAL( clicked ( Q3ListBoxItem * ) ), this, SLOT( updateButton() ) );
  connect( listSlide, SIGNAL( doubleClicked ( Q3ListBoxItem * ) ), this, SLOT( slotMoveInsertSlide() ) );
  connect( listSlideShow, SIGNAL( doubleClicked( Q3ListBoxItem * ) ), this, SLOT( slotMoveRemoveSlide() ) );
  m_insertSlide->setIconSet( SmallIconSet( ( QApplication::isRightToLeft() ? "back" : "forward" ) ) );
  m_removeSlide->setIconSet( SmallIconSet( ( QApplication::isRightToLeft() ? "forward" : "back") ) );
  m_moveUpSlide->setIconSet( SmallIconSet( "up" ) );
  m_moveDownSlide->setIconSet( SmallIconSet( "down" ) );

  slideNameChanged( m_name->text() );
  updateButton();
  resize( 600, 250 );

}

void KPrDefineCustomSlideShow::slideNameChanged( const QString & _name)
{
     enableButtonOK( !_name.isEmpty() );
}

void KPrDefineCustomSlideShow::updateButton()
{
    int pos = listSlideShow->currentItem();
    m_moveUpSlide->setEnabled( pos>0 );
    m_moveDownSlide->setEnabled( pos< (( int ) listSlideShow->count()-1 ) );
    m_removeSlide->setEnabled( listSlideShow->count()>0 );
    m_insertSlide->setEnabled( listSlide->currentItem()>-1 );
}

void KPrDefineCustomSlideShow::slotMoveUpSlide()
{
    int c = listSlideShow->currentItem();
    if ( c < 1 ) return;
    Q3ListBoxItem *item = listSlideShow->item( c );
    listSlideShow->takeItem( item );
    listSlideShow->insertItem( item, c-1 );
    listSlideShow->setCurrentItem( item );

    updateButton();
}

void KPrDefineCustomSlideShow::slotMoveDownSlide()
{
    int c = listSlideShow->currentItem();
    if ( c < 0 || c == int( listSlideShow->count() ) - 1 ) return;
    Q3ListBoxItem *item = listSlideShow->item( c );
    listSlideShow->takeItem( item );
    listSlideShow->insertItem( item, c+1 );
    listSlideShow->setCurrentItem( item );
    updateButton();
}

void KPrDefineCustomSlideShow::slotMoveRemoveSlide()
{
    // move all selected items from selected to available listbox
    Q3ListBoxItem *item = listSlideShow->firstItem();
    while ( item ) {
        if ( item->isSelected() ) {
            listSlideShow->takeItem( item );
        }
        item = item->next();
    }
    updateButton();
}

void KPrDefineCustomSlideShow::slotMoveInsertSlide()
{
    Q3ListBoxItem *item = listSlide->firstItem();
    while ( item ) {
        if ( item->isSelected() ) {
            KPrCustomSlideShowItem * i = dynamic_cast<KPrCustomSlideShowItem *>( item );
            if ( i )
            {
                listSlideShow->insertItem( new KPrCustomSlideShowItem( i->getPage() ), ( listSlideShow->count() )  );
            }
        }
        item = item->next();
    }
    listSlideShow->setFocus();
    updateButton();
}

Q3ValueList<KPrPage *> KPrDefineCustomSlideShow::customSlides()
{
    Q3ValueList<KPrPage *> pages;
    Q3ListBoxItem *item = listSlideShow->firstItem();
    while ( item )
    {
        KPrCustomSlideShowItem * i = dynamic_cast<KPrCustomSlideShowItem *>( item );
        if ( i )
        {
            pages.push_back( i->getPage() );
        }
        item = item->next();
    }
    return pages;
}

QString KPrDefineCustomSlideShow::customSlideShowName() const
{
    return m_name->text();
}


void KPrDefineCustomSlideShow::slotOk()
{
    if ( listNameCustomSlideShow.contains( m_name->text() ) )
    {
        KMessageBox::error(this, i18n("Custom Slide Show name is already used."), i18n("Define Custom Slide Show"));
    }
    else
    {
        if ( listSlideShow->count() == 0  )
            KMessageBox::error(this, i18n("You did not select any slides. Please select some slides."), i18n("Define Custom Slide Show"));
        else
            accept();
    }
}

#include "KPrCustomSlideShowDia.moc"
