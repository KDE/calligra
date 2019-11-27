/* This file is part of the KDE project
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef GEMINIMAINWINDOW_H
#define GEMINIMAINWINDOW_H

#include <QMainWindow>
#include "gemini_export.h"
class QQuickView;

/**
 * \short 
 * 
 */
class GEMINI_EXPORT GeminiMainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(GeminiState currentState READ currentState NOTIFY currentStateChanged)
    Q_PROPERTY(bool fullScreen READ fullScreen WRITE setFullScreen NOTIFY fullScreenChanged)
    Q_ENUMS(GeminiState)
public:
    enum GeminiState {
        AllStates = -3,
        UnknownState = -2,
        NoState = -1,
        CreateState = 1,
        EditState = 2,
        ViewState = 3
    };

    explicit GeminiMainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~GeminiMainWindow() override;

    GeminiState currentState() const;

    bool fullScreen() const;
    void setFullScreen(bool newState);

    bool stateLocked() const;
    void setStateLocked(bool locked);

    void changeState(GeminiState newState, bool lockNewState = false);

    void setViewForState(QWidget* widget, GeminiState state);
    void setViewForState(QQuickView* quickView, GeminiState state);

    void setEventReceiverForState(QObject* receiver, GeminiState state);

Q_SIGNALS:
    void stateLockedChanged();
    void currentStateChanged();
    void fullScreenChanged();

private Q_SLOTS:
    void stateChanging();

private:
    class Private;
    Private* d;
#ifdef Q_OS_WIN
    bool winEvent(MSG * message, long * result);
#endif
};

#endif // GEMINIMAINWINDOW_H
