/* This file is part of the KDE project

   Copyright 1999-2000 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_MAP_IFACE_H
#define KSPREAD_MAP_IFACE_H

#include <qstringlist.h>
#include <q3valuelist.h>
//Added by qt3to4:

#include <dcopobject.h>
#include <dcopref.h>


namespace KSpread
{
class Map;

class MapIface : virtual public DCOPObject
{
    K_DCOP
public:
    MapIface( Map* );

    virtual bool processDynamic(const DCOPCString &fun, const QByteArray &data,
				DCOPCString& replyType, QByteArray &replyData);

k_dcop:
    virtual DCOPRef sheet( const QString& name );
    virtual DCOPRef sheetByIndex( int index );
    virtual int sheetCount() const;
    virtual QStringList sheetNames() const;
    virtual QList<DCOPRef> sheets();
    virtual DCOPRef insertSheet( const QString& name );

private:
    Map* m_map;
};

} // namespace KSpread

#endif
