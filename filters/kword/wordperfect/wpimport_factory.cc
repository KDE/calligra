/* This file is part of the KDE project
   Copyright (C) 2001 Ariya Hidayat <ariyahidayat@yahoo.de>

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

#include <kinstance.h>
#include <kdebug.h>

#include "wpimport.h"
#include "wpimport_factory.h"

#include "wpimport_factory.moc"

extern "C"
{
    void* init_libwpimport()
    {
        return new WPImportFactory;
    }
};

KInstance* WPImportFactory::s_global = 0;

WPImportFactory::WPImportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "asciiimport" );
}

WPImportFactory::~WPImportFactory()
{
    delete s_global;
    s_global = 0L;
}

QObject* WPImportFactory::createObject( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    kdDebug(30502) << "WPImportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    WPImport *f = new WPImport( (KoFilter*)parent, name );
    return f;
}

KInstance* WPImportFactory::global()
{
    return s_global;
}
