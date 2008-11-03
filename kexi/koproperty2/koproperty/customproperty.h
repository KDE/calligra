/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_CUSTOMPROPERTY_H
#define KPROPERTY_CUSTOMPROPERTY_H

#include "koproperty_global.h"

class QVariant;

namespace KoProperty
{

class Property;

//! \brief Custom property implementation for QSize type
class KOPROPERTY_EXPORT SizeCustomProperty : public CustomProperty
{
public:
    explicit SizeCustomProperty(Property *parent);
    ~SizeCustomProperty();

    void setValue(const QVariant &value, bool rememberOldValue);
    QVariant value() const;
    bool handleValue() const;
};

//! \brief Custom property implementation for QPoint type
class KOPROPERTY_EXPORT PointCustomProperty : public CustomProperty
{
public:
    explicit PointCustomProperty(Property *parent);
    ~PointCustomProperty();

    void setValue(const QVariant &value, bool rememberOldValue);
    QVariant value() const;
    bool handleValue() const;
};

//! \brief Custom property implementation for QRect type
class KOPROPERTY_EXPORT RectCustomProperty : public CustomProperty
{
public:
    explicit RectCustomProperty(Property *parent);
    ~RectCustomProperty();

    void setValue(const QVariant &value, bool rememberOldValue);
    QVariant value() const;
    bool handleValue() const;
};

//! \brief Custom property implementation for QSizePolicy type
class KOPROPERTY_EXPORT SizePolicyCustomProperty : public CustomProperty
{
public:
    explicit SizePolicyCustomProperty(Property *parent);
    ~SizePolicyCustomProperty();

    void setValue(const QVariant &value, bool rememberOldValue);
    QVariant value() const;
    bool handleValue() const;
};

}

#endif
