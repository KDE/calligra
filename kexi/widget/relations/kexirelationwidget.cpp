/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexirelationwidget.h"

#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtimer.h>

#include <kcombobox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kpushbutton.h>

#include <kexidb/connection.h>
#include <kexidb/utils.h>

#include <kexiproject.h>
#include <keximainwindow.h>
#include "kexirelationview.h"
#include "kexirelationviewtable.h"
#include "kexirelationviewconnection.h"

KexiRelationWidget::KexiRelationWidget(KexiMainWindow *win, QWidget *parent, 
	const char *name)
	: KexiViewBase(win, parent, name)
	, m_win(win)
{
	m_conn = m_win->project()->dbConnection();

	QHBoxLayout *hlyr = new QHBoxLayout(0);
	QGridLayout *g = new QGridLayout(this);
	g->addLayout( hlyr, 0, 0 );

	m_tableCombo = new KComboBox(this, "tables_combo");
	m_tableCombo->setMinimumWidth(QFontMetrics(font()).width("w")*20);
	QLabel *lbl = new QLabel(m_tableCombo, i18n("Table")+": ", this);
	lbl->setIndent(3);
	m_tableCombo->setInsertionPolicy(QComboBox::NoInsertion);
	hlyr->addWidget(lbl);
	hlyr->addWidget(m_tableCombo);
	m_tableCombo->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
	fillTablesCombo();

	m_btnAdd = new KPushButton(i18n("&Add"), this);
	hlyr->addWidget(m_btnAdd);
	hlyr->addStretch(1);
	connect(m_btnAdd, SIGNAL(clicked()), this, SLOT(slotAddTable()));

	m_relationView = new KexiRelationView(this, "relation_view");
	setViewWidget(m_relationView);
	g->addWidget(m_relationView, 1, 0);
	//m_relationView->setFocus();

	//actions
	m_tableQueryPopup = new KPopupMenu(this, "m_popup");
	m_tableQueryPopupTitleID = m_tableQueryPopup->insertTitle(SmallIcon("table"), "");
	connect(m_tableQueryPopup, SIGNAL(aboutToShow()), this, SLOT(aboutToShowPopupMenu()));

	m_connectionPopup = new KPopupMenu(this, "m_connectionPopup");
	m_connectionPopupTitleID = m_connectionPopup->insertTitle("");
	connect(m_connectionPopup, SIGNAL(aboutToShow()), this, SLOT(aboutToShowPopupMenu()));

	m_areaPopup = new KPopupMenu(this, "m_areaPopup");
	
	m_openSelectedTableAction = new KAction(i18n("&Open Table"), SmallIcon("fileopen"), KShortcut(),
		this, SLOT(openSelectedTable()), this, "relationsview_openTable");
	m_openSelectedTableAction->plug( m_tableQueryPopup );
	m_designSelectedTableAction = new KAction(i18n("&Design Table"), SmallIcon("edit"), KShortcut(),
		this, SLOT(designSelectedTable()), this, "relationsview_designTable");
	m_designSelectedTableAction->plug( m_tableQueryPopup );
	m_tableQueryPopup->insertSeparator();

	KAction* hide_action = plugSharedAction("edit_delete", i18n("&Hide Table"), m_tableQueryPopup);
	hide_action->setIconSet(QIconSet());

	plugSharedAction("edit_delete",m_connectionPopup);
	plugSharedAction("edit_delete",this, SLOT(removeSelectedObject()));

	connect(m_relationView, SIGNAL(tableViewGotFocus()),
		this, SLOT(tableViewGotFocus()));
	connect(m_relationView, SIGNAL(connectionViewGotFocus()),
		this, SLOT(connectionViewGotFocus()));
	connect(m_relationView, SIGNAL(emptyAreaGotFocus()),
		this, SLOT(emptyAreaGotFocus()));
	connect(m_relationView, SIGNAL(tableContextMenuRequest( const QPoint& )),
		this, SLOT(tableContextMenuRequest( const QPoint& )));
	connect(m_relationView, SIGNAL(connectionContextMenuRequest( const QPoint& )),
		this, SLOT(connectionContextMenuRequest( const QPoint& )));
	connect(m_relationView, SIGNAL(tableHidden(KexiDB::TableSchema&)),
		this, SLOT(slotTableHidden(KexiDB::TableSchema&)));
	connect(m_relationView, SIGNAL(tablePositionChanged(KexiRelationViewTableContainer*)),
		this, SIGNAL(tablePositionChanged(KexiRelationViewTableContainer*)));
	connect(m_relationView, SIGNAL(aboutConnectionRemove(KexiRelationViewConnection*)),
		this, SIGNAL(aboutConnectionRemove(KexiRelationViewConnection*)));

#if 0
	if(!embedd)
	{
/*todo		setContextHelp(i18n("Relations"), i18n("To create a relationship simply drag the source field onto the target field. "
			"An arrowhead is used to show which table is the parent (master) and which table is the child (slave) in the relationship."));*/
	}
#endif
//	else
//js: while embedding means read-only?		m_relationView->setReadOnly(true);

#ifdef TESTING_KexiRelationWidget
	for (int i=0;i<(int)m_db->tableNames().count();i++)
		QTimer::singleShot(100,this,SLOT(slotAddTable()));
#endif

	invalidateActions();
}

KexiRelationWidget::~KexiRelationWidget()
{
}

TablesDict* KexiRelationWidget::tables() const
{
	return m_relationView->tables();
}

KexiRelationViewTableContainer* KexiRelationWidget::table(const QString& name) const
{
	return m_relationView->tables()->find( name );
}

const ConnectionList* KexiRelationWidget::connections() const
{ 
	return m_relationView->connections();
}

void
KexiRelationWidget::slotAddTable()
{
	if (m_tableCombo->currentItem()==-1)
		return;
	QString tname = m_tableCombo->text(m_tableCombo->currentItem());
	KexiDB::TableSchema *t = m_conn->tableSchema(tname);
	addTable(t);
}

void
KexiRelationWidget::addTable(KexiDB::TableSchema *t, const QRect &rect)
{
	if (!t)
		return;
	if (!m_relationView->tableContainer(t)) {
		KexiRelationViewTableContainer *c = m_relationView->addTableContainer(t, rect);
		kdDebug() << "KexiRelationWidget::slotAddTable(): adding table " << t->name() << endl;
		if (!c)
			return;
		connect(c->tableView(), SIGNAL(doubleClicked(QListViewItem*,const QPoint&,int)),
			this, SLOT(slotTableFieldDoubleClicked(QListViewItem*,const QPoint&,int)));
	}

	const QString tname = t->name().lower();
	const int count = m_tableCombo->count();
	int i = 0;
	for (; i < count; i++ ) {
		if (m_tableCombo->text(i).lower() == tname )
			break;
	}
	if (i<count) {
		int oi = m_tableCombo->currentItem();
		kdDebug()<<"KexiRelationWidget::slotAddTable(): removing a table from the combo box"<<endl;
		m_tableCombo->removeItem(i);
		if (m_tableCombo->count()>0) {
			if (oi>=m_tableCombo->count()) {
				oi=m_tableCombo->count()-1;
			}
			m_tableCombo->setCurrentItem(oi);
		}
		else {
			m_tableCombo->setEnabled(false);
			m_btnAdd->setEnabled(false);
		}
	}
	emit tableAdded(*t);
}

void
KexiRelationWidget::addConnection(const SourceConnection& conn)
{
	m_relationView->addConnection(conn);
}

void
KexiRelationWidget::addTable(const QString& t)
{
	for(int i=0; i < m_tableCombo->count(); i++)
	{
		if(m_tableCombo->text(i) == t)
		{
			m_tableCombo->setCurrentItem(i);
			slotAddTable();
		}
	}
}

void KexiRelationWidget::tableViewGotFocus()
{
//	if (m_relationView->focusedTableView == sender())
//		return;
//	kdDebug() << "GOT FOCUS!" <<endl;
//	clearSelection();
//	if (m_focusedTableView)
//		m_focusedTableView->unsetFocus();
//	m_focusedTableView = (KexiRelationViewTableContainer*)sender();
	invalidateActions();
}

void KexiRelationWidget::connectionViewGotFocus()
{
	invalidateActions();
}

void KexiRelationWidget::emptyAreaGotFocus()
{
	invalidateActions();
}

void KexiRelationWidget::tableContextMenuRequest(const QPoint& pos)
{
	invalidateActions();
	executePopup( pos );
}

void KexiRelationWidget::connectionContextMenuRequest(const QPoint& pos)
{
	invalidateActions();
	executePopup( pos );
//	m_connectionPopup->exec(pos);
}

void KexiRelationWidget::emptyAreaContextMenuRequest( const QPoint& /*pos*/ )
{
	invalidateActions();
	//TODO
}

void KexiRelationWidget::invalidateActions()
{
	setAvailable("edit_delete", m_relationView->selectedConnection() || m_relationView->focusedTableView());
}

void KexiRelationWidget::executePopup( QPoint pos )
{
	if (pos==QPoint(-1,-1)) {
		pos = mapToGlobal( 
			m_relationView->focusedTableView() ? m_relationView->focusedTableView()->pos() + m_relationView->focusedTableView()->rect().center() : rect().center() );
	}
	if (m_relationView->focusedTableView())
		m_tableQueryPopup->exec(pos);
	else if (m_relationView->selectedConnection())
		m_connectionPopup->exec(pos);
}

void KexiRelationWidget::removeSelectedObject()
{
	m_relationView->removeSelectedObject();
}

void KexiRelationWidget::openSelectedTable()
{
/*! @todo what about query? */
	if (!m_relationView->focusedTableView() || !m_relationView->focusedTableView()->schema()->table())
		return;
	bool openingCancelled;
	m_win->openObject("kexi/table", m_relationView->focusedTableView()->schema()->name(), 
		Kexi::DataViewMode, openingCancelled);
}

void KexiRelationWidget::designSelectedTable()
{
/*! @todo what about query? */
	if (!m_relationView->focusedTableView() || !m_relationView->focusedTableView()->schema()->table())
		return;
	bool openingCancelled;
	m_win->openObject("kexi/table", m_relationView->focusedTableView()->schema()->name(), 
		Kexi::DesignViewMode, openingCancelled);
}

QSize KexiRelationWidget::sizeHint() const
{
	return m_relationView->sizeHint();
}

void KexiRelationWidget::slotTableHidden(KexiDB::TableSchema &table)
{
	const QString &t = table.name().lower();
	int i;
	for (i=0; i<m_tableCombo->count() && t > m_tableCombo->text(i).lower(); i++)
		;
	m_tableCombo->insertItem(table.name(), i);
	if (!m_tableCombo->isEnabled()) {
		m_tableCombo->setCurrentItem(0);
		m_tableCombo->setEnabled(true);
		m_btnAdd->setEnabled(true);
	}

	emit tableHidden(table);
}

void KexiRelationWidget::aboutToShowPopupMenu()
{
/*! @todo what about query? */
	if (m_relationView->focusedTableView() && m_relationView->focusedTableView()->schema()->table()) {
		m_tableQueryPopup->changeTitle(m_tableQueryPopupTitleID, SmallIcon("table"),
			QString(m_relationView->focusedTableView()->schema()->name()) + " : " + i18n("Table"));
	}
	else if (m_relationView->selectedConnection()) {
		m_connectionPopup->changeTitle( m_connectionPopupTitleID, 
			 m_relationView->selectedConnection()->toString() + " : " + i18n("Relationship") );
	}
}

void
KexiRelationWidget::slotTableFieldDoubleClicked(QListViewItem *i,const QPoint&,int)
{
	if (!sender()->isA("KexiRelationViewTable"))
		return;
	const KexiRelationViewTable* t = static_cast<const KexiRelationViewTable*>(sender());
	const QStringList selectedFieldNames( t->selectedFieldNames() );
	if (selectedFieldNames.count()==1)
		emit tableFieldDoubleClicked( t->schema()->table(), selectedFieldNames.first() );
}

void 
KexiRelationWidget::clear()
{
	m_relationView->clear();
	fillTablesCombo();
}

/*! Removes all coonections from the view. */
void KexiRelationWidget::removeAllConnections()
{
	m_relationView->removeAllConnections();
}

void 
KexiRelationWidget::fillTablesCombo()
{
	m_tableCombo->clear();
	QStringList tmp = m_conn->tableNames();
	tmp.sort();
	m_tableCombo->insertStringList(tmp);
}

void
KexiRelationWidget::objectCreated(const QCString &mime, const QCString& name)
{
	if (mime=="kexi/table" || mime=="kexi/query") {
//! @todo query?
		m_tableCombo->insertItem(QString(name));
		m_tableCombo->listBox()->sort();
	}
}

void
KexiRelationWidget::objectDeleted(const QCString &mime, const QCString& name)
{
	if (mime=="kexi/table" || mime=="kexi/query") {
		QString strName(name);
		for (int i=0; i<m_tableCombo->count(); i++) {
//! @todo query?
			if (m_tableCombo->text(i)==strName) {
				m_tableCombo->removeItem(i);
				if (m_tableCombo->currentItem()==i) {
					if (i==(m_tableCombo->count()-1))
						m_tableCombo->setCurrentItem(i-1);
					else
						m_tableCombo->setCurrentItem(i);
				}
				break;
			}
		}
	}
}

void
KexiRelationWidget::objectRenamed(const QCString &mime, const QCString& name, const QCString& newName)
{
	if (mime=="kexi/table" || mime=="kexi/query") {
		QString strName(name);
		for (int i=0; i<m_tableCombo->count(); i++) {
//! @todo query?
			if (m_tableCombo->text(i)==strName) {
				m_tableCombo->changeItem(QString(newName), i);
				m_tableCombo->listBox()->sort();
				break;
			}
		}
	}
}

void
KexiRelationWidget::hideAllTablesExcept( KexiDB::TableSchema::List* tables )
{
	m_relationView->hideAllTablesExcept(tables);
}

#include "kexirelationwidget.moc"
