/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoTextShapeSavingContext.h"

#include <KoGenChanges.h>

#include "TextDebug.h"

KoTextShapeSavingContext::KoTextShapeSavingContext(KoXmlWriter &xmlWriter,
                                                   KoGenStyles &mainStyles,
                                                   KoEmbeddedDocumentSaver &embeddedSaver,
                                                   KoGenChanges &changes)
    : KoShapeSavingContext(xmlWriter, mainStyles, embeddedSaver)
    , m_changes(changes)
{
}

KoTextShapeSavingContext::~KoTextShapeSavingContext() = default;

KoGenChanges &KoTextShapeSavingContext::changes()
{
    return m_changes;
}
