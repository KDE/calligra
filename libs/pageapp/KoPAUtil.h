/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAUTIL_H
#define KOPAUTIL_H

#include "kopageapp_export.h"

class QSizeF;
class QRect;
struct KoPageLayout;
class KoZoomHandler;

class KOPAGEAPP_EXPORT KoPAUtil
{
public:
    /**
     * Set the zoom so the page fully fits into size
     */
    static void setZoom(const KoPageLayout &pageLayout, const QSizeF &size, KoZoomHandler &zoomHandler);

    /**
     * Set the zoom and adapt the size, so the page fully fits into size and the size matches the page rect
     */
    static void setSizeAndZoom(const KoPageLayout &pageLayout, QSizeF &size, KoZoomHandler &zoomHandler);

    /**
     * Get the page rect used
     */
    static QRect pageRect(const KoPageLayout &pageLayout, const QSizeF &size, const KoZoomHandler &zoomHandler);
};

#endif /* KOPAUTIL_H */
