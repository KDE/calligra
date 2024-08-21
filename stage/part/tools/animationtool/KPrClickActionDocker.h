/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 C. Boemann <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRCLICKACTIONDOCKER_H
#define KPRCLICKACTIONDOCKER_H

#include <KoCanvasObserverBase.h>
#include <QMap>
#include <QPixmap>
#include <QWidget>

class KUndo2Command;
class KoPAViewBase;
class KoCanvasBase;
class KPrSoundCollection;

/**
 * This is the click action docker widget that let's you choose a click action for your shapes
 */
class KPrClickActionDocker : public QWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit KPrClickActionDocker(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    void setView(KoPAViewBase *view);

public Q_SLOTS:
    void addCommand(KUndo2Command *command);

private Q_SLOTS:
    /// selection has changed
    void selectionChanged();

    /// reimplemented
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override
    {
        m_canvas = nullptr;
        m_view = nullptr;
    }

private:
    KoPAViewBase *m_view;
    KPrSoundCollection *m_soundCollection;
    KoCanvasBase *m_canvas;
    QMap<QString, QWidget *> m_eventActionWidgets;
};

#endif // KPRCLICKACTIONDOCKER_H
