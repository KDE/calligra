/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#ifndef __KSCRIPT_PROXY_H__
#define __KSCRIPT_PROXY_H__

#include <qshared.h>
#include <qcstring.h>

#include "kscript_value.h"

class QDataStream;
class QString;

class KSContext;

class KSProxy : public QShared
{
public:
    typedef KSSharedPtr<KSProxy> Ptr;

    /**
     */
    KSProxy( const QCString& app, const QCString& obj );

    virtual ~KSProxy();

    virtual KSValue::Ptr member( KSContext&, const QString& name );
    virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );

    QCString appId() const;
    QCString objId() const;

    static QString pack( KSContext&, QDataStream& str, KSValue::Ptr& v );
    static KSValue::Ptr unpack( KSContext&, QDataStream& str, const QCString& type );

protected:
    bool call( KSContext& context, const QString& name );

private:
    QCString m_app;
    QCString m_obj;
    bool m_propertyProxyCheckDone;
    bool m_supportsPropertyProxy;
};

#endif
