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

#ifndef KSCRIPT_EXT_QSTRUCTS_H
#define KSCRIPT_EXT_QSTRUCTS_H

#include "kscript_struct.h"

#include <qrect.h>
#include <qpoint.h>

namespace KSQt
{
    class Rect : public KSBuiltinStructClass
    {
    public:
	Rect( KSModule* module, const QString& name );

	bool constructor( KSContext& c );
	bool destructor( void* object );
	KSStruct* clone( KSBuiltinStruct* );

	static QRect convert( KSContext& context, const KSValue::Ptr& value );
	static KSValue::Ptr convert( KSContext& context, const QRect& rect );

    protected:
	KSValue::Ptr property( KSContext& context, void* object, const QString& name );
	bool setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value );
    };

    class Point : public KSBuiltinStructClass
    {
    public:
        Point( KSModule* module, const QString& name );

        bool constructor( KSContext& c );
        bool destructor( void* object );
        KSStruct* clone( KSBuiltinStruct* );

        static QPoint convert( KSContext& context, const KSValue::Ptr& value );
        static KSValue::Ptr convert( KSContext& context, const QPoint& value );

    protected:
        KSValue::Ptr property( KSContext& context, void* object, const QString& name );
        bool setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value );
    };

    class Size : public KSBuiltinStructClass
    {
    public:
        Size( KSModule* module, const QString& name );

        bool constructor( KSContext& c );
        bool destructor( void* object );
        KSStruct* clone( KSBuiltinStruct* );

        static QSize convert( KSContext& context, const KSValue::Ptr& value );
        static KSValue::Ptr convert( KSContext& context, const QSize& value );

    protected:
        KSValue::Ptr property( KSContext& context, void* object, const QString& name );
        bool setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value );
    };

};

#endif
