/* This file is part of the KDE project
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

#ifndef KOEMBEDDEDDOCUMENTLOADER_H
#define KOEMBEDDEDDOCUMENTLOADER_H

#include "koodf_export.h"

#include <QtGlobal>

class KoDocumentBase;
class KoXmlElement;
class KoOdfLoadingContext;

/**
 * This class is used to load embedded objects in ODF documents.
 *
 */
class KOODF_EXPORT KoEmbeddedDocumentLoader
{
public:
    /// Creator
    KoEmbeddedDocumentLoader(KoDocumentBase *doc);
    /// Destructor
    ~KoEmbeddedDocumentLoader();

    /// Load the embedded document linked to in @p element from the store in @p context
    bool loadEmbeddedDocument(const KoXmlElement &element, KoOdfLoadingContext &context);

private:
    class Private;
    Private * const d;
    Q_DISABLE_COPY(KoEmbeddedDocumentLoader)
};

#endif /* KOEMBEDDEDDOCUMENTLOADER_H */
