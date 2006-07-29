/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_dlg_pageshow.h"
#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_page.h"
#include "kivio_command.h"
#include "kivio_doc.h"
#include "kivio_map.h"
#include <kapplication.h>
#include <klocale.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <kbuttonbox.h>
#include <qstrlist.h>
#include <qptrlist.h>
#include <kdebug.h>
#include <KoTabBar.h>

KivioPageShow::KivioPageShow( KivioView* parent, const char* name )
: KDialogBase( parent, name, true, i18n("Show Page"), Ok|Cancel, Ok )
{
  m_pView = parent;

  QWidget* view = new QWidget(this);
  setMainWidget(view);

  QVBoxLayout *lay1 = new QVBoxLayout( view );
  lay1->setMargin( KDialog::marginHint() );
  lay1->setSpacing( KDialog::spacingHint() );

  QLabel *label = new QLabel( i18n("Select hidden page to show:"), view );
  lay1->addWidget( label );

  list = new QListBox(view);
  lay1->addWidget( list );

  QStringList tabsList = m_pView->doc()->map()->hiddenPages();
  list->insertStringList(tabsList);

  connect( list, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(slotDoubleClicked(QListBoxItem *)));
  resize( 200, 150 );
}

void KivioPageShow::slotDoubleClicked(QListBoxItem *)
{
  slotOk();
}

void KivioPageShow::slotOk()
{
  QString text;
  if (list->currentItem()!=-1) {
    text=list->text(list->currentItem());
    //m_pView->tabBar()->showPage(text);
    KivioPage * page = m_pView->doc()->map()->findPage( text );
    page->setHidden( false );
  }
  accept();
}

#include "kivio_dlg_pageshow.moc"
