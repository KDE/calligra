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

#ifndef CONTAINERFACTORY_H
#define CONTAINERFACTORY_H

#if !KDE_IS_VERSION(3,1,9) //TMP
# define KTabWidget QTabWidget
#include <qtabwidget.h>
#else
#include <ktabwidget.h>
#endif

#include <kcommand.h>

#include "widgetfactory.h"

namespace KFormDesigner
{
	class Form;
	class FormManager;
	class Container;
}

class InsertPageCommand : public KCommand
{
	public:
		InsertPageCommand(KFormDesigner::Container *container, QWidget *widget);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

	protected:
		KFormDesigner::Form *m_form;
		QString  m_containername;
		QString  m_name;
		QString  m_parentname;
		int      m_pageid;
};

//! Helper widget (used when using 'Lay out horizontally')
class KFORMEDITOR_EXPORT HBox : public QFrame
{
	Q_OBJECT

	public:
		HBox(QWidget *parent, const char *name);
		~HBox(){;}
		void setPreviewMode() {m_preview = true;}
		void paintEvent(QPaintEvent *ev);

	protected:
		bool  m_preview;
};

//! Helper widget (used when using 'Lay out vertically')
class KFORMEDITOR_EXPORT VBox : public QFrame
{
	Q_OBJECT

	public:
		VBox(QWidget *parent, const char *name);
		~VBox(){;}
		void setPreviewMode() {m_preview = true;}
		void paintEvent(QPaintEvent *ev);

	protected:
		bool  m_preview;
};

//! Helper widget (used when using 'Lay out in a grid')
class KFORMEDITOR_EXPORT Grid : public QFrame
{
	Q_OBJECT

	public:
		Grid(QWidget *parent, const char *name);
		~Grid(){;}
		void setPreviewMode() {m_preview = true;}
		void paintEvent(QPaintEvent *ev);

	protected:
		bool  m_preview;
};

class KFORMEDITOR_EXPORT MyTabWidget : public KTabWidget
{
	Q_OBJECT

	public:
		MyTabWidget(QWidget *parent, const char *name)
		 : KTabWidget(parent, name)
		{}
		~MyTabWidget() {;}

		virtual QSize sizeHint() const
		{
			return size() + QSize(0, tabBar()->height());
		}
};

//! A form embedded as a widget inside other form
class KFORMEDITOR_EXPORT SubForm : public QScrollView
{
	Q_OBJECT
	Q_PROPERTY(QString formName READ formName WRITE setFormName DESIGNABLE true);

	public:
		SubForm(KFormDesigner::FormManager *manager, QWidget *parent, const char *name);
		~SubForm() {}

		//! \return the name of the subform inside the db
		QString   formName() const { return m_formName; }
		void      setFormName(const QString &name);

	private:
		KFormDesigner::FormManager *m_manager;
		KFormDesigner::Form   *m_form;
		QWidget  *m_widget;
		QString   m_formName;
};

//! Standard Factory for all container widgets
class ContainerFactory : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		ContainerFactory(QObject *parent, const char *name, const QStringList &args);
		~ContainerFactory();

		virtual QWidget				*create(const QString &, QWidget *, const char *, KFormDesigner::Container *);
		virtual bool				createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu,
		   KFormDesigner::Container *container, QValueVector<int> *menuIds);
		virtual void		startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container);
		virtual void		previewWidget(const QString &classname, QWidget *widget, KFormDesigner::Container *container);
		virtual bool		showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple);
		virtual void    	saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
		                        QDomElement &parentNode, QDomDocument &parent);
		virtual bool            readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *item);
		virtual QStringList     autoSaveProperties(const QString &classname);

	protected:
		virtual void  changeText(const QString &newText);
		virtual void   resizeEditor(QWidget *widget, const QString &classname);

	public slots:
		void AddTabPage();
		void AddStackPage();
		void renameTabPage();
		void removeTabPage();
		void removeStackPage();
		void prevStackPage();
		void nextStackPage();
		void reorderTabs(int oldpos, int newpos);

	private:
		QWidget *m_widget;
		KFormDesigner::Container *m_container;
		KFormDesigner::FormManager  *m_manager;
};

#endif
