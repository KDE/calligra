#include "kiviooptionsdialog.h"

#include "kivio_view.h"

#include "guides1page.h"
#include "guides2page.h"
#include "guidessetupdialog.h"
#include "gridsetupdialog.h"
#include "gridsetupdialog.h"
#include "pagesetupdialog.h"
#include "pageoptionsdialog.h"
#include "stencilsbaroptionsdialog.h"

#include <kdebug.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qwidgetstack.h>

KivioOptionsDialog::KivioOptionsDialog(KivioView* view, Pages startPage, QWidget* parent, const char* name)
: KivioOptionsDialogBase(parent, name, true), m_pView(view)
{
  list->header()->hide();
  list->header()->removeLabel(1);
  list->header()->removeLabel(1);
  list->setSorting(-1);
  QListViewItemIterator it(list);
  // iterate through all items of the listview
  for ( ; it.current(); ++it ) {
    if ( it.current()->text(1).toInt() == startPage) {
      list->setCurrentItem(it.current());
      break;
    }
  }
}

KivioOptionsDialog::~KivioOptionsDialog()
{
}

void KivioOptionsDialog::apply()
{
  emit sig_apply(stack->visibleWidget());
}

void KivioOptionsDialog::slotCurrentChanged(QListViewItem* i)
{
  QString pname = i->text(1);
  QWidget* page = 0;

  page = (QWidget*)stack->child(pname.ascii());
  int id = pname.toInt();

  if (!page) {
    if (id == PageOption) {
      page = new PageOptionsDialog(m_pView, page, pname.ascii());
    }
    else if (id == PageSize) {
      page = new PageSetupDialog(m_pView, page, pname.ascii());
    }
    else if (id == PageGrid) {
      page = new GridSetupDialog(m_pView, page, pname.ascii());
    }
    else if (id == Guides) {
      page = new GuidesSetupDialog(m_pView, page, pname.ascii());
    }
    else if (id == GuidesAll) {
      page = new GuidesTwoPositionPage(m_pView, page, pname.ascii());
    }
    else if (id == GuidesHorizontal) {
      page = new GuidesOnePositionPage(Horizontal, m_pView, page, pname.ascii());
    }
    else if (id == GuidesVertical) {
      page = new GuidesOnePositionPage(Vertical, m_pView, page, pname.ascii());
    }
    else if (id == StencilsBar) {
      page = new StencilsBarOptionsDialog(m_pView, page, pname.ascii());
    }
    else
        kdDebug()<<"unknown id value\n";

    if (page) {
      connect(this, SIGNAL(sig_apply(QWidget*)), page, SLOT(apply(QWidget*)));
      stack->addWidget(page,0);
    }

  }

  if (page) {
    stack->raiseWidget(page);
    label->setText(i->text(2));
  }
}

void KivioOptionsDialog::slotOk()
{
  apply();
  accept();
}

#include "kiviooptionsdialog.moc"
