/* This file is part of the KDE project
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

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

#include "KoFilterChain.h"
#include "KoFilterManager.h"

#include <MainDebug.h>

int main(int /*argc*/, char ** /*argv*/)
{
    CalligraFilter::Graph g("application/x-kspread");
    g.dump();
    g.setSourceMimeType("application/vnd.oasis.opendocument.text");
    g.dump();

    KoFilterManager *manager = new KoFilterManager(0);
    qDebug() << "Trying to build some filter chains...";
    QByteArray mimeType("foo/bar");
    KoFilterChain::Ptr chain = g.chain(manager, mimeType);
    if (!chain)
        qDebug() << "Chain for 'foo/bar' is not available, OK";
    else {
        qCritical() << "Chain for 'foo/bar' is available!" << endl;
        chain->dump();
    }

    mimeType = "text/csv";
    chain = g.chain(manager, mimeType);
    if (!chain)
        qCritical() << "Chain for 'text/csv' is not available!" << endl;
    else {
        qDebug() << "Chain for 'text/csv' is available, OK";
        chain->dump();
    }

    // Try to find the closest Calligra part
    mimeType = "";
    chain = g.chain(manager, mimeType);
    if (!chain)
        qDebug() << "It was already a Calligra part, OK";
    else
        qCritical() << "We really got a chain? ugh :}" << endl;

    g.setSourceMimeType("text/csv");
    mimeType = "";
    chain = g.chain(manager, mimeType);
    if (!chain)
        qCritical() << "Hmm... why didn't we find a chain?" << endl;
    else {
        qDebug() << "Chain for 'text/csv' -> closest part is available ("
        << mimeType << "), OK" << endl;
        chain->dump();
    }

    qDebug() << "Checking mimeFilter() for Import:";
    QStringList list = KoFilterManager::mimeFilter("application/vnd.oasis.opendocument.text",  KoFilterManager::Import);
    Q_FOREACH(const QString& it, list)
        qDebug() << "" << it;
    qDebug() << "" << list.count() << " entries.";

    qDebug() << "Checking mimeFilter() for Export:";
    list = KoFilterManager::mimeFilter("application/vnd.oasis.opendocument.text",  KoFilterManager::Export);
    Q_FOREACH(const QString& it, list)
        qDebug() << "" << it;
    qDebug() << "" << list.count() << " entries.";

    qDebug() << "Checking KoShell's mimeFilter():";
    list = KoFilterManager::mimeFilter();
    Q_FOREACH(const QString& it, list)
        qDebug() << "" << it;
    qDebug() << "" << list.count() << " entries.";

    delete manager;
    return 0;
}
