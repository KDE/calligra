/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2019 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ProxyView.h"

#include <KXMLGUIFactory>

#include <QDebug>

ProxyView::ProxyView(KoPart *karbonPart, KoDocument *doc, QWidget *parent)
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
