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

#include "latexexport_factory.h"
#include "latexexport_factory.moc"
#include "latexexport.h"

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void* init_liblatexexport()
    {
        return new LATEXExportFactory;
    }
};

KInstance* LATEXExportFactory::s_global = 0;

LATEXExportFactory::LATEXExportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "latexexport" );
}

LATEXExportFactory::~LATEXExportFactory()
{
    delete s_global;
    s_global=0L;
}

QObject* LATEXExportFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    kdDebug(30503) << "LATEXExportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    LATEXExport *f = new LATEXExport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* LATEXExportFactory::global()
{
    return s_global;
}
