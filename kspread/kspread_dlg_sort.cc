/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Montel Laurent <montell@club-internet.fr>

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

#include "kspread_dlg_sort.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_table.h"
#include "kspread_util.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <qrect.h>



KSpreadsort::KSpreadsort( KSpreadView* parent, const char* name,const QPoint &_marker)
	: QDialog( 0L, name )
{
  m_pView = parent;
  marker= _marker;
  setCaption( i18n("Sort") );
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  QGridLayout *lay2 = new QGridLayout( lay1,2,2 );
  lay2->setSpacing( 15 );

  QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, "Sort by",this);
  grp->setRadioButtonExclusive( TRUE );
  grp->layout();
  lay2->addWidget(grp,0,0);
  rb_row = new QRadioButton( i18n("Row"), grp );
  rb_column = new QRadioButton( i18n("Column"), grp );
  rb_row->setChecked(true);

  combo=new QComboBox(this);
  lay2->addWidget(combo,0,1);
  list_column=new QListBox();
  list_row=new QListBox();
  init();
  //combo->setListBox(list_row);
  m_pOk = new QPushButton( i18n("Sort"), this );

  lay2->addWidget(m_pOk,1,0);
  m_pClose = new QPushButton( i18n("Close"), this );
  lay2->addWidget(m_pClose,1,1);


  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( grp, SIGNAL(pressed(int)),this,SLOT(slotpress(int)));
}

void KSpreadsort::init()
{


 QRect r( m_pView->activeTable()-> selectionRect() );
 if ( r.left() == 0 || r.top() == 0 ||
       r.right() == 0 || r.bottom() == 0 )
  {
  r.setCoords( marker.x(), marker.y(), marker.x(), marker.y() );
  }
 cout <<"left : "<<r.left()<<"Right() : " <<r.right()<<"bottom : " <<r.bottom()<<"top: "<<r.top()<<endl;
 for(int i=r.left();i<=r.right();i++)
 	{
 	QString tmp=i18n("Column ")+util_columnLabel(i);
 	list_column->insertItem(tmp);
 	}
 for(int i=r.top();i<=r.bottom();i++)
 	{
 	QString tmp2;
 	QString tmp=i18n("Row ")+tmp2.setNum(i);
 	list_row->insertItem(tmp);
 	}
 	
}

void KSpreadsort::slotpress(int id)
{
cout <<"Presse : "<<id<<endl;
switch(id)
	{
	case 0 :
		combo->clear();
		combo->setListBox(list_row);
		break;
	case 1 :
		combo->clear();
		combo->setListBox(list_column);
		break;
	default :
		cout <<"Error in signal : pressed(int id)\n";
		break;
	}
				
}

void KSpreadsort::slotOk()
{
	
accept();
}


void KSpreadsort::slotClose()
{
reject();
}

#include "kspread_dlg_sort.moc"
