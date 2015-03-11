/*
 *  Copyright (c) 2001 Graham Short.  <grahshrt@netscape.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef QPRO_TABLENAMES_H
#define QPRO_TABLENAMES_H

// -----------------------------------------------------------------------

class QpTableNames
{
public:
    enum  {cNameCnt = 256};

    QpTableNames();
    ~QpTableNames();

    void        name(unsigned pIdx, const char* pName);
    const char* name(unsigned pIdx);

    int         allocated(unsigned pIdx);
protected:
    char* cName[cNameCnt];
};

#endif // QPRO_TABLENAMES_H

