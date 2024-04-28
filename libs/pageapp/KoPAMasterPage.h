/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAMASTERPAGE_H
#define KOPAMASTERPAGE_H

#include "KoPAPageBase.h"

#include <KoPageLayout.h>

/// Stores the masterpage's shapes and settings
class KOPAGEAPP_EXPORT KoPAMasterPage : public KoPAPageBase
{
public:
    explicit KoPAMasterPage();
    ~KoPAMasterPage() override;

    /// reimplemented
    void saveOdf(KoShapeSavingContext &context) const override;

    /// @return the page layout set for this masterpage
    KoPageLayout &pageLayout() override
    {
        return m_pageLayout;
    }
    const KoPageLayout &pageLayout() const override
    {
        return m_pageLayout;
    }

    /// Set the page layout to @p layout
    void setPageLayout(const KoPageLayout &layout)
    {
        m_pageLayout = layout;
    }

    /// reimplemented
    bool displayMasterShapes() override;

    /// reimplemented
    void setDisplayMasterShapes(bool display) override;

    /// reimplemented
    bool displayMasterBackground() override;

    /// reimplemented
    void setDisplayMasterBackground(bool display) override;

    /// reimplemented
    bool displayShape(KoShape *shape) const override;

    /// reimplemented
    void pageUpdated() override;

    /// reimplemented
    void paintPage(QPainter &painter, KoZoomHandler &zoomHandler) override;

protected:
    /// Reimplemented from KoPageBase
    void loadOdfPageTag(const KoXmlElement &element, KoPALoadingContext &loadingContext) override;

    KoPageLayout m_pageLayout;
};

#endif /* KOPAMASTERPAGE_H */
