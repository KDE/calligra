#include "guides1page.h"
#include "guidessetupdialog.h"

#include "tkfloatspinbox.h"
#include "tkcombobox.h"

#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivio_canvas.h"
#include "kivio_view.h"
#include "kivio_guidelines.h"

#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kiconloader.h>

GuidesOnePositionPage::GuidesOnePositionPage(Orientation o, KivioView* view, QWidget* parent, const char* name)
: GuidesOnePositionPageBase(parent, name)
{
  m_pCanvas = view->canvasWidget();
  m_pPage = view->activePage();
  orientation = o;

  listView->addColumn("",20);
  listView->addColumn("",1);
  listView->header()->hide();
  listView->setColumnAlignment(1,AlignRight);
  listView->installEventFilter(this);

  connect(addButton,SIGNAL(clicked()),SLOT(slotAddButton()));
  connect(moveButton,SIGNAL(clicked()),SLOT(slotMoveButton()));
  connect(moveByButton,SIGNAL(clicked()),SLOT(slotMoveByButton()));
  connect(deleteButton,SIGNAL(clicked()),SLOT(slotDeleteButton()));
  connect(deleteAllButton,SIGNAL(clicked()),SLOT(slotDeleteAllButton()));
  connect(selectAllButton,SIGNAL(clicked()),SLOT(slotSelectAllButton()));
  connect(clearSelectionButton,SIGNAL(clicked()),SLOT(slotClearSelectionButton()));

  connect(units,SIGNAL(activated(int)),SLOT(slotUnitChanged(int)));

  connect(listView,SIGNAL(currentChanged(QListViewItem*)),SLOT(slotCurrentChanged(QListViewItem*)));
  connect(listView,SIGNAL(selectionChanged()),SLOT(selectionChanged()));

  units->setUnit(m_pPage->doc()->units());
  units->activate();

  icon->setPixmap(BarIcon(orientation == Qt::Vertical ? "guides_vertical":"guides_horizontal"));

  updateListView(true);
}

GuidesOnePositionPage::~GuidesOnePositionPage()
{
}

void GuidesOnePositionPage::slotUnitChanged(int u)
{
  for (QListViewItem* i = listView->firstChild(); i; i = i->nextSibling())
    ((GuidesListViewItem*)i)->setUnit(u);
}

void GuidesOnePositionPage::updateListView(bool rebuild)
{
  if (rebuild) {
    KivioGuidesList list = m_pPage->guideLines()->guides();
    list.sort();
    listView->clear();
    for (KivioGuideLineData* d = list.first(); d; d = list.next())
      if (d->orientation() == orientation) {
        GuidesListViewItem* i = new GuidesListViewItem(listView,d,false);
        i->setUnit(units->unit());
      }
    updateListViewColumn();
    slotCurrentChanged(0);
    listView->triggerUpdate();
    return;
  }

  bool haveSelections = false;
  for (QListViewItem* i = listView->firstChild(); i; i = i->nextSibling()) {
    KivioGuideLineData* d = ((GuidesListViewItem*)i)->guideData();
    i->setSelected(d->isSelected());
    if (d->isSelected())
      haveSelections = true;
  }
  listView->triggerUpdate();

  moveByButton->setEnabled(haveSelections);
  deleteButton->setEnabled(haveSelections);

  listView->setFocus();
}

void GuidesOnePositionPage::updateListViewColumn()
{
  int s = listView->width() - 2*(listView->margin() + listView->lineWidth());
  s -= listView->header()->sectionSize(0);
  listView->setColumnWidth(1,s);
  listView->triggerUpdate();
}

void GuidesOnePositionPage::slotAddButton()
{
  m_pCanvas->eraseGuides();
  KivioGuideLines* gl = m_pPage->guideLines();
  KivioGuideLineData* data = gl->add(pos->value(),orientation);
  m_pCanvas->paintGuides();
  updateListView(true);

  setCurrent(data);
}

void GuidesOnePositionPage::setCurrent(KivioGuideLineData* data)
{
  for (QListViewItem* i = listView->firstChild(); i; i = i->nextSibling()) {
    KivioGuideLineData* d = ((GuidesListViewItem*)i)->guideData();
    if (d == data) {
      listView->setCurrentItem(i);
      return;
    }
  }
  listView->setCurrentItem(0);
}

void GuidesOnePositionPage::slotMoveButton()
{
  KivioGuideLineData* data = 0;
  if (listView->currentItem())
    data = ((GuidesListViewItem*)listView->currentItem())->guideData();

  if (!data)
    return;

  m_pCanvas->eraseGuides();
  KivioGuideLines* gl = m_pPage->guideLines();
  if (orientation == Vertical)
    gl->moveSelectedByX(pos->value() - data->position());
  else
    gl->moveSelectedByY(pos->value() - data->position());

  m_pCanvas->paintGuides();
  updateListView(true);

  setCurrent(data);
}

void GuidesOnePositionPage::slotMoveByButton()
{
  KivioGuideLineData* data = 0;
  if (listView->currentItem())
    data = ((GuidesListViewItem*)listView->currentItem())->guideData();

  m_pCanvas->eraseGuides();
  KivioGuideLines* gl = m_pPage->guideLines();
  if (orientation == Vertical)
    gl->moveSelectedByX(pos->value());
  else
    gl->moveSelectedByY(pos->value());

  m_pCanvas->paintGuides();
  updateListView(true);

  setCurrent(data);
}

void GuidesOnePositionPage::slotDeleteButton()
{
  m_pCanvas->eraseGuides();
  KivioGuideLines* gl = m_pPage->guideLines();
  gl->removeSelected();
  m_pCanvas->paintGuides();
  updateListView(true);
  slotCurrentChanged(0);
}

void GuidesOnePositionPage::slotDeleteAllButton()
{
  listView->selectAll(true);
  slotDeleteButton();
  slotCurrentChanged(0);
}

void GuidesOnePositionPage::slotCurrentChanged(QListViewItem* i)
{
  if (!i) {
    pos->setValue(0.0,UnitPoint);
    moveButton->setEnabled(false);
    return;
  }

  KivioGuideLineData* d = ((GuidesListViewItem*)i)->guideData();
  pos->setValue(d->position(),UnitPoint);

  moveButton->setEnabled(true);
}

bool GuidesOnePositionPage::eventFilter(QObject* o, QEvent* ev)
{
  if (o == listView && (ev->type() == QEvent::LayoutHint || ev->type() == QEvent::Resize)) {
    updateListViewColumn();
  }
  return GuidesOnePositionPageBase::eventFilter(o,ev);
}

void GuidesOnePositionPage::slotClearSelectionButton()
{
  KivioGuideLines* gl = m_pPage->guideLines();
  for (QListViewItem* i = listView->firstChild(); i; i = i->nextSibling()) {
    KivioGuideLineData* d = ((GuidesListViewItem*)i)->guideData();
    gl->unselect(d);
  }

  m_pCanvas->updateGuides();
  updateListView(false);
}

void GuidesOnePositionPage::selectionChanged()
{
  KivioGuideLines* gl = m_pPage->guideLines();
  for (QListViewItem* i = listView->firstChild(); i; i = i->nextSibling()) {
    KivioGuideLineData* d = ((GuidesListViewItem*)i)->guideData();
    if (i->isSelected())
      gl->select(d);
    else
      gl->unselect(d);
  }

  m_pCanvas->updateGuides();
}

void GuidesOnePositionPage::slotSelectAllButton()
{
  KivioGuideLines* gl = m_pPage->guideLines();
  for (QListViewItem* i = listView->firstChild(); i; i = i->nextSibling()) {
    KivioGuideLineData* d = ((GuidesListViewItem*)i)->guideData();
    gl->select(d);
  }

  m_pCanvas->updateGuides();
  updateListView(false);
}
