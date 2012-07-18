/* This file is part of the KDE project
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// Animations Collection based in ShapeCollectionDocker

#include "KPrEditAnimationsWidget.h"

//Stage Headers
#include "KPrShapeAnimationDocker.h"
#include "KPrPage.h"
#include "KPrView.h"
#include "KPrDocument.h"
#include "KPrViewModePreviewShapeAnimations.h"
#include "animations/KPrShapeAnimation.h"
#include "KPrEditAnimationsWidget.h"
#include "KPrCollectionItemModel.h"
#include "KPrAnimationGroupProxyModel.h"
#include "KPrViewModePreviewShapeAnimations.h"
#include "KPrAnimationsTimeLineView.h"
#include "animations/KPrAnimationBase.h"
#include "animations/KPrShapeAnimation.h"
#include "animations/KPrAnimationFactory.h"
#include "commands/KPrAnimationRemoveCommand.h"
#include "KPrFactory.h"

//Qt Headers
#include <QToolButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTreeView>
#include <QMenu>
#include <QModelIndex>
#include <QDebug>
#include <QComboBox>
#include <QTimeEdit>
#include <QTime>
#include <QTabWidget>
#include <QListView>
#include <QListWidgetItem>
#include <QListWidget>
#include <QFile>
#include <QCheckBox>

//KDE Headers
#include <KIcon>
#include <KLocale>
#include <KIconLoader>
#include <KStandardDirs>
#include <KComponentData>
#include <KConfigGroup>
#include <KSharedConfigPtr>
#include <KDebug>

//Calligra Headers
#include <KoToolManager.h>
#include <KoSelection.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoShapeManager.h>
#include <KoPAViewBase.h>
#include <KoPACanvasBase.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoXmlNS.h>
#include <KoOdfStylesReader.h>
#include <KoStore.h>

KPrEditAnimationsWidget::KPrEditAnimationsWidget(KPrShapeAnimationDocker *docker, QWidget *parent)
    : QWidget(parent)
    , m_view(0)
    , m_docker(docker)
    , m_previewAnimation(0)
    , showAutomaticPreview(true)
{
    m_animations = QList<KPrShapeAnimation *>();
    m_animationContext = QList<KoXmlElement>();

    QVBoxLayout *layout = new QVBoxLayout;
    m_timeLineView = new KPrAnimationsTimeLineView();
    QLabel *label = new QLabel(i18n("Manage animation delay and duration: "));
    QLabel *startLabel = new QLabel(i18n("Start: "));
    m_triggerEventList = new QComboBox;
    m_triggerEventList->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_triggerEventList->setEditable(false);
    m_triggerEventList->addItem(KIcon("onclick"), i18n("on mouse click"));
    m_triggerEventList->addItem(KIcon("after_previous"), i18n("after previous animation"));
    m_triggerEventList->addItem(KIcon("with_previous"), i18n("with previous animation"));

    QLabel *delayLabel = new QLabel(i18n("Delay: "));
    m_delayEdit = new QTimeEdit;
    m_delayEdit->setTimeRange(QTime(0,0,0), QTime(0,30,0));
    m_delayEdit->setDisplayFormat("mm:ss.zzz");

    QLabel *durationLabel = new QLabel(i18n("Duration: "));
    m_durationEdit = new QTimeEdit;
    m_durationEdit->setTimeRange(QTime(0,0,0), QTime(1,0,0));
    m_durationEdit->setDisplayFormat("H:mm:ss.zzz");

    QToolButton *m_buttonPreviewAnimation = new QToolButton();
    m_buttonPreviewAnimation->setIcon(SmallIcon("media-playback-start"));
    m_buttonPreviewAnimation->setToolTip(i18n("Preview Shape Animation"));
    m_buttonPreviewAnimation->setEnabled(true);
    QCheckBox *previewCheckBox = new QCheckBox(i18n("Automatic animation preview"), this);
    previewCheckBox->setChecked(loadPreviewConfig());
    showAutomaticPreview = previewCheckBox->isChecked();

    QHBoxLayout *containerLayout = new QHBoxLayout;
    m_collectionChooser = new QListWidget;
    containerLayout->addWidget(m_collectionChooser);
    m_collectionChooser->setViewMode(QListView::IconMode);
    m_collectionChooser->setIconSize(QSize(KIconLoader::SizeLarge, KIconLoader::SizeLarge));
    m_collectionChooser->setSelectionMode(QListView::SingleSelection);
    m_collectionChooser->setResizeMode(QListView::Adjust);
    m_collectionChooser->setGridSize(QSize(75, 64));
    m_collectionChooser->setFixedWidth(90);
    m_collectionChooser->setMovement(QListView::Static);
    m_collectionChooser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_collectionChooser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(m_collectionChooser, SIGNAL(itemClicked(QListWidgetItem *)),
            this, SLOT(activateShapeCollection(QListWidgetItem *)));
    m_collectionView = new QListView;
    containerLayout->addWidget(m_collectionView);
    m_collectionView->setViewMode(QListView::IconMode);
    m_collectionView->setIconSize(QSize(KIconLoader::SizeLarge, KIconLoader::SizeLarge));
    m_collectionView->setDragDropMode(QListView::DragOnly);
    m_collectionView->setSelectionMode(QListView::SingleSelection);
    m_collectionView->setResizeMode(QListView::Adjust);
    m_collectionView->setGridSize(QSize(75, 64));
    m_collectionView->setWordWrap(true);
    m_collectionView->viewport()->setMouseTracking(true);
    connect(m_collectionView, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(setAnimation(const QModelIndex&)));

    // layout widgets
    layout->addLayout(containerLayout);
    QHBoxLayout *hlayout0 = new QHBoxLayout;
    hlayout0->addWidget(m_buttonPreviewAnimation);
    hlayout0->addStretch();
    hlayout0->addWidget(previewCheckBox);
    layout->addLayout(hlayout0);
    layout->addWidget(label);
    layout->addWidget(m_timeLineView);
    layout->addWidget(startLabel);
    layout->addWidget(m_triggerEventList);
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(delayLabel);
    hlayout->addWidget(m_delayEdit);
    hlayout->addWidget(durationLabel);
    hlayout->addWidget(m_durationEdit);
    layout->addLayout(hlayout);
    setLayout(layout);

    //Connect Signals.
    connect(m_buttonPreviewAnimation, SIGNAL(clicked()), this, SIGNAL(requestAnimationPreview()));
    connect(m_timeLineView, SIGNAL(clicked(QModelIndex)), this, SIGNAL(itemClicked(QModelIndex)));
    connect(m_timeLineView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateIndex(QModelIndex)));
    connect(m_timeLineView, SIGNAL(timeValuesChanged(QModelIndex)), this, SLOT(updateIndex(QModelIndex)));
    connect(m_timeLineView, SIGNAL(layoutChanged()), this, SLOT(syncCurrentItem()));
    connect(m_delayEdit, SIGNAL(editingFinished()), this, SLOT(setBeginTime()));
    connect(m_durationEdit, SIGNAL(editingFinished()), this, SLOT(setDuration()));
    connect(m_triggerEventList, SIGNAL(currentIndexChanged(int)), this, SLOT(setTriggerEvent(int)));
    connect(m_collectionView, SIGNAL(entered(QModelIndex)), this, SLOT(automaticPreviewRequested(QModelIndex)));
    connect(previewCheckBox, SIGNAL(toggled(bool)), this, SLOT(setPreviewState(bool)));

    loadDefaultAnimations();
}

KPrEditAnimationsWidget::~KPrEditAnimationsWidget()
{
    savePreviewConfig();
    // stop animation before delete it
    if (m_docker->previewMode()) {
        m_docker->previewMode()->stopAnimation();
    }
    if (m_previewAnimation) {
        delete(m_previewAnimation);
    }
}

void KPrEditAnimationsWidget::setView(KoPAViewBase *view)
{
    KPrView *n_view = dynamic_cast<KPrView *>(view);
    if (n_view) {
        m_view = n_view;
    }
}

void KPrEditAnimationsWidget::setCurrentIndex(const QModelIndex &index)
{
    Q_ASSERT(m_timeLineModel);
    m_timeLineView->setCurrentIndex(index);
    updateIndex(index);
}

void KPrEditAnimationsWidget::setProxyModel(KPrAnimationGroupProxyModel *model)
{
    m_timeLineModel = model;
    m_timeLineView->setModel(m_timeLineModel);
}

void KPrEditAnimationsWidget::updateView()
{
    m_timeLineView->update();
    updateGeometry();
}

QModelIndex KPrEditAnimationsWidget::currentIndex()
{
    return m_timeLineView->currentIndex();
}

void KPrEditAnimationsWidget::updateIndex(const QModelIndex &index)
{
    if (index.isValid() && (index.row() == m_timeLineView->currentIndex().row())) {
        QModelIndex triggerIndex = m_timeLineModel->index(index.row(), KPrShapeAnimations::Node_Type);
        QModelIndex beginTimeIndex = m_timeLineModel->index(index.row(), KPrShapeAnimations::StartTime);
        QModelIndex durationIndex = m_timeLineModel->index(index.row(), KPrShapeAnimations::Duration);
        m_triggerEventList->setCurrentIndex(m_timeLineModel->data(triggerIndex).toInt());
        m_delayEdit->setTime(QTime().addMSecs(m_timeLineModel->data(beginTimeIndex).toInt()));
        m_durationEdit->setTime(QTime().addMSecs(m_timeLineModel->data(durationIndex).toInt()));
    }
}

void KPrEditAnimationsWidget::setBeginTime()
{
    if (m_timeLineView->currentIndex().isValid()) {
        m_docker->mainModel()->setBeginTime(m_timeLineModel->mapToSource(m_timeLineView->currentIndex()), -m_delayEdit->time().msecsTo(QTime()));
    }
}

void KPrEditAnimationsWidget::setDuration()
{
    if (m_timeLineView->currentIndex().isValid()) {
        m_docker->mainModel()->setDuration(m_timeLineModel->mapToSource(m_timeLineView->currentIndex()), -m_durationEdit->time().msecsTo(QTime()));
    }
}

void KPrEditAnimationsWidget::setTriggerEvent(int row)
{
    QModelIndex index = m_timeLineView->currentIndex();
    if ((row >= 0) && index.isValid()) {
        QModelIndex triggerIndex = m_timeLineModel->index(index.row(), KPrShapeAnimations::Node_Type);
        if (row != m_timeLineModel->data(triggerIndex).toInt()) {
            KPrShapeAnimation::Node_Type newType;
            if (row == 0) newType = KPrShapeAnimation::On_Click;
            else if (row == 1) newType = KPrShapeAnimation::After_Previous;
            else newType = KPrShapeAnimation::With_Previous;
            m_docker->mainModel()->setTriggerEvent(m_timeLineModel->mapToSource(m_timeLineView->currentIndex()), newType);
        }
    }
}

void KPrEditAnimationsWidget::syncCurrentItem()
{
    QModelIndex index = m_timeLineView->currentIndex();
    if (index.isValid()) {
        updateIndex(index);
    }
}

void KPrEditAnimationsWidget::automaticPreviewRequested(const QModelIndex &index)
{
    if(!index.isValid()  || !showAutomaticPreview) {
        return;
    }
    KoXmlElement newAnimationContext = static_cast<KPrCollectionItemModel*>(m_collectionView->model())->animationContext(index);
    QModelIndex itemIndex = m_timeLineModel->mapToSource(m_timeLineView->currentIndex());
    if (!itemIndex.isValid()) {
        return;
    }
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, 0);
    KoShapeLoadingContext shapeContext(context, 0);

    KoShape *shape = m_docker->mainModel()->shapeByIndex(itemIndex);
    if (!shape) {
        return;
    }
    m_previewAnimation = loadOdfShapeAnimation(newAnimationContext, shapeContext, shape);
    if (m_previewAnimation) {
        m_previewAnimation->setKoTextBlockData(m_docker->mainModel()->animationByRow(itemIndex.row())->textBlockData());
        if(!m_docker->previewMode()) {
            m_docker->setPreviewMode(new KPrViewModePreviewShapeAnimations(m_view, m_view->kopaCanvas()));
        }
        m_docker->previewMode()->setShapeAnimation(m_previewAnimation);
        m_view->setViewMode(m_docker->previewMode()); // play the effect (it reverts to normal  when done)
    }
}

void KPrEditAnimationsWidget::setPreviewState(bool isEnable)
{
    showAutomaticPreview = isEnable;
}

void KPrEditAnimationsWidget::activateShapeCollection(QListWidgetItem *item)
{

    QString id = item->data(Qt::UserRole).toString();

    if(m_modelMap.contains(id)) {
        m_collectionView->setModel(m_modelMap[id]);
    }
    else {
        kWarning(31000) << "Didn't find a model with id ==" << id;
    }
}

void KPrEditAnimationsWidget::setAnimation(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }
    KoXmlElement newAnimationContext = static_cast<KPrCollectionItemModel*>(m_collectionView->model())->animationContext(index);
    QModelIndex itemIndex = m_timeLineModel->mapToSource(m_timeLineView->currentIndex());
    if (!itemIndex.isValid()) {
        return;
    }
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, 0);
    KoShapeLoadingContext shapeContext(context, 0);

    KoShape *shape = m_docker->mainModel()->shapeByIndex(itemIndex);
    KPrShapeAnimation *newAnimation = loadOdfShapeAnimation(newAnimationContext, shapeContext, shape);
    if (newAnimation) {
        m_docker->mainModel()->replaceAnimation(itemIndex, newAnimation);
    }
}

void KPrEditAnimationsWidget::loadDefaultAnimations()
{
    readDefaultAnimations();
    if (m_animations.isEmpty()) {
        return;
    }
    QList<KPrCollectionItem> entranceList;
    QList<KPrCollectionItem> emphasisList;
    QList<KPrCollectionItem> exitList;
    QList<KPrCollectionItem> customList;
    QList<KPrCollectionItem> motion_PathList;
    QList<KPrCollectionItem> ole_ActionList;
    QList<KPrCollectionItem> media_CallList;

    foreach(KPrShapeAnimation *animation, m_animations) {
        KPrCollectionItem temp;
        temp.id = animation->id();
        temp.name = animationName(animation->id());
        temp.toolTip = animationName(animation->id());
        temp.icon = loadAnimationIcon(animation->id());
        temp.animationContext = m_animationContext.value(m_animations.indexOf(animation));
        if (animation->presetClass() == KPrShapeAnimation::Entrance) {
            entranceList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Exit) {
            exitList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Emphasis) {
            emphasisList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Custom) {
            customList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Motion_Path) {
            motion_PathList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Ole_Action) {
            ole_ActionList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Media_Call) {
            media_CallList.append(temp);
        }
    }


    KPrCollectionItemModel* model = new KPrCollectionItemModel(this);
    model->setAnimationClassList(entranceList);
    addCollection("entrance", i18n("Entrance"), model);

    if (!exitList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(exitList);
        addCollection("exit", i18n("Exit"), model);
    }
    if (!emphasisList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(emphasisList);
        addCollection("emphasis", i18n("Emphasis"), model);
    }
    if (!customList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(customList);
        addCollection("custom", i18n("Custom"), model);
    }
    if (!motion_PathList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(motion_PathList);
        addCollection("motion_path", i18n("Motion path"), model);
    }
    if (!ole_ActionList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(ole_ActionList);
        addCollection("ole_action", i18n("Ole Action"), model);
    }
    if (!media_CallList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(media_CallList);
        addCollection("media_call", i18n("Media Call"), model);
    }

    m_collectionChooser->setCurrentRow(0);
    activateShapeCollection(m_collectionChooser->item(0));
}

void KPrEditAnimationsWidget::readDefaultAnimations()
{
    // use the same mechanism for loading the markers that are available
    // per default as when loading the normal markers.
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, 0);
    KoShapeLoadingContext shapeContext(context, 0);
    KoXmlDocument doc;

    const KStandardDirs* dirs = KGlobal::activeComponent().dirs();
    const QString filePath = dirs->findResource("data", "stage/animations/animations.xml");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        QString errorMessage;
        if (KoOdfReadStore::loadAndParse(&file, doc, errorMessage, filePath)) {
            const KoXmlElement docElement  = doc.documentElement();
            QString nodeType(docElement.attributeNS(KoXmlNS::presentation, "node-type"));
            KoXmlElement animationElement;
            forEachElement(animationElement, docElement) {
                KoXmlElement parAnimation;
                forEachElement(parAnimation, animationElement) {
                    KoXmlElement animation;
                    forEachElement(animation, parAnimation) {
                        KPrShapeAnimation *shapeAnimation = 0;
                        shapeAnimation = loadOdfShapeAnimation(animation, shapeContext);
                        if (shapeAnimation) {
                            m_animations.append(shapeAnimation);
                            m_animationContext.append(animation);
                        }
                    }
                }
            }
        }
        else {
            kWarning(30006) << "reading of" << filePath << "failed:" << errorMessage;
        }
    }
    else {
        kDebug(30006) << "animations.xml not found";
    }
}

QString KPrEditAnimationsWidget::animationName(const QString id) const
{
    //TODO: Identify animations supported by stage
    QStringList descriptionList = id.split("-");
    if (descriptionList.count() > 2) {
        descriptionList.removeFirst();
        descriptionList.removeFirst();
        return descriptionList.join(QString(" "));
    }
    return QString();
}

QIcon KPrEditAnimationsWidget::loadAnimationIcon(const QString id)
{
    QString name = animationName(id);
    if (!name.isEmpty()) {
        name = name.append("_animation");
        name.replace(" ", "_");
        QString path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
        if (!path.isNull()) {
            return KIcon(name);
        }
    }
    return KIcon("unrecognized_animation");
}

KPrShapeAnimation *KPrEditAnimationsWidget::loadOdfShapeAnimation(const KoXmlElement &element, KoShapeLoadingContext &context, KoShape *animShape)
{
    // load preset and id
    //TODO: motion-path, ole-action, media-call are not supported
    QString presetClass = element.attributeNS(KoXmlNS::presentation, "preset-class");
    QString animationId = element.attributeNS(KoXmlNS::presentation, "preset-id");

    KPrShapeAnimation::Node_Type l_nodeType = KPrShapeAnimation::On_Click;

    KPrShapeAnimation *shapeAnimation = 0;
    // The shape info and create a KPrShapeAnimation. If there is
    KoXmlElement e;
    forEachElement(e, element) {
        if (shapeAnimation == 0) {
            KoShape *shape = animShape;
            KoTextBlockData *textBlockData = 0;
            shapeAnimation = new KPrShapeAnimation(shape, textBlockData);
        }
        KPrAnimationBase *animation(KPrAnimationFactory::createAnimationFromOdf(e, context, shapeAnimation));
        if (shapeAnimation && animation) {
            shapeAnimation->addAnimation(animation);
        }
    }

    if (shapeAnimation) {
        shapeAnimation->setNodeType(l_nodeType);
        if (presetClass == "custom") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Custom);
        }
        if (presetClass == "entrance") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Entrance);
        }
        else if (presetClass == "exit") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Exit);
        }
        else if (presetClass == "emphasis") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Emphasis);
        }
        else if (presetClass == "motion-path") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Motion_Path);
        }
        else if (presetClass == "ole-action") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Ole_Action);
        }
        else if (presetClass == "media-call") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Media_Call);
        }
        else{
            shapeAnimation->setPresetClass(KPrShapeAnimation::None);
        }
        if (!animationId.isEmpty()) {
            shapeAnimation->setId(animationId);
        }
    }
    return shapeAnimation;
}

bool KPrEditAnimationsWidget::addCollection(const QString &id, const QString &title, KPrCollectionItemModel *model)
{
    if(m_modelMap.contains(id))
        return false;
    m_modelMap.insert(id, model);
    QString iconName = id;
    iconName.append("_animations");
    QIcon icon;
    QString path = KIconLoader::global()->iconPath(iconName, KIconLoader::Toolbar, true);
    if (!path.isNull()) {
        icon = KIcon(iconName);
    }
    else {
        icon = KIcon("unrecognized_animation");
    }

    QListWidgetItem *collectionChooserItem = new QListWidgetItem(icon, title);
    collectionChooserItem->setData(Qt::UserRole, id);
    m_collectionChooser->addItem(collectionChooserItem);
    return true;
}

bool KPrEditAnimationsWidget::loadPreviewConfig()
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    bool showPreview = true;

    if (config->hasGroup("Interface")) {
        const KConfigGroup interface = config->group("Interface");
        showPreview = interface.readEntry("ShowAutomaticPreviewAnimationEditDocker", showPreview);
    }
    return showPreview;
}

void KPrEditAnimationsWidget::savePreviewConfig()
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    KConfigGroup interface = config->group("Interface");
    interface.writeEntry("ShowAutomaticPreviewAnimationEditDocker", showAutomaticPreview);
}
