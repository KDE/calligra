/* This file is part of the KDE project

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

#ifndef EXCELFILTER_H
#define EXCELFILTER_H

#include <filterbase.h>
#include <qcstring.h>
#include <qdatastream.h>
#include <xmltree.h>

const int MAX_RECORD_SIZE = 0x2024;

class ExcelFilter:public FilterBase
{
public:
    ExcelFilter(const QByteArray &mainStream);
    ~ExcelFilter();

    virtual bool filter();
    virtual const QDomDocument* const part();

private:
    QDataStream *s;
    XMLTree *tree;
    double length;
};

#endif
