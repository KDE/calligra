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


#include "kspread_dlg_conditional.h"
#include "kspread_canvas.h"
#include "kspread_table.h"
#include <qlayout.h>
#include <kbuttonbox.h>
#include <klocale.h>
#include <qbuttongroup.h>
#include <kfontdialog.h>
#include <knumvalidator.h>
#include <kdebug.h>
#include <koGlobal.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <kcolorbutton.h>

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

void KSpreadWidgetconditional::init(KSpreadConditional tmp)
{
  font=tmp.fontcond;
  color->setColor(tmp.colorcond);
QString val;
switch(tmp.m_cond)
        {
        case None :
                break;
        case Equal :
                choose->setCurrentItem(1);
                edit1->setEnabled(true);
                val=val.setNum(tmp.val1);
                edit1->setText(val);
                break;
        case Superior :
                choose->setCurrentItem(2);
                edit1->setEnabled(true);
                val=val.setNum(tmp.val1);
                edit1->setText(val);
                break;
        case Inferior :
                choose->setCurrentItem(3);
                edit1->setEnabled(true);
                val=val.setNum(tmp.val1);
                edit1->setText(val);
                break;
        case SuperiorEqual :
                choose->setCurrentItem(4);
                edit1->setEnabled(true);
                val=val.setNum(tmp.val1);
                edit1->setText(val);
                break;
        case InferiorEqual :
                choose->setCurrentItem(5);
                edit1->setEnabled(true);
                val=val.setNum(tmp.val1);
                edit1->setText(val);
                break;

        case Between :
                choose->setCurrentItem(6);
                edit1->setEnabled(true);
                edit2->setEnabled(true);
                val=val.setNum(tmp.val1);
                edit1->setText(val);
                val=val.setNum(tmp.val2);
                edit2->setText(val);
                break;
        case Different :
                choose->setCurrentItem(7);
                edit1->setEnabled(true);
                edit2->setEnabled(true);
                val=val.setNum(tmp.val1);
                edit1->setText(val);
                val=val.setNum(tmp.val2);
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
  tmp = edit1->text();
  return (tmp.toDouble());
}

double KSpreadWidgetconditional::getBackSecondValue()
{
  QString tmp;
  tmp = edit2->text();
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

KSpreadconditional::KSpreadconditional( KSpreadView * parent, const char * name,
                                        const QRect & _marker)
        : KDialogBase( parent, name, TRUE, i18n("Relational Cell Attributes"),
                       Ok | Cancel )
{
  m_pView = parent;
  marker  = _marker;
  QWidget * page = new QWidget( this );
  setMainWidget(page);

  QGridLayout *grid1 = new QGridLayout(page, 4, 1, 15, 7);
  conditionals[0] = new KSpreadWidgetconditional(page, i18n("First Condition"));
  grid1->addWidget(conditionals[0], 0, 0);

  conditionals[1] = new KSpreadWidgetconditional(page,
						 i18n("Second Condition"));
  grid1->addWidget(conditionals[1], 1, 0);

  conditionals[2] = new KSpreadWidgetconditional(page,i18n("Third Condition"));
  grid1->addWidget(conditionals[2], 2, 0);

  init();
  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );

}

void KSpreadconditional::init()
{
  QValueList<KSpreadConditional> conditionList;
  QValueList<KSpreadConditional> otherList;
  bool found;
  int numCondition;

  QValueList<KSpreadConditional>::iterator it1;
  QValueList<KSpreadConditional>::iterator it2;
      
  KSpreadCell *obj = m_pView->activeTable()->cellAt( marker.left(), 
						     marker.top() );

  conditionList = obj->GetConditionList(); 
  /* this is the list, but only display the conditions common to all selected 
     cells*/

  for ( int x = marker.left(); x <= marker.right(); x++ )
  {
    for ( int y = marker.top(); y <= marker.bottom(); y++ )
    {
      KSpreadCell *obj2 = m_pView->activeTable()->cellAt( x, y );
      otherList = obj2->GetConditionList();
      
      it1 = conditionList.begin();
      while(it1 != conditionList.end())
      {
	found = false;
	for (it2 = otherList.begin(); !found && it2 != otherList.end(); it2++)
	{
	  found = ((*it1).val1 == (*it2).val1 &&
		   (*it1).val2 == (*it2).val2 &&
		   (*it1).colorcond == (*it2).colorcond &&
		   (*it1).fontcond == (*it2).fontcond &&
		   (*it1).m_cond == (*it2).m_cond);
	}
	
	if (!found)  /* if it's not here, don't display this condition */
	{
	  it1 = conditionList.remove(it1);
	}
	else
	{
	  it1++;
	}
      }
    }
  }

  numCondition = 0;
  for (it1 = conditionList.begin(); 
       numCondition < KSPREAD_NUM_CONDITIONALS && it1 != conditionList.end(); 
       it1++)
  {
    conditionals[numCondition]->init(*it1);
    numCondition++;
  }

  /* disable the unused ones */
  for (; numCondition < KSPREAD_NUM_CONDITIONALS; numCondition++)
  {
    conditionals[numCondition]->disabled();
  }
}

void KSpreadconditional::slotOk()
{
  QValueList<KSpreadConditional> newList;
  KSpreadConditional newCondition;

  kdDebug() << "Start creating list" << endl;
  /* copy the information from the form, put it in a list and send it to the
     appropriate cells */
  for (int i = 0; i < KSPREAD_NUM_CONDITIONALS; i++)
  {
    if (conditionals[i]->typeOfCondition() != None)
    {
      newCondition.val1 = conditionals[i]->getBackFirstValue();
      newCondition.fontcond = conditionals[i]->getFont();
      newCondition.colorcond = conditionals[i]->getColor();
      newCondition.m_cond = conditionals[i]->typeOfCondition();
      if (newCondition.m_cond == Different || newCondition.m_cond == Between)
      {
	newCondition.val2 = conditionals[i]->getBackSecondValue();
      }
      else
      {
	newCondition.val2 = -1;
      }
      newList.append(newCondition);
    }
  }
  kdDebug() << "Condition list created " << endl;

  m_pView->activeTable()->setConditional(marker, newList);

  kdDebug() << "Update done" << endl;

  accept();
}


#include "kspread_dlg_conditional.moc"
