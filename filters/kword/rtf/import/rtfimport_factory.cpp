/* This file is part of the KDE project
   Copyright (C) Michael Johnson <mikej@xnet.com>

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

#include <rtfimport_factory.h>
#include <rtfimport.h>

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void* init_librtfimport()
    {
        return new RTFImportFactory;
    }
};

KInstance* RTFImportFactory::s_global = 0;

RTFImportFactory::RTFImportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "rtfimport" );
}

RTFImportFactory::~RTFImportFactory()
{
    delete s_global;
}

QObject* RTFImportFactory::createObject( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    kdDebug(30502) << "RTFImportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    RTFImport *f = new RTFImport( (KoFilter*)parent, name );
    return f;
}

KInstance* RTFImportFactory::global()
{
    return s_global;
}

#include <rtfimport_factory.moc>
