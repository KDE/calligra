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

#ifndef CAUDOCUMENT_H
#define CAUDOCUMENT_H

#include <KWDocument.h>
#include <KoPart.h>

#ifdef SHOULD_BUILD_RDF
#include "metadata/CAuMetaDataManager.h"
#endif

class CAuDocument : public KWDocument
{
    Q_OBJECT
public:
    explicit CAuDocument(KoPart *part);

    /// Set cover image data at a QPair<cover mime type, cover data>.
    void setCoverImage(QPair<QString, QByteArray> cover);

    /// return cover data.
    QPair<QString, QByteArray> coverImage();

    /// reimplemented to save cover
    virtual bool saveOdf(SavingContext &documentContext);

#ifdef SHOULD_BUILD_RDF
    CAuMetaDataManager *metaManager() const;
#endif

private:
    QPair<QString, QByteArray> m_coverImage;
#ifdef SHOULD_BUILD_RDF
    CAuMetaDataManager *m_metaManager;
#endif
};

#endif //CAUDOCUMENT_H
