/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include <kdebug.h>
#include <kinstance.h>
#include "rtfimport_factory.h"
#include "rtfimport_factory.moc"
#include "rtfimport.h"


extern "C"
{
    void *init_librtfimport()
    {
        return new RTFImportFactory;
    }
};

KInstance *RTFImportFactory::s_global = 0L;


RTFImportFactory::RTFImportFactory( QObject *parent, const char *name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "rtfimport" );
}

RTFImportFactory::~RTFImportFactory()
{
    delete s_global;
    s_global = 0L;
}

QObject *RTFImportFactory::createObject( QObject *parent, const char *name,
					 const char *, const QStringList & )
{
    if (parent && !parent->inherits( "KoFilter" ))
    {
	kdDebug() << "RTFImportFactory: parent does not inherit KoFilter" << endl;
	return 0L;
    }
    return new RTFImport( (KoFilter *)parent, name );
}

KInstance *RTFImportFactory::global()
{
    return s_global;
}
