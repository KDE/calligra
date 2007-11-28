/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include "KexiRelationsView.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>

#include <kcombobox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <KMenu>
#include <KDialog>

#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexiutils/utils.h>

#include <kexiproject.h>
#include <KexiMainWindowIface.h>
#include "KexiRelationsScrollArea.h"
#include "KexiRelationsView.h"
#include "KexiRelationsConnection.h"

//! @internal
class KexiRelationsView::Private
{
	public:
		Private()
		 : conn( KexiMainWindowIface::global()->project()->dbConnection() )
		{
		}

		KComboBox *tableCombo;
		KPushButton *btnAdd;
		KexiRelationsScrollArea *scrollArea;
		KexiDB::Connection *conn;

		KMenu *tableQueryPopup //!< over table/query
			, *connectionPopup //!< over connection
			, *areaPopup; //!< over outer area
		KAction *openSelectedTableAction, *designSelectedTableAction, 
			*appendSelectedFieldAction, *appendSelectedFieldsAction, *hideTableAction;
};

//---------------

KexiRelationsView::KexiRelationsView(QWidget *parent)
 : KexiView(parent)
 , d( new Private )
{
	QWidget *mainWidget = new QWidget(this);
	QGridLayout *g = new QGridLayout(mainWidget);
	g->setContentsMargins(0, 0, 0, 0);
	g->setSpacing(KDialog::spacingHint());

	QWidget *horWidget = new QWidget(mainWidget);
	QHBoxLayout *hlyr = new QHBoxLayout(horWidget);
	hlyr->setContentsMargins(0, 0, 0, 0);
	g->addWidget( horWidget, 0, 0 );

	d->tableCombo = new KComboBox(horWidget);
	d->tableCombo->setObjectName("tables_combo");
	d->tableCombo->setMinimumWidth(QFontMetrics(font()).width("w")*20);
	d->tableCombo->setInsertPolicy(QComboBox::NoInsert);
	d->tableCombo->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
	QLabel *lbl = new QLabel(i18n("Table:"), horWidget);
	lbl->setBuddy(d->tableCombo);
	lbl->setIndent(3);
	hlyr->addWidget(lbl);
	hlyr->addWidget(d->tableCombo);
	fillTablesCombo();

	d->btnAdd = new KPushButton(i18nc("Insert table/query into relations view", "&Insert"), horWidget);
	hlyr->addWidget(d->btnAdd);
	hlyr->addStretch(1);
	connect(d->btnAdd, SIGNAL(clicked()), this, SLOT(slotAddTable()));

	d->scrollArea = new KexiRelationsScrollArea(mainWidget);
	d->scrollArea->setObjectName("scroll_area");
	setViewWidget(mainWidget, false/* no focus proxy */);
	setFocusProxy(d->scrollArea);
	g->addWidget(d->scrollArea, 1, 0);

	//actions
	d->tableQueryPopup = new KMenu(this);
	d->tableQueryPopup->setObjectName("tableQueryPopup");
	connect(d->tableQueryPopup, SIGNAL(aboutToShow()), this, SLOT(aboutToShowPopupMenu()));

	d->hideTableAction = plugSharedAction("edit_delete", i18n("&Hide Table"), d->tableQueryPopup);
	if (d->hideTableAction)
		d->hideTableAction->setIcon(KIcon());

	d->connectionPopup = new KMenu(this);
	d->connectionPopup->setObjectName("connectionPopup");
	connect(d->connectionPopup, SIGNAL(aboutToShow()), this, SLOT(aboutToShowPopupMenu()));

//! @todo areaPopup
	d->areaPopup = new KMenu(this);
	d->areaPopup->setObjectName("areaPopup");

	d->appendSelectedFieldAction = new KAction(KIcon("add_field"), i18n("&Append Field"), this);
	d->appendSelectedFieldAction->setObjectName("relationsview_appendField");
	connect(d->appendSelectedFieldAction, SIGNAL(triggered()),
		this, SLOT(appendSelectedFields()));

	d->appendSelectedFieldsAction = new KAction(KIcon("add_field"), i18n("&Append Fields"), this);
	d->appendSelectedFieldsAction->setObjectName("relationsview_appendFields");
	connect(d->appendSelectedFieldsAction, SIGNAL(triggered()),
		this, SLOT(appendSelectedFields()));

	d->openSelectedTableAction = new KAction(KIcon("document-open"), i18n("&Open Table"), this);
	d->openSelectedTableAction->setObjectName("relationsview_openTable");
	connect(d->openSelectedTableAction, SIGNAL(triggered()),
		this, SLOT(openSelectedTable()));
		
	d->designSelectedTableAction = new KAction(KIcon("object-edit"), i18n("&Design Table"), this);
	connect(d->designSelectedTableAction, SIGNAL(triggered()),
		this, SLOT(designSelectedTable()));
	d->designSelectedTableAction->setObjectName("relationsview_designTable");

	plugSharedAction("edit_delete", this, SLOT(removeSelectedObject()));

	connect(d->scrollArea, SIGNAL(tableViewGotFocus()),
		this, SLOT(tableViewGotFocus()));
	connect(d->scrollArea, SIGNAL(connectionViewGotFocus()),
		this, SLOT(connectionViewGotFocus()));
	connect(d->scrollArea, SIGNAL(emptyAreaGotFocus()),
		this, SLOT(emptyAreaGotFocus()));
	connect(d->scrollArea, SIGNAL(tableContextMenuRequest( const QPoint& )),
		this, SLOT(tableContextMenuRequest( const QPoint& )));
	connect(d->scrollArea, SIGNAL(connectionContextMenuRequest( const QPoint& )),
		this, SLOT(connectionContextMenuRequest( const QPoint& )));
	connect(d->scrollArea, SIGNAL(tableHidden(KexiDB::TableSchema&)),
		this, SLOT(slotTableHidden(KexiDB::TableSchema&)));
	connect(d->scrollArea, SIGNAL(tablePositionChanged(KexiRelationsTableContainer*)),
		this, SIGNAL(tablePositionChanged(KexiRelationsTableContainer*)));
	connect(d->scrollArea, SIGNAL(aboutConnectionRemove(KexiRelationsConnection*)),
		this, SIGNAL(aboutConnectionRemove(KexiRelationsConnection*)));

#if 0
	if(!embedd)
	{
/*todo		setContextHelp(i18n("Relations"), i18n("To create a relationship simply drag the source field onto the target field. "
			"An arrowhead is used to show which table is the parent (master) and which table is the child (slave) in the relationship."));*/
	}
#endif
//	else
//js: while embedding means read-only?		d->scrollArea->setReadOnly(true);

#ifdef TESTING_KexiRelationWidget
	for (int i=0;i<(int)d->db->tableNames().count();i++)
		QTimer::singleShot(100,this,SLOT(slotAddTable()));
#endif

	invalidateActions();
}

KexiRelationsView::~KexiRelationsView()
{
	delete d;
}

TablesHash* KexiRelationsView::tables() const
{
	return d->scrollArea->tables();
}

KexiRelationsTableContainer* KexiRelationsView::table(const QString& name) const
{
	return d->scrollArea->tables()->value( name );
}

const ConnectionSet* KexiRelationsView::connections() const
{ 
	return d->scrollArea->connections();
}

void
KexiRelationsView::slotAddTable()
{
	if (d->tableCombo->currentIndex()==-1)
		return;
	const QString tname = d->tableCombo->itemText(d->tableCombo->currentIndex());
	KexiDB::TableSchema *t = d->conn->tableSchema(tname);
	addTable(t);
}

void
KexiRelationsView::addTable(KexiDB::TableSchema *t, const QRect &rect)
{
	if (!t)
		return;
	if (!d->scrollArea->tableContainer(t)) {
		KexiRelationsTableContainer *c = d->scrollArea->addTableContainer(t, rect);
		kDebug() << "KexiRelationsView::slotAddTable(): adding table " << t->name() << endl;
		if (!c)
			return;
		connect(c, SIGNAL(fieldsDoubleClicked(KexiDB::TableOrQuerySchema&,const QStringList&)),
			this, SIGNAL(appendFields(KexiDB::TableOrQuerySchema&,const QStringList&)));
	}

	const QString tname = t->name().toLower();
	const int count = d->tableCombo->count();
	int i = 0;
	for (; i < count; i++ ) {
		if (d->tableCombo->itemText(i).toLower() == tname )
			break;
	}
	if (i<count) {
		int oi = d->tableCombo->currentIndex();
		kDebug()<<"KexiRelationsView::slotAddTable(): removing a table from the combo box"<<endl;
		d->tableCombo->removeItem(i);
		if (d->tableCombo->count()>0) {
			if (oi>=d->tableCombo->count()) {
				oi=d->tableCombo->count()-1;
			}
			d->tableCombo->setCurrentIndex(oi);
		}
		else {
			d->tableCombo->setEnabled(false);
			d->btnAdd->setEnabled(false);
		}
	}
	emit tableAdded(*t);
}

void
KexiRelationsView::addConnection(const SourceConnection& conn)
{
	d->scrollArea->addConnection(conn);
}

void
KexiRelationsView::addTable(const QString& t)
{
	for(int i=0; i < d->tableCombo->count(); i++)
	{
		if(d->tableCombo->itemText(i) == t)
		{
			d->tableCombo->setCurrentIndex(i);
			slotAddTable();
		}
	}
}

void KexiRelationsView::tableViewGotFocus()
{
//	if (d->scrollArea->focusedTableContainer == sender())
//		return;
//	kDebug() << "GOT FOCUS!" <<endl;
//	clearSelection();
//	if (d->focusedTableContainer)
//		d->focusedTableContainer->unsetFocus();
//	d->focusedTableContainer = (KexiRelationsTableContainer*)sender();
	invalidateActions();
}

void KexiRelationsView::connectionViewGotFocus()
{
	invalidateActions();
}

void KexiRelationsView::emptyAreaGotFocus()
{
	invalidateActions();
}

void KexiRelationsView::tableContextMenuRequest(const QPoint& pos)
{
	invalidateActions();
	executePopup( pos );
}

void KexiRelationsView::connectionContextMenuRequest(const QPoint& pos)
{
	invalidateActions();
	executePopup( pos );
}

void KexiRelationsView::emptyAreaContextMenuRequest( const QPoint& /*pos*/ )
{
	invalidateActions();
	//TODO
}

void KexiRelationsView::invalidateActions()
{
	setAvailable("edit_delete", d->scrollArea->selectedConnection() || d->scrollArea->focusedTableContainer());
}

void KexiRelationsView::executePopup( QPoint pos )
{
	if (pos==QPoint(-1,-1)) {
		pos = mapToGlobal( 
			d->scrollArea->focusedTableContainer() ? d->scrollArea->focusedTableContainer()->pos() + d->scrollArea->focusedTableContainer()->rect().center() : rect().center() );
	}
	if (d->scrollArea->focusedTableContainer())
		d->tableQueryPopup->exec(pos);
	else if (d->scrollArea->selectedConnection())
		d->connectionPopup->exec(pos);
}

void KexiRelationsView::removeSelectedObject()
{
	d->scrollArea->removeSelectedObject();
}

void KexiRelationsView::appendSelectedFields()
{
	KexiRelationsTableContainer* currentTableContainer = d->scrollArea->focusedTableContainer();
	if (!currentTableContainer)
		return;
	emit appendFields(*currentTableContainer->schema(), currentTableContainer->selectedFieldNames());
}

void KexiRelationsView::openSelectedTable()
{
/*! @todo what about query? */
	if (!d->scrollArea->focusedTableContainer() || !d->scrollArea->focusedTableContainer()->schema()->table())
		return;
	bool openingCancelled;
	KexiMainWindowIface::global()->openObject(
		"kexi/table", d->scrollArea->focusedTableContainer()->schema()->name(), 
		Kexi::DataViewMode, openingCancelled);
}

void KexiRelationsView::designSelectedTable()
{
/*! @todo what about query? */
	if (!d->scrollArea->focusedTableContainer() || !d->scrollArea->focusedTableContainer()->schema()->table())
		return;
	bool openingCancelled;
	KexiMainWindowIface::global()->openObject(
		"kexi/table", d->scrollArea->focusedTableContainer()->schema()->name(), 
		Kexi::DesignViewMode, openingCancelled);
}

QSize KexiRelationsView::sizeHint() const
{
	return d->scrollArea->sizeHint();
}

void KexiRelationsView::slotTableHidden(KexiDB::TableSchema &table)
{
	const QString &t = table.name().toLower();
	int i;
	for (i=0; i<d->tableCombo->count() && t > d->tableCombo->itemText(i).toLower(); i++)
		;
	d->tableCombo->insertItem(i, table.name());
	if (!d->tableCombo->isEnabled()) {
		d->tableCombo->setCurrentIndex(0);
		d->tableCombo->setEnabled(true);
		d->btnAdd->setEnabled(true);
	}

	emit tableHidden(table);
}

void KexiRelationsView::aboutToShowPopupMenu()
{
	KexiRelationsTableContainer* currentTableContainer = d->scrollArea->focusedTableContainer();
	if (currentTableContainer /*&& currentTableContainer->schema()->table()*/) {
/*! @todo what about query? */
		d->tableQueryPopup->clear();
		d->tableQueryPopup->addTitle(KIcon("table"),
			QString(d->scrollArea->focusedTableContainer()->schema()->name()) + " : " + i18n("Table"));
		QStringList selectedFieldNames( currentTableContainer->selectedFieldNames() );
		if (currentTableContainer && !selectedFieldNames.isEmpty()) {
			if (selectedFieldNames.count() > 1 || selectedFieldNames.first()=="*") //multiple
				d->tableQueryPopup->addAction(d->appendSelectedFieldsAction);
			else
				d->tableQueryPopup->addAction(d->appendSelectedFieldAction);
			d->tableQueryPopup->addSeparator();
		}
		d->tableQueryPopup->addAction(d->openSelectedTableAction);
		d->tableQueryPopup->addAction(d->designSelectedTableAction);
		d->tableQueryPopup->addSeparator();
		d->tableQueryPopup->addAction(d->hideTableAction);
	}
	else if (d->scrollArea->selectedConnection()) {
		unplugSharedAction("edit_delete", d->connectionPopup);
		d->connectionPopup->clear();
		d->connectionPopup->addTitle(KIcon(), 
			 d->scrollArea->selectedConnection()->toString() + " : " + i18n("Relationship") );
		plugSharedAction("edit_delete", d->connectionPopup);
	}
}

void 
KexiRelationsView::clear()
{
	d->scrollArea->clear();
	fillTablesCombo();
}

/*! Removes all coonections from the view. */
void KexiRelationsView::removeAllConnections()
{
	d->scrollArea->removeAllConnections();
}

void 
KexiRelationsView::fillTablesCombo()
{
	d->tableCombo->clear();
	QStringList tmp = d->conn->tableNames();
	tmp.sort();
	d->tableCombo->addItems(tmp);
}

void
KexiRelationsView::objectCreated(const QString &mime, const QString& name)
{
	if (mime=="kexi/table" || mime=="kexi/query") {
//! @todo query?
		const int count = d->tableCombo->count();
		QString strName(name);
		int i = 0;
		for (; i<count && d->tableCombo->itemText(i)<=strName; i++)
			;
		d->tableCombo->insertItem(i, name);
	}
}

void
KexiRelationsView::objectDeleted(const QString &mime, const QString& name)
{
	if (mime=="kexi/table" || mime=="kexi/query") {
		for (int i=0; i<d->tableCombo->count(); i++) {
//! @todo query?
			if (d->tableCombo->itemText(i) == name) {
				d->tableCombo->removeItem(i);
				if (d->tableCombo->currentIndex()==i) {
					if (i==(d->tableCombo->count()-1))
						d->tableCombo->setCurrentIndex(i-1);
					else
						d->tableCombo->setCurrentIndex(i);
				}
				break;
			}
		}
	}
}

void
KexiRelationsView::objectRenamed(const QString &mime, const QString& name, 
	const QString& newName)
{
	if (mime=="kexi/table" || mime=="kexi/query") {
		const int count = d->tableCombo->count();
		for (int i=0; i<count; i++) {
//! @todo query?
			if (d->tableCombo->itemText(i) == name) {
				d->tableCombo->removeItem(i);
				int j = 0;
				for (; j<count && d->tableCombo->itemText(j) <= newName; j++)
					;
				d->tableCombo->insertItem(j, newName);
				break;
			}
		}
	}
}

void
KexiRelationsView::hideAllTablesExcept( KexiDB::TableSchema::List* tables )
{
	d->scrollArea->hideAllTablesExcept(tables);
}

#include "KexiRelationsView.moc"
