/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#include "FunctionModuleFactory.h"

using namespace KSpread;

class FunctionModuleFactory::Private
{
public:
    QString id;
    QString name;
};


FunctionModuleFactory::FunctionModuleFactory(QObject* parent, const QString &id, const QString& name)
    : QObject(parent)
    , d(new Private)
{
    d->id = id;
    d->name = name;
}

FunctionModuleFactory::~FunctionModuleFactory()
{
    delete d;
}

QString FunctionModuleFactory::id() const
{
    return d->id;
}

QString FunctionModuleFactory::name() const
{
    return d->name;
}

#include "FunctionModuleFactory.moc"
