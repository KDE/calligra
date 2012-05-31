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

#include <kis_canvas2.h>
#include <kis_tool_proxy.h>

#include "kis_shortcut.h"
#include "kis_abstract_input_action.h"
#include "kis_tool_invocation_action.h"
#include "kis_pan_action.h"

class KisInputManager::Private
{
public:
    Private() : toolProxy(0), currentAction(0) { }
    void match(QEvent* event);
    void matchKey(QKeyEvent *event);
    void matchMouse(QMouseEvent *event);
    void matchTablet(QTabletEvent *event);

    KisCanvas2* canvas;
    KisToolProxy* toolProxy;

    KisAbstractInputAction* currentAction;

    QList<KisShortcut*> shortcuts;
    QList<KisAbstractInputAction*> actions;

    QList<KisShortcut*> potentialMatches;
};

KisInputManager::KisInputManager(KisCanvas2* canvas)
    : QObject( canvas ), d( new Private )
{
    d->canvas = canvas;

    KisAbstractInputAction* action = new KisToolInvocationAction;
    connect(action, SIGNAL(actionEnded()), SLOT(actionEnded()));
    KisShortcut* shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setButton(Qt::LeftButton);
    d->actions.append(action);
    d->shortcuts.append(shortcut);

    action = new KisPanAction;
    connect(action, SIGNAL(actionEnded()), SLOT(actionEnded()));
    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Space);
    d->actions.append(action);
    d->shortcuts.append(shortcut);
}

bool KisInputManager::eventFilter(QObject* object, QEvent* event)
{
    if(object != d->canvas->canvasWidget())
        return false;

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
            if(d->currentAction) {
                d->currentAction->inputEvent(event);
            } else {
                d->match(event);
            }
        default:
            break;
    }

    return false;
}

void KisInputManager::actionEnded()
{
    d->currentAction = 0;
    d->potentialMatches.clear();
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
        case QEvent::MouseMove:
        case QEvent::MouseButtonDblClick:
            matchMouse(static_cast<QMouseEvent*>(event));
            break;
        case QEvent::TabletPress:
        case QEvent::TabletRelease:
        case QEvent::TabletMove:
            matchTablet(static_cast<QTabletEvent*>(event));
            break;
        default:
            return;
    }

    if(potentialMatches.count() == 1) {
        currentAction = potentialMatches.at(0)->action();
        currentAction->trigger();
    }
}

void KisInputManager::Private::matchKey(QKeyEvent* event)
{
    foreach(KisShortcut* shortcut, shortcuts)
    {
        if(shortcut->containsKey(static_cast<Qt::Key>(event->key()))) {
            if(event->type() == QEvent::KeyPress) {
                potentialMatches.append(shortcut);
            } else {
                potentialMatches.removeOne(shortcut);
            }
        } else if(potentialMatches.contains(shortcut)) {
            potentialMatches.removeOne(shortcut);
        }
    }
}

void KisInputManager::Private::matchMouse(QMouseEvent* event)
{
    foreach(KisShortcut* shortcut, shortcuts)
    {
        if(shortcut->containsButton(event->button())) {
            if(event->type() == QEvent::MouseButtonPress) {
                potentialMatches.append(shortcut);
            } else if(event->type() == QEvent::MouseButtonRelease) {
                potentialMatches.removeOne(shortcut);
            }
        } else if(potentialMatches.contains(shortcut)) {
            potentialMatches.removeOne(shortcut);
        }
    }
}

void KisInputManager::Private::matchTablet(QTabletEvent* event)
{

}
