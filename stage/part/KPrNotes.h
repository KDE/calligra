/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2008-2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRNOTES_H
#define KPRNOTES_H

#include <KoPAPageBase.h>
#include <KoPageLayout.h>

class KoShape;
class KoImageCollection;
class KPrDocument;
class KPrPage;

class KPrNotes : public KoPAPageBase
{
public:
    KPrNotes(KPrPage *page, KPrDocument *document);
    ~KPrNotes() override;

    /// Get the main text note shape for this presentation notes
    KoShape *textShape();

    /// reimplemented
    void saveOdf(KoShapeSavingContext &context) const override;

    /// reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /// reimplemented
    void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;

    /// Get the page layout for this presentation notes
    KoPageLayout &pageLayout() override;

    const KoPageLayout &pageLayout() const override;

    /// update the page thumbnail to reflect current page
    void updatePageThumbnail();

    /// reimplemented
    bool displayMasterShapes() override;
    /// reimplemented
    void setDisplayMasterShapes(bool) override;
    /// reimplemented
    bool displayMasterBackground() override;
    /// reimplemented
    void setDisplayMasterBackground(bool) override;
    /// reimplemented
    bool displayShape(KoShape *shape) const override;

    QImage thumbImage(const QSize & = QSize(512, 512)) override;
    /// reimplemented
    virtual QPixmap generateThumbnail(const QSize &);
    /// reimplemented
    void paintPage(QPainter &painter, KoZoomHandler &zoomHandler) override;

private:
    KoShape *m_textShape;
    KoShape *m_thumbnailShape;
    KoPageLayout m_pageLayout;
    KPrPage *m_page;
    KPrDocument *m_doc;
    KoImageCollection *m_imageCollection;
};

#endif // KPRNOTES_H
