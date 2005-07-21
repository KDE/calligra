/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>

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

#ifndef KPROPERTY_PROPERTYWIDGET_H
#define KPROPERTY_PROPERTYWIDGET_H

#include <qwidget.h>
#include "koproperty_global.h"

namespace KoProperty {

class WidgetPrivate;
class Property;

/*! \brief The base class for all item editors used in Editor.
  \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
*/
class KOPROPERTY_EXPORT Widget : public QWidget
{
	Q_OBJECT

	public:
		Widget(Property *property, QWidget *parent, const char *name="property_editor");
		~Widget();

		/*! \return the value currently entered in the item editor widget.*/
		virtual QVariant value() const = 0;
		/*! Sets the value shown in the item editor widget. Set emitChange to false
		if you don't want to emit propertyChanged signal.*/
		virtual void setValue(const QVariant &value, bool emitChange=true) = 0;

		virtual Property* property() const;
		/*! Sets the name of edited property.*/
		virtual void setProperty(Property *property);

		/*! Function to draw a property viewer when the item editor isn't shown.*/
		virtual void drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value);

		/*! Reverts the property value to previous setting.*/
		virtual void undo();

		/*! Sets the widget that will receive focus when the Widget is selected. */
		void  setFocusWidget(QWidget*focusProxy);

		//! \sa d->leaveTheSpaceForRevertButton description
		bool leavesTheSpaceForRevertButton() const;
		bool hasBorders() const;

	signals:
		void valueChanged(Widget *widget);
		void acceptInput(Widget *widget);
		void rejectInput(Widget *widget);

	protected:
		/*! Filters some event for main widget, eg Enter or Esc key presses. */
		virtual bool  eventFilter(QObject* watched, QEvent* e);

		void setLeavesTheSpaceForRevertButton(bool set);
		void setHasBorders(bool set);

	protected:
		WidgetPrivate  *d;
};

}

#endif
