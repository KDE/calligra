#include "guides2page.h"
#include "guidessetupdialog.h"

#include "tkfloatspinbox.h"
#include "tkcombobox.h"

#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivio_canvas.h"
#include "kivio_view.h"
#include "kivio_guidelines.h"

#include <kiconloader.h>
#include <kdebug.h>

#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>

GuidesTwoPositionPage::GuidesTwoPositionPage(KivioView* view, QWidget* parent, const char* name)
: GuidesTwoPositionPageBase(parent, name)
{
  installEventFilter(this);

  m_pCanvas = view->canvasWidget();
  m_pPage = view->activePage();

  listView->addColumn("",20);
  listView->addColumn("",1);
  listView->addColumn("",1);
  listView->header()->hide();
  listView->setColumnAlignment(1,AlignRight);
  listView->setColumnAlignment(2,AlignRight);
  listView->clipper()->installEventFilter(this);

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

  updateListView(true);
}

GuidesTwoPositionPage::~GuidesTwoPositionPage()
{
}

void GuidesTwoPositionPage::slotUnitChanged(int u)
{
  for (QListViewItem* i = listView->firstChild(); i; i = i->nextSibling())
    ((GuidesListViewItem*)i)->setUnit(u);
}

void GuidesTwoPositionPage::updateListView(bool rebuild)
{
  if (rebuild) {
    KivioGuidesList list = m_pPage->guideLines()->guides();
    list.sort();
    listView->clear();
    for (KivioGuideLineData* d = list.first(); d; d = list.next()) {
      GuidesListViewItem* i = new GuidesListViewItem(listView,d,true);
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

void GuidesTwoPositionPage::updateListViewColumn()
{
  int s = listView->clipper()->width();
  s -= listView->header()->sectionSize(0);
  listView->setColumnWidth(1,s/2);
  listView->setColumnWidth(2,s/2);
  listView->triggerUpdate();
}

void GuidesTwoPositionPage::slotMoveButton()
{
  KivioGuideLineData* data = 0;
  if (listView->currentItem())
    data = ((GuidesListViewItem*)listView->currentItem())->guideData();

  if (!data)
    return;

  m_pCanvas->eraseGuides();
  KivioGuideLines* gl = m_pPage->guideLines();
  if (data->orientation() == Vertical) {
    gl->moveSelectedByX(posVertical->value() - data->position());
    gl->moveSelectedByY(posHorizontal->value());
  } else {
    gl->moveSelectedByY(posHorizontal->value() - data->position());
    gl->moveSelectedByX(posVertical->value());
  }

  m_pCanvas->paintGuides();
  updateListView(true);

  setCurrent(data);
}

void GuidesTwoPositionPage::slotMoveByButton()
{
  KivioGuideLineData* data = 0;
  if (listView->currentItem())
    data = ((GuidesListViewItem*)listView->currentItem())->guideData();

  m_pCanvas->eraseGuides();
  KivioGuideLines* gl = m_pPage->guideLines();
  gl->moveSelectedByX(posVertical->value());
  gl->moveSelectedByY(posHorizontal->value());
  m_pCanvas->paintGuides();
  updateListView(true);

  setCurrent(data);
}

void GuidesTwoPositionPage::slotDeleteButton()
{
  m_pCanvas->eraseGuides();
  KivioGuideLines* gl = m_pPage->guideLines();
  gl->removeSelected();
  m_pCanvas->paintGuides();
  updateListView(true);
  slotCurrentChanged(0);
}

void GuidesTwoPositionPage::slotDeleteAllButton()
{
  listView->selectAll(true);
  slotDeleteButton();
  slotCurrentChanged(0);
}

void GuidesTwoPositionPage::slotCurrentChanged(QListViewItem* i)
{
  if (!i) {
    posVertical->setValue(0.0,UnitPoint);
    posHorizontal->setValue(0.0,UnitPoint);
    icon->setPixmap(QPixmap());
    moveButton->setEnabled(false);
    return;
  }

  KivioGuideLineData* d = ((GuidesListViewItem*)i)->guideData();
  if (d->orientation() == Vertical) {
    posVertical->setValue(d->position(),UnitPoint);
    posHorizontal->setValue(0.0,UnitPoint);
    icon->setPixmap(BarIcon("guides_vertical"));
  } else {
    posHorizontal->setValue(d->position(),UnitPoint);
    posVertical->setValue(0.0,UnitPoint);
    icon->setPixmap(BarIcon("guides_horizontal"));
  }
  moveButton->setEnabled(true);
}

void GuidesTwoPositionPage::setCurrent(KivioGuideLineData* data)
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

bool GuidesTwoPositionPage::eventFilter(QObject* o, QEvent* ev)
{
  if (o == this && ev->type() == QEvent::Show) {
    updateListView(true);
  }

  if (o == listView->clipper() && (ev->type() == QEvent::LayoutHint || ev->type() == QEvent::Resize)) {
    updateListViewColumn();
  }
  return GuidesTwoPositionPageBase::eventFilter(o,ev);
}

void GuidesTwoPositionPage::slotClearSelectionButton()
{
  KivioGuideLines* gl = m_pPage->guideLines();
  gl->unselectAll();

  m_pCanvas->updateGuides();
  updateListView(false);
}

void GuidesTwoPositionPage::selectionChanged()
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

void GuidesTwoPositionPage::slotSelectAllButton()
{
  KivioGuideLines* gl = m_pPage->guideLines();
  gl->selectAll();

  m_pCanvas->updateGuides();
  updateListView(false);
}

void GuidesTwoPositionPage::apply(QWidget*)
{
}

#include "guides2page.moc"
