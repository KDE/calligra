/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2012 Shreya Pandit <shreya@shreyapandit.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWSTATISTICSDOCKER_H
#define KWSTATISTICSDOCKER_H

// Base classes
#include <KoCanvasObserverBase.h>
#include <QDockWidget>

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
    QDockWidget *createDockWidget() override;
    DockPosition defaultDockPosition() const override
    {
        return DockMinimized;
    }
};

#endif
