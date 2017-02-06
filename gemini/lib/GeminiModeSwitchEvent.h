/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef GEMINIMODESWITCHEVENT_H
#define GEMINIMODESWITCHEVENT_H

#include <QEvent>

struct GeminiModeSynchronisationObject {
    GeminiModeSynchronisationObject() : initialized(false) { }
    bool initialized;
};

class GeminiModeSwitchEvent : public QEvent
{
public:
    enum GeminiModeEventType {
        AboutToSwitchViewModeEvent = QEvent::User + 1,
        SwitchedToThisModeEvent ///< This is the case when 
    };

    inline GeminiModeSwitchEvent(GeminiModeEventType type, QObject* fromView, QObject* toView, GeminiModeSynchronisationObject* syncObject)
            : QEvent(static_cast<QEvent::Type>(type))
            , m_fromView(fromView)
            , m_toView(toView)
            , m_syncObject(syncObject) {

    }

    inline QObject* fromView() const {
        return m_fromView;
    }
    inline QObject* toView() const {
        return m_toView;
    }

    inline GeminiModeSynchronisationObject* synchronisationObject() const {
        return m_syncObject;
    }

private:
    QObject* m_fromView;
    QObject* m_toView;
    GeminiModeSynchronisationObject* m_syncObject;
};

#endif // GEMINIMODESWITCHEVENT_H

