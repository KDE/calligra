/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPAUtil.h"

#include <QRect>
#include <QSize>

#include <KoPageLayout.h>
#include <KoZoomHandler.h>

void KoPAUtil::setZoom(const KoPageLayout &pageLayout, const QSizeF &size, KoZoomHandler &zoomHandler)
{
    qreal zoom = size.width() / (zoomHandler.resolutionX() * pageLayout.width);
    zoom = qMin(zoom, size.height() / (zoomHandler.resolutionY() * pageLayout.height));
    zoomHandler.setZoom(zoom);
}

void KoPAUtil::setSizeAndZoom(const KoPageLayout &pageLayout, QSizeF &thumbnailSize, KoZoomHandler &zoomHandler)
{
    const qreal realWidth = zoomHandler.resolutionX() * pageLayout.width;
    const qreal realHeight = zoomHandler.resolutionY() * pageLayout.height;

    const qreal widthScale = thumbnailSize.width() / realWidth;
    const qreal heightScale = thumbnailSize.height() / realHeight;

    // adapt thumbnailSize to match the rendered page
    if (widthScale > heightScale) {
        const int thumbnailWidth = qMin(thumbnailSize.width(), realWidth * heightScale);
        thumbnailSize.setWidth(thumbnailWidth);
    } else {
        const int thumbnailHeight = qMin(thumbnailSize.height(), realHeight * widthScale);
        thumbnailSize.setHeight(thumbnailHeight);
    }

    // set zoom
    const qreal zoom = (widthScale > heightScale) ? heightScale : widthScale;
    zoomHandler.setZoom(zoom);
}

QRect KoPAUtil::pageRect(const KoPageLayout &pageLayout, const QSizeF &size, const KoZoomHandler &zoomHandler)
{
    int width = int(0.5 + zoomHandler.documentToViewX(pageLayout.width));
    int height = int(0.5 + zoomHandler.documentToViewY(pageLayout.height));
    int x = int((size.width() - width) / 2.0);
    int y = int((size.height() - height) / 2.0);
    return QRect(x, y, width, height);
}
