/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDBAUTOFIELD_H
#define KEXIDBAUTOFIELD_H

#include <qwidget.h>
#include <kexidb/field.h>
#include <formeditor/container.h>
#include "kexiformdataiteminterface.h"

class QBoxLayout;
class QLabel;

//! Universal "Auto Field" widget for Kexi forms
/*! It acts as a container for most data-aware widgets. */
class KEXIFORMUTILS_EXPORT KexiDBAutoField : 
	public QWidget,
	public KexiFormDataItemInterface,
	public KFormDesigner::DesignTimeDynamicChildWidgetHandler
{
	Q_OBJECT
//'caption' is uncovered now	Q_PROPERTY(QString labelCaption READ caption WRITE setCaption DESIGNABLE true)
	Q_OVERRIDE(QString caption READ caption WRITE setCaption DESIGNABLE true)
	Q_PROPERTY(bool autoCaption READ hasAutoCaption WRITE setAutoCaption DESIGNABLE true)
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
	Q_PROPERTY(LabelPosition labelPosition READ labelPosition WRITE setLabelPosition DESIGNABLE true)
	Q_PROPERTY(WidgetType widgetType READ widgetType WRITE setWidgetType DESIGNABLE true)
	/*internal, for design time only*/
	Q_PROPERTY(int fieldTypeInternal READ fieldTypeInternal WRITE setFieldTypeInternal DESIGNABLE true STORED false)
	Q_PROPERTY(QString fieldCaptionInternal READ fieldCaptionInternal WRITE setFieldCaptionInternal DESIGNABLE true STORED false)
	Q_ENUMS( WidgetType LabelPosition )

	public:
		enum WidgetType { Auto = 100, Text, Integer, Double, Boolean, Date, Time, DateTime,
			MultiLineText, Enum, Image };
		enum LabelPosition { Left = 300, Top, NoLabel };

		KexiDBAutoField(const QString &text, WidgetType type, LabelPosition pos, QWidget *parent = 0, const char *name = 0, bool designMode = true);
		KexiDBAutoField(QWidget *parent = 0, const char *name = 0, bool designMode = true);

		virtual ~KexiDBAutoField();

		inline QString dataSource() const { return KexiFormDataItemInterface::dataSource(); }
		inline QCString dataSourceMimeType() const { return KexiFormDataItemInterface::dataSourceMimeType(); }
		virtual void setDataSource( const QString &ds );
		virtual void setDataSourceMimeType(const QCString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
		virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);

		virtual void setInvalidState(const QString& text);
		virtual bool isReadOnly() const;
		virtual void setReadOnly( bool readOnly );

		virtual QVariant value();
		virtual bool valueIsNull();
		virtual bool valueIsEmpty();
		virtual bool valueIsValid();
		virtual bool valueChanged();
		virtual void clear();

		//! Reimpelmented to also install \a listenter for internal editor
		virtual void installListener(KexiDataItemChangesListener* listener);

		WidgetType widgetType() const { return m_widgetType_property; }
		void setWidgetType(WidgetType type);

		LabelPosition labelPosition() const { return m_lblPosition; }
		void setLabelPosition(LabelPosition position);

		QString caption() const { return m_caption; }
		void setCaption(const QString &caption);

		bool hasAutoCaption() const { return m_autoCaption; }
		void setAutoCaption(bool autoCaption);

		QWidget* editor() const { return m_editor; }
		QLabel* label() const { return m_label; }

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();

		static WidgetType widgetTypeForFieldType(KexiDB::Field::Type type);

		/*! On design time it is not possible to pass a reference to KexiDB::Field object
		 so we're just providing field type. Only used when widget type is Auto.
		 @internal */
		void setFieldTypeInternal(int kexiDBFieldType);

		/*! On design time it is not possible to pass a reference to KexiDB::Field object
		 so we're just providing field caption. Only used when widget type is Auto.
		 @internal */
		void setFieldCaptionInternal(const QString& text);

		/*! @internal */
		int fieldTypeInternal() const { return m_fieldTypeInternal; }

		/*! @internal */
		QString fieldCaptionInternal() const { return m_fieldCaptionInternal; }

		virtual QSize sizeHint() const;
		virtual void setFocusPolicy ( FocusPolicy policy );

	protected:
		virtual void setValueInternal(const QVariant&add, bool removeOld);
		void init(const QString &text, WidgetType type, LabelPosition pos);
		void createEditor();
		void changeText(const QString &text, bool beautify = true);
//		virtual void paintEvent( QPaintEvent* pe );
		void updateInformationAboutUnboundField();

	protected slots:
		void slotValueChanged();
		virtual void paletteChange( const QPalette& oldPal );

	private:
		WidgetType m_widgetType; //!< internal: equal to m_widgetType_property ir equal to result 
		                         //!< of widgetTypeForFieldType() if widgetTypeForFieldType is Auto
		WidgetType  m_widgetType_property; //!< provides widget type or Auto
		LabelPosition  m_lblPosition;
		QBoxLayout  *m_layout;
		QLabel  *m_label;
		QWidget *m_editor;
		QString  m_caption;
		KexiDB::Field::Type m_fieldTypeInternal;
		QString m_fieldCaptionInternal;
		bool m_autoCaption : 1;
		bool m_focusPolicyChanged : 1;
		bool m_designMode : 1;
};

#endif
