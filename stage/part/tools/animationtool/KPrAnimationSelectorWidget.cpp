/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Paul Mendez <paulestebanms@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrAnimationSelectorWidget.h"

// Stage Headers
#include "KPrCollectionItemModel.h"
#include "KPrFactory.h"
#include "KPrShapeAnimationDocker.h"
#include "KPrViewModePreviewShapeAnimations.h"
#include "StageDebug.h"
#include "animations/KPrShapeAnimation.h"
#include <KPrPredefinedAnimationsLoader.h>

// Qt Headers
#include <QCheckBox>
#include <QFont>
#include <QFontDatabase>
#include <QGridLayout>
#include <QListView>
#include <QListWidget>
#include <QToolButton>

// KF5 Headers
#include <KConfigGroup>
#include <KIconLoader>
#include <KLocalizedString>

// Calligra Headers
#include <KoComponentData.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoShapeLoadingContext.h>
#include <KoViewItemContextBar.h>
#include <KoXmlReader.h>

KPrAnimationSelectorWidget::KPrAnimationSelectorWidget(KPrShapeAnimationDocker *docker, KPrPredefinedAnimationsLoader *animationsData, QWidget *parent)
    : QWidget(parent)
    , m_docker(docker)
    , m_previewAnimation(nullptr)
    , m_showAutomaticPreview(false)
    , m_animationsData(animationsData)
    , m_collectionContextBar(nullptr)
    , m_collectionPreviewButton(nullptr)
    , m_subTypeContextBar(nullptr)
    , m_subTypePreviewButton(nullptr)
{
    QGridLayout *containerLayout = new QGridLayout;

    // Todo enable checkbox again when preview doesn't close popup
    m_previewCheckBox = new QCheckBox(i18n("Automatic animation preview"), this);
    m_previewCheckBox->setChecked(loadPreviewConfig());
    m_previewCheckBox->hide();
    m_showAutomaticPreview = false; // m_previewCheckBox->isChecked();

    QFont viewWidgetFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    qreal pointSize = QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont).pointSizeF();
    viewWidgetFont.setPointSizeF(pointSize);

    m_collectionChooser = new QListWidget;
    m_collectionChooser->setViewMode(QListView::IconMode);
    m_collectionChooser->setIconSize(QSize(KIconLoader::SizeLarge, KIconLoader::SizeLarge));
    m_collectionChooser->setSelectionMode(QListView::SingleSelection);
    m_collectionChooser->setResizeMode(QListView::Adjust);
    m_collectionChooser->setGridSize(QSize(75, 64));
    m_collectionChooser->setFixedWidth(90);
    m_collectionChooser->setMovement(QListView::Static);
    m_collectionChooser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_collectionChooser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_collectionChooser->setFont(viewWidgetFont);
    connect(m_collectionChooser, &QListWidget::itemClicked, this, &KPrAnimationSelectorWidget::activateShapeCollection);

    m_collectionView = new QListView;
    m_collectionView->setViewMode(QListView::IconMode);
    m_collectionView->setIconSize(QSize(KIconLoader::SizeLarge, KIconLoader::SizeLarge));
    m_collectionView->setDragDropMode(QListView::DragOnly);
    m_collectionView->setSelectionMode(QListView::SingleSelection);
    m_collectionView->setResizeMode(QListView::Adjust);
    m_collectionView->setGridSize(QSize(75, 64));
    m_collectionView->setWordWrap(true);
    m_collectionView->viewport()->setMouseTracking(true);
    m_collectionView->setFont(viewWidgetFont);
    connect(m_collectionView, &QAbstractItemView::clicked, this, &KPrAnimationSelectorWidget::setAnimation);

    m_subTypeView = new QListView;
    m_subTypeView->setViewMode(QListView::IconMode);
    m_subTypeView->setIconSize(QSize(KIconLoader::SizeLarge, KIconLoader::SizeLarge));
    m_subTypeView->setDragDropMode(QListView::DragOnly);
    m_subTypeView->setSelectionMode(QListView::SingleSelection);
    m_subTypeView->setResizeMode(QListView::Adjust);
    m_subTypeView->setGridSize(QSize(75, 64));
    m_subTypeView->setFixedHeight(79);
    m_subTypeView->setWordWrap(true);
    m_subTypeView->viewport()->setMouseTracking(true);
    m_subTypeView->hide();
    m_subTypeView->setFont(viewWidgetFont);
    connect(m_subTypeView, &QAbstractItemView::clicked, this, &KPrAnimationSelectorWidget::setAnimation);

    containerLayout->addWidget(m_collectionChooser, 0, 0, 2, 1);
    containerLayout->addWidget(m_collectionView, 0, 1, 1, 1);
    containerLayout->addWidget(m_subTypeView, 1, 1, 1, 1);
    containerLayout->addWidget(m_previewCheckBox, 2, 0, 1, 2);

    // set signals
    connect(m_collectionView, &QListView::entered, this, QOverload<const QModelIndex &>::of(&KPrAnimationSelectorWidget::automaticPreviewRequested));
    connect(m_subTypeView, &QListView::entered, this, QOverload<const QModelIndex &>::of(&KPrAnimationSelectorWidget::automaticPreviewRequested));
    connect(m_previewCheckBox, &QAbstractButton::toggled, this, &KPrAnimationSelectorWidget::setPreviewState);
    connect(docker, &KPrShapeAnimationDocker::previousStateChanged, this, &KPrAnimationSelectorWidget::setPreviewState);
    setLayout(containerLayout);
}

KPrAnimationSelectorWidget::~KPrAnimationSelectorWidget()
{
    savePreviewConfig();
    // stop animation before deleting it.
    if (m_docker->previewMode()) {
        m_docker->previewMode()->stopAnimation();
    }
    delete m_previewAnimation;
}

void KPrAnimationSelectorWidget::init()
{
    // load predefined animations data
    m_animationsData->populateMainView(m_collectionChooser);
    m_collectionChooser->setCurrentRow(0);
    activateShapeCollection(m_collectionChooser->item(0));
    // Init context bar
    if (!m_showAutomaticPreview) {
        createCollectionContextBar();
    }
}

void KPrAnimationSelectorWidget::automaticPreviewRequested(const QModelIndex &index)
{
    // Test if index is valid an show automatic preview checkbox is checked
    if (!index.isValid() || !m_showAutomaticPreview) {
        return;
    }
    // Parse xml context in a new animation
    KoXmlElement newAnimationContext;
    if (QObject::sender() == m_collectionView) {
        newAnimationContext = static_cast<KPrCollectionItemModel *>(m_collectionView->model())->animationContext(index);
    } else if (QObject::sender() == m_subTypeView) {
        newAnimationContext = static_cast<KPrCollectionItemModel *>(m_subTypeView->model())->animationContext(index);
    } else {
        return;
    }

    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, nullptr);
    KoShapeLoadingContext shapeContext(context, nullptr);

    KoShape *shape = m_docker->getSelectedShape();
    if (!shape) {
        return;
    }
    m_previewAnimation = m_animationsData->loadOdfShapeAnimation(newAnimationContext, shapeContext, shape);
    if (m_previewAnimation) {
        Q_EMIT requestPreviewAnimation(m_previewAnimation);
    }
}

void KPrAnimationSelectorWidget::automaticPreviewRequested()
{
    QModelIndex index;
    KoXmlElement newAnimationContext;
    if (QObject::sender() == m_collectionPreviewButton) {
        index = m_collectionContextBar->currentIndex();
        if (!index.isValid()) {
            return;
        }
        newAnimationContext = static_cast<KPrCollectionItemModel *>(m_collectionView->model())->animationContext(index);
    } else if (QObject::sender() == m_subTypePreviewButton) {
        index = m_subTypeContextBar->currentIndex();
        if (!index.isValid()) {
            return;
        }
        newAnimationContext = static_cast<KPrCollectionItemModel *>(m_subTypeView->model())->animationContext(index);
    } else {
        return;
    }
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, nullptr);
    KoShapeLoadingContext shapeContext(context, nullptr);

    KoShape *shape = m_docker->getSelectedShape();
    if (!shape) {
        return;
    }
    m_previewAnimation = m_animationsData->loadOdfShapeAnimation(newAnimationContext, shapeContext, shape);
    if (m_previewAnimation) {
        Q_EMIT requestPreviewAnimation(m_previewAnimation);
    }
}

void KPrAnimationSelectorWidget::activateShapeCollection(QListWidgetItem *item)
{
    if (!item) {
        return;
    }
    QString id = item->data(Qt::UserRole).toString();
    m_collectionView->setModel(m_animationsData->modelById(id));
    m_subTypeView->setModel(nullptr);
    m_subTypeView->hide();
}

void KPrAnimationSelectorWidget::setAnimation(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    KoXmlElement newAnimationContext;
    if (QObject::sender() == m_collectionView) {
        m_subTypeView->hide();
        QString id = static_cast<KPrCollectionItemModel *>(m_collectionView->model())->data(index, Qt::UserRole).toString();
        if (m_animationsData->subModelById(id)) {
            m_subTypeView->setModel(m_animationsData->subModelById(id));
            m_subTypeView->show();
            if (!m_showAutomaticPreview && !m_subTypeContextBar) {
                createSubTypeContextBar();
            }
            return;
        }
        newAnimationContext = static_cast<KPrCollectionItemModel *>(m_collectionView->model())->animationContext(index);
    } else if (QObject::sender() == m_subTypeView) {
        newAnimationContext = static_cast<KPrCollectionItemModel *>(m_subTypeView->model())->animationContext(index);
    } else {
        return;
    }

    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, nullptr);
    KoShapeLoadingContext shapeContext(context, nullptr);

    KoShape *shape = m_docker->getSelectedShape();
    if (!shape) {
        warnStageAnimation << "No shape found";
        return;
    }

    KPrShapeAnimation *newAnimation = m_animationsData->loadOdfShapeAnimation(newAnimationContext, shapeContext, shape);
    if (newAnimation) {
        Q_EMIT requestAcceptAnimation(newAnimation);
    }
}

void KPrAnimationSelectorWidget::setPreviewState(bool isEnable)
{
    if (m_showAutomaticPreview == isEnable) {
        return;
    }
    m_showAutomaticPreview = isEnable;
    m_previewCheckBox->setChecked(isEnable);
    if (!isEnable) {
        if (!m_collectionContextBar) {
            createCollectionContextBar();
        }
        if (!m_subTypeContextBar && m_subTypeView->model()) {
            createSubTypeContextBar();
        }
    } else {
        delete m_collectionContextBar;
        delete m_collectionPreviewButton;
        m_collectionContextBar = nullptr;
        m_collectionPreviewButton = nullptr;

        delete m_subTypeContextBar;
        delete m_subTypePreviewButton;
        m_subTypeContextBar = nullptr;
        m_subTypePreviewButton = nullptr;
    }
    Q_EMIT previousStateChanged(isEnable);
}

bool KPrAnimationSelectorWidget::loadPreviewConfig()
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    bool showPreview = true;

    if (config->hasGroup("Interface")) {
        const KConfigGroup interface = config->group("Interface");
        showPreview = interface.readEntry("ShowAutomaticPreviewAnimationEditDocker", showPreview);
    }
    return showPreview;
}

void KPrAnimationSelectorWidget::savePreviewConfig()
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    KConfigGroup interface = config->group("Interface");
    interface.writeEntry("ShowAutomaticPreviewAnimationEditDocker", m_showAutomaticPreview);
}

void KPrAnimationSelectorWidget::createCollectionContextBar()
{
    m_collectionContextBar = new KoViewItemContextBar(m_collectionView);
    m_collectionPreviewButton = m_collectionContextBar->addContextButton(i18n("Preview animation"), QString("media-playback-start"));
    m_collectionContextBar->setShowSelectionToggleButton(false);
    connect(m_collectionPreviewButton, &QToolButton::clicked, this, QOverload<>::of(&KPrAnimationSelectorWidget::automaticPreviewRequested));
}

void KPrAnimationSelectorWidget::createSubTypeContextBar()
{
    m_subTypeContextBar = new KoViewItemContextBar(m_subTypeView);
    m_subTypePreviewButton = m_subTypeContextBar->addContextButton(i18n("Preview animation"), QString("media-playback-start"));
    m_subTypeContextBar->setShowSelectionToggleButton(false);
    connect(m_subTypePreviewButton, &QToolButton::clicked, this, QOverload<>::of(&KPrAnimationSelectorWidget::automaticPreviewRequested));
}
