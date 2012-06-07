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

#include <QDebug>
#include <QQueue>

#include <KoToolProxy.h>

#include <kis_canvas2.h>
#include <ko_favorite_resource_manager.h>

#include "kis_shortcut.h"
#include "kis_abstract_input_action.h"
#include "kis_tool_invocation_action.h"
#include "kis_pan_action.h"
#include "kis_alternate_invocation_action.h"
#include "kis_rotate_canvas_action.h"
#include "kis_zoom_action.h"
#include "kis_show_palette_action.h"

class KisInputManager::Private
{
public:
    Private() : toolProxy(0), currentAction(0), currentShortcut(0) { }
    void match(QEvent *event);

    KisCanvas2 *canvas;
    KoToolProxy *toolProxy;

    KisAbstractInputAction* currentAction;
    KisShortcut* currentShortcut;

    QList<KisShortcut*> shortcuts;
    QList<KisAbstractInputAction*> actions;

    QList<KisShortcut*> potentialShortcuts;

    QPointF mousePosition;

    QQueue<QEvent*> eventQueue;
};

KisInputManager::KisInputManager(KisCanvas2 *canvas, KoToolProxy *proxy)
    : QObject(canvas), d(new Private)
{
    d->canvas = canvas;
    d->toolProxy = proxy;

    KisAbstractInputAction* action = new KisToolInvocationAction(this);
    KisShortcut* shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::LeftButton);
    d->actions.append(action);
    d->shortcuts.append(shortcut);

    action = new KisAlternateInvocationAction(this);
    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::LeftButton);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Control);
    d->actions.append(action);
    d->shortcuts.append(shortcut);

    action = new KisPanAction(this);
    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Space);
    d->actions.append(action);
    d->shortcuts.append(shortcut);

    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::MiddleButton);
    d->shortcuts.append(shortcut);

    action = new KisRotateCanvasAction(this);
    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Shift << Qt::Key_Space);
    d->actions.append(action);
    d->shortcuts.append(shortcut);

    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Shift);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::MiddleButton);
    d->shortcuts.append(shortcut);

    action = new KisZoomAction(this);
    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Control << Qt::Key_Space);
    d->actions.append(action);
    d->shortcuts.append(shortcut);

    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Control);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::MiddleButton);
    d->shortcuts.append(shortcut);

    action = new KisShowPaletteAction(this);
    shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::RightButton);
    d->actions.append(action);
    d->shortcuts.append(shortcut);

    d->potentialShortcuts = d->shortcuts;
}

bool KisInputManager::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object)
    switch(event->type()) {
        case QEvent::KeyPress:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick:
            if(canvas()->favoriteResourceManager()->isPopupPaletteVisible()) {
                canvas()->favoriteResourceManager()->slotShowPopupPalette();
            }
        case QEvent::MouseMove:
            if(event->type() == QEvent::MouseMove) {
                QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
                d->mousePosition = d->canvas->coordinatesConverter()->widgetToDocument(mevent->posF());
                if(!d->currentAction) {
                    d->toolProxy->mouseMoveEvent(mevent, d->mousePosition);
                }
            }
        case QEvent::KeyRelease:
        case QEvent::MouseButtonRelease:
        case QEvent::Wheel:
            if(d->currentAction) {
                d->currentShortcut->match(event);
                if(d->currentShortcut->matchLevel() == KisShortcut::NoMatch) {
                    d->currentAction->end();
                    d->currentAction = 0;
                    d->currentShortcut = 0;
                    d->potentialShortcuts = d->shortcuts;
                    d->eventQueue.clear();
                    break;
                }

                while(!d->eventQueue.isEmpty()) {
                    d->currentAction->inputEvent(d->eventQueue.dequeue());
                }
                d->currentAction->inputEvent(event);
            } else {
                d->match(event);
            }
            return true;
        case QEvent::Enter:
            d->canvas->canvasWidget()->setFocus();
            return true;
        case QEvent::Leave:
            if(d->currentAction) {
                d->currentShortcut->clear();
                d->currentAction->end();
                d->currentAction = 0;
                d->currentShortcut = 0;
                d->potentialShortcuts = d->shortcuts;
                d->eventQueue.clear();
            }
            return true;
        case QEvent::TabletMove:
            if(d->currentAction && d->currentAction->handleTablet()) {
                d->currentAction->inputEvent(event);
                return true;
            }
        case QEvent::TabletPress:
        case QEvent::TabletRelease:
            event->ignore();
        default:
            break;
    }

    return false;
}

KisCanvas2* KisInputManager::canvas() const
{
    return d->canvas;
}

KoToolProxy* KisInputManager::toolProxy() const
{
    return d->toolProxy;
}

QPointF KisInputManager::mousePosition() const
{
    return d->mousePosition;
}

KisInputManager::~KisInputManager()
{
    qDeleteAll(d->shortcuts);
    qDeleteAll(d->actions);
}

void KisInputManager::Private::match(QEvent* event)
{
    foreach(KisShortcut* shortcut, potentialShortcuts) {
        shortcut->match(event);
        if(shortcut->matchLevel() == KisShortcut::NoMatch) {
            potentialShortcuts.removeOne(shortcut);
        }
    }

    if(potentialShortcuts.count() == 0) {
        potentialShortcuts = shortcuts;
        eventQueue.clear();
        return;
    }

    eventQueue.enqueue(event);
    if(potentialShortcuts.count() == 1 || eventQueue.count() >= 5) {
        KisShortcut* completedShortcut = 0;
        foreach(KisShortcut* shortcut, potentialShortcuts) {
            if(shortcut->matchLevel() == KisShortcut::CompleteMatch) {
                if(!completedShortcut || completedShortcut->priority() < shortcut->priority()) {
                    completedShortcut = shortcut;
                }
            } else {
                shortcut->clear();
            }
        }

        if(completedShortcut) {
            currentShortcut = completedShortcut;
            currentAction = completedShortcut->action();
            currentAction->begin();
        }

        eventQueue.clear();
    }
}
