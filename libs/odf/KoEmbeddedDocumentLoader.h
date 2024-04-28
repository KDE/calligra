/* This file is part of the KDE project
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    Private *const d;
    Q_DISABLE_COPY(KoEmbeddedDocumentLoader)
};

#endif /* KOEMBEDDEDDOCUMENTLOADER_H */
