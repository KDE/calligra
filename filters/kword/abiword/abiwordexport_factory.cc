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
    koffice/filters/kword/ascii/asciiexport_factory.cc

   The old file was copyrighted by 
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#include "abiwordexport_factory.h"
#include "abiwordexport_factory.moc"
#include "abiwordexport.h"

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void* init_libabiwordexport()
    {
        return new ABIWORDExportFactory;
    }
};

KInstance* ABIWORDExportFactory::s_global = 0;

ABIWORDExportFactory::ABIWORDExportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "abiwordexport" );
}

ABIWORDExportFactory::~ABIWORDExportFactory()
{
    delete s_global;
    s_global = 0L;
}

QObject* ABIWORDExportFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    kdDebug() << "ABIWORDExportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    ABIWORDExport *f = new ABIWORDExport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* ABIWORDExportFactory::global()
{
    return s_global;
}
