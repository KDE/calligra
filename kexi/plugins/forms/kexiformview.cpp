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

#include <qlayout.h>
#include <qcursor.h>
#include <qobjectlist.h>
#include <qpainter.h>

#include <kdebug.h>
#include <kstaticdeleter.h>

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

#include "kexidbform.h"
#include "kexiformview.h"

#define NO_DSWIZARD

// @todo warning: not reentrant!
static KStaticDeleter<QPixmap> KexiFormScrollView_bufferPm_deleter;
QPixmap* KexiFormScrollView_bufferPm = 0;

KexiFormScrollView::KexiFormScrollView(QWidget *parent, bool preview)
 : QScrollView(parent, "formpart_kexiformview", WStaticContents)
 , m_widget(0)
 , m_form(0)
 , m_helpFont(font())
 , m_preview(preview)
{
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	viewport()->setPaletteBackgroundColor(colorGroup().mid());
	QColor fc = palette().active().foreground(),
		bc = viewport()->paletteBackgroundColor();
	m_helpColor	= QColor((fc.red()+bc.red()*2)/3, (fc.green()+bc.green()*2)/3, 
		(fc.blue()+bc.blue()*2)/3);
	m_helpFont.setPointSize( m_helpFont.pointSize() * 3 );

	setFocusPolicy(WheelFocus);

	//initial resize mode is always manual;
	//will be changed on show(), if needed
	setResizePolicy(Manual);

	viewport()->setMouseTracking(true);
	m_resizing = false;
	m_enableResizing = true;
	m_snapToGrid = false;
	m_gridX = 0;
	m_gridY = 0;

	connect(&m_delayedResize, SIGNAL(timeout()), this, SLOT(refreshContentsSize()));
	m_smodeSet = false;
	if (m_preview)
		refreshContentsSizeLater(true, true);
}

KexiFormScrollView::~KexiFormScrollView()
{
}

void KexiFormScrollView::show()
{
	QScrollView::show();

	//now get resize mode settings for entire form
	if (m_preview) {
		KexiFormView* fv = dynamic_cast<KexiFormView*>(parent());
		int resizeMode = fv ? fv->resizeMode() : KexiFormView::ResizeAuto;
		if (resizeMode == KexiFormView::ResizeAuto)
			setResizePolicy(AutoOneFit);
	}
}

void
KexiFormScrollView::setFormWidget(KexiDBForm *w)
{
	addChild(w);
	m_widget = w;
}

void KexiFormScrollView::refreshContentsSizeLater(bool horizontal, bool vertical)
{
	if (!m_smodeSet) {
		m_smodeSet = true;
		m_vsmode = vScrollBarMode();
		m_hsmode = hScrollBarMode();
	}
//	if (vertical)
		setVScrollBarMode(QScrollView::AlwaysOff);
	//if (horizontal)
		setHScrollBarMode(QScrollView::AlwaysOff);
	updateScrollBars();
	m_delayedResize.start( 100, true );
}

void
KexiFormScrollView::refreshContentsSize()
{
	if(!m_widget)
		return;
	if (m_preview) {
		resizeContents(m_widget->width(), m_widget->height());
		setVScrollBarMode(m_vsmode);
		setHScrollBarMode(m_hsmode);
		m_smodeSet = false;
		updateScrollBars();
	}
	else {
		// Ensure there is always space to resize Form
		if(m_widget->width() + 200 > contentsWidth())
			resizeContents(m_widget->width() + 300, contentsHeight());
		if(m_widget->height() + 200 > contentsHeight())
			resizeContents(contentsWidth(), m_widget->height() + 300);
	}
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
		const int exceeds_x = (tmpx - contentsX() + 5) - clipper()->width();
		const int exceeds_y = (tmpy - contentsY() + 5) - clipper()->height();
		if (exceeds_x > 0)
			tmpx -= exceeds_x;
		if (exceeds_y > 0)
			tmpy -= exceeds_y;
		if ((tmpx - contentsX()) < 0)
			tmpx = contentsX();
		if ((tmpy - contentsY()) < 0)
			tmpy = contentsY();

		// we look for the max widget right() (or bottom()), which would be the limit for form resizing (not to hide widgets)
		QObjectList *list = m_widget->queryList("QWidget", 0, true, false /* not recursive*/);
		for(QObject *o = list->first(); o; o = list->next())
		{
			QWidget *w = (QWidget*)o;
			tmpx = QMAX(tmpx, (w->geometry().right() + 10));
			tmpy = QMAX(tmpy, (w->geometry().bottom() + 10));
		}
		delete list;

		int neww = -1, newh;
		if(cursor().shape() == QCursor::SizeHorCursor)
		{
			if(m_snapToGrid)
				neww = int( float(tmpx) / float(m_gridX) + 0.5 ) * m_gridX;
			else
				neww = tmpx;
			newh = m_widget->height();
		}
		else if(cursor().shape() == QCursor::SizeVerCursor)
		{
			neww = m_widget->width();
			if(m_snapToGrid)
				newh = int( float(tmpy) / float(m_gridY) + 0.5 ) * m_gridY;
			else
				newh = tmpy;
		}
		else if(cursor().shape() == QCursor::SizeFDiagCursor)
		{
			if(m_snapToGrid) {
				neww = int( float(tmpx) / float(m_gridX) + 0.5 ) * m_gridX;
				newh = int( float(tmpy) / float(m_gridY) + 0.5 ) * m_gridY;
			} else {
				neww = tmpx;
				newh = tmpy;
			}
		}
		//needs update?
		if (neww!=-1 && m_widget->size() != QSize(neww, newh)) {
			m_widget->resize( neww, newh );
			refreshContentsSize();
			updateContents();
		}
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

void
KexiFormScrollView::drawContents( QPainter * p, int clipx, int clipy, int clipw, int cliph ) 
{
	QScrollView::drawContents(p, clipx, clipy, clipw, cliph);
	if (m_widget) {
		if (m_preview)
			return;

		//draw right and bottom borders
		const int wx = childX(m_widget);
		const int wy = childY(m_widget);
		p->setPen(palette().active().foreground());
		p->drawLine(wx+m_widget->width(), wy, wx+m_widget->width(), wy+m_widget->height());
		p->drawLine(wx, wy+m_widget->height(), wx+m_widget->width(), wy+m_widget->height());


		if (!KexiFormScrollView_bufferPm) {
			//create flicker-less buffer
			QString txt(i18n("Outer area"));
			QFontMetrics fm(m_helpFont);
			const int txtw = fm.width(txt), txth = fm.height();
			KexiFormScrollView_bufferPm_deleter.setObject( KexiFormScrollView_bufferPm, 
				new QPixmap(txtw, txth) );
			if (!KexiFormScrollView_bufferPm->isNull()) {
				//create pixmap once
				KexiFormScrollView_bufferPm->fill( viewport()->paletteBackgroundColor() );
				QPainter *pb = new QPainter(KexiFormScrollView_bufferPm, this);
				pb->setPen(m_helpColor);
				pb->setFont(m_helpFont);
				pb->drawText(0, 0, txtw, txth, Qt::AlignLeft|Qt::AlignTop, txt);
				delete pb;
			}
		}
		if (!KexiFormScrollView_bufferPm->isNull()) {
			p->drawPixmap((contentsWidth() + m_widget->width() - KexiFormScrollView_bufferPm->width())/2,
				(m_widget->height()-KexiFormScrollView_bufferPm->height())/2, 
				*KexiFormScrollView_bufferPm);
			p->drawPixmap((m_widget->width() - KexiFormScrollView_bufferPm->width())/2,
				(contentsHeight() + m_widget->height() - KexiFormScrollView_bufferPm->height())/2, 
				*KexiFormScrollView_bufferPm);
		}
	}
}

void KexiFormScrollView::leaveEvent( QEvent *e )
{
	QWidget::leaveEvent(e);
	m_widget->update(); //update form elements on too fast mouse move
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

KexiFormView::KexiFormView(KexiMainWindow *win, QWidget *parent, const char *name, 
	bool preview, KexiDB::Connection *conn)
 : KexiViewBase(win, parent, name), m_buffer(0), m_conn(conn)
 , m_resizeMode(KexiFormView::ResizeDefault)
{
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setAutoAdd(true);

	m_scrollView = new KexiFormScrollView(this, viewMode()==Kexi::DataViewMode);
	setViewWidget(m_scrollView);
//	m_scrollView->show();

	m_dbform = new KexiDBForm(m_scrollView->viewport(), name/*, conn*/);
//	m_dbform->resize(QSize(400, 300));
	m_scrollView->setFormWidget(m_dbform);
	m_scrollView->setResizingEnabled(viewMode()!=Kexi::DataViewMode);

//	initForm();

	if (viewMode()==Kexi::DataViewMode) {
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

	/// @todo skip this if ther're no borders
//	m_dbform->resize( m_dbform->size()+QSize(m_scrollView->verticalScrollBar()->width(), m_scrollView->horizontalScrollBar()->height()) );
}

KexiFormView::~KexiFormView()
{
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

	formPart()->manager()->importForm(form(), viewMode()==Kexi::DataViewMode);
	m_scrollView->setForm(form());
//	QSize s = m_dbform->size();
//	QApplication::sendPostedEvents();
//	m_scrollView->resize( s );
//	m_dbform->resize(s);
	m_scrollView->refreshContentsSize();
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
	if (mode!=viewMode() && viewMode()!=Kexi::DataViewMode) {
		//remember our pos
		tempData()->scrollViewContentsPos 
			= QPoint(m_scrollView->contentsX(), m_scrollView->contentsY());
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
		m_dbform = new KexiDBForm(m_scrollView->viewport());
		m_scrollView->setFormWidget(m_dbform);

		initForm();
		slotNoFormSelected();

		//reset position
		m_scrollView->setContentsPos(0,0);
		m_dbform->move(0,0);
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
	kexipluginsdbg << "KexiDBForm::storeNewData(): new id:" << s->id() << endl;

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
}

#include "kexiformview.moc"

