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
#include <qvgroupbox.h>

KSpreadresize::KSpreadresize( KSpreadView* parent, const char* name,type_resize re,int nb=-1)
	: QDialog( 0L, name )
{

  m_pView=parent;
  type=re;
  number=nb;
  QString tmp;

  int pos;

  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  m_pSize = new QLineEdit( this );
  lay1->addWidget(m_pSize);
  RowLayout *rl;
  ColumnLayout *cl;
  QRect selection( m_pView->activeTable()->selectionRect() );
  switch(type)
	{
	case resize_row:
		if(selection.right()==0x7fff&&(selection.top()==selection.bottom()))
			{
			pos=m_pView->canvasWidget()->vBorderWidget()->markerRow();
			tmp=i18n("Row ")+tmp.setNum(pos);
			rl = m_pView->activeTable()->rowLayout(pos);
			size=rl->height(m_pView->canvasWidget());
		        }
		 else
		 	{
		 	if(selection.bottom()==0 ||selection.top()==0 || selection.left()==0
			|| selection.right()==0||(selection.top()==selection.bottom()))
				{
				pos=m_pView->canvasWidget()->markerRow();
				tmp=i18n("Row ")+tmp.setNum(pos);
				rl = m_pView->activeTable()->rowLayout(pos);
				size=rl->height(m_pView->canvasWidget());
				}
			else
				{
		 		size=0;
		 		tmp=i18n("Row ");
		 		}
		 	}
		break;
	case resize_column:
		if(selection.bottom()==0x7fff &&(selection.left()==selection.right()))
			{
			pos=m_pView->canvasWidget()->hBorderWidget()->markerColumn();
			tmp=i18n("Column ")+util_columnLabel(pos);
			cl = m_pView->activeTable()->columnLayout(pos);
			size=cl->width(m_pView->canvasWidget());
			}
		else
			{
			if(selection.bottom()==0 ||selection.top()==0 || selection.left()==0
			|| selection.right()==0 || (selection.left()==selection.right()) )
				{
				pos=m_pView->canvasWidget()->markerColumn();
				tmp=i18n("Column ")+util_columnLabel(pos);
				cl = m_pView->activeTable()->columnLayout(pos);
				size=cl->width(m_pView->canvasWidget());
				}
			else
				{
				size=0;
				tmp=i18n("Column ");
				}
			}
		break;
	default :
		cout <<"Err in type_resize\n";
		break;
	}

  setCaption( tmp );
  m_pSize->setText(tmp.setNum(size));
  /*QVGroupBox *group = new QVGroupBox(  i18n("Size"), this );
  lay1->addWidget( group);
  m_pSize2 = new KIntNumInput(0L, 20, 400, 1,size,
                                 0L, 10, false, group);


  */
  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb );


  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
}

void KSpreadresize::slotOk()
{
QString tmp;
tmp=m_pSize->text();
if(tmp.toInt()!=0 || tmp.toDouble()!=0 )
{
int new_size;
if(tmp.toDouble()!=0)
	new_size=(int)tmp.toDouble();
else
	new_size=tmp.toInt();

switch(type)
	{
	case resize_row:
		m_pView->canvasWidget()->vBorderWidget()->resizeRow(new_size,number );
		break;
	case resize_column:
		m_pView->canvasWidget()->hBorderWidget()->resizeColumn(new_size,number );
		break;
	default :
		cout <<"Err in type_resize\n";
		break;
	}
accept();
}
else
	{
	QMessageBox::warning( 0L, i18n("Error"), i18n("It is not a number !"), i18n("Ok"));
 	m_pSize->setText(tmp.setNum(size));
	}
}

void KSpreadresize::slotClose()
{

reject();
}


#include "kspread_dlg_resize.moc"
