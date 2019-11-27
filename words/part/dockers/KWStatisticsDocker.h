/* This file is part of the KDE project
 * Copyright (C) 2007 Fredy Yanardi <fyanardi@gmail.com>
 * Copyright (C) 2011 Boudewijn Rempt <boud@valdyas.org>
 * Copyright (C) 2012 Shreya Pandit <shreya@shreyapandit.com>
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

#ifndef KWSTATISTICSDOCKER_H
#define KWSTATISTICSDOCKER_H

// Base classes
#include <QDockWidget>
#include <KoCanvasObserverBase.h>

// Qt
#include <QBoxLayout>
#include <QGridLayout>

// Calligra
#include <KoDockFactoryBase.h>

// Local
#include <dockers/KWStatisticsWidget.h>

class KoCanvasBase;


class KWStatisticsDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT

public:
    explicit KWStatisticsDocker();
    ~KWStatisticsDocker() override;
    /// reimplemented from KoCanvasObserver
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private:
    bool m_canvasReset;

    // The statistics widget
    KWStatisticsWidget *m_statisticsWidget;

    
public Q_SLOTS:
    void ondockLocationChanged(Qt::DockWidgetArea newArea);
};

class KWStatisticsDockerFactory : public KoDockFactoryBase
{
public:
    KWStatisticsDockerFactory();
    QString id() const override;
    QDockWidget* createDockWidget() override;
    DockPosition defaultDockPosition() const override {
        return DockMinimized;
    }
};

#endif
