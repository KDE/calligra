#include "pagesetupdialog.h"
#include "kivio_view.h"
#include "kivio_page.h"
#include "kivio_doc.h"
#include "kivio_map.h"
#include "kivio_config.h"

#include "tkcombobox.h"
#include "tkfloatspinbox.h"
#include "tkunits.h"
#include "tkvisualpage.h"

#include "qcheckbox.h"
#include "klocale.h"

struct PaperSizeDef {
  const char* title;
  float width;
  float height;
  int unit;
};

static const PaperSizeDef PapersTable[] = {
  {"A0",841,1189,UnitMillimeter},
  {"A1",594,841,UnitMillimeter},
  {"A2",420,594,UnitMillimeter},
  {"A3",297,420,UnitMillimeter},
  {"A4",210,297,UnitMillimeter},
  {"A5",148,210,UnitMillimeter},

  {"B4",250,353,UnitMillimeter},
  {"B6",125,176,UnitMillimeter},

  {"C4",229,324,UnitMillimeter},
  {"C5",162,229,UnitMillimeter},
  {"C5",114,162,UnitMillimeter},

  {"DL",110,220,UnitMillimeter},

  {"Letter",8.5,11,UnitInch},
  {"Legal",8.5,14,UnitInch},

  {"A",8.5,11,UnitInch},
  {"B",11,17,UnitInch},
  {"C",17,22,UnitInch},
  {"D",22,34,UnitInch},
  {"E",34,44,UnitInch},

  {"Custom",0,0,-1},  // -1 for Custom

  {"null",0,0,-2}     // -2 for end of list
};

PageSetupDialog::PageSetupDialog(KivioView* view, QWidget* parent, const char* name)
: PageSetupDialogBase(parent, name)
{
  m_pPage = view->activePage();

  TKPageLayout l = m_pPage->paperLayout();

  orientation->insertItem(i18n("Portrait"));
  orientation->insertItem(i18n("Landscape"));

  pUnit->setUnit(l.unit);
  pUnit->activate();

  int k = 0;
  int cur = -1;
  int orient = 0;
  PaperSizeDef pdef = PapersTable[0];
  while (pdef.unit != -2) {
    format->insertItem(pdef.title);
    pdef = PapersTable[++k];
    if (l.width == pdef.width && l.height == pdef.height && l.unit == pdef.unit) {
      cur = k;
      orient = 0;
    } else {
      if (l.width == pdef.height && l.height == pdef.width && l.unit == pdef.unit) {
        cur = k;
        orient = 1;
      }
    }
    if (pdef.unit == -1 && cur == -1) {
      cur = k;
      pWidth->setValue(l.width, l.unit);
      pHeight->setValue(l.height, l.unit);
    }
  }

  mLeft->setValue(l.marginLeft, l.unit);
  mRight->setValue(l.marginRight, l.unit);
  mTop->setValue(l.marginTop, l.unit);
  mBottom->setValue(l.marginBottom, l.unit);

  orientation->setCurrentItem(orient);
  format->setCurrentItem(cur);

  update();
}

PageSetupDialog::~PageSetupDialog()
{
}

void PageSetupDialog::update()
{
  PaperSizeDef pdef = PapersTable[format->currentItem()];
  if (pdef.unit == -1) { // it's Custom item
    pWidth->setEnabled(true);
    pHeight->setEnabled(true);
    pUnit->setEnabled(true);
    phLabel->setEnabled(true);
  } else {
    int unit = pdef.unit;
    pWidth->setEnabled(false);
    pHeight->setEnabled(false);
    pUnit->setEnabled(false);
    phLabel->setEnabled(false);
    pWidth->setValue(pdef.width, unit);
    pHeight->setValue(pdef.height, unit);
    pUnit->setUnit(unit);
    pUnit->activate();
  }

  int w = (int)pWidth->value();
  int h = (int)pHeight->value();
  int l = (int)mLeft->value();
  int r = (int)mRight->value();
  int t = (int)mTop->value();
  int b = (int)mBottom->value();

  bool f = orientation->currentItem() == 0;
  QSize s = f ? QSize(w,h) : QSize(h,w);
  vpage->setupPage(s, l, r, t, b);
}

void PageSetupDialog::apply(QWidget* page)
{
  if (page != this)
    return;

  TKPageLayout l;
  int u = pUnit->unit();
  l.unit = u;
  if (orientation->currentItem() == 0) {
    l.width = pWidth->value(u);
    l.height = pHeight->value(u);
  } else {
    l.width = pHeight->value(u);
    l.height = pWidth->value(u);
  }
  l.marginLeft = mLeft->value(u);
  l.marginRight = mRight->value(u);
  l.marginTop = mTop->value(u);
  l.marginBottom = mBottom->value(u);

  KivioDoc* doc = m_pPage->doc();

  if (resizeAll->isChecked()) {
    KivioMap* map = doc->map();
    for (KivioPage* p = map->firstPage(); p; p = map->nextPage())
      p->setPaperLayout(l);
  } else {
    m_pPage->setPaperLayout(l);
  }

  if (docDefault->isChecked())
    doc->config()->setDefaultPageLayout(l);

  if (globalDefault->isChecked())
    doc->config()->setGlobalDefaultPageLayout(l);
}

#include "pagesetupdialog.moc"
