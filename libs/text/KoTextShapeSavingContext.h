/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOTEXTSHAPESAVINGCONTEXT_H
#define KOTEXTSHAPESAVINGCONTEXT_H

#include "kotext_export.h"

#include <KoShapeSavingContext.h>

class KoGenChanges;

/**
 * The set of data for the ODF file format used during saving of a shape.
 */
class KOTEXT_EXPORT KoTextShapeSavingContext : public KoShapeSavingContext
{
public:
    /**
     * @brief Constructor
     * @param xmlWriter used for writing the xml
     * @param mainStyles for saving the styles
     * @param embeddedSaver for saving embedded documents
     * @param changes for saving the tracked changes
     */
    KoTextShapeSavingContext(KoXmlWriter &xmlWriter, KoGenStyles &mainStyles, KoEmbeddedDocumentSaver &embeddedSaver, KoGenChanges &changes);
    ~KoTextShapeSavingContext() override;

    /**
     * @brief Get the changes (tracked)
     *
     * @return changes (tracked)
     */
    KoGenChanges &changes();

private:
    KoGenChanges &m_changes;
};

#endif // KOTEXTSHAPESAVINGCONTEXT_H
