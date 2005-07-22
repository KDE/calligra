/* This file is part of the KDE project
  Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
  Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDBINPUTWIDGET_H
#define KEXIDBINPUTWIDGET_H

#include <qwidget.h>
#include "kexiformdataiteminterface.h"

class QBoxLayout;
class QLabel;

class KexiDBFieldEdit : public QWidget, public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString lblCaption READ caption WRITE setCaption DESIGNABLE true)
	Q_PROPERTY(bool autoCaption READ hasAutoCaption WRITE setAutoCaption DESIGNABLE true)
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
	Q_PROPERTY(WidgetType widgetType READ widgetType WRITE setWidgetType DESIGNABLE true)
	Q_PROPERTY(LabelPosition labelPosition READ labelPosition WRITE setLabelPosition DESIGNABLE true)
	Q_ENUMS( WidgetType LabelPosition )

	public:
		enum WidgetType { Auto = 100, Bool, Date, Time, DateTime, Double,
			Integer,  MultiLineText,  Text, Enum };
		enum LabelPosition { Top = 300, Left, NoLabel };

		KexiDBFieldEdit(const QString &text, WidgetType type, LabelPosition pos, QWidget *parent = 0, const char *name = 0 );
		KexiDBFieldEdit(QWidget *parent = 0, const char *name = 0);

		virtual ~KexiDBFieldEdit();

		inline QString dataSource() const { return KexiFormDataItemInterface::dataSource(); }
		inline QCString dataSourceMimeType() const { return KexiFormDataItemInterface::dataSourceMimeType(); }
		virtual void setDataSource( const QString &ds ) { KexiFormDataItemInterface::setDataSource(ds); }
		virtual void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
		virtual void setField(KexiDB::Field* field);

		virtual void setInvalidState(const QString& text);
		virtual bool isReadOnly() const;

		virtual QVariant value();
		virtual bool valueIsNull();
		virtual bool valueIsEmpty();
		virtual void clear();

		WidgetType widgetType() const { return m_widgetType; }
		void setWidgetType(WidgetType type);

		LabelPosition labelPosition() const { return m_lblPosition; }
		void setLabelPosition(LabelPosition position);

		QString  caption() const { return m_caption; }
		void  setCaption(const QString &caption);

		bool  hasAutoCaption() const { return m_autoCaption; }
		void  setAutoCaption(bool autoCaption);

		QWidget* editor() const { return m_editor; }
		QLabel* label() const { return m_label; }

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();

		QLabel*  label()  { return m_label; }

	protected:
		virtual void setValueInternal(const QVariant&add, bool removeOld);
		void init(const QString &text, WidgetType type, LabelPosition pos);
		void createEditor();
		void  changeText(const QString &text);
		WidgetType  widgetTypeFromFieldType();

	protected slots:
		void slotValueChanged();

	private:
		WidgetType m_widgetType;
		WidgetType  m_widgetType_property;
		LabelPosition  m_lblPosition;
		QBoxLayout  *m_layout;
		QLabel  *m_label;
		QWidget *m_editor;
		QString  m_caption;
		bool  m_autoCaption;
};

#endif

