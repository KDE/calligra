/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAPAGE_H
#define KOPAPAGE_H

#include "KoPAPageBase.h"

class KoPAMasterPage;

/// Class representing a page
class KOPAGEAPP_EXPORT KoPAPage : public KoPAPageBase
{
public:
    /** Constructor
     * @param masterPage masterpage used for this page
     */
    explicit KoPAPage(KoPAMasterPage *masterPage);
    ~KoPAPage() override;

    /// reimplemented
    void saveOdf(KoShapeSavingContext &context) const override;

    /// @return the layout set by the masterpage
    KoPageLayout &pageLayout() override;
    const KoPageLayout &pageLayout() const override;

    /// Set the masterpage for this page to @p masterPage
    void setMasterPage(KoPAMasterPage *masterPage);
    /// @return the masterpage of this page
    KoPAMasterPage *masterPage()
    {
        return m_masterPage;
    }

    /// reimplemented
    void paintBackground(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext) override;

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
    void paintPage(QPainter &painter, KoZoomHandler &zoomHandler) override;

protected:
    /**
     * DisplayMasterBackground and DisplayMasterShapes are only saved loaded in a presentation
     * They are however implemented here to reduce code duplication.
     */
    enum PageProperty {
        UseMasterBackground = 1, ///< Use the background of the master page. See ODF 14.13.2 Drawing Page Style
        DisplayMasterBackground = 2, ///< If the master page is used this indicated if its background should be used. See ODF 15.36.13 Background Visible
        DisplayMasterShapes = 4, ///< Set if the shapes of the master page should be shown. See ODF 15.36.12 Background Objects Visible
        DisplayHeader = 8, /// set if presentation:display-header is true
        DisplayFooter = 16, /// set if presentation:display-footer is true
        DisplayPageNumber = 32, /// set if presentation:display-page-number is true
        DisplayDateTime = 64 /// set if presentation:display-date-time is true
    };

    /// Reimplemented from KoPageBase
    void loadOdfPageTag(const KoXmlElement &element, KoPALoadingContext &loadingContext) override;

    /// Reimplemented from KoPageBase
    void saveOdfPageStyleData(KoGenStyle &style, KoPASavingContext &paContext) const override;

    KoPAMasterPage *m_masterPage;

    int m_pageProperties;
};

#endif /* KOPAPAGE_H */
