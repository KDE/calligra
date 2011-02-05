/*
 * This file is part of Calligra
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Thorsten Zachmann thorsten.zachmann@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#include <KoDocument.h>
#include <KoPADocument.h>
#include <KWDocument.h>
#include <tables/part/Doc.h>

#include <KMimeType>
#include <kmimetypetrader.h>
#include <kparts/componentfactory.h>
#include <kcmdlineargs.h>
#include <kdebug.h>

#include <QApplication>
#include <QBuffer>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QTimer>

#include "CSThumbProviderStage.h"
#include "CSThumbProviderTables.h"
#include "CSThumbProviderWords.h"

KoDocument* openFile(const QString &filename)
{
    const QString mimetype = KMimeType::findByPath(filename)->name();

    QString error;
    KoDocument *document = KMimeTypeTrader::self()->createPartInstanceFromQuery<KoDocument>(
                               mimetype, 0, 0, QString(),
                               QVariantList(), &error );

    if (!error.isEmpty()) {
        qWarning() << "Error cerating document" << mimetype << error;
        return 0;
    }

    if (0 != document) {
        KUrl url;
        url.setPath(filename);

        document->setCheckAutoSaveFile(false);
        document->setAutoErrorHandlingEnabled(false);

        if (document->openUrl(filename)) {
            document->setReadWrite(false);
        }
        else {
            kWarning(31000)<< "openUrl failed" << filename << mimetype << error;
            delete document;
            document = 0;
        }
    }
    return document;
}

QList<QPixmap> createThumbnails(KoDocument *document, const QSize &thumbSize)
{
    QList<QPixmap> thumbnails;

    CSThumbProvider *tp = 0;

    if (KoPADocument *doc = qobject_cast<KoPADocument*>(document)) {
        tp = new CSThumbProviderStage(doc);
    }
    else if (Calligra::Tables::Doc *doc = qobject_cast<Calligra::Tables::Doc*>(document)) {
        tp = new CSThumbProviderTables(doc);
    }
    else if (KWDocument *doc = qobject_cast<KWDocument*>(document)) {
        tp = new CSThumbProviderWords(doc);
    }

    return tp->createThumbnails(thumbSize);
}

void saveThumbnails(const QList<QPixmap> &thumbnails, const QString &dir)
{
    int i = 0;
    for (QList<QPixmap>::const_iterator it(thumbnails.constBegin()); it != thumbnails.constEnd(); ++it) {
        // it is not possible to use QString("%1/thumb_%2.png").arg(dir).arg(++i);
        // as dir can contain % values which then might or might not be overwritten by the second arg
        QString thumbFilename = dir + QString("/thumb_%2.png").arg(++i);
        it->save(thumbFilename, "PNG");
    }
}

bool checkThumbnails(const QList<QPixmap> &thumbnails, const QString &dir, bool verbose)
{
    bool success = true;
    int i = 0;
    for (QList<QPixmap>::const_iterator it(thumbnails.constBegin()); it != thumbnails.constEnd(); ++it) {
        QString thumbFilename = QString("%1/thumb_%2.png").arg(dir).arg(++i);

        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        it->save(&buffer, "PNG");

        QFile file(thumbFilename);
        if (!file.open(QFile::ReadOnly)) {
            return false;
        }
        QByteArray baCheck(file.readAll());

        if (ba != baCheck) {
            qDebug() << "Check failed:" << dir << "Page" << i << "differ";
            success = false;
        }
        else if (verbose) {
            qDebug() << "Check successful:" << dir << "Page" << i << "identical";
        }
    }
    return success;
}

int main(int argc, char *argv[])
{
    KCmdLineArgs::init(argc, argv, "cstester", 0, KLocalizedString(), 0, KLocalizedString());

    KCmdLineOptions options;
    options.add("create", ki18n("create verification data for file"));
    options.add("verbose", ki18n("be verbose"));
    options.add("!verify", ki18n("verify the file"));
    options.add( "+file", ki18n("file to use"));
    KCmdLineArgs::addCmdLineOptions(options);

    QApplication app(argc, argv);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("create") && args->isSet("verify")) {
        kError() << "create and verify cannot be used the same time";
        exit(1);
    }

    if (!args->isSet("create") && !args->isSet("verify")) {
        kError() << "one of the options create or verify needs to be specified";
        exit(1);
    }

    bool verbose = args->isSet("verbose");

    int exitValue = 0;

    int successful = 0;
    int failed = 0;
    for (int i=0; i < args->count(); ++i) {
        QString filename(args->arg(i));
        QFileInfo file(filename);
        QString resDir(filename + ".check");
        qDebug() << "filename" << filename << "path" << file.path() << file.completeBaseName() << resDir << file.absoluteFilePath();

        // filename must be a absolute path
        KoDocument* document = openFile(file.absoluteFilePath());
        if (!document) {
            exit(2);
        }

        QList<QPixmap> thumbnails(createThumbnails(document, QSize(800,800)));

        qDebug() << "created" << thumbnails.size() << "thumbnails";
        if (args->isSet("verify")) {
            if (checkThumbnails(thumbnails, resDir, verbose)) {
                ++successful;
            }
            else {
                ++failed;
                exitValue = 2;
            }
        }
        else {
            QDir dir(file.path());
            dir.mkdir(file.fileName() + ".check");
            saveThumbnails(thumbnails, resDir);
        }
        delete document;
    }

    if (args->isSet("verify")) {
        qDebug() << "Totals:" << successful << "passed" << failed << "failed";
    }

    QTimer::singleShot(1, &app, SLOT(quit()));
    app.exec();
    return exitValue;
}
