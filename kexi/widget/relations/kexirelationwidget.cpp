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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexirelationwidget.h"

#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qtimer.h>

#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>

#include <kexidb/connection.h>

#include <kexiproject.h>
#include <keximainwindow.h>
#include "kexirelationview.h"
#include "kexirelationviewtable.h"
#include "kexirelationviewconnection.h"

//#define TESTING_KexiRelationDialog

KexiRelationWidget::KexiRelationWidget(KexiMainWindow *win, QWidget *parent, 
	const char *name)
	: QWidget(parent, name)
	, KexiActionProxy(this)
	, m_win(win)
{
	m_conn = m_win->project()->dbConnection();

	QHBoxLayout *hlyr = new QHBoxLayout(0);
	QGridLayout *g = new QGridLayout(this);
	g->addLayout( hlyr, 0, 0 );

	m_tableCombo = new QComboBox(this);
	hlyr->addWidget(m_tableCombo);
	m_tableCombo->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
	m_tableCombo->insertStringList(m_conn->tableNames());
	QStringList tmp=m_conn->tableNames();

	QPushButton *btnAdd = new QPushButton(i18n("&Add"), this);
	hlyr->addWidget(btnAdd);
	hlyr->addStretch(1);
	connect(btnAdd, SIGNAL(clicked()), this, SLOT(slotAddTable()));

	m_relationView = new KexiRelationView(this, m_conn);
	g->addWidget(m_relationView, 1, 0);
	m_relationView->setFocus();

	//actions
	m_tableQueryPopup = new KPopupMenu(this, "m_popup");
	m_tableQueryPopup->insertTitle(i18n("Table"));
	m_connectionPopup = new KPopupMenu(this, "m_connectionPopup");
	m_connectionPopup->insertTitle(i18n("Relation"));
	m_areaPopup = new KPopupMenu(this, "m_areaPopup");
	
	m_openSelectedTableAction = new KAction(i18n("&Open Table"), SmallIcon("fileopen"), KShortcut(),
		this, SLOT(openSelectedTable()), this, "relationsview_openTable");
	m_openSelectedTableAction->plug( m_tableQueryPopup );
	m_designSelectedTableAction = new KAction(i18n("&Design Table"), SmallIcon("edit"), KShortcut(),
		this, SLOT(designSelectedTable()), this, "relationsview_designTable");
	m_designSelectedTableAction->plug( m_tableQueryPopup );
	m_tableQueryPopup->insertSeparator();

	KAction* hide_action = plugSharedAction("edit_delete", i18n("&Hide table"), m_tableQueryPopup);
	hide_action->setIconSet(QIconSet());

	plugSharedAction("edit_delete",m_connectionPopup);
	plugSharedAction("edit_delete",this, SLOT(removeSelectedObject()));

	connect(m_relationView, SIGNAL(tableViewGotFocus()),
		this, SLOT(tableViewGotFocus()));
	connect(m_relationView, SIGNAL(tableContextMenuRequest( const QPoint& )),
		this, SLOT(tableContextMenuRequest( const QPoint& )));
	connect(m_relationView, SIGNAL(connectionContextMenuRequest( const QPoint& )),
		this, SLOT(connectionContextMenuRequest( const QPoint& )));

#if 0
	if(!embedd)
	{
		setContextHelp(i18n("Relations"), i18n("To create a relation simply drag the source field onto the targetfield. "
			"An arrowhead is used to show which table is the parent (master) and which table is the child (slave) in the relation."));
	}
#endif
//	else
//js: while embedding means read-only?		m_relationView->setReadOnly(true);

#ifdef TESTING_KexiRelationWidget
	for (int i=0;i<(int)m_db->tableNames().count();i++)
		QTimer::singleShot(100,this,SLOT(slotAddTable()));
#endif
}

KexiRelationWidget::~KexiRelationWidget()
{
}

void
KexiRelationWidget::slotAddTable()
{
	if (m_tableCombo->currentItem()!=-1) //(m_tableCombo->count() > 0)
	{
		QString tname = m_tableCombo->text(m_tableCombo->currentItem());
		KexiDB::TableSchema *t = m_conn->tableSchema(tname);
		if (t)
		{
			m_relationView->addTable(t);
			kdDebug() << "KexiRelationWidget::slotAddTable(): adding table " << tname << endl;
		}

		int oi=m_tableCombo->currentItem();
		kdDebug()<<"KexiRelationWidget::slotAddTable(): removing a table from the combo box"<<endl;
		m_tableCombo->removeItem(m_tableCombo->currentItem());
		if (m_tableCombo->count()>0)
		{
			if (oi>=m_tableCombo->count()) oi=m_tableCombo->count()-1;
			m_tableCombo->setCurrentItem(oi);
		}
	}
}


void
KexiRelationWidget::chooseTable(QString t)
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

#if 0//js
void
KexiRelationWidget::keyPressEvent(QKeyEvent *ev)
{
	kdDebug() << "KexiRelationWidget::keyPressEvent()" << endl;
//	m_relationView->removeSelected();
}
#endif


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

void KexiRelationWidget::tableContextMenuRequest(const QPoint& pos)
{
//	if (m_focusedTableView != sender())
//		return;
	kdDebug() << "HEADER CTXT MENU!" <<endl;
	invalidateActions();
	executePopup( pos );
//	m_tableQueryPopup->exec(pos);
}

void KexiRelationWidget::connectionContextMenuRequest(const QPoint& pos)
{
	invalidateActions();
	executePopup( pos );
//	m_connectionPopup->exec(pos);
}

void KexiRelationWidget::emptyAreaContextMenuRequest( const QPoint& pos )
{
	invalidateActions();
	//TODO
}

//! Invalidates all actions availability
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
	if (!m_relationView->focusedTableView() || !m_relationView->focusedTableView()->table())
		return;
	m_win->openObject("kexi/table", m_relationView->focusedTableView()->table()->name(), false/*open*/);
}

void KexiRelationWidget::designSelectedTable()
{
	if (!m_relationView->focusedTableView() || !m_relationView->focusedTableView()->table())
		return;
	m_win->openObject("kexi/table", m_relationView->focusedTableView()->table()->name(), true/*design*/);
}

#include "kexirelationwidget.moc"
