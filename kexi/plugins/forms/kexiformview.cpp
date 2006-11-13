/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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
 * Boston, MA 02110-1301, USA.
*/

#include "kexiformview.h"

#include <qobject.h>
#include <qfileinfo.h>
//Added by qt3to4:
#include <Q3CString>
#include <QFocusEvent>
#include <QDragMoveEvent>
#include <QEvent>
#include <QDropEvent>
#include <Q3ValueList>
#include <Q3HBoxLayout>
#include <QResizeEvent>

#include <formeditor/form.h>
#include <formeditor/formIO.h>
#include <formeditor/formmanager.h>
#include <formeditor/objecttree.h>
#include <formeditor/container.h>
#include <formeditor/widgetpropertyset.h>
#include <formeditor/commands.h>
#include <formeditor/widgetwithsubpropertiesinterface.h>

#include <kexi.h>
#include <kexidialogbase.h>
#include <kexidragobjects.h>
#include <kexidb/field.h>
#include <kexidb/fieldlist.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/utils.h>
#include <kexidb/preparedstatement.h>
#include <tableview/kexitableitem.h>
#include <tableview/kexitableviewdata.h>
#include <widget/kexipropertyeditorview.h>
#include <formeditor/objecttree.h>

#include <koproperty/set.h>
#include <koproperty/property.h>

#include "widgets/kexidbform.h"
#include "kexiformscrollview.h"
#include "kexidatasourcepage.h"
#include "widgets/kexidbautofield.h"

#define NO_DSWIZARD

//! @todo #define KEXI_SHOW_SPLITTER_WIDGET

KexiFormView::KexiFormView(KexiMainWindow *mainWin, QWidget *parent,
	const char *name, bool /*dbAware*/)
 : KexiDataAwareView( mainWin, parent, name )
 , m_propertySet(0)
 , m_resizeMode(KexiFormView::ResizeDefault)
 , m_query(0)
 , m_queryIsOwned(false)
 , m_cursor(0)
// , m_firstFocusWidget(0)
{
	m_delayedFormContentsResizeOnShow = 0;

	Q3HBoxLayout *l = new Q3HBoxLayout(this);
	l->setAutoAdd(true);

	m_scrollView = new KexiFormScrollView(this, viewMode()==Kexi::DataViewMode);

//moved	setViewWidget(m_scrollView);
//	m_scrollView->show();

	m_dbform = new KexiDBForm(m_scrollView->viewport(), m_scrollView, name/*, conn*/);
//	m_dbform->resize( m_scrollView->viewport()->size() - QSize(20, 20) );
//	m_dbform->resize(QSize(400, 300));
	m_scrollView->setWidget(m_dbform);
	m_scrollView->setResizingEnabled(viewMode()!=Kexi::DataViewMode);

//	initForm();

	if (viewMode()==Kexi::DataViewMode) {
		m_scrollView->recordNavigator()->setRecordHandler( m_scrollView );
		m_scrollView->viewport()->setPaletteBackgroundColor(m_dbform->palette().active().background());
//moved to formmanager		connect(formPart()->manager(), SIGNAL(noFormSelected()), SLOT(slotNoFormSelected()));
	}
	else
	{
		connect(KFormDesigner::FormManager::self(), SIGNAL(propertySetSwitched(KoProperty::Set*, bool, const QCString&)),
			this, SLOT(slotPropertySetSwitched(KoProperty::Set*, bool, const QCString&)));
		connect(KFormDesigner::FormManager::self(), SIGNAL(dirty(KFormDesigner::Form *, bool)),
			this, SLOT(slotDirty(KFormDesigner::Form *, bool)));

		connect(m_dbform, SIGNAL(handleDragMoveEvent(QDragMoveEvent*)), 
			this, SLOT(slotHandleDragMoveEvent(QDragMoveEvent*)));
		connect(m_dbform, SIGNAL(handleDropEvent(QDropEvent*)), 
			this, SLOT(slotHandleDropEvent(QDropEvent*)));

		// action stuff
		plugSharedAction("formpart_taborder", KFormDesigner::FormManager::self(), SLOT(editTabOrder()));
		plugSharedAction("formpart_adjust_size", KFormDesigner::FormManager::self(), SLOT(adjustWidgetSize()));
//TODO		plugSharedAction("formpart_pixmap_collection", formPart()->manager(), SLOT(editFormPixmapCollection()));
//TODO		plugSharedAction("formpart_connections", formPart()->manager(), SLOT(editConnections()));

		plugSharedAction("edit_copy", KFormDesigner::FormManager::self(), SLOT(copyWidget()));
		plugSharedAction("edit_cut", KFormDesigner::FormManager::self(), SLOT(cutWidget()));
		plugSharedAction("edit_paste", KFormDesigner::FormManager::self(), SLOT(pasteWidget()));
		plugSharedAction("edit_delete", KFormDesigner::FormManager::self(), SLOT(deleteWidget()));
		plugSharedAction("edit_select_all", KFormDesigner::FormManager::self(), SLOT(selectAll()));
		plugSharedAction("formpart_clear_contents", KFormDesigner::FormManager::self(), SLOT(clearWidgetContent()));
		plugSharedAction("edit_undo", KFormDesigner::FormManager::self(), SLOT(undo()));
		plugSharedAction("edit_redo", KFormDesigner::FormManager::self(), SLOT(redo()));

		plugSharedAction("formpart_layout_menu", KFormDesigner::FormManager::self(), 0 );
		plugSharedAction("formpart_layout_hbox", KFormDesigner::FormManager::self(), SLOT(layoutHBox()) );
		plugSharedAction("formpart_layout_vbox", KFormDesigner::FormManager::self(), SLOT(layoutVBox()) );
		plugSharedAction("formpart_layout_grid", KFormDesigner::FormManager::self(), SLOT(layoutGrid()) );
#ifdef KEXI_SHOW_SPLITTER_WIDGET
		plugSharedAction("formpart_layout_hsplitter", KFormDesigner::FormManager::self(), SLOT(layoutHSplitter()) );
		plugSharedAction("formpart_layout_vsplitter", KFormDesigner::FormManager::self(), SLOT(layoutVSplitter()) );
#endif
		plugSharedAction("formpart_break_layout", KFormDesigner::FormManager::self(), SLOT(breakLayout()) );

		plugSharedAction("formpart_format_raise", KFormDesigner::FormManager::self(), SLOT(bringWidgetToFront()) );
		plugSharedAction("formpart_format_lower", KFormDesigner::FormManager::self(), SLOT(sendWidgetToBack()) );

		plugSharedAction("other_widgets_menu", KFormDesigner::FormManager::self(), 0 );
		setAvailable("other_widgets_menu", true);

		plugSharedAction("formpart_align_menu", KFormDesigner::FormManager::self(), 0 );
		plugSharedAction("formpart_align_to_left", KFormDesigner::FormManager::self(),SLOT(alignWidgetsToLeft()) );
		plugSharedAction("formpart_align_to_right", KFormDesigner::FormManager::self(), SLOT(alignWidgetsToRight()) );
		plugSharedAction("formpart_align_to_top", KFormDesigner::FormManager::self(), SLOT(alignWidgetsToTop()) );
		plugSharedAction("formpart_align_to_bottom", KFormDesigner::FormManager::self(), SLOT(alignWidgetsToBottom()) );
		plugSharedAction("formpart_align_to_grid", KFormDesigner::FormManager::self(), SLOT(alignWidgetsToGrid()) );

		plugSharedAction("formpart_adjust_size_menu", KFormDesigner::FormManager::self(), 0 );
		plugSharedAction("formpart_adjust_to_fit", KFormDesigner::FormManager::self(), SLOT(adjustWidgetSize()) );
		plugSharedAction("formpart_adjust_size_grid", KFormDesigner::FormManager::self(), SLOT(adjustSizeToGrid()) );
		plugSharedAction("formpart_adjust_height_small", KFormDesigner::FormManager::self(),  SLOT(adjustHeightToSmall()) );
		plugSharedAction("formpart_adjust_height_big", KFormDesigner::FormManager::self(), SLOT(adjustHeightToBig()) );
		plugSharedAction("formpart_adjust_width_small", KFormDesigner::FormManager::self(), SLOT(adjustWidthToSmall()) );
		plugSharedAction("formpart_adjust_width_big", KFormDesigner::FormManager::self(), SLOT(adjustWidthToBig()) );

		plugSharedAction("format_font", KFormDesigner::FormManager::self(), SLOT(changeFont()) );
	}

	initForm();

	KexiDataAwareView::init( m_scrollView, m_scrollView, m_scrollView,
		/* skip data-awarness if design mode */ viewMode()==Kexi::DesignViewMode );

	connect(this, SIGNAL(focus(bool)), this, SLOT(slotFocus(bool)));
	/// @todo skip this if ther're no borders
//	m_dbform->resize( m_dbform->size()+QSize(m_scrollView->verticalScrollBar()->width(), m_scrollView->horizontalScrollBar()->height()) );
}

KexiFormView::~KexiFormView()
{
	if (m_cursor) {
		KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
		conn->deleteCursor(m_cursor);
		m_cursor = 0;
	}
	deleteQuery();

	// Important: form window is closed.
	// Set property set to 0 because there is *only one* instance of a property set class
	// in Kexi, so the main window wouldn't know the set in fact has been changed.
	m_propertySet = 0;
	propertySetSwitched();
}

void
KexiFormView::deleteQuery()
{
	if (m_cursor) {
		KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
		conn->deleteCursor(m_cursor);
		m_cursor = 0;
	}

	if (m_queryIsOwned) {
		delete m_query;
	} else {
//! @todo remove this shared query from listened queries list
	}
	m_query = 0;
}

KFormDesigner::Form*
KexiFormView::form() const
{
	if(viewMode()==Kexi::DataViewMode)
		return tempData()->previewForm;
	else
		return tempData()->form;
}

void
KexiFormView::setForm(KFormDesigner::Form *f)
{
	if(viewMode()==Kexi::DataViewMode)
		tempData()->previewForm = f;
	else
		tempData()->form = f;
}

void
KexiFormView::initForm()
{
	setForm( new KFormDesigner::Form(KexiFormPart::library(), 0, viewMode()==Kexi::DesignViewMode) );
//	if (viewMode()==Kexi::DataViewMode)
		//form()->setDesignMode(false);
	form()->createToplevel(m_dbform, m_dbform);

	if (viewMode()==Kexi::DesignViewMode) {
		//we want to be informed about executed commands
		connect(form()->commandHistory(), SIGNAL(commandExecuted()),
			KFormDesigner::FormManager::self(), SLOT(slotHistoryCommandExecuted()));
	}

	const bool newForm = parentDialog()->id() < 0;

	KexiDB::FieldList *fields = 0;
	if (newForm) {
		// Show the form wizard if this is a new Form
#ifndef NO_DSWIZARD
		KexiDataSourceWizard *w = new KexiDataSourceWizard(mainWin(), (QWidget*)mainWin(), "datasource_wizard");
		if(!w->exec())
			fields = 0;
		else
			fields = w->fields();
		delete w;
#endif
	}

	if(fields)
	{
		QDomDocument dom;
		formPart()->generateForm(fields, dom);
		KFormDesigner::FormIO::loadFormFromDom(form(), m_dbform, dom);
		//! @todo handle errors
	}
	else
		loadForm();

	if(form()->autoTabStops())
		form()->autoAssignTabStops();

	//collect tab order information
	m_dbform->updateTabStopsOrder(form());

//	if (m_dbform->orderedFocusWidgets()->first())
	//	m_scrollView->setFocusProxy( m_dbform->orderedFocusWidgets()->first() );

	KFormDesigner::FormManager::self()->importForm(form(), viewMode()==Kexi::DataViewMode);
	m_scrollView->setForm(form());

//	m_dbform->updateTabStopsOrder(form());
//	QSize s = m_dbform->size();
//	QApplication::sendPostedEvents();
//	m_scrollView->resize( s );
//	m_dbform->resize(s);
	m_scrollView->refreshContentsSize();
//	m_scrollView->refreshContentsSizeLater(true,true);

	if (newForm && !fields) {
		/* Our form's area will be resized more than once.
		Let's resize form widget itself later. */
		m_delayedFormContentsResizeOnShow = 3;
	}

	updateDataSourcePage();

	if (!newForm && viewMode()==Kexi::DesignViewMode) {
		form()->clearCommandHistory();
	}
}

void KexiFormView::updateAutoFieldsDataSource()
{
//! @todo call this when form's data source is changed
	//update autofields: 
	//-inherit captions
	//-inherit data types
	//(this data has not been stored in the form)
	QString dataSourceString( m_dbform->dataSource() );
	QCString dataSourceMimeTypeString( m_dbform->dataSourceMimeType() );
	KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
	KexiDB::TableOrQuerySchema tableOrQuery(
		conn, dataSourceString.latin1(), dataSourceMimeTypeString=="kexi/table");
	if (!tableOrQuery.table() && !tableOrQuery.query())
		return;
	for (KFormDesigner::ObjectTreeDictIterator it(*form()->objectTree()->dict());
		it.current(); ++it)
	{
		KexiDBAutoField *afWidget = dynamic_cast<KexiDBAutoField*>( it.current()->widget() );
		if (afWidget) {
			KexiDB::QueryColumnInfo *colInfo = tableOrQuery.columnInfo( afWidget->dataSource() );
			if (colInfo) {
				afWidget->setColumnInfo(colInfo);
					//setFieldTypeInternal((int)colInfo->field->type());
					//afWidget->setFieldCaptionInternal(colInfo->captionOrAliasOrName());
			}
		}
	}
}

void KexiFormView::updateValuesForSubproperties()
{
//! @todo call this when form's data source is changed
	//update autofields: 
	//-inherit captions
	//-inherit data types
	//(this data has not been stored in the form)
	QString dataSourceString( m_dbform->dataSource() );
	QCString dataSourceMimeTypeString( m_dbform->dataSourceMimeType() );
	KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
	KexiDB::TableOrQuerySchema tableOrQuery(
		conn, dataSourceString.latin1(), dataSourceMimeTypeString=="kexi/table");
	if (!tableOrQuery.table() && !tableOrQuery.query())
		return;

	for (KFormDesigner::ObjectTreeDictIterator it(*form()->objectTree()->dict());
		it.current(); ++it)
	{
		// (delayed) set values for subproperties
//! @todo this could be at the KFD level, but KFD is going to be merged anyway with kexiforms, right?
		KFormDesigner::WidgetWithSubpropertiesInterface* subpropIface 
			= dynamic_cast<KFormDesigner::WidgetWithSubpropertiesInterface*>( it.current()->widget() );
		if (subpropIface && subpropIface->subwidget() && it.current()->subproperties() ) {
			QWidget *subwidget = subpropIface->subwidget();
			QMap<QString, QVariant>* subprops = it.current()->subproperties();
			for (QMapConstIterator<QString, QVariant> subpropIt = subprops->constBegin(); subpropIt!=subprops->constEnd(); ++subpropIt) {
				kexipluginsdbg << "KexiFormView::loadForm(): delayed setting of the subproperty: widget="
					<< it.current()->widget()->name() << " prop=" << subpropIt.key() << " val=" << subpropIt.data() << endl;
				subwidget->setProperty( subpropIt.key().latin1(), subpropIt.data() );
			}
		}
	}
}

void
KexiFormView::loadForm()
{
//@todo also load m_resizeMode !

	kexipluginsdbg << "KexiFormView::loadForm() Loading the form with id : " << parentDialog()->id() << endl;
	// If we are previewing the Form, use the tempData instead of the form stored in the db
	if(viewMode()==Kexi::DataViewMode && !tempData()->tempForm.isNull() )
	{
		KFormDesigner::FormIO::loadFormFromString(form(), m_dbform, tempData()->tempForm);
		updateAutoFieldsDataSource();
		updateValuesForSubproperties();
		return;
	}

	// normal load
	QString data;
	loadDataBlock(data);
	KFormDesigner::FormIO::loadFormFromString(form(), m_dbform, data);

	//"autoTabStops" property is loaded -set it within the form tree as well
	form()->setAutoTabStops( m_dbform->autoTabStops() );

	updateAutoFieldsDataSource();
	updateValuesForSubproperties();
}

void
KexiFormView::slotPropertySetSwitched(KoProperty::Set *set, bool forceReload, const QCString& propertyToSelect)
{
//	if (set && parentDialog()!=parentDialog()->mainWin()->currentDialog())
	if (form() != KFormDesigner::FormManager::self()->activeForm())
		return; //this is not the current form view
	m_propertySet = set;
	if (forceReload)
		propertySetReloaded(true/*preservePrevSelection*/, propertyToSelect);
	else
		propertySetSwitched();

	formPart()->dataSourcePage()->assignPropertySet(m_propertySet);
}

tristate
KexiFormView::beforeSwitchTo(int mode, bool &dontStore)
{
	if (mode!=viewMode()) {
		if (viewMode()==Kexi::DataViewMode) {
			if (!m_scrollView->acceptRowEdit())
				return cancelled;

			m_scrollView->beforeSwitchView();
		}
		else {
			//remember our pos
			tempData()->scrollViewContentsPos
				= QPoint(m_scrollView->contentsX(), m_scrollView->contentsY());
		}
	}

	// we don't store on db, but in our TempData
	dontStore = true;
	if(dirty() && (mode == Kexi::DataViewMode) && form()->objectTree()) {
		KexiFormPart::TempData* temp = tempData();
		if (!KFormDesigner::FormIO::saveFormToString(form(), temp->tempForm))
			return false;
	}

	return true;
}

tristate
KexiFormView::afterSwitchFrom(int mode)
{
	if (mode == 0 || mode == Kexi::DesignViewMode) {
		if (parentDialog()->neverSaved()) {
			m_dbform->resize(QSize(400, 300));
			m_scrollView->refreshContentsSizeLater(true,true);
			//m_delayedFormContentsResizeOnShow = false;
		}
	}

	if (mode != 0 && mode != Kexi::DesignViewMode) {
		//preserve contents pos after switching to other view
		m_scrollView->setContentsPos(tempData()->scrollViewContentsPos.x(),
			tempData()->scrollViewContentsPos.y());
	}
//	if (mode == Kexi::DesignViewMode) {
		//m_scrollView->move(0,0);
		//m_scrollView->setContentsPos(0,0);
		//m_scrollView->moveChild(m_dbform, 0, 0);
//	}

	if((mode == Kexi::DesignViewMode) && viewMode()==Kexi::DataViewMode) {
		// The form may have been modified, so we must recreate the preview
		delete m_dbform; // also deletes form()
		m_dbform = new KexiDBForm(m_scrollView->viewport(), m_scrollView, "KexiDBForm");
		m_scrollView->setWidget(m_dbform);

		initForm();
//moved to formmanager		slotNoFormSelected();

		//reset position
		m_scrollView->setContentsPos(0,0);
		m_dbform->move(0,0);

	}

	//update tab stops if needed
	if (viewMode()==Kexi::DataViewMode) {
//		//propagate current "autoTabStops" property value to the form tree
//		form()->setAutoTabStops( m_dbform->autoTabStops() );

//		if(form()->autoTabStops())
//			form()->autoAssignTabStops();
	}
	else {
		//set "autoTabStops" property
		m_dbform->setAutoTabStops( form()->autoTabStops() );
	}

	if (viewMode() == Kexi::DataViewMode) {
//TMP!!
		initDataSource();

		//handle events for this form
		m_scrollView->setMainWidgetForEventHandling(parentDialog()->mainWin(), m_dbform);

		//set focus on 1st focusable widget which has valid dataSource property set
		if (!m_dbform->orderedFocusWidgets()->isEmpty()) {
//			QWidget *www = focusWidget();
			//if (Kexi::hasParent(this, qApp->focusWidget())) {
				QEvent fe( QEvent::FocusOut );
				QFocusEvent::setReason(QFocusEvent::Tab);
				QApplication::sendEvent( qApp->focusWidget(), &fe );
				QFocusEvent::resetReason();
			//}

			Q3PtrListIterator<QWidget> it(*m_dbform->orderedFocusWidgets());
			for (;it.current(); ++it) {
				KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>(it.current());
				if (iface)
					kexipluginsdbg << iface->dataSource() << endl;
				if (iface && iface->columnInfo() && !iface->isReadOnly()
/*! @todo add option for skipping autoincremented fields */
					/* also skip autoincremented fields:*/
					&& !iface->columnInfo()->field->isAutoIncrement()) //!iface->dataSource().isEmpty()
					break;
			}
			if (!it.current()) //eventually, focus first available widget if nothing other is available
				it.toFirst();

			it.current()->setFocus();
			SET_FOCUS_USING_REASON(it.current(), QFocusEvent::Tab);
			m_setFocusInternalOnce = it.current();
		}

		if (m_query)
			m_scrollView->selectFirstRow();
	}

	//dirty only if it's a new object
	if (mode == 0)
		setDirty( parentDialog()->partItem()->neverSaved() );

	if (mode==Kexi::DataViewMode && viewMode()==Kexi::DesignViewMode) {
//		slotPropertySetSwitched
//		emit KFormDesigner::FormManager::self()->propertySetSwitched( KFormDesigner::FormManager::self()->propertySet()->set(), true );
	}

	return true;
}

void KexiFormView::initDataSource()
{
	deleteQuery();
	QString dataSourceString( m_dbform->dataSource() );
	Q3CString dataSourceMimeTypeString( m_dbform->dataSourceMimeType() );
//! @todo also handle anonymous (not stored) queries provided as statements here
	bool ok = !dataSourceString.isEmpty();

/*			if (m_previousDataSourceString.lower()==dataSourceString.lower() && !m_cursor) {
			//data source changed: delete previous cursor
			m_conn->deleteCursor(m_cursor);
			m_cursor = 0;
		}*/

	KexiDB::TableSchema *tableSchema = 0;
	KexiDB::Connection *conn = 0;
	QStringList sources;
	bool forceReadOnlyDataSource = false;

	if (ok) {
//		m_previousDataSourceString = dataSourceString;

		//collect all data-aware widgets and create query schema
		m_scrollView->setMainDataSourceWidget(m_dbform);
		sources = m_scrollView->usedDataSources();
		conn = parentDialog()->mainWin()->project()->dbConnection();
		if (dataSourceMimeTypeString.isEmpty() /*table type is the default*/ 
			|| dataSourceMimeTypeString=="kexi/table")
		{
			tableSchema = conn->tableSchema( dataSourceString );
			if (tableSchema) {
				/* We will build a _minimum_ query schema from selected table fields. */
				m_query = new KexiDB::QuerySchema();
				m_queryIsOwned = true;

				if (dataSourceMimeTypeString.isEmpty())
					m_dbform->setDataSourceMimeType("kexi/table"); //update for compatibility
			}
		}
		
		if (!tableSchema) {
			if (dataSourceMimeTypeString.isEmpty() /*also try to find a query (for compatibility with Kexi<=0.9)*/ 
				|| dataSourceMimeTypeString=="kexi/query")
			{
				//try to find predefined query schema.
				//Note: In general, we could not skip unused fields within this query because
				//      it can have GROUP BY clause.
		//! @todo check if the query could have skipped unused fields (no GROUP BY, no joins, etc.)
				m_query = conn->querySchema( dataSourceString );
				m_queryIsOwned = false;
				ok = m_query != 0;
				if (ok && dataSourceMimeTypeString.isEmpty())
					m_dbform->setDataSourceMimeType("kexi/query"); //update for compatibility
				// query results are read-only
//! @todo There can be read-write queries, e.g. simple "SELECT * FROM...". Add a checking function to KexiDB.
				forceReadOnlyDataSource = true;
			}
			else //no other mime types supported
				ok = false;
		}
	}

	Q3Dict<char> invalidSources(997);
	if (ok) {
		KexiDB::IndexSchema *pkey = tableSchema ? tableSchema->primaryKey() : 0;
		if (pkey) {
			//always add all fields from table's primary key
			// (don't worry about duplicates, unique list will be computed later)
			sources += pkey->names();
			kexipluginsdbg << "KexiFormView::initDataSource(): pkey added to data sources: " << pkey->names() << endl;
		}
		kexipluginsdbg << "KexiFormView::initDataSource(): sources=" << sources << endl;

		uint index = 0;
		for (QStringList::ConstIterator it = sources.constBegin();
			it!=sources.constEnd(); ++it, index++) {
/*! @todo add expression support */
			QString fieldName( (*it).lower() );
			//remove "tablename." if it was prepended
			if (tableSchema && fieldName.startsWith( tableSchema->name().lower()+"." ))
				fieldName = fieldName.mid(tableSchema->name().length()+1);
			//remove "queryname." if it was prepended
			if (!tableSchema && fieldName.startsWith( m_query->name().lower()+"." ))
				fieldName = fieldName.mid(m_query->name().length()+1);
			KexiDB::Field *f = tableSchema ? tableSchema->field(fieldName) : m_query->field(fieldName);
			if (!f) {
/*! @todo show error */
				//remove this widget from the set of data widgets in the provider
/*! @todo fieldName is ok, but what about expressions? */
				invalidSources.insert( fieldName, (const char*)1 ); // += index;
				kexipluginsdbg << "KexiFormView::initDataSource(): invalidSources+=" << index << " (" 
					<< (*it) << ")" << endl;
				continue;
			}
			if (tableSchema) {
				if (!m_query->hasField( f )) {
					//we're building a new query: add this field
					m_query->addField( f );
				}
			}
		}
		if (invalidSources.count()==sources.count()) {
			//all data sources are invalid! don't execute the query
			deleteQuery();
		}
		else {
			m_cursor = conn->executeQuery( *m_query );
		}
		m_scrollView->invalidateDataSources( invalidSources, m_query );
		ok = m_cursor!=0;
	}

	if (!invalidSources.isEmpty())
		m_dbform->updateTabStopsOrder();

	if (ok) {
//! @todo PRIMITIVE!! data setting:
//! @todo KexiTableViewData is not great name for data class here... rename/move?
		KexiTableViewData* data = new KexiTableViewData(m_cursor);
		if (forceReadOnlyDataSource)
			data->setReadOnly(true);
		data->preloadAllRows();

///*! @todo few backends return result count for free! - no need to reopen() */
//			int resultCount = -1;
//			if (ok) {
//				resultCount = m_conn->resultCount(m_conn->selectStatement(*m_query));
//				ok = m_cursor->reopen();
//			}
//			if (ok)
//				ok = ! (!m_cursor->moveFirst() && m_cursor->error());

		m_scrollView->setData( data, true /*owner*/ );
	}
	else
		m_scrollView->setData( 0, false );
}

void
KexiFormView::slotDirty(KFormDesigner::Form *dirtyForm, bool isDirty)
{
	if(dirtyForm == form())
		KexiViewBase::setDirty(isDirty);
}

KexiDB::SchemaData*
KexiFormView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
	KexiDB::SchemaData *s = KexiViewBase::storeNewData(sdata, cancel);
	kexipluginsdbg << "KexiDBForm::storeNewData(): new id:" << s->id() << endl;

	if (!s || cancel) {
		delete s;
		return 0;
	}
	if (!storeData()) {
		//failure: remove object's schema data to avoid garbage
		KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
		conn->removeObject( s->id() );
		delete s;
		return 0;
	}
	return s;
}

tristate
KexiFormView::storeData(bool dontAsk)
{
	Q_UNUSED(dontAsk);
	kexipluginsdbg << "KexiDBForm::storeData(): " << parentDialog()->partItem()->name() 
		<< " [" << parentDialog()->id() << "]" << endl;

	//-- first, store local BLOBs, so identifiers can be updated
//! @todo remove unused data stored previously
	KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
	KexiDB::TableSchema *blobsTable = conn->tableSchema("kexi__blobs");
	if (!blobsTable) { //compatibility check for older Kexi project versions
//! @todo show message about missing kexi__blobs?
		return false;
	}
	// Not all engines accept passing NULL to PKEY o_id, so we're omitting it.
	QStringList blobsFieldNamesWithoutID(blobsTable->names());
	blobsFieldNamesWithoutID.pop_front();
	KexiDB::FieldList *blobsFieldsWithoutID = blobsTable->subList(blobsFieldNamesWithoutID);
	
	KexiDB::PreparedStatement::Ptr st = conn->prepareStatement(
		KexiDB::PreparedStatement::InsertStatement, *blobsFieldsWithoutID);
//#if 0 
////! @todo reenable when all drivers get PreparedStatement support
	if (!st) {
		delete blobsFieldsWithoutID;
		//! @todo show message 
		return false;
	}
//#endif
	KexiBLOBBuffer *blobBuf = KexiBLOBBuffer::self();
	for (QMapConstIterator<QWidget*, KexiBLOBBuffer::Id_t> it = m_unsavedLocalBLOBs.constBegin(); 
		it!=m_unsavedLocalBLOBs.constEnd(); ++it)
	{
		if (!it.key()) {
			kexipluginswarn << "KexiFormView::storeData(): it.key()==0 !" << endl;
			continue;
		}
		kexipluginsdbg << "name=" << it.key()->name() << " dataID=" << it.data() << endl;
		KexiBLOBBuffer::Handle h( blobBuf->objectForId(it.data(), /*!stored*/false) );
		if (!h)
			continue; //no BLOB assigned

		QString originalFileName(h.originalFileName());
		QFileInfo fi(originalFileName);
		QString caption(fi.baseName().replace('_', " ").simplified());
////////

//		KexiDB::PreparedStatement st(KexiDB::PreparedStatement::InsertStatement, *conn, *blobsTable);
		if (st) {
			*st /* << NO, (pgsql doesn't support this):QVariant()*/ /*id*/ 
				<< h.data() << originalFileName << caption 
				<< h.mimeType() << (uint)/*! @todo unsafe */h.folderId();
			if (!st->execute()) {
				delete blobsFieldsWithoutID;
				kexipluginsdbg << " execute error" << endl;
				return false;
			}
		}
///////
#if 0
		if (!conn->insertRecord(*blobsFieldsWithoutID, h.data(), originalFileName, caption, h.mimeType())) {
			delete blobsFieldsWithoutID;
//! @todo show message?
			return false;
		}
#endif
		delete blobsFieldsWithoutID;
		blobsFieldsWithoutID=0;
		const quint64 storedBLOBID = conn->lastInsertedAutoIncValue("o_id", "kexi__blobs");
		if ((quint64)-1 == storedBLOBID) {
//! @todo show message?
			return false;
		}
		kexipluginsdbg << " storedDataID=" << storedBLOBID << endl;
		h.setStoredWidthID((KexiBLOBBuffer::Id_t /*unsafe - will be fixed in Qt4*/)storedBLOBID);
		//set widget's internal property so it can be saved...
		const QVariant oldStoredPixmapId( it.key()->property("storedPixmapId") );
		it.key()->setProperty("storedPixmapId", 
			QVariant((uint /* KexiBLOBBuffer::Id_t is unsafe and unsupported by QVariant - will be fixed in Qt4*/)storedBLOBID));
		KFormDesigner::ObjectTreeItem *widgetItem = form()->objectTree()->lookup(it.key()->name());
		if (widgetItem)
			widgetItem->addModifiedProperty( "storedPixmapId", oldStoredPixmapId );
		else
			kexipluginswarn << "KexiFormView::storeData(): no '" << widgetItem->name() << "' widget found within a form" << endl;
	}
//TODO: forall it.key()->setProperty(

	//-- now, save form's XML
	QString data;
	if (!KFormDesigner::FormIO::saveFormToString(tempData()->form, data))
		return false;
	if (!storeDataBlock(data))
		return false;

	//all blobs are now saved
	m_unsavedLocalBLOBs.clear();

	tempData()->tempForm = QString::null;
	return true;
}

#if 0
/// Action stuff /////////////////
void
KexiFormView::slotWidgetSelected(KFormDesigner::Form *f, bool multiple)
{
	if(f != form())
		return;

	enableFormActions();
	// Enable edit actions
	setAvailable("edit_copy", true);
	setAvailable("edit_cut", true);
	setAvailable("edit_clear", true);

	// 'Align Widgets' menu
	setAvailable("formpart_align_menu", multiple);
	setAvailable("formpart_align_to_left", multiple);
	setAvailable("formpart_align_to_right", multiple);
	setAvailable("formpart_align_to_top", multiple);
	setAvailable("formpart_align_to_bottom", multiple);

	setAvailable("formpart_adjust_size_menu", true);
	setAvailable("formpart_adjust_width_small", multiple);
	setAvailable("formpart_adjust_width_big", multiple);
	setAvailable("formpart_adjust_height_small", multiple);
	setAvailable("formpart_adjust_height_big", multiple);

	setAvailable("formpart_format_raise", true);
	setAvailable("formpart_format_lower", true);

	// If the widgets selected is a container, we enable layout actions
	if(!multiple)
	{
		KFormDesigner::ObjectTreeItem *item = f->objectTree()->lookup( f->selectedWidgets()->first()->name() );
		if(item && item->container())
			multiple = true;
	}
	// Layout actions
	setAvailable("formpart_layout_hbox", multiple);
	setAvailable("formpart_layout_vbox", multiple);
	setAvailable("formpart_layout_grid", multiple);

	KFormDesigner::Container *container = f->activeContainer();
	setAvailable("formpart_break_layout", container ?
		(container->layoutType() != KFormDesigner::Container::NoLayout) : false );
}

void
KexiFormView::slotFormWidgetSelected(KFormDesigner::Form *f)
{
	if(f != form())
		return;

	disableWidgetActions();
	enableFormActions();

	// Layout actions
	setAvailable("formpart_layout_hbox", true);
	setAvailable("formpart_layout_vbox", true);
	setAvailable("formpart_layout_grid", true);
	setAvailable("formpart_break_layout", (f->toplevelContainer()->layoutType() != KFormDesigner::Container::NoLayout));
}

void
KexiFormView::slotNoFormSelected() // == form in preview mode
{
	disableWidgetActions();

	// Disable paste action
	setAvailable("edit_paste", false);
	setAvailable("edit_undo", false);
	setAvailable("edit_redo", false);

	// Disable 'Tools' actions
	setAvailable("formpart_pixmap_collection", false);
	setAvailable("formpart_connections", false);
	setAvailable("formpart_taborder", false);
	setAvailable("formpart_change_style", false);
}

void
KexiFormView::enableFormActions()
{
	// Enable 'Tools' actions
	setAvailable("formpart_pixmap_collection", true);
	setAvailable("formpart_connections", true);
	setAvailable("formpart_taborder", true);

	setAvailable("edit_paste", KFormDesigner::FormManager::self()->isPasteEnabled());
}

void
KexiFormView::disableWidgetActions()
{
	// Disable edit actions
	setAvailable("edit_copy", false);
	setAvailable("edit_cut", false);
	setAvailable("edit_clear", false);

	// Disable format functions
	setAvailable("formpart_align_menu", false);
	setAvailable("formpart_align_to_left", false);
	setAvailable("formpart_align_to_right", false);
	setAvailable("formpart_align_to_top", false);
	setAvailable("formpart_align_to_bottom", false);

	setAvailable("formpart_adjust_size_menu", false);
	setAvailable("formpart_adjust_width_small", false);
	setAvailable("formpart_adjust_width_big", false);
	setAvailable("formpart_adjust_height_small", false);
	setAvailable("formpart_adjust_height_big", false);

	setAvailable("formpart_format_raise", false);
	setAvailable("formpart_format_lower", false);

	setAvailable("formpart_layout_hbox", false);
	setAvailable("formpart_layout_vbox", false);
	setAvailable("formpart_layout_grid", false);
	setAvailable("formpart_break_layout", false);
}

void
KexiFormView::setUndoEnabled(bool enabled)
{
	setAvailable("edit_undo", enabled);
}

void
KexiFormView::setRedoEnabled(bool enabled)
{
	setAvailable("edit_redo", enabled);
}
#endif //0

QSize
KexiFormView::preferredSizeHint(const QSize& otherSize)
{
	if (parentDialog()->neverSaved()) {
		//ignore otherSize if possible
//		return KexiViewBase::preferredSizeHint( (parentDialog() && parentDialog()->mdiParent()) ? QSize(10000,10000) : otherSize);
	}

	return (m_dbform->size()
			+QSize(m_scrollView->verticalScrollBar()->isVisible() ? m_scrollView->verticalScrollBar()->width()*3/2 : 10,
			 m_scrollView->horizontalScrollBar()->isVisible() ? m_scrollView->horizontalScrollBar()->height()*3/2 : 10))
		.expandedTo( KexiViewBase::preferredSizeHint(otherSize) );
}

void
KexiFormView::resizeEvent( QResizeEvent *e )
{
	if (viewMode()==Kexi::DataViewMode) {
		m_scrollView->refreshContentsSizeLater(
			e->size().width()!=e->oldSize().width(),
			e->size().height()!=e->oldSize().height()
		);
	}
	KexiViewBase::resizeEvent(e);
	m_scrollView->updateNavPanelGeometry();
	if (m_delayedFormContentsResizeOnShow>0) { // && isVisible()) {
		m_delayedFormContentsResizeOnShow--;
		m_dbform->resize( e->size() - QSize(30, 30) );
	}
}

void
KexiFormView::setFocusInternal()
{
	if (viewMode() == Kexi::DataViewMode) {
		if (m_dbform->focusWidget()) {
			//better-looking focus
			if (m_setFocusInternalOnce) {
				SET_FOCUS_USING_REASON(m_setFocusInternalOnce, QFocusEvent::Other);//Tab);
				m_setFocusInternalOnce = 0;
			}
			else {
				//ok? SET_FOCUS_USING_REASON(m_dbform->focusWidget(), QFocusEvent::Other);//Tab);
			}
			return;
		}
	}
	QWidget::setFocus();
}

void
KexiFormView::show()
{
	KexiDataAwareView::show();

//moved from KexiFormScrollView::show():

	//now get resize mode settings for entire form
	//	if (resizeMode() == KexiFormView::ResizeAuto)
	if (viewMode()==Kexi::DataViewMode) {
		if (resizeMode() == KexiFormView::ResizeAuto)
			m_scrollView->setResizePolicy(Q3ScrollView::AutoOneFit);
	}
}

void
KexiFormView::slotFocus(bool in)
{
	if(in && form() && KFormDesigner::FormManager::self() && KFormDesigner::FormManager::self()->activeForm() != form()) {
		KFormDesigner::FormManager::self()->windowChanged(m_dbform);
		updateDataSourcePage();
	}
}

void
KexiFormView::updateDataSourcePage()
{
	if (viewMode()==Kexi::DesignViewMode) {
		Q3CString dataSourceMimeType, dataSource;
		KFormDesigner::WidgetPropertySet *set = KFormDesigner::FormManager::self()->propertySet();
		if (set->contains("dataSourceMimeType"))
			dataSourceMimeType = (*set)["dataSourceMimeType"].value().toCString();
		if (set->contains("dataSource"))
			dataSource = (*set)["dataSource"].value().toCString();

		formPart()->dataSourcePage()->setDataSource(dataSourceMimeType, dataSource);
	}
}

void
KexiFormView::slotHandleDragMoveEvent(QDragMoveEvent* e)
{
	if (KexiFieldDrag::canDecodeMultiple( e )) {
		e->accept(true);
		//dirty:	drawRect(QRect( e->pos(), QSize(50, 20)), 2);
	}
}

void
KexiFormView::slotHandleDropEvent(QDropEvent* e)
{
	if (KexiFieldDrag::canDecodeMultiple( e )) {
		QString sourceMimeType, sourceName;
		QStringList fields;
		if (!KexiFieldDrag::decodeMultiple( e, sourceMimeType, sourceName, fields ))
			return;
		insertAutoFields(sourceMimeType, sourceName, fields, e->pos());
	}
}

void
KexiFormView::insertAutoFields(const QString& sourceMimeType, const QString& sourceName,
	const QStringList& fields, const QPoint& _pos)
{
	if (fields.isEmpty())
		return;

	KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
	KexiDB::TableOrQuerySchema tableOrQuery(conn, sourceName.latin1(), sourceMimeType=="kexi/table");
	if (!tableOrQuery.table() && !tableOrQuery.query()) {
		kexipluginswarn << "KexiFormView::insertAutoFields(): no such table/query \""
			<< sourceName << "\"" << endl;
		return;
	}

	QPoint pos(_pos);
	//if pos is not specified, compute a new position:
	if (pos==QPoint(-1,-1)) {
		if (m_widgetGeometryForRecentInsertAutoFields.isValid()) {
			pos = m_widgetGeometryForRecentInsertAutoFields.bottomLeft() 
				+ QPoint(0,form()->gridSize());
		}
		else {
			pos = QPoint(40, 40); //start here
		}
	}

	// there will be many actions performed, do not update property pane until all that's finished
	KFormDesigner::FormManager::self()->blockPropertyEditorUpdating(this);

//! todo unnamed query colums are not supported

//		KFormDesigner::WidgetList* prevSelection = form()->selectedWidgets();
	KFormDesigner::WidgetList widgetsToSelect;
	KFormDesigner::CommandGroup *group = new KFormDesigner::CommandGroup(
		fields.count()==1 ? i18n("Insert AutoField widget") : i18n("Insert %1 AutoField widgets").arg(fields.count()),
		KFormDesigner::FormManager::self()->propertySet()
	);
	
	foreach( QStringList::ConstIterator, it, fields ) {
		KexiDB::QueryColumnInfo* column = tableOrQuery.columnInfo(*it);
		if (!column) {
			kexipluginswarn << "KexiFormView::insertAutoFields(): no such field \""
				<< *it << "\" in table/query \"" << sourceName << "\"" << endl;
			continue;
		}
//! todo add autolabel using field's caption or name
		KFormDesigner::InsertWidgetCommand *insertCmd
			= new KFormDesigner::InsertWidgetCommand(form()->toplevelContainer(),
	//! todo this is hardcoded!
				"KexiDBAutoField",
	//! todo this name can be invalid for expressions: if so, fall back to a default class' prefix!
			pos, column->aliasOrName()
		);
		insertCmd->execute();
		group->addCommand(insertCmd, false/*don't exec twice*/);

		KFormDesigner::ObjectTreeItem *newWidgetItem 
			= form()->objectTree()->dict()->find(insertCmd->widgetName());
		KexiDBAutoField* newWidget 
			= newWidgetItem ? dynamic_cast<KexiDBAutoField*>(newWidgetItem->widget()) : 0;
		widgetsToSelect.append(newWidget);
//#if 0
		KFormDesigner::CommandGroup *subGroup 
			= new KFormDesigner::CommandGroup("", KFormDesigner::FormManager::self()->propertySet());
		QMap<Q3CString, QVariant> propValues;
		propValues.insert("dataSource", column->aliasOrName());
		propValues.insert("fieldTypeInternal", (int)column->field->type());
		propValues.insert("fieldCaptionInternal", column->captionOrAliasOrName());
		KFormDesigner::FormManager::self()->propertySet()->createPropertyCommandsInDesignMode(
			newWidget, propValues, subGroup, false/*!addToActiveForm*/, 
			true /*!execFlagForSubCommands*/);
		subGroup->execute();
		group->addCommand( subGroup, false/*will not be executed on CommandGroup::execute()*/ );

//#endif
		//set data source and caption
		//-we don't need to use PropertyCommand here beacause we don't need UNDO 
		// for these single commands
//		newWidget->setDataSource(column->aliasOrName());
//		newWidget->setFieldTypeInternal((int)column->field->type());
//		newWidget->setFieldCaptionInternal(column->captionOrAliasOrName());
		//resize again because autofield's type changed what can lead to changed sizeHint() 
//		newWidget->resize(newWidget->sizeHint());
		KFormDesigner::WidgetList list;
		list.append(newWidget);
		KFormDesigner::AdjustSizeCommand *adjustCommand 
			=	new KFormDesigner::AdjustSizeCommand(KFormDesigner::AdjustSizeCommand::SizeToFit,
				list, form());
		adjustCommand->execute();
		group->addCommand( adjustCommand,
			false/*will not be executed on CommandGroup::execute()*/
		);

		if (newWidget) {//move position down for next widget
			pos.setY( pos.y() + newWidget->height() + form()->gridSize());
		}
	}
	if (widgetsToSelect.last()) {
		//resize form if needed
		QRect oldFormRect( m_dbform->geometry() );
		QRect newFormRect( oldFormRect );
		newFormRect.setWidth(qMax(m_dbform->width(), widgetsToSelect.last()->geometry().right()+1));
		newFormRect.setHeight(qMax(m_dbform->height(), widgetsToSelect.last()->geometry().bottom()+1));
		if (newFormRect != oldFormRect) {
			//1. resize by hand
			m_dbform->setGeometry( newFormRect );
			//2. store information about resize
			KFormDesigner::PropertyCommand *resizeFormCommand = new KFormDesigner::PropertyCommand(
				KFormDesigner::FormManager::self()->propertySet(), m_dbform->name(),
				oldFormRect, newFormRect, "geometry"); 
			group->addCommand(resizeFormCommand, true/*will be executed on CommandGroup::execute()*/);
		}

		//remember geometry of the last inserted widget
		m_widgetGeometryForRecentInsertAutoFields = widgetsToSelect.last()->geometry();
	}

	//eventually, add entire command group to active form
	form()->addCommand( group, true/*exec*/ );

//	group->debug();

	//enable proper REDO usage
	group->resetAllowExecuteFlags();

	m_scrollView->repaint();
	m_scrollView->viewport()->repaint();
	m_scrollView->repaintContents();
	m_scrollView->updateContents();
	m_scrollView->clipper()->repaint();
	m_scrollView->refreshContentsSize();

	//select all inserted widgets, if multiple
	if (widgetsToSelect.count()>1) {
		form()->setSelectedWidget(0);
		foreach_list (KFormDesigner::WidgetListIterator, it, widgetsToSelect)
			form()->setSelectedWidget(it.current(), true/*add*/, true/*dontRaise*/);
	}

	// eventually, update property pane
	KFormDesigner::FormManager::self()->unblockPropertyEditorUpdating(this, KFormDesigner::FormManager::self()->propertySet());
}

void
KexiFormView::setUnsavedLocalBLOB(QWidget *widget, KexiBLOBBuffer::Id_t id)
{
//! @todo if there already was data assigned, remember it should be dereferenced
	if (id==0) 
		m_unsavedLocalBLOBs.remove(widget);
	else
		m_unsavedLocalBLOBs.insert(widget, id);
}

/*
todo
void KexiFormView::updateActions(bool activated)
{
	if (viewMode()==Kexi::DesignViewMode) {
		if (form()->selectedWidget()) {
			if (form()->widget() == form()->selectedWidget())
				KFormDesigner::FormManager::self()->emitFormWidgetSelected( form() );
			else
				KFormDesigner::FormManager::self()->emitWidgetSelected( form(), false );
		}
		else if (form()->selectedWidgets()) {
			KFormDesigner::FormManager::self()->emitWidgetSelected( form(), true );
		}
	}
	KexiDataAwareView::updateActions(activated);
}*/

/*
void KexiFormView::parentDialogDetached()
{
	m_dbform->updateTabStopsOrder(form());
}

void KexiFormView::parentDialogAttached(KMdiChildFrm *)
{
	m_dbform->updateTabStopsOrder(form());
}*/

#include "kexiformview.moc"

