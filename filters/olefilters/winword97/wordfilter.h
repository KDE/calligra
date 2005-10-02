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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef WORDFILTER_H
#define WORDFILTER_H

#include <filterbase.h>
#include <qcstring.h>
class myFile;
class WinWordDoc;

class WordFilter :
    public FilterBase
{
    Q_OBJECT

public:
    WordFilter(const myFile &mainStream, const myFile &table0Stream,
               const myFile &table1Stream, const myFile &dataStream);
    virtual ~WordFilter();

    virtual bool filter();
    virtual bool plainString() const { return true; }
    virtual QCString CString() const;

signals:
    void internalCommDelayStream( const char* delay );
    void internalCommShapeID( unsigned int& shapeID );

private:
    WordFilter(const WordFilter &);
    const WordFilter &operator=(const WordFilter &);
    QCString m_result;

    WinWordDoc *myDoc;
};
#endif // WORDFILTER_H
