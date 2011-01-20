/* This file is part of the KDE project
 * Copyright (C) 2005-2009 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWDOCUMENT_P_H
#define KWDOCUMENT_P_H

#include <QObject>

class KWPage;
class KWDocument;

/// \internal
class PageProcessingQueue : public QObject
{
    Q_OBJECT
public:
    explicit PageProcessingQueue(KWDocument *parent);

    /// add and start delayed process
    void addPage(KWPage page);

private slots:
    /// process (and stops any delayed process)
    void process();

private:
    QList<int> m_pages;
    bool m_triggered;
    KWDocument *m_document;
};

#endif
