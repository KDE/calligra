/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_input_manager.h"

#include <QtCore/QDebug>

#include <KoToolProxy.h>

#include <kis_canvas2.h>

#include "kis_shortcut.h"
#include "kis_abstract_input_action.h"
#include "kis_tool_invocation_action.h"
#include "kis_pan_action.h"
#include "kis_alternate_invocation_action.h"

class KisInputManager::Private
{
public:
    Private() : toolProxy(0), currentAction(0), eventCount(0) { }
    void match(QEvent* event);
    void matchKey(QKeyEvent *event);
    void matchMouse(QMouseEvent *event);
    void matchTablet(QTabletEvent *event);

    KisCanvas2* canvas;
    KoToolProxy* toolProxy;

    KisAbstractInputAction* currentAction;
    KisShortcut* currentShortcut;

    QList<KisShortcut*> shortcuts;
    QList<KisAbstractInputAction*> actions;

    QList<KisShortcut*> potentialShortcuts;

    int eventCount;
};

KisInputManager::KisInputManager(KisCanvas2* canvas, KoToolProxy* proxy)
    : QObject( canvas ), d( new Private )
{
    d->canvas = canvas;
    d->toolProxy = proxy;

    KisAbstractInputAction* action = new KisToolInvocationAction(canvas, proxy);
    KisShortcut* shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::LeftButton);
    d->actions.append(action);
    d->shortcuts.append(shortcut);

    action = new KisAlternateInvocationAction(canvas, proxy);
    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::LeftButton);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Control);
    d->actions.append(action);
    d->shortcuts.append(shortcut);

    action = new KisPanAction(canvas, proxy);
    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Space);
    d->actions.append(action);
    d->shortcuts.append(shortcut);

    d->potentialShortcuts = d->shortcuts;
}

bool KisInputManager::eventFilter(QObject* object, QEvent* event)
{
//     if(object != d->canvas->canvasWidget())
//         return false;

    switch(event->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::MouseButtonDblClick:
        case QEvent::TabletPress:
        case QEvent::TabletRelease:
        case QEvent::TabletMove:
            d->match(event);
            if(d->currentAction) {
                d->currentAction->inputEvent(event);
            }
            return true;
        case QEvent::Enter:
            d->canvas->canvasWidget()->setFocus();
            break;
        default:
            break;
    }

    return false;
}

void KisInputManager::actionEnded()
{
    d->currentAction = 0;
    d->potentialShortcuts.clear();
}

KisInputManager::~KisInputManager()
{

}

void KisInputManager::Private::match(QEvent* event)
{
    switch(event->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
            matchKey(static_cast<QKeyEvent*>(event));
            break;
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
            matchMouse(static_cast<QMouseEvent*>(event));
            break;
        case QEvent::MouseMove:
            if(!currentAction) {
                actions.at(0)->inputEvent(event);
            }
        case QEvent::TabletPress:
        case QEvent::TabletRelease:
            matchTablet(static_cast<QTabletEvent*>(event));
            break;
        default:
            break;
    }

    if(currentShortcut && currentShortcut->matchLevel() == KisShortcut::NoMatch) {
        currentAction->end();
        currentAction = 0;
        currentShortcut = 0;
        potentialShortcuts = shortcuts;
        eventCount = 0;
        return;
    }

    foreach(KisShortcut* shortcut, potentialShortcuts) {
        if(shortcut->matchLevel() == KisShortcut::NoMatch) {
            potentialShortcuts.removeOne(shortcut);
        }
    }

    if(potentialShortcuts.count() == 0) {
        potentialShortcuts = shortcuts;
        eventCount = 0;
        return;
    }

    if(!currentShortcut && (potentialShortcuts.count() == 1 || eventCount > 5)) {
        KisShortcut* completedShortcut = 0;
        foreach(KisShortcut* shortcut, potentialShortcuts) {
            if(shortcut->matchLevel() == KisShortcut::CompleteMatch) {
                if(!completedShortcut || completedShortcut->priority() < shortcut->priority()) {
                    completedShortcut = shortcut;
                }
            }
        }

        if(completedShortcut) {
            currentShortcut = completedShortcut;
            currentAction = completedShortcut->action();
            currentAction->begin();
        }

        eventCount = 0;
    }

    eventCount++;
}

void KisInputManager::Private::matchKey(QKeyEvent* event)
{
    foreach(KisShortcut* shortcut, potentialShortcuts)
    {
        if( event->type() == QEvent::KeyPress ) {
            shortcut->keyPress(static_cast<Qt::Key>(event->key()));
        } else {
            shortcut->keyRelease(static_cast<Qt::Key>(event->key()));
        }
    }
}

void KisInputManager::Private::matchMouse(QMouseEvent* event)
{
    foreach(KisShortcut* shortcut, potentialShortcuts)
    {
        if(event->type() == QEvent::MouseButtonPress) {
            shortcut->buttonPress(event->button());
        } else {
            shortcut->buttonRelease(event->button());
        }
    }
}

void KisInputManager::Private::matchTablet(QTabletEvent* event)
{

}
