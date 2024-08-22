/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009, 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonPatternTool.h"
#include "KarbonPatternEditStrategy.h"
#include <KarbonPatternOptionsWidget.h>

#include <KoCanvasBase.h>
#include <KoDocumentResourceManager.h>
#include <KoImageCollection.h>
#include <KoPattern.h>
#include <KoPatternBackground.h>
#include <KoPointerEvent.h>
#include <KoResource.h>
#include <KoResourceItemChooser.h>
#include <KoResourceServerAdapter.h>
#include <KoResourceServerProvider.h>
#include <KoSelection.h>
#include <KoShape.h>
#include <KoShapeBackgroundCommand.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>

#include <KLocalizedString>

#include <QPainter>
#include <QWidget>
#include <kundo2command.h>

KarbonPatternTool::KarbonPatternTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_currentStrategy(nullptr)
    , m_optionsWidget(nullptr)
{
}

KarbonPatternTool::~KarbonPatternTool() = default;

void KarbonPatternTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    painter.setBrush(Qt::green); // TODO make configurable
    painter.setPen(QPen(Qt::blue, 0)); // TODO make configurable

    // paint all the strategies
    foreach (KarbonPatternEditStrategyBase *strategy, m_strategies) {
        if (strategy == m_currentStrategy)
            continue;

        painter.save();
        strategy->paint(painter, converter);
        painter.restore();
    }

    // paint selected strategy with another color
    if (m_currentStrategy) {
        painter.setBrush(Qt::red); // TODO make configurable
        m_currentStrategy->paint(painter, converter);
    }
}

void KarbonPatternTool::repaintDecorations()
{
    foreach (KarbonPatternEditStrategyBase *strategy, m_strategies)
        canvas()->updateCanvas(strategy->boundingRect());
}

void KarbonPatternTool::mousePressEvent(KoPointerEvent *event)
{
    // m_currentStrategy = 0;

    foreach (KarbonPatternEditStrategyBase *strategy, m_strategies) {
        if (strategy->selectHandle(event->point, *canvas()->viewConverter())) {
            m_currentStrategy = strategy;
            m_currentStrategy->repaint();
            useCursor(Qt::SizeAllCursor);
            break;
        }
    }
    if (m_currentStrategy) {
        m_currentStrategy->setEditing(true);
        updateOptionsWidget();
    }
}

void KarbonPatternTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (m_currentStrategy) {
        m_currentStrategy->repaint();
        if (m_currentStrategy->isEditing()) {
            m_currentStrategy->handleMouseMove(event->point, event->modifiers());
            m_currentStrategy->repaint();
            return;
        }
    }
    foreach (KarbonPatternEditStrategyBase *strategy, m_strategies) {
        if (strategy->selectHandle(event->point, *canvas()->viewConverter())) {
            useCursor(Qt::SizeAllCursor);
            return;
        }
    }
    useCursor(Qt::ArrowCursor);
}

void KarbonPatternTool::mouseReleaseEvent(KoPointerEvent *event)
{
    Q_UNUSED(event)
    // if we are editing, get out of edit mode and add a command to the stack
    if (m_currentStrategy && m_currentStrategy->isEditing()) {
        m_currentStrategy->setEditing(false);
        KUndo2Command *cmd = m_currentStrategy->createCommand();
        if (cmd)
            canvas()->addCommand(cmd);

        updateOptionsWidget();
    }
}

void KarbonPatternTool::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_I: {
        KoDocumentResourceManager *rm = canvas()->shapeController()->resourceManager();
        uint handleRadius = rm->handleRadius();
        if (event->modifiers() & Qt::ControlModifier)
            handleRadius--;
        else
            handleRadius++;
        rm->setHandleRadius(handleRadius);
    } break;
    default:
        event->ignore();
        return;
    }
    event->accept();
}

void KarbonPatternTool::initialize()
{
    if (m_currentStrategy && m_currentStrategy->isEditing())
        return;

    QList<KoShape *> selectedShapes = canvas()->shapeManager()->selection()->selectedShapes();

    // remove all pattern strategies no longer applicable
    foreach (KarbonPatternEditStrategyBase *strategy, m_strategies) {
        // is this gradient shape still selected ?
        if (!selectedShapes.contains(strategy->shape()) || !strategy->shape()->isEditable()) {
            m_strategies.remove(strategy->shape());
            if (m_currentStrategy == strategy)
                m_currentStrategy = nullptr;
            delete strategy;
            continue;
        }

        // does the shape has no fill pattern anymore ?
        QSharedPointer<KoPatternBackground> fill = qSharedPointerDynamicCast<KoPatternBackground>(strategy->shape()->background());
        if (!fill) {
            // delete the gradient
            m_strategies.remove(strategy->shape());
            if (m_currentStrategy == strategy)
                m_currentStrategy = nullptr;
            delete strategy;
            continue;
        }

        strategy->updateHandles();
        strategy->repaint();
    }

    KoImageCollection *imageCollection = canvas()->shapeController()->resourceManager()->imageCollection();

    // now create new strategies if needed
    foreach (KoShape *shape, selectedShapes) {
        if (!shape->isEditable())
            continue;

        // do we already have a strategy for that shape?
        if (m_strategies.contains(shape))
            continue;

        if (qSharedPointerDynamicCast<KoPatternBackground>(shape->background())) {
            KarbonPatternEditStrategyBase *s = new KarbonOdfPatternEditStrategy(shape, imageCollection);
            m_strategies.insert(shape, s);
            s->repaint();
        }
    }
    // automatically select strategy when editing single shape
    if (m_strategies.count() == 1 && !m_currentStrategy) {
        m_currentStrategy = m_strategies.begin().value();
        updateOptionsWidget();
    }

    if (m_currentStrategy)
        m_currentStrategy->repaint();
}

void KarbonPatternTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    Q_UNUSED(toolActivation);
    if (shapes.isEmpty()) {
        Q_EMIT done();
        return;
    }

    initialize();

    KarbonPatternEditStrategyBase::setHandleRadius(handleRadius());
    KarbonPatternEditStrategyBase::setGrabSensitivity(grabSensitivity());

    useCursor(Qt::ArrowCursor);

    connect(canvas()->shapeManager(), &KoShapeManager::selectionContentChanged, this, &KarbonPatternTool::initialize);
}

void KarbonPatternTool::deactivate()
{
    // we are not interested in selection content changes when not active
    disconnect(canvas()->shapeManager(), &KoShapeManager::selectionContentChanged, this, &KarbonPatternTool::initialize);

    foreach (KarbonPatternEditStrategyBase *strategy, m_strategies) {
        strategy->repaint();
    }

    qDeleteAll(m_strategies);
    m_strategies.clear();

    foreach (KoShape *shape, canvas()->shapeManager()->selection()->selectedShapes())
        shape->update();

    m_currentStrategy = nullptr;
}

void KarbonPatternTool::documentResourceChanged(int key, const QVariant &res)
{
    switch (key) {
    case KoDocumentResourceManager::HandleRadius:
        foreach (KarbonPatternEditStrategyBase *strategy, m_strategies)
            strategy->repaint();

        KarbonPatternEditStrategyBase::setHandleRadius(res.toUInt());

        foreach (KarbonPatternEditStrategyBase *strategy, m_strategies)
            strategy->repaint();
        break;
    case KoDocumentResourceManager::GrabSensitivity:
        KarbonPatternEditStrategyBase::setGrabSensitivity(res.toUInt());
        break;
    default:
        return;
    }
}

QList<QPointer<QWidget>> KarbonPatternTool::createOptionWidgets()
{
    QList<QPointer<QWidget>> widgets;

    m_optionsWidget = new KarbonPatternOptionsWidget();
    connect(m_optionsWidget, &KarbonPatternOptionsWidget::patternChanged, this, &KarbonPatternTool::patternChanged);

    KoResourceServer<KoPattern> *rserver = KoResourceServerProvider::instance()->patternServer();
    QSharedPointer<KoAbstractResourceServerAdapter> adapter(new KoResourceServerAdapter<KoPattern>(rserver));
    KoResourceItemChooser *chooser = new KoResourceItemChooser(adapter, m_optionsWidget);
    chooser->setObjectName("KarbonPatternChooser");

    connect(chooser, &KoResourceItemChooser::resourceSelected, this, &KarbonPatternTool::patternSelected);

    m_optionsWidget->setWindowTitle(i18n("Pattern Options"));
    widgets.append(m_optionsWidget);
    chooser->setWindowTitle(i18n("Patterns"));
    widgets.append(chooser);
    updateOptionsWidget();
    return widgets;
}

void KarbonPatternTool::patternSelected(KoResource *resource)
{
    KoPattern *currentPattern = dynamic_cast<KoPattern *>(resource);
    if (!currentPattern || !currentPattern->valid())
        return;

    KoImageCollection *imageCollection = canvas()->shapeController()->resourceManager()->imageCollection();
    if (imageCollection) {
        QList<KoShape *> selectedShapes = canvas()->shapeManager()->selection()->selectedShapes();
        QSharedPointer<KoPatternBackground> newFill(new KoPatternBackground(imageCollection));
        newFill->setPattern(currentPattern->pattern());
        canvas()->addCommand(new KoShapeBackgroundCommand(selectedShapes, newFill));
        initialize();
    }
}

void KarbonPatternTool::updateOptionsWidget()
{
    if (m_optionsWidget && m_currentStrategy) {
        QSharedPointer<KoPatternBackground> fill = qSharedPointerDynamicCast<KoPatternBackground>(m_currentStrategy->shape()->background());
        if (fill) {
            m_optionsWidget->setRepeat(fill->repeat());
            m_optionsWidget->setReferencePoint(fill->referencePoint());
            m_optionsWidget->setReferencePointOffset(fill->referencePointOffset());
            m_optionsWidget->setTileRepeatOffset(fill->tileRepeatOffset());
            m_optionsWidget->setPatternSize(fill->patternDisplaySize().toSize());
        }
    }
}

void KarbonPatternTool::patternChanged()
{
    if (m_currentStrategy) {
        KoShape *shape = m_currentStrategy->shape();
        QSharedPointer<KoPatternBackground> oldFill = qSharedPointerDynamicCast<KoPatternBackground>(shape->background());
        if (!oldFill)
            return;
        KoImageCollection *imageCollection = canvas()->shapeController()->resourceManager()->imageCollection();
        if (!imageCollection)
            return;
        QSharedPointer<KoPatternBackground> newFill(new KoPatternBackground(imageCollection));
        if (!newFill)
            return;
        newFill->setTransform(oldFill->transform());
        newFill->setPattern(oldFill->pattern());

        newFill->setRepeat(m_optionsWidget->repeat());
        newFill->setReferencePoint(m_optionsWidget->referencePoint());
        newFill->setReferencePointOffset(m_optionsWidget->referencePointOffset());
        newFill->setTileRepeatOffset(m_optionsWidget->tileRepeatOffset());
        newFill->setPatternDisplaySize(m_optionsWidget->patternSize());
        canvas()->addCommand(new KoShapeBackgroundCommand(shape, newFill));
    }
}
