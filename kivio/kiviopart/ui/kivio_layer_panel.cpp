#include "kivio_layer_panel.h"

#include "kivio_view.h"
#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivio_layer.h"

#include "viewitemrenamedialog.h"

#include <qheader.h>
#include <qlayout.h>

#include <ktoolbar.h>
#include <kaction.h>
#include <kiconloader.h>

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
  setPixmap(0, data->visible() ? BarIcon("layer_visible"):BarIcon("layer_novisible"));
  setPixmap(1, true/*data->print()*/ ? BarIcon("layer_print"):BarIcon("layer_noprint"));
  setPixmap(2, true/*data->edit()*/ ? BarIcon("layer_editable"):BarIcon("layer_noeditable"));
  setPixmap(3, data->connectable() ? BarIcon("layer_connect"):BarIcon("layer_noconnect"));
  setText(4,data->name());
}
/*********************************************************/
KivioLayerPanel::KivioLayerPanel(KivioView* view, QWidget* parent, const char* name)
: KivioLayerPanelBase(parent, name), m_pView(view)
{
  list->header()->hide();
  list->addColumn("view",15);
  list->addColumn("print",15);
  list->addColumn("editable",15);
  list->addColumn("connect",15);
  list->addColumn("name",-1);
  list->setSorting(5,true);
  list->installEventFilter(this);

  actNew = new KAction( i18n("New Layer"), "layer_add", 0, this, SLOT(addItem()), this);
  actDel = new KAction( i18n("Remove Layer"), "layer_remove", 0, this, SLOT(removeItem()), this);
  actRename = new KAction( i18n("Rename Layer"), "item_rename", 0, this, SLOT(renameItem()), this);
  actUp = new KAction( i18n("Move Layer Up"), "up", 0, this, SLOT(upItem()), this);
  actDown = new KAction( i18n("Move Layer Down"), "down", 0, this, SLOT(downItem()), this);

  actNew->plug(bar);
  actDel->plug(bar);
  bar->insertSeparator();
  actRename->plug(bar);
  bar->insertSeparator();
  actUp->plug(bar);
  actDown->plug(bar);
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
  layer->setName(QString("Layer %1").arg(id));
  page->addLayer(layer);
  page->setCurLayer(layer);

  m_pView->doc()->updateView(page);

  KivioLayerItem* item = new KivioLayerItem(list, layer, id++);
  list->sort();
  list->setCurrentItem(item);
}

void KivioLayerPanel::removeItem()
{
  KivioLayerItem* item = (KivioLayerItem*)list->currentItem();
  if (!item)
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

  ViewItemRenameDialog* dlg = new ViewItemRenameDialog(i18n("Rename Layer"), i18n("Layer name:"), this);

  KivioLayer* layer = i->data;
  dlg->setText(layer->name());

  if( dlg->exec() == QDialog::Accepted )
  {
    layer->setName(dlg->text());
  }
  delete dlg;

  i->update();
  m_pView->doc()->setModified(true);
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
  if(pos == 0)
    return;

  layer = page->layers()->take();
  if( !layer )
    return;

  page->layers()->insert(pos-1, layer);

  QString t = above->text(5);
  above->setText(5,item->text(5));
  item->setText(5,t);

  list->sort();

  updateButtons(item);

  page->setCurLayer(layer);
  m_pView->doc()->updateView(page);
/*
*/
{
//  debug("---------------------------");
  KivioPage* page = m_pView->activePage();
  KivioLayer* layer = page->firstLayer();
//  KivioLayerItem* ci = 0;

  while (layer) {
  //  debug(layer->name());
    layer = page->nextLayer();
  }
}
/*
*/

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
  if (pos == (int)page->layers()->count()-1)
    return;

  layer = page->layers()->take();
  if( !layer )
    return;

  page->layers()->insert(pos+1, layer);

  QString t = below->text(5);
  below->setText(5,item->text(5));
  item->setText(5,t);

  list->sort();

  updateButtons(item);

  page->setCurLayer(layer);
  m_pView->doc()->updateView(page);

/*
*/
{
//  debug("---------------------------");
  KivioPage* page = m_pView->activePage();
  KivioLayer* layer = page->firstLayer();
//  KivioLayerItem* ci = 0;

  while (layer) {
  //  debug(layer->name());
    layer = page->nextLayer();
  }
}
/*
*/
}

void KivioLayerPanel::updateButtons(QListViewItem* i)
{
  if (!i) {
    actDel->setEnabled(false);
    actRename->setEnabled(false);
    actUp->setEnabled(false);
    actDown->setEnabled(false);
  } else {
    actDel->setEnabled(true);
    actRename->setEnabled(true);
    actUp->setEnabled(i->itemAbove());
    actDown->setEnabled(i->itemBelow());
  }
}

void KivioLayerPanel::itemClicked(QListViewItem* i, const QPoint&, int c)
{
  if (!i)
    return;

  KivioLayerItem* vi = (KivioLayerItem*)i;
  KivioLayer* layer = vi->data;

  if (c==0)
    layer->setVisible(!layer->visible());
/*
  if (c==1)
    layer->setPrint(!layer->print());

  if (c==2)
    layer->setEdit(!layer->editt());
*/
  if (c==3)
    layer->setConnectable(!layer->connectable());

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
  KivioLayerItem* ci = 0;

  while (layer) {
    KivioLayerItem* i = new KivioLayerItem(list, layer, id++);
    if (layer == page->curLayer())
      ci = i;
    layer = page->nextLayer();
  }

  if (ci)
    list->setCurrentItem(ci);

  list->sort();
  updateButtons(list->currentItem());
}

#include "kivio_layer_panel.moc"
