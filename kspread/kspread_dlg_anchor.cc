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

#include "kspread_dlg_anchor.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_table.h"

#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>


KSpreadanchor::KSpreadanchor( KSpreadView* parent, const char* name,const QPoint &_marker)
	: QDialog( parent, name )
{
  m_pView = parent;
  marker= _marker;
  setCaption( i18n("Create anchor") );
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  QVBoxLayout *lay2 = new QVBoxLayout( lay1); 
  lay2->setSpacing( 5 );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this);

  lay2->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("Text"));

  text = new QLineEdit( this );
  lay2->addWidget(text);

  tmpQLabel = new QLabel( this);
  lay2->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("Cell"));
  l_cell = new QLineEdit( this );

  lay2->addWidget(l_cell);
  l_cell->setText( "A1" );

  bold=new QCheckBox(i18n("Bold"),this);

  lay2->addWidget(bold);
  italic=new QCheckBox(i18n("Italic"),this);

  lay2->addWidget(italic);
  text->setFocus();
  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay2->addWidget( bb);
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

}


void KSpreadanchor::slotOk()
{
KSpreadCell *cell = m_pView->activeTable()->cellAt( m_pView->canvasWidget()->markerColumn(), m_pView->canvasWidget()->markerRow() );
if(l_cell->text().isEmpty()||text->text().isEmpty())
	{
	QMessageBox::warning( 0L, i18n("Error"), i18n("Area Text or cell is empty!"),
			   i18n("Ok") );
	}
else if(!cell->isDefault())
	{
	int ret = QMessageBox::warning( this, i18n("Create anchor"), i18n("Cell is not empty.\nDo you want to continue?"), i18n("Yes"), i18n("No"), QString::null, 1, 1);
 	if ( ret == 0 )
 		{
 		QString tmp;
 		tmp=create_anchor();
 		m_pView->canvasWidget()->setFocus();
  		m_pView->setText( tmp );
		m_pView->editWidget()->setText( tmp );
		accept();
 		}
 	else
 		{
 		 accept();
                }
    	}

else
	{
	//refresh editWidget
	QString tmp;
	tmp=create_anchor();

 	m_pView->canvasWidget()->setFocus();
  	m_pView->setText( tmp );
 	m_pView->editWidget()->setText( tmp );
	accept();

	}
}

QString KSpreadanchor::create_anchor()
{
QString anchor;
anchor="!<a href=\""+m_pView->activeTable()->tableName()+"!"+l_cell->text().upper()+"\""+">";
if(bold->isChecked()&&!italic->isChecked())
	{
	anchor+="<b>"+text->text()+"</b></a>";
	}
else if (!bold->isChecked()&&italic->isChecked())
	{
	anchor+="<i>"+text->text()+"</i></a>";
	}
else if(bold->isChecked()&&italic->isChecked())
	{
	anchor+="<i><b>"+text->text()+"</b></i></a>";
	}
else
	{
	anchor+=text->text()+"</a>";
	}

return anchor;
}

void KSpreadanchor::slotClose()
{
reject();
}


#include "kspread_dlg_anchor.moc"
