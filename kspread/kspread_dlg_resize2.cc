/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000,2001 Montel Laurent <lmontel@mandrakesoft.com>
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


#include "kspread_dlg_resize2.h"
#include "kspread_global.h"
#include "kspread_canvas.h"
#include "kspread_table.h"
#include "kspread_doc.h"
#include "kspread_undo.h"
#include <qlayout.h>
#include <kbuttonbox.h>
#include <kdebug.h>
#include <qcheckbox.h>
#include <knuminput.h>
#include <koUnit.h>

KSpreadresize2::KSpreadresize2( KSpreadView* parent, const char* name,type_resize re)
	: KDialogBase( parent, name,TRUE,i18n("Default"),Ok|Cancel )
{

  m_pView=parent;
  type=re;
  QString tmp;
  QString tmpCheck;
  //int pos;
  QString label;
  QWidget *page = new QWidget( this );
  setMainWidget(page);

  QVBoxLayout *lay1 = new QVBoxLayout( page, 0, spacingHint() );
  tmpCheck=i18n("Default");
  RowLayout *rl;
  ColumnLayout *cl;
  bool equals=true;
  int i;
  QRect selection( m_pView->activeTable()->selection() );
  switch(type)
	{
	case resize_row:
		setCaption( i18n("Resize Row") );
                rl = m_pView->activeTable()->rowLayout(selection.top());
		size=rl->height(m_pView->canvasWidget());
		for(i=selection.top()+1;i<=selection.bottom();i++)
			if(size!=m_pView->activeTable()->rowLayout(i)->height(m_pView->canvasWidget()))
			equals=false;
		label=i18n("Height (%1)").arg(m_pView->doc()->getUnitName());
                tmpCheck+=QString(" %1 %2").arg(KoUnit::ptToUnit( static_cast<int>( heightOfRow ), m_pView->doc()->getUnit() )).arg(m_pView->doc()->getUnitName());
		break;
	case resize_column:
		setCaption( i18n("Resize Column") );
                cl = m_pView->activeTable()->columnLayout(selection.left());
		size=cl->width(m_pView->canvasWidget());
		for(i=selection.left()+1;i<=selection.right();i++)
                  if(size!=m_pView->activeTable()->columnLayout(i)->width(m_pView->canvasWidget()))
                    equals=false;

		label=i18n("Width (%1)").arg(m_pView->doc()->getUnitName());
                tmpCheck+=QString(" %1 %2").arg(KoUnit::ptToUnit(  static_cast<int>( colWidth ) , m_pView->doc()->getUnit() )).arg(m_pView->doc()->getUnitName());
		break;
	default :
	        kdDebug(36001) <<"Err in type_resize" << endl;
		break;
	}


  if(!equals)
  	switch(type)
	{
	case resize_row:
          size=(int)KoUnit::ptToUnit(  static_cast<int>( heightOfRow ), m_pView->doc()->getUnit() );
          break;
	case resize_column:
          size=(int)KoUnit::ptToUnit(  static_cast<int>( colWidth ), m_pView->doc()->getUnit() );
          break;
	}

  m_pSize2=new KIntNumInput((int)KoUnit::ptToUnit( size, m_pView->doc()->getUnit() ), page, 10);
  m_pSize2->setRange(2, 400, 1);
  m_pSize2->setLabel(label);
  lay1->addWidget(m_pSize2);

  m_pDefault=new QCheckBox(tmpCheck,page);
  lay1->addWidget(m_pDefault);

  lay1->activate();
  m_pSize2->setFocus();
  connect( m_pDefault, SIGNAL(clicked() ),this, SLOT(slotChangeState()));
  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );

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
    QRect selection( m_pView->activeTable()->selection() );

    int new_size = (int) KoUnit::ptFromUnit( m_pSize2->value(), m_pView->doc()->getUnit() );
    if ( !m_pView->doc()->undoBuffer()->isLocked() )
      {
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pView->doc(),m_pView->activeTable() , selection );
        m_pView->doc()->undoBuffer()->appendUndo( undo );
      }
    switch(type)
      {
      case resize_row:
	if(m_pDefault->isChecked())
	  for(int i=selection.top();i<=selection.bottom();i++) //The loop seems to be doubled, already done in resizeRow: Philipp -> fixme
	    m_pView->vBorderWidget()->resizeRow( static_cast<int>( heightOfRow ), i, false );
	else
	  for(int i=selection.top();i<=selection.bottom();i++) //The loop seems to be doubled, already done in resizeRow: Philipp -> fixme
	    m_pView->vBorderWidget()->resizeRow(new_size,i,false );
	break;
      case resize_column:
	if(m_pDefault->isChecked())
	  for(int i=selection.left();i<=selection.right();i++) //The loop seems to be doubled, already done in resizeColumn: Philipp -> fixme
	    m_pView->hBorderWidget()->resizeColumn( static_cast<int>( colWidth ), i, false );
	else
	  for(int i=selection.left();i<=selection.right();i++) //The loop seems to be doubled, already done in resizeColumn: Philipp -> fixme
	    m_pView->hBorderWidget()->resizeColumn(new_size,i,false );
	break;
      default :
	kdDebug(36001) <<"Err in type_resize" << endl;
	break;
      }
    accept();
}


#include "kspread_dlg_resize2.moc"
