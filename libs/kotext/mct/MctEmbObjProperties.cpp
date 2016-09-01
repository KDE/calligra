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

MctEmbObjProperties::MctEmbObjProperties(const QString &name)
    : MctPropertyBase(name)
{

}

MctEmbObjProperties::MctEmbObjProperties(const QString &name, KoShape *shape)
    : MctPropertyBase(name)
    , m_shape(shape)
{
    fillUpProperties();
}

MctEmbObjProperties::MctEmbObjProperties(const QString &name, KoShape *shape, const QPointF &pos, const QSizeF &size)
    : MctPropertyBase(name)
    , m_shape(shape)
    , m_pos(pos)
{
    //this->shape->setPosition(pos);
    fillUpProperties();
    m_size = size;
}


MctEmbObjProperties::~MctEmbObjProperties()
{

}

QString MctEmbObjProperties::url() const
{
    return m_url;
}

void MctEmbObjProperties::setURL(const QString &value)
{
    m_url = value;
}

QString MctEmbObjProperties::odtURL() const
{
    return m_odtURL;
}

void MctEmbObjProperties::setOdtUrl(const QString &odt)
{
    m_odtURL = odt;
}

bool MctEmbObjProperties::isBackedUp() const
{
    return m_isBackedUp;
}

void MctEmbObjProperties::setIsBackedUp(bool value)
{
    m_isBackedUp = value;
}

bool MctEmbObjProperties::wasInserted() const
{
    return m_wasInserted;
}

void MctEmbObjProperties::setWasInserted(bool value)
{
    m_wasInserted = value;
}
bool MctEmbObjProperties::doRestoreWhenDeleted() const
{
    return m_doRestoreWhenDeleted;
}

void MctEmbObjProperties::setDoRestoreWhenDeleted(bool value)
{
    m_doRestoreWhenDeleted = value;
}

QPointF MctEmbObjProperties::centerPos() const
{
    QPointF centerPos = m_pos;
    centerPos.setX(m_pos.x() + (m_size.width()/2.0));
    centerPos.setY(m_pos.y() + (m_size.height()/2.0));
    return centerPos;
}

void MctEmbObjProperties::setPositionInExport()
{
    m_props2export->remove("PositionX");
    m_props2export->insert("PositionX", m_pos.x());
    m_props2export->remove("PositionY");
    m_props2export->insert("PositionY", m_pos.y());
}

QPointF MctEmbObjProperties::pos() const
{
    return m_pos;
}

void MctEmbObjProperties::fillUpProperties()
{
    qDebug() << "Pos x: " << m_shape->position().x() << " - Pos y: " << m_shape->position().y();
    m_props2export->insert("PositionX", m_shape->position().x());
    m_props2export->insert("PositionY", m_shape->position().y());
    m_props2export->insert("Height", m_shape->size().height());
    m_props2export->insert("Width", m_shape->size().width());
    m_props2export->insert("Rotation", m_shape->rotation());
    if (m_shape->anchor()){
        m_props2export->insert("AnchorType", m_shape->anchor()->anchorType());
        m_props2export->insert("VertOrientPosition", m_shape->anchor()->verticalPos());
        m_props2export->insert("HoriOrientPosition", m_shape->anchor()->horizontalPos());
    }
    if (m_shape->border()){
        m_props2export->insert("TopBorderDistance", m_shape->border()->borderSpacing(KoBorder::BorderSide::TopBorder));
        m_props2export->insert("BottomBorderDistance", m_shape->border()->borderSpacing(KoBorder::BorderSide::BottomBorder));
        m_props2export->insert("RightBorderDistance", m_shape->border()->borderSpacing(KoBorder::BorderSide::RightBorder));
        m_props2export->insert("LeftBorderDistance", m_shape->border()->borderSpacing(KoBorder::BorderSide::LeftBorder));
        m_props2export->insert("TopBorder", m_shape->border()->borderStyle(KoBorder::BorderSide::TopBorder));
        m_props2export->insert("BottomBorder", m_shape->border()->borderStyle(KoBorder::BorderSide::BottomBorder));
        m_props2export->insert("RightBorder", m_shape->border()->borderStyle(KoBorder::BorderSide::RightBorder));
        m_props2export->insert("LeftBorder", m_shape->border()->borderStyle(KoBorder::BorderSide::LeftBorder));
    }
    if (m_shape->rotation()){
        m_props2export->insert("GraphicRotation", m_shape->rotation());
    }
    m_props2export->insert("TopMargin", m_shape->textRunAroundDistanceTop());
    m_props2export->insert("BottomMargin", m_shape->textRunAroundDistanceBottom());
    m_props2export->insert("RightMargin", m_shape->textRunAroundDistanceRight());
    m_props2export->insert("LeftMargin", m_shape->textRunAroundDistanceLeft());
    m_props2export->insert("Transparency", m_shape->transparency());
    m_props2export->insert("ZOrder", m_shape->zIndex());
    auto backGround = m_shape->background();
    auto backgData = dynamic_cast<KoColorBackground*>(backGround.data());
    if (backgData){
        m_props2export->insert("BGRed", backgData->color().red());
        m_props2export->insert("BGGreen", backgData->color().green());
        m_props2export->insert("BGBlue", backgData->color().blue());
    }
}

void MctEmbObjProperties::addStrokeStyleChanges(KoShapeStroke *newStroke)
{
    if (m_shape->stroke()){
        KoShapeStroke* stroke = dynamic_cast<KoShapeStroke*>(m_shape->stroke());
        if (stroke->color() != newStroke->color()){
            m_props2export->insert("StrokeColorRed", stroke->color().red());
            m_props2export->insert("StrokeColorGreen", stroke->color().green());
            m_props2export->insert("StrokeColorBlue", stroke->color().blue());
            m_props2export->insert("StrokeColorAlpha", stroke->color().alpha());
        }

        if (stroke->lineWidth() != newStroke->lineWidth())
            m_props2export->insert("StrokeLineWidth", stroke->lineWidth());

        if (stroke->miterLimit() != newStroke->miterLimit())
            m_props2export->insert("StrokeMiterLimit", stroke->miterLimit());

        if (stroke->lineStyle() != newStroke->lineStyle()){
            m_props2export->insert("StrokeLineStyle", (int) stroke->lineStyle());
            QString ldString = "";
            int count = 0;
            for (qreal ld : stroke->lineDashes()){
                ldString = ldString + QString::number(ld);
                if (count != stroke->lineDashes().size()-1) ldString += ",";
                count++;
            }
            m_props2export->insert("StrokeLineDashes", ldString);
        }
    }
}

void MctEmbObjProperties::addShadowStyleChanges(KoShapeShadow *newShadow)
{
    if (m_shape->shadow()){
        KoShapeShadow *shadow = m_shape->shadow();
        if (shadow->isVisible() != newShadow->isVisible()){
            m_props2export->insert("ShadowVisible", shadow->isVisible());
        }

        if (shadow->color() != newShadow->color()){
            m_props2export->insert("ShadowColorRed", shadow->color().red());
            m_props2export->insert("ShadowColorGreen", shadow->color().green());
            m_props2export->insert("ShadowColorBlue", shadow->color().blue());
            m_props2export->insert("ShadowColorAlpha", shadow->color().alpha());
        }

        if (shadow->offset() != newShadow->offset()){
            QString offset = QString::number(shadow->offset().x()) + "," + QString::number(shadow->offset().y());
            m_props2export->insert("ShadowOffset", offset);
        }

        if (shadow->blur() != newShadow->blur()){
            m_props2export->insert("ShadowBlur", shadow->blur());
        }
    } else {
        m_props2export->insert("ShadowVisible", false);
    }
}

void MctEmbObjProperties::addSizeChanged(const QSizeF &prevPos)
{
    m_props2export->insert("PrevSizeWidth", prevPos.width());
    m_props2export->insert("PrevSizeHeight", prevPos.height());
}

void MctEmbObjProperties::addRotationChanged(double rotation)
{
    m_props2export->insert("PrevRotation", rotation);
}

KoShape *MctEmbObjProperties::shape()
{
    return m_shape;
}

void MctEmbObjProperties::setPrevPos(const QPointF &prevPos)
{
    m_props2export->insert("PrevPositionX", prevPos.x());
    m_props2export->insert("PrevPositionY", prevPos.y());
}

void MctEmbObjProperties::textGraphicStyleChanges(MctEmbObjProperties *props2, KoShape *shape)
{
    KoShapeStroke* stroke = dynamic_cast<KoShapeStroke*>(shape->stroke());

    if (this->props2Export()->contains("StrokeLineWidth")){
        KoShapeStroke* strokeChange = new KoShapeStroke(*stroke);
        strokeChange->setLineWidth(this->props2Export()->value("StrokeLineWidth").toDouble());
        props2->addStrokeStyleChanges(strokeChange);
        stroke->setLineWidth(this->props2Export()->value("StrokeLineWidth").toDouble());
        shape->setStroke(stroke);
        delete strokeChange;
    }

    if (this->props2Export()->contains("StrokeColorRed")){
        QColor color(this->props2Export()->value("StrokeColorRed").toInt(),this->props2Export()->value("StrokeColorGreen").toInt(),this->props2Export()->value("StrokeColorBlue").toInt(), this->props2Export()->value("StrokeColorAlpha").toInt());
        KoShapeStroke* strokeChange = new KoShapeStroke(*stroke);
        strokeChange->setColor(QColor(color));
        props2->addStrokeStyleChanges(strokeChange);
        stroke->setColor(QColor(color));
        shape->setStroke(stroke);
        delete strokeChange;
    }

    if (this->props2Export()->contains("StrokeMiterLimit")){
        KoShapeStroke* strokeChange = new KoShapeStroke(*stroke);
        strokeChange->setMiterLimit(this->props2Export()->value("StrokeMiterLimit").toDouble());
        props2->addStrokeStyleChanges(strokeChange);
        stroke->setMiterLimit(this->props2Export()->value("StrokeMiterLimit").toDouble());
        shape->setStroke(stroke);
        delete strokeChange;
    }

    if (this->props2Export()->contains("StrokeLineStyle")){
        KoShapeStroke* strokeChange = new KoShapeStroke(*stroke);
        QVector<qreal> dashes;
        if (this->props2Export()->value("StrokeLineDashes") != ""){
            QStringList dashSplit = this->props2Export()->value("StrokeLineDashes").toString().split(",");
            foreach (QString d, dashSplit){
               dashes.push_back(d.toDouble());
            }
        }
        strokeChange->setLineStyle(Qt::PenStyle(this->props2Export()->value("StrokeLineStyle").toInt()), dashes);
        props2->addStrokeStyleChanges(strokeChange);
        stroke->setLineStyle(Qt::PenStyle(this->props2Export()->value("StrokeLineStyle").toInt()), dashes);
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

    if (this->props2Export()->contains("ShadowVisible")){
        KoShapeShadow* shadowChange = copyShadow(shadow);
        shadowChange->setVisible(this->props2Export()->value("ShadowVisible").toBool());
        props2->addShadowStyleChanges(shadowChange);
        shadow->setVisible(this->props2Export()->value("ShadowVisible").toBool());
        shape->setShadow(shadow);
        delete shadowChange;
    }

    if (this->props2Export()->contains("ShadowColorRed")){
        QColor shadowColor(this->props2Export()->value("ShadowColorRed").toInt(),this->props2Export()->value("ShadowColorGreen").toInt(),this->props2Export()->value("ShadowColorBlue").toInt(), this->props2Export()->value("ShadowColorAlpha").toInt());
        KoShapeShadow* shadowChange = copyShadow(shadow);
        shadowChange->setColor(shadowColor);
        props2->addShadowStyleChanges(shadowChange);
        shadow->setColor(shadowColor);
        shape->setShadow(shadow);
        delete shadowChange;
    }

    if (this->props2Export()->contains("ShadowOffset")){
        KoShapeShadow* shadowChange = copyShadow(shadow);
        QStringList offsetStrip = this->props2Export()->value("ShadowOffset").toString().split(",");
        QPointF offset(offsetStrip[0].toDouble(), offsetStrip[1].toDouble());
        shadowChange->setOffset(offset);
        props2->addShadowStyleChanges(shadowChange);
        shadow->setOffset(offset);
        shape->setShadow(shadow);
        delete shadowChange;
    }

    if (this->props2Export()->contains("ShadowBlur")){
        KoShapeShadow* shadowChange = copyShadow(shadow);
        shadowChange->setBlur(this->props2Export()->value("ShadowBlur").toDouble());
        props2->addShadowStyleChanges(shadowChange);
        shadow->setBlur(this->props2Export()->value("ShadowBlur").toDouble());
        shape->setShadow(shadow);
        delete shadowChange;
    }
}

QString MctEmbObjProperties::convertURL(const QString &oldPrefix, const QString &newPrefix, const QString &name)
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

QString MctEmbObjProperties::backupFileFromOdt(const QString &innerURL, const QString &OdtURL)
{
    if(m_isBackedUp) {
        return m_url;
    }

    m_odtURL = OdtURL;
    m_innerURL = innerURL;

    if(m_odtURL.isEmpty() || m_innerURL.isEmpty()) {
        return "";
    }

    QFileInfo fi(m_odtURL);
    QString dirname = fi.path();
    QString outfilename_temp = fi.fileName();

    KZip *zfile = new KZip(m_odtURL);
    zfile->open(QIODevice::ReadOnly);

    bool found = false;
    QStringList entries = zfile->directory()->entries();
    foreach (QString item, entries) {
        if (item == this->m_innerURL) {
            found = true;
            this->m_innerURL = item;
            QFileInfo fi2(this->m_innerURL);
            QString innerdirname = fi2.path();
            m_url = dirname + QDir::separator() + "temp_mct_" + fi2.fileName();

            const KArchiveEntry * kentry = zfile->directory()->entry(item);
            const KZipFileEntry * zipEntry = static_cast<const KZipFileEntry *>(kentry);

            zipEntry->copyTo(dirname);

            QDir d(dirname + QDir::separator() + innerdirname);
            d.rename(dirname + QDir::separator() + this->m_innerURL, m_url);
            //d.removeRecursively();  //Does not work with Qt4
            removeDir(dirname + QDir::separator() + innerdirname);
        } else if (item == "META-INF/manifest.xml") {
            QString manifestfile = m_odtURL + "_manifest.xml";
            QFile fd(manifestfile);
            const KArchiveEntry * kentry = zfile->directory()->entry(item);
            const KZipFileEntry * zipEntry = static_cast<const KZipFileEntry *>(kentry);

            fd.open(QIODevice::WriteOnly);
            fd.write(zipEntry->data());
            fd.close();

            mediaType(manifestfile);

            fd.remove();
        }
    }

    zfile->close();
    qDebug() << "mediaType of the backed up file: " << m_mediaType;

    if(!found) {
        qDebug() << m_innerURL << " did not found in " << m_odtURL;
        return "";
    }

    qDebug() << "file " << innerURL << " backed up";
    m_isBackedUp = true;
    return m_url;
}

void MctEmbObjProperties::restoreBackupFile()
{
    if (isBackedUp()) {
        qCritical() << "Trying to restore without backup.";
    }

    QString  outfileName = MctStaticData::instance()->tempMctFile(m_odtURL);
    QFileInfo file(outfileName);
    QString dirName = file.path();
    QString outfilename_temp = dirName + QDir::separator() + "temp_mct_" + file.fileName() + "#";

    KZip *zFile, *zFileOut;

    if (file.exists() && file.isFile())
        zFile = new KZip(outfileName);
    else
        zFile = new KZip(m_odtURL);

    outfileName = zFile->fileName();

    zFileOut = new KZip(outfilename_temp);

    zFile->open(QIODevice::ReadOnly);
    zFileOut->open(QIODevice::WriteOnly);

    bool found = false;

    QStringList entries = zFile->directory()->entries();
    foreach (QString item, entries) {
        const KArchiveEntry * kentry = zFile->directory()->entry(item);
        if (item == this->m_innerURL) {
            // The odt contains the objects
            const KZipFileEntry * zipEntry = static_cast<const KZipFileEntry *>(kentry);
            zFileOut->writeFile(item, QString(""), QString(""), zipEntry->data().constData(), zipEntry->data().size());
            found = true;
        } else if (item == "META-INF/manifest.xml") {
            // Update metafile too
            const KZipFileEntry * zipEntry = static_cast<const KZipFileEntry *>(kentry);
            QString maniFileName = m_odtURL + "embObj_manifest.xml";
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
        zFileOut->addLocalFile(m_url, m_innerURL);

    zFile->close();
    zFileOut->close();

    QFile f(outfileName);
    f.remove();

    QFile::rename(outfilename_temp, outfileName);

    qDebug() << "Embedded file restored";
}

void MctEmbObjProperties::deleteBackupFile()
{
    if (!isBackedUp())
        return;

    QFile f(m_url);
    f.remove();

    m_isBackedUp = false;

    qDebug() << m_url << " backup file deleted";
}

void MctEmbObjProperties::mediaType(const QString &fileName)
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

    int innerURLLength = m_innerURL.length();
    for(uint i = 0; i < childs.length(); i++) {
        QDomNode node = childs.at(i);
        if(node.isElement()) {
            QDomElement elem = node.toElement();
            QString full_path = elem.attribute(manifest + "full-path");
            if (full_path == this->m_innerURL) {
                m_mediaType = elem.attribute(manifest + "media-type");
                break;  // FIXME: one element could be in the list only once, why to cycle furthert?
            }
        }
    }
}

void MctEmbObjProperties::regManifest(const QString &fileName)
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
    elem.setAttribute(manifest+"media-type", m_mediaType);  // FIXME: xmlns?
    elem.setAttribute(manifest+"full-path", m_innerURL);    // FIXME: xmlns?
    rootNode.appendChild(elem);

    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << manifest_tree->toString();
    file.close();
}

PropertyDictionary *MctEmbObjProperties::compareProperties(const MctEmbObjProperties &otherEmbObjProperties)
{
    QList<QString> keys = otherEmbObjProperties.m_props->keys();

    PropertyDictionary *differentProps = new PropertyDictionary();

    for (auto it = this->m_props->begin(); it != this->m_props->end(); ++it) {

        if (it.key() == "GraphicURL") continue;

        // Kulonbseg van, ha:
        if (!keys.contains(it.key())            // It does NOT contains
                || (keys.contains(it.key())     // It contains BUT with different value
                    && it.value() != otherEmbObjProperties.m_props->value(it.key())))
            differentProps->insert(it.key(), it.value());
    }

    return differentProps;
}

QString MctEmbObjProperties::createInnerURL(const QString &graphicURL)
{
    return convertURL(MctStaticData::GRAPHICOBJ, MctStaticData::GRAPHICOBJ_PATH, graphicURL);
}

QString MctEmbObjProperties::createGraphicURL(const QString &innerURL)
{
    return convertURL(MctStaticData::GRAPHICOBJ_PATH, MctStaticData::GRAPHICOBJ, innerURL);
}

