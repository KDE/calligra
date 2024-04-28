/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrViewModeSlidesSorter.h"
#include "KPrCustomSlideShows.h"
#include "KPrCustomSlideShowsModel.h"
#include "KPrDocument.h"
#include "KPrFactory.h"
#include "KPrSlidesManagerView.h"
#include "KPrSlidesSorterDocumentModel.h"
#include "KPrSlidesSorterItemDelegate.h"
#include "KPrView.h"
#include "StageDebug.h"

// Qt Headers
#include <QComboBox>
#include <QContextMenuEvent>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSplitter>

// Calligra Headers
#include <KoCanvasController.h>
#include <KoComponentData.h>
#include <KoCopyController.h>
#include <KoCutController.h>
#include <KoDocumentResourceManager.h>
#include <KoIcon.h>
#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPAView.h>
#include <KoText.h>
#include <KoViewItemContextBar.h>
#include <KoZoomController.h>

// KF5 Headers
#include <KActionCollection>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>

const int DEFAULT_ICON_SIZE = 200;

KPrViewModeSlidesSorter::KPrViewModeSlidesSorter(KoPAView *view, KoPACanvasBase *canvas)
    : KoPAViewMode(view, canvas)
    , m_slidesSorterView(new KPrSlidesManagerView())
    , m_customSlideShowView(new KPrSlidesManagerView())
    , m_slidesSorterModel(new KPrSlidesSorterDocumentModel(this, view->parentWidget()))
    , m_centralWidget(new QWidget())
    , m_customSlideShowModel(new KPrCustomSlideShowsModel(static_cast<KPrDocument *>(view->kopaDocument()), view->parentWidget()))
    , m_iconSize(QSize(200, 200))
    , m_editCustomSlideShow(false)
{
    setName(i18n("Slides Sorter"));
    // Create customSlideShow GUI
    QWidget *m_customShowsToolBar = new QWidget();

    QHBoxLayout *toolBarLayout = new QHBoxLayout(m_customShowsToolBar);
    toolBarLayout->setContentsMargins({});
    QVBoxLayout *centralWidgetLayout = new QVBoxLayout(m_centralWidget);
    centralWidgetLayout->setContentsMargins({});
    centralWidgetLayout->setSpacing(0);

    QLabel *slideShowsLabel = new QLabel(i18n("Slide Show: "));
    m_customSlideShowsList = new QComboBox;
    m_customSlideShowsList->setEditable(false);
    m_customSlideShowsList->setInsertPolicy(QComboBox::NoInsert);
    m_customSlideShowsList->setMinimumContentsLength(30);
    slideShowsLabel->setBuddy(m_customSlideShowsList);

    m_buttonAddCustomSlideShow = new QToolButton();
    m_buttonAddCustomSlideShow->setIcon(koIcon("list-add"));
    m_buttonAddCustomSlideShow->setToolTip(i18n("Add a new custom slide show"));

    m_buttonDelCustomSlideShow = new QToolButton();
    m_buttonDelCustomSlideShow->setIcon(koIcon("list-remove"));
    m_buttonDelCustomSlideShow->setEnabled(false);
    m_buttonDelCustomSlideShow->setToolTip(i18n("Delete current custom slide show"));

    m_buttonAddSlideToCurrentShow = new QToolButton();
    m_buttonAddSlideToCurrentShow->setIcon(koIcon("arrow-down"));
    m_buttonAddSlideToCurrentShow->setToolTip(i18n("Add slides to current custom slide show"));
    m_buttonAddSlideToCurrentShow->setEnabled(false);

    m_buttonDelSlideFromCurrentShow = new QToolButton();
    m_buttonDelSlideFromCurrentShow->setIcon(koIcon("arrow-up"));
    m_buttonDelSlideFromCurrentShow->setToolTip(i18n("Remove slides from current custom slide show"));
    m_buttonDelSlideFromCurrentShow->setEnabled(false);

    QSplitter *viewsSplitter = new QSplitter(Qt::Vertical);

    // hide Custom Shows View
    m_customSlideShowView->setMaximumHeight(0);

    // Layout Widgets
    toolBarLayout->addWidget(slideShowsLabel);
    toolBarLayout->addWidget(m_customSlideShowsList);
    toolBarLayout->addWidget(m_buttonAddCustomSlideShow);
    toolBarLayout->addWidget(m_buttonDelCustomSlideShow);
    toolBarLayout->addStretch();
    toolBarLayout->addWidget(m_buttonAddSlideToCurrentShow);
    toolBarLayout->addWidget(m_buttonDelSlideFromCurrentShow);
    viewsSplitter->addWidget(m_slidesSorterView);
    viewsSplitter->addWidget(m_customSlideShowView);

    centralWidgetLayout->addWidget(viewsSplitter);
    centralWidgetLayout->addWidget(m_customShowsToolBar);

    // initialize widgets
    m_centralWidget->hide();
    m_slidesSorterView->setIconSize(m_iconSize);
    m_slidesSorterView->setAutoScroll(true);
    m_customSlideShowView->setIconSize(m_iconSize);
    m_customSlideShowView->setAutoScroll(true);

    // Populate ComboBox
    customShowChanged(0);
    updateCustomSlideShowsList();

    // Setup customSlideShows view
    m_customSlideShowView->setModel(m_customSlideShowModel);
    m_customSlideShowView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_customSlideShowView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_customSlideShowView->setDragDropMode(QAbstractItemView::InternalMove);
    m_customSlideShowView->setSpacing(10);

    // Setup slides sorter view
    m_slidesSorterModel->setDocument(m_view->kopaDocument());
    m_slidesSorterView->setModel(m_slidesSorterModel);
    m_slidesSorterView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_slidesSorterView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_slidesSorterView->setDragDropMode(QAbstractItemView::InternalMove);
    m_slidesSorterView->setSpacing(10);

    // setup signals
    connect(m_slidesSorterView, &KPrSlidesManagerView::requestContextMenu, this, &KPrViewModeSlidesSorter::slidesSorterContextMenu);
    connect(m_customSlideShowView, &KPrSlidesManagerView::requestContextMenu, this, &KPrViewModeSlidesSorter::customSlideShowsContextMenu);
    connect(m_slidesSorterView, &KPrSlidesManagerView::slideDblClick, this, &KPrViewModeSlidesSorter::activateNormalViewMode);
    connect(m_buttonAddCustomSlideShow, &QAbstractButton::clicked, this, &KPrViewModeSlidesSorter::addCustomSlideShow);
    connect(m_buttonDelCustomSlideShow, &QAbstractButton::clicked, this, &KPrViewModeSlidesSorter::removeCustomSlideShow);
    connect(m_buttonAddSlideToCurrentShow, &QAbstractButton::clicked, this, &KPrViewModeSlidesSorter::addSlideToCustomShow);
    connect(m_buttonDelSlideFromCurrentShow, &QAbstractButton::clicked, this, &KPrViewModeSlidesSorter::deleteSlidesFromCustomShow);
    connect(m_customSlideShowModel, &KPrCustomSlideShowsModel::customSlideShowsChanged, this, &KPrViewModeSlidesSorter::updateCustomSlideShowsList);
    connect(m_customSlideShowModel, &KPrCustomSlideShowsModel::selectPages, this, &KPrViewModeSlidesSorter::selectCustomShowPages);

    // setup signals for manage edit actions
    connect(view->copyController(), &KoCopyController::copyRequested, this, &KPrViewModeSlidesSorter::editCopy);
    connect(view->cutController(), &KoCopyController::copyRequested, this, &KPrViewModeSlidesSorter::editCut);
    connect(view, &KoPAView::selectAllRequested, m_slidesSorterView, &QAbstractItemView::selectAll);
    connect(view, &KoPAView::deselectAllRequested, m_slidesSorterView, &QAbstractItemView::clearSelection);
    connect(m_slidesSorterView, &KPrSlidesManagerView::selectionCleared, this, &KPrViewModeSlidesSorter::disableEditActions);
    connect(m_slidesSorterView, &KPrSlidesManagerView::itemSelected, this, &KPrViewModeSlidesSorter::enableEditActions);
    connect(m_slidesSorterView, &KPrSlidesManagerView::focusLost, this, &KPrViewModeSlidesSorter::disableEditActions);
    connect(m_slidesSorterView, &KPrSlidesManagerView::focusGot, this, &KPrViewModeSlidesSorter::manageAddRemoveSlidesButtons);
    connect(m_slidesSorterView, &KPrSlidesManagerView::zoomIn, m_view->zoomController()->zoomAction(), &KoZoomAction::zoomIn);
    connect(m_slidesSorterView, &KPrSlidesManagerView::zoomOut, m_view->zoomController()->zoomAction(), &KoZoomAction::zoomOut);
    connect(m_customSlideShowView, &KPrSlidesManagerView::focusGot, this, &KPrViewModeSlidesSorter::disableEditActions);
    connect(m_customSlideShowView, &KPrSlidesManagerView::focusGot, this, &KPrViewModeSlidesSorter::manageAddRemoveSlidesButtons);

    // install selection manager for Slides Sorter View and Custom Shows View
    m_slidesSorterItemContextBar = new KoViewItemContextBar(m_slidesSorterView);
    new KoViewItemContextBar(m_customSlideShowView);
    QToolButton *duplicateButton = m_slidesSorterItemContextBar->addContextButton(i18n("Duplicate Slide"), QString("edit-copy"));
    QToolButton *deleteButton = m_slidesSorterItemContextBar->addContextButton(i18n("Delete Slide"), QString("edit-delete"));
    QToolButton *startPresentation = m_slidesSorterItemContextBar->addContextButton(i18n("Start Slideshow"), QString("view-presentation"));
    connect(view->kopaDocument(), QOverload<KoPAPageBase *, int>::of(&KoPADocument::pageRemoved), m_slidesSorterItemContextBar, &KoViewItemContextBar::update);

    // setup signals for item context bar buttons
    connect(duplicateButton, &QAbstractButton::clicked, this, &KPrViewModeSlidesSorter::contextBarDuplicateSlide);
    connect(deleteButton, &QAbstractButton::clicked, this, &KPrViewModeSlidesSorter::contextBarDeleteSlide);
    connect(startPresentation, &QAbstractButton::clicked, this, &KPrViewModeSlidesSorter::contextBarStartSlideshow);

    // install delegate for Slides Sorter View
    KPrSlidesSorterItemDelegate *slidesSorterDelegate = new KPrSlidesSorterItemDelegate(m_slidesSorterView);
    m_slidesSorterView->setItemDelegate(slidesSorterDelegate);
}

KPrViewModeSlidesSorter::~KPrViewModeSlidesSorter()
{
    // save zoom value
    saveZoomConfig(zoom());
    delete m_centralWidget;
}

void KPrViewModeSlidesSorter::paint(KoPACanvasBase * /*canvas*/, QPainter & /*painter*/, const QRectF & /*paintRect*/)
{
}

void KPrViewModeSlidesSorter::paintEvent(KoPACanvas *canvas, QPaintEvent *event)
{
    Q_UNUSED(canvas);
    Q_UNUSED(event);
    Q_ASSERT(m_canvas == canvas);
}

void KPrViewModeSlidesSorter::tabletEvent(QTabletEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::mousePressEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::mouseMoveEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::mouseReleaseEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::shortcutOverrideEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeSlidesSorter::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeSlidesSorter::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeSlidesSorter::wheelEvent(QWheelEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::activate(KoPAViewMode *previousViewMode)
{
    Q_UNUSED(previousViewMode);
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->replaceCentralWidget(m_centralWidget);
    }
    m_slidesSorterView->setFocus(Qt::ActiveWindowFocusReason);
    updateToActivePageIndex();

    // setup signals
    connect(m_slidesSorterView, &KPrSlidesManagerView::indexChanged, this, &KPrViewModeSlidesSorter::itemClicked);
    connect(m_slidesSorterView, &QAbstractItemView::pressed, this, &KPrViewModeSlidesSorter::itemClicked);
    connect(m_view->proxyObject, &KoPAViewProxyObject::activePageChanged, this, &KPrViewModeSlidesSorter::updateToActivePageIndex);

    // change zoom saving slot
    connect(m_view->zoomController(), &KoZoomController::zoomChanged, this, &KPrViewModeSlidesSorter::updateZoom);

    KPrView *kPrview = dynamic_cast<KPrView *>(m_view);
    if (kPrview) {
        disconnect(kPrview->zoomController(), &KoZoomController::zoomChanged, kPrview, &KPrView::zoomChanged);
        m_view->zoomController()->zoomAction()->setZoomModes(KoZoomMode::ZOOM_CONSTANT);
        loadZoomConfig();
        disconnect(kPrview->deleteSelectionAction(), &QAction::triggered, kPrview, &KPrView::editDeleteSelection);
        connect(kPrview->deleteSelectionAction(), &QAction::triggered, this, &KPrViewModeSlidesSorter::deleteSlide);
    }
    m_view->setActionEnabled(KoPAView::AllActions, false);
}

void KPrViewModeSlidesSorter::deactivate()
{
    // Give the resources back to the canvas
    m_canvas->resourceManager()->setResource(KoCanvasResourceManager::ShowTextShapeOutlines, QVariant(false));
    // Active the view as a basic but active one
    m_view->setActionEnabled(KoPAView::AllActions, true);
    m_view->doUpdateActivePage(m_view->activePage());
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->restoreCentralWidget();
    }

    // save zoom value
    saveZoomConfig(zoom());

    // change zoom saving slot and restore normal view zoom values
    disconnect(m_view->zoomController(), &KoZoomController::zoomChanged, this, &KPrViewModeSlidesSorter::updateZoom);
    m_view->zoomController()->zoomAction()->setZoomModes(KoZoomMode::ZOOM_PAGE | KoZoomMode::ZOOM_WIDTH);
    m_view->setActivePage(m_view->kopaDocument()->pageByIndex(m_slidesSorterView->currentIndex().row(), false));

    KPrView *kPrview = dynamic_cast<KPrView *>(m_view);
    if (kPrview) {
        kPrview->restoreZoomConfig();
        connect(kPrview->zoomController(), &KoZoomController::zoomChanged, kPrview, &KPrView::zoomChanged);
        connect(kPrview->deleteSelectionAction(), &QAction::triggered, kPrview, &KPrView::editDeleteSelection);
        disconnect(kPrview->deleteSelectionAction(), &QAction::triggered, this, &KPrViewModeSlidesSorter::deleteSlide);
    }
    disableEditActions();
}

void KPrViewModeSlidesSorter::updateActivePage(KoPAPageBase *page)
{
    if (m_view->activePage() != page) {
        m_view->setActivePage(page);
    }
    updateToActivePageIndex();
}

void KPrViewModeSlidesSorter::updateToActivePageIndex()
{
    int row = m_view->kopaDocument()->pageIndex(m_view->activePage());
    QModelIndex index = m_slidesSorterModel->index(row, 0, QModelIndex());
    m_slidesSorterView->setCurrentIndex(index);
}

void KPrViewModeSlidesSorter::updateActivePageToCurrentIndex()
{
    QModelIndex c_index = m_slidesSorterView->currentIndex();
    m_view->setActivePage(m_view->kopaDocument()->pageByIndex(c_index.row(), false));
}

void KPrViewModeSlidesSorter::addShape(KoShape *shape)
{
    Q_UNUSED(shape);
}

void KPrViewModeSlidesSorter::removeShape(KoShape *shape)
{
    Q_UNUSED(shape);
}

QSize KPrViewModeSlidesSorter::iconSize() const
{
    return m_iconSize;
}

void KPrViewModeSlidesSorter::selectSlides(const QList<KoPAPageBase *> &slides)
{
    if (slides.isEmpty()) {
        return;
    }

    m_slidesSorterView->clearSelection();

    foreach (KoPAPageBase *slide, slides) {
        int row = m_view->kopaDocument()->pageIndex(slide);
        QModelIndex index = m_slidesSorterModel->index(row, 0, QModelIndex());
        if (index.isValid()) {
            m_slidesSorterView->selectionModel()->select(index, QItemSelectionModel::Select);
        }
    }
}

void KPrViewModeSlidesSorter::selectCustomShowPages(int start, int count)
{
    if ((start < 0) || (count < 1)) {
        return;
    }

    m_customSlideShowView->clearSelection();

    for (int i = start; i < (start + count); ++i) {
        QModelIndex index = m_customSlideShowModel->index(i, 0, QModelIndex());
        if (index.isValid()) {
            m_customSlideShowView->selectionModel()->select(index, QItemSelectionModel::Select);
        }
    }
}

void KPrViewModeSlidesSorter::activateNormalViewMode()
{
    KPrView *view = static_cast<KPrView *>(m_view);
    view->showNormal();
}

void KPrViewModeSlidesSorter::itemClicked(const QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }

    // Avoid deselect slides when dragging
    if (m_slidesSorterView->selectionModel()->selectedIndexes().length() > 1) {
        return;
    }

    KoPAPageBase *page = m_view->kopaDocument()->pageByIndex(index.row(), false);
    if (page) {
        m_view->setActivePage(page);
    }
    enableEditActions();
}

QList<KoPAPageBase *> KPrViewModeSlidesSorter::extractSelectedSlides()
{
    QList<KoPAPageBase *> slides;
    QModelIndexList selectedItems = m_slidesSorterView->selectionModel()->selectedIndexes();
    if (selectedItems.count() == 0) {
        return slides;
    }

    foreach (const QModelIndex &index, selectedItems) {
        KoPAPageBase *page = m_view->kopaDocument()->pageByIndex(index.row(), false);
        if (page) {
            slides.append(page);
        }
    }

    // order slides
    QMap<int, KoPAPageBase *> map;
    foreach (KoPAPageBase *slide, slides)
        map.insert(m_view->kopaDocument()->pages(false).indexOf(slide), slide);
    slides = map.values();

    return slides;
}

void KPrViewModeSlidesSorter::deleteSlide()
{
    if (m_slidesSorterView->hasFocus()) {
        // create a list with all selected slides
        QList<KoPAPageBase *> selectedSlides = extractSelectedSlides();
        m_slidesSorterModel->removeSlides(selectedSlides);
    } else if (m_customSlideShowView->hasFocus()) {
        deleteSlidesFromCustomShow();
    }
}

void KPrViewModeSlidesSorter::addSlide()
{
    m_slidesSorterModel->addNewSlide();
}

void KPrViewModeSlidesSorter::renameCurrentSlide()
{
    QModelIndexList selectedItems = m_slidesSorterView->selectionModel()->selectedIndexes();
    m_slidesSorterView->edit(selectedItems.first());
}

void KPrViewModeSlidesSorter::editCut()
{
    editCopy();
    deleteSlide();
}

void KPrViewModeSlidesSorter::editCopy()
{
    // separate selected layers and selected shapes
    QList<KoPAPageBase *> slides = extractSelectedSlides();
    m_slidesSorterModel->copySlides(slides);
}

void KPrViewModeSlidesSorter::editPaste()
{
    m_slidesSorterModel->pasteSlides();
}

void KPrViewModeSlidesSorter::updateZoom(KoZoomMode::Mode mode, qreal zoom)
{
    Q_UNUSED(mode);
    // at zoom 100%, iconSize is set in 200 x 200
    // KPrSlidesSorterDocumentModel uses iconSize function in decorate Role.
    // Check if is enough room for context bar
    int newIconSize = (zoom * DEFAULT_ICON_SIZE > m_slidesSorterItemContextBar->preferredWidth()) ? qRound(zoom * DEFAULT_ICON_SIZE)
                                                                                                  : m_slidesSorterItemContextBar->preferredWidth();
    // Check if slide is not too big
    newIconSize = (newIconSize < qMin(m_centralWidget->size().height(), m_centralWidget->size().width()))
        ? newIconSize
        : qMin(m_centralWidget->size().height(), m_centralWidget->size().width());

    setIconSize(QSize(newIconSize, newIconSize));
    m_slidesSorterView->setIconSize(iconSize());
    m_customSlideShowModel->setIconSize(iconSize());
    m_customSlideShowView->setIconSize(iconSize());
    setZoom(qRound(zoom * 100.));
}

void KPrViewModeSlidesSorter::setIconSize(const QSize &size)
{
    if (size != m_iconSize) {
        m_iconSize = size;
    }
}

void KPrViewModeSlidesSorter::loadZoomConfig()
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    int s_zoom = 100;

    if (config->hasGroup("Interface")) {
        const KConfigGroup interface = config->group("Interface");
        s_zoom = interface.readEntry("ZoomSlidesSorter", s_zoom);
    }
    m_view->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, s_zoom / 100.);
}

void KPrViewModeSlidesSorter::saveZoomConfig(int zoom)
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    KConfigGroup interface = config->group("Interface");
    interface.writeEntry("ZoomSlidesSorter", zoom);
}

void KPrViewModeSlidesSorter::setZoom(int zoom)
{
    m_zoom = zoom;
}

int KPrViewModeSlidesSorter::zoom()
{
    return m_zoom;
}

void KPrViewModeSlidesSorter::slidesSorterContextMenu(QContextMenuEvent *event)
{
    QMenu menu(m_slidesSorterView);
    menu.addAction(koIcon("document-new"), i18n("Add a new slide"), this, &KPrViewModeSlidesSorter::addSlide);
    menu.addAction(koIcon("edit-delete"), i18n("Delete selected slides"), this, &KPrViewModeSlidesSorter::deleteSlide);

    QModelIndexList selectedItems = m_slidesSorterView->selectionModel()->selectedIndexes();
    if (selectedItems.count() == 1 && selectedItems.first().isValid()) {
        menu.addAction(koIcon("edit-rename"), i18n("Rename"), this, &KPrViewModeSlidesSorter::renameCurrentSlide);
    }

    menu.addSeparator();
    menu.addAction(koIcon("edit-cut"), i18n("Cut"), this, &KPrViewModeSlidesSorter::editCut);
    menu.addAction(koIcon("edit-copy"), i18n("Copy"), this, &KPrViewModeSlidesSorter::editCopy);
    menu.addAction(koIcon("edit-paste"), i18n("Paste"), this, &KPrViewModeSlidesSorter::editPaste);
    menu.exec(event->globalPos());
    enableEditActions();
}

void KPrViewModeSlidesSorter::customSlideShowsContextMenu(QContextMenuEvent *event)
{
    QMenu menu(m_customSlideShowView);
    menu.addAction(koIcon("edit-delete"), i18n("Delete selected slides"), this, &KPrViewModeSlidesSorter::deleteSlidesFromCustomShow);
    menu.exec(event->globalPos());
}

void KPrViewModeSlidesSorter::enableEditActions()
{
    KActionCollection *ac = canvas()->canvasController()->actionCollection();
    ac->action("edit_copy")->setEnabled(true);
    ac->action("edit_cut")->setEnabled(true);
    ac->action("edit_delete")->setEnabled(true);
}

void KPrViewModeSlidesSorter::disableEditActions()
{
    KActionCollection *ac = canvas()->canvasController()->actionCollection();
    ac->action("edit_copy")->setEnabled(false);
    ac->action("edit_cut")->setEnabled(false);
    ac->action("edit_delete")->setEnabled(false);
}

void KPrViewModeSlidesSorter::customShowChanged(int showNumber)
{
    QString name = m_customSlideShowsList->itemText(showNumber);

    bool panelVisible = true;
    if (showNumber < 1) {
        panelVisible = false;
        name.clear();
    }

    // Change document current custom slide show
    KPrDocument *doc = static_cast<KPrDocument *>(m_view->kopaDocument());
    doc->setActiveCustomSlideShow(name);

    // Decide show or hide Custom Slide Shows View
    if (panelVisible != m_editCustomSlideShow) {
        const bool animate = m_centralWidget->style()->styleHint(QStyle::SH_Widget_Animate);
        const int duration = animate ? 250 : 1;
        QPropertyAnimation *animation = new QPropertyAnimation(m_customSlideShowView, "maximumHeight");

        if (!panelVisible) {
            animation->setDuration(duration);
            animation->setStartValue(m_customSlideShowView->maximumHeight());
            animation->setEndValue(0);
            // Deactivate tool buttons and edition
            disableEditCustomShowButtons();
            m_slidesSorterView->setAutoScroll(true);
        } else {
            animation->setDuration(duration);
            animation->setStartValue(0);
            animation->setEndValue(m_slidesSorterView->height() / 2);
            // Activate tool buttons and edition
            enableEditCustomShowButtons();
            m_slidesSorterView->setAutoScroll(false);
        }
        animation->start();
    }

    m_editCustomSlideShow = panelVisible;

    // Populate Custom Slide Shows View if visible
    if (panelVisible) {
        m_customSlideShowModel->setActiveSlideShow(showNumber - 1);
    }
}

void KPrViewModeSlidesSorter::deleteSlidesFromCustomShow()
{
    QModelIndexList selectedItems = m_customSlideShowView->selectionModel()->selectedIndexes();
    if (selectedItems.count() == 0) {
        return;
    }
    m_customSlideShowModel->removeSlidesByIndexes(selectedItems);
}

void KPrViewModeSlidesSorter::addSlideToCustomShow()
{
    // create a list with all selected slides
    QList<KoPAPageBase *> selectedSlides = extractSelectedSlides();
    int row = (m_customSlideShowView->currentIndex().row() >= 0) ? m_customSlideShowView->currentIndex().row() + 1 : 0;
    m_customSlideShowModel->addSlides(selectedSlides, row);
}

void KPrViewModeSlidesSorter::addCustomSlideShow()
{
    // We create a different default name for every SlideShow:
    static int newSlideShowsCount = 1;
    while (m_customSlideShowModel->customShowsNamesList().contains(i18n("Slide Show %1", newSlideShowsCount))) {
        ++newSlideShowsCount;
    }

    m_customSlideShowModel->addNewCustomShow(i18n("Slide Show %1", newSlideShowsCount));
}

void KPrViewModeSlidesSorter::removeCustomSlideShow()
{
    m_customSlideShowModel->removeCustomShow(m_customSlideShowsList->currentText());
}

void KPrViewModeSlidesSorter::updateCustomSlideShowsList()
{
    disconnect(m_customSlideShowsList, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KPrViewModeSlidesSorter::customShowChanged);

    QStringList slideShows;
    slideShows << i18n("All slides") << (m_customSlideShowModel->customShowsNamesList());
    m_customSlideShowsList->clear();
    m_customSlideShowsList->addItems(slideShows);
    int index = slideShows.indexOf(m_customSlideShowModel->activeCustomSlideShow());
    m_customSlideShowsList->setCurrentIndex(index >= 0 ? index : 0);
    customShowChanged(m_customSlideShowsList->currentIndex());

    connect(m_customSlideShowsList, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KPrViewModeSlidesSorter::customShowChanged);
}

void KPrViewModeSlidesSorter::renameCustomSlideShow()
{
    QString newName = m_customSlideShowsList->currentText();

    if (newName == m_customSlideShowModel->activeCustomSlideShow()) {
        return;
    }

    // Empty string is not allowed as a name, if the name is empty, revert back to previous name
    if (newName.isEmpty()) {
        updateCustomSlideShowsList();
    }
    // If the name is not already in use, use it, otherwise let the user know
    else if (!m_customSlideShowModel->customShowsNamesList().contains(newName)) {
        m_customSlideShowModel->renameCustomShow(m_customSlideShowModel->activeCustomSlideShow(), newName);
        updateCustomSlideShowsList();
    } else {
        KMessageBox::error(m_customSlideShowView, i18n("There cannot be two slideshows with the same name."), i18n("Error"), KMessageBox::Notify);
        updateCustomSlideShowsList();
    }
}

void KPrViewModeSlidesSorter::enableEditCustomShowButtons()
{
    m_customSlideShowsList->setEditable(true);
    connect(m_customSlideShowsList->lineEdit(), &QLineEdit::editingFinished, this, &KPrViewModeSlidesSorter::renameCustomSlideShow);
    m_buttonDelCustomSlideShow->setEnabled(true);
}

void KPrViewModeSlidesSorter::disableEditCustomShowButtons()
{
    m_customSlideShowsList->setEditable(false);
    m_buttonDelCustomSlideShow->setEnabled(false);
    m_buttonAddSlideToCurrentShow->setEnabled(false);
    m_buttonDelSlideFromCurrentShow->setEnabled(false);
}

void KPrViewModeSlidesSorter::manageAddRemoveSlidesButtons()
{
    m_buttonAddSlideToCurrentShow->setEnabled(m_slidesSorterView->hasFocus() && m_editCustomSlideShow);
    m_buttonDelSlideFromCurrentShow->setEnabled(m_customSlideShowView->hasFocus());
    KActionCollection *ac = canvas()->canvasController()->actionCollection();
    ac->action("edit_delete")->setEnabled(m_customSlideShowView->hasFocus() || !m_slidesSorterView->selectionModel()->selectedIndexes().isEmpty());
}

void KPrViewModeSlidesSorter::setActiveCustomSlideShow(int index)
{
    disconnect(m_customSlideShowsList, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KPrViewModeSlidesSorter::customShowChanged);

    m_customSlideShowsList->setCurrentIndex(index >= 0 && index < m_customSlideShowsList->count() ? index : 0);
    customShowChanged(m_customSlideShowsList->currentIndex());

    connect(m_customSlideShowsList, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KPrViewModeSlidesSorter::customShowChanged);
}

void KPrViewModeSlidesSorter::contextBarDuplicateSlide()
{
    QList<KoPAPageBase *> slides;
    KoPAPageBase *page = m_view->kopaDocument()->pageByIndex(m_slidesSorterItemContextBar->currentIndex().row(), false);
    if (page) {
        slides.append(page);
        updateActivePage(page);
        m_slidesSorterModel->copySlides(slides);
        editPaste();
    }
}

void KPrViewModeSlidesSorter::contextBarDeleteSlide()
{
    QList<KoPAPageBase *> slides;
    if ((m_slidesSorterItemContextBar->currentIndex().row() >= 0)
        && (m_slidesSorterItemContextBar->currentIndex().row() < m_slidesSorterModel->rowCount(QModelIndex()))) {
        KoPAPageBase *page = m_view->kopaDocument()->pageByIndex(m_slidesSorterItemContextBar->currentIndex().row(), false);
        if (page) {
            slides.append(page);
            m_slidesSorterModel->removeSlides(slides);
        }
    }
}

void KPrViewModeSlidesSorter::contextBarStartSlideshow()
{
    KoPAPageBase *page = m_view->kopaDocument()->pageByIndex(m_slidesSorterItemContextBar->currentIndex().row(), false);
    updateActivePage(page);
    KPrView *kPrview = dynamic_cast<KPrView *>(m_view);
    if (kPrview) {
        kPrview->startPresentation();
    }
}
