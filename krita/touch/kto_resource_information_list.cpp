/*
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

#include "kto_resource_information_list.h"
#include <KoResourceServerAdapter.h>
#include <kto_resource_information.h>

KtoResourceInformationList::KtoResourceInformationList(KoAbstractResourceServerAdapter* resourceAdapter, QObject* parent) : QObject(parent)
{
    m_resources = resourceAdapter->resources();
    for(int i = 0; i < m_resources.size(); ++i)
    {
        m_resourcesInformation.append(new KtoResourceInformation(i, this));
    }
}

KtoResourceInformationList::~KtoResourceInformationList()
{

}

QList<KtoResourceInformation*> KtoResourceInformationList::resourcesInformation() const
{
    return m_resourcesInformation;
}

QList<QObject*> KtoResourceInformationList::resourcesInformationAsQObject() const
{
    QList<QObject*> list;
    foreach(KtoResourceInformation* info, m_resourcesInformation) list.append(info);
    return list;
}


QList<KoResource*> KtoResourceInformationList::resources() const
{
    return m_resources;
}

#include "kto_resource_information_list.moc"
