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

#include <qworkspace.h>
#include <qdockarea.h>
#include <qdockwindow.h>
#include <qhbox.h>
#include <qpainter.h>
#include <qevent.h>
#include <qobjectlist.h>

#include <kdeversion.h>
#include <kaction.h>
#include <kinstance.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <klibloader.h>
#include <kmessagebox.h>

#include "form.h"
#include "formIO.h"
#include "objecttree.h"
#include "container.h"
#include "formmanager.h"
#include "objecttreeview.h"
#include "kexipropertyeditor.h"

#include "kfd_part.h"

#define ENABLE_ACTION(name, enable) \
	if(actionCollection()->action( name )) \
		actionCollection()->action( name )->setEnabled( enable )

KInstance *KFDFactory::m_instance = 0L;

KFDFactory::KFDFactory()
{}

KFDFactory::~KFDFactory()
{
	if (m_instance)
	{
		delete m_instance->aboutData();
		delete m_instance;
	}

	m_instance = 0;
}

KParts::Part*
KFDFactory::createPartObject( QWidget *parentWidget, const char *, QObject *, const char *name,
  const char *classname, const QStringList &args)
{
	bool readOnly = (QCString(classname) == "KParts::ReadOnlyPart");
	KFormDesignerPart *part = new KFormDesignerPart(parentWidget, name, readOnly);
	if(!args.grep("multipleMode").isEmpty())
		part->setUniqueFormMode(false);
	return part;
}

KInstance*
KFDFactory::instance()
{
	if (!m_instance)
		m_instance = new KInstance(aboutData());
	return m_instance;
}

KAboutData*
KFDFactory::aboutData()
{
	KAboutData *about = new KAboutData("kformdesigner_part", I18N_NOOP("Form Designer Part"), "0.3");
	return about;
}

KFormDesignerPart::KFormDesignerPart(QWidget *parent, const char *name, bool readOnly)
: KParts::ReadWritePart(parent, name), m_count(0)
{
	setInstance(KFDFactory::instance());
	instance()->iconLoader()->addAppDir("kexi");
	instance()->iconLoader()->addAppDir("kformdesigner");

	setReadWrite(!readOnly);
	m_uniqueFormMode = true;
	m_openingFile = false;

	QHBox *container = new QHBox(parent, "kfd_container_widget");

	m_workspace = new QWorkspace(container, "kfd_workspace");
	m_workspace->show();
	m_manager = new KFormDesigner::FormManager(this, "kfd_manager");

	if(!readOnly)
	{
		QDockArea *dockArea = new QDockArea(Vertical, QDockArea::Reverse, container, "kfd_part_dockarea");

		QDockWindow *dockTree = new QDockWindow(dockArea);
		KFormDesigner::ObjectTreeView *view = new KFormDesigner::ObjectTreeView(dockTree);
		dockTree->setWidget(view);
		dockTree->setCaption(i18n("Objects"));
		dockTree->setResizeEnabled(true);
		dockTree->setFixedExtentWidth(256);

		QDockWindow *dockEditor = new QDockWindow(dockArea);
		KexiPropertyEditor *editor = new KexiPropertyEditor(dockEditor);
		dockEditor->setWidget(editor);
		dockEditor->setCaption(i18n("Properties"));
		dockEditor->setResizeEnabled(true);

		m_manager->setEditors(editor, view);

		setupActions();
		setModified(false);

		// action stuff
		connect(m_manager, SIGNAL(widgetSelected(Form*, bool)), SLOT(slotWidgetSelected(Form*, bool)));
		connect(m_manager, SIGNAL(formWidgetSelected(Form*)), SLOT(slotFormWidgetSelected(Form*)));
		connect(m_manager, SIGNAL(noFormSelected()), SLOT(slotNoFormSelected()));
		connect(m_manager, SIGNAL(undoEnabled(bool, const QString&)), SLOT(setUndoEnabled(bool, const QString&)));
		connect(m_manager, SIGNAL(redoEnabled(bool, const QString&)), SLOT(setRedoEnabled(bool, const QString&)));

		connect(m_manager, SIGNAL(dirty(KFormDesigner::Form*, bool)), this, SLOT(slotFormModified(KFormDesigner::Form*, bool)));
	}

	container->show();
	setWidget(container);
	connect(m_workspace, SIGNAL(windowActivated(QWidget*)), m_manager, SLOT(windowChanged(QWidget*)));
	slotNoFormSelected();
}

void
KFormDesignerPart::setupActions()
{
	KStdAction::open(this, SLOT(open()), actionCollection());
	KStdAction::openNew(this, SLOT(createBlankForm()), actionCollection());
	KStdAction::save(this, SLOT(save()), actionCollection());
	KStdAction::saveAs(this, SLOT(saveAs()), actionCollection());
	KStdAction::cut(m_manager, SLOT(cutWidget()), actionCollection());
	KStdAction::copy(m_manager, SLOT(copyWidget()), actionCollection());
	KStdAction::paste(m_manager, SLOT(pasteWidget()), actionCollection());
	KStdAction::undo(m_manager, SLOT(undo()), actionCollection());
	KStdAction::redo(m_manager, SLOT(redo()), actionCollection());
	new KAction(i18n("Preview Form"), "filequickprint", KShortcut(0), this, SLOT(slotPreviewForm()), actionCollection(), "preview_form");
	new KAction(i18n("Edit Tab Order"), "tab_order", KShortcut(0), m_manager, SLOT(editTabOrder()), actionCollection(), "taborder");
	new KAction(i18n("Edit Pixmap Collection"), "icons", KShortcut(0), m_manager, SLOT(editFormPixmapCollection()), actionCollection(), "pixmap_collection");
	new KAction(i18n("Edit Form Connections"), "connections", KShortcut(0), m_manager, SLOT(editConnections()), actionCollection(), "form_connections");
	//KStdAction::printPreview(this, SLOT(slotPreviewForm()), actionCollection());
#if KDE_IS_VERSION(3,1,9) //&& !defined(Q_WS_WIN)
	KStdAction::clear(m_manager, SLOT(deleteWidget()), actionCollection());
#else
	//TODO
#endif

	new KAction(i18n("Lay Out Widgets &Horizontally"), QString::null, KShortcut(0), m_manager, SLOT(layoutHBox()), actionCollection(), "layout_hbox");
	new KAction(i18n("Lay Out Widgets &Vertically"), QString::null, KShortcut(0), m_manager, SLOT(layoutVBox()), actionCollection(), "layout_vbox");
	new KAction(i18n("Lay Out Widgets in &Grid"), QString::null, KShortcut(0), m_manager, SLOT(layoutGrid()), actionCollection(), "layout_grid");
	new KAction(i18n("&Break Layout"), QString::null, KShortcut(0), m_manager, SLOT(breakLayout()), actionCollection(), "break_layout");

	new KAction(i18n("Bring Widget to Front"), "raise", KShortcut(0), m_manager, SLOT(bringWidgetToFront()), actionCollection(), "format_raise");
	new KAction(i18n("Send Widget to Back"), "lower", KShortcut(0), m_manager, SLOT(sendWidgetToBack()), actionCollection(), "format_lower");

	KActionMenu *alignMenu = new KActionMenu(i18n("Align Widgets Position"), "aopos2grid", actionCollection(), "align_menu");
	alignMenu->insert( new KAction(i18n("To Left"), "aoleft", KShortcut(0), m_manager, SLOT(alignWidgetsToLeft()), actionCollection(), "align_to_left") );
	alignMenu->insert( new KAction(i18n("To Right"), "aoright", KShortcut(0), m_manager, SLOT(alignWidgetsToRight()), actionCollection(), "align_to_right") );
	alignMenu->insert( new KAction(i18n("To Top"), "aotop", KShortcut(0), m_manager, SLOT(alignWidgetsToTop()), actionCollection(), "align_to_top") );
	alignMenu->insert( new KAction(i18n("To Bottom"), "aobottom", KShortcut(0), m_manager, SLOT(alignWidgetsToBottom()), actionCollection(), "align_to_bottom") );
	alignMenu->insert( new KAction(i18n("To Grid"), "aopos2grid", KShortcut(0), m_manager, SLOT(alignWidgetsToGrid()), actionCollection(), "align_to_grid") );

	KActionMenu *sizeMenu = new KActionMenu(i18n("Adjust Widgets Size"), "aogrid", actionCollection(), "adjust_size_menu");
	sizeMenu->insert( new KAction(i18n("To Fit"), "aofit", KShortcut(0), m_manager, SLOT(adjustWidgetSize()), actionCollection(), "adjust_to_fit") );
	sizeMenu->insert( new KAction(i18n("To Grid"), "aogrid", KShortcut(0), m_manager, SLOT(adjustSizeToGrid()), actionCollection(), "adjust_size_grid") );
	sizeMenu->insert( new KAction(i18n("To Shortest"), "aoshortest", KShortcut(0), m_manager, SLOT(adjustHeightToSmall()), actionCollection(), "adjust_height_small") );
	sizeMenu->insert( new KAction(i18n("To Tallest"), "aotallest", KShortcut(0), m_manager, SLOT(adjustHeightToBig()), actionCollection(), "adjust_height_big") );
	sizeMenu->insert( new KAction(i18n("To Narrowest"), "aonarrowest", KShortcut(0), m_manager, SLOT(adjustWidthToSmall()), actionCollection(), "adjust_width_small") );
	sizeMenu->insert( new KAction(i18n("To Widest"), "aowidest", KShortcut(0), m_manager, SLOT(adjustWidthToBig()), actionCollection(), "adjust_width_big") );

	m_manager->createActions(actionCollection());
	setXMLFile("kformdesigner_part.rc");
}

void
KFormDesignerPart::createBlankForm()
{
	if(m_manager->activeForm() && m_uniqueFormMode)
	{
		m_openingFile = true;
		closeURL();
		m_openingFile = false;
	}

	if(m_uniqueFormMode && m_manager->activeForm() && !m_manager->activeForm()->isModified() && m_manager->activeForm()->filename().isNull())
		return;  // active form is already a blank one

	QString n = i18n("Form") + QString::number(++m_count);
	Form *form = new Form(m_manager, n.latin1());
	FormWidgetBase *w = new FormWidgetBase(this, m_workspace, n.latin1());

	w->setCaption(n);
	w->setIcon(SmallIcon("form"));
	w->resize(350, 300);
	w->show();
	w->setFocus();

	form->createToplevel(w, w);
	m_manager->importForm(form);
}

void
KFormDesignerPart::open()
{
	m_openingFile = true;
	KURL url = KFileDialog::getOpenURL("::kformdesigner", i18n("*.ui|Qt Designer UI Files"), m_workspace->topLevelWidget());
	if(!url.isEmpty())
		ReadWritePart::openURL(url);
	m_openingFile = false;
}

bool
KFormDesignerPart::openFile()
{
	Form *form = new Form(m_manager);
	FormWidgetBase *w = new FormWidgetBase(this, m_workspace);
	form->createToplevel(w, w);

	if(!KFormDesigner::FormIO::loadForm(form, w, m_file))
	{
		delete form;
		delete w;
		return false;
	}

	w->show();
	m_manager->importForm(form, !isReadWrite());
	return true;
}

bool
KFormDesignerPart::saveFile()
{
	KFormDesigner::FormIO::saveForm(m_manager->activeForm(), m_file);
	return true;
}

void
KFormDesignerPart::saveAs()
{
	KURL url = KFileDialog::getSaveURL("::kformdesigner", i18n("*.ui|Qt Designer UI Files"), m_workspace);
	if(url.isEmpty())
		return;
	else
		ReadWritePart::saveAs(url);
}

bool
KFormDesignerPart::closeForm(Form *form)
{
	int res = KMessageBox::warningYesNoCancel( m_workspace->topLevelWidget(),
		i18n( "The form \"%1\" has been modified.\n"
		"Do you want to save your changes or discard them?" ).arg( form->objectTree()->name() ),
		i18n( "Close Form" ), KStdGuiItem::save(), KStdGuiItem::discard() );

	if(res == KMessageBox::Yes)
		save();

	return (res != KMessageBox::Cancel);
}

bool
KFormDesignerPart::closeForms()
{
	QWidgetList list = m_workspace->windowList(QWorkspace::CreationOrder);
	for(QWidget *w = list.first(); w; w = list.next())
		if(w->close() == false)
			return false;

	return true;
}

bool
KFormDesignerPart::closeURL()
{
	if(!m_manager->activeForm())
		return true;

	if(m_uniqueFormMode || !m_openingFile)
		return closeForms();

	return true;
}

void
KFormDesignerPart::slotFormModified(Form *, bool isDirty)
{
	setModified(isDirty);
}

void
KFormDesignerPart::slotPreviewForm()
{
	if(!m_manager->activeForm())
		return;

	FormWidgetBase *w = new FormWidgetBase(this, m_workspace);
	m_manager->previewForm(m_manager->activeForm(), w);
}

void
KFormDesignerPart::slotWidgetSelected(Form *form, bool multiple)
{
	enableFormActions();
	// Enable edit actions
	ENABLE_ACTION("edit_copy", true);
	ENABLE_ACTION("edit_cut", true);
#if KDE_IS_VERSION(3,1,9) //&& !defined(Q_WS_WIN)
	ENABLE_ACTION("edit_clear", true);
#endif

	// 'Align Widgets' menu
	ENABLE_ACTION("align_menu", multiple);
	ENABLE_ACTION("align_to_left", multiple);
	ENABLE_ACTION("align_to_right", multiple);
	ENABLE_ACTION("align_to_top", multiple);
	ENABLE_ACTION("align_to_bottom", multiple);

	ENABLE_ACTION("adjust_size_menu", true);
	ENABLE_ACTION("adjust_width_small", multiple);
	ENABLE_ACTION("adjust_width_big", multiple);
	ENABLE_ACTION("adjust_height_small", multiple);
	ENABLE_ACTION("adjust_height_big", multiple);

	ENABLE_ACTION("format_raise", true);
	ENABLE_ACTION("format_lower", true);

	// If the widgets selected is a container, we enable layout actions
	if(!multiple)
	{
		KFormDesigner::ObjectTreeItem *item = form->objectTree()->lookup( form->selectedWidgets()->first()->name() );
		if(item && item->container())
			multiple = true;
	}
	// Layout actions
	ENABLE_ACTION("layout_hbox", multiple);
	ENABLE_ACTION("layout_vbox", multiple);
	ENABLE_ACTION("layout_grid", multiple);

	KFormDesigner::Container *container = m_manager->activeForm()->activeContainer();
	ENABLE_ACTION("break_layout", (container->layoutType() != KFormDesigner::Container::NoLayout));
}

void
KFormDesignerPart::slotFormWidgetSelected(Form *form)
{
	disableWidgetActions();
	enableFormActions();

	// Layout actions
	ENABLE_ACTION("layout_hbox", true);
	ENABLE_ACTION("layout_vbox", true);
	ENABLE_ACTION("layout_grid", true);
	ENABLE_ACTION("break_layout", (form->toplevelContainer()->layoutType() != KFormDesigner::Container::NoLayout));
}

void
KFormDesignerPart::slotNoFormSelected()
{
	disableWidgetActions();

	// Disable paste action
	ENABLE_ACTION("edit_paste", false);

	ENABLE_ACTION("edit_undo", false);
	ENABLE_ACTION("edit_redo", false);

	// Disable 'Tools' actions
	ENABLE_ACTION("pixmap_collection", false);
	ENABLE_ACTION("form_connections", false);
	ENABLE_ACTION("taborder", false);
	ENABLE_ACTION("change_style", false);

	// Disable items in 'File'
	ENABLE_ACTION("file_save", false);
	ENABLE_ACTION("file_save_as", false);
	ENABLE_ACTION("preview_form", false);
}

void
KFormDesignerPart::enableFormActions()
{
	// Enable 'Tools' actions
	ENABLE_ACTION("pixmap_collection", true);
	ENABLE_ACTION("form_connections", true);
	ENABLE_ACTION("taborder", true);
	ENABLE_ACTION("change_style", true);

	// Enable items in 'File'
	ENABLE_ACTION("file_save", true);
	ENABLE_ACTION("file_save_as", true);
	ENABLE_ACTION("preview_form", true);

	ENABLE_ACTION("edit_paste", m_manager->isPasteEnabled());
}

void
KFormDesignerPart::disableWidgetActions()
{
	// Disable edit actions
	ENABLE_ACTION("edit_copy", false);
	ENABLE_ACTION("edit_cut", false);
	ENABLE_ACTION("edit_clear", false);

	// Disable format functions
	ENABLE_ACTION("align_menu", false);
	ENABLE_ACTION("align_to_left", false);
	ENABLE_ACTION("align_to_right", false);
	ENABLE_ACTION("align_to_top", false);
	ENABLE_ACTION("align_to_bottom", false);
	ENABLE_ACTION("adjust_size_menu", false);
	ENABLE_ACTION("format_raise", false);
	ENABLE_ACTION("format_lower", false);

	ENABLE_ACTION("layout_hbox", false);
	ENABLE_ACTION("layout_vbox", false);
	ENABLE_ACTION("layout_grid", false);
	ENABLE_ACTION("break_layout", false);
}

void
KFormDesignerPart::setUndoEnabled(bool enabled, const QString &text)
{
	KAction *undoAction = actionCollection()->action("edit_undo");
	if(undoAction)
	{
		undoAction->setEnabled(enabled);
		if(!text.isNull())
			undoAction->setText(text);
	}
}

void
KFormDesignerPart::setRedoEnabled(bool enabled, const QString &text)
{
	KAction *redoAction = actionCollection()->action("edit_redo");
	if(redoAction)
	{
		redoAction->setEnabled(enabled);
		if(!text.isNull())
			redoAction->setText(text);
	}
}

KFormDesignerPart::~KFormDesignerPart()
{
}


//////  FormWidgetBase : helper widget to draw rects on top of widgets

//repaint all children widgets
static void repaintAll(QWidget *w)
{
	QObjectList *list = w->queryList("QWidget");
	QObjectListIt it(*list);
	for (QObject *obj; (obj=it.current()); ++it ) {
		static_cast<QWidget*>(obj)->repaint();
	}
	delete list;
}

void
FormWidgetBase::drawRect(const QRect& r, int type)
{
	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	if (prev_rect.isValid()) {
		//redraw prev. selection's rectangle
		p.drawPixmap( QPoint(prev_rect.x()-2, prev_rect.y()-2), buffer, QRect(prev_rect.x()-2, prev_rect.y()-2, prev_rect.width()+4, prev_rect.height()+4));
	}
	p.setBrush(QBrush::NoBrush);
	if(type == 1) // selection rect
		p.setPen(QPen(white, 1, Qt::DotLine));
	else if(type == 2) // insert rect
		p.setPen(QPen(white, 2));
	p.setRasterOp(XorROP);
	p.drawRect(r);
	prev_rect = r;

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();
}

void
FormWidgetBase::initRect()
{
	repaintAll(this);
	buffer.resize( width(), height() );
	buffer = QPixmap::grabWindow( winId() );
	prev_rect = QRect();
}

void
FormWidgetBase::clearRect()
{
	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	//redraw entire form surface
	p.drawPixmap( QPoint(0,0), buffer, QRect(0,0,buffer.width(), buffer.height()) );

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();

	repaintAll(this);
}

void
FormWidgetBase::highlightWidgets(QWidget *from, QWidget *to)//, const QPoint &point)
{
	QPoint fromPoint, toPoint;
	if(from && from->parentWidget() && (from != this))
		fromPoint = from->parentWidget()->mapTo(this, from->pos());
	if(to && to->parentWidget() && (to != this))
		toPoint = to->parentWidget()->mapTo(this, to->pos());

	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	if (prev_rect.isValid()) {
		//redraw prev. selection's rectangle
		p.drawPixmap( QPoint(prev_rect.x(), prev_rect.y()), buffer, QRect(prev_rect.x(), prev_rect.y(), prev_rect.width(), prev_rect.height()));
	}

	p.setPen( QPen(Qt::red, 2) );

	if(to)
	{
		QPixmap pix1 = QPixmap::grabWidget(from);
		QPixmap pix2 = QPixmap::grabWidget(to);

		if((from != this) && (to != this))
			p.drawLine( from->parentWidget()->mapTo(this, from->geometry().center()), to->parentWidget()->mapTo(this, to->geometry().center()) );

		p.drawPixmap(fromPoint.x(), fromPoint.y(), pix1);
		p.drawPixmap(toPoint.x(), toPoint.y(), pix2);

		if(to == this)
			p.drawRoundRect(2, 2, width()-4, height()-4, 4, 4);
		else
			p.drawRoundRect(toPoint.x(), toPoint.y(), to->width(), to->height(), 5, 5);
	}

	if(from == this)
		p.drawRoundRect(2, 2, width()-4, height()-4, 4, 4);
	else
		p.drawRoundRect(fromPoint.x(),  fromPoint.y(), from->width(), from->height(), 5, 5);

	if((to == this) || (from == this))
		prev_rect = QRect(0, 0, buffer.width(), buffer.height());
	else if(to)
	{
		prev_rect.setX( (fromPoint.x() < toPoint.x()) ? (fromPoint.x() - 5) : (toPoint.x() - 5) );
		prev_rect.setY( (fromPoint.y() < toPoint.y()) ? (fromPoint.y() - 5) : (toPoint.y() - 5) );
		prev_rect.setRight( (fromPoint.x() < toPoint.x()) ? (toPoint.x() + to->width() + 10) : (fromPoint.x() + from->width() + 10) );
		prev_rect.setBottom( (fromPoint.y() < toPoint.y()) ? (toPoint.y() + to->height() + 10) : (fromPoint.y() + from->height() + 10) ) ;
	}
	else
		prev_rect = QRect(fromPoint.x()- 5,  fromPoint.y() -5, from->width() + 10, from->height() + 10);

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();
}

void
FormWidgetBase::closeEvent(QCloseEvent *ev)
{
	Form *form = m_part->manager()->formForWidget(this);
	if(!form || !form->isModified() || !form->objectTree()) // == preview form
		ev->accept();
	else
	{
		bool close = m_part->closeForm(form);
		if(close)
			ev->accept();
		else
			ev->ignore();
	}
}

K_EXPORT_COMPONENT_FACTORY(libkformdesigner_part, KFDFactory);

#include "kfd_part.moc"

