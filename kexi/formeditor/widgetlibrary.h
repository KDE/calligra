/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
#include <qmap.h>
#include <qdict.h>

#include "widgetfactory.h"

template<class type> class QPtrList;
template<class type> class QValueVector;
class KActionCollection;
class KAction;
class QWidget;
class QPopupMenu;
class QVariant;
class QDomDocument;
class QDomElement;

namespace KFormDesigner {

class Container;
class ObjectTreeItem;
class WidgetLibraryPrivate;

typedef QPtrList<KAction> ActionList;

/**
 * This class searches for factories and provides KActions for widget creation.
 * Every widget can be located using this library.
 * You call WidgetLibrary functions instead of calling directly factories.
 * See WidgetFactory for a description of the functions.
 */
class KFORMEDITOR_EXPORT WidgetLibrary : public QObject
{
	Q_OBJECT

	public:
		/*! Constructs WidgetLibrary object. 
		 In \a supportedFactoryGroups you can provide 
		 factory group list to be supported. Factory groups are defined by 
		 "X-KFormDesigner-FactoryGroup" field in every factory serviece's .desktop file.
		 By default (when supportedFactoryGroups is empty) only factories having empty
		 "X-KFormDesigner-FactoryGroup" field will be loaded. 
		 Factory group names are case-insensitive. */
		WidgetLibrary(QObject *parent=0, const QStringList& supportedFactoryGroups = QStringList());

		~WidgetLibrary();

		/**
		 * creates actions
		 */
		ActionList	createActions(KActionCollection *parent, QObject *receiver, const char *slot);

		/**
		 * creates the XML for widget actions
		 */
		QString	createXML();

		/**
		 * searches the right factory and creates a widget.
		 * @returns the widget or 0 if something falid
		 */
		QWidget	*createWidget(const QString &w, QWidget *parent, const char *name, Container *c);

		bool createMenuActions(const QString &c, QWidget *w, QPopupMenu *menu, 
			KFormDesigner::Container *container, QValueVector<int> *menuIds);

		QString displayName(const QString &classname);
		QString namePrefix(const QString &classname);
		QString textForWidgetName(const QString &name, const QString &className);

		/*! Checks if the \a classname is an alternate classname, 
		 and returns the good classname. 
		 If \a classname is not alternate, it is returned. */
		QString checkAlternateName(const QString &classname);
		QString icon(const QString &classname);
		QString includeFileName(const QString &classname);
		QString savingName(const QString &classname);

		void startEditing(const QString &classname, QWidget *w, Container *container);
		void previewWidget(const QString &classname, QWidget *widget, Container *container);
		void clearWidgetContent(const QString &classname, QWidget *w);

		void saveSpecialProperty(const QString &classname, const QString &name, 
			const QVariant &value, QWidget *w, QDomElement &parentNode, QDomDocument &parent);
		bool readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w, 
			ObjectTreeItem *item);
		bool showProperty(const QString &classname, QWidget *w, 
			const QString &property, bool multiple);
		QStringList autoSaveProperties(const QString &classname);

		WidgetFactory* factoryForClassName(const char* className);

	signals:
		void prepareInsert(const QString &c);

	protected:
		/**
		 * add a factory to the library. this creates actions for widgets in the added factory.
		 * this function mostly won't be called directly but by the factory locater
		 */
		void addFactory(WidgetFactory *f);

#if 0 //UNIMPLEMENTED
		/**
		 * you can restrict the loaded factories by setting the filter to a pattern
		 * like 'kexi|containers' in that case only factory containing 'kexi' or containers will be loaded.
		 * this is useful if you want to embedd formeditor and provide e.g. a LineEdit with special features
		 * but don't want to confuse the user... are you confused now?
		 * NB: not implemented yet
		 */
		void setFilter(const QRegExp &expr);
#endif

		/**
		 * Lookups widget factories list (note that this function get called once in ctor)
		 */
		void lookupFactories();

		/**
		 * Loads widget factories found in lookupFactories(). This is called once.
		 */
		void loadFactories();

		WidgetLibraryPrivate *d;
};

}
#endif
