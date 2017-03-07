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

#include "MctTableProperties.h"

MctTableProperties::MctTableProperties(const QString &name, int rows, int cols)
    : MctPropertyBase(name)
    , m_rows(rows)
    , m_cols(cols)
{

}

MctTableProperties::~MctTableProperties()
{

}

int MctTableProperties::rows() const
{
    return m_rows;
}

void MctTableProperties::setRows(int value)
{
    m_rows = value;
}

int MctTableProperties::cols() const
{
    return m_cols;
}

void MctTableProperties::setCols(int value)
{
    m_cols = value;
}



