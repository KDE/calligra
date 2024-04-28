/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Pierre Stirnweiss <pierre.stirnweiss_calligra@gadz.org>
   SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOGENCHANGES_H
#define KOGENCHANGES_H

#include "koodf_export.h"

#include <KoGenChange.h>

class KoXmlWriter;

/**
 * @brief Repository of changes used during saving of OASIS/OOo file.
 *
 * Inspired from KoGenStyles.h
 *
 * Is used to store all the change regions, which will be saved at the beginning of <office:body><office:text> elements
 * We use a container outside the changeTracker, as the change tracker is linked to the document of a TextShapeData and is then not aware of the other
 * TextShapeData.
 *
 */
class KOODF_EXPORT KoGenChanges
{
public:
    KoGenChanges();
    ~KoGenChanges();

    /**
     * Look up a change in the collection, inserting it if necessary. If the change already
     * exists, return the existing name. If not, assign a name to the change and returns it.
     *
     * @param change the change to look up.
     * @param name proposed internal name for the change. It will be modified to be guaranteed unique.
     * @return the name for this change
     */
    QString insert(const KoGenChange &change);

    /**
     * Save changes.
     *
     * This creates the text:changed-region tag containing all
     * changes.
     *
     * @param xmlWriter
     * @param stylesDotXml
     */
    void saveOdfChanges(KoXmlWriter *xmlWriter, bool trackChanges) const;

private:
    class Private;
    Private *const d;
};

#endif /* KOGENCHANGES_H */
