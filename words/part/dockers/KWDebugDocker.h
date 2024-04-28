/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWDEBUGDOCKER_H
#define KWDEBUGDOCKER_H

// Base classes
#include <KoCanvasObserverBase.h>
#include <QDockWidget>

class KWDebugWidget;
class KoCanvasBase;

class KWDebugDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit KWDebugDocker();
    ~KWDebugDocker() override;
    /// reimplemented from KoCanvasObserver
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private:
    // The debug widget
    KWDebugWidget *m_debugWidget;
};

#endif // KWDEBUGDOCKER_H
