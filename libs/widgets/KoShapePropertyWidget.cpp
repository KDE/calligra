// SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KoShapePropertyWidget.h"

#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoParameterShape.h>
#include <KoPathShape.h>
#include <KoSelection.h>
#include <KoShapeConfigWidgetBase.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeManager.h>
#include <KoShapeRegistry.h>
#include <KoUnit.h>

#include <KLocalizedString>

#include <QLabel>
#include <QPointer>
#include <QStackedWidget>
#include <QVBoxLayout>

class Q_DECL_HIDDEN KoShapePropertyWidget::Private
{
public:
    Private(KoShapePropertyWidget *widget)
        : q(widget)
    {
    }

    QList<QPointer<QWidget>> currentWidgetList;
    QSet<QWidget *> currentAuxWidgets;
    KoShapePropertyWidget *const q;
    KoCanvasBase *canvas = nullptr;
    KoShape *currentShape = nullptr;
    KoShapeConfigWidgetBase *currentPanel = nullptr;
    QVBoxLayout *mainLayout = nullptr;
};

KoShapePropertyWidget::KoShapePropertyWidget(QWidget *parent)
    : QWidget(parent)
    , d(std::make_unique<Private>(this))
{
    setWindowTitle(i18nc("@title:group", "Shape Properties"));
    setObjectName("ShapePropertyWidget");
    d->mainLayout = new QVBoxLayout(this);
    d->mainLayout->setContentsMargins({});
    addPlaceholder();
}

KoShapePropertyWidget::~KoShapePropertyWidget() = default;

void KoShapePropertyWidget::addPlaceholder()
{
    auto label = new QLabel(i18nc("@info", "No properties available"));
    label->setEnabled(false);
    label->setContentsMargins(0, 0, 0, 6);
    d->mainLayout->addWidget(label);
};

void KoShapePropertyWidget::setCanvas(KoCanvasBase *canvas)
{
    if (d->canvas) {
        d->canvas->disconnectCanvasObserver(this); // "Every connection you make emits a signal, so duplicate connections emit two signals"
    }

    d->canvas = canvas;

    if (d->canvas) {
        connect(d->canvas->shapeManager(), &KoShapeManager::selectionChanged, this, &KoShapePropertyWidget::selectionChanged);
        connect(d->canvas->shapeManager(), &KoShapeManager::selectionContentChanged, this, &KoShapePropertyWidget::selectionChanged);
        connect(d->canvas->resourceManager(), &KoCanvasResourceManager::canvasResourceChanged, this, &KoShapePropertyWidget::canvasResourceChanged);
    }
}

void KoShapePropertyWidget::selectionChanged()
{
    if (!d->canvas)
        return;

    KoSelection *selection = d->canvas->shapeManager()->selection();
    if (selection->count() == 1)
        addWidgetForShape(selection->firstSelectedShape());
    else
        addWidgetForShape(nullptr);
}

void KoShapePropertyWidget::addWidgetForShape(KoShape *shape)
{
    // remove the config widget if a null shape is set, or the shape has changed
    if (!shape || shape != d->currentShape) {
        while (d->mainLayout->count()) {
            auto item = d->mainLayout->itemAt(0);
            d->mainLayout->removeItem(item);
            delete item->widget();
            delete item;
        }
    }

    if (!shape) {
        d->currentShape = nullptr;
        d->currentPanel = nullptr;
        addPlaceholder();
        return;
    } else if (shape != d->currentShape) {
        // when a shape is set and is differs from the previous one
        // get the config widget and insert it into the option widget
        d->currentShape = shape;
        if (!d->currentShape) {
            d->currentPanel = nullptr;
            addPlaceholder();
            return;
        }
        QString shapeId = shape->shapeId();
        KoPathShape *path = dynamic_cast<KoPathShape *>(shape);
        if (path) {
            // use the path specific shape id if shape is a path, otherwise use the shape id
            shapeId = path->pathShapeId();
            // check if we have an edited parametric shape, then we use the path shape id
            KoParameterShape *paramShape = dynamic_cast<KoParameterShape *>(shape);
            if (paramShape && !paramShape->isParametricShape())
                shapeId = shape->shapeId();
        }
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(shapeId);
        if (!factory) {
            addPlaceholder();
            return;
        }
        QList<KoShapeConfigWidgetBase *> panels = factory->createShapeOptionPanels();
        if (!panels.count()) {
            addPlaceholder();
            return;
        }

        d->currentPanel = nullptr;
        uint panelCount = panels.count();
        for (uint i = 0; i < panelCount; ++i) {
            if (panels[i]->showOnShapeSelect()) {
                d->currentPanel = panels[i];
                break;
            }
        }
        if (d->currentPanel) {
            if (d->canvas) {
                d->currentPanel->setUnit(d->canvas->unit());
            }
            d->mainLayout->addWidget(d->currentPanel);
            connect(d->currentPanel, &KoShapeConfigWidgetBase::propertyChanged, this, &KoShapePropertyWidget::shapePropertyChanged);
        } else {
            addPlaceholder();
        }
    }

    if (d->currentPanel) {
        d->currentPanel->open(shape);
    }
}

void KoShapePropertyWidget::shapePropertyChanged()
{
    if (d->canvas && d->currentPanel) {
        KUndo2Command *cmd = d->currentPanel->createCommand();
        if (!cmd)
            return;
        d->canvas->addCommand(cmd);
    }
}

void KoShapePropertyWidget::canvasResourceChanged(int key, const QVariant &variant)
{
    if (key == KoCanvasResourceManager::Unit && d->currentPanel)
        d->currentPanel->setUnit(variant.value<KoUnit>());
}
