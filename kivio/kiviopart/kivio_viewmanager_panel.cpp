#include "kivio_viewmanager_panel.h"
#include "kivio_view.h"
#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivio_canvas.h"
#include "kivio_common.h"

#include "viewitemrenamedialog.h"

#include <qheader.h>
#include <qlayout.h>

#include <ktoolbar.h>
#include <kaction.h>
#include <kiconloader.h>

ViewItemList::ViewItemList(QObject* parent, const char* name)
: QObject(parent, name)
{
  list.setAutoDelete(true);
  freeId = 0;
}

ViewItemList::~ViewItemList()
{
}

void ViewItemList::save(QDomElement& element)
{
  for (ViewItemData* d = list.first(); d; d = list.next()) {
    QDomElement e = element.ownerDocument().createElement("ViewItemData");
    element.appendChild(e);
    XmlWriteString(e, "name", d->name);
    XmlWriteInt(e, "pageId", d->pageId);
    XmlWriteRect(e, "rect", d->rect);
    XmlWriteInt(e, "zoom", (int)d->isZoom);
    XmlWriteInt(e, "page", (int)d->isPage);
  }
}

void ViewItemList::load(const QDomElement& element)
{
  emit reset();

  freeId = 0;
  list.clear();

  QDomElement e = element.firstChild().toElement();
  for( ; !e.isNull(); e = e.nextSibling().toElement() )
  {
    ViewItemData* d = new ViewItemData;
    d->name = XmlReadString(e, "name", "NoName");
    d->id = freeId++;
    d->pageId = XmlReadInt(e, "pageId", 0);
    d->rect = XmlReadRect(e, "rect", KivioRect());
    d->isZoom = (bool)XmlReadInt(e, "zoom", 0);
    d->isPage = (bool)XmlReadInt(e, "page", 0);
    list.append(d);
  }
  emit reset();
}

int ViewItemList::add(ViewItemData* d)
{
  d->id = freeId++;
  list.append(d);
  emit itemAdd(d);

  return d->id;
}

void ViewItemList::remove(ViewItemData* d)
{
  emit itemRemoved(d);
  list.remove(d);
}

void ViewItemList::update(ViewItemData* d)
{
  emit itemChanged(d);
}

void ViewItemList::activate(ViewItemData* d, KivioView* view)
{
  if (d->isPage)
    view->setActivePage(KivioPage::find(d->pageId));

  if (d->isZoom)
   view->canvasWidget()->setVisibleArea(d->rect);
}
/*********************************************************/
KivioViewItem::KivioViewItem(QListView* parent, ViewItemData* d)
: QListViewItem(parent), data(d)
{
  QString ids;
  QString space;
  ids.setNum(data->id);
  space.fill(' ',10-ids.length());
  ids.prepend(space);
  setText(3,ids);

  update();
}

KivioViewItem::~KivioViewItem()
{
}

void KivioViewItem::update()
{
  setPixmap(0, data->isZoom ? BarIcon("zoom_enabled"):BarIcon("zoom_disabled"));
  setPixmap(1, data->isPage ? BarIcon("page_enabled"):BarIcon("page_disabled"));
  setText(2,data->name);
}
/*********************************************************/
KivioViewManagerPanel::KivioViewManagerPanel(KivioView* view, QWidget* parent, const char* name)
: QWidget(parent, name), m_pView(view)
{
  QVBoxLayout* l = new QVBoxLayout(this);

  list = new QListView(this);
  list->header()->hide();
  list->addColumn("zoom",15);
  list->addColumn("page",15);
  list->addColumn("name",-1);
  list->setAllColumnsShowFocus(true);
  list->setSorting(3,true);
  list->installEventFilter(this);

  connect(list, SIGNAL(clicked(QListViewItem*,const QPoint&,int)), SLOT(itemClicked(QListViewItem*,const QPoint&,int)));
  connect(list, SIGNAL(doubleClicked(QListViewItem*)), SLOT(itemActivated(QListViewItem*)));
  connect(list, SIGNAL(returnPressed(QListViewItem*)), SLOT(itemActivated(QListViewItem*)));
  connect(list, SIGNAL(currentChanged(QListViewItem*)), SLOT(updateButtons(QListViewItem*)));

  KToolBar* bar = new KToolBar(this);
  bar->setFullSize(true);

  actNew = new KAction( i18n("Add current view"), "item_add", 0, this, SLOT(addItem()), this);
  actDel = new KAction( i18n("Remove item"), "item_remove", 0, this, SLOT(removeItem()), this);
  actRename = new KAction( i18n("Rename item"), "item_rename", 0, this, SLOT(renameItem()), this);
  actUp = new KAction( i18n("Move item Up"), "up", 0, this, SLOT(upItem()), this);
  actDown = new KAction( i18n("Move item Down"), "down", 0, this, SLOT(downItem()), this);

  actNew->plug(bar);
  actDel->plug(bar);
  bar->insertSeparator();
  actRename->plug(bar);
  bar->insertSeparator();
  actUp->plug(bar);
  actDown->plug(bar);

  l->addWidget(bar);
  l->addWidget(list,1);

  viewItems = m_pView->doc()->viewItems();
  connect(viewItems, SIGNAL(itemAdd(ViewItemData*)), SLOT(itemAdd(ViewItemData*)));
  connect(viewItems, SIGNAL(itemRemoved(ViewItemData*)), SLOT(itemRemoved(ViewItemData*)));
  connect(viewItems, SIGNAL(itemChanged(ViewItemData*)), SLOT(itemChanged(ViewItemData*)));
  connect(viewItems, SIGNAL(reset()), SLOT(reset()));

  reset();
}

KivioViewManagerPanel::~KivioViewManagerPanel()
{
}

bool KivioViewManagerPanel::eventFilter(QObject* o, QEvent* ev)
{
  if (o == list && (ev->type() == QEvent::LayoutHint || ev->type() == QEvent::Resize)) {
    int s = list->width() - 2*(list->margin() + list->lineWidth());
    s -= list->header()->sectionSize(0);
    s -= list->header()->sectionSize(1);
    list->header()->resizeSection(2,s);
    list->triggerUpdate();
  }

  return QWidget::eventFilter(o, ev);
}

void KivioViewManagerPanel::addItem()
{
  ViewItemData* d = new ViewItemData;
  d->name = QString("%1-%2%").arg(m_pView->activePage()->pageName()).arg((int)(m_pView->canvasWidget()->zoom()*100.1f));
  d->pageId = m_pView->activePage()->id();
  d->rect = m_pView->canvasWidget()->visibleArea();
  d->isZoom = true;
  d->isPage = true;

  viewItems->add(d);
}

void KivioViewManagerPanel::removeItem()
{
  KivioViewItem* i = (KivioViewItem*)list->currentItem();
  if (!i)
    return;

  viewItems->remove(i->data);
}

void KivioViewManagerPanel::renameItem()
{
  KivioViewItem* i = (KivioViewItem*)list->currentItem();
  if (!i)
    return;

  ViewItemRenameDialog* dlg = new ViewItemRenameDialog(i18n("Rename View Item"), i18n("View item name:"), this);
  dlg->setText(i->data->name);

  if( dlg->exec() == QDialog::Accepted )
  {
    i->data->name = dlg->text();
  }
  delete dlg;

  viewItems->update(i->data);
}

void KivioViewManagerPanel::upItem()
{
  QListViewItem* item = list->currentItem();
  if (!item)
    return;

  QListViewItem* above = item->itemAbove();
  if (!above)
    return;

  QString t = above->text(3);
  above->setText(3,item->text(3));
  item->setText(3,t);

  list->sort();

  updateButtons(item);
}

void KivioViewManagerPanel::downItem()
{
  QListViewItem* item = list->currentItem();
  if (!item)
    return;

  QListViewItem* below = item->itemBelow();
  if (!below)
    return;

  QString t = below->text(3);
  below->setText(3,item->text(3));
  item->setText(3,t);

  list->sort();

  updateButtons(item);
}

void KivioViewManagerPanel::updateButtons(QListViewItem* i)
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

void KivioViewManagerPanel::itemClicked(QListViewItem* i, const QPoint&, int c)
{
  if (!i)
    return;

  KivioViewItem* vi = (KivioViewItem*)i;
  if (c==0)
    vi->data->isZoom = !vi->data->isZoom;

  if (c==1)
    vi->data->isPage = !vi->data->isPage;

  vi->update();
}

void KivioViewManagerPanel::itemActivated(QListViewItem* i)
{
  if (!i)
    return;

  KivioViewItem* vi = (KivioViewItem*)i;
  viewItems->activate(vi->data, m_pView);
}

void KivioViewManagerPanel::itemAdd(ViewItemData* d)
{
  KivioViewItem* i = new KivioViewItem(list, d);
  list->sort();
  list->setCurrentItem(i);
}

void KivioViewManagerPanel::itemRemoved(ViewItemData* d)
{
  KivioViewItem* item = (KivioViewItem*)list->firstChild();
  while (item) {
    if (item->data == d)
      break;
    item = (KivioViewItem*)item->itemBelow();
  }

  if (item)
    delete item;
}

void KivioViewManagerPanel::itemChanged(ViewItemData* d)
{
  KivioViewItem* item = (KivioViewItem*)list->firstChild();
  while (item) {
    if (item->data == d)
      break;
    item = (KivioViewItem*)item->itemBelow();
  }

  if (item)
    item->update();
}

void KivioViewManagerPanel::reset()
{
  list->clear();
  ViewItemDataList dl = viewItems->data();
  for (ViewItemData* d = dl.first(); d; d = dl.next())
    (void) new KivioViewItem(list, d);

  list->sort();
  updateButtons(list->currentItem());
}

