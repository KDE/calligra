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

#ifndef STDWIDGETFACTORY_H
#define STDWIDGETFACTORY_H

#include <qframe.h>

#include "widgetfactory.h"
#include "container.h"

class KFORMEDITOR_EXPORT MyPicLabel : public QLabel
{
	Q_OBJECT

	public:
		MyPicLabel(const QPixmap *pix, QWidget *parent, const char *name);
		~MyPicLabel(){;}

		virtual bool setProperty(const char *name, const QVariant &value);
};

class KFORMEDITOR_EXPORT Line : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(Orientation orientation READ orientation WRITE setOrientation)

	public:
		Line(Orientation orient, QWidget *parent, const char *name);
		~Line(){;}

		void setOrientation(Orientation orient);
		Orientation  orientation() const;
};

/**
 *
 * Lucijan Busch
 **/
class StdWidgetFactory : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		StdWidgetFactory(QObject *parent, const char *name, const QStringList &args);
		~StdWidgetFactory();

		virtual QString				name();
		virtual KFormDesigner::WidgetList	classes();
		virtual QWidget				*create(const QString &, QWidget *, const char *, KFormDesigner::Container *);
		virtual bool				createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container);
		virtual void		startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container);
		virtual void		previewWidget(const QString &classname, QWidget *widget, KFormDesigner::Container *container);
		virtual void		saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
		         QDomElement &parentNode, QDomDocument &parent);
		virtual void            readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *item);
		virtual bool		showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple);
		virtual QStringList     autoSaveProperties(const QString &classname);

	public slots:
		void  editText();
		void  editListContents();

	protected:
		virtual void  changeText(const QString &newText);

	private:
		KFormDesigner::WidgetList		m_classes;
		KFormDesigner::Container		*m_container;
		QWidget 				*m_widget;
};

#endif
