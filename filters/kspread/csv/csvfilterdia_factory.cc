/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

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

#ifndef USE_QFD

#include "csvfilterdia_factory.h"
#include "csvfilterdia_factory.moc"
#include "csvfilterdia.h"

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void* init_libcsvfilterdia()
    {
        return new CSVFilterDiaFactory;
    }
};

KInstance* CSVFilterDiaFactory::s_global = 0;

CSVFilterDiaFactory::CSVFilterDiaFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "csvfilterdia" );
}

CSVFilterDiaFactory::~CSVFilterDiaFactory()
{
    delete s_global;
    s_global = 0L;
}

QObject* CSVFilterDiaFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilterDialog") )
    {
	    kdDebug(30501) << "CSVFilterDiaFactory: parent does not inherit KoFilterDialog" << endl;
	    return 0L;
    }
    CSVFilterDia *f = new CSVFilterDia( (KoFilterDialog*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* CSVFilterDiaFactory::global()
{
    return s_global;
}
#endif
