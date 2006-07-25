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

#include <pptxml.h>
#include <powerpointfilter.h>

PowerPointFilter::PowerPointFilter(
    const myFile &mainStream,
    const myFile &currentUser,
    const myFile &pictures):
        FilterBase()
{
    m_tree = new PptXml(mainStream, currentUser, pictures);

    // Hook up the embedded object support.

    connect(
        m_tree,
        SIGNAL(signalPart(const QString&, QString &, QString &)),
        this,
        SIGNAL(signalPart(const QString&, QString &, QString &)));

    // Hook up the embedded picture support.

    connect(
        m_tree,
        SIGNAL(signalSavePic(const QString &, QString &, const QString &, unsigned int, const char *)),
        this,
        SIGNAL(signalSavePic(const QString &, QString &, const QString &, unsigned int, const char *)));

    // Hook up the embedded part support.

    connect(
        m_tree,
        SIGNAL(signalSavePart(const QString &, QString &, QString &, const QString &, unsigned int, const char *)),
        this,
        SIGNAL(signalSavePart(const QString &, QString &, QString &, const QString &, unsigned int, const char *)));
}

PowerPointFilter::~PowerPointFilter()
{
    delete m_tree;
}

bool PowerPointFilter::filter()
{
    return m_tree->convert();
}

const QDomDocument * const PowerPointFilter::part()
{
    m_part = QDomDocument("DOC");
    m_part.setContent(m_tree->getXml());
    return &m_part;
}

#include <powerpointfilter.moc>
