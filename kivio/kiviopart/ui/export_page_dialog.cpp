#include "export_page_dialog.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>

ExportPageDialog::ExportPageDialog( QWidget *parent, const char *name )
   : ExportPageDialogBase(parent, name, true)
{
   QObject::connect( m_ok, SIGNAL(clicked()),
		     this, SLOT(accept()) );
   QObject::connect( m_cancel, SIGNAL(clicked()),
		     this, SLOT(reject()) );
}

ExportPageDialog::~ExportPageDialog()
{
}

int ExportPageDialog::quality()
{
   return m_spinQuality->value();
}

int ExportPageDialog::border()
{
   return m_spinBorder->value();
}

bool ExportPageDialog::crop()
{
   return m_checkCrop->isChecked();
}

bool ExportPageDialog::fullPage()
{
   return m_radioFullPage->isChecked();
}

bool ExportPageDialog::selectedStencils()
{
   return m_radioSelectedStencils->isChecked();
}
