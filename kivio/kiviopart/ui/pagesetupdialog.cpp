#include "pagesetupdialog.h"
#include "kivio_view.h"
#include "tkcombobox.h"
#include "tkfloatspinbox.h"
#include "tkunits.h"
#include "tkvisualpage.h"

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

  {"null",0,0,-2}     // -2 foe end of list
};

PageSetupDialog::PageSetupDialog(KivioView* view, QWidget* parent, const char* name)
: PageSetupDialogBase(parent, name)
{
  orientation->insertItem(i18n("Partrait"));
  orientation->insertItem(i18n("Landscape"));

  int k = 0;
  PaperSizeDef pdef = PapersTable[0];
  while (pdef.unit != -2) {
    format->insertItem(pdef.title);
    pdef = PapersTable[++k];
  }

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

#include "pagesetupdialog.moc"
