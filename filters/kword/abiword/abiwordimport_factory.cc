/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>
  
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*
   This file is based on the old file:
    koffice/filters/kword/ascii/asciiimport.cc

   The old file was copyrighted by 
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#include "abiwordimport_factory.h"
#include "abiwordimport_factory.moc"
#include "abiwordimport.h"

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void* init_libabiwordimport()
    {
        return new ABIWORDImportFactory;
    }
};

KInstance* ABIWORDImportFactory::s_global = 0;

ABIWORDImportFactory::ABIWORDImportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "abiwordimport" );
}

ABIWORDImportFactory::~ABIWORDImportFactory()
{
    delete s_global;
    s_global = 0L;
}

QObject* ABIWORDImportFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    kdDebug() << "ABIWORDImportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    ABIWORDImport *f = new ABIWORDImport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* ABIWORDImportFactory::global()
{
    return s_global;
}
