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
	init();
}

KexiDataTable::KexiDataTable(KexiMainWindow *win, const QString &caption, KexiDB::Cursor *cursor)
 : KexiDialogBase(win, caption)
{
	m_view = new KexiDataTableView(this, "view", cursor);
	init();
}

void KexiDataTable::init()
{
	QVBoxLayout *box = new QVBoxLayout(this);
	box->addWidget(m_view);
	setMinimumSize(m_view->minimumSizeHint().width(),m_view->minimumSizeHint().height());
	resize(m_view->sizeHint());
//js	m_view->show();
	setFocusProxy(m_view);
	m_view->setFocus();
	setIcon(SmallIcon("table"));
	
	initActions();
//js already done in keximainwindow:	registerDialog();
}


KexiDataTable::~KexiDataTable()
{
}

void
KexiDataTable::initActions()
{
	kdDebug()<<"INIT ACTIONS***********************************************************************"<<endl;
	new KAction(i18n("Filter"), "filter", 0, this, SLOT(filter()), actionCollection(), "tablepart_filter");
	setXMLFile("kexidatatableui.rc");

}

void
KexiDataTable::setData(KexiDB::Cursor *c)
{
	m_view->setData(c);
}

void KexiDataTable::filter()
{
}

QWidget* KexiDataTable::mainWidget() 
{ return m_view; }

QSize KexiDataTable::minimumSizeHint() const
{
//	QWidget*const w= (QWidget*const)mainWidget();
	return m_view->minimumSizeHint();
//	return mainWidget() ? mainWidget()->minimumSizeHint() : KMdiChildView::minimumSizeHint();
}

QSize KexiDataTable::sizeHint() const
{
	return m_view->sizeHint();
}

#include "kexidatatable.moc"

