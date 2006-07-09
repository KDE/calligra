/*
    Copyright (C) 2001, Rob Buis <rwlbuis@wanadoo.nl>
    Copyright (C) 2003, Rob Buis <buis@kde.org>
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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

DESCRIPTION
*/

#include "xfigimport_factory.h"
#include "xfigimport_factory.moc"
#include "xfigimport.h"

#include <kinstance.h>
#include <kdebug.h>

K_EXPORT_COMPONENT_FACTORY( libxfigimport, XFIGImportFactory )

KInstance *XFIGImportFactory::s_global = 0;

XFIGImportFactory::XFIGImportFactory(
    QObject *parent,
    const char *name) :
        KLibFactory(parent, name)
{
    s_global = new KInstance("xfigimport");
}

XFIGImportFactory::~XFIGImportFactory()
{
    delete s_global;
    s_global = 0L;
}

QObject *XFIGImportFactory::createObject(
    QObject *parent,
    const char *name,
    const char*,
    const QStringList &)
{
    if (parent && !parent->inherits("KoFilter"))
    {
	    kdDebug(30502) << "XFIGImportFactory: parent does not inherit KoFilter" << endl;
	    return 0L;
    }
    XFIGImport *f = new XFIGImport((KoFilter*)parent, name);
    return f;
}

KInstance *XFIGImportFactory::global()
{
    return s_global;
}
