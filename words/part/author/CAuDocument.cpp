/* This file is part of the KDE project
 * Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
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

#include "CAuDocument.h"
#include <KoOdfWriteStore.h>
#include <author/CoverImage.h>

CAuDocument::CAuDocument(KoPart *part)
    : KWDocument(part)
#ifdef SHOULD_BUILD_RDF
    , m_metaManager(new CAuMetaDataManager(this))
#endif
{
}

void CAuDocument::setCoverImage(QPair<QString, QByteArray> cover)
{
    m_coverImage = cover;
}

QPair<QString, QByteArray> CAuDocument::coverImage()
{
    return m_coverImage;
}

bool CAuDocument::saveOdf(SavingContext &documentContext)
{
    bool result = KWDocument::saveOdf(documentContext);
    if (!result) {
        return false;
    }

    // save cover
    CoverImage coverImage;
    if (!coverImage.saveCoverImage(documentContext.odfStore.store(), documentContext.odfStore.manifestWriter(), m_coverImage)) {
        return false;
    }
    return true;
}

#ifdef SHOULD_BUILD_RDF
CAuMetaDataManager *CAuDocument::metaManager() const
{
    return m_metaManager;
}
#endif
