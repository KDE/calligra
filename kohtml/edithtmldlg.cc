
#include "edithtmldlg.h"
#include "edithtmldlg.moc"

#include <qlayout.h>
#include <qpushbutton.h>

#include <kapp.h>

HTMLEditDlg::HTMLEditDlg(QString data)
:QDialog(0L, "htmlEditDlg", true)
{
  setCaption(i18n("KoHTML: HTML Editor"));
  
  QVBoxLayout *layout = new QVBoxLayout(this, 4);
  
  e = new QMultiLineEdit(this, "multiLineEdit");
  e->setReadOnly(false);
  layout->addWidget(e, 100);
  
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  layout->addLayout(buttonLayout);
  
  QPushButton *ok = new QPushButton("Ok", this);
  ok->setMinimumSize( ok->sizeHint() );
  ok->adjustSize();
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  buttonLayout->addWidget(ok);
  
  QPushButton *cancel = new QPushButton("Cancel", this);
  cancel->setMinimumSize( cancel->sizeHint() );
  cancel->adjustSize();
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));
  buttonLayout->addWidget(cancel);

  e->setText(data);
  
  layout->activate();
}

HTMLEditDlg::~HTMLEditDlg()
{
  if (e) delete e;
}

QString HTMLEditDlg::getText()
{
  return e->text();
}
