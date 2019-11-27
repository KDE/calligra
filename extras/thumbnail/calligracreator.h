/*  This file is part of the KDE libraries
    Copyright (C) 2002 Simon MacMullen <calligra@babysimon.co.uk>
    Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef _CALLIGRACREATOR_H_
#define _CALLIGRACREATOR_H_

// KF5
#include <KIO/ThumbCreator>
// Qt
#include <QEventLoop>

class KoPart;
class KoDocument;


class CalligraCreator : public QObject, public ThumbCreator
{
    Q_OBJECT
public:
    CalligraCreator();
    ~CalligraCreator() override;
    bool create(const QString &path, int width, int height, QImage &image) override;
    Flags flags() const override;

private Q_SLOTS:
    void onLoadingCompleted();

private:
    KoPart *m_part;
    KoDocument *m_doc;

    bool m_loadingCompleted :1;
    QEventLoop m_eventLoop;
};

#endif
