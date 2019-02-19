/* This file is part of the KDE project
 * Copyright (C) 2019 Dag Andersen <danders@get2net.dk>
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

#include "ProxyView.h"

#include <KXMLGUIFactory>

#include <QDebug>

ProxyView::ProxyView(KoPart *karbonPart, KoDocument* doc, QWidget* parent)
    : KoView(karbonPart, doc, parent)
{
    setObjectName("Karbon view helper");
}

ProxyView::~ProxyView()
{
    if (factory()) {
        factory()->removeClient(this);
    }
}

void ProxyView::updateReadWrite(bool readwrite)
{
    view->updateReadWrite(readwrite);
}

KoZoomController *ProxyView::zoomController() const
{
    return view->zoomController();
}

KoPageLayout ProxyView::pageLayout() const
{
    return view->pageLayout();
}

void ProxyView::guiActivateEvent(bool activated)
{
    if (activated) {
        factory()->addClient(view);
    } else {
        factory()->removeClient(view);
    }
}

QPrintDialog *ProxyView::createPrintDialog(KoPrintJob *printJob, QWidget *parent)
{
    return view->createPrintDialog(printJob, parent);
}

KoPrintJob *ProxyView::createPrintJob()
{
    return view->createPrintJob();
}

KoPrintJob *ProxyView::createPdfPrintJob()
{
    return view->createPdfPrintJob();
}
