/* This file is part of the KDE project
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KFORMDESIGNERFORMWIDGETIFACE_H
#define KFORMDESIGNERFORMWIDGETIFACE_H

#include <kexi_export.h>

namespace KFormDesigner
{

//! Base class for all form widgets.
/*! Provides "design" flag that can affect rendering of the widget and other behaviour depending 
 on whether the parent form is in design or data mode.
 */
class KFORMEDITOR_EXPORT FormWidgetInterface
{
public:
    //! Initializes the interface. The design flag is set to false by default.
    FormWidgetInterface();

    virtual ~FormWidgetInterface();

    //! @return true if this widget is in design mode 
    virtual bool designMode() const;

    //! Sets design mode on or off. Sets the arrow mouse cursor for design mode.
    virtual void setDesignMode(bool design);

protected:
    class Private;
    Private *d;
};

}

#endif
