/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDBFACTORY_H
#define KEXIDBFACTORY_H

#include <formeditor/widgetfactory.h>

class KAction;

namespace KFormDesigner {
	class Form;
	class FormManager;
}

//! Kexi Factory (DB widgets + subform)
class KexiDBFactory : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		KexiDBFactory(QObject *parent, const char *name, const QStringList &args);
		virtual ~KexiDBFactory();

		virtual QWidget *createWidget(const QCString &classname, QWidget *parent, const char *name, 
			KFormDesigner::Container *container, int options = DefaultOptions );

		virtual void createCustomActions(KActionCollection* col);
		virtual bool createMenuActions(const QCString &classname, QWidget *w, QPopupMenu *menu,
		   KFormDesigner::Container *container);
		virtual bool startEditing(const QCString &classname, QWidget *w, KFormDesigner::Container *container);
		virtual bool previewWidget(const QCString &, QWidget *, KFormDesigner::Container *);
		virtual bool clearWidgetContent(const QCString &classname, QWidget *w);

		//virtual void		saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
		         //QDomElement &parentNode, QDomDocument &parent) {}
		//virtual void            readSpecialProperty(const QCString &classname, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *item) {}
		virtual QValueList<QCString> autoSaveProperties(const QCString &classname);

	protected slots:
		void slotImageBoxIdChanged(long id); /*KexiBLOBBuffer::Id_t*/

	protected:
		virtual bool changeText(const QString &newText);
		virtual void resizeEditor(QWidget *editor, QWidget *widget, const QCString &classname);

		virtual bool isPropertyVisibleInternal(const QCString& classname, QWidget *w,
			const QCString& property, bool isTopLevel);

		//! Sometimes property sets should be reloaded when a given property value changed.
		virtual bool propertySetShouldBeReloadedAfterPropertyChange(const QCString& classname, QWidget *w, 
			const QCString& property);

		KAction* m_assignAction;
};

#endif
