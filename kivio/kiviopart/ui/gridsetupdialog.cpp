#include "gridsetupdialog.h"
#include "kivio_view.h"
#include "kivio_doc.h"
#include "kivio_grid_data.h"
#include "tkfloatspinbox.h"
#include "tkunits.h"

#include <kcolorbtn.h>
#include <qcheckbox.h>

GridSetupDialog::GridSetupDialog(KivioView* view, QWidget* parent, const char* name)
: GridSetupDialogBase(parent, name)
{
  m_pDoc = view->doc();

  KivioGridData d = m_pDoc->grid();
  int unit = d.freq.unit;
  unitBox->setUnit(m_pDoc->units());
  unitBox->activate();

  showGrid->setChecked(d.isShow);
  snapGrid->setChecked(d.isSnap);
  gridColor->setColor(d.color);
  freqX->setValue(d.freq.w,unit);
  freqY->setValue(d.freq.h,unit);
  distX->setValue(d.snap.w,unit);
  distY->setValue(d.snap.h,unit);
}

GridSetupDialog::~GridSetupDialog()
{
}

void GridSetupDialog::apply(QWidget* page)
{
  if (page != this)
    return;

  KivioGridData d = m_pDoc->grid();
  int unit = unitBox->currentItem();

  d.color = gridColor->color();
  d.isShow =  showGrid->isChecked();
  d.isSnap =  snapGrid->isChecked();
  d.freq.set(freqX->value(unit),freqY->value(unit),unit);
  d.snap.set(distX->value(unit),distY->value(unit),unit);

  m_pDoc->setGrid(d);
  m_pDoc->updateView(0, true);
}

#include "gridsetupdialog.moc"
