/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000 Montel Laurent <montell@club-internet.fr>
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

#include "kspread_dlg_resize.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_util.h"
#include "kspread_layout.h"
#include "kspread_table.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <iostream.h>

KSpreadresize::KSpreadresize( KSpreadView* parent, const char* name,type_resize re)
	: QDialog( 0L, name )
{

  m_pView=parent;
  type=re;
  QString tmp;
  int pos;
  QString label;
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  RowLayout *rl;
  ColumnLayout *cl;
  switch(type)
	{
	case resize_row:
		pos=m_pView->vBorderWidget()->markerRow();
		tmp=i18n("Row ")+tmp.setNum(pos);
		rl = m_pView->activeTable()->rowLayout(pos);
		size=rl->height(m_pView->canvasWidget());
		label=i18n("Height");
		break;
	case resize_column:
		pos=m_pView->hBorderWidget()->markerColumn();
		tmp=i18n("Column ")+util_columnLabel(pos);
		cl = m_pView->activeTable()->columnLayout(pos);
		size=cl->width(m_pView->canvasWidget());
		label=i18n("Width");
		break;
	default :
		cout <<"Err in type_resize\n";
		break;
	}

  setCaption( tmp );

  //m_pSize2=new KIntNumInput( 20,400,1,size ,this,label, QString::null,false);
  m_pSize2=new KIntNumInput(size, this, 10);
  m_pSize2->setRange(20, 400, 1);
  //m_pSize2->layout();
  lay1->addWidget(m_pSize2);

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb );
  lay1->activate();
  m_pSize2->setFocus();
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );

}

void KSpreadresize::slotOk()
{
int new_size=m_pSize2->value();
if(new_size!=size)
{
switch(type)
	{
	case resize_row:
		m_pView->vBorderWidget()->resizeRow(new_size );
		break;
	case resize_column:
		m_pView->hBorderWidget()->resizeColumn(new_size );
		break;
	default :
		cout <<"Err in type_resize\n";
		break;
	}
}
accept();
}

void KSpreadresize::slotClose()
{

reject();
}


#include "kspread_dlg_resize.moc"
