#include "viewitemrenamedialog.h"

#include <klocale.h>
#include <qlabel.h>
#include <qlineedit.h>

ViewItemRenameDialog::ViewItemRenameDialog( QWidget* parent,  const char* name)
: EnterNameDialogBase(parent, name, true)
{
  setCaption(i18n("Rename View Item"));
  label->setText(i18n("View item name:"));
  edit->setFocus();
}

ViewItemRenameDialog::~ViewItemRenameDialog()
{
}

void ViewItemRenameDialog::slotOk()
{
  accept();
}

void ViewItemRenameDialog::setText(const QString& text)
{
  edit->setText(text);
  edit->selectAll();
}

QString ViewItemRenameDialog::text()
{
  return edit->text();
}

