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

#include "rtfexport_factory.h"
#include "rtfexport_factory.moc"
#include "rtfexport.h"

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void* init_librtfexport()
    {
        return new RTFExportFactory;
    }
};

KInstance* RTFExportFactory::s_global = 0;

RTFExportFactory::RTFExportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "rtfexport" );
}

RTFExportFactory::~RTFExportFactory()
{
    delete s_global;
}

QObject* RTFExportFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    kdDebug(30502) << "RTFExportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    RTFExport *f = new RTFExport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* RTFExportFactory::global()
{
    return s_global;
}

