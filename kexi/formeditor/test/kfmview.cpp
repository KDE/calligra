/***************************************************************************
 *   Copyright (C) 2003 by Lucijan Busch                                   *
 *   lucijan@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 ***************************************************************************/

#include <qworkspace.h>

#include <kdeversion.h>
#include <kaction.h>
#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstdaction.h>

#include "form.h"
#include "objecttree.h"
#include "container.h"
#include "formmanager.h"
#include "objecttreeview.h"
#include "kexipropertyeditor.h"
#include "kfmview.h"

KFMView::KFMView()
 : KMainWindow()
{
	w = new QWorkspace(this);
	setCentralWidget(w);
	w->show();

	manager = new KFormDesigner::FormManager(w, this, "manager");

	QDockWindow *dockTree = new QDockWindow(this);
	KFormDesigner::ObjectTreeView *view = new KFormDesigner::ObjectTreeView(dockTree);
	dockTree->setWidget(view);
	dockTree->setCaption(i18n("Objects"));
	dockTree->setResizeEnabled(true);
	dockTree->setFixedExtentWidth(256);
	moveDockWindow(dockTree, DockRight);

	QDockWindow *dockEditor = new QDockWindow(this);
	KexiPropertyEditor *editor = new KexiPropertyEditor(dockEditor);
	dockEditor->setWidget(editor);
	dockEditor->setCaption(i18n("Properties"));
	dockEditor->setResizeEnabled(true);
	moveDockWindow(dockEditor, DockRight);

	connect(w, SIGNAL(windowActivated(QWidget*)), manager, SLOT(windowChanged(QWidget*)));

	manager->setEditors(editor, view);

	//new KAction(i18n("Print Object Tree"), "view_tree", KShortcut(0), manager, SLOT(debugTree()), actionCollection(), "dtree");
	KStdAction::save(manager, SLOT(saveForm()), actionCollection());
	KStdAction::saveAs(manager, SLOT(saveFormAs()), actionCollection());
	KStdAction::open(manager, SLOT(loadForm()), actionCollection());
	KStdAction::openNew(manager, SLOT(createBlankForm()), actionCollection());
	KStdAction::cut(manager, SLOT(cutWidget()), actionCollection());
	KStdAction::copy(manager, SLOT(copyWidget()), actionCollection());
	KStdAction::paste(manager, SLOT(pasteWidget()), actionCollection());
	KStdAction::quit( kapp, SLOT(quit()), actionCollection());
	new KAction(i18n("Edit Tab Order"), "tab_order", KShortcut(0), manager, SLOT(editTabOrder()), actionCollection(), "taborder");
	//new KAction(i18n("Adjust Size"), "viewmagfit", KShortcut(0), manager, SLOT(adjustWidgetSize()), actionCollection(), "adjust");
	new KAction(i18n("Edit Pixmap Collection"), "icons", KShortcut(0), manager, SLOT(editFormPixmapCollection()), actionCollection(), "pixmap_collection");
	new KAction(i18n("Edit Form Connections"), "connections", KShortcut(0), manager, SLOT(editConnections()), actionCollection(), "form_connections");
	KStdAction::printPreview(this, SLOT(slotPreviewForm()), actionCollection());
#if KDE_IS_VERSION(3,1,9) && !defined(Q_WS_WIN)
	KStdAction::clear(manager, SLOT(deleteWidget()), actionCollection());
#else
	//TODO
#endif

	new KAction(i18n("Lay Out Widgets &Horizontally"), QString::null, KShortcut(0), manager, SLOT(layoutHBox()), actionCollection(), "layout_hbox");
	new KAction(i18n("Lay Out Widgets &Vertically"), QString::null, KShortcut(0), manager, SLOT(layoutVBox()), actionCollection(), "layout_vbox");
	new KAction(i18n("Lay Out Widgets in &Grid"), QString::null, KShortcut(0), manager, SLOT(layoutGrid()), actionCollection(), "layout_grid");

	new KAction(i18n("Bring Widget to Front"), "raise", KShortcut(0), manager, SLOT(bringWidgetToFront()), actionCollection(), "format_raise");
	new KAction(i18n("Send Widget to Back"), "lower", KShortcut(0), manager, SLOT(sendWidgetToBack()), actionCollection(), "format_lower");

	KActionMenu *alignMenu = new KActionMenu(i18n("Align Widgets position"), QString::null, actionCollection(), "align_menu");
	alignMenu->insert( new KAction(i18n("Align To Left"), "aoleft", KShortcut(0), manager, SLOT(alignWidgetsToLeft()), actionCollection(), "align_to_left") );
	alignMenu->insert( new KAction(i18n("Align To Right"), "aoright", KShortcut(0), manager, SLOT(alignWidgetsToRight()), actionCollection(), "align_to_right") );
	alignMenu->insert( new KAction(i18n("Align To Top"), "aotop", KShortcut(0), manager, SLOT(alignWidgetsToTop()), actionCollection(), "align_to_top") );
	alignMenu->insert( new KAction(i18n("Align To Bottom"), "aobottom", KShortcut(0), manager, SLOT(alignWidgetsToBottom()), actionCollection(), "align_to_bottom") );
	alignMenu->insert( new KAction(i18n("Align To Grid"), "grid", KShortcut(0), manager, SLOT(alignWidgetsToGrid()), actionCollection(), "align_to_grid") );

	KActionMenu *sizeMenu = new KActionMenu(i18n("Adjust Widgets size"), QString::null, actionCollection(), "adjust_size_menu");
	sizeMenu->insert( new KAction(i18n("To fit"), "viewmagfit", KShortcut(0), manager, SLOT(adjustWidgetSize()), actionCollection(), "adjust_to_fit") );
	sizeMenu->insert( new KAction(i18n("To grid"), QString::null, KShortcut(0), manager, SLOT(adjustSizeToGrid()), actionCollection(), "adjust_size_grid") );
	sizeMenu->insert( new KAction(i18n("To shortest"), QString::null, KShortcut(0), manager, SLOT(adjustHeightToSmall()), actionCollection(), "adjust_height_small") );
	sizeMenu->insert( new KAction(i18n("To tallest"), QString::null, KShortcut(0), manager, SLOT(adjustHeightToBig()), actionCollection(), "adjust_height_big") );
	sizeMenu->insert( new KAction(i18n("To narrowest"), QString::null, KShortcut(0), manager, SLOT(adjustWidthToSmall()), actionCollection(), "adjust_width_small") );
	sizeMenu->insert( new KAction(i18n("To widest"), QString::null, KShortcut(0), manager, SLOT(adjustWidthToBig()), actionCollection(), "adjust_width_big") );

	manager->createActions(actionCollection(), this);
	connect(manager, SIGNAL(createFormSlot(Form*, const QString &, const QString &)),
	   this, SLOT(slotCreateFormSlot(Form*, const QString&, const QString &)) );

	// action stuff
	connect(manager, SIGNAL(widgetSelected(Form*, bool)), SLOT(slotWidgetSelected(Form*, bool)));
	connect(manager, SIGNAL(formWidgetSelected(Form*)), SLOT(slotFormWidgetSelected(Form*)));
	connect(manager, SIGNAL(noFormSelected()), SLOT(slotNoFormSelected()));
	slotNoFormSelected();

	createGUI("kfmui.rc", true);
	setXMLFile("kfmui.rc", true);
	setAutoSaveSettings();
}

void
KFMView::slotPreviewForm()
{
	if(!manager->activeForm())
		return;
	QWidget *widg = new QWidget(w);
	manager->previewForm(manager->activeForm(), widg);
}

void
KFMView::loadUIFile(const QString &filename)
{
	manager->loadForm(false, filename);
}

void
KFMView::slotCreateFormSlot(KFormDesigner::Form *form, const QString &widget, const QString &signal)
{
	kdDebug() << "KFMView::slotCreateFormSlot()  The user wants to create a slot on Form " << form->toplevelContainer()->widget()->name() <<
	 " for widget " << widget << " connected to signal " << signal << endl;
}

void
KFMView::slotWidgetSelected(Form *form, bool multiple)
{
	enableFormActions();
	// Enable edit actions
	if(actionCollection()->action( KStdAction::name(KStdAction::Copy) ))
		actionCollection()->action(KStdAction::name(KStdAction::Copy) )->setEnabled(true);
	if(actionCollection()->action( KStdAction::name(KStdAction::Cut) ))
		actionCollection()->action( KStdAction::name(KStdAction::Cut) )->setEnabled(true);
#if KDE_IS_VERSION(3,1,9) && !defined(Q_WS_WIN)
	if(actionCollection()->action( KStdAction::name(KStdAction::Clear) ))
		actionCollection()->action( KStdAction::name(KStdAction::Clear) )->setEnabled(true);
#endif

	// 'Align Widgets' menu
	if(actionCollection()->action("align_menu"))
		actionCollection()->action("align_menu")->setEnabled(true);
	if(actionCollection()->action("align_to_left"))
		actionCollection()->action("align_to_left")->setEnabled(multiple);
	if(actionCollection()->action("align_to_right"))
		actionCollection()->action("align_to_right")->setEnabled(multiple);
	if(actionCollection()->action("align_to_top"))
		actionCollection()->action("align_to_top")->setEnabled(multiple);
	if(actionCollection()->action("align_to_bottom"))
		actionCollection()->action("align_to_bottom")->setEnabled(multiple);

	if(actionCollection()->action("adjust_size_menu"))
		actionCollection()->action("adjust_size_menu")->setEnabled(true);
	if(actionCollection()->action("format_raise"))
		actionCollection()->action("format_raise")->setEnabled(true);
	if(actionCollection()->action("format_lower"))
		actionCollection()->action("format_lower")->setEnabled(true);

	// If the widgets selected is a container, we enable layout actions
	if(!multiple)
	{
		KFormDesigner::ObjectTreeItem *item = form->objectTree()->lookup( form->selectedWidgets()->first()->name() );
		if(item && item->container())
			multiple = true;
	}
	// Layout actions
	if(actionCollection()->action("layout_hbox"))
		actionCollection()->action("layout_hbox")->setEnabled(multiple);
	if(actionCollection()->action("layout_vbox"))
		actionCollection()->action("layout_vbox")->setEnabled(multiple);
	if(actionCollection()->action("layout_grid"))
		actionCollection()->action("layout_grid")->setEnabled(multiple);
}

void
KFMView::slotFormWidgetSelected(Form *)
{
	disableWidgetActions();
	enableFormActions();
}

void
KFMView::slotNoFormSelected()
{
	disableWidgetActions();

	// Disable paste action
	if(actionCollection()->action( KStdAction::name(KStdAction::Paste) ))
		actionCollection()->action( KStdAction::name(KStdAction::Paste) )->setEnabled(false);

	// Disable 'Tools' actions
	if(actionCollection()->action("pixmap_collection"))
		actionCollection()->action("pixmap_collection")->setEnabled(false);
	if(actionCollection()->action("form_connections"))
		actionCollection()->action("form_connections")->setEnabled(false);
	if(actionCollection()->action("taborder"))
		actionCollection()->action("taborder")->setEnabled(false);
	if(actionCollection()->action("change_style"))
		actionCollection()->action("change_style")->setEnabled(false);

	// Disable items in 'File'
	if(actionCollection()->action( KStdAction::name(KStdAction::Save) ))
		actionCollection()->action( KStdAction::name(KStdAction::Save) )->setEnabled(false);
	if(actionCollection()->action( KStdAction::name(KStdAction::SaveAs) ))
		actionCollection()->action( KStdAction::name(KStdAction::SaveAs) )->setEnabled(false);
	if(actionCollection()->action( KStdAction::name(KStdAction::PrintPreview) ))
		actionCollection()->action( KStdAction::name(KStdAction::PrintPreview) )->setEnabled(false);
	if(actionCollection()->action( KStdAction::name(KStdAction::SaveAs) ))
		actionCollection()->action( KStdAction::name(KStdAction::SaveAs) )->setEnabled(false);
}

void
KFMView::enableFormActions()
{
	// Enable 'Tools' actions
	if(actionCollection()->action("pixmap_collection"))
		actionCollection()->action("pixmap_collection")->setEnabled(true);
	if(actionCollection()->action("form_connections"))
		actionCollection()->action("form_connections")->setEnabled(true);
	if(actionCollection()->action("taborder"))
		actionCollection()->action("taborder")->setEnabled(true);
	if(actionCollection()->action("change_style"))
		actionCollection()->action("change_style")->setEnabled(true);

	// Enable items in 'File'
	if(actionCollection()->action( KStdAction::name(KStdAction::Save) ))
		actionCollection()->action( KStdAction::name(KStdAction::Save) )->setEnabled(true);
	if(actionCollection()->action( KStdAction::name(KStdAction::SaveAs) ))
		actionCollection()->action( KStdAction::name(KStdAction::SaveAs) )->setEnabled(true);
	if(actionCollection()->action( KStdAction::name(KStdAction::PrintPreview) ))
		actionCollection()->action( KStdAction::name(KStdAction::PrintPreview) )->setEnabled(true);
	if(actionCollection()->action( KStdAction::name(KStdAction::SaveAs) ))
		actionCollection()->action( KStdAction::name(KStdAction::SaveAs) )->setEnabled(true);

	if(actionCollection()->action( KStdAction::name(KStdAction::Paste) ))
		actionCollection()->action( KStdAction::name(KStdAction::Paste) )->setEnabled(manager->isPasteEnabled());
}

void
KFMView::disableWidgetActions()
{
	// Disable edit actions
	if(actionCollection()->action( KStdAction::name(KStdAction::Copy) ))
		actionCollection()->action(KStdAction::name(KStdAction::Copy) )->setEnabled(false);
	if(actionCollection()->action( KStdAction::name(KStdAction::Cut) ))
		actionCollection()->action( KStdAction::name(KStdAction::Cut) )->setEnabled(false);
#if KDE_IS_VERSION(3,1,9) && !defined(Q_WS_WIN)
	if(actionCollection()->action( KStdAction::name(KStdAction::Clear) ))
		actionCollection()->action( KStdAction::name(KStdAction::Clear) )->setEnabled(false);
#endif

	// Disable format functions
	if(actionCollection()->action("align_menu"))
		actionCollection()->action("align_menu")->setEnabled(false);
	if(actionCollection()->action("adjust_size_menu"))
		actionCollection()->action("adjust_size_menu")->setEnabled(false);
	if(actionCollection()->action("format_raise"))
		actionCollection()->action("format_raise")->setEnabled(false);
	if(actionCollection()->action("format_lower"))
		actionCollection()->action("format_lower")->setEnabled(false);
	if(actionCollection()->action("layout_hbox"))
		actionCollection()->action("layout_hbox")->setEnabled(false);
	if(actionCollection()->action("layout_vbox"))
		actionCollection()->action("layout_vbox")->setEnabled(false);
	if(actionCollection()->action("layout_grid"))
		actionCollection()->action("layout_grid")->setEnabled(false);
}

KFMView::~KFMView()
{
}

#include "kfmview.moc"
