#include "viewitemrenamedialog.h"

#include <klocale.h>
#include <qlabel.h>
#include <qlineedit.h>

ViewItemRenameDialog::ViewItemRenameDialog(const QString& caption, const QString& prompt, QWidget* parent,  const char* name)
: EnterNameDialogBase(parent, name, true)
{
  setCaption(caption);
  label->setText(prompt);
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

