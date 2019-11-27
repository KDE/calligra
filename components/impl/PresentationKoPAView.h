/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Sujith Haridasan <sujith.h@gmail.com>
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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

namespace Calligra {
namespace Components {

class PresentationKoPAView : public QObject, public KoPAViewBase
{
    Q_OBJECT
public:
    PresentationKoPAView(KoCanvasController* canvasController, KoPACanvasBase* canvas, KPrDocument* prDocument);
    ~PresentationKoPAView() override;
    void setShowRulers (bool show) override;
    void editPaste() override;
    void pagePaste() override;
    void insertPage() override;
    void updatePageNavigationActions() override;
    void setActionEnabled (int actions, bool enable) override;
    void navigatePage (KoPageApp::PageNavigation pageNavigation) override;
    KoPAPageBase* activePage() const override;
    void setActivePage (KoPAPageBase* page) override;
    void doUpdateActivePage (KoPAPageBase* page) override;
    KoZoomController* zoomController() const override;
    KoPADocument* kopaDocument() const override;
    KoPACanvasBase* kopaCanvas() const override;

    void setZoomController(KoZoomController* controller);

public Q_SLOTS:
    void connectToZoomController();

private:
    class Private;
    Private* const d;

private Q_SLOTS:
    void slotZoomChanged (KoZoomMode::Mode mode, qreal zoom);
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_PRESENTATIONKOPAVIEW_H
