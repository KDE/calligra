/***************************************************************************
 *   Copyright (C) 2003 by Lucijan Busch          lucijan@kde.org          *
 *   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 ***************************************************************************/

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

#include <kiconloader.h>
#include <kgenericfactory.h>
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

#if !KDE_IS_VERSION(3,1,9) //TMP
# define KTabWidget QTabWidget
# define KInputDialog QInputDialog
# include <qtabwidget.h>
# include <qinputdialog.h>
# include <qlineedit.h>
#else
# include <ktabwidget.h>
# include <kinputdialog.h>
#endif

/*
class KFORMEDITOR_EXPORT MyTabWidget : public KTabWidget
{
	public:
		MyTabWidget(QWidget *parent, const char *name, QObject *container)
		 : KTabWidget(parent, name)
		{
			m_container = container;

			QObjectList *list = new QObjectList(*children());
			for(QObject *obj = list->first(); obj; obj = list->next())
			{
				if(obj->isA("KTabBar"))
					obj->installEventFilter(this);
			}
			delete list;
		}
		~MyTabWidget() {;}

		void setContainer(QObject *container)
		{
			m_container = container;
		}
		virtual bool eventFilter(QObject *o, QEvent *ev)
		{
			if((!m_container) || (ev->type() != QEvent::MouseButtonRelease))
				return KTabWidget::eventFilter(o, ev);

			QMouseEvent *mev = static_cast<QMouseEvent*>(ev);
			if(mev->button() != RightButton)
				return KTabWidget::eventFilter(o, ev);

			bool ok = m_container->eventFilter(this, ev);
			if(!ok)
				return KTabWidget::eventFilter(o, ev);
			return true;
		}

	private:
		QGuardedPtr<QObject>   m_container;
};*/

HBox::HBox(QWidget *parent, const char *name)
 : QFrame(parent, name), m_preview(false)
{}

void
HBox::paintEvent(QPaintEvent *)
{
	if(m_preview) return;
	QPainter p(this);
	p.setPen(QPen(red, 1, Qt::DashLine));
	p.drawRect(0, 0, width()-1, height() - 1);
}

VBox::VBox(QWidget *parent, const char *name)
 : QFrame(parent, name), m_preview(false)
{}

void
VBox::paintEvent(QPaintEvent *)
{
	if(m_preview) return;
	QPainter p(this);
	p.setPen(QPen(blue, 1, Qt::DashLine));
	p.drawRect(0, 0, width()-1, height() - 1);
}

Grid::Grid(QWidget *parent, const char *name)
 : QFrame(parent, name), m_preview(false)
{}

void
Grid::paintEvent(QPaintEvent *)
{
	if(m_preview) return;
	QPainter p(this);
	p.setPen(QPen(darkGreen, 1, Qt::DashLine));
	p.drawRect(0, 0, width()-1, height() - 1);
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
	KFormDesigner::Container *m_container = m_form->objectTree()->lookup(m_containername)->container();
	QWidget *parent = m_form->objectTree()->lookup(m_parentname)->widget();
	if(m_name.isEmpty())
		m_name = m_container->form()->objectTree()->genName(m_container->form()->manager()->lib()->displayName("QWidget"));

	QWidget *page = new QWidget(parent, m_name.latin1());
	new KFormDesigner::Container(m_container, page, parent);

	QString classname = parent->className();
	if(classname == "KTabWidget")
	{
		KTabWidget *tab = (KTabWidget *)parent;
		QString n = QString(i18n("Page %1").arg(tab->count() + 1));
		tab->addTab(page, n);
		tab->showPage(page);

		KFormDesigner::ObjectTreeItem *item = m_container->form()->objectTree()->lookup(m_name);
		item->addModProperty("title", n);
	}
	else if(classname == "QWidgetStack")
	{
		QWidgetStack *stack = (QWidgetStack*)parent;
		stack->addWidget(page, m_pageid);
		stack->raiseWidget(page);
		m_pageid = stack->id(page);

		KFormDesigner::ObjectTreeItem *item = m_container->form()->objectTree()->lookup(m_name);
		item->addModProperty("id", stack->id(page));
	}
}

void
InsertPageCommand::unexecute()
{
	QWidget *page = m_form->objectTree()->lookup(m_name)->widget();
	QWidget *parent = m_form->objectTree()->lookup(m_parentname)->widget();

	QtWidgetList list;
	list.append(page);
	KCommand *com = new KFormDesigner::DeleteWidgetCommand(list, m_form);

	QString classname = parent->className();
	if(classname == "KTabWidget")
	{
		KTabWidget *tab = (KTabWidget *)parent;
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

/////   The factory /////////////////////////

ContainerFactory::ContainerFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	m_classes.setAutoDelete(true);

	KFormDesigner::Widget *wBtnGroup = new KFormDesigner::Widget(this);
	wBtnGroup->setPixmap("frame");
	wBtnGroup->setClassName("QButtonGroup");
	wBtnGroup->setName(i18n("Button Group"));
	wBtnGroup->setNamePrefix(i18n("ButtonGroup"));
	wBtnGroup->setDescription(i18n("A simple container to group buttons"));
	m_classes.append(wBtnGroup);

	KFormDesigner::Widget *wTabWidget = new KFormDesigner::Widget(this);
	wTabWidget->setPixmap("tabwidget");
	wTabWidget->setClassName("KTabWidget");
	#if KDE_IS_VERSION(3,1,9) //TMP
	wTabWidget->setAlternateClassName("QTabWidget");
	#endif
	wTabWidget->setInclude("ktabwidget.h");
	wTabWidget->setName(i18n("Tab Widget"));
	wTabWidget->setNamePrefix(i18n("TabWidget"));
	wTabWidget->setDescription(i18n("A widget to display multiple pages using tabs"));
	m_classes.append(wTabWidget);

	KFormDesigner::Widget *wWidget = new KFormDesigner::Widget(this);
	wWidget->setPixmap("frame");
	wWidget->setClassName("QWidget");
	wWidget->setName(i18n("Basic container"));
	wWidget->setNamePrefix(i18n("BasicContainer"));
	wWidget->setDescription(i18n("An empty container with no frame"));
	m_classes.append(wWidget);

	KFormDesigner::Widget *wGroupBox = new KFormDesigner::Widget(this);
	wGroupBox->setPixmap("groupbox");
	wGroupBox->setClassName("QGroupBox");
	wGroupBox->setName(i18n("Group Box"));
	wGroupBox->setNamePrefix(i18n("GroupBox"));
	wGroupBox->setDescription(i18n("A container to group some widgets"));
	m_classes.append(wGroupBox);

	KFormDesigner::Widget *wFrame = new KFormDesigner::Widget(this);
	wFrame->setPixmap("frame");
	wFrame->setClassName("QFrame");
	wFrame->setName(i18n("Frame"));
	wFrame->setNamePrefix(i18n("Frame"));
	wFrame->setDescription(i18n("A very simple container"));
	m_classes.append(wFrame);

	KFormDesigner::Widget *wWidgetStack = new KFormDesigner::Widget(this);
	wWidgetStack->setPixmap("widgetstack");
	wWidgetStack->setClassName("QWidgetStack");
	wWidgetStack->setName(i18n("Widget Stack"));
	wWidgetStack->setNamePrefix(i18n("WidgetStack"));
	wWidgetStack->setDescription(i18n("A container with multiple pages"));
	m_classes.append(wWidgetStack);

	KFormDesigner::Widget *wHBox = new KFormDesigner::Widget(this);
	wHBox->setPixmap("frame");
	wHBox->setClassName("HBox");
	wHBox->setName(i18n("Horizontal Box"));
	wHBox->setNamePrefix(i18n("HorizontalBox"));
	wHBox->setDescription(i18n("A simple container to group widgets horizontally"));
	m_classes.append(wHBox);

	KFormDesigner::Widget *wVBox = new KFormDesigner::Widget(this);
	wVBox->setPixmap("frame");
	wVBox->setClassName("VBox");
	wVBox->setName(i18n("Vertical Box"));
	wVBox->setNamePrefix(i18n("VerticalBox"));
	wVBox->setDescription(i18n("A simple container to group widgets vertically"));
	m_classes.append(wVBox);

	KFormDesigner::Widget *wGrid = new KFormDesigner::Widget(this);
	wGrid->setPixmap("frame");
	wGrid->setClassName("Grid");
	wGrid->setName(i18n("Grid Box"));
	wGrid->setNamePrefix(i18n("GridBox"));
	wGrid->setDescription(i18n("A simple container to group widgets in a grid"));
	m_classes.append(wGrid);
}

QString
ContainerFactory::name()
{
	return("containers");
}

KFormDesigner::WidgetList
ContainerFactory::classes()
{
	return m_classes;
}

QWidget*
ContainerFactory::create(const QString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	kdDebug() << "ContainerFactory::create() " << this << endl;

	if(c == "QButtonGroup")
	{
		QString text = container->form()->manager()->lib()->textForWidgetName(n, c);
		QButtonGroup *w = new QButtonGroup(/*i18n("Button Group")*/text, p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "KTabWidget")
	{
		//MyTabWidget *tab = new MyTabWidget(p, n, container);
		KTabWidget *tab = new KTabWidget(p, n);
#if KDE_IS_VERSION(3,1,9)
		tab->setTabReorderingEnabled(true);
#endif
		connect(tab, SIGNAL(movedTab(int,int)), this, SLOT(reorderTabs(int,int)));
		container->form()->objectTree()->addChild(container->tree(), new KFormDesigner::ObjectTreeItem(
		        container->form()->manager()->lib()->displayName(c), n, tab, container));
		m_manager = container->form()->manager();

		if(container->form()->interactiveMode())
		{
			m_widget=tab;
			m_container=container;
			AddTabPage();
		}

		return tab;
	}
	else if(c == "QWidget")
	{
		QWidget *w = new QWidget(p, n);
		new KFormDesigner::Container(container, w, p);
		return w;
	}
	else if(c == "QGroupBox")
	{
		QString text = container->form()->manager()->lib()->textForWidgetName(n, c);
		QGroupBox *w = new QGroupBox(/*i18n("Group Box")*/text, p, n);
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
		container->form()->objectTree()->addChild(container->tree(), new KFormDesigner::ObjectTreeItem(
		     container->form()->manager()->lib()->displayName(c), n, stack, container));

		if(container->form()->interactiveMode())
		{
			m_widget = stack;
			m_container = container;
			AddStackPage();
		}
		return stack;
	}
	else if(c == "HBox")
	{
		HBox *w = new HBox(p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "VBox")
	{
		VBox *w = new VBox(p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "Grid")
	{
		Grid *w = new Grid(p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}

	return 0;
}

void
ContainerFactory::previewWidget(const QString &classname, QWidget *widget, KFormDesigner::Container *container)
{
	if(classname == "WidgetStack")
	{
		QWidgetStack *stack = ((QWidgetStack*)widget);
		KFormDesigner::ObjectTreeItem *tree = container->form()->objectTree()->lookup(widget->name());
		if(!tree->modifProp()->contains("frameShape"))
			stack->setFrameStyle(QFrame::NoFrame);
	}
	else if(classname == "HBox")
		((HBox*)widget)->setPreviewMode();
	else if(classname == "VBox")
		((VBox*)widget)->setPreviewMode();
	else if(classname == "Grid")
		((Grid*)widget)->setPreviewMode();
}

bool
ContainerFactory::createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container, QValueVector<int> *menuIds)
{
	m_widget = w;
	m_container = container;

	if(classname == "KTabWidget")
	{
		int id = menu->insertItem(SmallIconSet("tab_new"), i18n("Add Page"), this, SLOT(AddTabPage()) );
		menuIds->append(id);
		id = menu->insertItem(SmallIconSet("edit"), i18n("Rename Page"), this, SLOT(renameTabPage()));
		menuIds->append(id);
		id = menu->insertItem(SmallIconSet("tab_remove"), i18n("Remove Page"), this, SLOT(removeTabPage()));
		menuIds->append(id);
		if( ((KTabWidget*)w)->count() == 1)
			menu->setItemEnabled(id, false);
		return true;
	}
	else if(w->parentWidget()->parentWidget()->inherits("QTabWidget"))
	{
		m_widget = w->parentWidget()->parentWidget();
		m_container = m_container->toplevel();
		int id = menu->insertItem(SmallIconSet("tab_new"), i18n("Add Page"), this, SLOT(AddTabPage()) );
		menuIds->append(id);
		id = menu->insertItem(SmallIconSet("edit"), i18n("Rename Page"), this, SLOT(renameTabPage()));
		menuIds->append(id);
		id = menu->insertItem(SmallIconSet("tab_remove"), i18n("Remove Page"), this, SLOT(removeTabPage()));
		menuIds->append(id);
		if( ((KTabWidget*)m_widget)->count() == 1)
			menu->setItemEnabled(id, false);
		return true;
	}
	else if(w->parentWidget()->isA("QWidgetStack") && !w->parentWidget()->parentWidget()->inherits("QTabWidget"))
	{
		m_widget = w->parentWidget();
		QWidgetStack *stack = (QWidgetStack*)m_widget;
		m_container = container->form()->objectTree()->lookup(m_widget->name())->parent()->container();

		int id = menu->insertItem(SmallIconSet("tab_new"), i18n("Add Page"), this, SLOT(AddStackPage()) );
		menuIds->append(id);

		id = menu->insertItem(SmallIconSet("tab_remove"), i18n("Remove Page"), this, SLOT(removeStackPage()) );
		menuIds->append(id);
		if( ((QWidgetStack*)m_widget)->children()->count() == 4) // == the stack has only one page
			menu->setItemEnabled(id, false);

		id = menu->insertItem(SmallIconSet("next"), i18n("Jump to Next Page"), this, SLOT(nextStackPage()));
		menuIds->append(id);
		if(!stack->widget(stack->id(stack->visibleWidget())+1))
			menu->setItemEnabled(id, false);

		id = menu->insertItem(SmallIconSet("previous"), i18n("Jump to Previous Page"), this, SLOT(prevStackPage()));
		menuIds->append(id);
		if(!stack->widget(stack->id(stack->visibleWidget()) -1) )
			menu->setItemEnabled(id, false);
		return true;
	}
	return false;
}

void
ContainerFactory::startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;
	if(classname == "QButtonGroup")
	{
		QButtonGroup *group = static_cast<QButtonGroup*>(w);
		QRect r = QRect(group->x()+2, group->y()-5, group->width()-10, w->fontMetrics().height() + 10);
		createEditor(group->title(), group, container, r, Qt::AlignAuto);
		return;
	}
	if(classname == "QGroupBox")
	{
		QGroupBox *group = static_cast<QGroupBox*>(w);
		QRect r = QRect(group->x()+2, group->y()-5, group->width()-10, w->fontMetrics().height() + 10);
		createEditor(group->title(), group, container, r, Qt::AlignAuto);
		return;
	}
	return;
}

void
ContainerFactory::saveSpecialProperty(const QString &, const QString &name, const QVariant &, QWidget *w, QDomElement &parentNode, QDomDocument &parent)
{
	if((name == "title") && (w->parentWidget()->parentWidget()->inherits("QTabWidget")))
	{
		QTabWidget *tab = (QTabWidget*)w->parentWidget()->parentWidget();
		KFormDesigner::FormIO::saveProperty(parentNode, parent, "attribute", "title", tab->tabLabel(w));
	}
	else if((name == "id") && (w->parentWidget()->isA("QWidgetStack")))
	{
		QWidgetStack *stack = (QWidgetStack*)w->parentWidget();
		KFormDesigner::FormIO::saveProperty(parentNode, parent, "attribute", "id", stack->id(w));
	}
}

void
ContainerFactory::readSpecialProperty(const QString &, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *item)
{
	QString name = node.attribute("name");
	if((name == "title") && (item->parent()->widget()->inherits("QTabWidget")))
	{
		QTabWidget *tab = (QTabWidget*)w->parentWidget();
		tab->addTab(w, node.firstChild().toElement().text());
		item->addModProperty("title", node.firstChild().toElement().text());
	}
	else if((name == "id") && (w->parentWidget()->isA("QWidgetStack")))
	{
		QWidgetStack *stack = (QWidgetStack*)w->parentWidget();
		int id = KFormDesigner::FormIO::readProp(node.firstChild(), w, name).toInt();
		stack->addWidget(w, id);
		stack->raiseWidget(w);
		item->addModProperty("id", id);
	}
}

QStringList
ContainerFactory::autoSaveProperties(const QString &)
{
	return QStringList();
}

bool
ContainerFactory::showProperty(const QString &classname, QWidget *, const QString &property, bool multiple)
{
	if((classname == "HBox") || (classname == "VBox") || (classname == "Grid"))
	{
		return ((property == "name") || (property == "geometry"));
	}
	else
		return !multiple;
}

void
ContainerFactory::changeText(const QString &text)
{
	changeProperty("title", text, m_container);
}

void
ContainerFactory::resizeEditor(QWidget *widget, const QString classname)
{
	QSize s = widget->size();
	m_editor->move(widget->x() + 2, widget->y() - 5);
	m_editor->resize(s.width() - 20, widget->fontMetrics().height() +10);
}

// Widget Specific slots used in menu items

void ContainerFactory::AddTabPage()
{
	if (!m_widget->inherits("QTabWidget")){ return ;}
	KCommand *com = new InsertPageCommand(m_container, m_widget);
	if(((KTabWidget*)m_widget)->count() == 0)
	{
		com->execute();
		delete com;
	}
	else
		m_container->form()->addCommand(com, true);
}

void ContainerFactory::removeTabPage()
{
	if (!m_widget->inherits("QTabWidget")){ return ;}
	QTabWidget *tab = (QTabWidget *)m_widget;
	QWidget *w = tab->currentPage();

	QtWidgetList list;
	list.append(w);
	KCommand *com = new KFormDesigner::DeleteWidgetCommand(list, m_container->form());
	tab->removePage(w);
	m_container->form()->addCommand(com, true);
}

void ContainerFactory::renameTabPage()
{
	if (!m_widget->inherits("QTabWidget")){ return ;}
	QTabWidget *tab = (QTabWidget *)m_widget;
	QWidget *w = tab->currentPage();
	bool ok;

	QString name = KInputDialog::getText(i18n("New Page Title"), i18n("Enter a new title for the current page"),
#if !KDE_IS_VERSION(3,1,9) //TMP
	       QLineEdit::Normal,
#endif
	       tab->tabLabel(w), &ok, w->topLevelWidget());
	if(ok)
		tab->changeTab(w, name);
}

void ContainerFactory::reorderTabs(int oldpos, int newpos)
{
	KFormDesigner::ObjectTreeItem *tab = m_manager->activeForm()->objectTree()->lookup(sender()->name());
	if(!tab)
		return;

	kdDebug() << "ContainerFactory reordering tabs for " << tab->name() << endl;
	KFormDesigner::ObjectTreeItem *item = tab->children()->take(oldpos);
	tab->children()->insert(newpos, item);
}

void ContainerFactory::AddStackPage()
{
	if (!m_widget->isA("QWidgetStack")){ return ;}
	KCommand *com = new InsertPageCommand(m_container, m_widget);
	if(!((QWidgetStack*)m_widget)->visibleWidget())
	{
		com->execute();
		delete com;
	}
	else
		m_container->form()->addCommand(com, true);
}

void ContainerFactory::removeStackPage()
{
	if (!m_widget->isA("QWidgetStack")){ return ;}
	QWidgetStack *stack = (QWidgetStack*)m_widget;
	QWidget *page = stack->visibleWidget();

	QtWidgetList list;
	list.append(page);
	KCommand *com = new KFormDesigner::DeleteWidgetCommand(list, m_container->form());

	int id = stack->id(page) - 1;
	while(!stack->widget(id))
		id--;
	stack->raiseWidget(id);

	stack->removeWidget(page);
	m_container->form()->addCommand(com, true);
}

void ContainerFactory::prevStackPage()
{
	QWidgetStack *stack = (QWidgetStack*)m_widget;
	int id = stack->id(stack->visibleWidget()) - 1;
	if(stack->widget(id))
		stack->raiseWidget(id);
}

void ContainerFactory::nextStackPage()
{
	QWidgetStack *stack = (QWidgetStack*)m_widget;
	int id = stack->id(stack->visibleWidget()) + 1;
	if(stack->widget(id))
		stack->raiseWidget(id);
}

ContainerFactory::~ContainerFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(containers, KGenericFactory<ContainerFactory>)

#include "containerfactory.moc"
