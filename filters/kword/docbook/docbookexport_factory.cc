/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "docbookexport_factory.h"
#include "docbookexport_factory.moc"
#include "docbookexport.h"

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void* init_libdocbookexport()
    {
        return new DocBookExportFactory;
    }
};

KInstance* DocBookExportFactory::s_global = 0;

DocBookExportFactory::DocBookExportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "docbookexport" );
}

DocBookExportFactory::~DocBookExportFactory()
{
    delete s_global;
}

QObject* DocBookExportFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    kdDebug(30502) << "DocBookExportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    DocBookExport *f = new DocBookExport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* DocBookExportFactory::global()
{
    return s_global;
}
