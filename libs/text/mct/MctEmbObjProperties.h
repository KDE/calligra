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

/**
 * Represent an embedded object with properties
 *
 * @note this class was also designed for UNO interface
 * @todo refactoring, re-thinking is suggested
 */
class MctEmbObjProperties : public MctPropertyBase
{
public:
    MctEmbObjProperties(const QString &name);
    MctEmbObjProperties(const QString &name, KoShape *m_shape);
    MctEmbObjProperties(const QString &name, KoShape *m_shape, const QPointF &m_pos, const QSizeF &m_size);
    ~MctEmbObjProperties();

    /**
     * export odt content for backup
     *
     * make a copy of a metadata stored in the Odt.
     * @param innerURL relative URL in the Odt.
     * @param OdtURL absolute URL of the Odt.
     * @return the URL of the backed up file
     */
    QString backupFileFromOdt(const QString &m_innerURL, const QString &absOdtURL);

    /// restore embedded file from backup (if it exists)
    void restoreBackupFile();

    /// cleans up backup files.
    void deleteBackupFile();

    /**
     * update mediatype member
     *
     * update the value of this->mediaType member from the given manifest file
     * @param fileName manifest file (usually .../odtURL/manifest.xml)
     */
    void mediaType(const QString &fileName);

    /**
     * register manifest file
     *
     * @param fileName manifest file
     */
    void regManifest(const QString &fileName);

    /**
     * compare two embedded object property dictionary
     *
     * Gives a a dictionary about the not identical elements
     * @param otherEmbObjProperties dictionary that should contains all element of this object's props dictionary.
     * @return new dictionary which conatins the differences.
     */
    PropertyDictionary * compareProperties(const MctEmbObjProperties& otherEmbObjProperties);

    /**
     * transform graphicURL to innerURL
     *
     * @param graphicURL should start with someting like this: "nd.sun.star.GraphicObject:"identifier
     * @return The innerURL that looks like this: "Picture/"identifier
     * @todo probably legacy code
     */
    QString createInnerURL(const QString &graphicURL);

    /**
     * transforms innerURL into grapicURL
     *
     * @param innerURL example: "Picture/"identifier
     * @return example: "nd.sun.star.GraphicObject:"identifier
     * @todo probably legacy code
     */
    QString createGraphicURL(const QString &m_innerURL);

    /// setter
    void setOdtUrl(const QString &odt);
    /// getter
    QString url() const;
    /// setter
    void setURL(const QString &value);
    /// getter
    QString odtURL() const;
    /// setter
    void setOdtURL(const QString &value);
    /// getter
    bool isBackedUp() const;
    /// setter
    void setIsBackedUp(bool value);
    /// getter
    bool wasInserted() const;
    /// setter
    void setWasInserted(bool value);
    /// getter
    bool doRestoreWhenDeleted() const;
    /// setter
    void setDoRestoreWhenDeleted(bool value);
    /// getter
    KoShape* shape();
    /// getter
    QPointF pos() const;
    /// get center position (pos+dim_size/2)
    QPointF centerPos() const;

    /// add postion properties to property dictionary
    void setPositionInExport();

    virtual void fillUpProperties();

    /// insert stroke related properties
    void addStrokeStyleChanges(KoShapeStroke *newStroke);

    /// insert shadow related properties
    void addShadowStyleChanges(KoShapeShadow *newShadow);

    /// insert PrevSize properties
    void addSizeChanged(const QSizeF &prevPos);

    /// insert PrevRotation property
    void addRotationChanged(double rotation);

    /// insert PreviousPosition properties
    void setPrevPos(const QPointF &prevPos);

    /**
     * set graphical style proprties of the parameter from the property dictionary
     *
     * @param props2 @todo maybe remove and use this pointer instead?
     * @param m_shape grpahical object to style
     */
    void textGraphicStyleChanges(MctEmbObjProperties* props2, KoShape* m_shape);

    /// utility to delete a folder @todo redundant function maybe
    bool removeDir(const QString & dirName);

private:
    QString m_url;          /// URL of the backup file
    QString m_odtURL;       /// URL of the Odt file
    QString m_mediaType;    /// media type of the embedded object in the manifest file
    bool m_isBackedUp;      ///< true if the embedded object is backed up
    bool m_wasInserted;     ///< true if the object was inserted
    bool m_doRestoreWhenDeleted;  ///< backed up file is restored into the Odt when True
    KoShape *m_shape;       ///< describe formatting of document element
    QPointF m_pos;          ///< position (x, y)
    QSizeF m_size;          ///< size (width, height)

    /**
     * utility method to swap the prefix of URL
     *
     * @param oldPrefix URL should start with this.
     * @param newPrefix URL should start with this.
     * @param name original identifier that should contain oldPrefix.
     * @return identifier starting with the newPrefix substring.
     */
    QString convertURL(const QString &oldPrefix, const QString &newPrefix, const QString &name);
};

#endif // MCTEMBOBJPROPERTIES_H
