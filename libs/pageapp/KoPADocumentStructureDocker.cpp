/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008-2009 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPADocumentStructureDocker.h"

#include "KoPACanvas.h"
#include "KoPADocument.h"
#include "KoPADocumentModel.h"
#include "KoPAPageBase.h"
#include "KoPAViewBase.h"

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoDrag.h>
#include <KoPAOdfPageSaveHelper.h>
#include <KoSelection.h>
#include <KoShapeCreateCommand.h>
#include <KoShapeDeleteCommand.h>
#include <KoShapeLayer.h>
#include <KoShapeManager.h>
#include <KoShapeOdfSaveHelper.h>
#include <KoShapePaste.h>
#include <KoShapeReorderCommand.h>
#include <KoToolManager.h>
#include <KoViewItemContextBar.h>

#include <KoIcon.h>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>

#include <QActionGroup>
#include <QApplication>
#include <QButtonGroup>
#include <QClipboard>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QItemSelection>
#include <QMenu>
#include <QMimeData>
#include <QToolButton>
#include <QVBoxLayout>

#include <algorithm>

enum ButtonIds { Button_Raise, Button_Lower, Button_Delete };

KoPADocumentStructureDockerFactory::KoPADocumentStructureDockerFactory(KoDocumentSectionView::DisplayMode mode, KoPageApp::PageType pageType)
    : m_mode(mode)
    , m_pageType(pageType)
{
}

QString KoPADocumentStructureDockerFactory::id() const
{
    return QString("document section view");
}

QDockWidget *KoPADocumentStructureDockerFactory::createDockWidget()
{
    return new KoPADocumentStructureDocker(m_mode, m_pageType);
}

KoPADocumentStructureDocker::KoPADocumentStructureDocker(KoDocumentSectionView::DisplayMode mode, KoPageApp::PageType pageType, QWidget *parent)
    : QDockWidget(parent)
    , KoCanvasObserverBase()
    , m_doc(nullptr)
    , m_model(nullptr)
{
    setWindowTitle(i18n("Document"));

    auto mainWidget = new QWidget(this);
    auto vbox = new QVBoxLayout(mainWidget);
    vbox->setContentsMargins({});
    vbox->setSpacing(0);

    vbox->addWidget(m_sectionView = new KoDocumentSectionView(mainWidget));
    m_sectionView->setProperty("_breeze_borders_sides", QVariant::fromValue(QFlags{Qt::BottomEdge}));

    auto buttonLayout = new QHBoxLayout(this);
    buttonLayout->setContentsMargins(4, 4, 4, 3);
    buttonLayout->setSpacing(4);
    vbox->addLayout(buttonLayout);

    auto button = new QToolButton(mainWidget);
    button->setIcon(koIcon("list-add"));
    if (pageType == KoPageApp::Slide) {
        button->setToolTip(i18n("Add a new slide or layer"));
    } else {
        button->setToolTip(i18n("Add a new page or layer"));
    }
    buttonLayout->addWidget(button);

    auto menu = new QMenu(button);
    button->setMenu(menu);
    button->setPopupMode(QToolButton::InstantPopup);
    menu->addAction(koIcon("document-new"), (pageType == KoPageApp::Slide) ? i18n("Slide") : i18n("Page"), this, &KoPADocumentStructureDocker::addPage);
    m_addLayerAction = menu->addAction(koIcon("layer-new"), i18n("Layer"), this, &KoPADocumentStructureDocker::addLayer);

    m_buttonGroup = new QButtonGroup(mainWidget);
    m_buttonGroup->setExclusive(false);

    button = new QToolButton(mainWidget);
    button->setIcon(koIcon("list-remove"));
    button->setToolTip(i18n("Delete selected objects"));
    m_buttonGroup->addButton(button, Button_Delete);
    buttonLayout->addWidget(button);

    buttonLayout->addStretch();

    button = new QToolButton(mainWidget);
    button->setIcon(koIcon("arrow-up"));
    button->setToolTip(i18n("Raise selected objects"));
    m_buttonGroup->addButton(button, Button_Raise);
    buttonLayout->addWidget(button);

    button = new QToolButton(mainWidget);
    button->setIcon(koIcon("arrow-down"));
    button->setToolTip(i18n("Lower selected objects"));
    m_buttonGroup->addButton(button, Button_Lower);
    buttonLayout->addWidget(button);

    button = new QToolButton(mainWidget);
    menu = new QMenu(this);
    auto group = new QActionGroup(this);

    m_viewModeActions.insert(KoDocumentSectionView::MinimalMode,
                             menu->addAction(koIcon("view-list-text"), i18n("Minimal View"), this, &KoPADocumentStructureDocker::minimalView));
    m_viewModeActions.insert(KoDocumentSectionView::DetailedMode,
                             menu->addAction(koIcon("view-list-details"), i18n("Detailed View"), this, &KoPADocumentStructureDocker::detailedView));
    m_viewModeActions.insert(KoDocumentSectionView::ThumbnailMode,
                             menu->addAction(koIcon("view-preview"), i18n("Thumbnail View"), this, &KoPADocumentStructureDocker::thumbnailView));

    foreach (QAction *action, m_viewModeActions) {
        action->setCheckable(true);
        action->setActionGroup(group);
    }

    button->setMenu(menu);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setIcon(koIcon("view-choose"));
    button->setText(i18n("View mode"));
    buttonLayout->addWidget(button);

    setWidget(mainWidget);

    connect(m_buttonGroup, &QButtonGroup::idClicked, this, &KoPADocumentStructureDocker::slotButtonClicked);

    m_model = new KoPADocumentModel(this);
    m_sectionView->setModel(m_model);
    m_sectionView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_sectionView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_sectionView->setDragDropMode(QAbstractItemView::InternalMove);

    connect(m_sectionView, &QAbstractItemView::pressed, this, &KoPADocumentStructureDocker::itemClicked);
    connect(m_sectionView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &KoPADocumentStructureDocker::itemSelected);

    connect(m_model, &KoPADocumentModel::requestPageSelection, this, &KoPADocumentStructureDocker::selectPages);
    connect(m_model, &QAbstractItemModel::modelReset, this, &KoPADocumentStructureDocker::dockerReset);

    KConfigGroup configGroup = KSharedConfig::openConfig()->group("KoPageApp/DocumentStructureDocker");
    QString viewModeString = configGroup.readEntry("ViewMode", "");

    if (viewModeString.isEmpty()) {
        setViewMode(mode);
    } else {
        setViewMode(viewModeFromString(viewModeString));
    }

    m_itemsContextBar = new KoViewItemContextBar(m_sectionView);
}

KoPADocumentStructureDocker::~KoPADocumentStructureDocker()
{
    KConfigGroup configGroup = KSharedConfig::openConfig()->group("KoPageApp/DocumentStructureDocker");
    configGroup.writeEntry("ViewMode", viewModeToString(m_sectionView->displayMode()));
}

void KoPADocumentStructureDocker::updateView()
{
    m_model->update();
}

void KoPADocumentStructureDocker::slotButtonClicked(int buttonId)
{
    switch (buttonId) {
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

void KoPADocumentStructureDocker::itemClicked(const QModelIndex &index)
{
    Q_ASSERT(index.internalPointer());

    if (!index.isValid())
        return;

    KoShape *shape = static_cast<KoShape *>(index.internalPointer());
    if (!shape)
        return;
    // check whether the newly selected shape is a page or shape/layer
    bool isPage = (dynamic_cast<KoPAPageBase *>(shape) != nullptr);
    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();

    if (isPage) {
        // no shape is currently selected
        if (!m_selectedShapes.isEmpty()) {
            m_sectionView->clearSelection();
            selection->deselectAll();
            m_sectionView->setCurrentIndex(index);
            m_selectedShapes.clear();
            Q_EMIT pageChanged(dynamic_cast<KoPAPageBase *>(shape));
        } else {
            // There are more than one page selected
            if (m_sectionView->selectionModel()->selectedIndexes().size() == 1) {
                Q_EMIT pageChanged(dynamic_cast<KoPAPageBase *>(shape));
            }
        }
    } else {
        KoPAPageBase *newPageByShape = m_doc->pageByShape(shape);
        // there is already shape(s) selected
        if (!m_selectedShapes.isEmpty()) {
            // if the newly selected shape is not in the same page as previously
            // selected shape(s), then clear previous selection
            KoPAPageBase *currentPage = m_doc->pageByShape(m_selectedShapes.first());
            KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>(shape);
            if (currentPage != newPageByShape) {
                m_sectionView->clearSelection();
                selection->deselectAll();
                m_sectionView->setCurrentIndex(index);
                m_selectedShapes.clear();
                Q_EMIT pageChanged(newPageByShape);
                if (layer) {
                    selection->setActiveLayer(layer);
                } else {
                    selection->select(shape);
                    shape->update();
                }
            } else {
                QList<KoPAPageBase *> selectedPages;
                QList<KoShapeLayer *> selectedLayers;
                QList<KoShape *> selectedShapes;

                // separate selected layers and selected shapes
                extractSelectedLayersAndShapes(selectedPages, selectedLayers, selectedShapes);

                // XXX: Do stuff with the selected pages!

                foreach (KoShape *shape, selection->selectedShapes()) {
                    shape->update();
                }
                selection->deselectAll();
                foreach (KoShape *shape, selectedShapes) {
                    if (shape) {
                        selection->select(shape);
                        shape->update();
                    }
                }
                // if we just selected a layer, check whether this layer is already active, if not
                // then make it active
                if (layer && selection->activeLayer() != layer && selectedLayers.count() <= 1) {
                    selection->setActiveLayer(layer);
                }
            }
        }
        // no shape is selected, meaning only page(s) is selected
        else {
            m_sectionView->clearSelection();
            m_sectionView->setCurrentIndex(index);
            selection->select(shape);
            shape->update();
            Q_EMIT pageChanged(newPageByShape);
        }
        m_selectedShapes.append(shape);
    }
}

void KoPADocumentStructureDocker::addLayer()
{
    bool ok = true;
    QString name = QInputDialog::getText(this, i18n("New Layer"), i18n("Enter the name of the new layer:"), QLineEdit::Normal, i18n("New layer"), &ok);
    if (ok) {
        KoShapeLayer *layer = new KoShapeLayer();
        KoPACanvas *canvas = dynamic_cast<KoPACanvas *>(KoToolManager::instance()->activeCanvasController()->canvas());
        if (canvas) {
            layer->setParent(canvas->koPAView()->activePage());
            layer->setName(name);
            QList<KoShape *> layers(canvas->koPAView()->activePage()->shapes());
            if (!layers.isEmpty()) {
                std::sort(layers.begin(), layers.end(), KoShape::compareShapeZIndex);
                layer->setZIndex(layers.last()->zIndex() + 1);
            }
            KUndo2Command *cmd = new KoShapeCreateCommand(m_doc, layer, nullptr);
            cmd->setText(kundo2_i18n("Create Layer"));
            m_doc->addCommand(cmd);
            m_model->update();
        }
    }
}

void KoPADocumentStructureDocker::deleteItem()
{
    QList<KoPAPageBase *> selectedPages;
    QList<KoShapeLayer *> selectedLayers;
    QList<KoShape *> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes(selectedPages, selectedLayers, selectedShapes);

    KUndo2Command *cmd = nullptr;

    if (selectedLayers.count()) {
        if (m_doc->pages().count() > selectedPages.count()) {
            QList<KoShape *> deleteShapes;
            foreach (KoPAPageBase *page, selectedPages) {
                deleteShapes += page->shapes();
                deleteShapes.append(page);
            }
            cmd = new KoShapeDeleteCommand(m_doc, deleteShapes);
            cmd->setText(kundo2_i18n("Delete Layer"));
        } else {
            KMessageBox::error(nullptr, i18n("Could not delete all layers. At least one layer is required."), i18n("Error deleting layers"));
        }
    } else if (selectedShapes.count()) {
        cmd = new KoShapeDeleteCommand(m_doc, selectedShapes);
    } else if (!selectedPages.isEmpty() && selectedPages.count() < m_doc->pages().count()) {
        m_doc->removePages(selectedPages);
    }

    if (cmd) {
        m_doc->addCommand(cmd);
        m_model->update();
    }
}

void KoPADocumentStructureDocker::raiseItem()
{
    QList<KoPAPageBase *> selectedPages;
    QList<KoShapeLayer *> selectedLayers;
    QList<KoShape *> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes(selectedPages, selectedLayers, selectedShapes);

    KUndo2Command *cmd = nullptr;

    if (selectedLayers.count()) {
        //         // check if all layers could be raised
        //         foreach(KoShapeLayer* layer, selectedLayers)
        //             if (! m_document->canRaiseLayer(layer))
        //                 return;

        //        cmd = new KoPALayerReorderCommand(m_document, selectedLayers, KoPALayerReorderCommand::RaiseLayer);
    } else if (selectedShapes.count()) {
        cmd = KoShapeReorderCommand::createCommand(selectedShapes,
                                                   KoToolManager::instance()->activeCanvasController()->canvas()->shapeManager(),
                                                   KoShapeReorderCommand::RaiseShape);
    }

    if (cmd) {
        m_doc->addCommand(cmd);
        m_model->update();
    }
}

void KoPADocumentStructureDocker::lowerItem()
{
    QList<KoPAPageBase *> selectedPages;
    QList<KoShapeLayer *> selectedLayers;
    QList<KoShape *> selectedShapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes(selectedPages, selectedLayers, selectedShapes);

    KUndo2Command *cmd = nullptr;

    if (selectedLayers.count()) {
        //         // check if all layers could be raised
        //         foreach(KoShapeLayer* layer, selectedLayers)
        //             if (! m_document->canLowerLayer(layer))
        //                 return;

        //        cmd = new KoPALayerReorderCommand(m_document, selectedLayers, KoPALayerReorderCommand::LowerLayer);
    } else if (selectedShapes.count()) {
        cmd = KoShapeReorderCommand::createCommand(selectedShapes,
                                                   KoToolManager::instance()->activeCanvasController()->canvas()->shapeManager(),
                                                   KoShapeReorderCommand::LowerShape);
    }

    if (cmd) {
        m_doc->addCommand(cmd);
        m_model->update();
    }
}

void KoPADocumentStructureDocker::extractSelectedLayersAndShapes(QList<KoPAPageBase *> &pages, QList<KoShapeLayer *> &layers, QList<KoShape *> &shapes)
{
    pages.clear();
    layers.clear();
    shapes.clear();

    QModelIndexList selectedItems = m_sectionView->selectionModel()->selectedIndexes();
    if (selectedItems.count() == 0)
        return;

    // TODO tz: I don't know what is best:
    // 1. only make it possible to select one type of object page, layer, shape
    // 2. don't add shapes when we already have the page/layer/group in the selection
    // separate selected layers and selected shapes
    foreach (const QModelIndex &index, selectedItems) {
        KoShape *shape = static_cast<KoShape *>(index.internalPointer());
        KoPAPageBase *page = dynamic_cast<KoPAPageBase *>(shape);
        if (page) {
            pages.append(page);
        } else {
            KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>(shape);
            if (layer)
                layers.append(layer);
            else if (!selectedItems.contains(index.parent()))
                shapes.append(shape);
        }
    }
}

void KoPADocumentStructureDocker::setCanvas(KoCanvasBase *canvas)
{
    KoPACanvas *c = dynamic_cast<KoPACanvas *>(canvas);
    if (c) {
        m_doc = c->document();
        m_model->setDocument(m_doc);
        m_sectionView->setModel(m_model);
    }
}

void KoPADocumentStructureDocker::unsetCanvas()
{
    m_doc = nullptr;
    m_model->setDocument(nullptr);
    m_sectionView->setModel(nullptr);
}

void KoPADocumentStructureDocker::setActivePage(KoPAPageBase *page)
{
    if (m_doc) {
        int row = m_doc->pageIndex(page);
        QModelIndex index = m_model->index(row, 0);
        if (index != m_sectionView->currentIndex() && index != getRootIndex(m_sectionView->currentIndex())) {
            m_sectionView->setCurrentIndex(index);
        }
    }
}

void KoPADocumentStructureDocker::setMasterMode(bool master)
{
    m_model->setMasterMode(master);
}

void KoPADocumentStructureDocker::minimalView()
{
    setViewMode(KoDocumentSectionView::MinimalMode);
    m_itemsContextBar->disableContextBar();
}

void KoPADocumentStructureDocker::detailedView()
{
    setViewMode(KoDocumentSectionView::DetailedMode);
    m_itemsContextBar->disableContextBar();
}

void KoPADocumentStructureDocker::thumbnailView()
{
    setViewMode(KoDocumentSectionView::ThumbnailMode);
    m_itemsContextBar->enableContextBar();
}

void KoPADocumentStructureDocker::setViewMode(KoDocumentSectionView::DisplayMode mode)
{
    bool expandable = (mode != KoDocumentSectionView::ThumbnailMode);

    // if we switch to non-expandable mode (ThumbnailMode) and if current index
    // is not a page, we need to select the corresponding page first, otherwise
    // none of the page will be selected when we do collapse all
    if (!expandable) {
        QModelIndex currentIndex = m_sectionView->currentIndex();
        QModelIndex rootIndex = getRootIndex(currentIndex);
        if (currentIndex != rootIndex) {
            m_sectionView->setCurrentIndex(rootIndex);
        }
        m_sectionView->collapseAll();
    }

    m_sectionView->setDisplayMode(mode);
    m_sectionView->setItemsExpandable(expandable);
    m_sectionView->setRootIsDecorated(expandable);
    // m_sectionView->setSelectionMode(expandable ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection);

    m_viewModeActions[mode]->setChecked(true);
}

QModelIndex KoPADocumentStructureDocker::getRootIndex(const QModelIndex &index) const
{
    QModelIndex currentIndex;
    QModelIndex parentIndex = index.parent();
    if (!parentIndex.isValid()) {
        return index;
    }
    while (parentIndex.isValid()) {
        currentIndex = parentIndex;
        parentIndex = currentIndex.parent();
    }

    return currentIndex;
}

KoDocumentSectionView::DisplayMode KoPADocumentStructureDocker::viewModeFromString(const QString &mode)
{
    if (mode == "Minimal")
        return KoDocumentSectionView::MinimalMode;
    else if (mode == "Detailed")
        return KoDocumentSectionView::DetailedMode;
    else if (mode == "Thumbnail")
        return KoDocumentSectionView::ThumbnailMode;

    return KoDocumentSectionView::DetailedMode;
}

QString KoPADocumentStructureDocker::viewModeToString(KoDocumentSectionView::DisplayMode mode)
{
    switch (mode) {
    case KoDocumentSectionView::MinimalMode:
        return QString("Minimal");
        break;
    case KoDocumentSectionView::DetailedMode:
        return QString("Detailed");
        break;
    case KoDocumentSectionView::ThumbnailMode:
        return QString("Thumbnail");
        break;
    }

    return QString();
}

void KoPADocumentStructureDocker::itemSelected(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    if (selected.indexes().isEmpty()) {
        m_buttonGroup->button(Button_Raise)->setEnabled(false);
        m_buttonGroup->button(Button_Lower)->setEnabled(false);
        m_addLayerAction->setEnabled(false);
    } else {
        m_buttonGroup->button(Button_Raise)->setEnabled(true);
        m_buttonGroup->button(Button_Lower)->setEnabled(true);
        m_addLayerAction->setEnabled(true);
    }

    if (!m_sectionView->selectionModel()->selectedIndexes().empty() && m_sectionView->selectionModel()->selectedIndexes().count() < m_doc->pages().count()) {
        m_buttonGroup->button(Button_Delete)->setEnabled(true);
    } else {
        m_buttonGroup->button(Button_Delete)->setEnabled(false);
    }
}

void KoPADocumentStructureDocker::addPage()
{
    KoPACanvas *canvas = dynamic_cast<KoPACanvas *>(KoToolManager::instance()->activeCanvasController()->canvas());
    if (canvas) {
        canvas->koPAView()->insertPage();
    }
}

void KoPADocumentStructureDocker::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    // Not connected yet
    if (m_doc->pageType() == KoPageApp::Slide) {
        menu.addAction(koIcon("document-new"), i18n("Add a new slide"), this, &KoPADocumentStructureDocker::addPage);
    } else {
        menu.addAction(koIcon("document-new"), i18n("Add a new page"), this, &KoPADocumentStructureDocker::addPage);
    }
    menu.addAction(koIcon("edit-delete"), i18n("Delete selected objects"), this, &KoPADocumentStructureDocker::deleteItem);
    menu.addSeparator();
    menu.addAction(koIcon("edit-cut"), i18n("Cut"), this, &KoPADocumentStructureDocker::editCut);
    menu.addAction(koIcon("edit-copy"), i18n("Copy"), this, &KoPADocumentStructureDocker::editCopy);
    menu.addAction(koIcon("edit-paste"), i18n("Paste"), this, &KoPADocumentStructureDocker::editPaste);

    menu.exec(event->globalPos());
}

void KoPADocumentStructureDocker::editCut()
{
    editCopy();
    deleteItem();
}

void KoPADocumentStructureDocker::editCopy()
{
    QList<KoPAPageBase *> pages;
    QList<KoShapeLayer *> layers;
    QList<KoShape *> shapes;

    // separate selected layers and selected shapes
    extractSelectedLayersAndShapes(pages, layers, shapes);

    foreach (KoShape *shape, layers) {
        // Add layers to shapes
        shapes.append(shape);
    }

    if (!shapes.empty()) {
        // Copy Shapes or Layers
        KoShapeOdfSaveHelper saveHelper(shapes);
        KoDrag drag;
        drag.setOdf(KoOdf::mimeType(KoOdf::Text), saveHelper);
        drag.addToClipboard();
        return;
    }

    if (!pages.empty()) {
        // Copy Pages
        KoPAOdfPageSaveHelper saveHelper(m_doc, pages);
        KoDrag drag;
        drag.setOdf(KoOdf::mimeType(m_doc->documentType()), saveHelper);
        drag.addToClipboard();
    }
}

void KoPADocumentStructureDocker::editPaste()
{
    const QMimeData *data = QApplication::clipboard()->mimeData();

    if (data->hasFormat(KoOdf::mimeType(KoOdf::Text))) {
        // Paste Shapes or Layers
        KoCanvasBase *canvas = KoToolManager::instance()->activeCanvasController()->canvas();
        KoShapeManager *shapeManager = canvas->shapeManager();
        KoShapePaste paste(canvas, shapeManager->selection()->activeLayer());
        paste.paste(KoOdf::Text, data);

    } else {
        // Paste Pages
        KoPACanvas *canvas = static_cast<KoPACanvas *>(KoToolManager::instance()->activeCanvasController()->canvas());
        canvas->koPAView()->pagePaste();
    }
}

void KoPADocumentStructureDocker::selectPages(int start, int count)
{
    if ((start < 0) || (count < 1)) {
        return;
    }
    Q_EMIT pageChanged(m_doc->pageByIndex(start, false));
    m_sectionView->clearSelection();
    for (int i = start; i < (start + count); i++) {
        QModelIndex index = m_model->index(i, 0, QModelIndex());
        if (index.isValid()) {
            m_sectionView->selectionModel()->select(index, QItemSelectionModel::Select);
        }
    }
}

// kate: replace-tabs on; space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
