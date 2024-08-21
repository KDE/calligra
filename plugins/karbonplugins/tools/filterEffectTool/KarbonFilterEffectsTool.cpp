/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009-2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KarbonFilterEffectsTool.h"

#include "FilterEffectEditWidget.h"
#include "FilterEffectResource.h"
#include "FilterRegionChangeCommand.h"
#include "FilterRegionEditStrategy.h"
#include "FilterResourceServerProvider.h"
#include "FilterStackSetCommand.h"
#include "KoCanvasBase.h"
#include "KoDocumentResourceManager.h"
#include "KoFilterEffect.h"
#include "KoFilterEffectConfigWidgetBase.h"
#include "KoFilterEffectFactoryBase.h"
#include "KoFilterEffectRegistry.h"
#include "KoFilterEffectStack.h"
#include "KoResourceSelector.h"
#include "KoResourceServerAdapter.h"
#include "KoSelection.h"
#include "KoShapeManager.h"
#include "KoViewConverter.h"
#include <KoPointerEvent.h>

#include <KoIcon.h>

#include <KComboBox>
#include <KLocalizedString>
#include <QDialog>
#include <QSpinBox>

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

class KarbonFilterEffectsTool::Private
{
public:
    Private()
        : filterSelector(nullptr)
        , configSelector(nullptr)
        , configStack(nullptr)
        , posX(nullptr)
        , posY(nullptr)
        , posW(nullptr)
        , posH(nullptr)
        , clearButton(nullptr)
        , currentEffect(nullptr)
        , currentPanel(nullptr)
        , currentShape(nullptr)
    {
    }

    void fillConfigSelector(KoShape *shape, KarbonFilterEffectsTool *tool)
    {
        if (!configSelector)
            return;

        configSelector->clear();
        clearButton->setEnabled(false);

        if (!shape || !shape->filterEffectStack()) {
            addWidgetForEffect(nullptr, tool);
            return;
        }

        configSelector->blockSignals(true);

        int index = 0;
        foreach (KoFilterEffect *effect, shape->filterEffectStack()->filterEffects()) {
            configSelector->addItem(QString("%1 - ").arg(index) + effect->name());
            index++;
        }

        configSelector->blockSignals(false);

        KoFilterEffect *effect = index > 0 ? shape->filterEffectStack()->filterEffects().first() : 0;

        addWidgetForEffect(effect, tool);
        clearButton->setEnabled(shape->filterEffectStack() != nullptr);
    }

    void addWidgetForEffect(KoFilterEffect *filterEffect, KarbonFilterEffectsTool *tool)
    {
        // remove current widget if new effect is zero or effect type has changed
        if (!filterEffect || (currentEffect && filterEffect->id() != currentEffect->id())) {
            while (configStack->count())
                configStack->removeWidget(configStack->widget(0));
        }

        if (!filterEffect) {
            currentEffect = nullptr;
            currentPanel = nullptr;
        } else if (!currentEffect || currentEffect->id() != filterEffect->id()) {
            // when a effect is set and is differs from the previous one
            // get the config widget and insert it into the option widget
            currentEffect = filterEffect;

            KoFilterEffectRegistry *registry = KoFilterEffectRegistry::instance();
            KoFilterEffectFactoryBase *factory = registry->value(currentEffect->id());
            if (!factory)
                return;

            currentPanel = factory->createConfigWidget();
            if (!currentPanel)
                return;

            currentPanel->layout()->setContentsMargins(0, 0, 0, 0);
            configStack->insertWidget(0, currentPanel);
            configStack->layout()->setContentsMargins(0, 0, 0, 0);
            connect(currentPanel, &KoFilterEffectConfigWidgetBase::filterChanged, tool, &KarbonFilterEffectsTool::filterChanged);
        }

        if (currentPanel)
            currentPanel->editFilterEffect(filterEffect);

        updateFilterRegion();
    }

    void updateFilterRegion()
    {
        QRectF region = currentEffect ? currentEffect->filterRect() : QRectF(0, 0, 0, 0);

        posX->blockSignals(true);
        posX->setValue(100.0 * region.x());
        posX->blockSignals(false);
        posX->setEnabled(currentEffect != nullptr);
        posY->blockSignals(true);
        posY->setValue(100.0 * region.y());
        posY->blockSignals(false);
        posY->setEnabled(currentEffect != nullptr);
        posW->blockSignals(true);
        posW->setValue(100.0 * region.width());
        posW->blockSignals(false);
        posW->setEnabled(currentEffect != nullptr);
        posH->blockSignals(true);
        posH->setValue(100.0 * region.height());
        posH->blockSignals(false);
        posH->setEnabled(currentEffect != nullptr);
    }

    EditMode editModeFromMousePosition(const QPointF &mousePosition, KarbonFilterEffectsTool *tool)
    {
        if (currentShape && currentShape->filterEffectStack() && currentEffect) {
            // get the size rect of the shape
            QRectF sizeRect(QPointF(), currentShape->size());
            // get the filter rectangle in shape coordinates
            QRectF filterRect = currentEffect->filterRectForBoundingRect(sizeRect);
            // get the transformation from document to shape coordinates
            QTransform transform = currentShape->absoluteTransformation(nullptr).inverted();
            // adjust filter rectangle by grab sensitivity
            const int grabDistance = tool->grabSensitivity();
            QPointF border = tool->canvas()->viewConverter()->viewToDocument(QPointF(grabDistance, grabDistance));
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

    KoResourceSelector *filterSelector;
    KComboBox *configSelector;
    QStackedWidget *configStack;
    QDoubleSpinBox *posX;
    QDoubleSpinBox *posY;
    QDoubleSpinBox *posW;
    QDoubleSpinBox *posH;
    QToolButton *clearButton;
    KoFilterEffect *currentEffect;
    KoFilterEffectConfigWidgetBase *currentPanel;
    KoShape *currentShape;
};

KarbonFilterEffectsTool::KarbonFilterEffectsTool(KoCanvasBase *canvas)
    : KoInteractionTool(canvas)
    , d(new Private())
{
    connect(canvas->shapeManager(), &KoShapeManager::selectionChanged, this, &KarbonFilterEffectsTool::slotSelectionChanged);
    connect(canvas->shapeManager(), &KoShapeManager::selectionContentChanged, this, &KarbonFilterEffectsTool::slotSelectionChanged);
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
        KoFilterEffectStack *filterStack = d->currentShape->filterEffectStack();
        QRectF clipRect = filterStack->clipRectForBoundingRect(sizeRect);
        // finally paint the clipping rect
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::blue, 0));
        painter.drawRect(clipRect);

        if (currentStrategy()) {
            currentStrategy()->paint(painter, converter);
        } else if (d->currentEffect) {
            QRectF filterRect = d->currentEffect->filterRectForBoundingRect(sizeRect);
            // paint the filter subregion rect
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(Qt::red, 0));
            painter.drawRect(filterRect);
        }

        painter.restore();
    }
}

void KarbonFilterEffectsTool::repaintDecorations()
{
    if (d->currentShape && d->currentShape->filterEffectStack()) {
        QRectF bb = d->currentShape->boundingRect();
        const int radius = handleRadius();
        canvas()->updateCanvas(bb.adjusted(-radius, -radius, radius, radius));
    }
}

void KarbonFilterEffectsTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    Q_UNUSED(toolActivation);
    if (shapes.isEmpty()) {
        Q_EMIT done();
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
        switch (mode) {
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
        return nullptr;

    return new FilterRegionEditStrategy(this, d->currentShape, d->currentEffect, mode);
}

void KarbonFilterEffectsTool::presetSelected(KoResource *resource)
{
    if (!d->currentShape)
        return;

    FilterEffectResource *effectResource = dynamic_cast<FilterEffectResource *>(resource);
    if (!effectResource)
        return;

    KoFilterEffectStack *filterStack = effectResource->toFilterStack();
    if (!filterStack)
        return;

    canvas()->addCommand(new FilterStackSetCommand(filterStack, d->currentShape));
    d->fillConfigSelector(d->currentShape, this);
}

void KarbonFilterEffectsTool::editFilter()
{
    QPointer<QDialog> dlg = new QDialog();
    dlg->setWindowTitle(i18n("Filter Effect Editor"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    QWidget *mainWidget = new QWidget(nullptr);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    dlg->setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    connect(buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, dlg.data(), &QDialog::close);

    FilterEffectEditWidget *editor = new FilterEffectEditWidget(dlg);
    editor->editShape(d->currentShape, canvas());

    mainLayout->addWidget(editor);
    mainLayout->addWidget(buttonBox);
    dlg->exec();
    delete dlg;

    d->fillConfigSelector(d->currentShape, this);
}

void KarbonFilterEffectsTool::clearFilter()
{
    if (!d->currentShape)
        return;
    if (!d->currentShape->filterEffectStack())
        return;

    canvas()->addCommand(new FilterStackSetCommand(nullptr, d->currentShape));

    d->fillConfigSelector(d->currentShape, this);
}

void KarbonFilterEffectsTool::filterChanged()
{
    if (!d->currentShape)
        return;

    d->currentShape->update();
}

void KarbonFilterEffectsTool::filterSelected(int index)
{
    if (!d->currentShape || !d->currentShape->filterEffectStack())
        return;

    KoFilterEffect *effect = nullptr;
    QList<KoFilterEffect *> filterEffects = d->currentShape->filterEffectStack()->filterEffects();
    if (index >= 0 && index < filterEffects.count()) {
        effect = filterEffects[index];
    }

    d->addWidgetForEffect(effect, this);

    repaintDecorations();
}

void KarbonFilterEffectsTool::slotSelectionChanged()
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

QList<QPointer<QWidget>> KarbonFilterEffectsTool::createOptionWidgets()
{
    QList<QPointer<QWidget>> widgets;

    FilterResourceServerProvider *serverProvider = FilterResourceServerProvider::instance();
    KoResourceServer<FilterEffectResource> *server = serverProvider->filterEffectServer();
    QSharedPointer<KoAbstractResourceServerAdapter> adapter(new KoResourceServerAdapter<FilterEffectResource>(server));

    //---------------------------------------------------------------------

    QWidget *addFilterWidget = new QWidget();
    addFilterWidget->setObjectName("AddEffect");
    QGridLayout *addFilterLayout = new QGridLayout(addFilterWidget);

    d->filterSelector = new KoResourceSelector(addFilterWidget);
    d->filterSelector->setResourceAdapter(adapter);
    d->filterSelector->setDisplayMode(KoResourceSelector::TextMode);
    d->filterSelector->setColumnCount(1);
    addFilterLayout->addWidget(new QLabel(i18n("Effects"), addFilterWidget), 0, 0);
    addFilterLayout->addWidget(d->filterSelector, 0, 1);
    connect(d->filterSelector, &KoResourceSelector::resourceSelected, this, &KarbonFilterEffectsTool::presetSelected);

    connect(d->filterSelector, &KoResourceSelector::resourceApplied, this, &KarbonFilterEffectsTool::presetSelected);

    QToolButton *editButton = new QToolButton(addFilterWidget);
    editButton->setIcon(koIcon("view-filter"));
    editButton->setToolTip(i18n("View and edit filter"));
    addFilterLayout->addWidget(editButton, 0, 2);
    connect(editButton, &QAbstractButton::clicked, this, &KarbonFilterEffectsTool::editFilter);

    d->clearButton = new QToolButton(addFilterWidget);
    d->clearButton->setIcon(koIcon("edit-delete"));
    d->clearButton->setToolTip(i18n("Remove filter from object"));
    addFilterLayout->addWidget(d->clearButton, 0, 3);
    connect(d->clearButton, &QAbstractButton::clicked, this, &KarbonFilterEffectsTool::clearFilter);

    addFilterWidget->setWindowTitle(i18n("Add Filter"));
    widgets.append(addFilterWidget);

    //---------------------------------------------------------------------

    QWidget *configFilterWidget = new QWidget();
    configFilterWidget->setObjectName("ConfigEffect");
    QGridLayout *configFilterLayout = new QGridLayout(configFilterWidget);

    d->configSelector = new KComboBox(configFilterWidget);
    configFilterLayout->addWidget(d->configSelector, 0, 0);
    connect(d->configSelector, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &KarbonFilterEffectsTool::filterSelected);

    d->configStack = new QStackedWidget(configFilterWidget);
    configFilterLayout->addWidget(d->configStack, 1, 0);
    configFilterLayout->setContentsMargins(0, 0, 0, 0);

    configFilterWidget->setWindowTitle(i18n("Effect Properties"));
    widgets.append(configFilterWidget);

    //---------------------------------------------------------------------

    QWidget *filterRegionWidget = new QWidget();
    filterRegionWidget->setObjectName("EffectRegion");
    QGridLayout *filterRegionLayout = new QGridLayout(filterRegionWidget);

    d->posX = new QDoubleSpinBox(filterRegionWidget);
    d->posX->setSuffix("%");
    connect(d->posX, &QDoubleSpinBox::valueChanged, this, &KarbonFilterEffectsTool::regionXChanged);
    filterRegionLayout->addWidget(new QLabel(i18n("X:")), 0, 0);
    filterRegionLayout->addWidget(d->posX, 0, 1);

    d->posY = new QDoubleSpinBox(filterRegionWidget);
    d->posY->setSuffix("%");
    connect(d->posY, &QDoubleSpinBox::valueChanged, this, &KarbonFilterEffectsTool::regionYChanged);
    filterRegionLayout->addWidget(new QLabel(i18n("Y:")), 1, 0);
    filterRegionLayout->addWidget(d->posY, 1, 1);

    d->posW = new QDoubleSpinBox(filterRegionWidget);
    d->posW->setSuffix("%");
    connect(d->posW, &QDoubleSpinBox::valueChanged, this, &KarbonFilterEffectsTool::regionWidthChanged);
    filterRegionLayout->addWidget(new QLabel(i18n("W:")), 0, 2);
    filterRegionLayout->addWidget(d->posW, 0, 3);

    d->posH = new QDoubleSpinBox(filterRegionWidget);
    d->posH->setSuffix("%");
    connect(d->posH, &QDoubleSpinBox::valueChanged, this, &KarbonFilterEffectsTool::regionHeightChanged);
    filterRegionLayout->addWidget(new QLabel(i18n("H:")), 1, 2);
    filterRegionLayout->addWidget(d->posH, 1, 3);
    filterRegionLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 0);
    filterRegionLayout->setContentsMargins(0, 0, 0, 0);

    filterRegionWidget->setWindowTitle(i18n("Effect Region"));
    widgets.append(filterRegionWidget);

    //---------------------------------------------------------------------

    d->fillConfigSelector(d->currentShape, this);

    return widgets;
}
