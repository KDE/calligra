/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDATAITEMINTERFACE_H
#define KEXIDATAITEMINTERFACE_H

#include <qstring.h>
#include <qvariant.h>
#include <qguardedptr.h>

class KexiDataItemInterface;

class KEXICORE_EXPORT KexiDataItemChangesListener
{
	public:
		KexiDataItemChangesListener();
		~KexiDataItemChangesListener();

		//! Implement this to react for change of \a item.
		//! Called by KexiDataItemInterface::valueChanged()
		virtual void valueChanged(KexiDataItemInterface* item) = 0;
};

//! An interface for declaring widgets to be data-aware.
class KEXICORE_EXPORT KexiDataItemInterface
{
	public:
		KexiDataItemInterface();
		~KexiDataItemInterface();

		//! \return the name of the data source for this widget
		//! Data source usually means here a table or query or field name name.
		QString dataSource() const { return m_dataSource; }

		//! Sets the name of the data source for this widget
		//! Data source usually means here a table or query or field name name.
		inline void setDataSource(const QString &ds) { m_dataSource = ds; }

		//! Sets listener 
		void installListener(KexiDataItemChangesListener* listener);

		//! Sets value \a value for a widget. 
		//! Just calls setValueInternal(), but also blocks valueChanged() 
		//! as you probably don't want to react on your own change
		void setValue(const QVariant& value);

		//! \return value saved in a widget item.
		virtual QVariant value() = 0;

	protected:
		//! Sets value \a value for a widget. 
		//! Implement this method to allow setting value for this widget item.
		virtual void setValueInternal(const QVariant& value) = 0;

		//! Call this in your inplementation when value changes, 
		//! so installed listener can react on this change.
		void valueChanged();

		QString m_dataSource;
		KexiDataItemChangesListener* m_listener;
		bool m_disable_valueChanged : 1;
};

#endif
