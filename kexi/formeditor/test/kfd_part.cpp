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

#include <kdeversion.h>
#include <kaction.h>
#include <kinstance.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <kapplication.h>
#include <kiconloader.h>

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

extern "C"
{
	void* init_libkfd_part()
	{
		return new KFDFactory;
	}
};

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
  const char *classname, const QStringList &)
{
	bool readOnly = (QCString(classname) == "KParts::ReadOnlyPart");
	KFormDesignerPart *part = new KFormDesignerPart(parentWidget, name, readOnly);
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
	KAboutData *about = new KAboutData("kfd_part", I18N_NOOP("KFormDesigner Part"), "0.3");
	return about;
}

KFormDesignerPart::KFormDesignerPart(QWidget *parent, const char *name, bool readOnly)
: KParts::ReadWritePart(parent, name)
{
	setInstance(KFDFactory::instance());
	instance()->iconLoader()->addAppDir("kexi");
	instance()->iconLoader()->addAppDir("kformdesigner");

	QHBox *container = new QHBox(parent, "kfd_container_widget");

	m_workspace = new QWorkspace(container, "kfd_workspace");
	m_workspace->show();
	m_manager = new KFormDesigner::FormManager(m_workspace, this, "kfd_manager");

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

		connect(m_manager, SIGNAL(dirty(Form*)), this, SLOT(slotFormModified()));
	}

	container->show();
	setWidget(container);
	connect(m_workspace, SIGNAL(windowActivated(QWidget*)), m_manager, SLOT(windowChanged(QWidget*)));
}

void
KFormDesignerPart::setupActions()
{
	KStdAction::open(m_manager, SLOT(loadForm()), actionCollection());
	KStdAction::openNew(m_manager, SLOT(createBlankForm()), actionCollection());
	KStdAction::save(this, SLOT(save()), actionCollection());
	KStdAction::saveAs(m_manager, SLOT(saveFormAs()), actionCollection());
	KStdAction::cut(m_manager, SLOT(cutWidget()), actionCollection());
	KStdAction::copy(m_manager, SLOT(copyWidget()), actionCollection());
	KStdAction::paste(m_manager, SLOT(pasteWidget()), actionCollection());
	new KAction(i18n("Preview Form"), "filequickprint", KShortcut(0), m_manager, SLOT(slotPreviewForm()), actionCollection(), "preview_form");
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

	KActionMenu *alignMenu = new KActionMenu(i18n("Align Widgets position"), "aopos2grid", actionCollection(), "align_menu");
	alignMenu->insert( new KAction(i18n("To Left"), "aoleft", KShortcut(0), m_manager, SLOT(alignWidgetsToLeft()), actionCollection(), "align_to_left") );
	alignMenu->insert( new KAction(i18n("To Right"), "aoright", KShortcut(0), m_manager, SLOT(alignWidgetsToRight()), actionCollection(), "align_to_right") );
	alignMenu->insert( new KAction(i18n("To Top"), "aotop", KShortcut(0), m_manager, SLOT(alignWidgetsToTop()), actionCollection(), "align_to_top") );
	alignMenu->insert( new KAction(i18n("To Bottom"), "aobottom", KShortcut(0), m_manager, SLOT(alignWidgetsToBottom()), actionCollection(), "align_to_bottom") );
	alignMenu->insert( new KAction(i18n("To Grid"), "aopos2grid", KShortcut(0), m_manager, SLOT(alignWidgetsToGrid()), actionCollection(), "align_to_grid") );

	KActionMenu *sizeMenu = new KActionMenu(i18n("Adjust Widgets size"), "aogrid", actionCollection(), "adjust_size_menu");
	sizeMenu->insert( new KAction(i18n("To Fit"), "aofit", KShortcut(0), m_manager, SLOT(adjustWidgetSize()), actionCollection(), "adjust_to_fit") );
	sizeMenu->insert( new KAction(i18n("To Grid"), "aogrid", KShortcut(0), m_manager, SLOT(adjustSizeToGrid()), actionCollection(), "adjust_size_grid") );
	sizeMenu->insert( new KAction(i18n("To Shortest"), "aoshortest", KShortcut(0), m_manager, SLOT(adjustHeightToSmall()), actionCollection(), "adjust_height_small") );
	sizeMenu->insert( new KAction(i18n("To Tallest"), "aotallest", KShortcut(0), m_manager, SLOT(adjustHeightToBig()), actionCollection(), "adjust_height_big") );
	sizeMenu->insert( new KAction(i18n("To Narrowest"), "aonarrowest", KShortcut(0), m_manager, SLOT(adjustWidthToSmall()), actionCollection(), "adjust_width_small") );
	sizeMenu->insert( new KAction(i18n("To Widest"), "aowidest", KShortcut(0), m_manager, SLOT(adjustWidthToBig()), actionCollection(), "adjust_width_big") );

	m_manager->createActions(actionCollection());
	setXMLFile("kfd_part.rc");
}


bool
KFormDesignerPart::openFile()
{
	// Open the form in file m_file
	m_manager->loadForm(!isReadWrite(), m_file);
	return true;
}

bool
KFormDesignerPart::saveFile()
{
	KFormDesigner::FormIO::saveForm(m_manager->activeForm(), m_file);
	return true;
}

/*bool
KFormDesignerPart::closeURL()
{
	return true;
}*/

void
KFormDesignerPart::slotFormModified()
{
	setModified(true);
}

void
KFormDesignerPart::slotPreviewForm()
{
	if(!m_manager->activeForm())
		return;
	QWidget *w = new QWidget(m_workspace);
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

	// Disable 'Tools' actions
	ENABLE_ACTION("pixmap_collection", false);
	ENABLE_ACTION("form_connections", false);
	ENABLE_ACTION("taborder", false);
	ENABLE_ACTION("change_style", false);

	// Disable items in 'File'
	ENABLE_ACTION("file_save", false);
	ENABLE_ACTION("file_save_as", false);
	ENABLE_ACTION("file_print_preview", false);
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
	ENABLE_ACTION("file_print_preview", true);

	ENABLE_ACTION("edit_paste", m_manager->isPasteEnabled());
}

void
KFormDesignerPart::disableWidgetActions()
{
	// Disable edit actions
	ENABLE_ACTION("edit_paste", false);
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

KFormDesignerPart::~KFormDesignerPart()
{
	closeURL();
}

#include "kfd_part.moc"
