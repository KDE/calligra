/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef GEMINIMAINWINDOW_H
#define GEMINIMAINWINDOW_H

#include "gemini_export.h"
#include <QMainWindow>
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
public:
    enum GeminiState { AllStates = -3, UnknownState = -2, NoState = -1, CreateState = 1, EditState = 2, ViewState = 3 };
    Q_ENUM(GeminiState);

    explicit GeminiMainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~GeminiMainWindow() override;

    GeminiState currentState() const;

    bool fullScreen() const;
    void setFullScreen(bool newState);

    bool stateLocked() const;
    void setStateLocked(bool locked);

    void changeState(GeminiState newState, bool lockNewState = false);

    void setViewForState(QWidget *widget, GeminiState state);
    void setViewForState(QQuickView *quickView, GeminiState state);

    void setEventReceiverForState(QObject *receiver, GeminiState state);

Q_SIGNALS:
    void stateLockedChanged();
    void currentStateChanged();
    void fullScreenChanged();

private Q_SLOTS:
    void stateChanging();

private:
    class Private;
    Private *d;
#ifdef Q_OS_WIN
    bool winEvent(MSG *message, long *result);
#endif
};

#endif // GEMINIMAINWINDOW_H
