/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2014 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    Private *const d;
};

#endif
