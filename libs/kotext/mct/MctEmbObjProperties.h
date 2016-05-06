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

#ifndef MCTEMBOBJPROPERTIES_H
#define MCTEMBOBJPROPERTIES_H

#include "MctPropertyBase.h"
#include "KoShape.h"
#include "KoShapeStroke.h"
#include "KoShapeShadow.h"

#include <QSize>

// Embedded object properties class
class MctEmbObjProperties : public MctPropertyBase
{
public:
    MctEmbObjProperties(QString name);
    MctEmbObjProperties(QString name, KoShape *shape);
    MctEmbObjProperties(QString name, KoShape *shape, QPointF pos, QSizeF size);
    ~MctEmbObjProperties();

    QString backupFileFromOdt(QString innerURL, QString absOdtURL);    // Makes a copy of a metadata stored in the Odt.
    void restoreBackupFile();    // Makes a copy of a metadata stored in the Odt. The Manifest file is also modified in order to get valid odt.
    void deleteBackupFile();
    void getMediaType(QString fileName);    // Gets media type of the embedded object in the manifest.xml
    void regManifest(QString fileName);     // Registers the embedded filenames in the manifest.xml
    PropertyDictionary * compareProperties(const MctEmbObjProperties& otherEmbObjProperties); // Compares properties with properties of another embObjProps class
    QString createInnerURL(QString graphicURL); // Creates inner URL in the Odt for embedded object.
    QString createGraphicURL(QString innerURL); // Creates graphic URL in the Odt for embedded object.

    void setOdtUrl(QString odt);

    bool isBackedup();

    QString getURL() const;
    void setURL(const QString &value);

    QString getOdtURL() const;
    void setOdtURL(const QString &value);

    bool getIsBackedUp() const;
    void setIsBackedUp(bool value);

    bool getWasInserted() const;
    void setWasInserted(bool value);

    bool getDoRestoreWhenDeleted() const;
    void setDoRestoreWhenDeleted(bool value);

    QPointF getPos() const;
    QPointF getCenterPos() const;

    void setPositionInExport();

    virtual void fillUpProperties();
    void addStrokeStyleChanges(KoShapeStroke *newStroke);
    void addShadowStyleChanges(KoShapeShadow *newShadow);
    void addSizeChanged(QSizeF prevPos);
    void addRotationChanged(double rotation);
    KoShape* getShape();

    void setPrevPos(QPointF prevPos);
    void textGraphicStyleChanges(MctEmbObjProperties* props2, KoShape* shape);
    bool removeDir(const QString & dirName);

private:
    QString URL;                // URL of the backup file
    QString odtURL;             // URL of the Odt file
    QString mediaType;          // media type of the embedded object in the manifest file
    bool isBackedUp;            // True if the embedded object is backed up
    bool wasInserted;           // True if the object was inserted
    bool doRestoreWhenDeleted;  // backed up file is restored into the Odt when True
    KoShape *shape;
    QPointF pos;
    QSizeF size;

    QString convertURL(QString oldPrefix, QString newPrefix, QString name);
};

#endif // MCTEMBOBJPROPERTIES_H
