/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef __RTFIMPORT_FACTORY_H__
#define __RTFIMPORT_FACTORY_H__

#include <klibloader.h>


class KInstance;

class RTFImportFactory : public KLibFactory
{
    Q_OBJECT

public:
    RTFImportFactory( QObject *parent = 0L, const char *name = 0L );
    ~RTFImportFactory();

    QObject *createObject( QObject *parent = 0L, const char *name = 0L,
			   const char *classname = "QObject",
			   const QStringList &args = QStringList() );

    static KInstance *global();

private:
    static KInstance *s_global;
};

#endif
