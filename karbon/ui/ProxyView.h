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

#ifndef __PROXYVIEW_H__
#define __PROXYVIEW_H__

#include <KoView.h>
#include <KoPageLayout.h>

class ProxyView : public KoView
{
    Q_OBJECT
public:
    ProxyView(KoPart *part, KoDocument* doc, QWidget* parent = 0);
    ~ProxyView() override;

    void updateReadWrite(bool readwrite) override;
    KoZoomController *zoomController() const override;
    KoPageLayout pageLayout() const override;
    void guiActivateEvent(bool activated) override;

    QPrintDialog *createPrintDialog(KoPrintJob *printJob, QWidget *parent) override;
    KoPrintJob *createPrintJob() override;
    KoPrintJob *createPdfPrintJob() override;

private:
    friend class KarbonPart;
    KoView *view;
};

#endif

