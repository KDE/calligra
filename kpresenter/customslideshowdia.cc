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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qapplication.h>
#include <qlayout.h>
#include <kiconloader.h>
#include <kbuttonbox.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "kprpage.h"
#include "kpresenter_doc.h"

#include "customslideshowdia.h"


CustomSlideShowDia::CustomSlideShowDia( QWidget* parent, KPresenterDoc *_doc, const char* name )
    : KDialogBase( parent, name, true, i18n("Custom Slide Show"), Ok|Cancel ), m_doc( _doc )
{
  QWidget* page = new QWidget( this );
  setMainWidget( page );

  QGridLayout *grid1 = new QGridLayout( page,10,3,KDialog::marginHint(), KDialog::spacingHint());

  list=new QListBox(page);
  grid1->addMultiCellWidget(list,0,8,0,0);

  m_pAdd=new QPushButton(i18n("&Add"),page);
  grid1->addWidget(m_pAdd,1,2);

  m_pModify=new QPushButton(i18n("&Modify"),page);
  grid1->addWidget(m_pModify,2,2);

  m_pRemove=new QPushButton(i18n("&Remove"),page);
  grid1->addWidget(m_pRemove,3,2);

  m_pCopy=new QPushButton(i18n("Co&py"),page);
  grid1->addWidget(m_pCopy,4,2);

  //m_pAdd->setEnabled(false);

  connect( m_pRemove, SIGNAL( clicked() ), this, SLOT( slotRemove() ) );
  connect( m_pAdd, SIGNAL( clicked() ), this, SLOT( slotAdd() ) );
  connect( m_pModify, SIGNAL( clicked() ), this, SLOT( slotModify() ) );
  connect( m_pCopy, SIGNAL( clicked() ), this, SLOT( slotCopy() ) );
  connect( list, SIGNAL(doubleClicked(QListBoxItem *)),this,SLOT(slotDoubleClicked(QListBoxItem *)));
  connect( list, SIGNAL(clicked ( QListBoxItem * )),this,SLOT(slotTextClicked(QListBoxItem * )));

  init();
  updateButton();

  resize( 600, 250 );

  m_bChanged=false;
}

void CustomSlideShowDia::init()
{
    m_customListMap = m_doc->customListSlideShow();
    CustomListMap::Iterator it;
    for ( it = m_customListMap.begin(); it != m_customListMap.end(); ++it ) {
        list->insertItem( it.key() );
    }

    for( unsigned i = 0; i < m_doc->pageList().count(); i++ )
        listPageName.append( m_doc->pageList().at( i )->pageTitle() );
}

void CustomSlideShowDia::updateButton()
{
    bool state = ( list->currentItem() >= 0 );
    m_pRemove->setEnabled( state );
    m_pModify->setEnabled( state );
    m_pCopy->setEnabled( state );
}

void CustomSlideShowDia::slotTextClicked(QListBoxItem*)
{
    updateButton();
}

void CustomSlideShowDia::slotDoubleClicked(QListBoxItem *)
{
    updateButton();
    slotModify();
}

void CustomSlideShowDia::slotAdd()
{
    DefineCustomSlideShow * dlg = new DefineCustomSlideShow( this, listPageName );
    if ( dlg->exec() )
    {
        //insert new element
        m_customListMap.insert( dlg->customSlideShowName(), dlg->customListSlideShow() );
        list->insertItem( dlg->customSlideShowName() );
        updateButton();
    }
    delete dlg;

}

void CustomSlideShowDia::slotRemove()
{
    if (list->selectedItem() )
    {
        m_customListMap.remove( list->selectedItem()->text() );
        list->removeItem( list->currentItem() );
        updateButton();
    }
}

void CustomSlideShowDia::slotOk()
{
    m_doc->updateCustomListSlideShow(m_customListMap);
    accept();
}

void CustomSlideShowDia::slotModify()
{
    QListBoxItem *item = list->selectedItem();
    if ( item )
    {
        DefineCustomSlideShow * dlg = new DefineCustomSlideShow( this, item->text(), listPageName, m_customListMap[item->text()]);
        if ( dlg->exec() )
        {
            //insert new element
            m_customListMap.remove( list->selectedItem()->text() );
            m_customListMap.insert( dlg->customSlideShowName(), dlg->customListSlideShow() );
            list->changeItem( dlg->customSlideShowName(), list->currentItem() );
        }
        delete dlg;
    }

}

void CustomSlideShowDia::slotCopy()
{
    QListBoxItem *item = list->selectedItem();
    if ( item )
    {
        QString str( list->selectedItem()->text() );
        str+=i18n( "(Copy %1)" );
        for ( int i =1;; ++i )
        {
            if ( !uniqueName( i, str ) )
            {
                str = str.arg( i );
                m_customListMap.insert( str, m_customListMap[item->text()] );
                list->insertItem( str );
                break;
            }
        }
    }
}

bool CustomSlideShowDia::uniqueName( int val, const QString & name ) const
{
    QString str = name.arg( val );
    for ( int i= 0; i < ( int )list->count(); ++i )
    {
        if ( list->text ( i ) == str )
            return true;
    }
    return false;
}


DefineCustomSlideShow::DefineCustomSlideShow( QWidget* parent, QStringList & _listPage, const char* name )
    : KDialogBase( parent, name, true, i18n("Define Custom Slide Show"), Ok|Cancel )
{
    init();
    listSlide->insertStringList( _listPage );
}

DefineCustomSlideShow::DefineCustomSlideShow( QWidget* parent, const QString &_customName, QStringList& _listPage, QStringList &_customListPage, const char* name )
    : KDialogBase( parent, name, true, i18n("Define Custom Slide Show"), Ok|Cancel )
{
    init();
    m_name->setText( _customName );
    listSlide->insertStringList( _listPage );
    listSlideShow->insertStringList( _customListPage );
}

void DefineCustomSlideShow::init()
{
  QWidget* page = new QWidget( this );
  setMainWidget( page );

  QVBoxLayout *lov = new QVBoxLayout( page );
  lov->setSpacing( KDialog::spacingHint() );
  QHBoxLayout *loh = new QHBoxLayout( lov );

  QLabel *lab = new QLabel( i18n( "Name" ), page );
  loh->addWidget( lab );
  m_name = new QLineEdit( page );
  loh->addWidget( m_name );

  QHBoxLayout *lo = new QHBoxLayout( lov );
  lo->setSpacing( KDialog::spacingHint() );

  QVBoxLayout *loAv = new QVBoxLayout( lo );
  lab = new QLabel( i18n("Existing slides:"), page );
  loAv->addWidget( lab );
  listSlide = new QListBox( page );
  loAv->addWidget( listSlide );
  lab->setBuddy( listSlide );

  QVBoxLayout *loHBtns = new QVBoxLayout( lo );
  loHBtns->addStretch( 1 );
  m_insertSlide = new QToolButton( page );
  loHBtns->addWidget( m_insertSlide );
  m_removeSlide = new QToolButton( page );
  loHBtns->addWidget( m_removeSlide );
  loHBtns->addStretch( 1 );

  QVBoxLayout *loS = new QVBoxLayout( lo );
  lab = new QLabel( i18n("Selected slides:"), page );
  loS->addWidget( lab );
  listSlideShow = new QListBox( page );
  loS->addWidget( listSlideShow );
  lab->setBuddy( listSlideShow );

  QVBoxLayout *loVBtns = new QVBoxLayout( lo );
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
  connect( listSlideShow, SIGNAL( clicked ( QListBoxItem * ) ), this, SLOT( updateButton() ) );
  connect( listSlide, SIGNAL( clicked ( QListBoxItem * ) ), this, SLOT( updateButton() ) );
  connect( listSlide, SIGNAL( doubleClicked ( QListBoxItem * ) ), this, SLOT( slotMoveInsertSlide() ) );
  connect( listSlideShow, SIGNAL( doubleClicked( QListBoxItem * ) ), this, SLOT( slotMoveRemoveSlide() ) );
  m_insertSlide->setIconSet( SmallIconSet( ( QApplication::reverseLayout() ? "back" : "forward" ) ) );
  m_removeSlide->setIconSet( SmallIconSet( ( QApplication::reverseLayout() ? "forward" : "back") ) );
  m_moveUpSlide->setIconSet( SmallIconSet( "up" ) );
  m_moveDownSlide->setIconSet( SmallIconSet( "down" ) );

  slideNameChanged( m_name->text() );
  updateButton();
  resize( 600, 250 );

}

void DefineCustomSlideShow::slideNameChanged( const QString & _name)
{
     enableButtonOK( !_name.isEmpty() );
}

void DefineCustomSlideShow::updateButton()
{
    int pos = listSlideShow->currentItem();
    m_moveUpSlide->setEnabled( pos>0 );
    m_moveDownSlide->setEnabled( pos< (( int ) listSlideShow->count()-1 ) );
    m_removeSlide->setEnabled( listSlideShow->count()>0 );
    m_insertSlide->setEnabled( listSlide->currentItem()>-1 );
}

void DefineCustomSlideShow::slotMoveUpSlide()
{
    int c = listSlideShow->currentItem();
    if ( c < 1 ) return;
    QListBoxItem *item = listSlideShow->item( c );
    listSlideShow->takeItem( item );
    listSlideShow->insertItem( item, c-1 );
    listSlideShow->setCurrentItem( item );

    updateButton();
}

void DefineCustomSlideShow::slotMoveDownSlide()
{
    int c = listSlideShow->currentItem();
    if ( c < 0 || c == int( listSlideShow->count() ) - 1 ) return;
    QListBoxItem *item = listSlideShow->item( c );
    listSlideShow->takeItem( item );
    listSlideShow->insertItem( item, c+1 );
    listSlideShow->setCurrentItem( item );
    updateButton();
}

void DefineCustomSlideShow::slotMoveRemoveSlide()
{
    // move all selected items from selected to available listbox
    QListBoxItem *item = listSlideShow->firstItem();
    while ( item ) {
        if ( item->isSelected() ) {
            listSlideShow->takeItem( item );
        }
        item = item->next();
    }
    updateButton();
}

void DefineCustomSlideShow::slotMoveInsertSlide()
{
    QListBoxItem *item = listSlide->firstItem();
    while ( item ) {
        if ( item->isSelected() ) {
            listSlideShow->insertItem( item->text(), 0 );
        }
        item = item->next();
    }
    listSlideShow->setFocus();
    updateButton();
}

QStringList DefineCustomSlideShow::customListSlideShow()
{
    QStringList lst;
    QListBoxItem *item = listSlideShow->firstItem();
    while ( item ) {
        lst << item->text();
        item = item->next();
    }
    return lst;
}

QString DefineCustomSlideShow::customSlideShowName() const
{
    return m_name->text();
}


#include "customslideshowdia.moc"
