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

#ifndef FORMIO_H
#define FORMIO_H

#include <qobject.h>
#include <qstring.h>

class QString;
class QDomElement;
class QDomNode;
class QDomDocument;
class QVariant;

namespace KFormDesigner {

class ObjectPropertyBuffer;
class Form;
class ObjectTreeItem;
class Container;
class WidgetLibrary;

class KEXIPROPERTYEDITOR_EXPORT FormIO : public QObject
{
	Q_OBJECT
	
	public:
		FormIO(QObject *parent, const char *name);
		~FormIO(){;}
		
		static int saveForm(Form *form, const QString &filename=QString::null);
		static int loadForm(Form *form, QWidget *parent, const QString &filename=QString::null);

	protected:
		static QDomElement  prop(QDomDocument &parent, const char *name, const QVariant &value, QWidget *w);
		static QVariant     readProp(QDomNode node, QObject *obj, const QString &name);
		static void         readAttribute(QDomNode node, QObject *obj, const QString &name);
		
		static void         saveWidget(ObjectTreeItem *item, QDomElement &parent, QDomDocument &domDoc);
		static void         loadWidget(Container *container, WidgetLibrary *lib, const QDomElement &el, QWidget *parent=0);
		static void         createToplevelWidget(Form *form, QWidget *parent, QDomElement &element);

		static QString      saveImage(QDomDocument &domDoc, const QPixmap &pixmap);
		static QPixmap      loadImage(QDomDocument domDoc, QString name);
};

}

#endif


