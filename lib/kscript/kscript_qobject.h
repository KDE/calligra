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

#ifndef __KSCRIPT_QOBJECT_H__
#define __KSCRIPT_QOBJECT_H__

#include <qshared.h>
#include <qguardedptr.h>
#include <qobject.h>

#include "kscript_value.h"

class KSContext;
class QObject;
class QVariant;

class KSQObject : public QShared
{
public:
    typedef KSSharedPtr<KSQObject> Ptr;

    /**
     */
    KSQObject( QObject* obj );

    virtual ~KSQObject();

    virtual KSValue::Ptr member( KSContext&, const QString& name );
    virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );

    virtual const char* className() const;

    static bool pack( KSContext& context, QVariant& var, const KSValue::Ptr& v );
    static KSValue::Ptr unpack( KSContext& context, QVariant& var );

private:
    QGuardedPtr<QObject> m_ptr;
};

#endif
