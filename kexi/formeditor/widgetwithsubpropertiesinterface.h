/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef WIDGETWITHSUBPROPERTIESINTERFACE_H
#define WIDGETWITHSUBPROPERTIESINTERFACE_H

#include <q3cstring.h>
#include <qwidget.h>
#include <qpointer.h>
#include <qvariant.h>
#include <QSet>

#include <kexi_export.h>

namespace KFormDesigner {

//! An interface for declaring form widgets to have subproperties.
/*! Currently used in KexiDBAutoField to allow editing specific properties 
 of its internal editor. For example, if the autofield is of type Image Box,
 the Image Box widget has some specific properties like "lineWidth". 
 Such properties are provided by the parent KexiDBAutoField object as subproperties. */
class KFORMEDITOR_EXPORT WidgetWithSubpropertiesInterface
{
	public:
		WidgetWithSubpropertiesInterface();
		virtual ~WidgetWithSubpropertiesInterface();

		//! Sets \a widget subwidget handling subproperties. Setting 0 clears subwidget.
//! @todo maybe someone wants to add more than one widget here?
		void setSubwidget(QWidget *widget);

		//! \return the assigned subwidget.
		QWidget* subwidget() const;

		//! \return a set of subproperties avaliable for this widget.
		QSet<Q3CString> subproperies() const;

		//! \return a metaproperty for a widget's subproperty 
		//! or invalid metaproperty if there is no such subproperty.
		QMetaProperty findMetaSubproperty(const char * name) const;

		//! \return a value of widget's subproperty. \a ok is set to true on success
		//! and to false on failure.
		QVariant subproperty( const char * name, bool &ok  ) const;

		//! Sets a subproperty value \a value for a subproperty \a name
		//! \return true on successful setting and false when there 
		//! is no such a subproperty in the subwidget or QObject::setProperty() failed.
		bool setSubproperty( const char * name, const QVariant & value );

	protected:
		QPointer<QWidget> m_subwidget;
		QSet<Q3CString> m_subproperies;
};
}

#endif
