#include <qlayout.h>
#include <qlabel.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kaction.h>

#include <kexidb/cursor.h>

#include "kexidatatableview.h"
#include "kexidatatable.h"

KexiDataTable::KexiDataTable(KexiMainWindow *win, const QString &caption)
 : KexiDialogBase(win, caption)
{
	m_view = new KexiDataTableView(this, "view");
	QVBoxLayout *box = new QVBoxLayout(this);
	box->addWidget(m_view);
	m_view->show();

	setIcon(SmallIcon("table"));
}

KexiDataTable::KexiDataTable(KexiMainWindow *win, const QString &caption, KexiDB::Cursor *cursor)
 : KexiDialogBase(win, caption)
{
	m_view = new KexiDataTableView(this, "view", cursor);
	QVBoxLayout *box = new QVBoxLayout(this);
	box->addWidget(m_view);
	m_view->show();
	m_view->setFocus();
	setIcon(SmallIcon("table"));

	initActions();
}

void
KexiDataTable::initActions()
{
	setXMLFile("kexitablepartui.rc");

	new KAction(i18n("Filter"), "filter", 0, this, SLOT(filter()), actionCollection(), "filter");
}

void
KexiDataTable::setData(KexiDB::Cursor *c)
{
	m_view->setData(c);
}

KexiDataTable::~KexiDataTable()
{
}

#include "kexidatatable.moc"

