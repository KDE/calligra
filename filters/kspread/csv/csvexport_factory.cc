/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

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

#include <csvexport_factory.h>
#include <csvexport.h>

#include <kinstance.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

extern "C"
{
    void* init_libcsvexport()
    {
	KGlobal::locale()->insertCatalogue("csvfilter");
        return new CSVExportFactory;
    }
};

KInstance* CSVExportFactory::s_global = 0;

CSVExportFactory::CSVExportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "csvexport_test" );
}

CSVExportFactory::~CSVExportFactory()
{
    delete s_global;
    s_global = 0L;
}

QObject* CSVExportFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    kdDebug(31000) << "CSVExportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    CSVExport *f = new CSVExport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* CSVExportFactory::global()
{
    return s_global;
}

#include <csvexport_factory.moc>
