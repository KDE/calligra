// $Header$

/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

/*
   This file is based on the old file:
    /home/kde/koffice/filters/kspread/csv/csvfilterdia_factory.cc

   The old file was copyrighted by
    Copyright (C) 1999 David Faure <faure@kde.org>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#ifndef USE_QFD

#include "htmlexportdia_factory.h"
#include "htmlexportdia_factory.moc"
#include "htmlexportdia.h"

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void* init_libhtmlexportdia()
    {
        return new HTMLExportDiaFactory;
    }
};

KInstance* HTMLExportDiaFactory::s_global = 0;

HTMLExportDiaFactory::HTMLExportDiaFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "htmlexportdia" );
}

HTMLExportDiaFactory::~HTMLExportDiaFactory()
{
    delete s_global;
    s_global = 0L;
}

QObject* HTMLExportDiaFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilterDialog") )
    {
	    kdDebug(30501) << "HTMLExportDiaFactory: parent does not inherit KoFilterDialog" << endl;
	    return 0L;
    }
    HTMLExportDia *f = new HTMLExportDia( (KoFilterDialog*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* HTMLExportDiaFactory::global()
{
    return s_global;
}
#endif
