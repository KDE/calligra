#include "export_page_dialog.h"

#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include <klocale.h>

#include "export_page_dialog_base.h"
#include "kivio_config.h"

ExportPageDialog::ExportPageDialog( QWidget *parent, const char *name )
   : KDialogBase(parent, name, true, i18n("Export Page"), Ok|Cancel, Ok)
{
  m_view = new ExportPageDialogBase(this);
  setMainWidget(m_view);
  
  m_view->m_checkCrop->setChecked(Kivio::Config::crop());
  m_view->m_radioFullPage->setChecked(!Kivio::Config::selectedStencils());
  m_view->m_radioSelectedStencils->setChecked(Kivio::Config::selectedStencils());
  m_view->m_spinBorder->setValue(Kivio::Config::border());
  m_view->m_spinQuality->setValue(Kivio::Config::quality());
}

int ExportPageDialog::quality()
{
   return m_view->m_spinQuality->value();
}

int ExportPageDialog::border()
{
   return m_view->m_spinBorder->value();
}

bool ExportPageDialog::crop()
{
   return m_view->m_checkCrop->isChecked();
}

bool ExportPageDialog::fullPage()
{
   return m_view->m_radioFullPage->isChecked();
}

bool ExportPageDialog::selectedStencils()
{
   return m_view->m_radioSelectedStencils->isChecked();
}

void ExportPageDialog::slotOk()
{
  Kivio::Config::setCrop(m_view->m_checkCrop->isChecked());
  Kivio::Config::setSelectedStencils(m_view->m_radioSelectedStencils->isChecked());
  Kivio::Config::setBorder(m_view->m_spinBorder->value());
  Kivio::Config::setQuality(m_view->m_spinQuality->value());
  
  accept();
}

#include "export_page_dialog.moc"
