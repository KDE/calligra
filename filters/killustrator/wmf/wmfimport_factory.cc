/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

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

DESCRIPTION
*/

#include "wmfimport_factory.h"
#include "wmfimport_factory.moc"
#include "wmfimport.h"

#include <kinstance.h>
#include <kdebug.h>

extern "C"
{
    void *init_libwmfimport()
    {
        return new WMFImportFactory;
    }
};

KInstance *WMFImportFactory::s_global = 0;

WMFImportFactory::WMFImportFactory(
    QObject *parent,
    const char *name) :
        KLibFactory(parent, name)
{
    s_global = new KInstance("wmfimport");
}

WMFImportFactory::~WMFImportFactory()
{
    delete s_global;
}

QObject *WMFImportFactory::create(
    QObject *parent,
    const char *name,
    const char*,
    const QStringList &)
{
    if (parent && !parent->inherits("KoFilter"))
    {
	    kdDebug(30502) << "WMFImportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    WMFImport *f = new WMFImport((KoFilter*)parent, name);
    emit objectCreated(f);
    return f;
}

KInstance *WMFImportFactory::global()
{
    return s_global;
}
