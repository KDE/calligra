/* This file is part of the KDE project
 * Copyright (C) 2006-2009 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonLayerDocker.h"
#include "KarbonLayerModel.h"
#include "KarbonLayerSortingModel.h"
#include "KarbonFactory.h"

#include <KarbonDocument.h>
#include <KarbonPart.h>
#include <KarbonLayerReorderCommand.h>

#include <KoShapeManager.h>
#include <KoShapeBorderModel.h>
#include <KoShapeContainer.h>
#include <KoToolManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoShapeControllerBase.h>
#include <KoSelection.h>
#include <KoShapeCreateCommand.h>
#include <KoShapeDeleteCommand.h>
#include <KoShapeReorderCommand.h>
#include <KoShapeLayer.h>
#include <KoShapeGroup.h>

#include <klocale.h>
#include <kicon.h>
#include <kiconloader.h>
#include <kinputdialog.h>
#include <kmessagebox.h>
#include <kparts/part.h>
#include <KMenu>
#include <KConfigGroup>

#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QToolButton>

enum ButtonIds {
    Button_New,
    Button_Raise,
    Button_Lower,
    Button_Delete
};

KarbonLayerDockerFactory::KarbonLayerDockerFactory()
{
}

QString KarbonLayerDockerFactory::id() const
{
    return QString("Layer view");
}

QDockWidget* KarbonLayerDockerFactory::createDockWidget()
{
    KarbonLayerDocker* widget = new KarbonLayerDocker();
    widget->setObjectName(id());

    return widget;
}

KarbonLayerDocker::KarbonLayerDocker()
        : m_part(0), m_model(0), m_updateTimer(this)
{
    setWindowTitle(i18n("Layer view"));

    QWidget *mainWidget = new QWidget(this);
    QGridLayout* layout = new QGridLayout(mainWidget);
    layout->addWidget(m_layerView = new KoDocumentSectionView(mainWidget), 0, 0, 1, 6);

    QButtonGroup *buttonGroup = new QButtonGroup(mainWidget);
    buttonGroup->setExclusive(false);

    QPushButton *button = new QPushButton(mainWidget);
    button->setIcon(SmallIcon("list-add"));
    button->setToolTip(i18n("Add a new layer"));
    buttonGroup->addButton(button, Button_New);
    layout->addWidget(button, 1, 0);

    button = new QPushButton(mainWidget);
    button->setIcon(SmallIcon("list-remove"));
    button->setToolTip(i18n("Delete selected objects"));
    buttonGroup->addButton(button, Button_Delete);
    layout->addWidget(button, 1, 1);

    button = new QPushButton(mainWidget);
    button->setIcon(SmallIcon("go-up"));
    button->setToolTip(i18n("Raise selected objects"));
    buttonGroup->addButton(button, Button_Raise);
    layout->addWidget(button, 1, 2);

    button = new QPushButton(mainWidget);
    button->setIcon(SmallIcon("go-down"));
    button->setToolTip(i18n("Lower selected objects"));
    buttonGroup->addButton(button, Button_Lower);
    layout->addWidget(button, 1, 3);

    QToolButton * toolButton = new QToolButton(mainWidget);
    KMenu * menu = new KMenu(this);
    QActionGroup *group = new QActionGroup(this);

    m_viewModeActions.insert(KoDocumentSectionView::MinimalMode,
                             menu->addAction(SmallIcon("view-list-text"), i18n("Minimal View"), this, SLOT(minimalView())));
    m_viewModeActions.insert(KoDocumentSectionView::DetailedMode,
                             menu->addAction(SmallIcon("view-list-details"), i18n("Detailed View"), this, SLOT(detailedView())));
    m_viewModeActions.insert(KoDocumentSectionView::ThumbnailMode,
                             menu->addAction(SmallIcon("view-preview"), i18n("Thumbnail View"), this, SLOT(thumbnailView())));

    foreach(QAction* action, m_viewModeActions) {
        action->setCheckable(true);
        action->setActionGroup(group);
    }

    toolButton->setMenu(menu);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    toolButton->setIcon(SmallIcon("view-choose"));
    toolButton->setText(i18n("View mode"));
    layout->addWidget(toolButton, 1, 5);
    layout->setSpacing(0);
    layout->setMargin(3);

    setWidget(mainWidget);

    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonClicked(int)));

    m_model = new KarbonLayerModel(this);
    m_model->setDocument(m_part ? &m_part->document() : 0);
    m_sortModel = new KarbonLayerSortingModel(this);
    m_sortModel->setDocument(m_part ? &m_part->document() : 0);
    m_sortModel->setSourceModel(m_model);

    m_layerView->setItemsExpandable(true);
    m_layerView->setModel(m_sortModel);
    m_layerView->setDisplayMode(KoDocumentSectionView::MinimalMode);
    m_layerView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_layerView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_layerView->setDragDropMode(QAbstractItemView::InternalMove);
    m_layerView->setSortingEnabled(true);

    KoDocumentSectionView::DisplayMode mode = KoDocumentSectionView::MinimalMode;
    KSharedConfigPtr config = KarbonFactory::componentData().config();
    if (config->hasGroup("Interface")) {
        QString modeStr = config->group("Interface").readEntry("LayerDockerMode", "minimal");
        if (modeStr == "detailed")
            mode = KoDocumentSectionView::DetailedMode;
        else if (modeStr == "thumbnail")
            mode = KoDocumentSectionView::ThumbnailMode;
    }
    setViewMode(mode);

    connect(m_layerView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(itemClicked(const QModelIndex&)));

    m_updateTimer.setSingleShot(true);
    m_updateTimer.setInterval(250);
    connect(&m_updateTimer, SIGNAL(timeout()), m_model, SLOT(update()));
}

KarbonLayerDocker::~KarbonLayerDocker()
{
    KSharedConfigPtr config = KarbonFactory::componentData().config();
    QString modeStr;
    switch (m_layerView->displayMode()) {
    case KoDocumentSectionView::MinimalMode:
        modeStr = "minimal";
        break;
    case KoDocumentSectionView::DetailedMode:
        modeStr = "detailed";
        break;
    case KoDocumentSectionView::ThumbnailMode:
        modeStr = "thumbnail";
        break;
    }
    config->group("Interface").writeEntry("LayerDockerMode", modeStr);
}

void KarbonLayerDocker::updateView()
{
    if (m_updateTimer.isActive())
        return;

    m_updateTimer.start();
}

void KarbonLayerDocker::setPart(KParts::Part * part)
{
    m_part = dynamic_cast<KarbonPart*>(part);
    if (! m_part) {
        m_sortModel->setDocument(0);
        m_model->setDocument(0);
    } else {
        m_sortModel->setDocument(&m_part->document());
        m_model->setDocument(&m_part->document());
    }
    m_model->update();
}

void KarbonLayerDocker::slotButtonClicked(int buttonId)
{
    switch (buttonId) {
    case Button_New:
        addLayer();
        break;
    case Button_Raise:
        raiseItem();
        break;
    case Button_Lower:
        lowerItem();
        break;
    case Button_Delete:
        deleteItem();
        break;
    }
}

void KarbonLayerDocker::itemClicked(const QModelIndex &index)
{
    KoShape *shape = shapeFromIndex(index);
    if (! shape)
        return;

    KoCanvasController * canvasController = KoToolManager::instance()->activeCanvasController();
    if (! canvasController)
        return;

    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    if (! selection)
        return;

    KoShapeLayer * layer = dynamic_cast<KoShapeLayer*>(shape);
    if (layer) {
        selection->setActiveLayer(layer);
        return;
    }

    QList<KoShapeLayer*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes(selectedLayers, selectedShapes);

    foreach(KoShape* shape, selection->selectedShapes())
    shape->update();

    selection->deselectAll();

    foreach(KoShape* shape, selectedShapes) {
        if (shape) {
            selection->select(shape, false);
            shape->update();
        }
    }
}

void KarbonLayerDocker::addLayer()
{
    bool ok = true;
    QString name = KInputDialog::getText(i18n("New Layer"),
                                         i18n("Enter the name of the new layer:"),
                                         i18n("New layer"), &ok, this);
    if (ok) {
        KoShapeLayer* layer = new KoShapeLayer();
        layer->setName(name);
        KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
        QUndoCommand *cmd = new KoShapeCreateCommand(m_part, layer, 0);
        cmd->setText(i18n("Create Layer"));
        canvasController->canvas()->addCommand(cmd);
        m_model->update();
    }
}

void KarbonLayerDocker::deleteItem()
{
    QList<KoShapeLayer*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes(selectedLayers, selectedShapes);

    QUndoCommand *cmd = 0;

    if (selectedLayers.count()) {
        if (m_part->document().layers().count() > selectedLayers.count()) {
            QList<KoShape*> deleteShapes;
            foreach(KoShapeLayer* layer, selectedLayers) {
                deleteShapes += layer->shapes();
                deleteShapes.append(layer);
            }
            cmd = new KoShapeDeleteCommand(m_part, deleteShapes);
            cmd->setText(i18n("Delete Layer"));
        } else {
            KMessageBox::error(0L, i18n("Could not delete all layers. At least one layer is required."), i18n("Error deleting layers"));
        }
    } else if (selectedShapes.count()) {
        cmd = new KoShapeDeleteCommand(m_part, selectedShapes);
    }

    if (cmd) {
        KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
        canvasController->canvas()->addCommand(cmd);
        m_model->update();
    }
}

void KarbonLayerDocker::raiseItem()
{
    QList<KoShapeLayer*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes(selectedLayers, selectedShapes, true);

    KoCanvasBase* canvas = KoToolManager::instance()->activeCanvasController()->canvas();

    QUndoCommand *cmd = 0;

    if (selectedLayers.count()) {
        // check if all layers could be raised
        foreach(KoShapeLayer* layer, selectedLayers)
        if (! m_part->document().canRaiseLayer(layer))
            return;

        cmd = new KarbonLayerReorderCommand(&m_part->document(), selectedLayers, KarbonLayerReorderCommand::RaiseLayer);
    } else if (selectedShapes.count()) {
        cmd = KoShapeReorderCommand::createCommand(selectedShapes, canvas->shapeManager(), KoShapeReorderCommand::RaiseShape);
    }

    if (cmd) {
        canvas->addCommand(cmd);
        m_model->update();

        // adjust layer selection
        if (selectedLayers.count()) {
            selectLayers(selectedLayers);
        }
    }
}

void KarbonLayerDocker::lowerItem()
{
    QList<KoShapeLayer*> selectedLayers;
    QList<KoShape*> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes(selectedLayers, selectedShapes, true);

    KoCanvasBase* canvas = KoToolManager::instance()->activeCanvasController()->canvas();

    QUndoCommand *cmd = 0;

    if (selectedLayers.count()) {
        // check if all layers could be raised
        foreach(KoShapeLayer* layer, selectedLayers)
        if (! m_part->document().canLowerLayer(layer))
            return;

        cmd = new KarbonLayerReorderCommand(&m_part->document(), selectedLayers, KarbonLayerReorderCommand::LowerLayer);
    } else if (selectedShapes.count()) {
        cmd = KoShapeReorderCommand::createCommand(selectedShapes, canvas->shapeManager(), KoShapeReorderCommand::LowerShape);
    }

    if (cmd) {
        canvas->addCommand(cmd);
        m_model->update();

        // adjust layer selection
        if (selectedLayers.count()) {
            selectLayers(selectedLayers);
        }
    }
}

void KarbonLayerDocker::selectLayers(QList<KoShapeLayer*> layers)
{
    QModelIndex root = m_layerView->rootIndex();
    QItemSelectionModel * selModel = m_layerView->selectionModel();
    selModel->clearSelection();
    foreach(KoShapeLayer * layer, layers) {
        int layerPos = m_part->document().layerPos(layer);
        QModelIndex child = m_model->index(layerPos, 0);
        selModel->select(m_sortModel->mapFromSource(child), QItemSelectionModel::Select);
    }
}

void KarbonLayerDocker::extractSelectedLayersAndShapes(
    QList<KoShapeLayer*> &layers, QList<KoShape*> &shapes, bool addChilds)
{
    layers.clear();
    shapes.clear();

    QModelIndexList selectedItems = m_layerView->selectionModel()->selectedIndexes();
    if (selectedItems.count() == 0)
        return;

    // separate selected layers and selected shapes
    foreach(const QModelIndex & index, selectedItems) {
        KoShape *shape = shapeFromIndex(index);
        KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>(shape);
        if (layer) {
            layers.append(layer);
        } else if (! selectedItems.contains(index.parent())) {
            shapes.append(shape);
            KoShapeGroup * group = dynamic_cast<KoShapeGroup*>(shape);
            if (group && addChilds)
                addChildsRecursive(group, shapes);
        }
    }
}

void KarbonLayerDocker::addChildsRecursive(KoShapeGroup * parent, QList<KoShape*> &shapes)
{
    foreach(KoShape * child, parent->shapes()) {
        if (! shapes.contains(child))
            shapes.append(child);
        KoShapeGroup * group = dynamic_cast<KoShapeGroup*>(child);
        if (group)
            addChildsRecursive(group, shapes);
    }
}

KoShape * KarbonLayerDocker::shapeFromIndex(const QModelIndex &index)
{
    Q_ASSERT(index.internalPointer());

    QModelIndex sourceIndex = index;
    if (index.model() != m_model)
        sourceIndex = m_sortModel->mapToSource(index);

    if (! sourceIndex.isValid())
        return 0;

    return static_cast<KoShape*>(sourceIndex.internalPointer());
}

void KarbonLayerDocker::minimalView()
{
    setViewMode(KoDocumentSectionView::MinimalMode);
}

void KarbonLayerDocker::detailedView()
{
    setViewMode(KoDocumentSectionView::DetailedMode);
}

void KarbonLayerDocker::thumbnailView()
{
    setViewMode(KoDocumentSectionView::ThumbnailMode);
}

void KarbonLayerDocker::setViewMode(KoDocumentSectionView::DisplayMode mode)
{
    bool expandable = (mode != KoDocumentSectionView::ThumbnailMode);

    m_layerView->setDisplayMode(mode);
    m_layerView->setItemsExpandable(expandable);
    m_layerView->setRootIsDecorated(expandable);
    m_layerView->setSortingEnabled(true);
    m_viewModeActions[mode]->setChecked(true);
}

#include "KarbonLayerDocker.moc"

// kate: replace-tabs on; space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
