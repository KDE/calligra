/* This file is part of the KDE project
   Copyright (C) 2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXI_UTILS_PROPERTYMAP_H
#define KEXI_UTILS_PROPERTYMAP_H

#include "kexiutils_export.h"
#include <QVariant>
#include <QByteArray>

namespace KexiUtils
{

//! @short A simple property map
/*!
  Some classes may need internal property system, not defined as Qt proprety
  system and not publicly available (e.g. KexiPart class).
  Property names are not case-sensitive.
  This class is for use by other classes by inheriting.
*/
class KEXIUTILS_EXPORT InternalPropertyMap
{
public:
    InternalPropertyMap();
    ~InternalPropertyMap();

    /*! \returns property value for name \a name.
     If \a defaultValue is provided and there is no property with
     specified name, \a defaultValue is returned. */
    QVariant internalPropertyValue(const QByteArray& name,
                                   const QVariant& defaultValue = QVariant()) const;

    /*! Sets property value \a value for property \a name.
     If \a value is null, the property is deleted. */
    void setInternalPropertyValue(const QByteArray& name, const QVariant& value);

private:
    class Private;
    Private * const d;
};

}
#endif
