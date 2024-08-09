/*
 * SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2005-2008 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _KO_MODEBOX_DOCKER_H_
#define _KO_MODEBOX_DOCKER_H_

#include <KoCanvasObserverBase.h>

#include <QDockWidget>

class KoCanvasBase;
class KoModeBox;

class KoModeBoxDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit KoModeBoxDocker(KoModeBox *modeBox);

    /// reimplemented from KoCanvasObserverBase
    QString observerName() const override
    {
        return QStringLiteral("KoModeBoxDocker");
    }
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private:
    KoModeBox *m_modeBox;
};

#endif // _KO_MODEBOX_DOCKER_H_
