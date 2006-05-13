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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kivio_layer_panel.h"

#include "kivio_view.h"
#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivio_layer.h"
#include "kivio_factory.h"
#include "kivio_command.h"
#include "layerlisttooltip.h"

#include <qheader.h>
#include <QLayout>

#include <ktoolbar.h>
#include <klocale.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kinputdialog.h>

KivioLayerItem::KivioLayerItem(QListView* parent, KivioLayer* d, int id)
: QListViewItem(parent), data(d)
{
  QString ids;
  QString space;
  ids.setNum(id);
  space.fill(' ',10-ids.length());
  ids.prepend(space);
  setText(5,ids);

  update();
}

KivioLayerItem::~KivioLayerItem()
{
}

void KivioLayerItem::update()
{
  setPixmap(0, data->visible() ? SmallIcon("layer_visible",KivioFactory::global()):SmallIcon("layer_novisible",KivioFactory::global()));
  setPixmap(1, data->printable() ? SmallIcon("layer_print",KivioFactory::global()):SmallIcon("layer_noprint",KivioFactory::global()));
  setPixmap(2, data->editable() ? SmallIcon("layer_editable",KivioFactory::global()):SmallIcon("layer_noeditable",KivioFactory::global()));
  setPixmap(3, data->connectable() ? SmallIcon("layer_connect",KivioFactory::global()):SmallIcon("layer_noconnect",KivioFactory::global()));
  setText(4,data->name());
}


/*********************************************************/
KivioLayerPanel::KivioLayerPanel(KivioView* view, QWidget* parent, const char* name)
: KivioLayerPanelBase(parent, name), m_pView(view)
{
  list->header()->hide();
  list->addColumn(i18n("View"),15);
  list->addColumn(i18n("Print"),15);
  list->addColumn(i18n("Editable"),15);
  list->addColumn(i18n("Connect"),15);
  list->addColumn(i18n("Name"),-1);
  list->setSorting(5, false);
  list->installEventFilter(this);

  actNew = new KAction( i18n("New Layer"), BarIcon("layer_add",KivioFactory::global()), 0, this, SLOT(addItem()), this);
  actDel = new KAction( i18n("Remove Layer"), BarIcon("layer_remove",KivioFactory::global()), 0, this, SLOT(removeItem()), this);
  actRename = new KAction( i18n("Rename Layer"), BarIcon("item_rename",KivioFactory::global()), 0, this, SLOT(renameItem()), this);
  actUp = new KAction( i18n("Move Layer Up"), "up", 0, this, SLOT(upItem()), this);
  actDown = new KAction( i18n("Move Layer Down"), "down", 0, this, SLOT(downItem()), this);

  actNew->plug(bar);
  actDel->plug(bar);
  bar->insertSeparator();
  actRename->plug(bar);
  bar->insertSeparator();
  actUp->plug(bar);
  actDown->plug(bar);

  new Kivio::LayerListToolTip(list->viewport(), list);
}

KivioLayerPanel::~KivioLayerPanel()
{
}

bool KivioLayerPanel::eventFilter(QObject* o, QEvent* ev)
{
  if (o == list && (ev->type() == QEvent::LayoutHint || ev->type() == QEvent::Resize)) {
    int s = list->width() - 2*(list->margin() + list->lineWidth());
    s -= list->header()->sectionSize(0);
    s -= list->header()->sectionSize(1);
    s -= list->header()->sectionSize(2);
    s -= list->header()->sectionSize(3);
    list->header()->resizeSection(4,s);
    list->triggerUpdate();
  }

  return QWidget::eventFilter(o, ev);
}

void KivioLayerPanel::addItem()
{
  KivioPage* page = m_pView->activePage();
  KivioLayer* layer = new KivioLayer(page);

  // Set the layer name to something a bit different. This isn't
  // guaranteed to be unique, but it's better than "Untitled"
  layer->setName(i18n("Layer %1", id));
  page->addLayer(layer);
  page->setCurLayer(layer);

  m_pView->doc()->updateView(page);

  KivioAddLayerCommand * cmd = new KivioAddLayerCommand( i18n("Add Layer"), page, layer, id );
  m_pView->doc()->addCommand(cmd );

  KivioLayerItem* item = new KivioLayerItem(list, layer, id++);
  list->sort();
  list->setCurrentItem(item);
}

void KivioLayerPanel::removeItem()
{
  KivioLayerItem* item = (KivioLayerItem*)list->currentItem();
  if (!item || (m_pView->activePage()->layers()->count() <= 1))
    return;

  itemActivated(item);

  m_pView->activePage()->removeCurrentLayer();
  m_pView->doc()->updateView(m_pView->activePage());

  delete item;
}

void KivioLayerPanel::renameItem()
{
  KivioLayerItem* i = (KivioLayerItem*)list->currentItem();
  if (!i)
    return;

  KivioLayer* layer = i->data;
  QString oldText = layer->name();

  bool ok=false;
  QString newName = KInputDialog::getText(i18n("Rename Layer"),
    i18n("Layer name:"), oldText, &ok, this);

  if (ok) {
    layer->setName(newName);
    KivioRenameLayerCommand *cmd = new KivioRenameLayerCommand( i18n("Rename Layer"), layer, oldText, newName);
    m_pView->doc()->addCommand( cmd );
  }

  i->update();
}

void KivioLayerPanel::upItem()
{
  KivioLayerItem* item = (KivioLayerItem*)list->currentItem();
  if (!item)
    return;

  QListViewItem* above = item->itemAbove();
  if (!above)
    return;

  KivioPage* page = m_pView->activePage();
  KivioLayer* layer = item->data;

  int pos = page->layers()->find(layer);
  // It's already the top layer... return
  if(pos == (static_cast<int>(page->layers()->count()) - 1))
    return;

  layer = page->layers()->take();
  if( !layer )
    return;

  page->layers()->insert(pos + 1, layer);

  QString t = above->text(5);
  above->setText(5,item->text(5));
  item->setText(5,t);

  list->sort();

  updateButtons(item);

  page->setCurLayer(layer);
  m_pView->doc()->updateView(page);
}

void KivioLayerPanel::downItem()
{
  KivioLayerItem* item = (KivioLayerItem*)list->currentItem();
  if (!item)
    return;

  QListViewItem* below = item->itemBelow();
  if (!below)
    return;

  KivioPage* page = m_pView->activePage();
  KivioLayer* layer = item->data;

  int pos = page->layers()->find(layer);
  // It's already the bottom layer... return
  if (pos == 0)
    return;

  layer = page->layers()->take();
  if( !layer )
    return;

  page->layers()->insert(pos - 1, layer);

  QString t = below->text(5);
  below->setText(5,item->text(5));
  item->setText(5,t);

  list->sort();

  updateButtons(item);

  page->setCurLayer(layer);
  m_pView->doc()->updateView(page);
}

void KivioLayerPanel::updateButtons(QListViewItem* i)
{
  if (!i) {
    actDel->setEnabled(false);
    actRename->setEnabled(false);
    actUp->setEnabled(false);
    actDown->setEnabled(false);
  } else {
    if(m_pView->activePage()->layers()->count() > 1) {
      actDel->setEnabled(true);
    } else {
      actDel->setEnabled(false);
    }

    actRename->setEnabled(true);
    actUp->setEnabled(i->itemAbove());
    actDown->setEnabled(i->itemBelow());
  }
}

void KivioLayerPanel::itemClicked(QListViewItem* i, const QPoint&, int c)
{
  if(!i) {
    return;
  }

  KivioLayerItem* vi = static_cast<KivioLayerItem*>(i);
  KivioLayer* layer = vi->data;

  switch(c)
  {
    case 0:
      layer->setVisible(!layer->visible());
      break;
    case 1:
      layer->setPrintable(!layer->printable());
      break;
    case 2:
      layer->setEditable(!layer->editable());
      break;
    case 3:
      layer->setConnectable(!layer->connectable());
      break;
    default:
      break;
  }

  vi->update();
  m_pView->doc()->updateView(m_pView->activePage());
}

void KivioLayerPanel::itemActivated(QListViewItem* i)
{
  if (!i)
    return;

  KivioLayerItem* vi = (KivioLayerItem*)i;

  KivioPage* page = m_pView->activePage();
  page->setCurLayer(vi->data);

  // Switching layers unselects all stencils.  We REALLY don't want multiple
  // selections between layers.  If you don't do this, and multiple stencils
  // on various layers do occur and a group operation occurs, I have NO idea
  // what will happen since I didn't code it with multi-layer-group in mind...
  // or did I?  Anyway, if you get rid of this, you have to go check all the
  // multi-select operations.
  page->unselectAllStencils();
  m_pView->doc()->updateView(page);

  updateButtons(i);
}

void KivioLayerPanel::reset()
{
  id = 1;
  list->clear();

  KivioPage* page = m_pView->activePage();
  KivioLayer* layer = page->firstLayer();
  QListViewItem* ci = 0;

  while (layer) {
    KivioLayerItem* i = new KivioLayerItem(list, layer, id++);
    if (layer == page->curLayer())
      ci = i;
    layer = page->nextLayer();
  }

  if(!ci) {
    ci = list->firstChild();
  }

  list->setSelected(ci, true);
  list->sort();
  updateButtons(list->currentItem());
}

#include "kivio_layer_panel.moc"
