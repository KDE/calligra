/* This file is part of the KDE project
*
* Copyright (C) 2010 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
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

#include "KPrViewModeSlidesSorter.h"

#include <QtCore/QEvent>
#include <QtGui/QPainter>
#include <QVariant>
#include <QScrollBar>
#include <QMenu>
#include <QtCore/qmath.h>
#include <QGridLayout>
#include <QPushButton>
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QPropertyAnimation>

#include "KPrSlidesSorterDocumentModel.h"
#include "KPrFactory.h"
#include "KPrSlidesManagerView.h"
#include "KPrSelectionManager.h"
#include "KPrCustomSlideShowsModel.h"
#include "KPrDocument.h"
#include "KPrCustomSlideShows.h"
#include "commands/KPrSetCustomSlideShowsCommand.h"

#include <KoResourceManager.h>
#include <KoRuler.h>
#include <KoSelection.h>
#include <KoShapeLayer.h>
#include <KoShapeManager.h>
#include <KoText.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoZoomController.h>

#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPAMasterPage.h>
#include <KoPAView.h>
#include <KPrView.h>
#include <KoPAPageMoveCommand.h>
#include <KoPAPageDeleteCommand.h>
#include <KoPAOdfPageSaveHelper.h>
#include <KoDrag.h>

#include <klocale.h>
#include <KDebug>
#include <kconfiggroup.h>
#include <KIconLoader>
#include <KGlobalSettings>

KPrViewModeSlidesSorter::KPrViewModeSlidesSorter(KoPAView *view, KoPACanvas *canvas)
    : KoPAViewMode(view, canvas)
    , m_slidesSorter(new KPrSlidesManagerView())
    , m_customSlidesShowView(new KPrSlidesManagerView())
    , m_documentModel(new KPrSlidesSorterDocumentModel(this, view->parentWidget()))
    , m_centralWidget(new QWidget(view->parentWidget()))
    , m_customShowsModel(new KPrCustomSlideShowsModel(view->parentWidget()))
    , m_iconSize(QSize(200, 200))
    , m_editCustomShows(false)
{
    //Create customSlideShow GUI
    QWidget *m_customShowsToolBar = new QWidget();

    QHBoxLayout *toolBarLayout = new QHBoxLayout(m_customShowsToolBar);
    QVBoxLayout *centralWidgetLayout = new QVBoxLayout(m_centralWidget);

    QLabel *slideShowsLabel = new QLabel(i18n("Slide Show: "));
    QComboBox *slideShowsList = new QComboBox;

    QToolButton *buttonAdd = new QToolButton();
    buttonAdd->setIcon(SmallIcon("list-add"));
    buttonAdd->setEnabled(false);

    QToolButton *buttonDel = new QToolButton();
    buttonDel->setIcon(SmallIcon("list-remove"));
    buttonDel->setEnabled(false);

    QSplitter *viewsSplitter = new QSplitter(Qt::Vertical);

    //hide Custom Shows View
    m_customSlidesShowView->setMaximumHeight(0);

    //Layout Widgets

    toolBarLayout->addWidget(slideShowsLabel);
    toolBarLayout->addWidget(slideShowsList);
    toolBarLayout->addWidget(buttonAdd);
    toolBarLayout->addWidget(buttonDel);

    viewsSplitter->addWidget(m_slidesSorter);
    viewsSplitter->addWidget(m_customSlidesShowView);
    centralWidgetLayout->addWidget(viewsSplitter);
    centralWidgetLayout->addWidget(m_customShowsToolBar);

    //initialize widgets
    m_centralWidget->hide();
    m_slidesSorter->setIconSize(m_iconSize);
    m_customSlidesShowView->setIconSize(m_iconSize);


    //Populate ComboBox
    KPrDocument *document = dynamic_cast<KPrDocument *>(view->kopaDocument());
    if (document) {
        m_customShowsModel->setDocument(document);
    }

    QStringList slideShows;

    slideShows << i18n("Default") << (m_customShowsModel->customShowsNamesList());

    slideShowsList->addItems(slideShows);

    //Setup customSlideShows view
    m_customSlidesShowView->setModel(m_customShowsModel);

    m_customSlidesShowView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_customSlidesShowView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_customSlidesShowView->setDragDropMode(QAbstractItemView::InternalMove);

    //setup signals

    connect(m_slidesSorter, SIGNAL(requestContextMenu(QContextMenuEvent*)), this, SLOT(slidesSorterContextMenu(QContextMenuEvent*)));
    connect(m_customSlidesShowView, SIGNAL(requestContextMenu(QContextMenuEvent*)), this, SLOT(customSlideShowsContextMenu(QContextMenuEvent*)));
    connect(m_slidesSorter, SIGNAL(slideDblClick()), this, SLOT(activateNormalViewMode()));
    connect(slideShowsList, SIGNAL(currentIndexChanged(int)), this, SLOT(customShowChanged(int)));
    connect(slideShowsList, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeActiveSlideShow(QString)));


    //filter some m_slidesSorter key events
    m_slidesSorter->installEventFilter(this);
    m_customSlidesShowView->installEventFilter(this);

    //install selection manager for Slides Sorter View
    m_selectionManagerSlidesSorter = new KPrSelectionManager(m_slidesSorter, m_view->kopaDocument());

}

KPrViewModeSlidesSorter::~KPrViewModeSlidesSorter()
{
    //save zoom value
    saveZoomConfig(zoom());
}

void KPrViewModeSlidesSorter::paint(KoPACanvasBase* /*canvas*/, QPainter& /*painter*/, const QRectF &/*paintRect*/)
{
}

void KPrViewModeSlidesSorter::paintEvent( KoPACanvas * canvas, QPaintEvent* event )
{
    Q_UNUSED(canvas);
    Q_UNUSED(event);
    Q_ASSERT( m_canvas == canvas );
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
    populate();
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->hide();
    }
    //m_slidesSorter->show();
    m_centralWidget->show();
    m_slidesSorter->setFocus(Qt::ActiveWindowFocusReason);
    updateToActivePageIndex();

    connect(m_slidesSorter,SIGNAL(indexChanged(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
    connect(m_slidesSorter, SIGNAL(pressed(QModelIndex)), this, SLOT(itemClicked(const QModelIndex)));
    connect(m_view->proxyObject, SIGNAL(activePageChanged()), this, SLOT(updateToActivePageIndex()));
    connect(m_view->kopaDocument(),SIGNAL(pageAdded(KoPAPageBase*)),this, SLOT(updateSlidesSorterDocumentModel()));
    connect(m_view->kopaDocument(),SIGNAL(pageRemoved(KoPAPageBase*)),this, SLOT(updateSlidesSorterDocumentModel()));

    //change zoom saving slot
    connect(m_view->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)), this, SLOT(updateZoom(KoZoomMode::Mode, qreal)));

    KPrView *kPrview = dynamic_cast<KPrView *>(m_view);
    if (kPrview) {
        disconnect(kPrview->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)), kPrview, SLOT(zoomChanged(KoZoomMode::Mode, qreal)));
        m_view->zoomController()->zoomAction()->setZoomModes(KoZoomMode::ZOOM_CONSTANT);
        loadZoomConfig();
    }

}

void KPrViewModeSlidesSorter::deactivate()
{
    //m_slidesSorter->hide();
    m_centralWidget->hide();
    // Give the ressources back to the canvas
    m_canvas->resourceManager()->setResource(KoText::ShowTextFrames, 0);
    // Active the view as a basic but active one
    m_view->setActionEnabled(KoPAView::AllActions, true);
    m_view->doUpdateActivePage(m_view->activePage());
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->show();
    }

    //save zoom value
    saveZoomConfig(zoom());

    //change zoom saving slot and restore normal view zoom values
    disconnect(m_view->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)), this, SLOT(updateZoom(KoZoomMode::Mode, qreal)));

    m_view->zoomController()->zoomAction()->setZoomModes(KoZoomMode::ZOOM_PAGE | KoZoomMode::ZOOM_WIDTH);

    m_view->setActivePage(m_view->kopaDocument()->pageByIndex(m_slidesSorter->currentIndex().row(), false));

    KPrView *kPrview = dynamic_cast<KPrView *>(m_view);
    if (kPrview) {
        kPrview->restoreZoomConfig();
        connect(kPrview->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)), kPrview, SLOT(zoomChanged(KoZoomMode::Mode, qreal)));
    }


}

void KPrViewModeSlidesSorter::updateSlidesSorterDocumentModel()
{
    m_documentModel->update();
    updateToActivePageIndex();
}

void KPrViewModeSlidesSorter::updateActivePage( KoPAPageBase *page )
{
    if (m_view->activePage() != page) {
        m_view->setActivePage(page);
    }
    updateToActivePageIndex();
}

void KPrViewModeSlidesSorter::updateToActivePageIndex()
{
    int row = m_view->kopaDocument()->pageIndex(m_view->activePage());
    QModelIndex index = m_documentModel->index(row, 0, QModelIndex());
    m_slidesSorter->setCurrentIndex(index);
}

void KPrViewModeSlidesSorter::updateActivePageToCurrentIndex()
{
    QModelIndex c_index = m_slidesSorter->currentIndex();
    m_view->setActivePage(m_view->kopaDocument()->pageByIndex(c_index.row(), false));
}

void KPrViewModeSlidesSorter::addShape( KoShape *shape )
{
    Q_UNUSED(shape);
}

void KPrViewModeSlidesSorter::removeShape( KoShape *shape )
{
    Q_UNUSED(shape);
}

void KPrViewModeSlidesSorter::populate()
{
    //Init m_slidesSorter view
    m_documentModel->setDocument(m_view->kopaDocument());
    m_slidesSorter->setModel(m_documentModel);

    m_slidesSorter->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_slidesSorter->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_slidesSorter->setDragDropMode(QAbstractItemView::InternalMove);
    QModelIndex item = m_documentModel->index(0, 0, QModelIndex());
    m_slidesSorter->setItemSize(m_slidesSorter->visualRect(item));

    //Init m_customSlidesShowsView
    m_customSlidesShowView->setItemSize(m_slidesSorter->visualRect(item));

}

QSize KPrViewModeSlidesSorter::iconSize() const
{
    return m_iconSize;
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

    //Avoid deselect slides when dragging
    if (m_slidesSorter->selectionModel()->selectedIndexes().length () > 1) {
        return;
    }

    KoPAPageBase *page = m_view->kopaDocument()->pageByIndex(index.row(), false);

    if (page) {
        m_view->setActivePage(page);
    }
}

QList<KoPAPageBase *> KPrViewModeSlidesSorter::extractSelectedSlides()
{
    QList<KoPAPageBase *> slides;

    QModelIndexList selectedItems = m_slidesSorter->selectionModel()->selectedIndexes();
    if (selectedItems.count() == 0) {
        return slides;
    }

    foreach (const QModelIndex & index, selectedItems) {
        KoPAPageBase * page = m_view->kopaDocument()->pageByIndex(index.row (), false);
        if (page) {
            slides.append(page);
        }
    }

    //order slides
    QMap<int, KoPAPageBase*> map;
    foreach (KoPAPageBase* slide, slides)
        map.insert(m_view->kopaDocument()->pages(false).indexOf(slide), slide);
    slides = map.values();

    return slides;
}

void KPrViewModeSlidesSorter::deleteSlide()
{
    // create a list with all selected slides
    QList<KoPAPageBase*> selectedSlides = extractSelectedSlides();

    if (!selectedSlides.empty() && m_view->kopaDocument()->pages().count() > selectedSlides.count()) {
         QUndoCommand *cmd = new KoPAPageDeleteCommand(m_view->kopaDocument(), selectedSlides);
        if (cmd) {
            m_view->kopaDocument()->addCommand(cmd);
            m_customShowsModel->removeSlidesFromAll(selectedSlides);
        }
    }
}

void KPrViewModeSlidesSorter::addSlide()
{
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->insertPage();
    }
}

void KPrViewModeSlidesSorter::editCut()
{
    editCopy();
    deleteSlide();
}

void KPrViewModeSlidesSorter::editCopy()
{
    // separate selected layers and selected shapes
    QList<KoPAPageBase*> slides = extractSelectedSlides();;

    if (!slides.empty()) {
        // Copy Pages
        KoPAOdfPageSaveHelper saveHelper(m_view->kopaDocument (), slides);
        KoDrag drag;
        drag.setOdf(KoOdf::mimeType(m_view->kopaDocument()->documentType()), saveHelper);
        drag.addToClipboard();
    }
}

void KPrViewModeSlidesSorter::editPaste()
{
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->pagePaste();
    }
}

void KPrViewModeSlidesSorter::updateZoom(KoZoomMode::Mode mode, qreal zoom)
{
    Q_UNUSED(mode);

    //at zoom 100%, iconSize is set in 200 x 200
    //KPrSlidesSorterDocumentModel uses iconSize function in decorate Role.
    setIconSize(QSize(qRound(zoom*200),qRound(zoom*200)));
    m_slidesSorter->setIconSize(iconSize());

    //update item size
    QModelIndex item = m_documentModel->index(0, 0, QModelIndex());
    m_slidesSorter->setItemSize(m_slidesSorter->visualRect(item));

    setZoom(qRound(zoom * 100.));
}

void KPrViewModeSlidesSorter::setIconSize(QSize size)
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
    m_view->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, s_zoom/100.);
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
    QMenu menu(m_slidesSorter);

    // Not connected yet
    menu.addAction(KIcon("document-new"), i18n("Add a new slide"), this, SLOT(addSlide()));
    menu.addAction(KIcon("edit-delete"), i18n("Delete selected slides"), this, SLOT(deleteSlide()));

    menu.addAction(KIcon("edit-cut"), i18n( "Cut" ) ,this,  SLOT(editCut()));
    menu.addAction(KIcon("edit-copy"), i18n( "Copy" ), this,  SLOT(editCopy()));
    menu.addAction(KIcon("edit-paste"), i18n( "Paste" ), this, SLOT(editPaste()));

    menu.exec(event->globalPos());
}

void KPrViewModeSlidesSorter::customSlideShowsContextMenu(QContextMenuEvent *event)
{
    QMenu menu(m_customSlidesShowView);

    // Not connected yet
    menu.addAction(KIcon("edit-delete"), i18n("Delete selected slides"), this, SLOT(deleteSlideFromCustomShow()));
    menu.exec(event->globalPos());
}

bool KPrViewModeSlidesSorter::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_slidesSorter) {
        switch (event->type()) {
            case QEvent::KeyPress: {
                QKeyEvent *keyEv = static_cast<QKeyEvent *>(event);
                switch (keyEv->key()) {
                    case Qt::Key_Delete: {
                        deleteSlide();
                        break;
                    }

                    default:
                       break;
                }
                break;
            }

            default:
                break;
        }
    } //endIf

    if (watched == m_customSlidesShowView) {
        switch (event->type()) {
            case QEvent::KeyPress: {
                QKeyEvent *keyEv = static_cast<QKeyEvent *>(event);
                switch (keyEv->key()) {
                    case Qt::Key_Delete: {
                        deleteSlideFromCustomShow();
                        break;
                    }

                    default:
                       break;
                }
                break;
            }

            default:
                break;
        }
    } //endIf

    return QObject::eventFilter(watched, event);
}

void KPrViewModeSlidesSorter::customShowChanged(int showNumber)
{
    bool panelVisible = true;
    if (showNumber < 1) {
        panelVisible = false;
    }

    //Decide show or hide Custom Slide Shows View
    if (panelVisible != m_editCustomShows) {

        const bool animate = KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects;
        const int duration = animate ? 600 : 1;

        QPropertyAnimation *animation = new QPropertyAnimation (m_customSlidesShowView, "maximumHeight");

        if (!panelVisible) {
            animation->setDuration(duration);
            animation->setStartValue(m_customSlidesShowView->maximumHeight());
             animation->setEndValue(0);
        } else {
            animation->setDuration(duration);
            animation->setStartValue(0);
            animation->setEndValue(m_slidesSorter->height()/2);
        }
        animation->start();
    }

    m_editCustomShows = panelVisible;

    //Populate Custom Slide Shows View if visible

    if (panelVisible) {
        m_customShowsModel->setCurrentSlideShow(showNumber - 1);
    }
}

void KPrViewModeSlidesSorter::changeActiveSlideShow(QString name)
{
    //Change document current custom slide show
    if (name == i18n("Default"))
        name = QString();
    KPrDocument *doc = static_cast<KPrDocument *>(m_view->kopaDocument());
    if (doc) {
        doc->setActiveCustomSlideShow(name);
    }
}

void KPrViewModeSlidesSorter::deleteSlideFromCustomShow()
{
    QModelIndexList selectedItems = m_customSlidesShowView->selectionModel()->selectedIndexes();
    if (selectedItems.count() == 0) {
        return;
    }

    m_customShowsModel->removeIndexes(selectedItems);

}
