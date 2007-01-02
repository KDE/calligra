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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qworkspace.h>
#include <q3dockarea.h>
#include <q3dockwindow.h>
#include <khbox.h>
#include <qpainter.h>
#include <qevent.h>
#include <qobject.h>
//Added by qt3to4:
#include <QPixmap>
#include <QCloseEvent>
#include <Q3ValueList>

#include <kdeversion.h>
#include <kaction.h>
#include <kinstance.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <kstandardaction.h>
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

#include "kfd_kdev_part.h"

#define ENABLE_ACTION(name, enable) \
	if(actionCollection()->action( name )) \
		actionCollection()->action( name )->setEnabled( enable )

KInstance *KFDFactory::m_instance = 0L;

KFDFactory::KFDFactory()
: KParts::Factory(0, "libkformdesigner_kdev_part")
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
	bool readOnly = (classname == "KParts::ReadOnlyPart");
	KFormDesignerKDevPart *part = new KFormDesignerKDevPart(parentWidget, name, readOnly, args);
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
	KAboutData *about = new KAboutData("kformdesigner_kdev_part", I18N_NOOP("Form Designer Part"), "0.3");
	return about;
}

// copied from kfd_part.cpp
class KFDPart_FormManager : public KFormDesigner::FormManager
{
	public:
		/*! Constructs FormManager object.
		 See WidgetLibrary's constructor documentation for information about
		 \a supportedFactoryGroups parameter.
		 Using \a options you can control manager's behaviour, see \ref Options. */
		KFDPart_FormManager(KFormDesignerPart *part, int options = 0, const char *name = 0)
		 : KFormDesigner::FormManager(part, options, name)
		 , m_part(part)
		{
		}

		virtual KAction* action( const char* name)
		{
			return m_part->actionCollection()->action( name );
		}

		virtual void enableAction( const char* name, bool enable ) {
			if(m_part->actionCollection()->action( name ))
				m_part->actionCollection()->action( name )->setEnabled( enable );
		}

		KFormDesignerPart *m_part;
};

//////////////

KFormDesigner::WidgetLibrary* KFormDesignerKDevPart::static_formsLibrary = 0L;

KFormDesignerKDevPart::KFormDesignerKDevPart(QWidget *parent, const char *name, bool readOnly, const QStringList &args)
: Designer(parent, name), m_count(0)
{
	setInstance(KFDFactory::instance());
	instance()->iconLoader()->addAppDir("kexi");
	instance()->iconLoader()->addAppDir("kformdesigner");

	setReadWrite(!readOnly);
	m_uniqueFormMode = true;
	m_openingFile = false;

	if(!args.grep("multipleMode").isEmpty())
		setUniqueFormMode(false);
	m_inShell = (!args.grep("shell").isEmpty());

	KHBox *container = new KHBox(parent, "kfd_container_widget");
	container->setFocusPolicy(Qt::ClickFocus);

	m_workspace = new QWorkspace(container, "kfd_workspace");
	m_workspace->show();
	QStringList supportedFactoryGroups;
/* @todo add configuration for supported factory groups */
	static_formsLibrary = KFormDesigner::FormManager::createWidgetLibrary( 
		new KFDPart_FormManager(this, 0, "kfd_manager"), supportedFactoryGroups );

	if(!readOnly)
	{
		Q3DockArea *dockArea = new Q3DockArea(Qt::Vertical, Q3DockArea::Reverse, container, "kfd_part_dockarea");

		Q3DockWindow *dockTree = new Q3DockWindow(dockArea);
		KFormDesigner::ObjectTreeView *view = new KFormDesigner::ObjectTreeView(dockTree);
		dockTree->setWidget(view);
		dockTree->setCaption(i18n("Objects"));
		dockTree->setResizeEnabled(true);
		dockTree->setFixedExtentWidth(256);

		Q3DockWindow *dockEditor = new Q3DockWindow(dockArea);
		KoProperty::Editor *editor = new KoProperty::Editor(dockEditor);
		dockEditor->setWidget(editor);
		dockEditor->setCaption(i18n("Properties"));
		dockEditor->setResizeEnabled(true);

		KFormDesigner::FormManager::self()->setEditor(editor);
		KFormDesigner::FormManager::self()->setObjectTreeView(view);

		setupActions();
		setModified(false);

		// action stuff
		connect(KFormDesigner::FormManager::self(), SIGNAL(widgetSelected(KFormDesigner::Form*, bool)), SLOT(slotWidgetSelected(KFormDesigner::Form*, bool)));
		connect(KFormDesigner::FormManager::self(), SIGNAL(formWidgetSelected(KFormDesigner::Form*)), SLOT(slotFormWidgetSelected(KFormDesigner::Form*)));
		connect(KFormDesigner::FormManager::self(), SIGNAL(noFormSelected()), SLOT(slotNoFormSelected()));
		connect(KFormDesigner::FormManager::self(), SIGNAL(undoEnabled(bool, const QString&)), SLOT(setUndoEnabled(bool, const QString&)));
		connect(KFormDesigner::FormManager::self(), SIGNAL(redoEnabled(bool, const QString&)), SLOT(setRedoEnabled(bool, const QString&)));

		connect(KFormDesigner::FormManager::self(), SIGNAL(dirty(KFormDesigner::Form*, bool)), this, SLOT(slotFormModified(KFormDesigner::Form*, bool)));

		connect(KFormDesigner::FormManager::self(), SIGNAL(createFormSlot(KFormDesigner::Form*, const QString&, const QString&)),
			this, SLOT(slotCreateFormSlot(KFormDesigner::Form*, const QString&, const QString &)));
	}

	container->show();
	setWidget(container);
	connect(m_workspace, SIGNAL(windowActivated(QWidget*)), KFormDesigner::FormManager::self(), SLOT(windowChanged(QWidget*)));
	slotNoFormSelected();
}

KFormDesigner::WidgetLibrary* KFormDesignerKDevPart::formsLibrary()
{
	return static_formsLibrary;
}

void
KFormDesignerKDevPart::setupActions()
{
	KStandardAction::open(this, SLOT(open()), actionCollection());
	KStandardAction::openNew(this, SLOT(createBlankForm()), actionCollection());
	KStandardAction::save(this, SLOT(save()), actionCollection());
	KStandardAction::saveAs(this, SLOT(saveAs()), actionCollection());
	KStandardAction::cut(KFormDesigner::FormManager::self(), SLOT(cutWidget()), actionCollection());
	KStandardAction::copy(KFormDesigner::FormManager::self(), SLOT(copyWidget()), actionCollection());
	KStandardAction::paste(KFormDesigner::FormManager::self(), SLOT(pasteWidget()), actionCollection());
	KStandardAction::undo(KFormDesigner::FormManager::self(), SLOT(undo()), actionCollection());
	KStandardAction::redo(KFormDesigner::FormManager::self(), SLOT(redo()), actionCollection());
	KStandardAction::selectAll(KFormDesigner::FormManager::self(), SLOT(selectAll()), actionCollection());
	new KAction(i18n("Clear Widget Contents"), "editclear", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(clearWidgetContent()), actionCollection(), "clear_contents");
	new KAction(i18n("Delete Widget"), "editdelete", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(deleteWidget()), actionCollection(), "edit_delete");
	new KAction(i18n("Preview Form"), "filequickprint", "Ctrl+T", this, SLOT(slotPreviewForm()), actionCollection(), "preview_form");
	new KAction(i18n("Edit Tab Order"), "tab_order", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(editTabOrder()), actionCollection(), "taborder");
	new KAction(i18n("Edit Pixmap Collection"), "icons", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(editFormPixmapCollection()), actionCollection(), "pixmap_collection");
	new KAction(i18n("Edit Form Connections"), "connections", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(editConnections()), actionCollection(), "form_connections");

	new KAction(i18n("Lay Out Widgets &Horizontally"), QString::null, KShortcut(0), KFormDesigner::FormManager::self(), SLOT(layoutHBox()), actionCollection(), "layout_hbox");
	new KAction(i18n("Lay Out Widgets &Vertically"), QString::null, KShortcut(0), KFormDesigner::FormManager::self(), SLOT(layoutVBox()), actionCollection(), "layout_vbox");
	new KAction(i18n("Lay Out Widgets in &Grid"), QString::null, KShortcut(0), KFormDesigner::FormManager::self(), SLOT(layoutGrid()), actionCollection(), "layout_grid");
	new KAction(i18n("&Break Layout"), QString::null, KShortcut(0), KFormDesigner::FormManager::self(), SLOT(breakLayout()), actionCollection(), "break_layout");

	new KAction(i18n("Bring Widget to Front"), "raise", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(bringWidgetToFront()), actionCollection(), "format_raise");
	new KAction(i18n("Send Widget to Back"), "lower", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(sendWidgetToBack()), actionCollection(), "format_lower");

	KActionMenu *alignMenu = new KActionMenu(i18n("Align Widgets' Positions"), "aopos2grid", actionCollection(), "align_menu");
	alignMenu->insert( new KAction(i18n("To Left"), "aoleft", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(alignWidgetsToLeft()), actionCollection(), "align_to_left") );
	alignMenu->insert( new KAction(i18n("To Right"), "aoright", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(alignWidgetsToRight()), actionCollection(), "align_to_right") );
	alignMenu->insert( new KAction(i18n("To Top"), "aotop", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(alignWidgetsToTop()), actionCollection(), "align_to_top") );
	alignMenu->insert( new KAction(i18n("To Bottom"), "aobottom", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(alignWidgetsToBottom()), actionCollection(), "align_to_bottom") );
	alignMenu->insert( new KAction(i18n("To Grid"), "aopos2grid", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(alignWidgetsToGrid()), actionCollection(), "align_to_grid") );

	KActionMenu *sizeMenu = new KActionMenu(i18n("Adjust Widgets' Sizes"), "aogrid", actionCollection(), "adjust_size_menu");
	sizeMenu->insert( new KAction(i18n("To Fit"), "aofit", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(adjustWidgetSize()), actionCollection(), "adjust_to_fit") );
	sizeMenu->insert( new KAction(i18n("To Grid"), "aogrid", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(adjustSizeToGrid()), actionCollection(), "adjust_size_grid") );
	sizeMenu->insert( new KAction(i18n("To Shortest"), "aoshortest", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(adjustHeightToSmall()), actionCollection(), "adjust_height_small") );
	sizeMenu->insert( new KAction(i18n("To Tallest"), "aotallest", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(adjustHeightToBig()), actionCollection(), "adjust_height_big") );
	sizeMenu->insert( new KAction(i18n("To Narrowest"), "aonarrowest", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(adjustWidthToSmall()), actionCollection(), "adjust_width_small") );
	sizeMenu->insert( new KAction(i18n("To Widest"), "aowidest", KShortcut(0), KFormDesigner::FormManager::self(), SLOT(adjustWidthToBig()), actionCollection(), "adjust_width_big") );

	if(m_inShell)
		setXMLFile("kformdesigner_part_shell.rc");
	else
		setXMLFile("kformdesigner_part.rc");
	KFormDesigner::FormManager::self()->createActions(formsLibrary(), actionCollection(), this);
}

void
KFormDesignerKDevPart::createBlankForm()
{
	if(KFormDesigner::FormManager::self()->activeForm() && m_uniqueFormMode)
	{
		m_openingFile = true;
		closeUrl();
		m_openingFile = false;
	}

	if(m_uniqueFormMode && KFormDesigner::FormManager::self()->activeForm() && !KFormDesigner::FormManager::self()->activeForm()->isModified() && KFormDesigner::FormManager::self()->activeForm()->filename().isNull())
		return;  // active form is already a blank one

	QString n = i18n("Form") + QString::number(++m_count);
	Form *form = new Form(formsLibrary(), n.latin1());
	FormWidgetBase *w = new FormWidgetBase(this, m_workspace, n.latin1());

	w->setCaption(n);
	w->setIcon(SmallIcon("form"));
	w->resize(350, 300);
	w->show();
	w->setFocus();

	form->createToplevel(w, w);
	KFormDesigner::FormManager::self()->importForm(form);
}

void
KFormDesignerKDevPart::open()
{
	m_openingFile = true;
	KUrl url = KFileDialog::getOpenURL("::kformdesigner", i18n("*.ui|Qt Designer UI Files"), m_workspace->topLevelWidget());
	if(!url.isEmpty())
		ReadWritePart::openURL(url);
	m_openingFile = false;
}

bool
KFormDesignerKDevPart::openFile()
{
	Form *form = new Form(formsLibrary());
	FormWidgetBase *w = new FormWidgetBase(this, m_workspace);
	form->createToplevel(w, w);

	if(!KFormDesigner::FormIO::loadForm(form, w, m_file))
	{
		delete form;
		delete w;
		return false;
	}

	w->show();
	KFormDesigner::FormManager::self()->importForm(form, !isReadWrite());
	return true;
}

bool
KFormDesignerKDevPart::saveFile()
{
	KFormDesigner::FormIO::saveForm(KFormDesigner::FormManager::self()->activeForm(), m_file);
	return true;
}

void
KFormDesignerKDevPart::saveAs()
{
	KUrl url = KFileDialog::getSaveURL("::kformdesigner", i18n("*.ui|Qt Designer UI Files"), m_workspace);
	if(url.isEmpty())
		return;
	else
		ReadWritePart::saveAs(url);
}

bool
KFormDesignerKDevPart::closeForm(Form *form)
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
KFormDesignerKDevPart::closeForms()
{
	QWidgetList list = m_workspace->windowList(QWorkspace::CreationOrder);
	for(QWidget *w = list.first(); w; w = list.next())
		if(w->close() == false)
			return false;

	return true;
}

bool
KFormDesignerKDevPart::closeUrl()
{
	if(!KFormDesigner::FormManager::self()->activeForm())
		return true;

	if(m_uniqueFormMode || !m_openingFile)
		return closeForms();

	return true;
}

void
KFormDesignerKDevPart::slotFormModified(Form *, bool isDirty)
{
	setModified(isDirty);
}

void
KFormDesignerKDevPart::slotPreviewForm()
{
	if(!KFormDesigner::FormManager::self()->activeForm())
		return;

	FormWidgetBase *w = new FormWidgetBase(this, m_workspace);
	KFormDesigner::FormManager::self()->previewForm(KFormDesigner::FormManager::self()->activeForm(), w);
}

void
KFormDesignerKDevPart::slotWidgetSelected(Form *form, bool multiple)
{
	enableFormActions();
	// Enable edit actions
	ENABLE_ACTION("edit_copy", true);
	ENABLE_ACTION("edit_cut", true);
	ENABLE_ACTION("edit_delete", true);
	ENABLE_ACTION("clear_contents", true);

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

	KFormDesigner::Container *container = KFormDesigner::FormManager::self()->activeForm()->activeContainer();
	ENABLE_ACTION("break_layout", (container->layoutType() != KFormDesigner::Container::NoLayout));
}

void
KFormDesignerKDevPart::slotFormWidgetSelected(Form *form)
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
KFormDesignerKDevPart::slotNoFormSelected()
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
KFormDesignerKDevPart::enableFormActions()
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

	ENABLE_ACTION("edit_paste", KFormDesigner::FormManager::self()->isPasteEnabled());
	ENABLE_ACTION("edit_select_all", true);
}

void
KFormDesignerKDevPart::disableWidgetActions()
{
	// Disable edit actions
	ENABLE_ACTION("edit_copy", false);
	ENABLE_ACTION("edit_cut", false);
	ENABLE_ACTION("edit_delete", false);
	ENABLE_ACTION("clear_contents", false);

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
KFormDesignerKDevPart::setUndoEnabled(bool enabled, const QString &text)
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
KFormDesignerKDevPart::setRedoEnabled(bool enabled, const QString &text)
{
	KAction *redoAction = actionCollection()->action("edit_redo");
	if(redoAction)
	{
		redoAction->setEnabled(enabled);
		if(!text.isNull())
			redoAction->setText(text);
	}
}

void
KFormDesignerKDevPart::slotCreateFormSlot(Form *form, const QString &widget, const QString &signal)
{
	Function f;
	f.returnType = "void";
	f.function = widget + "_" + signal;
	f.specifier = "non virtual";
	f.access = "public";
	f.type = ftQtSlot;
	emit addedFunction(designerType(), form->objectTree()->name(), f);
}

KFormDesignerKDevPart::~KFormDesignerKDevPart()
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
FormWidgetBase::drawRects(const Q3ValueList<QRect> &list, int type)
{
	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	if (prev_rect.isValid()) {
		//redraw prev. selection's rectangle
		p.drawPixmap( QPoint(prev_rect.x()-2, prev_rect.y()-2), buffer, QRect(prev_rect.x()-2, prev_rect.y()-2, prev_rect.width()+4, prev_rect.height()+4));
	}
	p.setBrush(Qt::NoBrush);
	if(type == 1) // selection rect
		p.setPen(QPen(Qt::white, 1, Qt::DotLine));
	else if(type == 2) // insert rect
		p.setPen(QPen(Qt::white, 2));
	p.setRasterOp(XorROP);

	prev_rect = QRect();
	Q3ValueList<QRect>::ConstIterator endIt = list.constEnd();
	for(Q3ValueList<QRect>::ConstIterator it = list.constBegin(); it != endIt; ++it) {
		p.drawRect(*it);
		prev_rect = prev_rect.unite(*it);
	}

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();
}

void
FormWidgetBase::drawRect(const QRect& r, int type)
{
	Q3ValueList<QRect> l;
	l.append(r);
	drawRects(l, type);
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
	Form *form = KFormDesigner::FormManager::self()->formForWidget(this);
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

K_EXPORT_COMPONENT_FACTORY(libkformdesigner_kdev_part, KFDFactory)

#include "kfd_kdev_part.moc"

