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

#include "GeminiMainWindow.h"
#include "GeminiModeSwitchEvent.h"

#include <QApplication>
#include <QMap>
#include <QQuickView>
#include <QTimer>
#include <QVBoxLayout>

#ifdef Q_OS_WIN
// Slate mode/docked detection stuff
#include <Shellapi.h>
#define SM_CONVERTIBLESLATEMODE 0x2003
#define SM_SYSTEMDOCKED         0x2004
#endif

struct View {
    View() : quickView(0), widget(0) {}
    QQuickView* quickView;
    QWidget* widget;
    QWidget* getWidget(QWidget* parent = 0) {
        QWidget* view;
        if(widget) {
            view = widget;
        }
        else {
            QWidget* container = QWidget::createWindowContainer(quickView);
            view = new QWidget(parent);
            view->setLayout(new QVBoxLayout());
            view->layout()->setContentsMargins(0,0,0,0);
            view->layout()->setSpacing(0);
            view->layout()->addWidget(container);
        }
        return view;
    }
};

class GeminiMainWindow::Private {
public:
    Private(GeminiMainWindow* qq)
        : q(qq)
        , fullScreenThrottle(new QTimer(qq))
        , currentState(UnknownState)
        , stateLocked(false)
    {
        fullScreenThrottle->setInterval(500);
        fullScreenThrottle->setSingleShot(true);

        // Initialise the event receiver map to empty
        eventReceivers.insert(CreateState, 0);
        eventReceivers.insert(EditState, 0);
        eventReceivers.insert(ViewState, 0);

        // Initialise the view map to empty
        views.insert(CreateState, 0);
        views.insert(EditState, 0);
        views.insert(ViewState, 0);
    }
    GeminiMainWindow* q;
    QTimer* fullScreenThrottle;

    GeminiState previousState;
    GeminiState currentState;
    bool stateLocked;

    QMap<GeminiState, View*> views;
    QMap<GeminiState, QObject*> eventReceivers;

    GeminiModeSynchronisationObject* syncObject;
};

GeminiMainWindow::GeminiMainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , d(new Private(this))
{
}

GeminiMainWindow::~GeminiMainWindow()
{
    delete d;
}

GeminiMainWindow::GeminiState GeminiMainWindow::currentState() const
{
    return d->currentState;
}

bool GeminiMainWindow::fullScreen() const
{
    return (windowState() & Qt::WindowFullScreen) == Qt::WindowFullScreen;
}

void GeminiMainWindow::setFullScreen(bool newState)
{
    if(newState) {
        if(d->fullScreenThrottle->isActive()) {
            // not a good thing... you need to avoid this happening. This exists to avoid a death-loop,
            // such as what might happen if readermode is enabled when the window is not maximised
            // as this causes a resize loop which makes readermode switch between enabled and disabled,
            // which in turn makes fullScreen be set and reset all the time... very bad, so let's try
            // and avoid that.
        }
        else {
            setWindowState(windowState() | Qt::WindowFullScreen);
        }
    }
    else {
        // this is really unpleasant... however, fullscreen is very twitchy, and exiting it as below
        // will cause an inconsistent state, so we simply assume exiting fullscreen leaves you maximised.
        // It isn't optimal, but it is the best state for now, this has taken too long to work out.
        // setWindowState(windowState() & ~Qt::WindowFullScreen);
        // should really do it, but... it doesn't. So, we end up with what we have next:
        showMaximized();
    }
    d->fullScreenThrottle->start();
    emit fullScreenChanged();
}


void GeminiMainWindow::changeState(GeminiMainWindow::GeminiState newState, bool lockNewState)
{
    d->syncObject = new GeminiModeSynchronisationObject;

    if(centralWidget()) {
        //Notify the view we are switching away from that we are about to switch away from it
        //giving it the possibility to set up the synchronisation object.
        GeminiModeSwitchEvent aboutToSwitchEvent(GeminiModeSwitchEvent::AboutToSwitchViewModeEvent, d->eventReceivers[d->currentState], d->eventReceivers[newState], d->syncObject);
        QApplication::sendEvent(d->eventReceivers[d->currentState], &aboutToSwitchEvent);

        centralWidget()->setParent(0);
    }

    View* view = d->views[newState];
    setCentralWidget(view->getWidget());
    qApp->processEvents();
    if(view->quickView)
        view->quickView->setVisible(true);
    resize(size());

    d->previousState = d->currentState;
    d->currentState = newState;
    emit currentStateChanged();

    QTimer::singleShot(50, this, SLOT(stateChanging()));
}

void GeminiMainWindow::stateChanging()
{
    qApp->processEvents();
    //Notify the new view that we just switched to it, passing our synchronisation object
    //so it can use those values to sync with the old view.
    GeminiModeSwitchEvent switchedEvent(GeminiModeSwitchEvent::SwitchedToThisModeEvent, d->eventReceivers[d->previousState], d->eventReceivers[d->currentState], d->syncObject);
    QApplication::sendEvent(d->eventReceivers[d->currentState], &switchedEvent);
    d->syncObject = 0;
    qApp->processEvents();
}

bool GeminiMainWindow::stateLocked() const
{
    return d->stateLocked;
}

void GeminiMainWindow::setStateLocked(bool locked)
{
    d->stateLocked = locked;
    emit stateLockedChanged();
}

void GeminiMainWindow::setViewForState(QWidget* widget, GeminiMainWindow::GeminiState state)
{
    View* view = d->views[state];
    view->quickView = 0;
    view->widget = widget;
}

void GeminiMainWindow::setViewForState(QQuickView* quickView, GeminiMainWindow::GeminiState state)
{
    View* view = d->views[state];
    view->quickView = quickView;
    view->widget = 0;
}

void GeminiMainWindow::setEventReceiverForState(QObject* receiver, GeminiMainWindow::GeminiState state)
{
    d->eventReceivers[state] = receiver;
}

#ifdef Q_OS_WIN
bool GeminiMainWindow::winEvent( MSG * message, long * result )
{
    if (message && message->message == WM_SETTINGCHANGE && message->lParam && !d->stateLocked)
    {
        if (wcscmp(TEXT("ConvertibleSlateMode"), (TCHAR *) message->lParam) == 0 || wcscmp(TEXT("SystemDockMode"), (TCHAR *) message->lParam) == 0) {
            bool slateMode = (GetSystemMetrics(SM_CONVERTIBLESLATEMODE) == 0);
            bool undocked = !(GetSystemMetrics(SM_SYSTEMDOCKED) != 0);
            if(slateMode || undocked) {
                // find out if we are entirely without sensible input devices, or portrait
                // if we are, change to ViewState. EditState will do for now as an autoswitch...
                changeState(EditState);
            }
            else {
                // If we are neither slate nor undocked, then we're in clamshell or we are docked, which is the same thing.
                changeState(CreateState);
            }
            *result = 0;
            return true;
        }
    }
    return false;
}
#endif
