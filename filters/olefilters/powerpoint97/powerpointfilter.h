/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <trobin@kde.org>

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
*/

#ifndef POWERPOINTFILTER_H
#define POWERPOINTFILTER_H

#include <filterbase.h>
#include <pptxml.h>

class PowerPointFilter:
    public FilterBase
{
    Q_OBJECT

public:
    PowerPointFilter(
        const myFile &mainStream,
        const myFile &currentUser,
        const myFile &pictures);
    virtual ~PowerPointFilter();
    virtual bool filter();
    virtual const QDomDocument * const part();

private:
    PowerPointFilter(const PowerPointFilter &);
    const PowerPointFilter &operator=(const PowerPointFilter &);

    PptXml *m_tree;
};
#endif

