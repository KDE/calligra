/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2005 Peter Simonsson
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
#include "objectlistpalette.h"

#include <qlayout.h>
#include <qheader.h>
#include <qptrlist.h>

#include <klistview.h>
#include <kdebug.h>
#include <klocale.h>

#include "kivio_view.h"
#include "kivio_stencil.h"
#include "kivio_layer.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_page.h"
#include "kivioglobal.h"
#include "kivio_doc.h"

namespace Kivio {

class ObjectListItem : public KListViewItem
{
  public:
    ObjectListItem(KListView* parent, KivioStencil* _stencil)
      : KListViewItem(parent, "")
    {
      m_stencil = _stencil;
      setPixmap(0, Kivio::generatePixmapFromStencil(22, 22, m_stencil));

      QString type;
      QString name;

      switch(m_stencil->type()) {
        case kstGroup:
          type = i18n("Group");
          name = type;
          break;
        case kstConnector:
          type = i18n("Connector");
          name = _stencil->spawner()->info()->title();
          break;
        case kstText:
          type = i18n("Text Area");
          name = type;
          break;
        case kstNormal:
        default:
          type = i18n("Stencil");
          name = _stencil->spawner()->info()->title();
          break;
      }

      setText(0, name);
      setText(1, type);
      setSelected(m_stencil->isSelected());
    }

    KivioStencil* stencil() const { return m_stencil; }

  private:
    KivioStencil* m_stencil;
};

ObjectListPalette::ObjectListPalette(KivioView* parent, const char* name)
  : QWidget(parent, name), m_view(parent)
{
  m_blockUpdate = false;

  QVBoxLayout* layout = new QVBoxLayout(this, 0, 2);

  m_objectList = new KListView(this);
  m_objectList->setFullWidth(true);
  m_objectList->setAllColumnsShowFocus(true);
  m_objectList->setSorting(-1);
  m_objectList->setSelectionMode(QListView::Extended);
  m_objectList->addColumn(i18n("Name"));
  m_objectList->addColumn(i18n("Type"));

  layout->addWidget(m_objectList);

  connect(m_objectList, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));
}

ObjectListPalette::~ObjectListPalette()
{
}

void ObjectListPalette::updateObjectList()
{
  if(m_blockUpdate) {
    m_blockUpdate = false;
    return;
  }

  KivioPage* page = m_view->activePage();

  if(!page)
    return;

  m_objectList->clear();

  QPtrList<KivioLayer>* layers = page->layers();

  KivioLayer* layer = layers->first();
  KivioStencil* stencil;
  m_objectList->blockSignals(true);

  while(layer) {
    stencil = layer->firstStencil();

    while(stencil) {
      new ObjectListItem(m_objectList, stencil);
      stencil = layer->nextStencil();
    }

    layer = layers->next();
  }

  m_objectList->blockSignals(false);
}

void ObjectListPalette::updateSelection()
{
  KivioPage* page = m_view->activePage();

  if(!page)
    return;

  page->unselectAllStencils();

  QPtrList<QListViewItem> selectedItems = m_objectList->selectedItems();
  QPtrListIterator<QListViewItem> it(selectedItems);
  ObjectListItem* item = 0;

  while((item = static_cast<ObjectListItem*>(it.current())) != 0) {
    page->selectStencil(item->stencil());
    ++it;
  }

  m_blockUpdate = true;
  m_view->doc()->updateView(page);
}

}

#include "objectlistpalette.moc"
