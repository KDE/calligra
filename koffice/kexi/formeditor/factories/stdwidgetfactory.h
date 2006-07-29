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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef STDWIDGETFACTORY_H
#define STDWIDGETFACTORY_H

#include <qframe.h>

#include "widgetfactory.h"
#include "container.h"

class KFORMEDITOR_EXPORT KexiPictureLabel : public QLabel
{
	Q_OBJECT

	public:
		KexiPictureLabel(const QPixmap &pix, QWidget *parent, const char *name);
		~KexiPictureLabel(){;}

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

//! Factory for all basic widgets, including Spring (not containers)
class StdWidgetFactory : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		StdWidgetFactory(QObject *parent, const char *name, const QStringList &args);
		~StdWidgetFactory();

		virtual QWidget *createWidget(const QCString &c, QWidget *p, const char *n, 
			KFormDesigner::Container *container, int options = DefaultOptions);

		virtual bool createMenuActions(const QCString &classname, QWidget *w, QPopupMenu *menu,
		   KFormDesigner::Container *container);
		virtual bool startEditing(const QCString &classname, QWidget *w,
			KFormDesigner::Container *container);
		virtual bool previewWidget(const QCString &classname, QWidget *widget,
			KFormDesigner::Container *container);
		virtual bool clearWidgetContent(const QCString &classname, QWidget *w);

		virtual bool saveSpecialProperty(const QCString &classname,
			const QString &name, const QVariant &value, QWidget *w,
			QDomElement &parentNode, QDomDocument &parent);
		virtual bool readSpecialProperty(const QCString &classname, QDomElement &node,
			QWidget *w, KFormDesigner::ObjectTreeItem *item);
		virtual QValueList<QCString> autoSaveProperties(const QCString &classname);

		virtual void setPropertyOptions( KFormDesigner::WidgetPropertySet& buf,
			 const KFormDesigner::WidgetInfo& info, QWidget *w );

	public slots:
		void  editText();
		void  editListContents();

	protected:
		virtual bool isPropertyVisibleInternal(const QCString &classname, QWidget *w,
			const QCString &property, bool isTopLevel);
		virtual bool changeText(const QString &newText);
		virtual void resizeEditor(QWidget *editor, QWidget *widget, const QCString &classname);
		void saveListItem(QListViewItem *item, QDomNode &parentNode, QDomDocument &domDoc);
		void readListItem(QDomElement &node, QListViewItem *parent, KListView *listview);

	private:
//		KFormDesigner::Container *m_container;
//		QWidget *m_widget;
};

#endif
