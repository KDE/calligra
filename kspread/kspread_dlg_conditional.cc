/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999,2000,2001 Montel Laurent <lmontel@mandrakesoft.com>
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

#include "kspread_dlg_conditional.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_table.h"
#include <qlayout.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <qbuttongroup.h>
#include <qstringlist.h>
#include <kfontdialog.h>
#include <knumvalidator.h>
#include <kdebug.h>

KSpreadWidgetconditional::KSpreadWidgetconditional(QWidget *_parent,const QString &name )
        : QWidget( _parent )

{
  tmpCond.val1=0;
  tmpCond.val2=0;
  tmpCond.fontcond = font;
  tmpCond.colorcond=Qt::black;

  QGridLayout *grid1 = new QGridLayout(this,2,2,15,7);
  QButtonGroup* gb = new QButtonGroup( name, this );

  QGridLayout *grid2 = new QGridLayout(gb,2,4,15,7);
  QLabel *tmplabel = new QLabel( i18n( "Cell is" ), gb );
  tmplabel->resize( tmplabel->sizeHint() );
  grid2->addWidget(tmplabel,0,0);

  preview=new QLineEdit(gb);
  preview->resize( preview->sizeHint() );

  preview->setAlignment(AlignCenter);
  preview->setBackgroundColor(colorGroup().base());
//  preview->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
//  preview->setLineWidth( 1 );
  preview->setText(i18n("Preview"));
  grid2->addWidget(preview,1,1);

  choose=new QComboBox(gb);
  //choose->resize( choose->sizeHint() );
  grid2->addWidget(choose,0,1);

  edit1=new QLineEdit(gb);
  edit1->resize( edit1->sizeHint() );
  grid2->addWidget(edit1,0,2);
  edit1->setValidator( new KFloatValidator( edit1 ) );

  edit2=new QLineEdit(gb);
  //edit2->resize( edit2->sizeHint() );
  grid2->addWidget(edit2,0,3);
  edit2->setValidator( new KFloatValidator( edit2 ) );

  color=new KColorButton(gb);
  //color->resize( color->sizeHint() );
  color->setColor(Qt::black);
  grid2->addWidget(color,1,2);


  fontButton = new QPushButton( gb);
  fontButton->setText(i18n("Font"));
  //fontButton->resize( fontButton->sizeHint() );
  grid2->addWidget( fontButton,1,3);



  grid2->activate();
  grid1->addWidget(gb,0,0);
  grid1->addRowSpacing(0,gb->height());

  grid1->activate();
  QStringList list;
  list+=i18n("<none>");
  list+=i18n("equal to");
  list+=i18n("greater than");
  list+=i18n("less than");
  list+=i18n("equal to or greater than");
  list+=i18n("equal to or less than");
  list+=i18n("between");
  list+=i18n("different from");


  choose->insertStringList(list);
  choose->setCurrentItem(0);
  choose->resize( choose->sizeHint() );
  edit1->setEnabled(false);
  edit2->setEnabled(false);
  connect(choose,SIGNAL(highlighted(const QString &)),this,SLOT(changeIndex(const QString &)));
  connect(fontButton,SIGNAL(clicked()),this,SLOT(changeLabelFont()));

  connect(this,SIGNAL(fontSelected()),
          this,SLOT(refreshPreview()));
  emit(fontSelected());
}

void KSpreadWidgetconditional::init(KSpreadConditional *tmp)
{
font=tmp->fontcond;
color->setColor(tmp->colorcond);
QString val;
switch(tmp->m_cond)
        {
        case None :
                break;
        case Equal :
                choose->setCurrentItem(1);
                edit1->setEnabled(true);
                val=val.setNum(tmp->val1);
                edit1->setText(val);
                break;
        case Superior :
                choose->setCurrentItem(2);
                edit1->setEnabled(true);
                val=val.setNum(tmp->val1);
                edit1->setText(val);
                break;
        case Inferior :
                choose->setCurrentItem(3);
                edit1->setEnabled(true);
                val=val.setNum(tmp->val1);
                edit1->setText(val);
                break;
        case SuperiorEqual :
                choose->setCurrentItem(4);
                edit1->setEnabled(true);
                val=val.setNum(tmp->val1);
                edit1->setText(val);
                break;
        case InferiorEqual :
                choose->setCurrentItem(5);
                edit1->setEnabled(true);
                val=val.setNum(tmp->val1);
                edit1->setText(val);
                break;

        case Between :
                choose->setCurrentItem(6);
                edit1->setEnabled(true);
                edit2->setEnabled(true);
                val=val.setNum(tmp->val1);
                edit1->setText(val);
                val=val.setNum(tmp->val2);
                edit2->setText(val);
                break;
        case Different :
                choose->setCurrentItem(7);
                edit1->setEnabled(true);
                edit2->setEnabled(true);
                val=val.setNum(tmp->val1);
                edit1->setText(val);
                val=val.setNum(tmp->val2);
                edit2->setText(val);
                break;
        }

emit(fontSelected());
}

void KSpreadWidgetconditional::disabled()
{
fontButton->setEnabled(false);
color->setEnabled(false);
}

void KSpreadWidgetconditional::refreshPreview()
{

  preview->setFont(font);
//  preview->color(color->color());
  preview->repaint();

}

void KSpreadWidgetconditional::changeLabelFont()
{
if (KFontDialog::getFont( font,true,this ) == QDialog::Rejected )
        return;
emit(fontSelected());
}

void KSpreadWidgetconditional::changeIndex(const QString &text)
{

if(text==i18n("<none>"))
        {
         edit1->setEnabled(false);
         edit2->setEnabled(false);
         fontButton->setEnabled(false);
         color->setEnabled(false);
        }
else
        {
        fontButton->setEnabled(true);
        color->setEnabled(true);
        if((text==i18n("between"))||(text==i18n("different from")))
                {
                edit1->setEnabled(true);
                edit2->setEnabled(true);
                }
        else
                {
                edit1->setEnabled(true);
                edit2->setEnabled(false);
                }
        }
}

double KSpreadWidgetconditional::getBackFirstValue()
{
QString tmp;
tmp=edit1->text();
return (tmp.toDouble());
}

double KSpreadWidgetconditional::getBackSecondValue()
{
QString tmp;
tmp=edit2->text();
return( tmp.toDouble());
}




QColor KSpreadWidgetconditional::getColor()
{
return color->color();
}

Conditional KSpreadWidgetconditional::typeOfCondition()
{
Conditional result=None;
switch(  choose->currentItem())
        {
         case 0 :
                result=None;
                break;
         case 1 :
                result=Equal;
                break;

         case 2 :
                result=Superior;
                break;
         case 3 :
                result=Inferior;
                break;
         case 4 :
                result=SuperiorEqual;
                break;
         case 5 :
                result=InferiorEqual;
                break;
         case 6 :
                result=Between;
                break;
         case 7 :
                result=Different;
                break;
         default:
                kdDebug(36001) << "Error in list" << endl;
                break;
        }

return result;
}

KSpreadconditional::KSpreadconditional( KSpreadView* parent, const char* name,const QRect &_marker)
        : QDialog( parent, name,TRUE )
{
  m_pView = parent;
  marker=_marker;
  setCaption( i18n("Relational cell attributes") );

  QGridLayout *grid1 = new QGridLayout(this,4,1,15,7);
  firstCond=new KSpreadWidgetconditional(this,i18n("First condition"));
  grid1->addWidget(firstCond,0,0);

  secondCond=new KSpreadWidgetconditional(this,i18n("Second condition"));
  grid1->addWidget(secondCond,1,0);

  thirdCond=new KSpreadWidgetconditional(this,i18n("Third condition"));
  grid1->addWidget(thirdCond,2,0);

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  grid1->addWidget(bb,3,0);
  init();
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

}

void KSpreadconditional::init()
{
//init different condition
//KSpreadConditional *tmpCondition=0;
bool bFirstCond=false;
bool bSecondCond=false;
bool bThirdCond=false;

KSpreadConditional *tmpCondition=0;
KSpreadConditional *tmpCondition2=0;
KSpreadConditional *tmpCondition3=0;
KSpreadCell *obj = m_pView->activeTable()->cellAt( marker.left(), marker.top() );
tmpCondition=obj->getFirstCondition(0);
if(tmpCondition!=0)
       bFirstCond=true;
tmpCondition2=obj->getSecondCondition(0);
if(tmpCondition2!=0)
       bSecondCond=true;
tmpCondition3=obj->getThirdCondition(0);
if(tmpCondition3!=0)
       bThirdCond=true;

for ( int x = marker.left(); x <= marker.right(); x++ )
        {
        for ( int y = marker.top(); y <= marker.bottom(); y++ )
                {
                KSpreadCell *obj2 = m_pView->activeTable()->cellAt( x, y );
                if((obj2->getFirstCondition(0)!=0)&& bFirstCond)
                        {
                        if((obj2->getFirstCondition(0)->val1==tmpCondition->val1)&&
                           (obj2->getFirstCondition(0)->val2==tmpCondition->val2)&&
                           (obj2->getFirstCondition(0)->fontcond==tmpCondition->fontcond)&&
                           (obj2->getFirstCondition(0)->colorcond==tmpCondition->colorcond)&&
                           (obj2->getFirstCondition(0)->m_cond==tmpCondition->m_cond))
                                {
                                 bFirstCond=true;
                                }
                        else
                                {
                                 bFirstCond=false;
                                }
                        }
                else
                        bFirstCond=false;

                if((obj2->getSecondCondition(0)!=0)&& bSecondCond)
                        {
                        if((obj2->getSecondCondition(0)->val1==tmpCondition2->val1)&&
                           (obj2->getSecondCondition(0)->val2==tmpCondition2->val2)&&
                           (obj2->getSecondCondition(0)->fontcond==tmpCondition2->fontcond)&&
                           (obj2->getSecondCondition(0)->colorcond==tmpCondition2->colorcond)&&
                           (obj2->getSecondCondition(0)->m_cond==tmpCondition2->m_cond))
                                {
                                 bSecondCond=true;
                                }
                        else
                                {
                                 bSecondCond=false;
                                }
                        }
                else
                        bSecondCond=false;

                if((obj2->getThirdCondition(0)!=0)&& bThirdCond)
                        {
                        if((obj2->getThirdCondition(0)->val1==tmpCondition3->val1)&&
                           (obj2->getThirdCondition(0)->val2==tmpCondition3->val2)&&
                           (obj2->getThirdCondition(0)->fontcond==tmpCondition3->fontcond)&&
                           (obj2->getThirdCondition(0)->colorcond==tmpCondition3->colorcond)&&
                           (obj2->getThirdCondition(0)->m_cond==tmpCondition3->m_cond))
                                {
                                 bThirdCond=true;
                                }
                        else
                                {
                                 bThirdCond=false;
                                }
                        }
                else
                        bThirdCond=false;

                }
        }
//kdDebug(36001)<<"Condition1 :"<<bFirstCond<<endl;
//kdDebug(36001)<<"Condition2 :"<<bSecondCond<<endl;
//kdDebug(36001)<<"Condition3 :"<<bThirdCond<<endl;

for(int i=0;i<3;i++)
   {
   switch(i)
        {
        case 0:
                //tmpCondition=obj->getFirstCondition(0);
                if(bFirstCond)//tmpCondition!=0)
                        {
                        firstCond->init(tmpCondition);
                        }
                else
                    {
                    firstCond->disabled();
                    }
                break;
        case 1:
                //tmpCondition=obj->getSecondCondition(0);
                if(bSecondCond)//tmpCondition!=0)
                        {
                        secondCond->init(tmpCondition2);
                        }
                else
                    {
                    secondCond->disabled();
                    }
                break;
        case 2:
                //tmpCondition=obj->getThirdCondition(0);
                if(bThirdCond)//tmpCondition!=0)
                        {
                        thirdCond->init(tmpCondition3);
                        }
                else
                    {
                    thirdCond->disabled();
                    }
                break;
        }


    }

}

void KSpreadconditional::slotOk()
{
KSpreadConditional tmpCond[3];

if(firstCond->typeOfCondition()!=None)
  {
    tmpCond[0].val1=firstCond->getBackFirstValue();
    tmpCond[0].fontcond=firstCond->getFont();
    tmpCond[0].colorcond=firstCond->getColor();
    tmpCond[0].m_cond=firstCond->typeOfCondition();
    if((firstCond->typeOfCondition()==Different)||
       (firstCond->typeOfCondition()==Between))
      {
        tmpCond[0].val2=firstCond->getBackSecondValue();
      }
        else
          tmpCond[0].val2=-1;
  }
 else
   {
     QFont font = KGlobalSettings::generalFont();
     tmpCond[0].m_cond=None;
     tmpCond[0].fontcond=font;
     tmpCond[0].colorcond=Qt::black;
     tmpCond[0].val2=0;
     tmpCond[0].val1=0;
   }

 if(secondCond->typeOfCondition()!=None)
   {
     tmpCond[1].val1=secondCond->getBackFirstValue();
     tmpCond[1].fontcond=secondCond->getFont();
     tmpCond[1].colorcond=secondCond->getColor();
     tmpCond[1].m_cond=secondCond->typeOfCondition();
     if((secondCond->typeOfCondition()==Different)||
        (secondCond->typeOfCondition()==Between))
       {
         tmpCond[1].val2=secondCond->getBackSecondValue();
       }
     else
       tmpCond[1].val2=-1;
   }
 else
   {
     QFont font = KGlobalSettings::generalFont();
     tmpCond[1].m_cond=None;
     tmpCond[1].fontcond=font;
     tmpCond[1].colorcond=Qt::black;
     tmpCond[1].val2=0;
     tmpCond[1].val1=0;
   }

 if(thirdCond->typeOfCondition()!=None)
   {
     tmpCond[2].val1=thirdCond->getBackFirstValue();
     tmpCond[2].fontcond=thirdCond->getFont();
     tmpCond[2].colorcond=thirdCond->getColor();
     tmpCond[2].m_cond=thirdCond->typeOfCondition();
     if((thirdCond->typeOfCondition()==Different)||
        (thirdCond->typeOfCondition()==Between))
       {
         tmpCond[2].val2=thirdCond->getBackSecondValue();
       }
     else
       tmpCond[2].val2=-1;
   }
 else
   {
     QFont font = KGlobalSettings::generalFont();
     tmpCond[2].m_cond=None;
     tmpCond[2].fontcond=font;
     tmpCond[2].colorcond=Qt::black;
     tmpCond[2].val2=0;
     tmpCond[2].val1=0;
   }

 m_pView->activeTable()->setConditional( QPoint(  m_pView->canvasWidget()->markerColumn(),
                                                  m_pView->canvasWidget()->markerRow() ), tmpCond );

 accept();


}


void KSpreadconditional::slotClose()
{
reject();
}


#include "kspread_dlg_conditional.moc"
