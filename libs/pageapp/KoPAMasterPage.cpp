/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPAMasterPage.h"

#include <QBuffer>
#include <QPainter>

#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoShapePainter.h>
#include <KoShapePaintingContext.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoZoomHandler.h>

#include "KoPALoadingContext.h"
#include "KoPAPixmapCache.h"
#include "KoPASavingContext.h"

KoPAMasterPage::KoPAMasterPage()
    : KoPAPageBase()
{
    setName("Standard");
}

KoPAMasterPage::~KoPAMasterPage() = default;

void KoPAMasterPage::saveOdf(KoShapeSavingContext &context) const
{
    KoPASavingContext &paContext = static_cast<KoPASavingContext &>(context);

    KoGenStyle pageLayoutStyle = pageLayout().saveOdf();
    pageLayoutStyle.setAutoStyleInStylesDotXml(true);
    pageLayoutStyle.addAttribute("style:page-usage", "all");
    QString pageLayoutName(paContext.mainStyles().insert(pageLayoutStyle, "pm"));

    KoGenStyle pageMaster(KoGenStyle::MasterPageStyle);
    pageMaster.addAttribute("style:page-layout-name", pageLayoutName);
    pageMaster.addAttribute("style:display-name", name());
    pageMaster.addAttribute("draw:style-name", saveOdfPageStyle(paContext));

    KoXmlWriter &savedWriter = paContext.xmlWriter();

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);

    paContext.setXmlWriter(xmlWriter);

    saveOdfPageContent(paContext);

    paContext.setXmlWriter(savedWriter);

    QString contentElement = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    pageMaster.addChildElement(paContext.masterPageElementName(), contentElement);
    paContext.addMasterPage(this, paContext.mainStyles().insert(pageMaster, "Default"));
}

void KoPAMasterPage::loadOdfPageTag(const KoXmlElement &element, KoPALoadingContext &loadingContext)
{
    KoPAPageBase::loadOdfPageTag(element, loadingContext);
    if (element.hasAttributeNS(KoXmlNS::style, "display-name")) {
        setName(element.attributeNS(KoXmlNS::style, "display-name"));
    } else {
        setName(element.attributeNS(KoXmlNS::style, "name"));
    }
    QString pageLayoutName = element.attributeNS(KoXmlNS::style, "page-layout-name");
    const KoOdfStylesReader &styles = loadingContext.odfLoadingContext().stylesReader();
    const KoXmlElement *masterPageStyle = styles.findStyle(pageLayoutName);
    KoPageLayout pageLayout;

    if (masterPageStyle) {
        pageLayout.loadOdf(*masterPageStyle);
    }

    setPageLayout(pageLayout);
}

bool KoPAMasterPage::displayMasterShapes()
{
    return false;
}

void KoPAMasterPage::setDisplayMasterShapes(bool display)
{
    Q_UNUSED(display);
}

bool KoPAMasterPage::displayMasterBackground()
{
    return false;
}

void KoPAMasterPage::setDisplayMasterBackground(bool display)
{
    Q_UNUSED(display);
}

bool KoPAMasterPage::displayShape(KoShape *shape) const
{
    Q_UNUSED(shape);
    return true;
}

void KoPAMasterPage::pageUpdated()
{
    KoPAPageBase::pageUpdated();
    // TODO that is not the best way as it removes to much from the cache
    KoPAPixmapCache::instance()->clear(false);
}

void KoPAMasterPage::paintPage(QPainter &painter, KoZoomHandler &zoomHandler)
{
    KoShapePaintingContext context;
    paintBackground(painter, zoomHandler, context);

    KoShapePainter shapePainter;
    shapePainter.setShapes(shapes());
    shapePainter.paint(painter, zoomHandler);
}
