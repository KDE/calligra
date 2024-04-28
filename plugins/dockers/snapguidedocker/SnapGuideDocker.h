/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   SPDX-FileCopyrightText: 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   SPDX-FileCopyrightText: 2002 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
   SPDX-FileCopyrightText: 2005 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2005-2008 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006 C. Boemann <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SNAPGUIDEDOCKER_H
#define SNAPGUIDEDOCKER_H

#include <KoCanvasObserverBase.h>
#include <QDockWidget>

/// A docker for setting properties of a snapping
class SnapGuideDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT

public:
    /// Creates the stroke docker
    SnapGuideDocker();
    ~SnapGuideDocker() override;

private Q_SLOTS:
    void locationChanged(Qt::DockWidgetArea area);

private:
    /// reimplemented
    QString observerName() const override
    {
        return QStringLiteral("SnapGuideDocker");
    }
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private:
    class Private;
    Private *const d;
};

#endif // SNAPGUIDEDOCKER_H
