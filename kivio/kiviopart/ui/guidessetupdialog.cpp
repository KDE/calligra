#include "guidessetupdialog.h"

#include "kivio_view.h"
#include "kivio_guidelines.h"

#include <qcheckbox.h>
#include <kiconloader.h>

GuidesListViewItem::GuidesListViewItem(QListView* parent, KivioGuideLineData* gd, bool isTwoColumn)
: QListViewItem(parent), d(gd), twoColumn(isTwoColumn)
{
  setPixmap(0, BarIcon(d->orientation() == Qt::Vertical ? "guides_vertical":"guides_horizontal"));
  QString s = QString::number(d->position(),'f',3);
  if (twoColumn) {
    setText(1,d->orientation() == Qt::Vertical ? s:QString::null);
    setText(2,d->orientation() == Qt::Vertical ? QString::null:s);
  } else {
    setText(1,s);
  }
  setSelected(d->isSelected());
}

GuidesListViewItem::~GuidesListViewItem()
{
}

void GuidesListViewItem::setUnit(int u)
{
  QString s = QString::number(cvtPtToUnit(u,d->position()),'f',3);
  if (twoColumn) {
    setText(1,d->orientation() == Qt::Vertical ? s:QString::null);
    setText(2,d->orientation() == Qt::Vertical ? QString::null:s);
  } else {
    setText(1,s);
  }
}
/****************************************************************************************/
GuidesSetupDialog::GuidesSetupDialog(KivioView* view, QWidget* parent, const char* name)
: GuidesSetupDialogBase(parent, name)
{
  guidesShow->setChecked(view->isShowGuides());
  guidesSnap->setChecked(view->isSnapGuides());

  connect(guidesShow,SIGNAL(toggled(bool)),view,SLOT(toggleShowGuides(bool)));
  connect(guidesSnap,SIGNAL(toggled(bool)),view,SLOT(toggleSnapGuides(bool)));
}

GuidesSetupDialog::~GuidesSetupDialog()
{
}
