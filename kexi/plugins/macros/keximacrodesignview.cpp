/* This file is part of the KDE project
   Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "keximacrodesignview.h"

#include <qdom.h>
#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>
#include <kexidb/error.h>

#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipartmanager.h>
#include <core/kexipartinfo.h>

#include <widget/tableview/kexitableview.h>
#include <widget/tableview/kexitableviewdata.h>
#include <widget/tableview/kexitableitem.h>
#include <widget/tableview/kexidataawarepropertyset.h>

#include <koproperty/set.h>
#include <koproperty/property.h>

#include "lib/macro.h"
#include "lib/macroitem.h"
#include "lib/xmlhandler.h"

//! constants used to name columns instead of hardcoding indices
#define COLUMN_ID_ACTION 0
#define COLUMN_ID_COMMENT 1

/**
 * The \a KexiTableView implementation to display a list of actions
 * a \a Macro provides.
 */
class KexiMacroTableView : public KexiTableView
{
	public:
		/**
		* Constructor.
		*
		* \param data The \a KexiTableViewData data-modell which
		* contains the data this \a KexiTableView should display.
		* \param parent The parent widget.
		*/
		KexiMacroTableView(KexiTableViewData* data, QWidget* parent)
			: KexiTableView(data, parent)
		{
		}

		/**
		* Destructor.
		*/
		virtual ~KexiMacroTableView()
		{
		}
};

/**
* \internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroDesignView::Private
{
	public:

		/**
		* The \a KexiMacroTableView used to display the actions
		* a \a Macro has.
		*/
		KexiMacroTableView* tableview;

		/**
		* The \a KexiTableViewData data-model for the
		* \a KexiMacroTableView .
		*/
		KexiTableViewData* tabledata;

		/**
		* The \a KexiDataAwarePropertySet is used to display
		* properties an action provides in the propertyview.
		*/
		KexiDataAwarePropertySet* propertyset;

		/**
		* Constructor.
		*
		* \param m The passed \a KoMacro::Manager instance our
		*        \a manager points to.
		*/
		Private()
			: propertyset(0)
		{
		}

};

KexiMacroDesignView::KexiMacroDesignView(KexiMainWindow *mainwin, QWidget *parent, ::KoMacro::Macro* const macro)
	: KexiMacroView(mainwin, parent, macro, "KexiMacroDesignView")
	, d( new Private() )
{
	// The table's data-model.
	d->tabledata = new KexiTableViewData();
	d->tabledata->setSorting(-1); // disable sorting

	// Add the "Action" column.
	KexiTableViewColumn* actioncol = new KexiTableViewColumn(
		"action", // name/identifier
		KexiDB::Field::Enum, // fieldtype
		KexiDB::Field::NoConstraints, // constraints
		KexiDB::Field::NoOptions, // options
		0, // length
		0, // precision
		QVariant(), // default value
		i18n("Action"), // caption
		QString::null, // description
		0 // width
	);
	d->tabledata->addColumn(actioncol);

	QValueVector<QString> items;
	items.append(""); // empty means no action

	QStringList actionnames = KoMacro::Manager::self()->actionNames();
	QStringList::ConstIterator it, end( actionnames.constEnd() );
	for( it = actionnames.constBegin(); it != end; ++it) {
		KoMacro::Action::Ptr action = KoMacro::Manager::self()->action(*it);
		items.append( action->text() );
	}

	actioncol->field()->setEnumHints(items);

	// Add the "Comment" column.
	d->tabledata->addColumn( new KexiTableViewColumn(
		"comment", // name/identifier
		KexiDB::Field::Text, // fieldtype
		KexiDB::Field::NoConstraints, // constraints
		KexiDB::Field::NoOptions, // options
		0, // length
		0, // precision
		QVariant(), // default value
		i18n("Comment"), // caption
		QString::null, // description
		0 // width
	) );

	connect(d->tabledata, SIGNAL(itemSelected(KexiTableItem*)),
		this, SLOT(itemSelected(KexiTableItem*)));
	connect(d->tabledata, SIGNAL(aboutToChangeCell(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)),
		this, SLOT(beforeCellChanged(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)));
	connect(d->tabledata, SIGNAL(rowUpdated(KexiTableItem*)),
		this, SLOT(rowUpdated(KexiTableItem*)));
	connect(d->tabledata, SIGNAL(aboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)),
		this, SLOT(aboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)));
	connect(d->tabledata, SIGNAL(aboutToDeleteRow(KexiTableItem&,KexiDB::ResultInfo*,bool)),
		this, SLOT(aboutToDeleteRow(KexiTableItem&,KexiDB::ResultInfo*,bool)));

	// Create the tableview.
	QHBoxLayout* layout = new QHBoxLayout(this);
	d->tableview = new KexiMacroTableView(d->tabledata, this);
	d->tableview->setSpreadSheetMode();
	d->tableview->setColumnStretchEnabled( true, COLUMN_ID_COMMENT ); //last column occupies the rest of the area
	layout->addWidget(d->tableview);

	// Create the propertyset.
	d->propertyset = new KexiDataAwarePropertySet(this, d->tableview);
	//connect(d->propertyset, SIGNAL(rowDeleted()), this, SLOT(updateActions()));
	//connect(d->propertyset, SIGNAL(rowInserted()), this, SLOT(updateActions()));
	connect(d->propertyset, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
		this, SLOT(propertyChanged(KoProperty::Set&, KoProperty::Property&)));

	// Everything is ready. So, update the data now.
	updateData();
}

KexiMacroDesignView::~KexiMacroDesignView()
{
	delete d;
}

void KexiMacroDesignView::updateData()
{
	//d->tableview->blockSignals(true);
	//d->tabledata->blockSignals(true);

	// Remove previous content of tabledata.
	d->tabledata->deleteAllRows();
	// Remove old property sets.
	d->propertyset->clear();

	// Add some empty rows
	for (int i=0; i<50; i++) {
		d->tabledata->append( d->tabledata->createItem() );
	}

	// Set the MacroItem's
	QStringList actionnames = KoMacro::Manager::self()->actionNames();
	::KoMacro::MacroItem::List macroitems = macro()->items();
	::KoMacro::MacroItem::List::ConstIterator it(macroitems.constBegin()), end(macroitems.constEnd());
	for(uint idx = 0; it != end; ++it, idx++) {
		KexiTableItem* tableitem = d->tabledata->at(idx);
		if(! tableitem) {
			// If there exists no such item, add it.
			tableitem = d->tabledata->createItem();
			d->tabledata->append(tableitem);
		}
		// Set the action-column.
		::KoMacro::Action::Ptr action = (*it)->action();
		if(action.data()) {
			int i = actionnames.findIndex( action->name() );
			if(i >= 0) {
				tableitem->at(COLUMN_ID_ACTION) = i + 1;
				//setAction(tableitem, action->name());
			}
		}
		// Set the comment-column.
		tableitem->at(COLUMN_ID_COMMENT) = (*it)->comment();
	}

	// set data for our spreadsheet: this will clear our sets
	d->tableview->setData(d->tabledata);

	// Add the property sets.
	it = macroitems.constBegin();
	for(uint idx = 0; it != end; ++it, idx++) {
		updateProperties(idx, 0, (*it)->action());
	}

	// work around a bug in the KexiTableView where we lose the stretch-setting...
	d->tableview->setColumnStretchEnabled( true, COLUMN_ID_COMMENT ); //last column occupies the rest of the area

	propertySetSwitched();

	//d->tableview->blockSignals(false);
	//d->tabledata->blockSignals(false);
}

void KexiMacroDesignView::updateProperties(int row, KoProperty::Set* set, KoMacro::Action::Ptr action)
{
	if(row < 0) {
		return; // ignore invalid rows.
	}

	if(! action.data()) {
		// don't display a propertyset if there is no action defined.
		d->propertyset->remove(row);
		return; // job done.
	}

	if(set) {
		// we need to clear old data before adding the new content.
		set->clear();
	}
	else {
		// if there exists no such propertyset yet, create one.
		set = new KoProperty::Set(d->propertyset, action->name());
		d->propertyset->insert(row, set, true);
	}

	// The caption.
	KoProperty::Property* prop = new KoProperty::Property("this:classString", action->text());
	prop->setVisible(false);
	set->addProperty(prop);

	// Display the list of variables.
	KoMacro::Variable::Map varmap = action->variables();
	KoMacro::Variable::Map::ConstIterator it, end( varmap.constEnd() );
	for( it = varmap.constBegin(); it != end; ++it) {
		KoMacro::Variable::Ptr v = it.data();
		kdDebug() << "KexiMacroDesignView::updateProperties() type=" << QVariant::typeToName( v->variant().type() ) << " name=" << it.key() << " value=" << v->variant().toString() << endl;
		KoProperty::Property* p = new KoProperty::Property(
			it.key().latin1(), //v->name().latin1(), // name
			0, // ListData
			v->variant(), // value
			v->caption(), // caption
			action->comment(), // description
			KoProperty::String // auto-detect type
		);
		//TODO why KoProperty::Auto doesn't detect the QString?
		set->addProperty(p);
	}
}

bool KexiMacroDesignView::loadData()
{
	if(! KexiMacroView::loadData())
		return false;
	updateData(); // update the tableview's data.
	return true;
}

KoProperty::Set* KexiMacroDesignView::propertySet()
{
	return d->propertyset ? d->propertyset->currentPropertySet() : 0;
}

void KexiMacroDesignView::itemSelected(KexiTableItem*)
{
	kdDebug() << "KexiMacroDesignView::itemSelected" << endl;
}

void KexiMacroDesignView::beforeCellChanged(KexiTableItem* item, int colnum, QVariant& newvalue, KexiDB::ResultInfo* result)
{
	Q_UNUSED(result);
	kdDebug() << "KexiMacroDesignView::beforeCellChanged() colnum=" << colnum << " newvalue=" << newvalue.toString() << endl;

	//int index = d->tabledata->find(item);
	int rowindex = d->tabledata->findRef(item);
	if(rowindex < 0) {
		kdWarning() << "KexiMacroDesignView::beforeCellChanged() No such item" << endl;
		return;
	}

	for(int i = macro()->items().count(); i <= rowindex; i++) {
		macro()->addItem( KoMacro::MacroItem::Ptr( new KoMacro::MacroItem() ) );
	}
	KoMacro::MacroItem::Ptr macroitem = macro()->items()[rowindex];

	switch(colnum) {
		case COLUMN_ID_ACTION: {
			QString actionname;
			bool ok;
			int selectedindex = newvalue.toInt(&ok);
			if(ok && selectedindex > 0) {
				QStringList actionnames = KoMacro::Manager::self()->actionNames();
				actionname = actionnames[ selectedindex - 1 ]; // first item is empty
			}
			KoMacro::Action::Ptr action = KoMacro::Manager::self()->action(actionname);
			macroitem->setAction(action);
			updateProperties(d->propertyset->currentRow(), d->propertyset->currentPropertySet(), action);
			//propertySetSwitched();
			propertySetReloaded(true);
		} break;
		case COLUMN_ID_COMMENT: {
			macroitem->setComment( newvalue.toString() );
		} break;
		default:
			kdWarning() << "KexiMacroDesignView::beforeCellChanged() No such column number " << colnum << endl;
			return;
	}

	setDirty();
}

void KexiMacroDesignView::rowUpdated(KexiTableItem*)
{
	kdDebug() << "KexiMacroDesignView::rowUpdated" << endl;
	//propertySetSwitched();
	//propertySetReloaded(true);
	//setDirty();
}

void KexiMacroDesignView::aboutToInsertRow(KexiTableItem*, KexiDB::ResultInfo*, bool)
{
	kdDebug() << "KexiMacroDesignView::aboutToInsertRow" << endl;
}

void KexiMacroDesignView::aboutToDeleteRow(KexiTableItem&, KexiDB::ResultInfo*, bool)
{
	kdDebug() << "KexiMacroDesignView::aboutToDeleteRow" << endl;
}

void KexiMacroDesignView::propertyChanged(KoProperty::Set&, KoProperty::Property&)
{
	kdDebug() << "KexiMacroDesignView::propertyChanged" << endl;
	//propertySetSwitched();
	//setDirty();
}

#include "keximacrodesignview.moc"

