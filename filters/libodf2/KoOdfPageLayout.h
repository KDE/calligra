/* This file is part of the KDE project
 *
 * Copyright (C) 2014 Inge Wallin <inge@lysator.liu.se>
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


#ifndef KOODF_PAGE_LAYOUT_H
#define KOODF_PAGE_LAYOUT_H

#include "KoOdfStyleBase.h"
#include "koodf2_export.h"

#include <QHash>

class QString;
class KoXmlStreamReader;
class KoOdfPageLayoutProperties;
class KoOdfHeaderFooterProperties;
class KoXmlWriter;


class KOODF2_EXPORT KoOdfPageLayout : public KoOdfStyleBase
{
 public:
    KoOdfPageLayout();
    ~KoOdfPageLayout() override;

    QString pageUsage() const;
    void setPageUsage(const QString &family);

    /**
     * @brief Return the page layout properties of this page layout style.
     */
    KoOdfPageLayoutProperties *pageLayoutProperties() const;

    /**
     * @brief Return the header properties of this page layout style.
     */
    KoOdfHeaderFooterProperties *headerProperties() const;
    /**
     * @brief Return the footer properties of this page layout style.
     */
    KoOdfHeaderFooterProperties *footerProperties() const;

    // Reimplemented from KoOdfStyleBase
    bool readOdf(KoXmlStreamReader &reader) override;
    bool saveOdf(KoXmlWriter *writer) override;

 private:
    class Private;
    Private * const d;
};


#endif
