/* This file is part of the KDE project
   Copyright (C) 2001 George Staikos <staikos@kde.org>

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

#include "kocryptexport_factory.h"
#include "kocryptexport.h"

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void* init_libkocryptexport()
    {
        return new KoCryptExportFactory;
    }
};

KInstance* KoCryptExportFactory::s_global = 0;

KoCryptExportFactory::KoCryptExportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "kocryptexport" );
}

KoCryptExportFactory::~KoCryptExportFactory()
{
    delete s_global;
    s_global = 0L;
}

QObject* KoCryptExportFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    kdDebug(30502) << "KoCryptExportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    KoCryptExport *f = new KoCryptExport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* KoCryptExportFactory::global()
{
    return s_global;
}

#include "kocryptexport_factory.moc"

