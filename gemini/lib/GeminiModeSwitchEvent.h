/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef GEMINIMODESWITCHEVENT_H
#define GEMINIMODESWITCHEVENT_H

#include <QEvent>

struct GeminiModeSynchronisationObject {
    GeminiModeSynchronisationObject()
        : initialized(false)
    {
    }
    bool initialized;
};

class GeminiModeSwitchEvent : public QEvent
{
public:
    /**
     * Which type of event you are dealing with. You will only receive this in two cases:
     * When you are about to switch away from a particular mode (sent to fromView before switching)
     * When you have been made the current mode (sent to toView after switching)
     */
    enum GeminiModeEventType {
        AboutToSwitchViewModeEvent = QEvent::User + 1, ///< Write information into the event (and mark the sync object as initialized)
        SwitchedToThisModeEvent ///< Read information out of the event
    };

    inline GeminiModeSwitchEvent(GeminiModeEventType type, QObject *fromView, QObject *toView, GeminiModeSynchronisationObject *syncObject)
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

    inline GeminiModeSynchronisationObject *synchronisationObject() const
    {
        return m_syncObject;
    }

private:
    QObject *m_fromView;
    QObject *m_toView;
    GeminiModeSynchronisationObject *m_syncObject;
};

#endif // GEMINIMODESWITCHEVENT_H
