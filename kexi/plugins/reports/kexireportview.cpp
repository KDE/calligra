/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
#include <form.h>
#include <formIO.h>
#include <formmanager.h>
#include <objecttree.h>
#include <objpropbuffer.h>
#include <container.h>

#include <kexidialogbase.h>
#include <kexidatasourcewizard.h>
#include <kexidb/fieldlist.h>
#include <kexidb/connection.h>

#include "kexireportform.h"
#include "kexireportview.h"

#define NO_DSWIZARD

KexiReportScrollView::KexiReportScrollView(QWidget *parent, bool preview)
 : KexiScrollView(parent, preview)
{
	if(preview)
		setRecordNavigatorVisible(true);
	connect(this, SIGNAL(resizingStarted()), this, SLOT(slotResizingStarted()));
}

KexiReportScrollView::~KexiReportScrollView()
{
}

void
KexiReportScrollView::show()
{
	KexiScrollView::show();

	//now get resize mode settings for entire form
	if (m_preview) {
		KexiReportView* fv = dynamic_cast<KexiReportView*>(parent());
		int resizeMode = fv ? fv->resizeMode() : KexiReportView::ResizeAuto;
		if (resizeMode == KexiReportView::ResizeAuto)
			setResizePolicy(AutoOneFit);
	}
}

void
KexiReportScrollView::slotResizingStarted()
{
	if(m_form && m_form->manager())
		setSnapToGrid(m_form->manager()->snapWidgetsToGrid(), m_form->gridX(), m_form->gridY());
	else
		setSnapToGrid(false);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

KexiReportView::KexiReportView(KexiMainWindow *win, QWidget *parent, const char *name,
	KexiDB::Connection *conn)
 : KexiViewBase(win, parent, name), m_buffer(0), m_conn(conn)
 , m_resizeMode(KexiReportView::ResizeDefault)
{
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setAutoAdd(true);

	m_scrollView = new KexiReportScrollView(this, viewMode()==Kexi::DataViewMode);
	setViewWidget(m_scrollView);
//	m_scrollView->show();

	m_reportform = new KexiReportForm(m_scrollView->viewport(), name/*, conn*/);
//	m_reportform->resize(QSize(400, 300));
	m_scrollView->setWidget(m_reportform);
	m_scrollView->setResizingEnabled(viewMode()!=Kexi::DataViewMode);

//	initForm();

	if (viewMode()==Kexi::DataViewMode) {
		m_scrollView->viewport()->setPaletteBackgroundColor(m_reportform->palette().active().background());
		connect(reportPart()->manager(), SIGNAL(noFormSelected()), SLOT(slotNoFormSelected()));
	}
	else {
		connect(reportPart()->manager(), SIGNAL(bufferSwitched(KexiPropertyBuffer *)),
			this, SLOT(managerPropertyChanged(KexiPropertyBuffer *)));
		connect(reportPart()->manager(), SIGNAL(dirty(KFormDesigner::Form *, bool)),
			this, SLOT(slotDirty(KFormDesigner::Form *, bool)));

		// action stuff
		connect(reportPart()->manager(), SIGNAL(widgetSelected(Form*, bool)), SLOT(slotWidgetSelected(Form*, bool)));
		connect(reportPart()->manager(), SIGNAL(formWidgetSelected(Form*)), SLOT(slotFormWidgetSelected(Form*)));
		connect(reportPart()->manager(), SIGNAL(undoEnabled(bool, const QString&)), this, SLOT(setUndoEnabled(bool)));
		connect(reportPart()->manager(), SIGNAL(redoEnabled(bool, const QString&)), this, SLOT(setRedoEnabled(bool)));

		plugSharedAction("edit_copy", reportPart()->manager(), SLOT(copyWidget()));
		plugSharedAction("edit_cut", reportPart()->manager(), SLOT(cutWidget()));
		plugSharedAction("edit_paste", reportPart()->manager(), SLOT(pasteWidget()));
		plugSharedAction("edit_delete", reportPart()->manager(), SLOT(deleteWidget()));
		plugSharedAction("edit_select_all", reportPart()->manager(), SLOT(selectAll()));
		plugSharedAction("reportpart_clear_contents", reportPart()->manager(), SLOT(clearWidgetContent()));
		plugSharedAction("edit_undo", reportPart()->manager(), SLOT(undo()));
		plugSharedAction("edit_redo", reportPart()->manager(), SLOT(redo()));

		plugSharedAction("reportpart_format_raise", reportPart()->manager(), SLOT(bringWidgetToFront()) );
		plugSharedAction("reportpart_format_lower", reportPart()->manager(), SLOT(sendWidgetToBack()) );

		plugSharedAction("reportpart_align_menu", reportPart()->manager(), 0 );
		plugSharedAction("reportpart_align_to_left", reportPart()->manager(),SLOT(alignWidgetsToLeft()) );
		plugSharedAction("reportpart_align_to_right", reportPart()->manager(), SLOT(alignWidgetsToRight()) );
		plugSharedAction("reportpart_align_to_top", reportPart()->manager(), SLOT(alignWidgetsToTop()) );
		plugSharedAction("reportpart_align_to_bottom", reportPart()->manager(), SLOT(alignWidgetsToBottom()) );
		plugSharedAction("reportpart_align_to_grid", reportPart()->manager(), SLOT(alignWidgetsToGrid()) );

		plugSharedAction("reportpart_adjust_size_menu", reportPart()->manager(), 0 );
		plugSharedAction("reportpart_adjust_to_fit", reportPart()->manager(), SLOT(adjustWidgetSize()) );
		plugSharedAction("reportpart_adjust_size_grid", reportPart()->manager(), SLOT(adjustSizeToGrid()) );
		plugSharedAction("reportpart_adjust_height_small", reportPart()->manager(),  SLOT(adjustHeightToSmall()) );
		plugSharedAction("reportpart_adjust_height_big", reportPart()->manager(), SLOT(adjustHeightToBig()) );
		plugSharedAction("reportpart_adjust_width_small", reportPart()->manager(), SLOT(adjustWidthToSmall()) );
		plugSharedAction("reportpart_adjust_width_big", reportPart()->manager(), SLOT(adjustWidthToBig()) );
	}

	initForm();

	/// @todo skip this if ther're no borders
//	m_reportform->resize( m_reportform->size()+QSize(m_scrollView->verticalScrollBar()->width(), m_scrollView->horizontalScrollBar()->height()) );
}

KexiReportView::~KexiReportView()
{
}

KFormDesigner::Form*
KexiReportView::form() const
{
	if(viewMode()==Kexi::DataViewMode)
		return tempData()->previewForm;
	else
		return tempData()->form;
}

void
KexiReportView::setForm(KFormDesigner::Form *f)
{
	if(viewMode()==Kexi::DataViewMode)
		tempData()->previewForm = f;
	else
		tempData()->form = f;
}

void
KexiReportView::initForm()
{
	setForm( new KFormDesigner::Form(reportPart()->manager()) );
	form()->createToplevel(m_reportform, m_reportform);

	// Show the form wizard if this is a new Form
	KexiDB::FieldList *fields = 0;
	if(parentDialog()->id() < 0)
	{
#ifndef NO_DSWIZARD
		KexiDataSourceWizard *w = new KexiDataSourceWizard(mainWin(), (QWidget*)mainWin(), "datasource_wizard");
		if(!w->exec())
			fields = 0;
		else
			fields = w->fields();
		delete w;
#endif
	}

/*	if(fields)
	{
	 @todo generate a report from a table or a query
		QDomDocument dom;
		reportPart()->generateForm(fields, dom);
		KFormDesigner::FormIO::loadFormFromDom(form(), m_reportform, dom);
	}
	else*/
		loadForm();

	reportPart()->manager()->importForm(form(), viewMode()==Kexi::DataViewMode);
	m_scrollView->setForm(form());
	m_scrollView->refreshContentsSize();
}

void
KexiReportView::loadForm()
{

//@todo also load m_resizeMode !

	kexipluginsdbg << "KexiReportForm::loadForm() Loading the form with id : " << parentDialog()->id() << endl;
	// If we are previewing the Form, use the tempData instead of the form stored in the db
	if(viewMode()==Kexi::DataViewMode && !tempData()->tempForm.isNull() ) {
		KFormDesigner::FormIO::loadFormFromString(form(), m_reportform, tempData()->tempForm);
		return;
	}

	// normal load
	QString data;
	loadDataBlock(data);
	KFormDesigner::FormIO::loadFormFromString(form(), m_reportform, data);
}

void
KexiReportView::managerPropertyChanged(KexiPropertyBuffer *b)
{
	m_buffer = b;
	propertyBufferSwitched();
}

tristate
KexiReportView::beforeSwitchTo(int mode, bool &dontStore)
{
	if (mode!=viewMode() && viewMode()!=Kexi::DataViewMode) {
		//remember our pos
		tempData()->scrollViewContentsPos
			= QPoint(m_scrollView->contentsX(), m_scrollView->contentsY());
	}

	// we don't store on db, but in our TempData
	dontStore = true;
	if(dirty() && (mode == Kexi::DataViewMode) && form()->objectTree())
		KFormDesigner::FormIO::saveFormToString(form(), tempData()->tempForm);

	return true;
}

tristate
KexiReportView::afterSwitchFrom(int mode)
{
	if (mode != 0 && mode != Kexi::DesignViewMode) {
		//preserve contents pos after switching to other view
		m_scrollView->setContentsPos(tempData()->scrollViewContentsPos.x(),
			tempData()->scrollViewContentsPos.y());
	}
//	if (mode == Kexi::DesignViewMode) {
		//m_scrollView->move(0,0);
		//m_scrollView->setContentsPos(0,0);
		//m_scrollView->moveChild(m_reportform, 0, 0);
//	}

	if((mode == Kexi::DesignViewMode) && viewMode()==Kexi::DataViewMode) {
		// The form may have been modified, so we must recreate the preview
		delete m_reportform; // also deletes form()
		m_reportform = new KexiReportForm(m_scrollView->viewport());
		m_scrollView->setWidget(m_reportform);

		initForm();
		slotNoFormSelected();

		//reset position
		m_scrollView->setContentsPos(0,0);
		m_reportform->move(0,0);
	}
	return true;
}

void
KexiReportView::slotDirty(KFormDesigner::Form *dirtyForm, bool isDirty)
{
	if(dirtyForm == form())
		KexiViewBase::setDirty(isDirty);
}

KexiDB::SchemaData*
KexiReportView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
	KexiDB::SchemaData *s = KexiViewBase::storeNewData(sdata, cancel);
	kexipluginsdbg << "KexiReportForm::storeNewData(): new id:" << s->id() << endl;

	if (!s || cancel) {
		delete s;
		return 0;
	}
	if (!storeData()) {
		//failure: remove object's schema data to avoid garbage
		m_conn->removeObject( s->id() );
		delete s;
		return 0;
	}
	return s;
}

tristate
KexiReportView::storeData()
{
	kexipluginsdbg << "KexiReportForm::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;
	QString data;
	KFormDesigner::FormIO::saveFormToString(tempData()->form, data);
	if (!storeDataBlock(data))
		return false;
	tempData()->tempForm = QString();

	return true;
}


/// Action stuff /////////////////
void
KexiReportView::slotWidgetSelected(KFormDesigner::Form *f, bool multiple)
{
	if(f != form())
		return;

	enableFormActions();
	// Enable edit actions
	setAvailable("edit_copy", true);
	setAvailable("edit_cut", true);
	setAvailable("edit_clear", true);

	// 'Align Widgets' menu
	setAvailable("reportpart_align_menu", multiple);
	setAvailable("reportpart_align_to_left", multiple);
	setAvailable("reportpart_align_to_right", multiple);
	setAvailable("reportpart_align_to_top", multiple);
	setAvailable("reportpart_align_to_bottom", multiple);

	setAvailable("reportpart_adjust_size_menu", true);
	setAvailable("reportpart_adjust_width_small", multiple);
	setAvailable("reportpart_adjust_width_big", multiple);
	setAvailable("reportpart_adjust_height_small", multiple);
	setAvailable("reportpart_adjust_height_big", multiple);

	setAvailable("reportpart_format_raise", true);
	setAvailable("reportpart_format_lower", true);
}

void
KexiReportView::slotFormWidgetSelected(KFormDesigner::Form *f)
{
	if(f != form())
		return;

	disableWidgetActions();
	enableFormActions();
}

void
KexiReportView::slotNoFormSelected() // == form in preview mode
{
	disableWidgetActions();

	// Disable paste action
	setAvailable("edit_paste", false);
	setAvailable("edit_undo", false);
	setAvailable("edit_redo", false);
}

void
KexiReportView::enableFormActions()
{
	setAvailable("edit_paste", reportPart()->manager()->isPasteEnabled());
}

void
KexiReportView::disableWidgetActions()
{
	// Disable edit actions
	setAvailable("edit_copy", false);
	setAvailable("edit_cut", false);
	setAvailable("edit_clear", false);

	// Disable format functions
	setAvailable("reportpart_align_menu", false);
	setAvailable("reportpart_align_to_left", false);
	setAvailable("reportpart_align_to_right", false);
	setAvailable("reportpart_align_to_top", false);
	setAvailable("reportpart_align_to_bottom", false);

	setAvailable("reportpart_adjust_size_menu", false);
	setAvailable("reportpart_adjust_width_small", false);
	setAvailable("reportpart_adjust_width_big", false);
	setAvailable("reportpart_adjust_height_small", false);
	setAvailable("reportpart_adjust_height_big", false);

	setAvailable("reportpart_format_raise", false);
	setAvailable("reportpart_format_lower", false);
}

void
KexiReportView::setUndoEnabled(bool enabled)
{
	setAvailable("edit_undo", enabled);
}

void
KexiReportView::setRedoEnabled(bool enabled)
{
	setAvailable("edit_redo", enabled);
}

QSize
KexiReportView::preferredSizeHint(const QSize& otherSize)
{
	return (m_reportform->size()
			+QSize(m_scrollView->verticalScrollBar()->isVisible() ? m_scrollView->verticalScrollBar()->width()*3/2 : 10,
			 m_scrollView->horizontalScrollBar()->isVisible() ? m_scrollView->horizontalScrollBar()->height()*3/2 : 10))
		.expandedTo( KexiViewBase::preferredSizeHint(otherSize) );
}

void
KexiReportView::resizeEvent( QResizeEvent *e )
{
	if (viewMode()==Kexi::DataViewMode) {
		m_scrollView->refreshContentsSizeLater(
			e->size().width()!=e->oldSize().width(),
			e->size().height()!=e->oldSize().height()
		);
	}
	KexiViewBase::resizeEvent(e);
	m_scrollView->updateNavPanelGeometry();
}

#include "kexireportview.moc"

