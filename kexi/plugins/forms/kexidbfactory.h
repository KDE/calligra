/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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
#include <kexidataiteminterface.h>

#include <klineedit.h>

namespace KFormDesigner {
	class Form;
	class FormManager;
}

using KFormDesigner::Form;

//! A form embedded as a widget inside other form
class KexiSubForm : public QScrollView
{
	Q_OBJECT
	Q_PROPERTY(QString formName READ formName WRITE setFormName DESIGNABLE true)

	public:
		KexiSubForm(KFormDesigner::FormManager *manager, QWidget *parent, const char *name);
		~KexiSubForm() {}

		//! \return the name of the subform to display inside this widget
		QString formName() const { return m_formName; }

		//! Sets the name of the subform to display inside this widget
		void setFormName(const QString &name);

	private:
		KFormDesigner::FormManager *m_manager;
		Form *m_form;
		QWidget *m_widget;
		QString m_formName;
};

class KexiDBLineEdit : public KLineEdit, public KexiDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)

	public:
		KexiDBLineEdit(QWidget *parent, const char *name=0);
		virtual ~KexiDBLineEdit();

		inline QString dataSource() const { return KexiDataItemInterface::dataSource(); }
		inline void setDataSource(const QString &ds) { KexiDataItemInterface::setDataSource(ds); }
		virtual QVariant value();

	protected slots:
		void slotTextChanged(const QString&);

	protected:
		virtual void setValueInternal(const QVariant& value);
};

//! Kexi Factory (DB widgets + subform)
class KexiDBFactory : public KFormDesigner::WidgetFactory
{
	Q_OBJECT

	public:
		KexiDBFactory(QObject *parent, const char *name, const QStringList &args);
		virtual ~KexiDBFactory();

//		virtual QString	name();
		virtual QWidget *create(const QCString &, QWidget *, const char *, KFormDesigner::Container *);

		virtual bool createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu,
		   KFormDesigner::Container *container, QValueVector<int> *menuIds);
		virtual void startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container);
		virtual void previewWidget(const QString &, QWidget *, KFormDesigner::Container *) {}
		virtual void clearWidgetContent(const QString &classname, QWidget *w);

		//virtual void		saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
		         //QDomElement &parentNode, QDomDocument &parent) {}
		//virtual void            readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w, KFormDesigner::ObjectTreeItem *item) {}
		virtual bool showProperty(const QString &, QWidget *, const QString &, bool multiple) { return !multiple;}
		virtual QStringList autoSaveProperties(const QString &classname);

	/*protected:
		virtual void  changeText(const QString &newText);
		virtual void   resizeEditor(QWidget *widget, const QString &classname);*/

	protected:
		QWidget *m_widget;
		KFormDesigner::Container *m_container;
};

#endif
