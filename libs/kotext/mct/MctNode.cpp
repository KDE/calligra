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

#include "MctNode.h"
#include "MctStaticData.h"

MctNode::MctNode()
{
    m_id = MctStaticData::UNDEFINED; //ulong max
}

MctNode::MctNode(unsigned long id)
{
    m_id = id;
}

MctNode::~MctNode()
{

}

void MctNode::setId(unsigned long id)
{
    m_id = id;
}

unsigned long MctNode::id() const
{
    return m_id;
}

void MctNode::setName(const QString &name)
{
    m_name = name;
}

QString MctNode::name()
{
    return m_name;
}
