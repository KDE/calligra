/* This file is part of the KDE project
   
   Copyright 1999 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_APP_IFACE_H
#define KSPREAD_APP_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <QMap>
#include <QString>

namespace KSpread
{

class AppIface : public DCOPObject
{
    K_DCOP
public:
    AppIface();

k_dcop:
    virtual DCOPRef createDoc();
    virtual DCOPRef createDoc( const QString& name );
    virtual QMap<QString,DCOPRef> documents();
    virtual DCOPRef document( const QString& name );
};

} // namespace KSpread

#endif
