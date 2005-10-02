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

#ifndef XFIGIMPORT_FACTORY_H
#define XFIGIMPORT_FACTORY_H

#include <klibloader.h>

class KInstance;

class XFIGImportFactory :
    public KLibFactory
{
    Q_OBJECT
public:
    XFIGImportFactory(QObject* parent = 0, const char* name = 0);
    virtual ~XFIGImportFactory();

    virtual QObject* createObject(QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList());

    static KInstance* global();

private:
    static KInstance* s_global;
};
#endif
