/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "MctPropertyBase.h"
#include <QDebug>

MctPropertyBase::MctPropertyBase(QString name)
    : propName(name)
    , props(new PropertyDictionary())
    , props2export(new PropertyDictionary())
{
    // FIXME: if its implemented... ?
    //fillUpProperties();
}

MctPropertyBase::~MctPropertyBase()
{

}

QString MctPropertyBase::getPropertyName() const
{
    return this->propName;
}

void MctPropertyBase::setPropertyName(QString name)
{
    this->propName = name;
}

PropertyDictionary * MctPropertyBase::getProps() const
{
    return this->props;
}

void MctPropertyBase::setProps(PropertyDictionary *props)
{
    this->props = props;
}

PropertyDictionary * MctPropertyBase::getProps2Export() const
{

    return this->props2export;
}

void MctPropertyBase::setProps2Export(PropertyDictionary *props)
{
    props2export = props;
}

QString MctPropertyBase::getInnerUrl() const
{
    return innerURL;
}

void MctPropertyBase::setInnerUrl(QString url)
{
    innerURL = url;
}

void MctPropertyBase::fillUpProperties()
{
    //TODO
    qCritical() << "TODO";
}
