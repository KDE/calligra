/* This file is part of the KDE project
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

#include <kexidataiteminterface.h>

/**
KexiDBInputWidget is a special widget that appears in several different ways.
It's behaviour depends on the property "dataType".
It will be able to behave as a LineEdit, SpinBox, ComboBox and other widgets
that allows the user to enter data.

@author Christian Nitschkowski
*/
class KexiDBInputWidget : public QWidget, public KexiDataItemInterface
{
	Q_OBJECT
	Q_OVERRIDE(QString text DESIGNABLE false)
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(WidgetType widgetType READ widgetType WRITE setWidgetType DESIGNABLE true)
	Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly DESIGNABLE true)
	Q_ENUMS( WidgetType )

	public:
	
		enum WidgetType {
			Undefined,
			Date,
			DateTime,
			Decimal,
			Integer,
			MultiLineText,
			Text,
			Time
		};
	
		KexiDBInputWidget( WidgetType type, QWidget *parent = 0, const char *name = 0 );
		KexiDBInputWidget( QWidget *parent = 0, const char *name = 0 );

		virtual ~KexiDBInputWidget();

		inline QString dataSource() const { return KexiDataItemInterface::dataSource(); }
		virtual QVariant value();
		virtual void setInvalidState( const QString& text );

		WidgetType widgetType() const {
			return p_widgetType;
		}

		bool readOnly() const {
			return p_readOnly;
		}

	public slots:
		void setReadOnly( bool );
		void setWidgetType( WidgetType type ) {
			const bool differ = type != p_widgetType;
			p_widgetType = type;
			if ( differ) {
				reinit();
			}
		}
		
		inline void setDataSource( const QString &ds ) { KexiDataItemInterface::setDataSource( ds ); }

		void setMinValue( int value );
		void setMaxValue( int value );
		void setMinValue( double value );
		void setMaxValue( double value );
		void setMaxLength( int value );
		void setPrecision( int precision );

	protected:
		virtual void setValueInternal( const QVariant& value );
	
	protected slots:
		void slotValueChanged( const QString& );
		void slotValueChanged();

	private:
		static double getDoubleRangeFromPrecision( int precision );
		void reinit();

	private:
		int p_precision;
		int p_intMin;
		int p_intMax;
		double p_doubleMin;
		double p_doubleMax;
		int p_maxStringLength;
		WidgetType p_widgetType;
		QWidget* p_widget;
		bool p_readOnly;
	};

#endif
