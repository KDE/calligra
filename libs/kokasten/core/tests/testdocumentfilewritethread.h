/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TESTDOCUMENTFILEWRITETHREAD_H
#define TESTDOCUMENTFILEWRITETHREAD_H

// Qt
#include <QtCore/QByteArray>
#include <QtCore/QThread>

class QFile;


namespace Kasten2
{

class TestDocument;


class TestDocumentFileWriteThread : public QThread
{
  Q_OBJECT
  public:
    TestDocumentFileWriteThread( QObject* parent, const QByteArray& header,
                                 TestDocument* document, QFile* file );
    virtual ~TestDocumentFileWriteThread();

  public: // QThread API
    virtual void run();

  public:
    bool success() const;

  Q_SIGNALS:
    void documentWritten( bool success );

  protected:
    const QByteArray mHeader;
    const TestDocument* mDocument;
    QFile* mFile;

    bool mSuccess;
};


inline TestDocumentFileWriteThread::TestDocumentFileWriteThread( QObject* parent, const QByteArray& header,
    TestDocument* document, QFile* file )
 : QThread( parent ), mHeader( header ), mDocument( document ), mFile( file ), mSuccess( false )
{}

inline bool TestDocumentFileWriteThread::success() const { return mSuccess; }

}

#endif
