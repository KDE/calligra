/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999,2000 Montel Laurent <montell@club-internet.fr>
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

#include <qprinter.h>

#include "kspread_dlg_validity.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_table.h"
#include "kspread_map.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <qbuttongroup.h>
#include <qstringlist.h>
#include <kfontdialog.h>
#include <knumvalidator.h>
#include <kdebug.h>

KSpreadDlgValidity::KSpreadDlgValidity(KSpreadView* parent,const char* name , const QRect &_marker )
        :KDialogBase(KDialogBase::Tabbed, i18n("Validity"), Ok|Cancel, Ok, parent, name)

{
    m_pView=parent;
    marker=_marker;
   QFrame *page1 = addPage(i18n("Values"));
   QVBoxLayout *lay1 = new QVBoxLayout( page1 );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );

    QButtonGroup* tmpQButtonGroup;
    tmpQButtonGroup = new QButtonGroup( page1, "ButtonGroup_1" );
    tmpQButtonGroup->setFrameStyle( 49 );
    tmpQButtonGroup->setTitle( i18n("Validity criteria :") );
    QGridLayout *grid1 = new QGridLayout(tmpQButtonGroup,2,4,15,7);

    QLabel *tmpQLabel = new QLabel( tmpQButtonGroup, "Label_1" );
    tmpQLabel->setText(i18n("Allow:" ));
    grid1->addWidget(tmpQLabel,0,0);

    chooseType=new QComboBox(tmpQButtonGroup);
    chooseType->resize( chooseType->sizeHint() );
    grid1->addWidget(chooseType,0,1);
    QStringList listType;
    listType+=i18n("All");
    listType+=i18n("Number");
    listType+=i18n("Text");
    chooseType->insertStringList(listType);
    chooseType->setCurrentItem(0);


    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_2" );
    tmpQLabel->setText(i18n("Data:" ));
    grid1->addWidget(tmpQLabel,1,0);

    choose=new QComboBox(tmpQButtonGroup);
    choose->resize( choose->sizeHint() );
    grid1->addWidget(choose,1,1);
    QStringList list;
    list+=i18n("Equal");
    list+=i18n("superior");
    list+=i18n("inferior");
    list+=i18n("superior or equal");
    list+=i18n("inferior or equal");
    list+=i18n("between");
    list+=i18n("different from");
    choose->insertStringList(list);
    choose->setCurrentItem(0);

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_3" );
    tmpQLabel->setText(i18n("Minimum : " ));
    grid1->addWidget(tmpQLabel,2,0);


    val_min=new QLineEdit(tmpQButtonGroup);
    val_min->resize( val_min->sizeHint() );
    grid1->addWidget(val_min,2,1);
    val_min->setValidator( new KFloatValidator( val_min ) );

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_4" );
    tmpQLabel->setText(i18n("Maximum : " ));
    grid1->addWidget(tmpQLabel,3,0);

    val_max=new QLineEdit(tmpQButtonGroup);
    val_max->resize( val_max->sizeHint() );
    grid1->addWidget(val_max,3,1);
    val_max->setValidator( new KFloatValidator( val_max ) );
    lay1->addWidget(tmpQButtonGroup);

    QFrame *page2 = addPage(i18n("Error alert"));

    lay1 = new QVBoxLayout( page2 );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );

    tmpQButtonGroup = new QButtonGroup( page2, "ButtonGroup_2" );
    tmpQButtonGroup->setFrameStyle( 49 );
    tmpQButtonGroup->setTitle( i18n("Contents :") );
    grid1 = new QGridLayout(tmpQButtonGroup,2,4,15,7);

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_5" );
    tmpQLabel->setText(i18n("Action:" ));
    grid1->addWidget(tmpQLabel,0,0);

    chooseAction=new QComboBox(tmpQButtonGroup);
    chooseAction->resize( chooseAction->sizeHint() );
    grid1->addWidget(chooseAction,0,1);
    QStringList list2;
    list2+=i18n("Stop");
    list2+=i18n("Warning");
    list2+=i18n("Information");
    chooseAction->insertStringList(list2);
    chooseAction->setCurrentItem(0);
    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_6" );
    tmpQLabel->setText(i18n("Title:" ));
    grid1->addWidget(tmpQLabel,1,0);

    title=new QLineEdit(  tmpQButtonGroup);
    grid1->addWidget(title,1,1);

    tmpQLabel = new QLabel( tmpQButtonGroup, "Label_7" );
    tmpQLabel->setText(i18n("Message:" ));
    grid1->addWidget(tmpQLabel,2,0);

     message =new QMultiLineEdit( tmpQButtonGroup);
     grid1->addMultiCellWidget(message,2,3,1,1);
     lay1->addWidget(tmpQButtonGroup);

     connect(choose,SIGNAL(highlighted(int )),this,SLOT(changeIndexCond(int)));
     connect(chooseType,SIGNAL(highlighted(int )),this,SLOT(changeIndexType(int)));
     connect( this, SIGNAL(okClicked()), SLOT(OkPressed()) );
     init();
}

void KSpreadDlgValidity::changeIndexType(int _index)
{
switch(_index)
        {
        case 0:
                val_max->setEnabled(false);
                val_min->setEnabled(false);
                choose->setEnabled(false);
                break;
        case 1:
                val_max->setEnabled(true);
                val_min->setEnabled(true);
                choose->setEnabled(true);
                break;
        case 2:
                val_max->setEnabled(false);
                val_min->setEnabled(true);
                choose->setEnabled(false);
                break;
        }
}

void KSpreadDlgValidity::changeIndexCond(int _index)
{
switch(_index)
        {
        case 0:
                val_max->setEnabled(false);
                break;
        case 1:
                val_max->setEnabled(false);
                break;
        case 2:
                val_max->setEnabled(false);
                break;
        case 3:
                val_max->setEnabled(false);
                break;
        case 4:
                val_max->setEnabled(false);
                break;
        case 5:
                val_max->setEnabled(true);
                break;
        case 6:
                val_max->setEnabled(true);
                break;
        }
}

void KSpreadDlgValidity::init()
{
  KSpreadCell *c = m_pView->activeTable()->cellAt( marker.left(), marker.top() );
  KSpreadValidity * tmpValidity=c->getValidity(0);
  if(tmpValidity!=0)
        {
        message->setText(tmpValidity->avertissment);
        title->setText(tmpValidity->title);
        QString tmp;
        val_max->setText(tmp.setNum(tmpValidity->valMax));
        val_min->setText(tmp.setNum(tmpValidity->valMin));
        switch( tmpValidity->m_allow)
                {
                case Allow_All:
                        chooseType->setCurrentItem(0);
                        break;
                case Allow_Number:
                        chooseType->setCurrentItem(1);
                        break;
                case Allow_Text:
                        chooseType->setCurrentItem(2);
                        break;
                default :
                        chooseType->setCurrentItem(0);
                        break;
                }
        switch (tmpValidity->m_action)
                {
                case Stop:
                        chooseAction->setCurrentItem(0);
                        break;
                case Warning:
                        chooseAction->setCurrentItem(1);
                        break;
                case Information:
                        chooseAction->setCurrentItem(2);
                        break;
                default :
                        chooseAction->setCurrentItem(0);
                        break;
                }
        switch ( tmpValidity->m_cond)
                {
                case Equal:
                        choose->setCurrentItem(0);
                        break;
                case Superior:
                        choose->setCurrentItem(1);
                        break;
                case Inferior:
                        choose->setCurrentItem(2);
                        break;
                case SuperiorEqual:
                        choose->setCurrentItem(3);
                        break;
                case InferiorEqual:
                        choose->setCurrentItem(4);
                        break;
                case Between:
                        choose->setCurrentItem(5);
                        break;
                case Different:
                        choose->setCurrentItem(6);
                        break;
                default :
                        choose->setCurrentItem(0);
                        break;
                }
        }
changeIndexType(chooseType->currentItem()) ;
changeIndexCond(choose->currentItem()) ;
}

void KSpreadDlgValidity::OkPressed()
{
if( chooseType->currentItem()==0)
        {//no validity
        result.m_allow=Allow_All;
        result.m_action=Stop;
        result.m_cond=Equal;
        result.avertissment=message->text();
        result.title=title->text();
        result.valMin=val_min->text().toDouble();
        result.valMax=val_max->text().toDouble();
        }
else
        {
        switch( chooseType->currentItem())
                {
                case 0:
                        result.m_allow=Allow_All;
                        break;
                case 1:
                        result.m_allow=Allow_Number;
                        break;
                case 2:
                        result.m_allow=Allow_Text;
                        break;
                default :
                        break;
                }
        switch (chooseAction->currentItem())
                {
                case 0:
                        result.m_action=Stop;
                        break;
                case 1:
                        result.m_action=Warning;
                        break;
                case 2:
                        result.m_action=Information;
                        break;
                default :
                        break;
                }
        switch ( choose->currentItem())
                {
                case 0:
                        result.m_cond=Equal;
                        break;
                case 1:
                        result.m_cond=Superior;
                        break;
                case 2:
                        result.m_cond=Inferior;
                        break;
                case 3:
                        result.m_cond=SuperiorEqual;
                        break;
                case 4:
                        result.m_cond=InferiorEqual;
                        break;
                case 5:
                        result.m_cond=Between;
                        break;
                case 6:
                        result.m_cond=Different;
                        break;
                default :
                        break;
                }
        result.avertissment=message->text();
        result.title=title->text();
        result.valMin=val_min->text().toDouble();
        result.valMax=val_max->text().toDouble();
        }

m_pView->activeTable()->setValidity( QPoint(  m_pView->canvasWidget()->markerColumn(),
                                                  m_pView->canvasWidget()->markerRow() ),  result);
}

#include "kspread_dlg_validity.moc"
