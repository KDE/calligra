/* This file is part of the KDE project
   Copyright (C) 2000 Thomas Zander zander@earthling.net

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

#include "asciiimport_factory.h"
#include "asciiimport_factory.moc"
#include "asciiimport.h"

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void* init_libasciiimport()
    {
        return new ASCIIImportFactory;
    }
};

KInstance* ASCIIImportFactory::s_global = 0;

ASCIIImportFactory::ASCIIImportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "asciiimport" );
}

ASCIIImportFactory::~ASCIIImportFactory()
{
    delete s_global;
}

QObject* ASCIIImportFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    kdDebug(30502) << "ASCIIImportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    ASCIIImport *f = new ASCIIImport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* ASCIIImportFactory::global()
{
    return s_global;
}
