/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "PptToOdp.h"
#include <KoOdf.h>
#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>

int convert(const QString &in, KoStore *out)
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
int convertAllFilesInDir(const QDir &dir, const QString firstFile = QString())
{
    bool skip = firstFile.size() > 0;
    foreach (const QFileInfo &f, dir.entryInfoList(QDir::Files, QDir::Size | QDir::Reversed)) {
        if (skip) {
            skip = !f.absoluteFilePath().endsWith(firstFile);
        }
        if (!skip) {
            QBuffer buffer;
            KoStore *storeout = KoStore::createStore(&buffer, KoStore::Write, KoOdf::mimeType(KoOdf::Presentation), KoStore::Tar);
            qDebug() << "Converting " << f.size() << " " << f.absoluteFilePath();
            convert(f.absoluteFilePath(), storeout);
            delete storeout;
        }
    }
    return 0;
}

int main(int argc, char **argv)
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
    KoStore *storeout;
    QBuffer buffer;
    if (file2.isNull()) {
        storeout = KoStore::createStore(&buffer, KoStore::Write, KoOdf::mimeType(KoOdf::Presentation), KoStore::Tar);
    } else {
        storeout = KoStore::createStore(file2, KoStore::Write, KoOdf::mimeType(KoOdf::Presentation), KoStore::Zip);
    }
    if (!storeout) {
        return KoFilter::FileNotFound;
    }

    PptToOdp ppttoodp;
    int r = ppttoodp.convert(storage, storeout);
    delete storeout;

    return r;
}
