/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include <qlayout.h>
#include <qcursor.h>
#include <qobjectlist.h>

#include <kdebug.h>

#include <form.h>
#include <formIO.h>
#include <formmanager.h>
#include <objecttree.h>
#include <objpropbuffer.h>
#include <container.h>

#include <kexidialogbase.h>
#include <kexidatasourcewizard.h>
#include <kexidb/fieldlist.h>

#include "kexidbform.h"
#include "kexiformview.h"

#define NO_DSWIZARD

KexiFormScrollView::KexiFormScrollView(QWidget *parent, const char *name)
 : QScrollView(parent, name, WStaticContents), m_widget(0), m_form(0)
{
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	viewport()->setPaletteBackgroundColor(colorGroup().mid());

	setFocusPolicy(WheelFocus);
	setResizePolicy(Manual);

	viewport()->setMouseTracking(true);
	m_resizing = false;
	m_enableResizing = true;
	m_snapToGrid = false;
	m_gridX = 0;
	m_gridY = 0;
}

void
KexiFormScrollView::setWidget(QWidget *w)
{
	addChild(w);
	m_widget = w;
}

void
KexiFormScrollView::refreshContentsSize()
{
	// Ensure there is always space to resize Form
	if(m_widget->width() + 200 > contentsWidth())
		resizeContents(m_widget->width() + 300, contentsHeight());
	if(m_widget->height() + 200 > contentsHeight())
		resizeContents(contentsWidth(), m_widget->height() + 300);
}

void
KexiFormScrollView::contentsMousePressEvent(QMouseEvent *ev)
{
	if(!m_widget)
		return;

	QRect r3(0, 0, m_widget->width() + 4, m_widget->height() + 4);
	if(!r3.contains(ev->pos())) // clicked outside form
		m_form->resetSelection();

	if(!m_enableResizing)
		return;

	QRect r(m_widget->width(),  0, 4, m_widget->height() + 4); // right limit
	QRect r2(0, m_widget->height(), m_widget->width() + 4, 4); // bottom limit
	if(r.contains(ev->pos()) || r2.contains(ev->pos()))
	{
		m_resizing = true;
		m_snapToGrid = m_form->manager()->snapWidgetsToGrid();
		m_gridX = m_form->gridY();
		m_gridY = m_form->gridY();
	}
}

void
KexiFormScrollView::contentsMouseReleaseEvent(QMouseEvent *)
{
	if(m_resizing)
		m_resizing = false;
	unsetCursor();
}

void
KexiFormScrollView::contentsMouseMoveEvent(QMouseEvent *ev)
{
	if(!m_widget || !m_enableResizing)
		return;

	if(m_resizing) // resize widget
	{
		int tmpx = ev->x(), tmpy = ev->y();
		// we look for the max widget right() (or bottom()), which would be the limit for form resizing (not to hide widgets)
		QObjectList *list = m_widget->queryList("QWidget", 0, true, false /* not recursive*/);
		for(QObject *o = list->first(); o; o = list->next())
		{
			QWidget *w = (QWidget*)o;
			tmpx = QMAX(tmpx, (w->geometry().right() + 10));
			tmpy = QMAX(tmpy, (w->geometry().bottom() + 10));
		}
		delete list;

		if(cursor().shape() == QCursor::SizeHorCursor)
		{
			if(m_snapToGrid)
				m_widget->resize( int( float(tmpx) / float(m_gridX) + 0.5 ) * m_gridX, m_widget->height());
			else
				m_widget->resize(tmpx, m_widget->height());
		}
		else if(cursor().shape() == QCursor::SizeVerCursor)
		{
			if(m_snapToGrid)
				m_widget->resize(m_widget->width(), int( float(tmpy) / float(m_gridY) + 0.5 ) * m_gridY);
			else
				m_widget->resize(m_widget->width(), tmpy);
		}
		else if(cursor().shape() == QCursor::SizeFDiagCursor)
		{
			if(m_snapToGrid)
				m_widget->resize(int( float(tmpx) / float(m_gridX) + 0.5 ) * m_gridX,
				   int( float(tmpy) / float(m_gridY) + 0.5 ) * m_gridY);
			else
				m_widget->resize(tmpx, tmpy);
		}

		refreshContentsSize();
	}
	else // update mouse cursor
	{
		QPoint p = ev->pos();
		QRect r(m_widget->width(),  0, 4, m_widget->height()); // right
		QRect r2(0, m_widget->height(), m_widget->width(), 4); // bottom
		QRect r3(m_widget->width(), m_widget->height(), 4, 4); // bottom-right corner

		if(r.contains(p))
			setCursor(QCursor::SizeHorCursor);
		else if(r2.contains(p))
			setCursor(QCursor::SizeVerCursor);
		else if(r3.contains(p))
			setCursor(QCursor::SizeFDiagCursor);
		else
			unsetCursor();
	}
}

KexiFormScrollView::~KexiFormScrollView()
{}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

KexiFormView::KexiFormView(KexiMainWindow *win, QWidget *parent, const char *name, bool preview, KexiDB::Connection *conn)
 : KexiViewBase(win, parent, name), m_preview(preview), m_buffer(0), m_conn(conn)
{
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setAutoAdd(true);

	m_scrollView = new KexiFormScrollView(this);
	m_scrollView->show();

	m_dbform = new KexiDBForm(m_scrollView->viewport(), name/*, conn*/);
	m_dbform->resize(QSize(400, 300));
	m_scrollView->setWidget(m_dbform);
	m_scrollView->setResizingEnabled(!preview);

	initForm();

	if(!preview)
	{
		connect(formPart()->manager(), SIGNAL(bufferSwitched(KexiPropertyBuffer *)), this, SLOT(managerPropertyChanged(KexiPropertyBuffer *)));
		connect(formPart()->manager(), SIGNAL(dirty(KFormDesigner::Form *, bool)), this, SLOT(slotDirty(KFormDesigner::Form *, bool)));

		// action stuff
		connect(formPart()->manager(), SIGNAL(widgetSelected(Form*, bool)), SLOT(slotWidgetSelected(Form*, bool)));
		connect(formPart()->manager(), SIGNAL(formWidgetSelected(Form*)), SLOT(slotFormWidgetSelected(Form*)));
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
	else
		connect(formPart()->manager(), SIGNAL(noFormSelected()), SLOT(slotNoFormSelected()));

}

KFormDesigner::Form*
KexiFormView::form() const
{
	if(m_preview)
		return tempData()->previewForm;
	else
		return tempData()->form;
}

void
KexiFormView::setForm(KFormDesigner::Form *f)
{
	if(m_preview)
		tempData()->previewForm = f;
	else
		tempData()->form = f;
}

void
KexiFormView::initForm()
{
	setForm( new KFormDesigner::Form(formPart()->manager()) );
	form()->createToplevel(m_dbform, m_dbform);

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
	else
		fields = 0;

	if(fields)
	{
		QDomDocument dom;
		formPart()->generateForm(fields, dom);
		KFormDesigner::FormIO::loadFormFromDom(form(), m_dbform, dom);
	}
	else
		loadForm();

	formPart()->manager()->importForm(form(), m_preview);
	m_scrollView->setForm(form());
	m_scrollView->refreshContentsSize();
}

void
KexiFormView::loadForm()
{
	kdDebug() << "KexiDBForm::loadForm() Loading the form with id : " << parentDialog()->id() << endl;
	// If we are previewing the Form, use the tempData instead of the form stored in the db
	if(m_preview && !tempData()->tempForm.isNull() )
	{
		KFormDesigner::FormIO::loadFormFromString(form(), m_dbform, tempData()->tempForm);
		return;
	}

	// normal load
	QString data;
	loadDataBlock(data);
	KFormDesigner::FormIO::loadFormFromString(form(), m_dbform, data);
}

void
KexiFormView::managerPropertyChanged(KexiPropertyBuffer *b)
{
	m_buffer = b;
	propertyBufferSwitched();
}

bool
KexiFormView::beforeSwitchTo(int mode, bool &, bool &dontStore)
{
	// we don't store on db, but in our TempData
	dontStore = true;
	if(dirty() && (mode == Kexi::DataViewMode) && form()->objectTree())
		KFormDesigner::FormIO::saveFormToString(form(), tempData()->tempForm);

	return true;
}

bool
KexiFormView::afterSwitchFrom(int mode, bool &)
{
	if((mode == Kexi::DesignViewMode) && m_preview) //aka !preview
	{
		// The form may have been modified, so we must recreate the preview
		delete m_dbform; // also deletes form()
		m_dbform = new KexiDBForm(m_scrollView->viewport());
		m_scrollView->setWidget(m_dbform);

		initForm();
		slotNoFormSelected();
	}

	return true;
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
	kdDebug() << "KexiDBForm::storeNewData(): new id:" << s->id() << endl;

	storeData(cancel);
	return s;
}

bool
KexiFormView::storeData(bool &)
{
	kdDebug(44000) << "KexiDBForm::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;
	QString data;
	KFormDesigner::FormIO::saveFormToString(tempData()->form, data);
	storeDataBlock(data);
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

KexiFormView::~KexiFormView()
{}

#include "kexiformview.moc"

