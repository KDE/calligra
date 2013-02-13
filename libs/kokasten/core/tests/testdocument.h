/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007,2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef TESTDOCUMENT_H
#define TESTDOCUMENT_H

// lib
#include "abstractdocument.h"
// Qt
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace Kasten2
{

class TestDocument : public AbstractDocument
{
    Q_OBJECT

  public:
    TestDocument();
    explicit TestDocument( const QByteArray& data );
    virtual ~TestDocument();

  public: // API to be implemented
    virtual QString title() const;
    virtual QString typeName() const;
    virtual QString mimeType() const;
    virtual ContentFlags contentFlags() const;

  public:
    const QByteArray* data() const;
    void setData( const QByteArray& data );

  public: // instruction functions
    void setTitle( const QString& title );
    void setContentFlags( ContentFlags contentFlags );

  protected:
    QString mTitle;
    QByteArray mData;
    ContentFlags mContentFlags;
};

}

#endif
