/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexiformview.h"

#include <qobjectlist.h>

#include <formeditor/form.h>
#include <formeditor/formIO.h>
#include <formeditor/formmanager.h>
#include <formeditor/objecttree.h>
#include <formeditor/objpropbuffer.h>
#include <formeditor/container.h>

#include <kexidialogbase.h>
#include <kexidatasourcewizard.h>
#include <kexidb/fieldlist.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <tableview/kexitableitem.h>
#include <tableview/kexitableviewdata.h>

#include "kexidbform.h"
#include "kexiformscrollview.h"

#define NO_DSWIZARD

KexiFormView::KexiFormView(KexiMainWindow *mainWin, QWidget *parent, 
	const char *name, bool /*dbAware*/)
 : KexiDataAwareView( mainWin, parent, name )
 , m_buffer(0)
 , m_resizeMode(KexiFormView::ResizeDefault)
 , m_query(0)
 , m_queryIsOwned(false)
 , m_cursor(0)
// , m_firstFocusWidget(0)
{
	m_delayedFormContentsResizeOnShow = false;

	QHBoxLayout *l = new QHBoxLayout(this);
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
		connect(formPart()->manager(), SIGNAL(noFormSelected()), SLOT(slotNoFormSelected()));
	}
	else
	{
		connect(formPart()->manager(), SIGNAL(bufferSwitched(KexiPropertyBuffer *)), 
			this, SLOT(managerPropertyChanged(KexiPropertyBuffer *)));
		connect(formPart()->manager(), SIGNAL(dirty(KFormDesigner::Form *, bool)), 
			this, SLOT(slotDirty(KFormDesigner::Form *, bool)));

		// action stuff
		connect(formPart()->manager(), SIGNAL(widgetSelected(KFormDesigner::Form*, bool)), this, SLOT(slotWidgetSelected(KFormDesigner::Form*, bool)));
		connect(formPart()->manager(), SIGNAL(formWidgetSelected(KFormDesigner::Form*)), this, SLOT(slotFormWidgetSelected(KFormDesigner::Form*)));
		connect(formPart()->manager(), SIGNAL(undoEnabled(bool, const QString&)), this, SLOT(setUndoEnabled(bool)));
		connect(formPart()->manager(), SIGNAL(redoEnabled(bool, const QString&)), this, SLOT(setRedoEnabled(bool)));

		plugSharedAction("formpart_taborder", formPart()->manager(), SLOT(editTabOrder()));
		plugSharedAction("formpart_adjust_size", formPart()->manager(), SLOT(adjustWidgetSize()));
		plugSharedAction("formpart_pixmap_collection", formPart()->manager(), SLOT(editFormPixmapCollection()));
		plugSharedAction("formpart_connections", formPart()->manager(), SLOT(editConnections()));

		plugSharedAction("edit_copy", formPart()->manager(), SLOT(copyWidget()));
		plugSharedAction("edit_cut", formPart()->manager(), SLOT(cutWidget()));
		plugSharedAction("edit_paste", formPart()->manager(), SLOT(pasteWidget()));
		plugSharedAction("edit_delete", formPart()->manager(), SLOT(deleteWidget()));
		plugSharedAction("edit_select_all", formPart()->manager(), SLOT(selectAll()));
		plugSharedAction("formpart_clear_contents", formPart()->manager(), SLOT(clearWidgetContent()));
		plugSharedAction("edit_undo", formPart()->manager(), SLOT(undo()));
		plugSharedAction("edit_redo", formPart()->manager(), SLOT(redo()));

		plugSharedAction("formpart_layout_hbox", formPart()->manager(), SLOT(layoutHBox()) );
		plugSharedAction("formpart_layout_vbox", formPart()->manager(), SLOT(layoutVBox()) );
		plugSharedAction("formpart_layout_grid", formPart()->manager(), SLOT(layoutGrid()) );
		plugSharedAction("formpart_break_layout", formPart()->manager(), SLOT(breakLayout()) );

		plugSharedAction("formpart_format_raise", formPart()->manager(), SLOT(bringWidgetToFront()) );
		plugSharedAction("formpart_format_lower", formPart()->manager(), SLOT(sendWidgetToBack()) );

		plugSharedAction("formpart_align_menu", formPart()->manager(), 0 );
		plugSharedAction("formpart_align_to_left", formPart()->manager(),SLOT(alignWidgetsToLeft()) );
		plugSharedAction("formpart_align_to_right", formPart()->manager(), SLOT(alignWidgetsToRight()) );
		plugSharedAction("formpart_align_to_top", formPart()->manager(), SLOT(alignWidgetsToTop()) );
		plugSharedAction("formpart_align_to_bottom", formPart()->manager(), SLOT(alignWidgetsToBottom()) );
		plugSharedAction("formpart_align_to_grid", formPart()->manager(), SLOT(alignWidgetsToGrid()) );

		plugSharedAction("formpart_adjust_size_menu", formPart()->manager(), 0 );
		plugSharedAction("formpart_adjust_to_fit", formPart()->manager(), SLOT(adjustWidgetSize()) );
		plugSharedAction("formpart_adjust_size_grid", formPart()->manager(), SLOT(adjustSizeToGrid()) );
		plugSharedAction("formpart_adjust_height_small", formPart()->manager(),  SLOT(adjustHeightToSmall()) );
		plugSharedAction("formpart_adjust_height_big", formPart()->manager(), SLOT(adjustHeightToBig()) );
		plugSharedAction("formpart_adjust_width_small", formPart()->manager(), SLOT(adjustWidthToSmall()) );
		plugSharedAction("formpart_adjust_width_big", formPart()->manager(), SLOT(adjustWidthToBig()) );
	}

	initForm();

	KexiDataAwareView::init( m_scrollView, m_scrollView, m_scrollView );

	/// @todo skip this if ther're no borders
//	m_dbform->resize( m_dbform->size()+QSize(m_scrollView->verticalScrollBar()->width(), m_scrollView->horizontalScrollBar()->height()) );
}

KexiFormView::~KexiFormView()
{
//	if (m_cursor)
//		m_conn->deleteCursor(m_cursor);
//	delete m_provider;
	if (m_queryIsOwned)
		delete m_query;
	KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
	conn->deleteCursor(m_cursor);
//	delete m_data;
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
	setForm( new KFormDesigner::Form(formPart()->manager()) );
	form()->createToplevel(m_dbform, m_dbform);

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
	}
	else
		loadForm();

	if(form()->autoTabStops())
		form()->autoAssignTabStops();

	//collect tab order information
	m_dbform->updateTabStopsOrder(form());

//	if (m_dbform->orderedFocusWidgets()->first())
	//	m_scrollView->setFocusProxy( m_dbform->orderedFocusWidgets()->first() );

	formPart()->manager()->importForm(form(), viewMode()==Kexi::DataViewMode);
	m_scrollView->setForm(form());

//	m_dbform->updateTabStopsOrder(form());
//	QSize s = m_dbform->size();
//	QApplication::sendPostedEvents();
//	m_scrollView->resize( s );
//	m_dbform->resize(s);
	m_scrollView->refreshContentsSize();

	if (newForm && !fields) {
		/* Our form's area will be resized more than once. 
		Let's resize form widget itself later. */
		m_delayedFormContentsResizeOnShow = true;
	}
}

void
KexiFormView::loadForm()
{

//@todo also load m_resizeMode !

	kexipluginsdbg << "KexiDBForm::loadForm() Loading the form with id : " << parentDialog()->id() << endl;
	// If we are previewing the Form, use the tempData instead of the form stored in the db
	if(viewMode()==Kexi::DataViewMode && !tempData()->tempForm.isNull() )
	{
		KFormDesigner::FormIO::loadFormFromString(form(), m_dbform, tempData()->tempForm);
		return;
	}

	// normal load
	QString data;
	loadDataBlock(data);
	KFormDesigner::FormIO::loadFormFromString(form(), m_dbform, data);

	//"autoTabStops" property is loaded -set it within the form tree as well
	form()->setAutoTabStops( m_dbform->autoTabStops() );
}

void
KexiFormView::managerPropertyChanged(KexiPropertyBuffer *b)
{
	m_buffer = b;
	propertyBufferSwitched();
}

tristate
KexiFormView::beforeSwitchTo(int mode, bool &dontStore)
{
	if (mode!=viewMode()) {
		if (viewMode()!=Kexi::DataViewMode) {
		//remember our pos
		tempData()->scrollViewContentsPos 
			= QPoint(m_scrollView->contentsX(), m_scrollView->contentsY());
		}
	}

	// we don't store on db, but in our TempData
	dontStore = true;
	if(dirty() && (mode == Kexi::DataViewMode) && form()->objectTree()) {
		KFormDesigner::FormIO::saveFormToString(form(), tempData()->tempForm);
	}

	return true;
}

tristate
KexiFormView::afterSwitchFrom(int mode)
{
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
		slotNoFormSelected();

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

		//set focus on 1st focusable widget
//		if (form()->tabStops()->first() && form()->tabStops()->first()->widget())
//			form()->tabStops()->first()->widget()->setFocus();
		if (m_dbform->orderedFocusWidgets()->first())
			SET_FOCUS_USING_REASON(m_dbform->orderedFocusWidgets()->first(), QFocusEvent::Tab);
//			m_dbform->orderedFocusWidgets()->first()->setFocus();
	}
	return true;
}

void KexiFormView::initDataSource()
{
	QString dataSourceString = m_dbform->dataSource();
	if (dataSourceString.isEmpty())
		return;
/*			if (m_previousDataSourceString.lower()==dataSourceString.lower() && !m_cursor) {
			//data source changed: delete previous cursor
			m_conn->deleteCursor(m_cursor);
			m_cursor = 0;
		}*/
	m_previousDataSourceString = dataSourceString;
	bool ok = true;
	//collect all data-aware widgets and create query schema
//	if (!m_provider)
//		m_provider = new KexiDataProvider();
	m_scrollView->setMainWidget(m_dbform);
//			if (m_cursor)
//				m_conn->deleteCursor(m_cursor);
//	KexiDB::Cursor *cursor = 0;
	if (m_queryIsOwned)
		delete m_query;
	m_query = new KexiDB::QuerySchema();
	QStringList sources( m_scrollView->usedDataSources() );
	KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
	KexiDB::TableSchema *tableSchema = conn->tableSchema( dataSourceString );
	ok = tableSchema != 0;
	if (ok) {
		QValueList<uint> invalidSources;
		uint index = 0;
		for (QStringList::ConstIterator it = sources.constBegin(); 
			it!=sources.constEnd(); ++it, index++) {
/*! @todo add expression support */
			KexiDB::Field *f = tableSchema->field(*it);
			if (!f) {
/*! @todo show error */
				//remove this widget from the set of data widgets in the provider
				invalidSources += index;
				continue;
			}
			m_query->addField( f );
		}
		if (invalidSources.count()==sources.count()) {
			//all data sources are invalid! don't execute the query
			if (m_queryIsOwned)
				delete m_query;
			m_query = 0;
		}
		else {
			m_cursor = conn->executeQuery( *m_query );
		}
		m_scrollView->invalidateDataSources( invalidSources, m_query );
		ok = cursor!=0;
	}
//			delete m_data;
	if (ok) {
//! @todo PRIMITIVE!! data setting:
//! @todo KexiTableViewData is not great name for data class here... rename/move?
		KexiTableViewData* data = new KexiTableViewData(m_cursor);
		data->preloadAllRows();
//not needed here: cursor will be owned		conn->deleteCursor(cursor);

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
KexiFormView::storeData()
{
	kexipluginsdbg << "KexiDBForm::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;
	QString data;
	KFormDesigner::FormIO::saveFormToString(tempData()->form, data);
	if (!storeDataBlock(data))
		return false;
	tempData()->tempForm = QString();

	return true;
}


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
	setAvailable("formpart_break_layout", (container->layoutType() != KFormDesigner::Container::NoLayout));
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

	setAvailable("edit_paste", formPart()->manager()->isPasteEnabled());
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

QSize
KexiFormView::preferredSizeHint(const QSize& otherSize)
{
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
	if (m_delayedFormContentsResizeOnShow && isVisible()) {
		m_delayedFormContentsResizeOnShow = false;
		m_dbform->resize( e->size() - QSize(20, 20) );
	}
}

void 
KexiFormView::setFocusInternal()
{
	if (viewMode() == Kexi::DataViewMode) {
		if (m_dbform->focusWidget()) {
			//better-looking focus
			SET_FOCUS_USING_REASON(m_dbform->focusWidget(), QFocusEvent::Tab);
			return;
		}
	}
	QWidget::setFocus();
}


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

