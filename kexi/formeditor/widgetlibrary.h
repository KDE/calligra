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

#ifndef KFORMDESIGNERWIDGETLIBRARY_H
#define KFORMDESIGNERWIDGETLIBRARY_H

#include <qobject.h>
#include <qdict.h>

template<class type> class QPtrList;
class KActionCollection;
class KAction;
class QWidget;
class QPopupMenu;
class QVariant;
class QDomDocument;
class QDomElement;

namespace KFormDesigner {
/**
 *
 * Lucijan Busch
 **/

class Widget;
class WidgetFactory;
class Container;

typedef QDict<Widget> Widgets;
typedef QPtrList<KAction> Actions;

/**
 * this class searches for factroies and provides KActions for widgetselection
 * every widget can be located using this library.
 * calles won't borther with factories directly than, but with the Library
 */

class KFORMEDITOR_EXPORT WidgetLibrary : public QObject
{
	Q_OBJECT

	public:
		WidgetLibrary(QObject *parent=0);
		~WidgetLibrary();

		/**
		 * add a factory to the library. this creates actions for widgets in the added factory.
		 * this function mostly won't be called directly but by the factory locater
		 */
		void	addFactory(WidgetFactory *f);

		/**
		 * you can restrict the loaded factories by setting the filter to a pattern
		 * like 'kexi|containers' in that case only factory containing 'kexi' or containers will be loaded.
		 * this is useful if you want to embedd formeditor and provide e.g. a LineEdit with special features
		 * but don't want to confuse the user... are you confused now?
		 */
		void	setFilter(const QRegExp &expr);

		/**
		 * scans for widget factories (note that this function get called by the constructor as well!)
		 */
		void	scan();

		/**
		 * creates actions
		 */
		Actions	createActions(KActionCollection *parent, QObject *receiver, const char *slot);

		/**
		 * creates the XML for widget actions
		 */
		QString	createXML();

		/**
		 * searches the right factory and creates a widget.
		 * @returns the widget or 0 if something falid
		 */
		QWidget	*createWidget(const QString &w, QWidget *parent, const char *name, Container *c);

		bool	createMenuActions(const QString &c, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container);

		QString  displayName(const QString &classname);

		void    startEditing(const QString &classname, QWidget *w, Container *container);

		void	saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
		        QDomElement &parentNode, QDomDocument &parent);
		void	readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w);
		bool    showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple);
		QStringList  autoSaveProperties(const QString &classname);

	signals:
		void	prepareInsert(const QString &c);

	private:
		// dict which associates a class name with a Widget class
		Widgets	m_widgets;
};

}
#endif
