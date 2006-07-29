/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KIVIO_MAP_IFACE_H
#define KIVIO_MAP_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qvaluelist.h>
#include <qstringlist.h>

class KivioMap;

class KIvioMapIface : virtual public DCOPObject
{
    K_DCOP
public:
    KIvioMapIface( KivioMap* );

    virtual bool processDynamic(const QCString &fun, const QByteArray &data,
				QCString& replyType, QByteArray &replyData);

k_dcop:
    virtual DCOPRef page( const QString& name );
    virtual DCOPRef pageByIndex( int index );
    virtual int pageCount() const;
    virtual QStringList pageNames() const;
    virtual QValueList<DCOPRef> pages();
    virtual DCOPRef insertPage( const QString& name );

private:
    KivioMap* m_map;
};

#endif
