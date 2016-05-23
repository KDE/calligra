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
    MctEmbObjProperties(QString name, KoShape *m_shape);
    MctEmbObjProperties(QString name, KoShape *m_shape, QPointF m_pos, QSizeF m_size);
    ~MctEmbObjProperties();

    QString backupFileFromOdt(QString m_innerURL, QString absOdtURL);    // Makes a copy of a metadata stored in the Odt.
    void restoreBackupFile();    // Makes a copy of a metadata stored in the Odt. The Manifest file is also modified in order to get valid odt.
    void deleteBackupFile();
    void mediaType(QString fileName);    // Gets media type of the embedded object in the manifest.xml
    void regManifest(QString fileName);     // Registers the embedded filenames in the manifest.xml
    PropertyDictionary * compareProperties(const MctEmbObjProperties& otherEmbObjProperties); // Compares properties with properties of another embObjProps class
    QString createInnerURL(QString graphicURL); // Creates inner URL in the Odt for embedded object.
    QString createGraphicURL(QString m_innerURL); // Creates graphic URL in the Odt for embedded object.

    void setOdtUrl(QString odt);

    bool isBackedup();

    QString url() const;
    void setURL(const QString &value);

    QString odtURL() const;
    void setOdtURL(const QString &value);

    bool isBackedUp() const;
    void setIsBackedUp(bool value);

    bool wasInserted() const;
    void setWasInserted(bool value);

    bool doRestoreWhenDeleted() const;
    void setDoRestoreWhenDeleted(bool value);

    QPointF pos() const;
    QPointF centerPos() const;

    void setPositionInExport();

    virtual void fillUpProperties();
    void addStrokeStyleChanges(KoShapeStroke *newStroke);
    void addShadowStyleChanges(KoShapeShadow *newShadow);
    void addSizeChanged(QSizeF prevPos);
    void addRotationChanged(double rotation);
    KoShape* shape();

    void setPrevPos(QPointF prevPos);
    void textGraphicStyleChanges(MctEmbObjProperties* props2, KoShape* m_shape);
    bool removeDir(const QString & dirName);

private:
    QString m_url;      // unused    // URL of the backup file
    QString m_odtURL;   // unused    // URL of the Odt file
    QString m_mediaType;// unused          // media type of the embedded object in the manifest file
    bool m_isBackedUp;  // unused           // True if the embedded object is backed up
    bool m_wasInserted; // u      // True if the object was inserted
    bool m_doRestoreWhenDeleted;  // backed up file is restored into the Odt when True
    KoShape *m_shape;   // u
    QPointF m_pos;      // u
    QSizeF m_size;      // u

    QString convertURL(QString oldPrefix, QString newPrefix, QString name);
};

#endif // MCTEMBOBJPROPERTIES_H
