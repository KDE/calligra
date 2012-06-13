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
#include "kis_change_primary_setting_action.h"

class KisInputManager::Private
{
public:
    Private(KisInputManager *qq) : q(qq), toolProxy(0), currentAction(0), currentShortcut(0), tabletPressEvent(0) { }
    void match(QEvent *event);
    void setupActions();
    KisShortcut *createShortcut(KisAbstractInputAction* action, int index);

    KisInputManager *q;

    KisCanvas2 *canvas;
    KoToolProxy *toolProxy;

    KisAbstractInputAction* currentAction;
    KisShortcut* currentShortcut;

    QList<KisShortcut*> shortcuts;
    QList<KisAbstractInputAction*> actions;

    QList<KisShortcut*> potentialShortcuts;

    QPointF mousePosition;

    QQueue<QEvent*> eventQueue;

    QTabletEvent *tabletPressEvent;
};

KisInputManager::KisInputManager(KisCanvas2 *canvas, KoToolProxy *proxy)
    : QObject(canvas), d(new Private(this))
{
    d->canvas = canvas;
    d->toolProxy = proxy;

    d->setupActions();

    d->potentialShortcuts = d->shortcuts;
}

bool KisInputManager::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object)
    switch(event->type()) {
        case QEvent::KeyPress:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick: {
            QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
            d->mousePosition = d->canvas->coordinatesConverter()->widgetToDocument(mevent->posF());
            if(canvas()->favoriteResourceManager()->isPopupPaletteVisible()) {
                canvas()->favoriteResourceManager()->slotShowPopupPalette();
            }
        }
        case QEvent::MouseMove:
            if(event->type() == QEvent::MouseMove) {
                if(!d->currentAction) {
                    QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
                    d->toolProxy->mouseMoveEvent(mevent, d->canvas->coordinatesConverter()->widgetToDocument(mevent->posF()));
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
                    d->tabletPressEvent = 0;
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
                d->tabletPressEvent = 0;
                d->potentialShortcuts = d->shortcuts;
                d->eventQueue.clear();
            }
            return true;
        case QEvent::TabletPress: {
            QTabletEvent* tevent = static_cast<QTabletEvent*>(event);

            QTabletEvent* newEvent = new QTabletEvent(QEvent::TabletPress,
                tevent->pos(),
                tevent->globalPos(),
                tevent->hiResGlobalPos(),
                tevent->device(),
                tevent->pointerType(),
                tevent->pressure(),
                tevent->xTilt(),
                tevent->yTilt(),
                tevent->tangentialPressure(),
                tevent->rotation(),
                tevent->z(),
                tevent->modifiers(),
                tevent->uniqueId()
            );
            d->tabletPressEvent = newEvent;
            event->ignore();
            break;
        }
        case QEvent::TabletMove:
            if(d->currentAction && d->currentAction->handleTablet()) {
                d->currentAction->inputEvent(event);
                return true;
            } else {
                event->ignore();
            }
            break;
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

QTabletEvent* KisInputManager::tabletPressEvent() const
{
    return d->tabletPressEvent;
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
            currentAction->begin(completedShortcut->shortcutIndex());
        }

        eventQueue.clear();
    }
}

void KisInputManager::Private::setupActions()
{
    KisAbstractInputAction* action = new KisToolInvocationAction(q);
    actions.append(action);

    KisShortcut* shortcut = createShortcut(action, 0);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::LeftButton);

    action = new KisAlternateInvocationAction(q);
    actions.append(action);

    shortcut = createShortcut(action, 0);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::LeftButton);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Control);

    action = new KisChangePrimarySettingAction(q);
    actions.append(action);

    shortcut = createShortcut(action, 0);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::LeftButton);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Shift);

    action = new KisPanAction(q);
    actions.append(action);

    shortcut = createShortcut(action, KisPanAction::PanToggleShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Space);

    shortcut = createShortcut(action, KisPanAction::PanToggleShortcut);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::MiddleButton);

    shortcut = createShortcut(action, KisPanAction::PanLeftShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Left);
    shortcut = createShortcut(action, KisPanAction::PanRightShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Right);
    shortcut = createShortcut(action, KisPanAction::PanUpShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Up);
    shortcut = createShortcut(action, KisPanAction::PanDownShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Down);

    action = new KisRotateCanvasAction(q);
    actions.append(action);

    shortcut = createShortcut(action, KisRotateCanvasAction::RotateToggleShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Shift << Qt::Key_Space);

    shortcut = createShortcut(action, KisRotateCanvasAction::RotateToggleShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Shift);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::MiddleButton);

    shortcut = createShortcut(action, KisRotateCanvasAction::RotateLeftShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_4);

    shortcut = createShortcut(action, KisRotateCanvasAction::RotateRightShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_6);

    shortcut = createShortcut(action, KisRotateCanvasAction::RotateResetShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_5);

    action = new KisZoomAction(q);
    actions.append(action);

    shortcut = createShortcut(action, KisZoomAction::ZoomToggleShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Control << Qt::Key_Space);

    shortcut = createShortcut(action, KisZoomAction::ZoomToggleShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Control);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::MiddleButton);

    shortcut = createShortcut(action, KisZoomAction::ZoomInShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Plus);
    shortcut = createShortcut(action, KisZoomAction::ZoomOutShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_Minus);

    shortcut = createShortcut(action, KisZoomAction::ZoomResetShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_1);
    shortcut = createShortcut(action, KisZoomAction::ZoomToPageShortcut);
    shortcut->setKeys(QList<Qt::Key>() << Qt::Key_0);

    action = new KisShowPaletteAction(q);
    actions.append(action);

    shortcut = createShortcut(action, 0);
    shortcut->setButtons(QList<Qt::MouseButton>() << Qt::RightButton);
}

KisShortcut* KisInputManager::Private::createShortcut(KisAbstractInputAction* action, int index)
{
    KisShortcut* shortcut = new KisShortcut;
    shortcut->setAction(action);
    shortcut->setShortcutIndex(index);
    shortcuts.append(shortcut);

    return shortcut;
}
