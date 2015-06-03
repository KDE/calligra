/* This file is part of the KDE project
   Copyright (C) 2005-2008 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXICUSTOMPROPFACTORY_H
#define KEXICUSTOMPROPFACTORY_H

#include "kexiextwidgets_export.h"

#include <KPropertyFactory>

//! Kexi-specific custom property factory for the KProperty library
class KEXIEXTWIDGETS_EXPORT KexiCustomPropertyFactory : public KPropertyFactory
{
public:
    enum PropertyType {
        PixmapId = KProperty::UserDefined + 0,  //!< Shared Kexi pixmap
        Identifier = KProperty::UserDefined + 1 //!< string allowing nonempty identifiers
    };

    //! Called once to register all property and editor types provided by this factory.
    static void init();

    KexiCustomPropertyFactory();
};

#endif
