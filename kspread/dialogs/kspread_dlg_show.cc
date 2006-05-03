/* This file is part of the KDE project
   Copyright (C) 1999-2004 Laurent Montel <montel@kde.org>
             (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qlayout.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <klocale.h>
#include <q3listbox.h>
#include <qlabel.h>

#include <kcommand.h>

#include "kspread_view.h"
#include "kspread_doc.h"
#include "kspread_map.h"
#include "commands.h"

#include "kspread_dlg_show.h"

using namespace KSpread;

ShowDialog::ShowDialog( View* parent, const char* name )
  : KDialogBase( KDialogBase::Plain, Qt::Dialog, parent, name,true,i18n("Show Sheet"),Ok|Cancel )
{
  m_pView = parent;
  QWidget *page = plainPage();
  QVBoxLayout *lay1 = new QVBoxLayout( page );
  lay1->setMargin(0);
  lay1->setSpacing(spacingHint());

  QLabel *label = new QLabel( i18n("Select hidden sheets to show:"), page );
  lay1->addWidget( label );

  list=new Q3ListBox(page);
  lay1->addWidget( list );

  list->setSelectionMode(Q3ListBox::Multi);
  QString text;
  QStringList::Iterator it;
  QStringList tabsList=m_pView->doc()->map()->hiddenSheets();
  for ( it = tabsList.begin(); it != tabsList.end(); ++it )
    	{
    	text=*it;
    	list->insertItem(text);
    	}
  if(!list->count())
  	enableButtonOK(false);
  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
  connect( list, SIGNAL(doubleClicked(Q3ListBoxItem *)),this,SLOT(slotDoubleClicked(Q3ListBoxItem *)));
  resize( 200, 150 );
  setFocus();
}

void ShowDialog::slotDoubleClicked(Q3ListBoxItem *)
{
    slotOk();
}



void ShowDialog::slotOk()
{
    m_pView->doc()->emitBeginOperation( false );

    QStringList listSheet;

    for (int i=0; i < list->numRows(); i++)
    {
        if (list->isSelected(i))
        {
            listSheet.append( list->text(i));
        }
    }

    //m_pView->tabBar()->showSheet(listSheet);

    if ( listSheet.count()==0 )
        return;

    Sheet *sheet;
    KMacroCommand *macroUndo=new KMacroCommand( i18n("Show Sheet") );
    for ( QStringList::Iterator it = listSheet.begin(); it != listSheet.end(); ++it )
    {
        sheet=m_pView->doc()->map()->findSheet( *it );
        KCommand* command = new ShowSheetCommand( sheet );
        macroUndo->addCommand( command );
    }
    m_pView->doc()->addCommand( macroUndo );
    macroUndo->execute();
    m_pView->slotUpdateView( m_pView->activeSheet() );
    accept();
}

#include "kspread_dlg_show.moc"
