/***************************************************************************
 *   Copyright (C) 2003 by Lucijan Busch                                   *
 *   lucijan@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 ***************************************************************************/

#include <qwidgetstack.h>
#include <qframe.h>
#include <qbuttongroup.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qstring.h>
#include <qpopupmenu.h>
#include <qdom.h>
#include <qobjectlist.h>

#include <kiconloader.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>
#include <kinputdialog.h>

#include "containerfactory.h"
#include "container.h"
#include "form.h"
#include "formIO.h"
#include "objecttree.h"
#include "commands.h"

ContainerFactory::ContainerFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	KFormDesigner::Widget *wBtnGroup = new KFormDesigner::Widget(this);
	wBtnGroup->setPixmap("frame");
	wBtnGroup->setClassName("QButtonGroup");
	wBtnGroup->setName(i18n("Button Group"));
	wBtnGroup->setDescription(i18n("A simple container to group buttons"));
	m_classes.append(wBtnGroup);

	KFormDesigner::Widget *wTabWidget = new KFormDesigner::Widget(this);
	wTabWidget->setPixmap("tabwidget");
	wTabWidget->setClassName("QTabWidget");
	wTabWidget->setName(i18n("Tab Widget"));
	wTabWidget->setDescription(i18n("A widget to display multiple pages using tabs"));
	m_classes.append(wTabWidget);

	KFormDesigner::Widget *wWidget = new KFormDesigner::Widget(this);
	wWidget->setPixmap("widget");
	wWidget->setClassName("QWidget");
	wWidget->setName(i18n("Basic container"));
	wWidget->setDescription(i18n("An empty container with no frame"));
	m_classes.append(wWidget);

	KFormDesigner::Widget *wGroupBox = new KFormDesigner::Widget(this);
	wGroupBox->setPixmap("frame");
	wGroupBox->setClassName("QGroupBox");
	wGroupBox->setName(i18n("Group Box"));
	wGroupBox->setDescription(i18n("A container to group some widgets"));
	m_classes.append(wGroupBox);

	KFormDesigner::Widget *wFrame = new KFormDesigner::Widget(this);
	wFrame->setPixmap("frame");
	wFrame->setClassName("QFrame");
	wFrame->setName(i18n("Frame"));
	wFrame->setDescription(i18n("A very simple container"));
	m_classes.append(wFrame);

	KFormDesigner::Widget *wWidgetStack = new KFormDesigner::Widget(this);
	wWidgetStack->setPixmap("frame");
	wWidgetStack->setClassName("QWidgetStack");
	wWidgetStack->setName(i18n("Widget Stack"));
	wWidgetStack->setDescription(i18n("A container with multiple pages"));
	m_classes.append(wWidgetStack);
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
		QButtonGroup *w = new QButtonGroup(i18n("Button Broup"), p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "QTabWidget")
	{
		QTabWidget *tab = new QTabWidget(p, n);
		container->form()->objectTree()->addChild(container->tree(), new KFormDesigner::ObjectTreeItem(c, n, tab));
		tab->installEventFilter(container);

		QObjectList *list = new QObjectList( *(tab->children()) );
		for(QObject *obj = list->first(); obj; obj = list->next())
			obj->installEventFilter(this);
		delete list;

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
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "QGroupBox")
	{
		QGroupBox *w = new QGroupBox(i18n("Group Box"), p, n);
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
		container->form()->objectTree()->addChild(container->tree(), new KFormDesigner::ObjectTreeItem(c, n, stack));
		stack->installEventFilter(container);

		if(container->form()->interactiveMode())
		{
			m_widget = stack;
			m_container = container;
			AddStackPage();
		}
		return stack;
	}

	return 0;
}

bool
ContainerFactory::createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container)
{
	m_widget = w;
	m_container = container;

	if(classname == "QTabWidget")
	{
		menu->insertItem(i18n("Add Page"), this, SLOT(AddTabPage()) );
		menu->insertItem(i18n("Rename Page"), this, SLOT(renameTabPage()));
		int id = menu->insertItem(i18n("Remove Page"), this, SLOT(removeTabPage()));
		if( ((QTabWidget*)w)->count() == 1)
			menu->setItemEnabled(id, false);
		return true;
	}
	else if(w->parentWidget()->isA("QWidgetStack") && !w->parentWidget()->parentWidget()->isA("QTabWidget"))
	{
		m_widget = w->parentWidget();
		QWidgetStack *stack = (QWidgetStack*)m_widget;
		m_container = container->form()->objectTree()->lookup(m_widget->name())->parent()->container();
		menu->insertItem(i18n("Add Page"), this, SLOT(AddStackPage()) );

		int id = menu->insertItem(i18n("Jump to next page"), this, SLOT(nextStackPage()));
		if(!stack->widget(stack->id(stack->visibleWidget())+1))
			menu->setItemEnabled(id, false);

		id = menu->insertItem(i18n("Jump to previous page"), this, SLOT(prevStackPage()));
		if(stack->id(stack->visibleWidget()) == 0)
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
		QRect r = QRect(group->x()+2, group->y()-5, group->width()-10, 20);
		createEditor(group->title(), group, r, Qt::AlignAuto);
		return;
	}
	if(classname == "QGroupBox")
	{
		QGroupBox *group = static_cast<QGroupBox*>(w);
		QRect r = QRect(group->x()+2, group->y()-5, group->width()-10, 20);
		createEditor(group->title(), group, r, Qt::AlignAuto);
		return;
	}
	return;
}

void
ContainerFactory::saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w, QDomElement &parentNode, QDomDocument &parent)
{
	if((name == "title") && (w->parentWidget()->isA("QTabWidget")))
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
ContainerFactory::readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w)
{
	QString name = node.attribute("name");
	if((name == "title") && (w->parentWidget()->isA("QTabWidget")))
	{
		QTabWidget *tab = (QTabWidget*)w->parentWidget();
		tab->addTab(w, node.firstChild().toElement().text());
	}
	else if((name == "id") && (w->parentWidget()->isA("QWidgetStack")))
	{
		QWidgetStack *stack = (QWidgetStack*)w->parentWidget();
		int id = KFormDesigner::FormIO::readProp(node.firstChild(), w, name).toInt();
		stack->addWidget(w, id);
	}
}

QStringList
ContainerFactory::autoSaveProperties(const QString &classname)
{
	return QStringList();
}

void
ContainerFactory::changeText(const QString &text)
{
	changeProperty("title", text, m_container);
}

void ContainerFactory::AddTabPage()
{
	if (!m_widget->isA("QTabWidget")){ return ;}
	QString type = "QWidget";
	QString name = (m_container->form()->objectTree()->genName(type));

	QTabWidget *tab = (QTabWidget *)m_widget;
	QWidget *page = new QWidget(tab,name.latin1());
	new KFormDesigner::Container(m_container,page,tab);

	QString n = QString(i18n("Page %1").arg(tab->count() + 1));
	tab->addTab(page,n);
	tab->showPage(page);

	KFormDesigner::ObjectTreeItem *item = m_container->form()->objectTree()->lookup(name);
	item->addModProperty("title", n);
}

void ContainerFactory::removeTabPage()
{
	if (!m_widget->isA("QTabWidget")){ return ;}
	QTabWidget *tab = (QTabWidget *)m_widget;
	QWidget *w = tab->currentPage();

	WidgetList list;
	list.append(w);
	KCommand *com = new KFormDesigner::DeleteWidgetCommand(list, m_container->form());
	tab->removePage(w);
	m_container->form()->addCommand(com, true);
}

void ContainerFactory::renameTabPage()
{
	if (!m_widget->isA("QTabWidget")){ return ;}
	QTabWidget *tab = (QTabWidget *)m_widget;
	QWidget *w = tab->currentPage();
	bool ok;

	QString name = KInputDialog::getText(i18n("New Page Title"), i18n("Enter a new title for the current page"),
	       tab->tabLabel(w), &ok, w->topLevelWidget());
	if(ok)
		tab->changeTab(w, name);
}

void ContainerFactory::AddStackPage()
{
	if (!m_widget->isA("QWidgetStack")){ return ;}
	QString type = "QWidget";
	QString name = (m_container->form()->objectTree()->genName(type));

	QWidgetStack *stack = (QWidgetStack*)m_widget;
	QWidget *page = new QWidget(stack, name.latin1());
	new KFormDesigner::Container(m_container, page, stack);

	stack->addWidget(page);
	KFormDesigner::ObjectTreeItem *item = m_container->form()->objectTree()->lookup(name);
	item->addModProperty("id", stack->id(page));
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
