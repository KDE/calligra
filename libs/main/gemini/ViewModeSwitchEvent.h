/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef VIEWMODESWITCHEVENT_H
#define VIEWMODESWITCHEVENT_H

#include <KoZoomMode.h>
#include <QEvent>
#include <QPointF>

class KoShape;
class KoGridData;
struct ViewModeSynchronisationObject {
    ViewModeSynchronisationObject()
        : initialized(false)
        , currentIndex(-1)
        , scrollBarValue(QPoint())
        , zoomLevel(0)
        , gridData(nullptr)
    {
    }

    bool initialized;

    int currentIndex;
    QPoint scrollBarValue;
    float zoomLevel;

    QString activeToolId;

    KoGridData *gridData;
    QList<KoShape *> shapes;
};

class ViewModeSwitchEvent : public QEvent
{
public:
    enum ViewModeEventType {
        AboutToSwitchViewModeEvent = QEvent::User + 1,
        SwitchedToDesktopModeEvent,
        SwitchedToTouchModeEvent,
    };

    inline ViewModeSwitchEvent(ViewModeEventType type, QObject *fromView, QObject *toView, ViewModeSynchronisationObject *syncObject)
        : QEvent(static_cast<QEvent::Type>(type))
        , m_fromView(fromView)
        , m_toView(toView)
        , m_syncObject(syncObject)
    {
    }

    inline QObject *fromView() const
    {
        return m_fromView;
    }
    inline QObject *toView() const
    {
        return m_toView;
    }

    inline ViewModeSynchronisationObject *synchronisationObject() const
    {
        return m_syncObject;
    }

private:
    QObject *m_fromView;
    QObject *m_toView;
    ViewModeSynchronisationObject *m_syncObject;
};

#endif // VIEWMODESWITCHEVENT_H
