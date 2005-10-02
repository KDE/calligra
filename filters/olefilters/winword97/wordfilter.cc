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

#include <winworddoc.h>
#include <wordfilter.h>

WordFilter::WordFilter(const myFile &mainStream, const myFile &table0Stream,
                       const myFile &table1Stream, const myFile &dataStream) :
                       FilterBase()
{
    myDoc = new WinWordDoc(m_result, mainStream, table0Stream, table1Stream, dataStream);

    // Hook up the embedded object support.

    connect(
        myDoc,
        SIGNAL(signalPart(const QString&, QString &, QString &)),
        this,
        SIGNAL(signalPart(const QString&, QString &, QString &)));

    // Hook up the document info support.

    connect(
        myDoc,
        SIGNAL(signalSaveDocumentInformation(const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &)),
        this,
        SIGNAL(signalSaveDocumentInformation(const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &)));

    // Hook up the embedded picture support.

    connect(
        myDoc,
        SIGNAL(signalSavePic(const QString &, QString &, const QString &, unsigned int, const char *)),
        this,
        SIGNAL(signalSavePic(const QString &, QString &, const QString &, unsigned int, const char *)));

    // Hook up the embedded part support.

    connect(
        myDoc,
        SIGNAL(signalSavePart(const QString &, QString &, QString &, const QString &, unsigned int, const char *)),
        this,
        SIGNAL(signalSavePart(const QString &, QString &, QString &, const QString &, unsigned int, const char *)));

    // forward the internal communication calls
    connect( this, SIGNAL( internalCommShapeID( unsigned int& ) ), myDoc, SLOT( internalCommShapeID( unsigned int& ) ) );
    connect( this, SIGNAL( internalCommDelayStream( const char* ) ), myDoc, SLOT( internalCommDelayStream( const char* ) ) );
}

WordFilter::~WordFilter()
{
    delete myDoc;
    myDoc = 0L;
}

QCString WordFilter::CString() const
{
    return m_result;
}

bool WordFilter::filter()
{
    m_success = myDoc->convert();
    m_ready = true;
    return true;
}

#include <wordfilter.moc>
