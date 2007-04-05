/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2006-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include <qwidgetstack.h>
#include <qframe.h>
#include <qbuttongroup.h>
#include <qwidget.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qstring.h>
#include <qpopupmenu.h>
#include <qdom.h>
#include <qevent.h>
#include <qobjectlist.h>
#include <qpainter.h>
#include <qvaluevector.h>
#include <qfileinfo.h>
#include <qscrollview.h>
#include <qtabbar.h>
#include <qsplitter.h>
#include <qlayout.h>

#include <kiconloader.h>
#include <kgenericfactory.h>
#include <ktextedit.h>
#include <klineedit.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdeversion.h>

#include "containerfactory.h"
#include "container.h"
#include "form.h"
#include "formIO.h"
#include "objecttree.h"
#include "commands.h"
#include "formmanager.h"
#include "widgetlibrary.h"
#include <formeditor/utils.h>

#if KDE_VERSION < KDE_MAKE_VERSION(3,1,9)
# define KInputDialog QInputDialog
# include <qinputdialog.h>
# include <qlineedit.h>
#else
# include <kinputdialog.h>
#endif

ContainerWidget::ContainerWidget(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
}

ContainerWidget::~ContainerWidget()
{
}

QSize ContainerWidget::sizeHint() const
{
	return QSize(30,30); //default
}

void ContainerWidget::dragMoveEvent( QDragMoveEvent *e )
{
	QWidget::dragMoveEvent(e);
	emit handleDragMoveEvent(e);
}

void ContainerWidget::dropEvent( QDropEvent *e )
{
	QWidget::dropEvent(e);
	emit handleDropEvent(e);
}

////////////////////////

GroupBox::GroupBox(const QString & title, QWidget *parent, const char *name)
 : QGroupBox(title, parent, name)
{
}

GroupBox::~GroupBox()
{
}

void GroupBox::dragMoveEvent( QDragMoveEvent *e )
{
	QGroupBox::dragMoveEvent(e);
	emit handleDragMoveEvent(e);
}

void GroupBox::dropEvent( QDropEvent *e )
{
	QGroupBox::dropEvent(e);
	emit handleDropEvent(e);
}

////////////////////////

KFDTabWidget::KFDTabWidget(QWidget *parent, const char *name)
 : KFormDesigner::TabWidget(parent, name)
{
}

KFDTabWidget::~KFDTabWidget()
{
}

QSize
KFDTabWidget::sizeHint() const
{
	QSize s(30,30); // default min size
	for(int i=0; i < count(); i++)
		s = s.expandedTo( KFormDesigner::getSizeFromChildren(page(i)) );

	return s + QSize(10/*margin*/, tabBar()->height() + 20/*margin*/);
}

void KFDTabWidget::dragMoveEvent( QDragMoveEvent *e )
{
	TabWidgetBase::dragMoveEvent( e );
	if (dynamic_cast<ContainerWidget*>(currentPage()))
		emit dynamic_cast<ContainerWidget*>(currentPage())->handleDragMoveEvent(e);
	emit handleDragMoveEvent(e);
}

void KFDTabWidget::dropEvent( QDropEvent *e ) 
{
	TabWidgetBase::dropEvent( e );
	if (dynamic_cast<ContainerWidget*>(currentPage()))
		emit dynamic_cast<ContainerWidget*>(currentPage())->handleDropEvent(e);
	emit handleDropEvent(e);
}

/// Various layout widgets /////////////////:

HBox::HBox(QWidget *parent, const char *name)
 : QFrame(parent, name), m_preview(false)
{}

void
HBox::paintEvent(QPaintEvent *)
{
	if(m_preview) return;
	QPainter p(this);
	p.setPen(QPen(red, 2, Qt::DashLine));
	p.drawRect(1, 1, width()-1, height() - 1);
}

VBox::VBox(QWidget *parent, const char *name)
 : QFrame(parent, name), m_preview(false)
{}

void
VBox::paintEvent(QPaintEvent *)
{
	if(m_preview) return;
	QPainter p(this);
	p.setPen(QPen(blue, 2, Qt::DashLine));
	p.drawRect(1, 1, width()-1, height() - 1);
}

Grid::Grid(QWidget *parent, const char *name)
 : QFrame(parent, name), m_preview(false)
{}

void
Grid::paintEvent(QPaintEvent *)
{
	if(m_preview) return;
	QPainter p(this);
	p.setPen(QPen(darkGreen, 2, Qt::DashLine));
	p.drawRect(1, 1, width()-1, height() - 1);
}

HFlow::HFlow(QWidget *parent, const char *name)
 : QFrame(parent, name), m_preview(false)
{}

void
HFlow::paintEvent(QPaintEvent *)
{
	if(m_preview) return;
	QPainter p(this);
	p.setPen(QPen(magenta, 2, Qt::DashLine));
	p.drawRect(1, 1, width()-1, height() - 1);
}

VFlow::VFlow(QWidget *parent, const char *name)
 : QFrame(parent, name), m_preview(false)
{}

void
VFlow::paintEvent(QPaintEvent *)
{
	if(m_preview) return;
	QPainter p(this);
	p.setPen(QPen(cyan, 2, Qt::DashLine));
	p.drawRect(1, 1, width()-1, height() - 1);
}

QSize
VFlow::sizeHint() const
{
	if(layout())
		return layout()->sizeHint();
	else
		return QSize(700, 50); // default
}

///////  Tab related KCommand (to allow tab creation/deletion undoing)

InsertPageCommand::InsertPageCommand(KFormDesigner::Container *container, QWidget *parent)
  : KCommand()
{
	m_containername = container->widget()->name();
	m_form = container->form();
	m_parentname = parent->name();
	m_pageid = -1;
}

void
InsertPageCommand::execute()
{
	KFormDesigner::Container *container = m_form->objectTree()->lookup(m_containername)->container();
	QWidget *parent = m_form->objectTree()->lookup(m_parentname)->widget();
	if(m_name.isEmpty()) {
		m_name = container->form()->objectTree()->generateUniqueName(
			container->form()->library()->displayName("QWidget").latin1(),
			/*!numberSuffixRequired*/false);
	}

	QWidget *page = container->form()->library()->createWidget("QWidget", parent, m_name.latin1(), container);
//	QWidget *page = new ContainerWidget(parent, m_name.latin1());
//	new KFormDesigner::Container(container, page, parent);

	QCString classname = parent->className();
	if(classname == "KFDTabWidget")
	{
		TabWidgetBase *tab = dynamic_cast<TabWidgetBase*>(parent);
		QString n = i18n("Page %1").arg(tab->count() + 1);
		tab->addTab(page, n);
		tab->showPage(page);

		KFormDesigner::ObjectTreeItem *item = container->form()->objectTree()->lookup(m_name);
		item->addModifiedProperty("title", n);
	}
	else if(classname == "QWidgetStack")
	{
		QWidgetStack *stack = (QWidgetStack*)parent;
		stack->addWidget(page, m_pageid);
		stack->raiseWidget(page);
		m_pageid = stack->id(page);

		KFormDesigner::ObjectTreeItem *item = container->form()->objectTree()->lookup(m_name);
		item->addModifiedProperty("id", stack->id(page));
	}
}

void
InsertPageCommand::unexecute()
{
	QWidget *page = m_form->objectTree()->lookup(m_name)->widget();
	QWidget *parent = m_form->objectTree()->lookup(m_parentname)->widget();

	KFormDesigner::WidgetList list;
	list.append(page);
	KCommand *com = new KFormDesigner::DeleteWidgetCommand(list, m_form);

	QCString classname = parent->className();
	if(classname == "KFDTabWidget")
	{
		TabWidgetBase *tab = dynamic_cast<TabWidgetBase*>(parent);
		tab->removePage(page);
	}
	else if(classname == "QWidgetStack")
	{
		QWidgetStack *stack = (QWidgetStack*)parent;
		int id = stack->id(page) - 1;
		while(!stack->widget(id))
			id--;

		stack->raiseWidget(id);
		stack->removeWidget(page);
	}

	com->execute();
	delete com;
}

QString
InsertPageCommand::name() const
{
	return i18n("Add Page");
}

/////// Sub forms ////////////////////////:

SubForm::SubForm(QWidget *parent, const char *name)
: QScrollView(parent, name), m_form(0), m_widget(0)
{
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	viewport()->setPaletteBackgroundColor(colorGroup().mid());
}

void
SubForm::setFormName(const QString &name)
{
	if(name.isEmpty())
		return;

	QFileInfo info(name);
	if(!info.exists()
		|| (KFormDesigner::FormManager::self()->activeForm()
			&& (info.fileName() == KFormDesigner::FormManager::self()->activeForm()->filename()) ) )
		return; // we check if this is valid form

	// we create the container widget
	delete m_widget;
	m_widget = new QWidget(viewport(), "subform_widget");
//	m_widget->show();
	addChild(m_widget);
	m_form = new KFormDesigner::Form(
		KFormDesigner::FormManager::self()->activeForm()->library(), this->name());
	m_form->createToplevel(m_widget);

	// and load the sub form
	KFormDesigner::FormIO::loadFormFromFile(m_form, m_widget, name);
	m_form->setDesignMode(false);

	m_formName = name;

}

/////   The factory /////////////////////////

ContainerFactory::ContainerFactory(QObject *parent, const char *, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, "containers")
{
	KFormDesigner::WidgetInfo *wBtnGroup = new KFormDesigner::WidgetInfo(this);
	wBtnGroup->setPixmap("frame");
	wBtnGroup->setClassName("QButtonGroup");
	wBtnGroup->setName(i18n("Button Group"));
	wBtnGroup->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "buttonGroup"));
	wBtnGroup->setDescription(i18n("A simple container to group buttons"));
	addClass(wBtnGroup);

	KFormDesigner::WidgetInfo *wTabWidget = new KFormDesigner::WidgetInfo(this);
	wTabWidget->setPixmap("tabwidget");
	wTabWidget->setClassName("KFDTabWidget");
#if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9)
	wTabWidget->addAlternateClassName("KTabWidget");
	wTabWidget->addAlternateClassName("QTabWidget");
//tmp:	wTabWidget->setSavingName("QTabWidget");
	wTabWidget->setSavingName("KTabWidget");
#else
	wTabWidget->setSavingName("QTabWidget");
#endif
	wTabWidget->setIncludeFileName("ktabwidget.h");
	wTabWidget->setName(i18n("Tab Widget"));
	wTabWidget->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "tabWidget"));
	wTabWidget->setDescription(i18n("A widget to display multiple pages using tabs"));
	addClass(wTabWidget);

	KFormDesigner::WidgetInfo *wWidget = new KFormDesigner::WidgetInfo(this);
	wWidget->setPixmap("frame");
	wWidget->setClassName("QWidget");
	wWidget->addAlternateClassName("ContainerWidget");
	wWidget->setName(i18n("Basic container"));
	wWidget->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "container"));
	wWidget->setDescription(i18n("An empty container with no frame"));
	addClass(wWidget);

	KFormDesigner::WidgetInfo *wGroupBox = new KFormDesigner::WidgetInfo(this);
	wGroupBox->setPixmap("groupbox");
	wGroupBox->setClassName("QGroupBox");
	wGroupBox->addAlternateClassName("GroupBox");
	wGroupBox->setName(i18n("Group Box"));
	wGroupBox->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "groupBox"));
	wGroupBox->setDescription(i18n("A container to group some widgets"));
	addClass(wGroupBox);

	KFormDesigner::WidgetInfo *wFrame = new KFormDesigner::WidgetInfo(this);
	wFrame->setPixmap("frame");
	wFrame->setClassName("QFrame");
	wFrame->setName(i18n("Frame"));
	wFrame->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "frame"));
	wFrame->setDescription(i18n("A simple frame container"));
	addClass(wFrame);

	KFormDesigner::WidgetInfo *wWidgetStack = new KFormDesigner::WidgetInfo(this);
	wWidgetStack->setPixmap("widgetstack");
	wWidgetStack->setClassName("QWidgetStack");
	wWidgetStack->setName(i18n("Widget Stack"));
	wWidgetStack->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "widgetStack"));
	wWidgetStack->setDescription(i18n("A container with multiple pages"));
	addClass(wWidgetStack);

	KFormDesigner::WidgetInfo *wHBox = new KFormDesigner::WidgetInfo(this);
	wHBox->setPixmap("frame");
	wHBox->setClassName("HBox");
	wHBox->setName(i18n("Horizontal Box"));
	wHBox->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "horizontalBox"));
	wHBox->setDescription(i18n("A simple container to group widgets horizontally"));
	addClass(wHBox);

	KFormDesigner::WidgetInfo *wVBox = new KFormDesigner::WidgetInfo(this);
	wVBox->setPixmap("frame");
	wVBox->setClassName("VBox");
	wVBox->setName(i18n("Vertical Box"));
	wVBox->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "verticalBox"));
	wVBox->setDescription(i18n("A simple container to group widgets vertically"));
	addClass(wVBox);

	KFormDesigner::WidgetInfo *wGrid = new KFormDesigner::WidgetInfo(this);
	wGrid->setPixmap("frame");
	wGrid->setClassName("Grid");
	wGrid->setName(i18n("Grid Box"));
	wGrid->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "gridBox"));
	wGrid->setDescription(i18n("A simple container to group widgets in a grid"));
	addClass(wGrid);

	KFormDesigner::WidgetInfo *wSplitter = new KFormDesigner::WidgetInfo(this);
//! @todo horizontal/vertical splitter icons
	wSplitter->setPixmap("frame");
	wSplitter->setClassName("Splitter");
	wSplitter->addAlternateClassName("QSplitter");
	wSplitter->setName(i18n("Splitter"));
	wSplitter->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "splitter"));
	wSplitter->setDescription(i18n("A container that enables user to resize its children"));
	addClass(wSplitter);

	KFormDesigner::WidgetInfo *wHFlow = new KFormDesigner::WidgetInfo(this);
//! @todo hflow icon
	wHFlow->setPixmap("frame");
	wHFlow->setClassName("HFlow");
	wHFlow->setName(i18n("Row Layout"));
	wHFlow->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "rowLayout"));
	wHFlow->setDescription(i18n("A simple container to group widgets by rows"));
	addClass(wHFlow);

	KFormDesigner::WidgetInfo *wVFlow = new KFormDesigner::WidgetInfo(this);
//! @todo vflow icon
	wVFlow->setPixmap("frame");
	wVFlow->setClassName("VFlow");
	wVFlow->setName(i18n("Column Layout"));
	wVFlow->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "columnLayout"));
	wVFlow->setDescription(i18n("A simple container to group widgets by columns"));
	addClass(wVFlow);

	KFormDesigner::WidgetInfo *wSubForm = new KFormDesigner::WidgetInfo(this);
	wSubForm->setPixmap("form");
	wSubForm->setClassName("SubForm");
	wSubForm->setName(i18n("Sub Form"));
	wSubForm->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "subForm"));
	wSubForm->setDescription(i18n("A form widget included in another Form"));
	wSubForm->setAutoSyncForProperty( "formName", false );
	addClass(wSubForm);

	//groupbox
	m_propDesc["title"] = i18n("Title");
	m_propDesc["flat"] = i18n("Flat");

	//tab widget
	m_propDesc["tabPosition"] = i18n("Tab Position");
	m_propDesc["currentPage"] = i18n("Current Page");
	m_propDesc["tabShape"] = i18n("Tab Shape");

	m_propDesc["tabPosition"] = i18n("Tab Position");
	m_propDesc["tabPosition"] = i18n("Tab Position");

	m_propValDesc["Rounded"] = i18n("for Tab Shape", "Rounded");
	m_propValDesc["Triangular"] = i18n("for Tab Shape", "Triangular");
}

QWidget*
ContainerFactory::createWidget(const QCString &c, QWidget *p, const char *n,
	KFormDesigner::Container *container, int options)
{
	if(c == "QButtonGroup")
	{
		QString text = container->form()->library()->textForWidgetName(n, c);
		QButtonGroup *w = new QButtonGroup(/*i18n("Button Group")*/text, p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "KFDTabWidget")
	{
		KFDTabWidget *tab = new KFDTabWidget(p, n);
#if defined(USE_KTabWidget) && KDE_VERSION >= KDE_MAKE_VERSION(3,1,9)
		tab->setTabReorderingEnabled(true);
		connect(tab, SIGNAL(movedTab(int,int)), this, SLOT(reorderTabs(int,int)));
#endif
		container->form()->objectTree()->addItem(container->objectTree(),
			new KFormDesigner::ObjectTreeItem(
				container->form()->library()->displayName(c), n, tab, container));
//		m_manager = container->form()->manager();

		// if we are loading, don't add this tab
		if(container->form()->interactiveMode())
		{
			//m_widget=tab;
			setWidget(tab, container);
//			m_container=container;
			addTabPage();
		}

		return tab;
	}
	else if(c == "QWidget" || c=="ContainerWidget")
	{
		QWidget *w = new ContainerWidget(p, n);
		new KFormDesigner::Container(container, w, p);
		return w;
	}
	else if(c == "QGroupBox" || c == "GroupBox")
	{
		QString text = container->form()->library()->textForWidgetName(n, c);
		QGroupBox *w = new GroupBox(text, p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "QFrame")
	{
		QFrame *w = new QFrame(p, n);
		w->setLineWidth(2);
		w->setFrameStyle(QFrame::StyledPanel|QFrame::Raised);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "QWidgetStack")
	{
		QWidgetStack *stack = new QWidgetStack(p, n);
		stack->setLineWidth(2);
		stack->setFrameStyle(QFrame::StyledPanel|QFrame::Raised);
		container->form()->objectTree()->addItem( container->objectTree(),
			new KFormDesigner::ObjectTreeItem(
				container->form()->library()->displayName(c), n, stack, container));

		if(container->form()->interactiveMode())
		{
			//m_widget = stack;
			setWidget(stack, container);
//			m_container = container;
			addStackPage();
		}
		return stack;
	}
	else if(c == "HBox") {
		HBox *w = new HBox(p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "VBox") {
		VBox *w = new VBox(p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "Grid") {
		Grid *w = new Grid(p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "HFlow") {
		HFlow *w = new HFlow(p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "VFlow") {
		VFlow *w = new VFlow(p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "SubForm") {
		SubForm *subform = new SubForm(p, n);
		return subform;
	}
	else if(c == "QSplitter") {
		QSplitter *split = new QSplitter(p, n);
		if (0 == (options & WidgetFactory::AnyOrientation))
			split->setOrientation(
				(options & WidgetFactory::VerticalOrientation) ? Qt::Vertical : Qt::Horizontal);
		new KFormDesigner::Container(container, split, container);
		return split;
	}

	return 0;
}

bool
ContainerFactory::previewWidget(const QCString &classname, QWidget *widget, KFormDesigner::Container *container)
{
	if(classname == "WidgetStack")
	{
		QWidgetStack *stack = ((QWidgetStack*)widget);
		KFormDesigner::ObjectTreeItem *tree = container->form()->objectTree()->lookup(widget->name());
		if(!tree->modifiedProperties()->contains("frameShape"))
			stack->setFrameStyle(QFrame::NoFrame);
	}
	else if(classname == "HBox")
		((HBox*)widget)->setPreviewMode();
	else if(classname == "VBox")
		((VBox*)widget)->setPreviewMode();
	else if(classname == "Grid")
		((Grid*)widget)->setPreviewMode();
	else if(classname == "HFlow")
		((HFlow*)widget)->setPreviewMode();
	else if(classname == "VFlow")
		((VFlow*)widget)->setPreviewMode();
	else
		return false;
	return true;
}

bool
ContainerFactory::createMenuActions(const QCString &classname, QWidget *w, QPopupMenu *menu,
	KFormDesigner::Container *container)
{
	setWidget(w, container);
	//m_widget = w;
//	m_container = container;

	if((classname == "KFDTabWidget") || (w->parentWidget()->parentWidget()->inherits("QTabWidget")))
	{
		if(w->parentWidget()->parentWidget()->inherits("QTabWidget"))
		{
			//m_widget = w->parentWidget()->parentWidget();
			setWidget(w->parentWidget()->parentWidget(), m_container->toplevel());
//			m_container = m_container->toplevel();
		}

		int id = menu->insertItem(SmallIconSet("tab_new"), i18n("Add Page"), this, SLOT(addTabPage()) );
		id = menu->insertItem(SmallIconSet("edit"), i18n("Rename Page..."), this, SLOT(renameTabPage()));
		id = menu->insertItem(SmallIconSet("tab_remove"), i18n("Remove Page"), this, SLOT(removeTabPage()));
//		if( dynamic_cast<TabWidgetBase*>(m_widget)->count() == 1)
		if( dynamic_cast<TabWidgetBase*>(widget())->count() == 1)
			menu->setItemEnabled(id, false);
		return true;
	}
	else if(w->parentWidget()->isA("QWidgetStack") && !w->parentWidget()->parentWidget()->inherits("QTabWidget"))
	{
		//m_widget = w->parentWidget();
		QWidgetStack *stack = (QWidgetStack*)w->parentWidget(); //m_widget;
		setWidget(
			w->parentWidget(),
			container->form()->objectTree()->lookup(stack->name())->parent()->container()
		);
//		m_container = container->form()->objectTree()->lookup(m_widget->name())->parent()->container();
//		m_container = container->form()->objectTree()->lookup(stack->name())->parent()->container();

		int id = menu->insertItem(SmallIconSet("tab_new"), i18n("Add Page"), this, SLOT(addStackPage()) );

		id = menu->insertItem(SmallIconSet("tab_remove"), i18n("Remove Page"), this, SLOT(removeStackPage()) );
//		if( ((QWidgetStack*)m_widget)->children()->count() == 4) // == the stack has only one page
		if(stack->children()->count() == 4) // == the stack has only one page
			menu->setItemEnabled(id, false);

		id = menu->insertItem(SmallIconSet("next"), i18n("Jump to Next Page"), this, SLOT(nextStackPage()));
		if(!stack->widget(stack->id(stack->visibleWidget())+1))
			menu->setItemEnabled(id, false);

		id = menu->insertItem(SmallIconSet("previous"), i18n("Jump to Previous Page"), this, SLOT(prevStackPage()));
		if(!stack->widget(stack->id(stack->visibleWidget()) -1) )
			menu->setItemEnabled(id, false);
		return true;
	}
	return false;
}

bool
ContainerFactory::startEditing(const QCString &classname, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;
	if(classname == "QButtonGroup")
	{
		QButtonGroup *group = static_cast<QButtonGroup*>(w);
		QRect r = QRect(group->x()+2, group->y()-5, group->width()-10, w->fontMetrics().height() + 10);
		createEditor(classname, group->title(), group, container, r, Qt::AlignAuto);
		return true;
	}
	if(classname == "QGroupBox" || classname == "GroupBox")
	{
		QGroupBox *group = static_cast<QGroupBox*>(w);
		QRect r = QRect(group->x()+2, group->y()-5, group->width()-10, w->fontMetrics().height() + 10);
		createEditor(classname, group->title(), group, container, r, Qt::AlignAuto);
		return true;
	}
	return false;
}

bool
ContainerFactory::saveSpecialProperty(const QCString &, const QString &name, const QVariant &, QWidget *w, QDomElement &parentNode, QDomDocument &parent)
{
	if((name == "title") && (w->parentWidget()->parentWidget()->inherits("QTabWidget")))
	{
		TabWidgetBase *tab = dynamic_cast<TabWidgetBase*>(w->parentWidget()->parentWidget());
		KFormDesigner::FormIO::savePropertyElement(parentNode, parent, "attribute", "title", tab->tabLabel(w));
	}
	else if((name == "id") && (w->parentWidget()->isA("QWidgetStack")))
	{
		QWidgetStack *stack = (QWidgetStack*)w->parentWidget();
		KFormDesigner::FormIO::savePropertyElement(parentNode, parent, "attribute", "id", stack->id(w));
	}
	else
		return false;
	return true;
}

bool
ContainerFactory::readSpecialProperty(const QCString &, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *item)
{
	QString name = node.attribute("name");
	if((name == "title") && (item->parent()->widget()->inherits("QTabWidget")))
	{
		TabWidgetBase *tab = dynamic_cast<TabWidgetBase*>(w->parentWidget());
		tab->addTab(w, node.firstChild().toElement().text());
		item->addModifiedProperty("title", node.firstChild().toElement().text());
		return true;
	}

	if((name == "id") && (w->parentWidget()->isA("QWidgetStack")))
	{
		QWidgetStack *stack = (QWidgetStack*)w->parentWidget();
		int id = KFormDesigner::FormIO::readPropertyValue(node.firstChild(), w, name).toInt();
		stack->addWidget(w, id);
		stack->raiseWidget(w);
		item->addModifiedProperty("id", id);
		return true;
	}

	return false;
}

QValueList<QCString>
ContainerFactory::autoSaveProperties(const QCString &c)
{
	QValueList<QCString> lst;
//	if(c == "SubForm")
//		lst << "formName";
	if(c == "QSplitter")
		lst << "orientation";
	return lst;
}

bool
ContainerFactory::isPropertyVisibleInternal(const QCString &classname,
	QWidget *w, const QCString &property, bool isTopLevel)
{
	bool ok = true;

	if((classname == "HBox") || (classname == "VBox") || (classname == "Grid") ||
		(classname == "HFlow") || (classname == "VFlow"))
	{
		return property == "name" || property == "geometry";
	}
	else if (classname == "QGroupBox" || classname=="GroupBox") {
		ok =
#ifdef KEXI_NO_UNFINISHED
/*! @todo Hidden for now in Kexi. "checkable" and "checked" props need adding
a fake properties which will allow to properly work in design mode, otherwise
child widgets become frozen when checked==true */
			(m_showAdvancedProperties || (property != "checkable" && property != "checked")) &&
#endif
			true
			;
	}
	else if (classname == "KFDTabWidget") {
		ok = (m_showAdvancedProperties || (property != "tabReorderingEnabled" && property != "hoverCloseButton" && property != "hoverCloseButtonDelayed"));
	}

	return ok && WidgetFactory::isPropertyVisibleInternal(classname, w, property, isTopLevel);
}

bool
ContainerFactory::changeText(const QString &text)
{
	changeProperty("title", text, m_container->form());
	return true;
}

void
ContainerFactory::resizeEditor(QWidget *editor, QWidget *widget, const QCString &)
{
	QSize s = widget->size();
	editor->move(widget->x() + 2, widget->y() - 5);
	editor->resize(s.width() - 20, widget->fontMetrics().height() +10);
}

// Widget Specific slots used in menu items

void ContainerFactory::addTabPage()
{
//	if (!m_widget->inherits("QTabWidget"))
	if (!widget()->inherits("QTabWidget"))
		return;
	KCommand *com = new InsertPageCommand(m_container, widget());
	if(dynamic_cast<TabWidgetBase*>(widget())->count() == 0)
	{
		com->execute();
		delete com;
	}
	else
		m_container->form()->addCommand(com, true);
}

void ContainerFactory::removeTabPage()
{
	if (!widget()->inherits("QTabWidget"))
		return;
	TabWidgetBase *tab = dynamic_cast<TabWidgetBase*>(widget());
	QWidget *w = tab->currentPage();

	KFormDesigner::WidgetList list;
	list.append(w);
	KCommand *com = new KFormDesigner::DeleteWidgetCommand(list, m_container->form());
	tab->removePage(w);
	m_container->form()->addCommand(com, true);
}

void ContainerFactory::renameTabPage()
{
	if (!widget()->inherits("QTabWidget"))
		return;
	TabWidgetBase *tab = dynamic_cast<TabWidgetBase*>(widget());
	QWidget *w = tab->currentPage();
	bool ok;

	QString name = KInputDialog::getText(i18n("New Page Title"), i18n("Enter a new title for the current page:"),
#if KDE_VERSION < KDE_MAKE_VERSION(3,1,9)
	       QLineEdit::Normal,
#endif
	       tab->tabLabel(w), &ok, w->topLevelWidget());
	if(ok)
		tab->changeTab(w, name);
}

void ContainerFactory::reorderTabs(int oldpos, int newpos)
{
	KFormDesigner::ObjectTreeItem *tab
		= KFormDesigner::FormManager::self()->activeForm()->objectTree()->lookup(sender()->name());
	if(!tab)
		return;

	KFormDesigner::ObjectTreeItem *item = tab->children()->take(oldpos);
	tab->children()->insert(newpos, item);
}

void ContainerFactory::addStackPage()
{
	if (!widget()->isA("QWidgetStack"))
		return;
	KCommand *com = new InsertPageCommand(m_container, widget());
	if(!((QWidgetStack*)widget())->visibleWidget())
	{
		com->execute();
		delete com;
	}
	else
		m_container->form()->addCommand(com, true);
}

void ContainerFactory::removeStackPage()
{
	if (!widget()->isA("QWidgetStack"))
		return;
	QWidgetStack *stack = (QWidgetStack*)widget();
	QWidget *page = stack->visibleWidget();

	KFormDesigner::WidgetList list;
	list.append(page);
	KCommand *com = new KFormDesigner::DeleteWidgetCommand(list, m_container->form());

	// raise prev widget
	int id = stack->id(page) - 1;
	while(!stack->widget(id))
		id--;
	stack->raiseWidget(id);

	stack->removeWidget(page);
	m_container->form()->addCommand(com, true);
}

void ContainerFactory::prevStackPage()
{
	QWidgetStack *stack = (QWidgetStack*)widget();
	int id = stack->id(stack->visibleWidget()) - 1;
	if(stack->widget(id))
		stack->raiseWidget(id);
}

void ContainerFactory::nextStackPage()
{
	QWidgetStack *stack = (QWidgetStack*)widget();
	int id = stack->id(stack->visibleWidget()) + 1;
	if(stack->widget(id))
		stack->raiseWidget(id);
}

ContainerFactory::~ContainerFactory()
{
}

KFORMDESIGNER_WIDGET_FACTORY(ContainerFactory, containers)

#include "containerfactory.moc"
