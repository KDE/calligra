/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

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

#ifndef KFORMDESIGNERWIDGETFACTORY_H
#define KFORMDESIGNERWIDGETFACTORY_H


#include <qobject.h>
#include <qptrlist.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

// class QPixmap;

class QWidget;

namespace KFormDesigner {


class WidgetFactory;
class Container;

/**
 * this class holds properties of widgets
 */
class KFORMEDITOR_EXPORT Widget
{
	public:
		Widget(WidgetFactory *f=0) {m_factory = f; }
		virtual ~Widget() { }

		/**
		 * returns a pixmap associated with the widget
		 */
		virtual QPixmap	pixmap() { return m_pixmap; }

		/**
		 * returns the class name of a widget e.g. 'QLineEdit'
		 */
		virtual QString	className() { return m_class; }

		/**
		 * returns the real name e.g. 'Line Edit'
		 */
		virtual QString	name() { return m_name; }

		/**
		 * this is executed at doubleclick on the widget
		 */
		virtual void	properties() {; }

		virtual WidgetFactory *factory() { return m_factory; }

		void		setPixmap(const QPixmap &p) { m_pixmap = p; }
		void		setClassName(const QString &s) { m_class = s; }
		void		setName(const QString &n) { m_name = n; }

	private:
		QPixmap		m_pixmap;
		QString		m_class;
		QString		m_name;
		WidgetFactory	*m_factory;

};

typedef QPtrList<Widget> WidgetList;


/**
 * this is a poor virtual class, used for making widgets
 * avaible to the WidgetLibrary.
 * you can either make a KPart module out of it or call @ref WidgetLibrary::addFactory()
 */
class KFORMEDITOR_EXPORT WidgetFactory : public QObject
{
	Q_OBJECT
	public:
		WidgetFactory(QObject *parent=0, const char *name=0);
		virtual ~WidgetFactory();

		/**
		 * returns the name of the factory
		 */
		virtual QString		name()=0;

		/**
		 * retruns all classes, which are provided by this factory
		 */
		virtual	WidgetList	classes()=0;

		/**
		 * creates a widget (and if needed a container)
		 * @returns the created widgets
		 * @param classname the classname of the widget, which should get created
		 * @param parent the parent for the created widgets
		 * @param name the name of the created widget
		 * @param toplevel the toplevelcontainer (if a container should get created)
		 */
		virtual QWidget*	create(const QString &classname, QWidget *parent, const char *name,
					 KFormDesigner::Container *container)=0;
					 
		virtual void		createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container)=0;
};

}
#endif
