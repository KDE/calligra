/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
#include "PptToOdp.h"
#include <KoOdf.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QBuffer>

int
convert(const QString& in, KoStore* out)
{
    // open inputFile
    POLE::Storage storage(in.toLocal8Bit());
    if (!storage.open()) {
        qDebug() << "Cannot open " << in;
        return KoFilter::StupidError;
    }
    PptToOdp ppttoodp;
    return ppttoodp.convert(storage, out);
}

/* convert all files in a directory */
int
convertAllFilesInDir(const QDir &dir, const QString firstFile = QString())
{
    bool skip = firstFile.size() > 0;
    foreach(const QFileInfo& f, dir.entryInfoList(QDir::Files,
            QDir::Size | QDir::Reversed)) {
        if (skip) {
            skip = !f.absoluteFilePath().endsWith(firstFile);
        }
        if (!skip) {
            QBuffer buffer;
            KoStore* storeout = KoStore::createStore(&buffer, KoStore::Write,
                                KoOdf::mimeType(KoOdf::Presentation), KoStore::Tar);
            qDebug() << "Converting " << f.size() << " " << f.absoluteFilePath();
            convert(f.absoluteFilePath(), storeout);
            delete storeout;
        }
    }
    return 0;
}

int
main(int argc, char** argv)
{
    if (argc != 2 && argc != 3) {
        return 1;
    }
    QCoreApplication app(argc, argv);

    const QString file1 = argv[1];
    QString file2;
    if (argc == 3) {
        file2 = argv[2];
    }

    QFileInfo in(file1);
    if (in.isDir()) {
        QDir d(argv[1]);
        // output
        convertAllFilesInDir(d, file2);
        return 0;
    }

    // open inputFile
    POLE::Storage storage(file1.toLocal8Bit());
    if (!storage.open()) {
        qDebug() << "Cannot open " << file1;
        return KoFilter::StupidError;
    }
    // create output store
    KoStore* storeout;
    QBuffer buffer;
    if (file2.isNull()) {
        storeout = KoStore::createStore(&buffer, KoStore::Write,
                                        KoOdf::mimeType(KoOdf::Presentation), KoStore::Tar);
    } else {
        storeout = KoStore::createStore(file2, KoStore::Write,
                                        KoOdf::mimeType(KoOdf::Presentation), KoStore::Zip);
    }
    if (!storeout) {
        return KoFilter::FileNotFound;
    }

    PptToOdp ppttoodp;
    int r = ppttoodp.convert(storage, storeout);
    delete storeout;

    return r;
}
