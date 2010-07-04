/* This file is part of the KDE project
 * Copyright (c) 2009-2010 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonFilterEffectsTool.h"

#include "KoGenericRegistryModel.h"
#include "KoFilterEffect.h"
#include "KoFilterEffectStack.h"
#include "KoFilterEffectFactoryBase.h"
#include "KoFilterEffectRegistry.h"
#include "KoFilterEffectConfigWidgetBase.h"
#include "KoCanvasBase.h"
#include "KoResourceManager.h"
#include "KoShapeManager.h"
#include "KoSelection.h"
#include "FilterEffectEditWidget.h"
#include "FilterEffectResource.h"
#include "FilterResourceServerProvider.h"
#include "FilterStackSetCommand.h"
#include "FilterRegionChangeCommand.h"
#include "FilterRegionEditStrategy.h"
#include "KoResourceServerAdapter.h"
#include "KoResourceSelector.h"
#include <KoPointerEvent.h>

#include <KComboBox>
#include <KLocale>
#include <KIcon>
#include <KDialog>
#include <KNumInput>

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QToolButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QLabel>

class KarbonFilterEffectsTool::Private
{
public:
    Private()
            : filterSelector(0), configSelector(0)
            , configStack(0), posX(0), posY(0), posW(0), posH(0)
            , currentEffect(0), currentPanel(0), currentShape(0)
    {
    }

    void fillConfigSelector(KoShape *shape, KarbonFilterEffectsTool * tool)
    {
        if (!configSelector)
            return;

        configSelector->clear();

        if (!shape || !shape->filterEffectStack()) {
            addWidgetForEffect(0, tool);
            return;
        }

        configSelector->blockSignals(true);

        int index = 0;
        foreach(KoFilterEffect *effect, shape->filterEffectStack()->filterEffects()) {
            configSelector->addItem(QString("%1 - ").arg(index) + effect->name());
            index++;
        }

        configSelector->blockSignals(false);

        KoFilterEffect * effect = index > 0 ? shape->filterEffectStack()->filterEffects().first() : 0;

        addWidgetForEffect(effect, tool);
    }

    void addWidgetForEffect(KoFilterEffect * filterEffect, KarbonFilterEffectsTool * tool)
    {
        // remove current widget if new effect is zero or effect type has changed
        if (!filterEffect || (currentEffect && filterEffect->id() != currentEffect->id())) {
            while (configStack->count())
                configStack->removeWidget(configStack->widget(0));
        }

        if (! filterEffect) {
            currentEffect = 0;
            currentPanel = 0;
        } else if (!currentEffect || currentEffect->id() != filterEffect->id()) {
            // when a effect is set and is differs from the previous one
            // get the config widget and insert it into the option widget
            currentEffect = filterEffect;

            KoFilterEffectRegistry * registry = KoFilterEffectRegistry::instance();
            KoFilterEffectFactoryBase * factory = registry->value(currentEffect->id());
            if (!factory)
                return;

            currentPanel = factory->createConfigWidget();
            if (! currentPanel)
                return;

            currentPanel->layout()->setContentsMargins(0, 0, 0, 0);
            configStack->insertWidget(0, currentPanel);
            configStack->layout()->setContentsMargins(0, 0, 0, 0);
            connect(currentPanel, SIGNAL(filterChanged()), tool, SLOT(filterChanged()));
        }

        if (currentPanel)
            currentPanel->editFilterEffect(filterEffect);

        updateFilterRegion();
    }

    void updateFilterRegion()
    {
        QRectF region = currentEffect ? currentEffect->filterRect() : QRectF(0, 0, 0, 0);

        posX->blockSignals(true);
        posX->setValue(100.0*region.x());
        posX->blockSignals(false);
        posX->setEnabled(currentEffect != 0);
        posY->blockSignals(true);
        posY->setValue(100.0*region.y());
        posY->blockSignals(false);
        posY->setEnabled(currentEffect != 0);
        posW->blockSignals(true);
        posW->setValue(100.0*region.width());
        posW->blockSignals(false);
        posW->setEnabled(currentEffect != 0);
        posH->blockSignals(true);
        posH->setValue(100.0*region.height());
        posH->blockSignals(false);
        posH->setEnabled(currentEffect != 0);
    }

    EditMode editModeFromMousePosition(const QPointF &mousePosition, KarbonFilterEffectsTool * tool)
    {
        if (currentShape && currentShape->filterEffectStack() && currentEffect) {
            // get the size rect of the shape
            QRectF sizeRect(QPointF(), currentShape->size());
            // get the filter rectangle in shape coordinates
            QRectF filterRect = currentEffect->filterRectForBoundingRect(sizeRect);
            // get the transformation from document to shape coordinates
            QTransform transform = currentShape->absoluteTransformation(0).inverted();
            // adjust filter rectangle by grab sensitivity
            int grabSensitivity = tool->canvas()->resourceManager()->grabSensitivity();
            QPointF border = tool->canvas()->viewConverter()->viewToDocument(QPointF(grabSensitivity, grabSensitivity));
            filterRect.adjust(-border.x(), -border.y(), border.x(), border.y());
            // map event point from document to shape coordinates
            QPointF shapePoint = transform.map(mousePosition);
            // check if the mouse is inside/near our filter rect
            if (filterRect.contains(shapePoint)) {
                if (qAbs(shapePoint.x() - filterRect.left()) <= border.x()) {
                    return MoveLeft;
                } else if (qAbs(shapePoint.x() - filterRect.right()) <= border.x()) {
                    return MoveRight;
                } else if (qAbs(shapePoint.y() - filterRect.top()) <= border.y()) {
                   return MoveTop;
                } else if (qAbs(shapePoint.y() - filterRect.bottom()) <= border.y()) {
                    return MoveBottom;
                } else {
                    return MoveAll;
                }
            } else {
                return None;
            }
        }
        return None;
    }

    KoResourceSelector * filterSelector;
    KComboBox * configSelector;
    QStackedWidget * configStack;
    KDoubleNumInput * posX;
    KDoubleNumInput * posY;
    KDoubleNumInput * posW;
    KDoubleNumInput * posH;
    KoFilterEffect * currentEffect;
    KoFilterEffectConfigWidgetBase * currentPanel;
    KoShape * currentShape;
};

KarbonFilterEffectsTool::KarbonFilterEffectsTool(KoCanvasBase *canvas)
        : KoInteractionTool(canvas), d(new Private())
{
    connect(canvas->shapeManager(), SIGNAL(selectionChanged()),
            this, SLOT(selectionChanged()));
    connect(canvas->shapeManager(), SIGNAL(selectionContentChanged()),
            this, SLOT(selectionChanged()));
}

KarbonFilterEffectsTool::~KarbonFilterEffectsTool()
{
    delete d;
}

void KarbonFilterEffectsTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (d->currentShape && d->currentShape->filterEffectStack()) {
        painter.save();
        // apply the shape transformation
        QTransform transform = d->currentShape->absoluteTransformation(&converter);
        painter.setTransform(transform, true);
        // apply the zoom transformation
        KoShape::applyConversion(painter, converter);
        // get the size rect of the shape
        QRectF sizeRect(QPointF(), d->currentShape->size());
        // get the clipping rect of the filter stack
        KoFilterEffectStack * filterStack = d->currentShape->filterEffectStack();
        QRectF clipRect = filterStack->clipRectForBoundingRect(sizeRect);
        // finally paint the clipping rect
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::blue);
        painter.drawRect(clipRect);

        if (currentStrategy()) {
            currentStrategy()->paint(painter, converter);
        } else if (d->currentEffect) {
            QRectF filterRect = d->currentEffect->filterRectForBoundingRect(sizeRect);
            // paint the filter subregion rect
            painter.setBrush(Qt::NoBrush);
            painter.setPen(Qt::red);
            painter.drawRect(filterRect);
        }

        painter.restore();
    }
}

void KarbonFilterEffectsTool::repaintDecorations()
{
    if (d->currentShape && d->currentShape->filterEffectStack()) {
        QRectF bb = d->currentShape->boundingRect();
        int radius = canvas()->resourceManager()->handleRadius();
        canvas()->updateCanvas(bb.adjusted(-radius, -radius, radius, radius));
    }
}

void KarbonFilterEffectsTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);
    if (shapes.isEmpty()) {
        emit done();
        return;
    }

    d->currentShape = canvas()->shapeManager()->selection()->firstSelectedShape(KoFlake::TopLevelSelection);
    d->fillConfigSelector(d->currentShape, this);
}

void KarbonFilterEffectsTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (currentStrategy()) {
        KoInteractionTool::mouseMoveEvent(event);
    } else {
        EditMode mode = d->editModeFromMousePosition(event->point, this);
        switch(mode) {
            case MoveAll:
                useCursor(Qt::SizeAllCursor);
                break;
            case MoveLeft:
            case MoveRight:
                useCursor(Qt::SizeHorCursor);
                break;
            case MoveTop:
            case MoveBottom:
                useCursor(Qt::SizeVerCursor);
                break;
            case None:
                useCursor(Qt::ArrowCursor);
                break;
        }
    }
}

KoInteractionStrategy *KarbonFilterEffectsTool::createStrategy(KoPointerEvent *event)
{
    EditMode mode = d->editModeFromMousePosition(event->point, this);
    if (mode == None)
        return 0;

    return new FilterRegionEditStrategy(this, d->currentShape, d->currentEffect, mode);
}

void KarbonFilterEffectsTool::presetSelected(KoResource *resource)
{
    if (!d->currentShape)
        return;

    FilterEffectResource * effectResource = dynamic_cast<FilterEffectResource*>(resource);
    if (!effectResource)
        return;

    KoFilterEffectStack * filterStack = effectResource->toFilterStack();
    if (!filterStack)
        return;

    canvas()->addCommand(new FilterStackSetCommand(filterStack, d->currentShape));
    d->fillConfigSelector(d->currentShape, this);
}

void KarbonFilterEffectsTool::editFilter()
{
    QPointer<KDialog> dlg = new KDialog();
    dlg->setCaption(i18n("Filter Effect Editor"));
    dlg->setButtons(KDialog::Close);

    FilterEffectEditWidget * editor = new FilterEffectEditWidget(dlg);
    editor->editShape(d->currentShape, canvas());

    dlg->setMainWidget(editor);
    dlg->exec();
    delete dlg;

    d->fillConfigSelector(d->currentShape, this);
}

void KarbonFilterEffectsTool::filterChanged()
{
    if (! d->currentShape)
        return;

    d->currentShape->update();
}

void KarbonFilterEffectsTool::filterSelected(int index)
{
    if (! d->currentShape || ! d->currentShape->filterEffectStack())
        return;

    KoFilterEffect * effect = 0;
    QList<KoFilterEffect*> filterEffects = d->currentShape->filterEffectStack()->filterEffects();
    if (index >= 0 && index < filterEffects.count()) {
        effect = filterEffects[index];
    }

    d->addWidgetForEffect(effect, this);

    repaintDecorations();
}

void KarbonFilterEffectsTool::selectionChanged()
{
    d->currentShape = canvas()->shapeManager()->selection()->firstSelectedShape(KoFlake::TopLevelSelection);
    d->fillConfigSelector(d->currentShape, this);
}

void KarbonFilterEffectsTool::regionXChanged(double x)
{
    if (!d->currentEffect)
        return;

    QRectF region = d->currentEffect->filterRect();
    region.setX(x / 100.0);
    canvas()->addCommand(new FilterRegionChangeCommand(d->currentEffect, region, d->currentShape));
}

void KarbonFilterEffectsTool::regionYChanged(double y)
{
    if (!d->currentEffect)
        return;

    QRectF region = d->currentEffect->filterRect();
    region.setY(y / 100.0);
    canvas()->addCommand(new FilterRegionChangeCommand(d->currentEffect, region, d->currentShape));
}

void KarbonFilterEffectsTool::regionWidthChanged(double width)
{
    if (!d->currentEffect)
        return;

    QRectF region = d->currentEffect->filterRect();
    region.setWidth(width / 100.0);
    canvas()->addCommand(new FilterRegionChangeCommand(d->currentEffect, region, d->currentShape));
}

void KarbonFilterEffectsTool::regionHeightChanged(double height)
{
    if (!d->currentEffect)
        return;

    QRectF region = d->currentEffect->filterRect();
    region.setHeight(height / 100.0);
    canvas()->addCommand(new FilterRegionChangeCommand(d->currentEffect, region, d->currentShape));
}

QMap<QString, QWidget *> KarbonFilterEffectsTool::createOptionWidgets()
{
    QMap<QString, QWidget*> widgets;

    FilterResourceServerProvider * serverProvider = FilterResourceServerProvider::instance();
    KoResourceServer<FilterEffectResource> * server = serverProvider->filterEffectServer();
    KoAbstractResourceServerAdapter * adapter = new KoResourceServerAdapter<FilterEffectResource>(server, this);

    //---------------------------------------------------------------------

    QWidget * addFilterWidget = new QWidget();
    addFilterWidget->setObjectName("AddEffect");
    QGridLayout * addFilterLayout = new QGridLayout(addFilterWidget);

    d->filterSelector = new KoResourceSelector(addFilterWidget);
    d->filterSelector->setResourceAdapter(adapter);
    d->filterSelector->setDisplayMode(KoResourceSelector::TextMode);
    d->filterSelector->setColumnCount(1);
    addFilterLayout->addWidget(new QLabel(i18n("Effects"), addFilterWidget), 0, 0);
    addFilterLayout->addWidget(d->filterSelector, 0, 1);
    connect(d->filterSelector, SIGNAL(resourceSelected(KoResource*)),
            this, SLOT(presetSelected(KoResource*)));

    connect(d->filterSelector, SIGNAL(resourceApplied(KoResource*)),
            this, SLOT(presetSelected(KoResource*)));

    QToolButton * editButton = new QToolButton(addFilterWidget);
    editButton->setIcon(KIcon("view-filter"));
    editButton->setToolTip(i18n("View and edit filter"));
    addFilterLayout->addWidget(editButton, 0, 2);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editFilter()));

    widgets.insert(i18n("Add Filter"), addFilterWidget);

    //---------------------------------------------------------------------

    QWidget * configFilterWidget = new QWidget();
    configFilterWidget->setObjectName("ConfigEffect");
    QGridLayout * configFilterLayout = new QGridLayout(configFilterWidget);

    d->configSelector = new KComboBox(configFilterWidget);
    configFilterLayout->addWidget(d->configSelector, 0, 0);
    connect(d->configSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(filterSelected(int)));

    d->configStack = new QStackedWidget(configFilterWidget);
    configFilterLayout->addWidget(d->configStack, 1, 0);
    configFilterLayout->setContentsMargins(0, 0, 0, 0);

    widgets.insert(i18n("Effect Properties"), configFilterWidget);

    //---------------------------------------------------------------------

    QWidget * filterRegionWidget = new QWidget();
    filterRegionWidget->setObjectName("EffectRegion");
    QGridLayout * filterRegionLayout = new QGridLayout(filterRegionWidget);

    d->posX = new KDoubleNumInput(filterRegionWidget);
    d->posX->setSuffix("%");
    connect(d->posX, SIGNAL(valueChanged(double)), this, SLOT(regionXChanged(double)));
    filterRegionLayout->addWidget(new QLabel(i18n("X:")), 0, 0);
    filterRegionLayout->addWidget(d->posX, 0, 1);

    d->posY = new KDoubleNumInput(filterRegionWidget);
    d->posY->setSuffix("%");
    connect(d->posY, SIGNAL(valueChanged(double)), this, SLOT(regionYChanged(double)));
    filterRegionLayout->addWidget(new QLabel(i18n("Y:")), 1, 0);
    filterRegionLayout->addWidget(d->posY, 1, 1);

    d->posW = new KDoubleNumInput(filterRegionWidget);
    d->posW->setSuffix("%");
    connect(d->posW, SIGNAL(valueChanged(double)), this, SLOT(regionWidthChanged(double)));
    filterRegionLayout->addWidget(new QLabel(i18n("W:")), 0, 2);
    filterRegionLayout->addWidget(d->posW, 0, 3);

    d->posH = new KDoubleNumInput(filterRegionWidget);
    d->posH->setSuffix("%");
    connect(d->posH, SIGNAL(valueChanged(double)), this, SLOT(regionHeightChanged(double)));
    filterRegionLayout->addWidget(new QLabel(i18n("H:")), 1, 2);
    filterRegionLayout->addWidget(d->posH, 1, 3);
    filterRegionLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 0);

    filterRegionLayout->setContentsMargins(0, 0, 0, 0);

    widgets.insert(i18n("Effect Region"), filterRegionWidget);

    //---------------------------------------------------------------------

    d->fillConfigSelector(d->currentShape, this);

    return widgets;
}

#include "KarbonFilterEffectsTool.moc"
