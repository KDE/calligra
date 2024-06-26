/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAPAGEBASE_H
#define KOPAPAGEBASE_H

#include <KoShapeContainer.h>

#include "KoPageApp.h"
#include "kopageapp_export.h"

#include <QSize>

#define CACHE_PAGE_THUMBNAILS

struct KoPageLayout;
class KoGenStyle;
class KoPALoadingContext;
class KoShapeManagerPaintingStrategy;
class KoZoomHandler;
class KoPASavingContext;

class QPixmap;
class QString;

/**
 * Base class used for KoPAMasterPage and KoPAPage
 *
 * A Page contains KoShapeLayer shapes as direct children. The layers than can
 * contain all the different shapes.
 */
class KOPAGEAPP_EXPORT KoPAPageBase : public KoShapeContainer
{
public:
    explicit KoPAPageBase();
    ~KoPAPageBase() override;

    /**
     * @brief Save a page
     *
     * See ODF 9.1.4 Drawing Pages
     *
     * @param context the pageapp saving context
     * @return true on success, false otherwise
     */
    void saveOdf(KoShapeSavingContext &context) const override = 0;

    /// reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /// @return the layout of the page
    virtual KoPageLayout &pageLayout() = 0;
    virtual const KoPageLayout &pageLayout() const = 0;

    void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;

    /**
     * @brief Paint background
     *
     * @param painter used to paint the background
     * @param converter to convert between internal and view coordinates
     * @param paintContext the painting context
     */
    virtual void paintBackground(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext);

    /**
     * Get if master shapes should be displayed
     *
     * For master pages this always returns false
     *
     * @return true if master shapes should be displayed
     */
    virtual bool displayMasterShapes() = 0;

    /**
     * Set if the master shapes should be displayed
     *
     * For master pages this does nothing
     */
    virtual void setDisplayMasterShapes(bool display) = 0;

    /**
     * Get if master page background should be used
     *
     * For master pages this always returns false
     *
     * @return true if master page background should be used
     */
    virtual bool displayMasterBackground() = 0;

    virtual void setDisplayMasterBackground(bool display) = 0;

    /**
     * Get if the shape should be displayed or not
     *
     * This is used for hiding special objects e.g. presentation:display-page-number="false"
     *
     * @param shape for which to check if it should be shown or not.
     * @return true if the shape should be shown, otherwise false.
     */
    virtual bool displayShape(KoShape *shape) const = 0;

    virtual QPixmap thumbnail(const QSize &size = QSize(512, 512));

    virtual QImage thumbImage(const QSize &size = QSize(512, 512));

    /**
     * This function is called when the content of the page changes
     *
     * It invalidates the pages thumbnail cache.
     */
    virtual void pageUpdated();

    /// reimplemented
    QSizeF size() const override;

    // reimplemented
    QRectF boundingRect() const override;

    /**
     * Returns the bounding rectangle of the pages content
     */
    virtual QRectF contentRect() const;

    /**
     * This function is called after a shape is added to the document on this page
     * The default implementation is empty.
     *
     * @param shape The shape that was added
     */
    virtual void shapeAdded(KoShape *shape);

    /**
     * This function is called after a shape is removed from the document off this page
     * The default implementation is empty.
     *
     * @param shape The shape that was removed
     */
    virtual void shapeRemoved(KoShape *shape);

    /**
     * Get the page type used in the document
     *
     * The default page type KoPageApp::Page is returned
     */
    virtual KoPageApp::PageType pageType() const;

    /**
     * Paint to content of the page to the painter
     *
     * @param painter The painter used to paint the page
     * @param zoomHandler The zoomHandler used to paint the page
     */
    virtual void paintPage(QPainter &painter, KoZoomHandler &zoomHandler) = 0;

protected:
    /**
     * @param paContext the pageapp saving context
     */
    virtual void saveOdfPageContent(KoPASavingContext &paContext) const;

    /**
     * @brief Save the layers of a page
     */
    void saveOdfLayers(KoPASavingContext &paContext) const;

    /**
     * @brief Save the shapes of a page
     *
     * See ODF 9.2 Drawing Shapes
     *
     * @param context the pageapp saving context
     * @return true on success, false otherwise
     */
    void saveOdfShapes(KoShapeSavingContext &context) const;

    /**
     * @brief Save animations
     *
     * Here is a empty implementation as not all page apps need animations.
     *
     * @param paContext the pageapp saving context
     * @return true on success, false otherwise
     */
    virtual bool saveOdfAnimations(KoPASavingContext &paContext) const;

    /**
     * @brief Save presentation notes
     *
     * Here is a empty implementation as not all page apps presentations notes.
     *
     * @param paContext the pageapp saving context
     * @return true on success, false otherwise
     */
    virtual bool saveOdfPresentationNotes(KoPASavingContext &paContext) const;

    /**
     * @brief Save the style of the page
     *
     * See ODF 14.13.2 Drawing Page Style
     *
     * @return name of the page style
     */
    QString saveOdfPageStyle(KoPASavingContext &paContext) const;

    /**
     * @brief Save special data of a style
     *
     * @param style the page style
     * @param paContext the pageapp saving context
     *
     * @see saveOdfPageStyle
     */
    virtual void saveOdfPageStyleData(KoGenStyle &style, KoPASavingContext &paContext) const;

    /**
     * @brief Load page data
     *
     * @param element the page element
     * @param loadingContext the pageapp loading context
     */
    virtual void loadOdfPageTag(const KoXmlElement &element, KoPALoadingContext &loadingContext);

    /**
     * @brief Load extra page data
     *
     * This method gets called after all shapes of the page are loaded.
     * The default implementation is empty
     *
     * @param element the page element
     * @param loadingContext the pageapp loading context
     */
    virtual void loadOdfPageExtra(const KoXmlElement &element, KoPALoadingContext &loadingContext);

    /**
     * Create thumbnail for the page
     */
    QPixmap generateThumbnail(const QSize &size = QSize(512, 512));

    /**
     * Get the key used for caching the thumbnail pixmap
     */
    QString thumbnailKey() const;

    /**
     * Get the painting strategy used for generating thumbnails
     *
     * The returned strategy needs to be alloced by new
     *
     * @return 0 which mean use the default strategy
     */
    virtual KoShapeManagerPaintingStrategy *getPaintingStrategy() const;
};

#endif /* KOPAPAGEBASE_H */
