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

#include <kbuttonbox.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "customslideshowdia.h"


CustomSlideShowDia::CustomSlideShowDia( QWidget* parent, const char* name )
    : KDialogBase( parent, name, true, i18n("Custom Slide Show"), Ok|Cancel )
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

  m_pAdd->setEnabled(false);

  connect( m_pRemove, SIGNAL( clicked() ), this, SLOT( slotRemove() ) );
  connect( m_pAdd, SIGNAL( clicked() ), this, SLOT( slotAdd() ) );
  connect( m_pModify, SIGNAL( clicked() ), this, SLOT( slotModify() ) );
  connect( m_pCopy, SIGNAL( clicked() ), this, SLOT( slotCopy() ) );
  connect( list, SIGNAL(doubleClicked(QListBoxItem *)),this,SLOT(slotDoubleClicked(QListBoxItem *)));
  connect( list, SIGNAL(clicked ( QListBoxItem * )),this,SLOT(slotTextClicked(QListBoxItem * )));

  m_pModify->setEnabled(false);
  if(list->count()<=0)
    m_pRemove->setEnabled(false);
  resize( 600, 250 );
  m_bChanged=false;
}


void CustomSlideShowDia::slotTextClicked(QListBoxItem*)
{
    slotModify();
}

void CustomSlideShowDia::slotDoubleClicked(QListBoxItem *)
{
}

void CustomSlideShowDia::slotAdd()
{
}

void CustomSlideShowDia::slotRemove()
{
}

void CustomSlideShowDia::slotOk()
{
    //todo
    accept();
}

void CustomSlideShowDia::slotModify()
{
//todo
}

void CustomSlideShowDia::slotCopy()
{
    //todo
}


DefineCustomSlideShow::DefineCustomSlideShow( QWidget* parent, const char* name )
    : KDialogBase( parent, name, true, i18n("Define Slide Show Custom"), Ok|Cancel )
{
  QWidget* page = new QWidget( this );
  setMainWidget( page );

  QGridLayout *grid1 = new QGridLayout( page,10,4,KDialog::marginHint(), KDialog::spacingHint());

  QLabel *lab = new QLabel( i18n( "Name" ), page );
  grid1->addWidget( lab, 0, 0 );

  m_name = new QLineEdit( page );
  grid1->addMultiCellWidget( m_name, 0, 0, 1,2 );

  lab = new QLabel( i18n( "Existing slides" ), page );
  grid1->addWidget( lab, 1, 0 );


  listSlide=new QListBox(page);
  grid1->addMultiCellWidget(listSlide,2,8,0,0);

  lab = new QLabel( i18n( "Selected slides" ), page );
  grid1->addWidget( lab, 1, 2 );

  listSlideShow=new QListBox(page);
  grid1->addMultiCellWidget(listSlideShow,2,8,2,2);

}

void DefineCustomSlideShow::slotMoveUpSlide()
{
}

void DefineCustomSlideShow::slotMoveDownSlide()
{
}

void DefineCustomSlideShow::slotMoveRemoveSlide()
{
}

void DefineCustomSlideShow::slotMoveInsertSlide()
{
}

#include "customslideshowdia.moc"
