/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SvgSavingContext.h"
#include "SvgUtil.h"

#include <KoImageData.h>
#include <KoShape.h>
#include <KoShapeGroup.h>
#include <KoShapeLayer.h>
#include <KoXmlWriter.h>

#include <QTemporaryFile>

#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QMimeDatabase>
#include <QMimeType>
#include <QTransform>

class Q_DECL_HIDDEN SvgSavingContext::Private
{
public:
    Private(QIODevice &outputDevice)
        : output(outputDevice)
        , styleWriter(nullptr)
        , shapeWriter(nullptr)
        , saveInlineImages(true)
    {
        styleWriter = new KoXmlWriter(&styleBuffer);
        styleWriter->startElement("defs");
        shapeWriter = new KoXmlWriter(&shapeBuffer);

        const qreal scaleToUserSpace = SvgUtil::toUserSpace(1.0);
        userSpaceMatrix.scale(scaleToUserSpace, scaleToUserSpace);
    }

    ~Private()
    {
        delete styleWriter;
        delete shapeWriter;
    }

    QIODevice &output;
    QBuffer styleBuffer;
    QBuffer shapeBuffer;
    KoXmlWriter *styleWriter;
    KoXmlWriter *shapeWriter;

    QHash<QString, int> uniqueNames;
    QHash<const KoShape *, QString> shapeIds;
    QTransform userSpaceMatrix;
    bool saveInlineImages;
};

SvgSavingContext::SvgSavingContext(QIODevice &outputDevice, bool saveInlineImages)
    : d(new Private(outputDevice))
{
    d->saveInlineImages = saveInlineImages;
}

SvgSavingContext::~SvgSavingContext()
{
    d->styleWriter->endElement();
    d->output.write(d->styleBuffer.data());
    d->output.write("\n");
    d->output.write(d->shapeBuffer.data());

    delete d;
}

KoXmlWriter &SvgSavingContext::styleWriter()
{
    return *d->styleWriter;
}

KoXmlWriter &SvgSavingContext::shapeWriter()
{
    return *d->shapeWriter;
}

QString SvgSavingContext::createUID(const QString &base)
{
    QString idBase = base.isEmpty() ? "defitem" : base;
    int counter = d->uniqueNames.value(idBase);
    d->uniqueNames.insert(idBase, counter + 1);

    return idBase + QString("%1").arg(counter);
}

QString SvgSavingContext::getID(const KoShape *obj)
{
    QString id;
    // do we have already an id for this object ?
    if (d->shapeIds.contains(obj)) {
        // use existing id
        id = d->shapeIds[obj];
    } else {
        // initialize from object name
        id = obj->name();
        // if object name is not empty and was not used already
        // we can use it as is
        if (!id.isEmpty() && !d->uniqueNames.contains(id)) {
            // add to unique names so it does not get reused
            d->uniqueNames.insert(id, 1);
        } else {
            if (id.isEmpty()) {
                // differentiate a little between shape types
                if (dynamic_cast<const KoShapeGroup *>(obj))
                    id = "group";
                else if (dynamic_cast<const KoShapeLayer *>(obj))
                    id = "layer";
                else
                    id = "shape";
            }
            // create a completely new id based on object name
            // or a generic name
            id = createUID(id);
        }
        // record id for this shape
        d->shapeIds.insert(obj, id);
    }
    return id;
}

QTransform SvgSavingContext::userSpaceTransform() const
{
    return d->userSpaceMatrix;
}

bool SvgSavingContext::isSavingInlineImages() const
{
    return d->saveInlineImages;
}

QString SvgSavingContext::createFileName(const QString &extension)
{
    QFile *file = qobject_cast<QFile *>(&d->output);
    if (!file)
        return QString();

    QFileInfo fi(file->fileName());
    QString path = fi.absolutePath();
    QString dstBaseFilename = fi.baseName();

    // create a filename for the image file at the destination directory
    QString fname = dstBaseFilename + '_' + createUID("file");

    // check if file exists already
    int i = 0;
    QString counter;
    // change filename as long as the filename already exists
    while (QFile(path + fname + counter + extension).exists()) {
        counter = QString("_%1").arg(++i);
    }

    return fname + counter + extension;
}

QString SvgSavingContext::saveImage(const QImage &image)
{
    if (isSavingInlineImages()) {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        if (image.save(&buffer, "PNG")) {
            QMimeDatabase db;
            const QString mimeType(db.mimeTypeForData(ba).name());
            const QString header("data:" + mimeType + ";base64,");
            return header + ba.toBase64();
        }
    } else {
        // write to a temp file first
        QTemporaryFile imgFile;
        if (image.save(&imgFile, "PNG")) {
            // tz: TODO the new version of KoImageData has the extension save inside maybe that can be used
            // get the mime type from the temp file content
            QMimeDatabase db;
            QMimeType mimeType = db.mimeTypeForFile(imgFile.fileName());
            // get extension from mimetype
            QString ext;
            QStringList patterns = mimeType.globPatterns();
            if (patterns.count())
                ext = patterns.first().mid(1);

            QString dstFilename = createFileName(ext);

            if (QFile::copy(imgFile.fileName(), dstFilename)) {
                return dstFilename;
            } else {
                QFile f(imgFile.fileName());
                f.remove();
            }
        }
    }

    return QString();
}

QString SvgSavingContext::saveImage(KoImageData *image)
{
    if (isSavingInlineImages()) {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        if (image->saveData(buffer)) {
            QMimeDatabase db;
            const QString mimeType(db.mimeTypeForData(ba).name());
            const QString header("data:" + mimeType + ";base64,");
            return header + ba.toBase64();
        }
    } else {
        // write to a temp file first
        QTemporaryFile imgFile;
        if (image->saveData(imgFile)) {
            // tz: TODO the new version of KoImageData has the extension save inside maybe that can be used
            // get the mime type from the temp file content
            QMimeDatabase db;
            QMimeType mimeType = db.mimeTypeForFile(imgFile.fileName());
            // get extension from mimetype
            QString ext;
            QStringList patterns = mimeType.globPatterns();
            if (patterns.count())
                ext = patterns.first().mid(1);

            QString dstFilename = createFileName(ext);

            // move the temp file to the destination directory
            if (QFile::copy(imgFile.fileName(), dstFilename)) {
                return dstFilename;
            } else {
                QFile f(imgFile.fileName());
                f.remove();
            }
        }
    }
    return QString();
}
