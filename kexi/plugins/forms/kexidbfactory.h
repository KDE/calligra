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

#ifndef KEXIDBFACTORY_H
#define KEXIDBFACTORY_H

#include <widgetfactory.h>

namespace KFormDesigner {
	class Form;
	class FormManager;
}

using KFormDesigner::Form;

//! A form embedded as a widget inside other form
class KFORMEDITOR_EXPORT KexiSubForm : public QScrollView
{
	Q_OBJECT
	Q_PROPERTY(QString formName READ formName WRITE setFormName DESIGNABLE true);

	public:
		KexiSubForm(KFormDesigner::FormManager *manager, QWidget *parent, const char *name);
		~KexiSubForm() {}

		//! \return the name of the subform inside the db
		QString   formName() const { return m_formName; }
		void      setFormName(const QString &name);

	private:
		KFormDesigner::FormManager *m_manager;
		Form   *m_form;
		QWidget  *m_widget;
		QString   m_formName;
};

//! Kexi Factory (DB widgets + subform)
class KexiDBFactory : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		KexiDBFactory(QObject *parent, const char *name, const QStringList &args);
		~KexiDBFactory();

		virtual QString	name();
		virtual KFormDesigner::WidgetInfo::List classes();
		virtual QWidget *create(const QString &, QWidget *, const char *, KFormDesigner::Container *);

		virtual bool createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu,
		   KFormDesigner::Container *container, QValueVector<int> *menuIds);
		virtual void startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container);
		virtual void previewWidget(const QString &, QWidget *, KFormDesigner::Container *) {}

		//virtual void		saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
		         //QDomElement &parentNode, QDomDocument &parent) {}
		//virtual void            readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *item) {}
		virtual bool		showProperty(const QString &, QWidget *, const QString &, bool multiple) { return !multiple;}
		virtual QStringList autoSaveProperties(const QString &classname);

	/*protected:
		virtual void  changeText(const QString &newText);
		virtual void   resizeEditor(QWidget *widget, const QString &classname);*/

	private:
		KFormDesigner::WidgetInfo::List m_classes;
		QWidget *m_widget;
		KFormDesigner::Container *m_container;
};

#endif
