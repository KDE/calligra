/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWNAVIGATIONDOCKER_H
#define KWNAVIGATIONDOCKER_H

// Base classes
#include <KoCanvasObserverBase.h>
#include <QDockWidget>

class KWNavigationWidget;
class KoCanvasBase;

class KWNavigationDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit KWNavigationDocker();
    ~KWNavigationDocker() override;
    /// reimplemented from KoCanvasObserver
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private:
    bool m_canvasReset;

    // The navigation widget
    KWNavigationWidget *m_navigationWidget;
};

#endif // KWNAVIGATIONDOCKER_H
