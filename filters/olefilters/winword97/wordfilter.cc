/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <wtrobin@carinthia.com>

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

#include <wordfilter.h>
#include <wordfilter.moc>

WordFilter::WordFilter(const myFile &mainStream, const myFile &table0Stream,
                       const myFile &table1Stream, const myFile &dataStream) :
                       FilterBase() {

    myDoc=0L;
    m_part=QDomDocument("word");
    myDoc=new WinWordDoc(m_part, mainStream, table0Stream, table1Stream, dataStream);
}

WordFilter::~WordFilter() {

    delete myDoc;
    myDoc=0L;
}

const QDomDocument * const WordFilter::part() {
    return myDoc->part();
}

const bool WordFilter::filter() {

    m_success=myDoc->convert();
    m_ready=true;
    return m_success;
}
