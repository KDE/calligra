/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
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

#include "MctEmbObjProperties.h"
#include "MctStaticData.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <KoBorder.h>
#include <KoShapeAnchor.h>
#include "KoProperties.h"
#include "KoColorBackground.h"
#include "KoShapeStrokeModel.h"
#include "KoShapeStroke.h"

#include "kzip.h"

MctEmbObjProperties::MctEmbObjProperties(QString name)
    : MctPropertyBase(name)
{

}

MctEmbObjProperties::MctEmbObjProperties(QString name, KoShape *shape)
    : MctPropertyBase(name)
    , shape(shape)
{
    fillUpProperties();
}

MctEmbObjProperties::MctEmbObjProperties(QString name, KoShape *shape, QPointF pos, QSizeF size)
    : MctPropertyBase(name)
    , shape(shape)
    , pos(pos)
{
    //this->shape->setPosition(pos);
    fillUpProperties();
    this->size = size;
}


MctEmbObjProperties::~MctEmbObjProperties()
{

}

bool MctEmbObjProperties::isBackedup()
{
    return isBackedUp;
}

QString MctEmbObjProperties::getURL() const
{
    return URL;
}

void MctEmbObjProperties::setURL(const QString &value)
{
    URL = value;
}

QString MctEmbObjProperties::getOdtURL() const
{
    return odtURL;
}

void MctEmbObjProperties::setOdtUrl(QString odt)
{
    odtURL = odt;
}

bool MctEmbObjProperties::getIsBackedUp() const
{
    return isBackedUp;
}

void MctEmbObjProperties::setIsBackedUp(bool value)
{
    isBackedUp = value;
}

bool MctEmbObjProperties::getWasInserted() const
{
    return wasInserted;
}

void MctEmbObjProperties::setWasInserted(bool value)
{
    wasInserted = value;
}
bool MctEmbObjProperties::getDoRestoreWhenDeleted() const
{
    return doRestoreWhenDeleted;
}

void MctEmbObjProperties::setDoRestoreWhenDeleted(bool value)
{
    doRestoreWhenDeleted = value;
}

QPointF MctEmbObjProperties::getCenterPos() const
{
    QPointF centerPos = pos;
    centerPos.setX(pos.x() + (this->size.width()/2.0));
    centerPos.setY(pos.y() + (this->size.height()/2.0));
    return centerPos;
}

void MctEmbObjProperties::setPositionInExport()
{
    props2export->remove("PositionX");
    props2export->insert("PositionX", this->pos.x());
    props2export->remove("PositionY");
    props2export->insert("PositionY", this->pos.y());
}

QPointF MctEmbObjProperties::getPos() const
{
    return pos;
}

void MctEmbObjProperties::fillUpProperties()
{
    qDebug() << "Pos x: " << shape->position().x() << " - Pos y: " << shape->position().y();
    props2export->insert("PositionX", shape->position().x());
    props2export->insert("PositionY", shape->position().y());
    props2export->insert("Height", shape->size().height());
    props2export->insert("Width", shape->size().width());
    props2export->insert("Rotation", shape->rotation());
    if (shape->anchor()){
        props2export->insert("AnchorType", shape->anchor()->anchorType());
        props2export->insert("VertOrientPosition", shape->anchor()->verticalPos());
        props2export->insert("HoriOrientPosition", shape->anchor()->horizontalPos());
    }
    if (shape->border()){
        props2export->insert("TopBorderDistance", shape->border()->borderSpacing(KoBorder::BorderSide::TopBorder));
        props2export->insert("BottomBorderDistance", shape->border()->borderSpacing(KoBorder::BorderSide::BottomBorder));
        props2export->insert("RightBorderDistance", shape->border()->borderSpacing(KoBorder::BorderSide::RightBorder));
        props2export->insert("LeftBorderDistance", shape->border()->borderSpacing(KoBorder::BorderSide::LeftBorder));
        props2export->insert("TopBorder", shape->border()->borderStyle(KoBorder::BorderSide::TopBorder));
        props2export->insert("BottomBorder", shape->border()->borderStyle(KoBorder::BorderSide::BottomBorder));
        props2export->insert("RightBorder", shape->border()->borderStyle(KoBorder::BorderSide::RightBorder));
        props2export->insert("LeftBorder", shape->border()->borderStyle(KoBorder::BorderSide::LeftBorder));
    }
    if (shape->rotation()){
        props2export->insert("GraphicRotation", shape->rotation());
    }
    props2export->insert("TopMargin", shape->textRunAroundDistanceTop());
    props2export->insert("BottomMargin", shape->textRunAroundDistanceBottom());
    props2export->insert("RightMargin", shape->textRunAroundDistanceRight());
    props2export->insert("LeftMargin", shape->textRunAroundDistanceLeft());
    props2export->insert("Transparency", shape->transparency());
    props2export->insert("ZOrder", shape->zIndex());
    auto backGround = shape->background();
    auto backgData = dynamic_cast<KoColorBackground*>(backGround.data());
    if (backgData){
        props2export->insert("BGRed", backgData->color().red());
        props2export->insert("BGGreen", backgData->color().green());
        props2export->insert("BGBlue", backgData->color().blue());
    }
}

void MctEmbObjProperties::addStrokeStyleChanges(KoShapeStroke *newStroke)
{
    if (shape->stroke()){
        KoShapeStroke* stroke = dynamic_cast<KoShapeStroke*>(shape->stroke());
        if (stroke->color() != newStroke->color()){
            props2export->insert("StrokeColorRed", stroke->color().red());
            props2export->insert("StrokeColorGreen", stroke->color().green());
            props2export->insert("StrokeColorBlue", stroke->color().blue());
            props2export->insert("StrokeColorAlpha", stroke->color().alpha());
        }

        if (stroke->lineWidth() != newStroke->lineWidth())
            props2export->insert("StrokeLineWidth", stroke->lineWidth());

        if (stroke->miterLimit() != newStroke->miterLimit())
            props2export->insert("StrokeMiterLimit", stroke->miterLimit());

        if (stroke->lineStyle() != newStroke->lineStyle()){
            props2export->insert("StrokeLineStyle", (int) stroke->lineStyle());
            QString ldString = "";
            int count = 0;
            for (qreal ld : stroke->lineDashes()){
                ldString = ldString + QString::number(ld);
                if (count != stroke->lineDashes().size()-1) ldString += ",";
                count++;
            }
            props2export->insert("StrokeLineDashes", ldString);
        }
    }
}

void MctEmbObjProperties::addShadowStyleChanges(KoShapeShadow *newShadow)
{
    if (shape->shadow()){
        KoShapeShadow *shadow = shape->shadow();
        if (shadow->isVisible() != newShadow->isVisible()){
            props2export->insert("ShadowVisible", shadow->isVisible());
        }

        if (shadow->color() != newShadow->color()){
            props2export->insert("ShadowColorRed", shadow->color().red());
            props2export->insert("ShadowColorGreen", shadow->color().green());
            props2export->insert("ShadowColorBlue", shadow->color().blue());
            props2export->insert("ShadowColorAlpha", shadow->color().alpha());
        }

        if (shadow->offset() != newShadow->offset()){
            QString offset = QString::number(shadow->offset().x()) + "," + QString::number(shadow->offset().y());
            props2export->insert("ShadowOffset", offset);
        }

        if (shadow->blur() != newShadow->blur()){
            props2export->insert("ShadowBlur", shadow->blur());
        }
    } else {
        props2export->insert("ShadowVisible", false);
    }
}

void MctEmbObjProperties::addSizeChanged(QSizeF prevPos)
{
    props2export->insert("PrevSizeWidth", prevPos.width());
    props2export->insert("PrevSizeHeight", prevPos.height());
}

void MctEmbObjProperties::addRotationChanged(double rotation)
{
    props2export->insert("PrevRotation", rotation);
}

KoShape *MctEmbObjProperties::getShape()
{
    return shape;
}

void MctEmbObjProperties::setPrevPos(QPointF prevPos)
{
    props2export->insert("PrevPositionX", prevPos.x());
    props2export->insert("PrevPositionY", prevPos.y());
}

void MctEmbObjProperties::textGraphicStyleChanges(MctEmbObjProperties *props2, KoShape *shape)
{
    KoShapeStroke* stroke = dynamic_cast<KoShapeStroke*>(shape->stroke());

    if (this->getProps2Export()->contains("StrokeLineWidth")){
        KoShapeStroke* strokeChange = new KoShapeStroke(*stroke);
        strokeChange->setLineWidth(this->getProps2Export()->value("StrokeLineWidth").toDouble());
        props2->addStrokeStyleChanges(strokeChange);
        stroke->setLineWidth(this->getProps2Export()->value("StrokeLineWidth").toDouble());
        shape->setStroke(stroke);
        delete strokeChange;
    }

    if (this->getProps2Export()->contains("StrokeColorRed")){
        QColor color(this->getProps2Export()->value("StrokeColorRed").toInt(),this->getProps2Export()->value("StrokeColorGreen").toInt(),this->getProps2Export()->value("StrokeColorBlue").toInt(), this->getProps2Export()->value("StrokeColorAlpha").toInt());
        KoShapeStroke* strokeChange = new KoShapeStroke(*stroke);
        strokeChange->setColor(QColor(color));
        props2->addStrokeStyleChanges(strokeChange);
        stroke->setColor(QColor(color));
        shape->setStroke(stroke);
        delete strokeChange;
    }

    if (this->getProps2Export()->contains("StrokeMiterLimit")){
        KoShapeStroke* strokeChange = new KoShapeStroke(*stroke);
        strokeChange->setMiterLimit(this->getProps2Export()->value("StrokeMiterLimit").toDouble());
        props2->addStrokeStyleChanges(strokeChange);
        stroke->setMiterLimit(this->getProps2Export()->value("StrokeMiterLimit").toDouble());
        shape->setStroke(stroke);
        delete strokeChange;
    }

    if (this->getProps2Export()->contains("StrokeLineStyle")){
        KoShapeStroke* strokeChange = new KoShapeStroke(*stroke);
        QVector<qreal> dashes;
        if (this->getProps2Export()->value("StrokeLineDashes") != ""){
            QStringList dashSplit = this->getProps2Export()->value("StrokeLineDashes").toString().split(",");
            foreach (QString d, dashSplit){
               dashes.push_back(d.toDouble());
            }
        }
        strokeChange->setLineStyle(Qt::PenStyle(this->getProps2Export()->value("StrokeLineStyle").toInt()), dashes);
        props2->addStrokeStyleChanges(strokeChange);
        stroke->setLineStyle(Qt::PenStyle(this->getProps2Export()->value("StrokeLineStyle").toInt()), dashes);
        shape->setStroke(stroke);
        delete strokeChange;
    }

    KoShapeShadow *shadow = shape->shadow();
    auto copyShadow = [] (KoShapeShadow* shadow) -> KoShapeShadow* {
        KoShapeShadow* shadowChange = new KoShapeShadow();
        shadowChange->setBlur(shadow->blur());
        shadowChange->setColor(shadow->color());
        shadowChange->setOffset(shadow->offset());
        shadowChange->setVisible(shadow->isVisible());
        return shadowChange;
    };

    if (this->getProps2Export()->contains("ShadowVisible")){
        KoShapeShadow* shadowChange = copyShadow(shadow);
        shadowChange->setVisible(this->getProps2Export()->value("ShadowVisible").toBool());
        props2->addShadowStyleChanges(shadowChange);
        shadow->setVisible(this->getProps2Export()->value("ShadowVisible").toBool());
        shape->setShadow(shadow);
        delete shadowChange;
    }

    if (this->getProps2Export()->contains("ShadowColorRed")){
        QColor shadowColor(this->getProps2Export()->value("ShadowColorRed").toInt(),this->getProps2Export()->value("ShadowColorGreen").toInt(),this->getProps2Export()->value("ShadowColorBlue").toInt(), this->getProps2Export()->value("ShadowColorAlpha").toInt());
        KoShapeShadow* shadowChange = copyShadow(shadow);
        shadowChange->setColor(shadowColor);
        props2->addShadowStyleChanges(shadowChange);
        shadow->setColor(shadowColor);
        shape->setShadow(shadow);
        delete shadowChange;
    }

    if (this->getProps2Export()->contains("ShadowOffset")){
        KoShapeShadow* shadowChange = copyShadow(shadow);
        QStringList offsetStrip = this->getProps2Export()->value("ShadowOffset").toString().split(",");
        QPointF offset(offsetStrip[0].toDouble(), offsetStrip[1].toDouble());
        shadowChange->setOffset(offset);
        props2->addShadowStyleChanges(shadowChange);
        shadow->setOffset(offset);
        shape->setShadow(shadow);
        delete shadowChange;
    }

    if (this->getProps2Export()->contains("ShadowBlur")){
        KoShapeShadow* shadowChange = copyShadow(shadow);
        shadowChange->setBlur(this->getProps2Export()->value("ShadowBlur").toDouble());
        props2->addShadowStyleChanges(shadowChange);
        shadow->setBlur(this->getProps2Export()->value("ShadowBlur").toDouble());
        shape->setShadow(shadow);
        delete shadowChange;
    }
}

/**
 * @brief MctEmbObjProperties::convertURL common method to swap the prefix of a URL.
 * @param oldPrefix The URL should start with this.
 * @param newPrefix The URL should start with this.
 * @param name The original identifier that should contain oldPrefix.
 * @return The identifier starting with the newPrefix substring.
 */
QString MctEmbObjProperties::convertURL(QString oldPrefix, QString newPrefix, QString name)
{
    QString returnURL;

    if (name.startsWith(oldPrefix))
        returnURL = newPrefix + name.mid(oldPrefix.length());
    else
        returnURL = name;

    return returnURL;
}

bool MctEmbObjProperties::removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

/**
 * @brief This makes a copy of a metadata stored in the Odt.
 * @param innerURL The relative URL in the Odt.
 * @param OdtURL The absolute URL of the Odt.
 * @return returns with the URL of the backed up file
 */
QString MctEmbObjProperties::backupFileFromOdt(QString innerURL, QString OdtURL)
{
    if(isBackedUp) {
        return URL;
    }

    this->odtURL = OdtURL;
    this->innerURL = innerURL;

    if(odtURL.isEmpty() || this->innerURL.isEmpty()) {
        return "";
    }

    QFileInfo fi(odtURL);
    QString dirname = fi.path();
    QString outfilename_temp = fi.fileName();

    KZip *zfile = new KZip(odtURL);
    zfile->open(QIODevice::ReadOnly);

    bool found = false;
    QStringList entries = zfile->directory()->entries();
    foreach (QString item, entries) {
        if (item == this->innerURL) {
            found = true;
            this->innerURL = item;
            QFileInfo fi2(this->innerURL);
            QString innerdirname = fi2.path();
            URL = dirname + QDir::separator() + "temp_mct_" + fi2.fileName();

            const KArchiveEntry * kentry = zfile->directory()->entry(item);
            const KZipFileEntry * zipEntry = static_cast<const KZipFileEntry *>(kentry);

            zipEntry->copyTo(dirname);

            QDir d(dirname + QDir::separator() + innerdirname);
            d.rename(dirname + QDir::separator() + this->innerURL, URL);
            //d.removeRecursively();  //Does not work with Qt4
            removeDir(dirname + QDir::separator() + innerdirname);
        } else if (item == "META-INF/manifest.xml") {
            QString manifestfile = odtURL + "_manifest.xml";
            QFile fd(manifestfile);
            const KArchiveEntry * kentry = zfile->directory()->entry(item);
            const KZipFileEntry * zipEntry = static_cast<const KZipFileEntry *>(kentry);

            fd.open(QIODevice::WriteOnly);
            fd.write(zipEntry->data());
            fd.close();

            getMediaType(manifestfile);

            fd.remove();
        }
    }

    zfile->close();
    qDebug() << "mediaType of the backed up file: " << mediaType;

    if(!found) {
        qDebug() << innerURL << " did not found in " << odtURL;
        return "";
    }

    qDebug() << "file " << innerURL << " backed up";
    isBackedUp = true;
    return URL;
}

/**
 * @brief MctEmbObjProperties::restoreBackupFile Restore embedded file from backup (if it exists)
 */
void MctEmbObjProperties::restoreBackupFile()
{
    if (isBackedup()) {
        qCritical() << "Trying to restore without backup.";
    }

    QString  outfileName = MctStaticData::instance()->tempMctFile(this->odtURL);
    QFileInfo file(outfileName);
    QString dirName = file.path();
    QString outfilename_temp = dirName + QDir::separator() + "temp_mct_" + file.fileName() + "#";

    KZip *zFile, *zFileOut;

    if (file.exists() && file.isFile())
        zFile = new KZip(outfileName);
    else
        zFile = new KZip(this->odtURL);

    outfileName = zFile->fileName();

    zFileOut = new KZip(outfilename_temp);

    zFile->open(QIODevice::ReadOnly);
    zFileOut->open(QIODevice::WriteOnly);

    bool found = false;

    QStringList entries = zFile->directory()->entries();
    foreach (QString item, entries) {
        const KArchiveEntry * kentry = zFile->directory()->entry(item);
        if (item == this->innerURL) {
            // The odt contains the objects
            const KZipFileEntry * zipEntry = static_cast<const KZipFileEntry *>(kentry);
            zFileOut->writeFile(item, QString(""), QString(""), zipEntry->data().constData(), zipEntry->data().size());
            found = true;
        } else if (item == "META-INF/manifest.xml") {
            // Update metafile too
            const KZipFileEntry * zipEntry = static_cast<const KZipFileEntry *>(kentry);
            QString maniFileName = odtURL + "embObj_manifest.xml";
            QFile manifestFile(maniFileName);
            manifestFile.open(QIODevice::ReadWrite);
            manifestFile.write(zipEntry->data());

            regManifest(maniFileName);

            zFileOut->addLocalFile(maniFileName, item);

            manifestFile.remove();
        } else {
            // Other entries
            if (kentry->isDirectory()) {
                // If it's a dir: recursive copy
                MctStaticData::recursiveDirectoryCopy(static_cast<const KArchiveDirectory *>(kentry), item, zFileOut);
            } else {
                // ?
            }
        }
    }

    if (!found) // If the odt doesn't contains object, recover it
        zFileOut->addLocalFile(URL, innerURL);

    zFile->close();
    zFileOut->close();

    QFile f(outfileName);
    f.remove();

    QFile::rename(outfilename_temp, outfileName);

    qDebug() << "Embedded file restored";
}

/**
 * @brief MctEmbObjProperties::deleteBackupFile cleans up backup files.
 */
void MctEmbObjProperties::deleteBackupFile()
{
    if (!isBackedup())
        return;

    QFile f(URL);
    f.remove();

    isBackedUp = false;

    qDebug() << URL << " backup file deleted";
}

/**
 * @brief MctEmbObjProperties::getMediaType Update the value of this->mediaType member from the given manifest file
 * @param fileName Name of manifest file. (Usually .../odtURL/manifest.xml)
 */
void MctEmbObjProperties::getMediaType(QString fileName)
{
    QDomDocument *manifest_tree = new QDomDocument(fileName);

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray xml = file.readAll();
    manifest_tree->setContent(xml);
    file.close();

    QDomElement rootNode = manifest_tree->firstChildElement();
    QStringList tmplist = rootNode.tagName().split(":");
    QString manifest = tmplist.first() + ":";

    QDomNodeList childs = rootNode.childNodes();

    int innerURLLength = innerURL.length();
    for(uint i = 0; i < childs.length(); i++) {
        QDomNode node = childs.at(i);
        if(node.isElement()) {
            QDomElement elem = node.toElement();
            QString full_path = elem.attribute(manifest + "full-path");
            if (full_path == this->innerURL) {
                this->mediaType = elem.attribute(manifest + "media-type");
                break;  // FIXME: one element could be in the list only once, why to cycle furthert?
            }
        }
    }
}

/**
 * @brief MctEmbObjProperties::regManifest
 * @param fileName
 */
void MctEmbObjProperties::regManifest(QString fileName)
{
    qDebug() << "registering manifest";
    QDomDocument *manifest_tree = new QDomDocument(fileName);

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray xml = file.readAll();
    manifest_tree->setContent(xml);
    file.close();

    QDomElement rootNode = manifest_tree->firstChildElement();
    QStringList tmplist = rootNode.tagName().split(":");
    QString manifest = tmplist.first() + ":";

    /// FIXME: Really similar to MctAbstractGraph::regManifest(QString filename)...

    QString tag = manifest + "file-entry";
    QDomElement elem = manifest_tree->createElement(tag);
    elem.setAttribute(manifest+"media-type", this->mediaType);
    elem.setAttribute(manifest+"full-path", this->innerURL);
    rootNode.appendChild(elem);

    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << manifest_tree->toString();
    file.close();
}

/**
 * @brief MctEmbObjProperties::compareProperties gives a a dictionary about the not identical elements
 * @param otherEmbObjProperties The dictionary that should contains all element of this object's props dictionary.
 * @return The new dictionary about the differences.
 */
PropertyDictionary *MctEmbObjProperties::compareProperties(const MctEmbObjProperties &otherEmbObjProperties)
{
    QList<QString> keys = otherEmbObjProperties.props->keys();

    PropertyDictionary *differentProps = new PropertyDictionary();

    for (auto it = this->props->begin(); it != this->props->end(); ++it) {

        if (it.key() == "GraphicURL") continue;

        // Kulonbseg van, ha:
        if (!keys.contains(it.key())            // It does NOT contains
                || (keys.contains(it.key())     // It contains BUT with different value
                    && it.value() != otherEmbObjProperties.props->value(it.key())))
            differentProps->insert(it.key(), it.value());
    }

    return differentProps;
}

/**
 * @brief MctEmbObjProperties::createInnerURL transforms grapicURL into innerURL
 * @param graphicURL Should starts with someting like this: "nd.sun.star.GraphicObject:"identifier
 * @return The innerURL that looks like this: "Picture/"identifier
 */
QString MctEmbObjProperties::createInnerURL(QString graphicURL)
{
    return convertURL(MctStaticData::GRAPHICOBJ, MctStaticData::GRAPHICOBJ_PATH, graphicURL);
}

/**
 * @brief MctEmbObjProperties::createGraphicURL transforms innerURL into grapicURL
 * @param innerURL The innerURL should looks like this: "Picture/"identifier
 * @return The grapicURL that looks like this: "nd.sun.star.GraphicObject:"identifier
 */
QString MctEmbObjProperties::createGraphicURL(QString innerURL)
{
    return convertURL(MctStaticData::GRAPHICOBJ_PATH, MctStaticData::GRAPHICOBJ, innerURL);
}

