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

#include "kspread_dlg_resize2.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_util.h"
#include "kspread_layout.h"
#include "kspread_table.h"
#include "kspread_doc.h"
#include "kspread_undo.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <kdebug.h>

KSpreadresize2::KSpreadresize2( KSpreadView* parent, const char* name,type_resize re)
	: QDialog( parent, name,TRUE )
{

  m_pView=parent;
  type=re;
  QString tmp;
  QString tmpCheck;
  //int pos;
  QString label;
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  tmpCheck=i18n("Default");
  RowLayout *rl;
  ColumnLayout *cl;
  bool equals=true;
  int i;
  QRect selection( m_pView->activeTable()->selectionRect() ); 
  switch(type)
	{
	case resize_row:
		setCaption( i18n("Resize row") );
		if(selection.top()==0)
			rl = m_pView->activeTable()->rowLayout( m_pView->canvasWidget()->markerRow());
		else
			rl = m_pView->activeTable()->rowLayout(selection.top());
		size=rl->height(m_pView->canvasWidget());
		for(i=selection.top()+1;i<=selection.bottom();i++)
			if(size!=m_pView->activeTable()->rowLayout(i)->height(m_pView->canvasWidget()))
			equals=false;
		label=i18n("Height");
		tmpCheck+=" (20)";
		break;
	case resize_column:
		setCaption( i18n("Resize column") );
		if(selection.left()==0)
			cl = m_pView->activeTable()->columnLayout( m_pView->canvasWidget()->markerColumn());
		else
			cl = m_pView->activeTable()->columnLayout(selection.left());
		size=cl->width(m_pView->canvasWidget());
		for(i=selection.left()+1;i<=selection.right();i++)
			if(size!=m_pView->activeTable()->columnLayout(i)->width(m_pView->canvasWidget()))
			equals=false;

		label=i18n("Width");
		tmpCheck+=" (60)";
		break;
	default :
	        kdDebug(36001) <<"Err in type_resize" << endl;
		break;
	}


  if(!equals)
  	switch(type)
	{
	case resize_row:
			size=20;
			break;
	case resize_column:
			size=60;
			break;
	}

  m_pSize2=new KIntNumInput(size, this, 10);
  m_pSize2->setRange(2, 400, 1);
  m_pSize2->setLabel(label);
  lay1->addWidget(m_pSize2);

  m_pDefault=new QCheckBox(tmpCheck,this);
  lay1->addWidget(m_pDefault);

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb );
  lay1->activate();
  m_pSize2->setFocus();
  connect( m_pDefault, SIGNAL(clicked() ),this, SLOT(slotChangeState()));
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );

}

void KSpreadresize2::slotChangeState()
{
    if(m_pDefault->isChecked())
	m_pSize2->setEnabled(false);
    else
	m_pSize2->setEnabled(true);
}


void KSpreadresize2::slotOk()
{
    QRect selection( m_pView->activeTable()->selectionRect() );
    QRect rect=selection;
    if(selection.bottom()<=0 ||selection.top()<=0 || selection.left()<=0
       || selection.right()<=0)
      {
	switch(type)
	  {
	  case resize_row:
	    rect.setCoords(1,m_pView->canvasWidget()->markerRow(),0x7FFF,m_pView->canvasWidget()->markerRow());
	    break;
	  case resize_column:
	    rect.setCoords(m_pView->canvasWidget()->markerColumn(),1,m_pView->canvasWidget()->markerColumn(),0x7FFF);
	    break;
	  }
      }
    int new_size=m_pSize2->value();
    if ( !m_pView->doc()->undoBuffer()->isLocked() )
      {
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pView->doc(),m_pView->activeTable() , rect );
        m_pView->doc()->undoBuffer()->appendUndo( undo );
      }
    switch(type)
      {
      case resize_row:
	if(m_pDefault->isChecked())
	  for(int i=rect.top();i<=rect.bottom();i++)
	    m_pView->vBorderWidget()->resizeRow(20,i,false );
	else
	  for(int i=rect.top();i<=rect.bottom();i++)
	    m_pView->vBorderWidget()->resizeRow(new_size,i,false );
	break;
      case resize_column:
	if(m_pDefault->isChecked())
	  for(int i=rect.left();i<=rect.right();i++)
	    m_pView->hBorderWidget()->resizeColumn(60,i,false );
	else
	  for(int i=rect.left();i<=rect.right();i++)
	    m_pView->hBorderWidget()->resizeColumn(new_size,i,false );
	break;
      default :
	kdDebug(36001) <<"Err in type_resize" << endl;
	break;
      }
    accept();
}

void KSpreadresize2::slotClose()
{
    reject();
}


#include "kspread_dlg_resize2.moc"
