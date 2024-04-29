/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2019 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __PROXYVIEW_H__
#define __PROXYVIEW_H__

#include <KoPageLayout.h>
#include <KoView.h>

class ProxyView : public KoView
{
    Q_OBJECT
public:
    ProxyView(KoPart *part, KoDocument *doc, QWidget *parent = nullptr);
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
