/* This file is part of the KDE project
   Copyright (C) 2004-2006 David Faure <faure@kde.org>
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>

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

#include <KoElementReference.h>

class KoXmlWriter;
class KoGenStyles;
class KoEmbeddedDocumentSaver;
class KoStore;

class KoOdfSavingContextPrivate;


/**
 * A set of data for the ODF file format usable during saving..
 */
class KOODF_EXPORT KoOdfSavingContext
{
public:
    /**
     * @brief Constructor
     * @param xmlWriter used for writing the xml
     * @param mainStyles for saving the styles
     * @param embeddedSaver for saving embedded documents
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
     * @brief Get the embedded document saver
     *
     * @return embedded document saver
     */
    KoEmbeddedDocumentSaver &embeddedSaver();


    /**
     * @brief xmlid returns an element reference that can be related
     *   to the given referent. If there is a prefix given, this
     *   prefix will be used in addition to either the counter or the
     *   uuid.
     *
     * @param referent the object we are referring to
     * @param prefix a prefix for the xml:id string
     * @param counter if counter is true, shapesavingcontext will use a counter to create the xml:id
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


private:
    KoOdfSavingContextPrivate *d;
};


#endif // KOODFSAVINGCONTEXT_H
