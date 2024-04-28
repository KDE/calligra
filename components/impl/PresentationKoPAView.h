/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Sujith Haridasan <sujith.h@gmail.com>
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_PRESENTATIONKOPAVIEW_H
#define CALLIGRA_COMPONENTS_PRESENTATIONKOPAVIEW_H

#include <KoPAViewBase.h>
#include <KoZoomMode.h>

class KoCanvasController;
class KoZoomMode;
class KPrDocument;
class KoPACanvasBase;

namespace Calligra
{
namespace Components
{

class PresentationKoPAView : public QObject, public KoPAViewBase
{
    Q_OBJECT
public:
    PresentationKoPAView(KoCanvasController *canvasController, KoPACanvasBase *canvas, KPrDocument *prDocument);
    ~PresentationKoPAView() override;
    void setShowRulers(bool show) override;
    void editPaste() override;
    void pagePaste() override;
    void insertPage() override;
    void updatePageNavigationActions() override;
    void setActionEnabled(int actions, bool enable) override;
    void navigatePage(KoPageApp::PageNavigation pageNavigation) override;
    KoPAPageBase *activePage() const override;
    void setActivePage(KoPAPageBase *page) override;
    void doUpdateActivePage(KoPAPageBase *page) override;
    KoZoomController *zoomController() const override;
    KoPADocument *kopaDocument() const override;
    KoPACanvasBase *kopaCanvas() const override;

    void setZoomController(KoZoomController *controller);

public Q_SLOTS:
    void connectToZoomController();

private:
    class Private;
    Private *const d;

private Q_SLOTS:
    void slotZoomChanged(KoZoomMode::Mode mode, qreal zoom);
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_PRESENTATIONKOPAVIEW_H
