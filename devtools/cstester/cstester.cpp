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
#include <sheets/part/Doc.h>
#include <KoPart.h>
#include <KoPluginLoader.h>
#include <KoDocumentEntry.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QBuffer>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QTimer>
#include <QMimeDatabase>
#include <QDebug>

#include "CSThumbProviderStage.h"
#include "CSThumbProviderTables.h"
#include "CSThumbProviderWords.h"

#include "config_cstester.h"

#ifdef BUILD_KARBON
#include <KarbonPart.h>
#include <KarbonDocument.h>
#include "CSThumbProviderKarbon.h"
#endif

KoDocument* openFile(const QString &filename)
{
    const QString mimetype = QMimeDatabase().mimeTypeForFile(filename).name();

    KoPart *part;
    QString error;
    QList<QPluginLoader *> pluginLoaders = KoPluginLoader::pluginLoaders(QStringLiteral("calligra/parts"), mimetype);
    if (!pluginLoaders.isEmpty()) {
        // take first
        KoDocumentEntry entry(pluginLoaders.takeFirst());
        qDeleteAll(pluginLoaders);
        part = entry.createKoPart(&error);
    } else {
        error = "Could not find component";
    }

    if (!error.isEmpty()) {
        qWarning() << "Error creating document" << mimetype << error;
        return 0;
    }

    KoDocument *document = part->document();

    if (0 != document) {
        QUrl url = QUrl::fromLocalFile(filename);

        document->setCheckAutoSaveFile(false);
        document->setAutoErrorHandlingEnabled(false);

        if (document->openUrl(url)) {
            document->setReadWrite(false);
        }
        else {
            qWarning()<< "openUrl failed" << filename << mimetype << error;
            delete document;
            document = 0;
        }
    }
    return document;
}

QString saveFile(KoDocument *document, const QString &filename, const QString &outname)
{
    QString saveAs = outname;
    // use the name and add -roundtrip
    if (outname.isEmpty()) {
        saveAs = filename;
        int dotPos = saveAs.lastIndexOf('.');
        if (dotPos != -1) {
            saveAs.truncate(dotPos);
        }
        saveAs += "-roundtrip";
    }

    QByteArray mimetype = document->nativeFormatMimeType();
    QMimeType mime = QMimeDatabase().mimeTypeForName(mimetype);
    Q_ASSERT(mime.isValid());
    QString extension = mime.preferredSuffix();
    saveAs += extension;

    QUrl url = QUrl::fromLocalFile(saveAs);
    document->setOutputMimeType(mimetype, 0);
    document->saveAs(url);
    qDebug() << "save done";
    return saveAs;
}

QVector<QImage> createThumbnails(KoDocument *document, const QSize &thumbSize)
{
    CSThumbProvider *tp = 0;

    if (KoPADocument *doc = qobject_cast<KoPADocument*>(document)) {
        tp = new CSThumbProviderStage(doc);
    }
    else if (Calligra::Sheets::Doc *doc = qobject_cast<Calligra::Sheets::Doc*>(document)) {
        tp = new CSThumbProviderTables(doc);
    }
    else if (KWDocument *doc = qobject_cast<KWDocument*>(document)) {
        tp = new CSThumbProviderWords(doc);
    }
#ifdef BUILD_KARBON
    else if (KarbonDocument *doc = qobject_cast<KarbonDocument *>(document)) {
        tp = new CSThumbProviderKarbon(doc);
    }
#endif

    return tp ? tp->createThumbnails(thumbSize) : QVector<QImage>();
}

void saveThumbnails(const QVector<QImage> &thumbnails, const QString &dir)
{
    int i = 0;
    for (QVector<QImage>::const_iterator it(thumbnails.constBegin()); it != thumbnails.constEnd(); ++it) {
        // it is not possible to use QString("%1/thumb_%2.png").arg(dir).arg(++i);
        // as dir can contain % values which then might or might not be overwritten by the second arg
        QString thumbFilename = dir + QString("/thumb_%2.png").arg(++i);
        it->save(thumbFilename, "PNG");
    }
}

void saveThumbnails(const QFileInfo &file, const QVector<QImage> &thumbnails, const QString &outdir)
{
    QDir dir(outdir);
    QString checkSubDir(file.fileName() + ".check");
    dir.mkdir(checkSubDir);
    saveThumbnails(thumbnails, outdir + '/' + checkSubDir);
}

bool checkThumbnails(const QVector<QImage> &thumbnails, const QString &dir, bool verbose)
{
    bool success = true;
    int i = 0;
    for (QVector<QImage>::const_iterator it(thumbnails.constBegin()); it != thumbnails.constEnd(); ++it) {
        QString thumbFilename = dir + QString("/thumb_%2.png").arg(++i);

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

bool checkThumbnails(const QVector<QImage> &thumbnails, const QVector<QImage> &others, bool verbose)
{
    bool success = true;
    if (thumbnails.size() != others.size()) {
        qDebug() << "Check failed: number of pages different" << thumbnails.size() << "!=" << others.size();
        return false;
    }
    int i = 1;
    QVector<QImage>::const_iterator it(thumbnails.constBegin());
    QVector<QImage>::const_iterator oIt(others.constBegin());

    for (; it != thumbnails.constEnd(); ++it, ++oIt, ++i) {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        it->save(&buffer, "PNG");

        QByteArray baCheck;
        QBuffer oBuffer(&baCheck);
        oBuffer.open(QIODevice::WriteOnly);
        oIt->save(&oBuffer, "PNG");

        if (ba != baCheck) {
            qDebug() << "Check failed:" << "Page" << i << "differ";
            success = false;
        }
        else if (verbose) {
            qDebug() << "Check successful:" << "Page" << i << "identical";
        }
    }
    return success;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("cstester");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("create"), i18n("create verification data for file")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("indir"), i18n("directory to read the data from"), QStringLiteral("dir")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("outdir"), i18n("directory to save the data to"), QStringLiteral("dir")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("roundtrip"), i18n("load/save/load and check the document is the same after load and save/load")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("verbose"), i18n("be verbose")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("verify"), i18n("verify the file")));

    parser.addPositionalArgument(QStringLiteral("file"), i18n("file to use"));

    parser.process(app);

    bool create = false;
    bool roundtrip = false;
    bool verify = false;
    int optionCount = 0;

    if (parser.isSet("create")) {
        create = true;
        optionCount++;
    }
    if (parser.isSet("roundtrip")) {
        roundtrip = true;
        optionCount++;
    }
    if (parser.isSet("verify")) {
        verify = true;
        optionCount++;
    }

    if (optionCount > 1) {
        qCritical() << "create, roundtrip and verify cannot be used the same time";
        exit(1);
    }
    else if (optionCount < 1) {
        qCritical() << "one of the options create, roundtrip or verify needs to be specified";
        exit(1);
    }

    QString outDir;
    if (parser.isSet("outdir")) {
        // check if it is a directory
        QDir dir(parser.value("outdir"));
        if (!dir.exists()) {
            qCritical() << "outdir" << parser.value("outdir") << "does not exist";
            exit(1);
        }
        outDir = dir.path();
    }

    QString inDir;
    if (parser.isSet("indir")) {
        // check if it is a directory
        QDir dir(parser.value("indir"));
        if (!dir.exists()) {
            qCritical() << "indir" << parser.value("indir") << "does not exist";
            exit(1);
        }
        inDir = dir.path();
    }

    bool verbose = parser.isSet("verbose");

    int exitValue = 0;

    int successful = 0;
    int failed = 0;
    foreach(const QString &filename, parser.positionalArguments()) {
        QFileInfo file(filename);
        QString checkDir;
        if (!parser.isSet("indir")) {
            checkDir = filename + ".check";
        }
        else {
            checkDir = inDir + '/' + file.fileName() + ".check";
        }

        // this is wrong for multiple files in different dirs
        if (!parser.isSet("outdir")) {
            outDir = file.path();
        }

        qDebug() << "filename" << filename << "path" << file.path() << file.completeBaseName() << checkDir << file.absoluteFilePath();
        qDebug() << "inDir" << inDir << "outDir" << outDir << "checkDir" << checkDir;

        // filename must be a absolute path
        KoDocument* document = openFile(file.absoluteFilePath());
        if (!document) {
            exit(2);
        }

        QVector<QImage> thumbnails(createThumbnails(document, QSize(800,800)));

        qDebug() << "created" << thumbnails.size() << "thumbnails";
        if (create) {
            saveThumbnails(file, thumbnails, outDir);
        }
        else if (verify) {
            if (parser.isSet("outdir")) {
                saveThumbnails(file, thumbnails, outDir);
            }
            if (checkThumbnails(thumbnails, checkDir, verbose)) {
                ++successful;
            }
            else {
                ++failed;
                exitValue = 2;
            }
        }
        else if (roundtrip) {
            QString rFilename = saveFile(document, filename, "cstester-roundtrip");
            delete document;
            QFileInfo rFile(rFilename);
            qDebug() << roundtrip << "rFilename" << rFilename << rFile.absoluteFilePath();
            document = openFile(rFile.absoluteFilePath());
            QVector<QImage> others(createThumbnails(document, QSize(800,800)));
            if (parser.isSet("outdir")) {
                saveThumbnails(file, others, outDir);
                saveThumbnails(file, thumbnails, outDir + "/before");
            }
            if (checkThumbnails(thumbnails, others, verbose)) {
                ++successful;
            }
            else {
                ++failed;
                exitValue = 2;
            }
        }
        delete document;
    }

    if (verify || roundtrip) {
        qDebug() << "Totals:" << successful << "passed" << failed << "failed";
    }

    QTimer::singleShot(1, &app, SLOT(quit()));
    app.exec();
    return exitValue;
}
