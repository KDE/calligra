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

//#include <qlayout.h>
//#include <qsplitter.h>
//#include <qtimer.h>
//#include <qdatetime.h>
#include <qdom.h>
//#include <qstylesheet.h>
//#include <ktextbrowser.h>
#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>
#include <kexidb/error.h>

#include <widget/tableview/kexitableview.h>
#include <widget/tableview/kexitableviewdata.h>
#include <widget/tableview/kexitableitem.h>

#include <koproperty/set.h>
#include <koproperty/property.h>

#include "lib/macro.h"

class KexiMacroTableView : public KexiTableView
{
	public:
		KexiMacroTableView(KexiTableViewData* data, QWidget* parent)
			: KexiTableView(data, parent)
		{
		}

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
		* The \a KoMacro::Manager instance used to access the
		* Macro Framework.
		*/
		::KoMacro::Macro::Ptr const macro;

		KexiMacroTableView* tableview;
		KexiTableViewData* tabledata;
		KoProperty::Set* properties;

		/**
		* Constructor.
		*
		* \param m The passed \a KoMacro::Manager instance our
		*        \a manager points to.
		*/
		Private(::KoMacro::Macro* const m)
			: macro(m)
			, properties(0)
		{
		}

};

KexiMacroDesignView::KexiMacroDesignView(KexiMainWindow *mainwin, QWidget *parent, ::KoMacro::Macro* const macro)
	: KexiViewBase(mainwin, parent, "KexiMacroDesignView")
	, d( new Private(macro) )
{
	initTable();
	updateProperties();

	plugSharedAction( "data_execute", this, SLOT(execute()) );

	loadData();
}

KexiMacroDesignView::~KexiMacroDesignView()
{
	delete d;
}

void KexiMacroDesignView::initTable()
{
	d->tabledata = new KexiTableViewData();
	//d->tabledata->setReadOnly(true); // set read-only
	//d->tabledata->setValidator(); // set the value-validator
	d->tabledata->setSorting(-1); // disable sorting

	KexiTableViewColumn* actioncol = new KexiTableViewColumn(
		"action", // name/identifier
		KexiDB::Field::Enum, // fieldtype
		KexiDB::Field::NoConstraints, // constraints
		KexiDB::Field::NoOptions, // options
		0, // length
		0, // precision
		QVariant(), // defaultValue
		i18n("Action"), // caption
		QString::null, // description
		0 // width
	);
	d->tabledata->addColumn(actioncol);
	QValueVector<QString> items;
	items.append("");
	items.append("Application");
	items.append("Open Object");
	items.append("Close Object");
	items.append("Execute");
	actioncol->field()->setEnumHints(items);

	d->tabledata->addColumn( new KexiTableViewColumn(
		"comment", // name/identifier
		KexiDB::Field::Text, // fieldtype
		KexiDB::Field::NoConstraints, // constraints
		KexiDB::Field::NoOptions, // options
		0, // length
		0, // precision
		QVariant(), // defaultValue
		i18n("Comment"), // caption
		QString::null, // description
		0 // width
	) );

	connect(d->tabledata, SIGNAL(aboutToChangeCell(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)),
		this, SLOT(beforeCellChanged(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)));
	connect(d->tabledata, SIGNAL(rowUpdated(KexiTableItem*)),
		this, SLOT(rowUpdated(KexiTableItem*)));
	connect(d->tabledata, SIGNAL(aboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)),
		this, SLOT(aboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)));
	connect(d->tabledata, SIGNAL(aboutToDeleteRow(KexiTableItem&,KexiDB::ResultInfo*,bool)),
		this, SLOT(aboutToDeleteRow(KexiTableItem&,KexiDB::ResultInfo*,bool)));

	QHBoxLayout* layout = new QHBoxLayout(this);
	d->tableview = new KexiMacroTableView(d->tabledata, this);
	layout->addWidget(d->tableview);
}

void KexiMacroDesignView::updateProperties(int nr)
{
	if(d->properties) {
		delete d->properties;
	}
	d->properties = new KoProperty::Set(this, "KexiMacro");

	switch(nr) {
		case 1: { // Application
			QStringList actionname, actiontext;
			const KActionPtrList& actionlist = parentDialog()->mainWin()->actionCollection()->actions();
			KActionPtrList::ConstIterator it( actionlist.begin() );
			for(; it != actionlist.end(); ++it) {
				actionname.append( (*it)->name() );
				actiontext.append( (*it)->text().replace("&","") );
			}
			KoProperty::Property::ListData* proplist = new KoProperty::Property::ListData(actionname, actiontext);
			KoProperty::Property* prop = new KoProperty::Property(
				"action", // name
				proplist, // ListData
				"", // value
				i18n("Action"), // caption
				QString::null, // description
				KoProperty::List // type
			);
			d->properties->addProperty(prop);
		} break;
		case 2: { // Open Object
			QStringList objects;
			objects << "Table" << "Query" << "Form" << "Script";
			KoProperty::Property::ListData* proplist = new KoProperty::Property::ListData(objects, objects);
			KoProperty::Property* objprop = new KoProperty::Property(
				"object", // name
				proplist, // ListData
				"Table", // value
				i18n("Object"), // caption
				QString::null, // description
				KoProperty::List // type
			);
			d->properties->addProperty(objprop);

			KoProperty::Property* nameprop = new KoProperty::Property(
				"name", // name
				"", // value
				i18n("Name"), // caption
				QString::null, // description
				KoProperty::Auto // type
			);
			d->properties->addProperty(nameprop);
		} break;
		case 3: { // Close Object
		} break;
		case 4: { // Execute
		} break;
		default: {
		} break;
	}

	propertySetSwitched();

	connect(d->properties, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
		this, SLOT(propertyChanged(KoProperty::Set&, KoProperty::Property&)));
}

tristate KexiMacroDesignView::beforeSwitchTo(int mode, bool& dontstore)
{
	kexipluginsdbg << "KexiMacroDesignView::beforeSwitchTo mode=" << mode << endl;
	Q_UNUSED(dontstore);
	return true;
}

tristate KexiMacroDesignView::afterSwitchFrom(int mode)
{
	kexipluginsdbg << "KexiMacroDesignView::afterSwitchFrom mode=" << mode << endl;
	return true;
}

KoProperty::Set* KexiMacroDesignView::propertySet()
{
	return d->properties;
}

bool KexiMacroDesignView::loadData()
{
	QString data;
	if(! loadDataBlock(data)) {
		kexipluginsdbg << "KexiMacroDesignView::loadData(): no DataBlock" << endl;
		return false;
	}

	QString errmsg;
	int errline;
	int errcol;

	QDomDocument domdoc;
	bool parsed = domdoc.setContent(data, false, &errmsg, &errline, &errcol);

	if(! parsed) {
		kexipluginsdbg << "KexiMacroDesignView::loadData() XML parsing error line: " << errline << " col: " << errcol << " message: " << errmsg << endl;
		return false;
	}

	/*
	QDomElement scriptelem = domdoc.namedItem("script").toElement();
	if(scriptelem.isNull()) {
		kexipluginsdbg << "KexiMacroDesignView::loadData(): script domelement is null" << endl;
		return false;
	}
	QString interpretername = scriptelem.attribute("language");
	*/

	return true;
}

KexiDB::SchemaData* KexiMacroDesignView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
	KexiDB::SchemaData *s = KexiViewBase::storeNewData(sdata, cancel);
	kexipluginsdbg << "KexiMacroDesignView::storeNewData(): new id:" << s->id() << endl;

	if(!s || cancel) {
		delete s;
		return 0;
	}

	if(! storeData()) {
		kdWarning() << "KexiMacroDesignView::storeNewData Failed to store the data." << endl;
		//failure: remove object's schema data to avoid garbage
		KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
		conn->removeObject( s->id() );
		delete s;
		return 0;
	}

	return s;
}

tristate KexiMacroDesignView::storeData(bool /*dontAsk*/)
{
	kexipluginsdbg << "KexiMacroDesignView::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;

	QDomDocument domdoc("macro");
	/*
	QDomElement scriptelem = domdoc.createElement("script");
	domdoc.appendChild(scriptelem);
	scriptelem.setAttribute("language", language);
	scriptelem.setAttribute(it.key(), it.data().toString());
	QDomText scriptcode = domdoc.createTextNode(d->scriptaction->getCode());
	scriptelem.appendChild(scriptcode);
	*/

	return storeDataBlock( domdoc.toString() );
}

void KexiMacroDesignView::execute()
{
	kdDebug() << "KexiMacroDesignView::execute" << endl;
}

void KexiMacroDesignView::beforeCellChanged(KexiTableItem* item, int colnum, QVariant& newvalue, KexiDB::ResultInfo* result)
{
	Q_UNUSED(item);
	Q_UNUSED(colnum);
	Q_UNUSED(newvalue);
	Q_UNUSED(result);
	int value = newvalue.toInt();
	kdDebug() << "KexiMacroDesignView::beforeCellChanged colnum=" << colnum << " newvalue=" << value << endl;
	updateProperties(value);
}

void KexiMacroDesignView::rowUpdated(KexiTableItem* item)
{
	Q_UNUSED(item);
	setDirty();
	kdDebug() << "KexiMacroDesignView::rowUpdated" << endl;
}

void KexiMacroDesignView::aboutToInsertRow(KexiTableItem* item, KexiDB::ResultInfo* result, bool)
{
	Q_UNUSED(item);
	Q_UNUSED(result);
	kdDebug() << "KexiMacroDesignView::aboutToInsertRow" << endl;
}

void KexiMacroDesignView::aboutToDeleteRow(KexiTableItem& item, KexiDB::ResultInfo* result, bool)
{
	Q_UNUSED(item);
	Q_UNUSED(result);
	kdDebug() << "KexiMacroDesignView::aboutToDeleteRow" << endl;
}

void KexiMacroDesignView::propertyChanged(KoProperty::Set& set, KoProperty::Property& prop)
{
	Q_UNUSED(set);
	Q_UNUSED(prop);
	kdDebug() << "KexiMacroDesignView::propertyChanged" << endl;
}

#include "keximacrodesignview.moc"

