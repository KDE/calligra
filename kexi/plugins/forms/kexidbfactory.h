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

#include "kexiformdataiteminterface.h"
#include <widget/utils/kexidisplayutils.h>

#include <klineedit.h>
#include <kpushbutton.h>

class KAction;

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
		KexiSubForm(KFormDesigner::Form *parentForm, QWidget *parent, const char *name);
		~KexiSubForm() {}

		//! \return the name of the subform to display inside this widget
		QString formName() const { return m_formName; }

		//! Sets the name of the subform to display inside this widget
		void setFormName(const QString &name);

		//void  paintEvent(QPaintEvent *ev);

	private:
		Form *m_parentForm;
		Form *m_form;
		QWidget *m_widget;
		QString m_formName;
};

//! Line edit widget for Kexi forms
class KexiDBLineEdit : public KLineEdit, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)

	public:
		KexiDBLineEdit(QWidget *parent, const char *name=0);
		virtual ~KexiDBLineEdit();

		inline QString dataSource() const { return KexiFormDataItemInterface::dataSource(); }
		virtual QVariant value();
		virtual void setInvalidState( const QString& displayText );

		//! \return true if editor's value is null (not empty)
		//! Used for checking if a given constraint within table of form is met.
		virtual bool valueIsNull();

		//! \return true if editor's value is empty (not necessary null).
		//! Only few data types can accept "EMPTY" property
		//! (use KexiDB::Field::hasEmptyProperty() to check this).
		//! Used for checking if a given constraint within table or form is met.
		virtual bool valueIsEmpty();

		/*! \return 'readOnly' flag for this item. The flag is usually taken from
		 the item's widget, e.g. KLineEdit::isReadOnly().
		 By default, always returns false. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void setField(KexiDB::Field* field);

	public slots:
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }

	protected slots:
		void slotTextChanged(const QString&);

	protected:
		virtual void paintEvent ( QPaintEvent * );
		virtual void setValueInternal(const QVariant& add, bool removeOld);
		virtual bool event ( QEvent * );

		KexiDisplayUtils::DisplayParameters *m_autonumberDisplayParameters;
};

//! Push Button widget for Kexi forms
class KexiPushButton : public KPushButton
{
	Q_OBJECT
	Q_PROPERTY(QCString onClickAction READ onClickAction WRITE setOnClickAction DESIGNABLE true)

	public:
		KexiPushButton( const QString & text, QWidget * parent, const char * name = 0 );
		~KexiPushButton();

	public slots:
		QCString onClickAction() const { return m_onClickAction; }
		void setOnClickAction(const QCString& actionName) { m_onClickAction = actionName; }

	protected:
		QCString m_onClickAction;
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

	/*protected:
		virtual void  changeText(const QString &newText);
		virtual void   resizeEditor(QWidget *widget, const QString &classname);*/

	protected:
		virtual bool isPropertyVisibleInternal(const QCString &, QWidget *, const QCString &);
		QWidget *m_widget;
		KFormDesigner::Container *m_container;

		KAction* m_assignAction;
};

#endif
