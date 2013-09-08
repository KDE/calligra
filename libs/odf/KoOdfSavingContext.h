/* This file is part of the KDE project
   Copyright (C) 2004-2006 David Faure <faure@kde.org>
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2013 inge Wallin <inge@lysator.liu.se>

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

#ifndef KOODFSAVINGCONTEXT_H
#define KOODFSAVINGCONTEXT_H

#include "koodf_export.h"

#include <QImage>
#include <QTransform>
#include <QTextBlockUserData>
#include <KoElementReference.h>

class KoXmlWriter;
class KoGenStyles;
//class KoDataCenterBase;
class KoEmbeddedDocumentSaver;
//class KoImageData;
//class KoMarker;
class KoStore;

/**
 * The set of data for the ODF file format used during saving of an ODF document.
 */
class KOODF_EXPORT KoOdfSavingContext
{
public:

    /**
     * @brief Constructor
     * @param xmlWriter used for writing the xml
     * @param mainStyles for saving the styles
     * @param embeddedSaver for saving embedded files and documents
     */
    KoOdfSavingContext(KoXmlWriter &xmlWriter, KoGenStyles &mainStyles,
                       KoEmbeddedDocumentSaver &embeddedSaver);
    virtual ~KoOdfSavingContext();

    /**
     * @brief Get the xml writer
     *
     * @return xmlWriter
     */
    KoXmlWriter &xmlWriter();

    /**
     * @brief Set the xml writer
     *
     * Change the xmlWriter that is used in the Context e.g. for saving to styles.xml
     * instead of content.xml
     *
     * @param xmlWriter to use
     */
    void setXmlWriter(KoXmlWriter &xmlWriter);

    /**
     * @brief Get the main styles
     *
     * @return main styles
     */
    KoGenStyles &mainStyles();

    /**
     * @brief Get the embedded file saver
     *
     * @return embedded document saver
     */
    KoEmbeddedDocumentSaver &embeddedSaver();


    /**
     * @brief xmlid returns an element reference that can be related to the given referent. If there is a
     *   prefix given, this prefix will be used in addition to either the counter or the uuid.
     * @param referent the object we are referring to
     * @param prefix a prefix for the xml:id string
     * @param counter if counter is true, odfsavingcontext will use a counter to create the xml:id
     * @return a KoElementReference; if insert is false and referent doesn't exist yet in the list, the elementrefence will be invalid.
     */
    KoElementReference xmlid(const void *referent, const QString& prefix = QString(),
                             KoElementReference::GenerationOption counter = KoElementReference::UUID);

    /**
     * @brief existingXmlid retrieve an existing xml id or invalid xml id if the referent object doesn't exist
     */
    KoElementReference existingXmlid(const void *referent);

    /**
     * @brief Clear out all given draw ids
     * @param prefix: removes all xml:id's that have the given prefix.
     *
     * This is needed for checking if master pages are the same. In normal saving
     * this should not be called.
     *
     * @see KoPAPastePage::process
     */
    void clearXmlIds(const QString &prefix);

#if 0  // FIXME: Move image data do libs/odf
    /**
     * Get the image href under which the image will be saved in the store
     */
    QString imageHref(const KoImageData *image);

    /**
     * Get the image href under which the image will be saved in the store
     *
     * This should only be used for temporary images that are onle there during
     * saving, e.g. a pixmap representation of a draw:frame
     */
    QString imageHref(const QImage &image);

    /**
     * Get the images that needs to be saved to the store
     */
    QMap<qint64, QString> imagesToSave();
#endif
#if 0
    /**
     * Get the reference to use for the marker lookup
     */
    QString markerRef(const KoMarker *marker);
#endif
#if 0 // FIXME: Move datacenter to libs/odf and call it KoCollection
    /**
     * Add data center
     */
    void addDataCenter(KoDataCenterBase *dataCenter);

    /**
     * Save the data centers
     *
     * This calls KoDataCenterBase::completeSaving()
     * @returns false if an error occurred, which typically cancels the save.
     */
    bool saveDataCenter(KoStore *store, KoXmlWriter *manifestWriter);
#endif

private:
    class Private;
    Private * const d;
};

#endif // KOODFSAVINGCONTEXT_H
