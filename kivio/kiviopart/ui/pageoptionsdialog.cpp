#include "pageoptionsdialog.h"

#include "kivio_doc.h"
#include "kivio_view.h"

#include <qcheckbox.h>

PageOptionsDialog::PageOptionsDialog(KivioView* view, QWidget* parent, const char* name)
: PageOptionsDialogBase(parent, name)
{
  m_pView = view;
  units->setUnit(m_pView->doc()->units());
  showBorders->setChecked(m_pView->isShowPageBorders());
  showMargins->setChecked(m_pView->isShowPageMargins());
  showRules->setChecked(m_pView->isShowRulers());
}

PageOptionsDialog::~PageOptionsDialog()
{
}

void PageOptionsDialog::apply(QWidget* page)
{
  if (page != this)
    return;

  m_pView->doc()->setUnits(units->unit());
  m_pView->togglePageBorders(showBorders->isChecked());
  m_pView->togglePageMargins(showMargins->isChecked());
  m_pView->toggleShowRulers(showRules->isChecked());
}

#include "pageoptionsdialog.moc"
