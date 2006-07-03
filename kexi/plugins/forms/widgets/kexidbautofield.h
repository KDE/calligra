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
#include <formeditor/widgetwithsubpropertiesinterface.h>
#include "kexiformdataiteminterface.h"

class QBoxLayout;
class QLabel;

//! Universal "Auto Field" widget for Kexi forms
/*! It acts as a container for most data-aware widgets. */
class KEXIFORMUTILS_EXPORT KexiDBAutoField : 
	public QWidget,
	public KexiFormDataItemInterface,
	public KFormDesigner::DesignTimeDynamicChildWidgetHandler,
	public KFormDesigner::WidgetWithSubpropertiesInterface
{
	Q_OBJECT
//'caption' is uncovered now	Q_PROPERTY(QString labelCaption READ caption WRITE setCaption DESIGNABLE true)
	Q_OVERRIDE(QString caption READ caption WRITE setCaption DESIGNABLE true)
	Q_OVERRIDE(QColor paletteForegroundColor READ paletteForegroundColor WRITE setPaletteForegroundColor DESIGNABLE true RESET unsetPalette)
	Q_OVERRIDE(QColor paletteBackgroundColor READ paletteBackgroundColor WRITE setPaletteBackgroundColor DESIGNABLE true RESET unsetPalette)
	Q_PROPERTY(bool autoCaption READ hasAutoCaption WRITE setAutoCaption DESIGNABLE true)
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(QCString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)
	Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
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

		WidgetType widgetType() const;
		void setWidgetType(WidgetType type);

		LabelPosition labelPosition() const;
		void setLabelPosition(LabelPosition position);

		QString caption() const;
		void setCaption(const QString &caption);

		bool hasAutoCaption() const;
		void setAutoCaption(bool autoCaption);

		QWidget* editor() const;
		QLabel* label() const;

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
		int fieldTypeInternal() const;

		/*! @internal */
		QString fieldCaptionInternal() const;

		virtual QSize sizeHint() const;
		virtual void setFocusPolicy ( FocusPolicy policy );

		//! Reimplemented to return internal editor's color.
		const QColor & paletteForegroundColor() const;

		//! Reimplemented to set internal editor's color.
		void setPaletteForegroundColor( const QColor & color );

		//! Reimplemented to return internal editor's color.
		const QColor & paletteBackgroundColor() const;

		//! Reimplemented to set internal editor's color.
		virtual void setPaletteBackgroundColor( const QColor & color );

		virtual QVariant property( const char * name ) const;

		//! Reimplemented to accept subproperties. @see KFormDesigner::WidgetWithSubpropertiesInterface
		virtual bool setProperty( const char * name, const QVariant & value );

	protected slots:
//		void slotValueChanged();
		virtual void paletteChange( const QPalette& oldPal );

	protected:
		virtual void setValueInternal(const QVariant&add, bool removeOld);
		void init(const QString &text, WidgetType type, LabelPosition pos);
		void createEditor();
		void changeText(const QString &text, bool beautify = true);
//		virtual void paintEvent( QPaintEvent* pe );
		void updateInformationAboutUnboundField();

		//! internal editor can be created too late, so certain properties should be copied
		void copyPropertiesToEditor();

		virtual bool eventFilter( QObject *o, QEvent *e );
	private:
		class Private;
		Private *d;
};

#endif
