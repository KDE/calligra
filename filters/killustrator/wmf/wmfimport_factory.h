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

#ifndef WMFIMPORT_FACTORY_H
#define WMFIMPORT_FACTORY_H

#include <klibloader.h>

class KInstance;

class WMFImportFactory :
    public KLibFactory
{
    Q_OBJECT
public:
    WMFImportFactory(QObject* parent = 0, const char* name = 0);
    virtual ~WMFImportFactory();

    virtual QObject* create(QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList());

    static KInstance* global();

private:
    static KInstance* s_global;
};
#endif
