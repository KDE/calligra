/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2005-2010 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006-2008 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// flake
#include "KoToolManager.h"
#include "KoToolManager_p.h"

#include "FlakeDebug.h"
#include "KoCanvasBase.h"
#include "KoCanvasController.h"
#include "KoCanvasControllerWidget.h"
#include "KoInputDeviceHandlerEvent.h"
#include "KoInputDeviceHandlerRegistry.h"
#include "KoPointerEvent.h"
#include "KoSelection.h"
#include "KoShape.h"
#include "KoShapeLayer.h"
#include "KoShapeManager.h"
#include "KoShapeRegistry.h"
#include "KoToolProxy.h"
#include "KoToolProxy_p.h"
#include "KoToolRegistry.h"
#include "tools/KoCreateShapesTool.h"
#include "tools/KoPanTool.h"
#include "tools/KoZoomTool.h"

// KF5
#include <KActionCollection>
#include <KLocalizedString>

// Qt
#include <QAction>
#include <QApplication>
#include <QEvent>
#include <QGlobalStatic>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStack>
#include <QStringList>
#include <QTabletEvent>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWidget>

Q_GLOBAL_STATIC(KoToolManager, s_instance)

class Q_DECL_HIDDEN KoToolAction::Private
{
public:
    ToolHelper *toolHelper;
};

KoToolAction::KoToolAction(ToolHelper *toolHelper)
    : QObject(toolHelper)
    , d(new Private)
{
    d->toolHelper = toolHelper;
}

KoToolAction::~KoToolAction()
{
    delete d;
}

void KoToolAction::trigger()
{
    d->toolHelper->activate();
}

QString KoToolAction::iconText() const
{
    return d->toolHelper->iconText();
}

QString KoToolAction::toolTip() const
{
    return d->toolHelper->toolTip();
}

QString KoToolAction::id() const
{
    return d->toolHelper->id();
}

QString KoToolAction::iconName() const
{
    return d->toolHelper->iconName();
}

QKeySequence KoToolAction::shortcut() const
{
    return d->toolHelper->shortcut();
}

QString KoToolAction::section() const
{
    return d->toolHelper->toolType();
}

int KoToolAction::priority() const
{
    return d->toolHelper->priority();
}

int KoToolAction::buttonGroupId() const
{
    return d->toolHelper->uniqueId();
}

QString KoToolAction::visibilityCode() const
{
    return d->toolHelper->activationShapeId();
}

class CanvasData
{
public:
    CanvasData(KoCanvasController *cc, const KoInputDevice &id)
        : activeTool(nullptr)
        , canvas(cc)
        , inputDevice(id)
        , dummyToolWidget(nullptr)
        , dummyToolLabel(nullptr)
    {
    }

    ~CanvasData()
    {
        // the dummy tool widget does not necessarily have a parent and we create it, so we delete it.
        delete dummyToolWidget;
    }

    void activateToolActions()
    {
        disabledDisabledActions.clear();
        disabledActions.clear();
        disabledCanvasShortcuts.clear();
        // we do several things here
        // 1. enable the actions of the active tool
        // 2. disable conflicting actions
        // 3. replace conflicting actions in the action collection
        KActionCollection *canvasActionCollection = canvas->actionCollection();

        QHash<QString, QAction *> toolActions = activeTool->actions();
        QHash<QString, QAction *>::const_iterator it(toolActions.constBegin());

        for (; it != toolActions.constEnd(); ++it) {
            if (canvasActionCollection) {
                QString toolActionID = it.key();
                QAction *toolAction = it.value();

                QAction *action = qobject_cast<QAction *>(canvasActionCollection->action(it.key()));
                if (action) {
                    canvasActionCollection->takeAction(action);
                    if (action != it.value()) {
                        if (action->isEnabled()) {
                            action->setEnabled(false);
                            disabledActions.append(action);
                        } else {
                            disabledDisabledActions.append(action);
                        }
                    }
                }
                foreach (QAction *a, canvasActionCollection->actions()) {
                    QAction *canvasAction = dynamic_cast<QAction *>(a);
                    if (canvasAction && !canvasAction->shortcut().toString().isEmpty() && canvasAction->shortcut() == toolAction->shortcut()) {
                        warnFlake << activeToolId << ": action" << toolActionID << "conflicts with canvas action" << canvasAction->objectName()
                                  << "shortcut:" << canvasAction->shortcut().toString();
                        disabledCanvasShortcuts[canvasAction] = canvasAction->shortcut().toString();
                        canvasAction->setShortcut(QKeySequence());
                    }
                }
                canvasActionCollection->addAction(toolActionID, toolAction);
            }
            it.value()->setEnabled(true);
        }
        canvasActionCollection->readSettings(); // The shortcuts might have been configured in the meantime.
    }

    void deactivateToolActions()
    {
        if (!activeTool)
            return;
        // disable actions of active tool
        foreach (QAction *action, activeTool->actions()) {
            action->setEnabled(false);
        }

        // enable actions which where disabled on activating the active tool
        // and re-add them to the action collection
        KActionCollection *ac = canvas->actionCollection();
        foreach (QPointer<QAction> action, disabledDisabledActions) {
            if (action) {
                if (ac) {
                    ac->addAction(action->objectName(), action);
                }
            }
        }
        disabledDisabledActions.clear();

        foreach (QPointer<QAction> action, disabledActions) {
            if (action) {
                action->setEnabled(true);
                if (ac) {
                    ac->addAction(action->objectName(), action);
                }
            }
        }
        disabledActions.clear();

        QMap<QPointer<QAction>, QString>::const_iterator it(disabledCanvasShortcuts.constBegin());
        for (; it != disabledCanvasShortcuts.constEnd(); ++it) {
            QAction *action = it.key();
            QString shortcut = it.value();
            action->setShortcut(shortcut);
        }
        disabledCanvasShortcuts.clear();
    }

    KoToolBase *activeTool; // active Tool
    QString activeToolId; // the id of the active Tool
    QString activationShapeId; // the shape-type (KoShape::shapeId()) the activeTool 'belongs' to.
    QHash<QString, KoToolBase *> allTools; // all the tools that are created for this canvas.
    QStack<QString> stack; // stack of temporary tools
    KoCanvasController *const canvas;
    const KoInputDevice inputDevice;
    QWidget *dummyToolWidget; // the widget shown in the toolDocker.
    QLabel *dummyToolLabel;
    QList<QPointer<QAction>> disabledActions; ///< disabled conflicting actions
    QList<QPointer<QAction>> disabledDisabledActions; ///< disabled conflicting actions that were already disabled
    QMap<QPointer<QAction>, QString> disabledCanvasShortcuts; ///< Shortcuts that were temporarily removed from canvas actions because the tool overrides
};

KoToolManager::Private::Private(KoToolManager *qq)
    : q(qq)
    , canvasData(nullptr)
    , layerExplicitlyDisabled(false)
{
}

KoToolManager::Private::~Private()
{
    qDeleteAll(tools);
}

// helper method.
CanvasData *KoToolManager::Private::createCanvasData(KoCanvasController *controller, const KoInputDevice &device)
{
    QHash<QString, KoToolBase *> toolsHash;
    foreach (ToolHelper *tool, tools) {
        QPair<QString, KoToolBase *> toolPair = q->createTools(controller, tool);
        if (toolPair.second) { // only if a real tool was created
            toolsHash.insert(toolPair.first, toolPair.second);
        }
    }
    KoCreateShapesTool *createShapesTool = dynamic_cast<KoCreateShapesTool *>(toolsHash.value(KoCreateShapesTool_ID));
    Q_ASSERT(createShapesTool);
    QString id = KoShapeRegistry::instance()->keys()[0];
    createShapesTool->setShapeId(id);

    CanvasData *cd = new CanvasData(controller, device);
    cd->allTools = toolsHash;
    return cd;
}

void KoToolManager::Private::setup()
{
    if (tools.size() > 0)
        return;

    KoShapeRegistry::instance();
    KoToolRegistry *registry = KoToolRegistry::instance();
    foreach (const QString &id, registry->keys()) {
        ToolHelper *t = new ToolHelper(registry->value(id));
        tools.append(t);
    }

    // connect to all tools so we can hear their button-clicks
    foreach (ToolHelper *tool, tools)
        connect(tool, &ToolHelper::toolActivated, q, [this](ToolHelper *tool) {
            toolActivated(tool);
        });

    // load pluggable input devices
    KoInputDeviceHandlerRegistry::instance();
}

void KoToolManager::Private::connectActiveTool()
{
    if (canvasData->activeTool) {
        connect(canvasData->activeTool, &KoToolBase::cursorChanged, q, [this](const QCursor &cursor) {
            updateCursor(cursor);
        });
        connect(canvasData->activeTool, &KoToolBase::activateTool, q, &KoToolManager::switchToolRequested);
        connect(canvasData->activeTool, &KoToolBase::activateTemporary, q, &KoToolManager::switchToolTemporaryRequested);
        connect(canvasData->activeTool, &KoToolBase::done, q, &KoToolManager::switchBackRequested);
        connect(canvasData->activeTool, &KoToolBase::statusTextChanged, q, &KoToolManager::changedStatusText);
    }

    // we expect the tool to emit a cursor on activation.
    updateCursor(Qt::ForbiddenCursor);
}

void KoToolManager::Private::disconnectActiveTool()
{
    if (canvasData->activeTool) {
        canvasData->deactivateToolActions();
        // repaint the decorations before we deactivate the tool as it might deleted
        // data needed for the repaint
        canvasData->activeTool->deactivate();
        disconnect(canvasData->activeTool, &KoToolBase::cursorChanged, q, nullptr);
        disconnect(canvasData->activeTool, &KoToolBase::activateTool, q, &KoToolManager::switchToolRequested);
        disconnect(canvasData->activeTool, &KoToolBase::activateTemporary, q, &KoToolManager::switchToolTemporaryRequested);
        disconnect(canvasData->activeTool, &KoToolBase::done, q, &KoToolManager::switchBackRequested);
        disconnect(canvasData->activeTool, &KoToolBase::statusTextChanged, q, &KoToolManager::changedStatusText);
    }

    // emit a empty status text to clear status text from last active tool
    Q_EMIT q->changedStatusText(QString());
}

void KoToolManager::Private::switchTool(KoToolBase *tool, bool temporary)
{
    Q_ASSERT(tool);
    if (canvasData == nullptr)
        return;

    if (canvasData->activeTool == tool && tool->toolId() != KoInteractionTool_ID)
        return;

    disconnectActiveTool();
    canvasData->activeTool = tool;
    connectActiveTool();
    postSwitchTool(temporary);
}

void KoToolManager::Private::switchTool(const QString &id, bool temporary)
{
    Q_ASSERT(canvasData);
    if (!canvasData)
        return;

    if (canvasData->activeTool && temporary)
        canvasData->stack.push(canvasData->activeToolId);
    canvasData->activeToolId = id;
    KoToolBase *tool = canvasData->allTools.value(id);
    if (!tool) {
        return;
    }

    foreach (ToolHelper *th, tools) {
        if (th->id() == id) {
            canvasData->activationShapeId = th->activationShapeId();
            break;
        }
    }

    switchTool(tool, temporary);
}

void KoToolManager::Private::postSwitchTool(bool temporary)
{
#ifndef NDEBUG
    int canvasCount = 1;
    foreach (QList<CanvasData *> list, canvasses) {
        bool first = true;
        foreach (CanvasData *data, list) {
            if (first) {
                debugFlake << "Canvas" << canvasCount++;
            }
            debugFlake << "  +- Tool:" << data->activeToolId << (data == canvasData ? " *" : "");
            first = false;
        }
    }
#endif
    Q_ASSERT(canvasData);
    if (!canvasData)
        return;

    KoToolBase::ToolActivation toolActivation;
    if (temporary)
        toolActivation = KoToolBase::TemporaryActivation;
    else
        toolActivation = KoToolBase::DefaultActivation;
    QSet<KoShape *> shapesToOperateOn;
    if (canvasData->activeTool && canvasData->activeTool->canvas() && canvasData->activeTool->canvas()->shapeManager()) {
        KoSelection *selection = canvasData->activeTool->canvas()->shapeManager()->selection();
        Q_ASSERT(selection);

        foreach (KoShape *shape, selection->selectedShapes()) {
            QSet<KoShape *> delegates = shape->toolDelegates();
            if (delegates.isEmpty()) { // no delegates, just the orig shape
                shapesToOperateOn << shape;
            } else {
                shapesToOperateOn += delegates;
            }
        }
    }

    if (canvasData->canvas->canvas()) {
        // Caller of postSwitchTool expect this to be called to update the selected tool
        updateToolForProxy();
        canvasData->activeTool->activate(toolActivation, shapesToOperateOn);
        KoCanvasBase *canvas = canvasData->canvas->canvas();
        canvas->updateInputMethodInfo();
    } else {
        canvasData->activeTool->activate(toolActivation, shapesToOperateOn);
    }

    QList<QPointer<QWidget>> optionWidgetList = canvasData->activeTool->optionWidgets();
    if (optionWidgetList.empty()) { // no option widget.
        QWidget *toolWidget;
        QString title;
        foreach (ToolHelper *tool, tools) {
            if (tool->id() == canvasData->activeTool->toolId()) {
                title = tool->toolTip();
                break;
            }
        }
        toolWidget = canvasData->dummyToolWidget;
        if (toolWidget == nullptr) {
            toolWidget = new QWidget();
            toolWidget->setObjectName("DummyToolWidget");
            QVBoxLayout *layout = new QVBoxLayout(toolWidget);
            layout->setContentsMargins(3, 3, 3, 3);
            canvasData->dummyToolLabel = new QLabel(toolWidget);
            layout->addWidget(canvasData->dummyToolLabel);
            layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
            toolWidget->setLayout(layout);
            canvasData->dummyToolWidget = toolWidget;
        }
        canvasData->dummyToolLabel->setText(i18n("Active tool: %1", title));
        optionWidgetList.append(toolWidget);
    }

    // Activate the actions for the currently active tool
    canvasData->activateToolActions();

    Q_EMIT q->changedTool(canvasData->canvas, uniqueToolIds.value(canvasData->activeTool));

    KoCanvasControllerWidget *canvasControllerWidget = dynamic_cast<KoCanvasControllerWidget *>(canvasData->canvas);
    if (canvasControllerWidget) {
        canvasControllerWidget->setToolOptionWidgets(optionWidgetList);
    }
}

void KoToolManager::Private::switchCanvasData(CanvasData *cd)
{
    Q_ASSERT(cd);

    KoCanvasBase *oldCanvas = nullptr;
    KoInputDevice oldInputDevice;

    if (canvasData) {
        oldCanvas = canvasData->canvas->canvas();
        oldInputDevice = canvasData->inputDevice;

        if (canvasData->activeTool) {
            disconnectActiveTool();
        }

        KoToolProxy *proxy = proxies.value(oldCanvas);
        Q_ASSERT(proxy);
        proxy->setActiveTool(nullptr);
    }

    canvasData = cd;
    inputDevice = canvasData->inputDevice;

    if (canvasData->activeTool) {
        connectActiveTool();
        postSwitchTool(false);
    }

    if (oldInputDevice != canvasData->inputDevice) {
        Q_EMIT q->inputDeviceChanged(canvasData->inputDevice);
    }

    if (oldCanvas != canvasData->canvas->canvas()) {
        Q_EMIT q->changedCanvas(canvasData->canvas->canvas());
    }
}

void KoToolManager::Private::toolActivated(ToolHelper *tool)
{
    Q_ASSERT(tool);

    Q_ASSERT(canvasData);
    if (!canvasData)
        return;
    KoToolBase *t = canvasData->allTools.value(tool->id());
    Q_ASSERT(t);

    canvasData->activeToolId = tool->id();
    canvasData->activationShapeId = tool->activationShapeId();

    switchTool(t, false);
}

void KoToolManager::Private::detachCanvas(KoCanvasController *controller)
{
    Q_ASSERT(controller);
    // check if we are removing the active canvas controller
    if (canvasData && canvasData->canvas == controller) {
        KoCanvasController *newCanvas = nullptr;
        // try to find another canvas controller beside the one we are removing
        foreach (KoCanvasController *canvas, canvasses.keys()) {
            if (canvas != controller) {
                // yay found one
                newCanvas = canvas;
                break;
            }
        }
        if (newCanvas) {
            switchCanvasData(canvasses.value(newCanvas).first());
        } else {
            KoCanvasControllerWidget *canvasControllerWidget = dynamic_cast<KoCanvasControllerWidget *>(canvasData->canvas);
            if (canvasControllerWidget) {
                canvasControllerWidget->setToolOptionWidgets(QList<QPointer<QWidget>>());
            }
            // as a last resort just set a blank one
            canvasData = nullptr;
        }
    }

    KoToolProxy *proxy = proxies.value(controller->canvas());
    if (proxy)
        proxy->setActiveTool(nullptr);

    QList<KoToolBase *> tools;
    foreach (CanvasData *canvasData, canvasses.value(controller)) {
        foreach (KoToolBase *tool, canvasData->allTools) {
            if (!tools.contains(tool)) {
                tools.append(tool);
            }
        }
        delete canvasData;
    }
    foreach (KoToolBase *tool, tools) {
        uniqueToolIds.remove(tool);
        delete tool;
    }
    canvasses.remove(controller);
    Q_EMIT q->changedCanvas(canvasData ? canvasData->canvas->canvas() : nullptr);
}

void KoToolManager::Private::attachCanvas(KoCanvasController *controller)
{
    Q_ASSERT(controller);
    CanvasData *cd = createCanvasData(controller, KoInputDevice::mouse());

    // switch to new canvas as the active one.
    switchCanvasData(cd);

    inputDevice = cd->inputDevice;
    QList<CanvasData *> canvasses_;
    canvasses_.append(cd);
    canvasses[controller] = canvasses_;

    KoToolProxy *tp = proxies[controller->canvas()];
    if (tp)
        tp->priv()->setCanvasController(controller);

    if (cd->activeTool == nullptr) {
        // no active tool, so we activate the highest priority main tool
        int highestPriority = INT_MAX;
        ToolHelper *helper = nullptr;
        foreach (ToolHelper *th, tools) {
            if (th->toolType() == KoToolFactoryBase::mainToolType()) {
                if (th->priority() < highestPriority) {
                    highestPriority = qMin(highestPriority, th->priority());
                    helper = th;
                }
            }
        }
        if (helper)
            toolActivated(helper);
    }

    Connector *connector = new Connector(controller->canvas()->shapeManager());
    connect(connector, &Connector::selectionChanged, q, [this](const QList<KoShape *> &list) {
        selectionChanged(list);
    });

    connect(controller->canvas()->shapeManager()->selection(), &KoSelection::currentLayerChanged, q, [this](const KoShapeLayer *layer) {
        currentLayerChanged(layer);
    });

    Q_EMIT q->changedCanvas(canvasData ? canvasData->canvas->canvas() : nullptr);
}

void KoToolManager::Private::movedFocus(QWidget *from, QWidget *to)
{
    Q_UNUSED(from);
    // no canvas anyway or no focus set anyway?
    if (!canvasData || to == nullptr) {
        return;
    }

    // Check if this app is about QWidget-based KoCanvasControllerWidget canvasses
    // XXX: Focus handling for non-qwidget based canvases!
    KoCanvasControllerWidget *canvasControllerWidget = dynamic_cast<KoCanvasControllerWidget *>(canvasData->canvas);
    if (!canvasControllerWidget) {
        return;
    }

    // canvasWidget is set as focusproxy for KoCanvasControllerWidget,
    // so all focus checks are to be done against canvasWidget objects

    // focus returned to current canvas?
    if (to == canvasData->canvas->canvas()->canvasWidget()) {
        // nothing to do
        return;
    }

    // if the 'to' is one of our canvasWidgets, then switch.

    // for code simplicity the current canvas will be checked again,
    // but would have been caught already in the lines above, so no issue
    KoCanvasController *newCanvas = nullptr;
    foreach (KoCanvasController *canvas, canvasses.keys()) {
        if (canvas->canvas()->canvasWidget() == to) {
            newCanvas = canvas;
            break;
        }
    }

    // none of our canvasWidgets got focus?
    if (newCanvas == nullptr) {
        return;
    }

    // switch to canvasdata matching inputdevice used last with this app instance
    foreach (CanvasData *data, canvasses.value(newCanvas)) {
        if (data->inputDevice == inputDevice) {
            switchCanvasData(data);
            return;
        }
    }
    // if no such inputDevice for this canvas, then simply fallback to first one
    switchCanvasData(canvasses.value(newCanvas).first());
}

void KoToolManager::Private::updateCursor(const QCursor &cursor)
{
    Q_ASSERT(canvasData);
    Q_ASSERT(canvasData->canvas);
    Q_ASSERT(canvasData->canvas->canvas());
    canvasData->canvas->canvas()->setCursor(cursor);
}

void KoToolManager::Private::selectionChanged(const QList<KoShape *> &shapes)
{
    QList<QString> types;
    foreach (KoShape *shape, shapes) {
        QSet<KoShape *> delegates = shape->toolDelegates();
        if (delegates.isEmpty()) { // no delegates, just the orig shape
            delegates << shape;
        }

        foreach (KoShape *shape2, delegates) {
            Q_ASSERT(shape2);
            if (!types.contains(shape2->shapeId())) {
                types.append(shape2->shapeId());
            }
        }
    }

    // check if there is still a shape selected the active tool can work on
    // there needs to be at least one shape for a tool without an activationShapeId
    // to work
    // if not change the current tool to the default tool
    if (!(canvasData->activationShapeId.isNull() && shapes.size() > 0) && canvasData->activationShapeId != "flake/always"
        && canvasData->activationShapeId != "flake/edit") {
        bool currentToolWorks = false;
        foreach (const QString &type, types) {
            if (canvasData->activationShapeId.split(',').contains(type)) {
                currentToolWorks = true;
                break;
            }
        }
        if (!currentToolWorks) {
            switchTool(KoInteractionTool_ID, false);
        }
    }
    Q_EMIT q->toolCodesSelected(types);
    // First time the tool is activated, it is not shown
    // because activetool must be set before optionwidgets are set.
    // Activetool is not set until q->toolCodesSelected() is emitted above,
    // so the setting in postSwitchTool() does not work.
    // NOTE: May only be true for non-default tools like for chart, formula etc,
    // so do not remove the postSwitchTool() setting until you are absolutely certain.
    QList<QPointer<QWidget>> optionWidgetList = canvasData->activeTool->optionWidgets();
    KoCanvasControllerWidget *canvasControllerWidget = dynamic_cast<KoCanvasControllerWidget *>(canvasData->canvas);
    if (canvasControllerWidget && !optionWidgetList.isEmpty()) {
        canvasControllerWidget->setToolOptionWidgets(optionWidgetList);
    }
}

void KoToolManager::Private::currentLayerChanged(const KoShapeLayer *layer)
{
    Q_EMIT q->currentLayerChanged(canvasData->canvas, layer);
    layerExplicitlyDisabled = layer && !layer->isEditable();
    updateToolForProxy();

    debugFlake << "Layer changed to" << layer << "explicitly disabled:" << layerExplicitlyDisabled;
}

void KoToolManager::Private::updateToolForProxy()
{
    KoToolProxy *proxy = proxies.value(canvasData->canvas->canvas());
    if (!proxy)
        return;

    bool canUseTool = !layerExplicitlyDisabled || canvasData->activationShapeId.endsWith(QLatin1String("/always"));
    proxy->setActiveTool(canUseTool ? canvasData->activeTool : nullptr);
}

void KoToolManager::Private::switchInputDevice(const KoInputDevice &device)
{
    Q_ASSERT(canvasData);
    if (!canvasData)
        return;
    if (inputDevice == device)
        return;
    if (inputDevice.isMouse() && device.isMouse())
        return;
    if (device.isMouse() && !inputDevice.isMouse()) {
        // we never switch back to mouse from a tablet input device, so the user can use the
        // mouse to edit the settings for a tool activated by a tablet. See bugs
        // https://bugs.kde.org/show_bug.cgi?id=283130 and https://bugs.kde.org/show_bug.cgi?id=285501.
        // We do continue to switch between tablet devices, thought.
        return;
    }

    QList<CanvasData *> items = canvasses[canvasData->canvas];

    // disable all actions for all tools in the all canvasdata objects for this canvas.
    foreach (CanvasData *cd, items) {
        foreach (KoToolBase *tool, cd->allTools) {
            foreach (QAction *action, tool->actions()) {
                action->setEnabled(false);
            }
        }
    }

    // search for a canvasdata object for the current input device
    foreach (CanvasData *cd, items) {
        if (cd->inputDevice == device) {
            switchCanvasData(cd);

            if (!canvasData->activeTool) {
                switchTool(KoInteractionTool_ID, false);
            }

            return;
        }
    }

    // still here?  That means we need to create a new CanvasData instance with the current InputDevice.
    CanvasData *cd = createCanvasData(canvasData->canvas, device);
    // switch to new canvas as the active one.
    QString oldTool = canvasData->activeToolId;

    items.append(cd);
    canvasses[cd->canvas] = items;

    switchCanvasData(cd);

    q->switchToolRequested(oldTool);
}

void KoToolManager::Private::registerToolProxy(KoToolProxy *proxy, KoCanvasBase *canvas)
{
    proxies.insert(canvas, proxy);
    foreach (KoCanvasController *controller, canvasses.keys()) {
        if (controller->canvas() == canvas) {
            proxy->priv()->setCanvasController(controller);
            break;
        }
    }
}

void KoToolManager::Private::switchToolByShortcut(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && event->modifiers() == 0) {
        switchTool(KoPanTool_ID, true);
    } else if (event->key() == Qt::Key_Escape && event->modifiers() == 0) {
        switchTool(KoInteractionTool_ID, false);
    }
}

// ******** KoToolManager **********
KoToolManager::KoToolManager()
    : QObject()
    , d(new Private(this))
{
    connect(qApp, &QApplication::focusChanged, this, [this](QWidget *old, QWidget *now) {
        d->movedFocus(old, now);
    });
}

KoToolManager::~KoToolManager()
{
    delete d;
}

QList<KoToolAction *> KoToolManager::toolActionList() const
{
    QList<KoToolAction *> answer;
    answer.reserve(d->tools.count());
    foreach (ToolHelper *tool, d->tools) {
        if (tool->id() == KoCreateShapesTool_ID)
            continue; // don't show this one.
        answer.append(tool->toolAction());
    }
    return answer;
}

void KoToolManager::requestToolActivation(KoCanvasController *controller)
{
    if (d->canvasses.contains(controller)) {
        QString activeToolId = d->canvasses.value(controller).first()->activeToolId;
        foreach (ToolHelper *th, d->tools) {
            if (th->id() == activeToolId) {
                d->toolActivated(th);
                break;
            }
        }
    }
}

KoInputDevice KoToolManager::currentInputDevice() const
{
    return d->inputDevice;
}

void KoToolManager::registerTools(KActionCollection *ac, KoCanvasController *controller)
{
    Q_ASSERT(controller);
    Q_ASSERT(ac);

    d->setup();

    if (!d->canvasses.contains(controller)) {
        return;
    }

    // Actions available during the use of individual tools
    CanvasData *cd = d->canvasses.value(controller).first();
    foreach (KoToolBase *tool, cd->allTools) {
        QHash<QString, QAction *> actions = tool->actions();
        QHash<QString, QAction *>::const_iterator action(actions.constBegin());
        for (; action != actions.constEnd(); ++action) {
            if (!ac->action(action.key()))
                ac->addAction(action.key(), action.value());
        }
    }

    // Actions used to switch tools via shortcuts
    foreach (ToolHelper *th, d->tools) {
        if (ac->action(th->id())) {
            continue;
        }
        ShortcutToolAction *action = th->createShortcutToolAction(ac);
        ac->addAction(th->id(), action);
    }
}

void KoToolManager::addController(KoCanvasController *controller)
{
    Q_ASSERT(controller);
    if (d->canvasses.contains(controller))
        return;
    d->setup();
    d->attachCanvas(controller);
    connect(controller->proxyObject, &QObject::destroyed, this, &KoToolManager::attemptCanvasControllerRemoval);
    connect(controller->proxyObject, &KoCanvasControllerProxyObject::canvasRemoved, this, [this](KoCanvasController *c) {
        d->detachCanvas(c);
    });
    connect(controller->proxyObject, &KoCanvasControllerProxyObject::canvasSet, this, [this](KoCanvasController *c) {
        d->attachCanvas(c);
    });
}

void KoToolManager::removeCanvasController(KoCanvasController *controller)
{
    Q_ASSERT(controller);
    disconnect(controller->proxyObject, &KoCanvasControllerProxyObject::canvasRemoved, this, nullptr);
    disconnect(controller->proxyObject, &KoCanvasControllerProxyObject::canvasSet, this, nullptr);
    d->detachCanvas(controller);
}

void KoToolManager::attemptCanvasControllerRemoval(QObject *controller)
{
    KoCanvasControllerProxyObject *controllerActual = qobject_cast<KoCanvasControllerProxyObject *>(controller);
    if (controllerActual) {
        removeCanvasController(controllerActual->canvasController());
    }
}

void KoToolManager::updateShapeControllerBase(KoShapeBasedDocumentBase *shapeController, KoCanvasController *canvasController)
{
    if (!d->canvasses.contains(canvasController))
        return;

    QList<CanvasData *> canvasses = d->canvasses[canvasController];
    foreach (CanvasData *canvas, canvasses) {
        foreach (KoToolBase *tool, canvas->allTools) {
            tool->updateShapeController(shapeController);
        }
    }
}

void KoToolManager::switchToolRequested(const QString &id)
{
    Q_ASSERT(d->canvasData);
    if (!d->canvasData)
        return;

    while (!d->canvasData->stack.isEmpty()) // switching means to flush the stack
        d->canvasData->stack.pop();
    d->switchTool(id, false);
}

void KoToolManager::switchInputDeviceRequested(const KoInputDevice &id)
{
    if (!d->canvasData)
        return;
    d->switchInputDevice(id);
}

void KoToolManager::switchToolTemporaryRequested(const QString &id)
{
    d->switchTool(id, true);
}

void KoToolManager::switchBackRequested()
{
    if (!d->canvasData)
        return;

    if (d->canvasData->stack.isEmpty()) {
        // default to changing to the interactionTool
        d->switchTool(KoInteractionTool_ID, false);
        return;
    }
    d->switchTool(d->canvasData->stack.pop(), false);
}

KoCreateShapesTool *KoToolManager::shapeCreatorTool(KoCanvasBase *canvas) const
{
    Q_ASSERT(canvas);
    foreach (KoCanvasController *controller, d->canvasses.keys()) {
        if (controller->canvas() == canvas) {
            KoCreateShapesTool *createTool = dynamic_cast<KoCreateShapesTool *>(d->canvasData->allTools.value(KoCreateShapesTool_ID));
            Q_ASSERT(createTool /* ID changed? */);
            return createTool;
        }
    }
    Q_ASSERT(0); // this should not happen
    return nullptr;
}

KoToolBase *KoToolManager::toolById(KoCanvasBase *canvas, const QString &id) const
{
    Q_ASSERT(canvas);
    foreach (KoCanvasController *controller, d->canvasses.keys()) {
        if (controller->canvas() == canvas)
            return d->canvasData->allTools.value(id);
    }
    return nullptr;
}

KoCanvasController *KoToolManager::activeCanvasController() const
{
    if (!d->canvasData)
        return nullptr;
    return d->canvasData->canvas;
}

QString KoToolManager::preferredToolForSelection(const QList<KoShape *> &shapes)
{
    QList<QString> types;
    foreach (KoShape *shape, shapes)
        if (!types.contains(shape->shapeId()))
            types.append(shape->shapeId());

    QString toolType = KoInteractionTool_ID;
    int prio = INT_MAX;
    foreach (ToolHelper *helper, d->tools) {
        if (helper->priority() >= prio)
            continue;
        if (helper->toolType() == KoToolFactoryBase::mainToolType())
            continue;

        bool toolWillWork = false;
        foreach (const QString &type, types) {
            if (helper->activationShapeId().split(',').contains(type)) {
                toolWillWork = true;
                break;
            }
        }
        if (toolWillWork) {
            toolType = helper->id();
            prio = helper->priority();
        }
    }
    return toolType;
}

void KoToolManager::injectDeviceEvent(KoInputDeviceHandlerEvent *event)
{
    if (d->canvasData && d->canvasData->canvas->canvas()) {
        if (static_cast<KoInputDeviceHandlerEvent::Type>(event->type()) == KoInputDeviceHandlerEvent::ButtonPressed)
            d->canvasData->activeTool->customPressEvent(event->pointerEvent());
        else if (static_cast<KoInputDeviceHandlerEvent::Type>(event->type()) == KoInputDeviceHandlerEvent::ButtonReleased)
            d->canvasData->activeTool->customReleaseEvent(event->pointerEvent());
        else if (static_cast<KoInputDeviceHandlerEvent::Type>(event->type()) == KoInputDeviceHandlerEvent::PositionChanged)
            d->canvasData->activeTool->customMoveEvent(event->pointerEvent());
    }
}

void KoToolManager::addDeferredToolFactory(KoToolFactoryBase *toolFactory)
{
    ToolHelper *tool = new ToolHelper(toolFactory);
    // make sure all plugins are loaded as otherwise we will not load them
    d->setup();
    d->tools.append(tool);

    // connect to all tools so we can hear their button-clicks
    connect(tool, &ToolHelper::toolActivated, this, [this](ToolHelper *helper) {
        d->toolActivated(helper);
    });

    // now create tools for all existing canvases
    foreach (KoCanvasController *controller, d->canvasses.keys()) {
        // this canvascontroller is unknown, which is weird
        if (!d->canvasses.contains(controller)) {
            continue;
        }

        // create a tool for all canvasdata objects (i.e., all input devices on this canvas)
        foreach (CanvasData *cd, d->canvasses[controller]) {
            QPair<QString, KoToolBase *> toolPair = createTools(controller, tool);
            if (toolPair.second) {
                cd->allTools.insert(toolPair.first, toolPair.second);
            }
        }

        // Then create a button for the toolbox for this canvas
        if (tool->id() == KoCreateShapesTool_ID) {
            continue;
        }

        Q_EMIT addedTool(tool->toolAction(), controller);
    }
}

QPair<QString, KoToolBase *> KoToolManager::createTools(KoCanvasController *controller, ToolHelper *tool)
{
    // XXX: maybe this method should go into the private class?

    QHash<QString, KoToolBase *> origHash;

    if (d->canvasses.contains(controller)) {
        origHash = d->canvasses.value(controller).first()->allTools;
    }

    if (origHash.contains(tool->id())) {
        return QPair<QString, KoToolBase *>(tool->id(), origHash.value(tool->id()));
    }

    debugFlake << "Creating tool" << tool->id() << ". Activated on:" << tool->activationShapeId() << ", prio:" << tool->priority();

    KoToolBase *tl = tool->createTool(controller->canvas());
    if (tl) {
        d->uniqueToolIds.insert(tl, tool->uniqueId());

        tl->setObjectName(tool->id());

        foreach (QAction *action, tl->actions()) {
            action->setEnabled(false);
        }
    }

    KoZoomTool *zoomTool = dynamic_cast<KoZoomTool *>(tl);
    if (zoomTool) {
        zoomTool->setCanvasController(controller);
    }

    KoPanTool *panTool = dynamic_cast<KoPanTool *>(tl);
    if (panTool) {
        panTool->setCanvasController(controller);
    }

    return QPair<QString, KoToolBase *>(tool->id(), tl);
}

KoToolManager *KoToolManager::instance()
{
    return s_instance;
}

QString KoToolManager::activeToolId() const
{
    if (!d->canvasData)
        return QString();
    return d->canvasData->activeToolId;
}

KoToolManager::Private *KoToolManager::priv()
{
    return d;
}

// have to include this because of Q_PRIVATE_SLOT
#include "moc_KoToolManager.cpp"
