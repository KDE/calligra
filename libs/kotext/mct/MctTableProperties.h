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

#ifndef MCTTABLEPROPERTIES_H
#define MCTTABLEPROPERTIES_H

#include "MctPropertyBase.h"

// Text table properties class
class MctTableProperties : public MctPropertyBase
{
public:
    MctTableProperties(QString name, int rows = -1, int cols = -1);
    ~MctTableProperties();
    
    int rows() const;
    void setRows(int value);
    
    int cols() const;
    void setCols(int value);
    
private:
    int m_rows;
    int m_cols;
    
};

#endif // MCTTABLEPROPERTIES_H
