#include "export_page_dialog.h"

#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include <klocale.h>

#include "export_page_dialog_base.h"

ExportPageDialog::ExportPageDialog( QWidget *parent, const char *name )
   : KDialogBase(parent, name, true, i18n("Export Page"), Ok|Cancel, Ok)
{
  m_view = new ExportPageDialogBase(this);
  setMainWidget(m_view);
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

#include "export_page_dialog.moc"
