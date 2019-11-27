/* This file is part of the KDE project
 * Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
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

#ifndef KWNAVIGATIONDOCKER_H
#define KWNAVIGATIONDOCKER_H

// Base classes
#include <QDockWidget>
#include <KoCanvasObserverBase.h>

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

#endif //KWNAVIGATIONDOCKER_H
