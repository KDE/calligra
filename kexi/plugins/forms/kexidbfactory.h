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
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3CString>
#include <Q3PopupMenu>

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
		KexiDBFactory(QObject *parent, const QStringList &args);
		virtual ~KexiDBFactory();

		virtual QWidget *createWidget(const Q3CString &classname, QWidget *parent, const char *name, 
			KFormDesigner::Container *container, int options = DefaultOptions );

		virtual void createCustomActions(KActionCollection* col);
		virtual bool createMenuActions(const Q3CString &classname, QWidget *w, QMenu *menu,
		   KFormDesigner::Container *container);
		virtual bool startEditing(const Q3CString &classname, QWidget *w, KFormDesigner::Container *container);
		virtual bool previewWidget(const Q3CString &, QWidget *, KFormDesigner::Container *);
		virtual bool clearWidgetContent(const Q3CString &classname, QWidget *w);

		//virtual void		saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
		         //QDomElement &parentNode, QDomDocument &parent) {}
		//virtual void            readSpecialProperty(const QCString &classname, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *item) {}
		virtual Q3ValueList<Q3CString> autoSaveProperties(const Q3CString &classname);

	protected slots:
		void slotImageBoxIdChanged(long id); /*KexiBLOBBuffer::Id_t*/

	protected:
		virtual bool changeText(const QString &newText);
		virtual void resizeEditor(QWidget *editor, QWidget *widget, const Q3CString &classname);

		virtual bool isPropertyVisibleInternal(const Q3CString& classname, QWidget *w,
			const Q3CString& property, bool isTopLevel);

		//! Sometimes property sets should be reloaded when a given property value changed.
		virtual bool propertySetShouldBeReloadedAfterPropertyChange(const Q3CString& classname, QWidget *w, 
			const Q3CString& property);

		KAction* m_assignAction;
};

#endif
